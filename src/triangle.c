#include "triangle.h"
#include "appstate.h"
#include "config.h"
#include "display.h"
#include "lights.h"
#include "texture.h"
#include "utilities.h"
#include "vector.h"
#include <math.h>
#include <stdint.h>

// Digital Differential Analyzer(DDA) line drawing algorithm
void draw_line(float x0, float y0, float x1, float y1, app_state_t *app_state) {
  float delta_x = x1 - x0;
  float delta_y = y1 - y0;

  float longest_side =
      (fabsf(delta_x) > fabsf(delta_y)) ? fabsf(delta_x) : fabsf(delta_y);

  if (longest_side == 0)
    return;

  float x_increment = delta_x / longest_side;
  float y_increment = delta_y / longest_side;

  float current_x = x0;
  float current_y = y0;
  for (int i = 0; i < longest_side; ++i) {
    display_draw_pixel(current_x, current_y, 0xFFFFFFFF, app_state);
    current_x += x_increment;
    current_y += y_increment;
  }
}

void draw_triangle_wireframe(triangle_t triangle, app_state_t *app_state) {
  float x0 = triangle.vertices[0].x;
  float y0 = triangle.vertices[0].y;

  float x1 = triangle.vertices[1].x;
  float y1 = triangle.vertices[1].y;

  float x2 = triangle.vertices[2].x;
  float y2 = triangle.vertices[2].y;

  draw_line(x0, y0, x1, y1, app_state);
  draw_line(x1, y1, x2, y2, app_state);
  draw_line(x2, y2, x0, y0, app_state);
}

bool is_top_flat_or_left(vec2_t edge) {
  bool is_top_flat = edge.y == 0 && edge.x > 0;
  bool is_left = edge.y < 0;

  return is_top_flat || is_left;
}

