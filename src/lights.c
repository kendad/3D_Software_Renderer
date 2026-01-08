#include "lights.h"
#include "utilities.h"
#include "vector.h"
#include <math.h>
#include <stdint.h>

#define AMBIENT_STRENGTH 0.2
#define SPECULAR_STRENGTH 1.5
#define GAMMA_INVERSE (1.0 / 2.2)

void init_lights_in_scene(light_t *lights, int *number_of_lights) {
  if (*number_of_lights > MAX_NUMBER_OF_LIGHTS)
    return;
  vec3_t light1_pos = {.x = 0, .y = 5.0, .z = 0};
  color_t yellow_light = {.r = 0xFF, .g = 0xFF, .b = 0xFF, .a = 0xFF};
  light_t light1 = {.position = light1_pos,
                    .color = create_color_uint32(yellow_light)};
  lights[*number_of_lights] = light1;
  (*number_of_lights)++;
}

vec3_t light_reflect(vec3_t light_direction, vec3_t normal) {
  // [2(l.n)n - l]-->reflection vector formula

  // 2(l.n)
  float l_dot_n = vec3_dot(light_direction, normal);
  l_dot_n *= 2.0;

  // 2(l.n)n
  vec3_mul(&normal, l_dot_n);

  vec3_t reflection_direction = vec3_sub(normal, light_direction);
  vec3_normalize(&reflection_direction);
  return reflection_direction;
}

uint32_t light_mix(uint32_t color1, uint32_t color2,
                   float interpolation_factor) {
  return lerp_uint32(color1, color2, interpolation_factor);
}

uint32_t light_phong(light_t lights[], int total_lights_in_scene,
                     vec3_t vertex_position, vec3_t camera_position,
                     vec3_t normal, uint32_t vertex_color) {
  float light_total_r = AMBIENT_STRENGTH;
  float light_total_g = AMBIENT_STRENGTH;
  float light_total_b = AMBIENT_STRENGTH;

  // loop through all the lights in the scene and accumulate them in the
  // variables light_total_r/g/b
  for (int l = 0; l < total_lights_in_scene; ++l) {
    light_t light = lights[l];

    // a light_direction vector pointing from the surface to the light
    vec3_t light_direction = vec3_sub(light.position, vertex_position);
    vec3_normalize(&light_direction);

    // a view direction vector that points from the surface to the camera
    vec3_t view_direction = vec3_sub(camera_position, vertex_position);
    vec3_normalize(&view_direction);

    // a reflection direction vector for my light direction about the normal
    vec3_t reflection_direction = light_reflect(light_direction, normal);

    // the diffuse component of the light
    float diffuse = vec3_dot(normal, light_direction);
    if (diffuse < 0.0)
      diffuse = 0.0;

    // the specular component of the light
    float spec = vec3_dot(view_direction, reflection_direction);
    if (spec < 0.0)
      spec = 0.0;
    spec = powf(spec, 32.0);
    float specular = spec * SPECULAR_STRENGTH;

    // extract light color's R G B vallues as float in the range [0,1]
    float light_color_r = ((light.color >> 16) & 0xFF) / 255.0;
    float light_color_g = ((light.color >> 8) & 0xFF) / 255.0;
    float light_color_b = (light.color & 0xFF) / 255.0;

    // accumulate the light
    light_total_r += (diffuse + specular) * light_color_r;
    light_total_g += (diffuse + specular) * light_color_g;
    light_total_b += (diffuse + specular) * light_color_b;
  }

  // extract the R G B A from the vertex color
  // this vertex colors are in the gamma corrected space which is non linear
  uint32_t tex_color_a = (vertex_color >> 24) & 0xFF;
  uint32_t tex_color_r = (vertex_color >> 16) & 0xFF;
  uint32_t tex_color_g = (vertex_color >> 8) & 0xFF;
  uint32_t tex_color_b = (vertex_color >> 0) & 0xFF;

  // bring the vertex color to the linear space
  float tex_color_linear_r = powf((tex_color_r), 2.2);
  float tex_color_linear_g = powf((tex_color_g), 2.2);
  float tex_color_linear_b = powf((tex_color_b), 2.2);

  // combine light with the vertex colors
  float linear_r = (tex_color_linear_r * light_total_r);
  float linear_g = (tex_color_linear_g * light_total_g);
  float linear_b = (tex_color_linear_b * light_total_b);

  // apply gamma correction to the linear light
  float corrected_r = powf(linear_r, GAMMA_INVERSE);
  float corrected_g = powf(linear_g, GAMMA_INVERSE);
  float corrected_b = powf(linear_b, GAMMA_INVERSE);

  // combine the light with the vertex colors for phong lighting effect
  uint32_t final_r = (uint32_t)(corrected_r);
  uint32_t final_g = (uint32_t)(corrected_g);
  uint32_t final_b = (uint32_t)(corrected_b);
  // clamp the final colors to 255.0
  if (final_r > 255)
    final_r = 255;
  if (final_g > 255)
    final_g = 255;
  if (final_b > 255)
    final_b = 255;

  // combine the modified r g b values into the final color for the vertex
  uint32_t final_color =
      (tex_color_a << 24) | (final_r << 16) | (final_g << 8) | final_b;

  return final_color;
}