void draw_triangle_fill(triangle_t triangle, texture_t *texture_data,
                        light_t lights[], int total_lights_in_scene,
                        app_state_t *app_state) {
  // the three vertices of the triangle in vec2
  vec2_t v0 = vec2_from_vec4(triangle.vertices[0]);
  vec2_t v1 = vec2_from_vec4(triangle.vertices[1]);
  vec2_t v2 = vec2_from_vec4(triangle.vertices[2]);

  // the three vertices of the triangle in vec4
  vec4_t v0_pos = triangle.view_space_vertices[0];
  vec4_t v1_pos = triangle.view_space_vertices[1];
  vec4_t v2_pos = triangle.view_space_vertices[2];

  // the three normals of the triangle
  vec3_t v0_normal = triangle.normals[0];
  vec3_t v1_normal = triangle.normals[1];
  vec3_t v2_normal = triangle.normals[2];

  // Renormalize the normals as rotations/scale might have change their values
  vec3_normalize(&v0_normal);
  vec3_normalize(&v1_normal);
  vec3_normalize(&v2_normal);

  // the three texture coordinate for the three vertices
  tex2_t v0_tex_coord = triangle.texcoords[0];
  tex2_t v1_tex_coord = triangle.texcoords[1];
  tex2_t v2_tex_coord = triangle.texcoords[2];

  // the depth value of the three vertex points[needed for perspective correct
  // interpolation]
  float z0 = triangle.vertices[0].w;
  float z1 = triangle.vertices[1].w;
  float z2 = triangle.vertices[2].w;

  // Bounding box inside containing the three vertices of the triangle
  int x_min = min(v0.x, min(v1.x, v2.x));
  int y_min = min(v0.y, min(v1.y, v2.y));
  int x_max = max(v0.x, max(v1.x, v2.x));
  int y_max = max(v0.y, max(v1.y, v2.y));

  // constant Edge Function Deltas used for the horizontal and vertical steps
  float delta_w0_col = (v0.y - v1.y);
  float delta_w1_col = (v1.y - v2.y);
  float delta_w2_col = (v2.y - v0.y);

  float delta_w0_row = (v1.x - v0.x);
  float delta_w1_row = (v2.x - v1.x);
  float delta_w2_row = (v0.x - v2.x);

  // the three triangle edges
  vec2_t v0v1 = vec2_sub(v1, v0);
  vec2_t v1v2 = vec2_sub(v2, v1);
  vec2_t v2v0 = vec2_sub(v0, v2);

  // area of the triangle
  float area = fabsf(vec2_cross(v0v1, v2v0));

  // For all the edges of the triangle
  // find if they are flat_top or left
  float bias0 = is_top_flat_or_left(v0v1) ? 0 : -0.0001;
  float bias1 = is_top_flat_or_left(v1v2) ? 0 : -0.0001;
  float bias2 = is_top_flat_or_left(v2v0) ? 0 : -0.0001;

  // take a starting point at the top left of the bounding box
  vec2_t p0 = {x_min + 0.5, y_min + 0.5};
  // the three vectors pointing from the three vertices to the point
  vec2_t v0p = vec2_sub(p0, v0);
  vec2_t v1p = vec2_sub(p0, v1);
  vec2_t v2p = vec2_sub(p0, v2);

  // Check if the point is left or right of the triangle edges(using Edge
  // Function)
  float w0_row = vec2_cross(v0v1, v0p) + bias0;
  float w1_row = vec2_cross(v1v2, v1p) + bias1;
  float w2_row = vec2_cross(v2v0, v2p) + bias2;

  // Loop through all the pixels contained within this bounding box around the
  // triangle
  for (int y = y_min; y <= y_max; ++y) {
    float w0 = w0_row;
    float w1 = w1_row;
    float w2 = w2_row;
    for (int x = x_min; x <= x_max; ++x) {

      // check if the point is inside the triangle
      bool is_inside_triangle = w0 >= 0 && w1 >= 0 && w2 >= 0;

      // Draw on the pixel if it is inside the triangle
      if (is_inside_triangle) {
        float alpha = w1 / area; // Edge v1->v2
        float beta = w2 / area;  // Edge v2->v0
        float gamma = w0 / area; // Edge v0->v1

        // Interpolate on the UV coordinates to get the texture
        float u = alpha * (v0_tex_coord.u / z0) + beta * (v1_tex_coord.u / z1) +
                  gamma * (v2_tex_coord.u / z2);
        float v = alpha * (v0_tex_coord.v / z0) + beta * (v1_tex_coord.v / z1) +
                  gamma * (v2_tex_coord.v / z2);

        float interpolated_z =
            alpha * (1 / z0) + beta * (1 / z1) + gamma * (1 / z2);

        u /= interpolated_z;
        v /= interpolated_z;

        int tex_x = abs((int)(u * texture_data->width) % texture_data->width);
        int tex_y = abs((int)(v * texture_data->height) % texture_data->height);

        uint32_t interpolated_color =
            texture_data->data[tex_x + (texture_data->width * tex_y)];

        // // interpolate on the positions
        float pos_x = alpha * (v0_pos.x / z0) + beta * (v1_pos.x / z1) +
                      gamma * (v2_pos.x / z2);
        pos_x /= interpolated_z;

        float pos_y = alpha * (v0_pos.y / z0) + beta * (v1_pos.y / z1) +
                      gamma * (v2_pos.y / z2);
        pos_y /= interpolated_z;

        float pos_z = alpha * (v0_pos.z / z0) + beta * (v1_pos.z / z1) +
                      gamma * (v2_pos.z / z2);
        pos_z /= interpolated_z;

        vec3_t interpolated_position = {.x = pos_x, .y = pos_y, .z = pos_z};

        // // Interpolate on the normals
        float normal_x = alpha * (v0_normal.x / z0) +
                         beta * (v1_normal.x / z1) + gamma * (v2_normal.x / z2);
        normal_x /= interpolated_z;

        float normal_y = alpha * (v0_normal.y / z0) +
                         beta * (v1_normal.y / z1) + gamma * (v2_normal.y / z2);
        normal_y /= interpolated_z;

        float normal_z = alpha * (v0_normal.z / z0) +
                         beta * (v1_normal.z / z1) + gamma * (v2_normal.z / z2);
        normal_z /= interpolated_z;

        vec3_t interpolated_normal = {
            .x = normal_x, .y = normal_y, .z = normal_z};
        vec3_normalize(&interpolated_normal);

        // lighting calculatons
        float light_total_r = 0.0, light_total_g = 0.0, light_total_b = 0.0;
        float ambient_strength = 0.2;

        for (int l = 0; l < total_lights_in_scene; ++l) {
          light_t light = lights[l];

          vec3_t light_direction =
              vec3_sub(light.position, interpolated_position);
          vec3_normalize(&light_direction);
          float diffuse = vec3_dot(interpolated_normal, light_direction);
          if (diffuse < 0.0)
            diffuse = 0;

          // extract light color as float in the range [0,1]
          float light_color_r = ((light.color >> 16) & 0xFF) / 255.0;
          float light_color_g = ((light.color >> 8) & 0xFF) / 255.0;
          float light_color_b = (light.color & 0xFF) / 255.0;

          // Accummulate the light
          light_total_r += (ambient_strength + diffuse) * light_color_r;
          light_total_g += (ambient_strength + diffuse) * light_color_g;
          light_total_b += (ambient_strength + diffuse) * light_color_b;
        }

        // extract the r g b a from the interpolated texture color
        uint32_t tex_color_a = (interpolated_color >> 24) & 0xFF;
        uint32_t tex_color_r = (interpolated_color >> 16) & 0xFF;
        uint32_t tex_color_g = (interpolated_color >> 8) & 0xFF;
        uint32_t tex_color_b = (interpolated_color >> 0) & 0xFF;

        // combine the light with the texture colors for lighting effect
        uint32_t final_r = (uint32_t)(tex_color_r * light_total_r);
        uint32_t final_g = (uint32_t)(tex_color_g * light_total_g);
        uint32_t final_b = (uint32_t)(tex_color_b * light_total_b);
        // clamp the final colors to 255.0
        if (final_r > 255)
          final_r = 255;
        if (final_g > 255)
          final_g = 255;
        if (final_b > 255)
          final_b = 255;

        // combine the modified r g b values into the final color for the vertex
        interpolated_color =
            (tex_color_a << 24) | (final_r << 16) | (final_g << 8) | final_b;

        if (interpolated_z > app_state->z_buffer[x + (WINDOW_WIDTH * y)]) {
          display_draw_pixel(x, y, interpolated_color, app_state);
          app_state->z_buffer[x + (WINDOW_WIDTH * y)] = interpolated_z;
        }
      }
      w0 += delta_w0_col;
      w1 += delta_w1_col;
      w2 += delta_w2_col;
    }
    w0_row += delta_w0_row;
    w1_row += delta_w1_row;
    w2_row += delta_w2_row;
  }
}
