#include "lights.h"
#include "texture.h"
#include "utilities.h"
#include "vector.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define AMBIENT_STRENGTH 0.5
#define PBR_AMBIENT_STRENGTH 0.02
#define SPECULAR_STRENGTH 1.5
#define GAMMA_INVERSE (1.0 / 2.2)
#define ALPHA 0.1 // Surface Roughness Parameter value for shiny metal objects
#define Li 1.0    // TODO: Moodify such that Li comes from Image based lighting

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

  // extract the R G B A from the vertex color
  // this vertex colors are in the gamma corrected space which is non linear
  uint32_t tex_color_a = (vertex_color >> 24) & 0xFF;
  uint32_t tex_color_r = (vertex_color >> 16) & 0xFF;
  uint32_t tex_color_g = (vertex_color >> 8) & 0xFF;
  uint32_t tex_color_b = (vertex_color >> 0) & 0xFF;

  // bring the vertex color to the linear space for any further calculations
  float tex_color_linear_r = powf((tex_color_r / 255.0), 2.2);
  float tex_color_linear_g = powf((tex_color_g / 255.0), 2.2);
  float tex_color_linear_b = powf((tex_color_b / 255.0), 2.2);

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

  // combine light with the vertex colors
  float linear_r = (tex_color_linear_r * light_total_r);
  float linear_g = (tex_color_linear_g * light_total_g);
  float linear_b = (tex_color_linear_b * light_total_b);

  // apply gamma correction back again to the linear light
  float corrected_r = powf(linear_r, GAMMA_INVERSE);
  float corrected_g = powf(linear_g, GAMMA_INVERSE);
  float corrected_b = powf(linear_b, GAMMA_INVERSE);

  // combine the light with the vertex colors for phong lighting effect
  uint32_t final_r = (uint32_t)(corrected_r * 255.0);
  uint32_t final_g = (uint32_t)(corrected_g * 255.0);
  uint32_t final_b = (uint32_t)(corrected_b * 255.0);
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

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// PBR /////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

float fresnel_reflectance(vec3_t halfway_vector, vec3_t light_direction) {
  // We use Fresnel Reflectance using Schlicks Approximation
  // F(n,l)=F0 + (1-F0)(1-(n.l))^(1/p)
  // For most everyday materials we will consider F0=0.04
  // (h.l) is clamped in the range [0,1]
  // (1/p) = 5.0

  float one_over_p = 5.0;

  float F0 = 0.04;
  float one_minus_F0 = 1.0 - F0;

  float h_dot_l = vec3_dot(halfway_vector, light_direction);
  if (h_dot_l < 0)
    h_dot_l = 0.0;
  if (h_dot_l > 1)
    h_dot_l = 1.0;

  float one_minus_h_dot_l = 1.0 - h_dot_l;

  return F0 + (one_minus_F0 * powf(one_minus_h_dot_l, one_over_p));
}

float visibility(vec3_t surface_normal, vec3_t view_direction,
                 vec3_t light_direction) {
  // This is the visibity term and describes how much of the surface will
  // interfere with light intensity[microfacet theory] We use Hammon's
  // Approximation
  //  G2(l,v) /(4*|n.l||n.v|) Approximation is 0.5 / lerp(2|n.l||n.v| , |n.l| +
  //  |n.v| , ALPHA)
  //

  float n_dot_l = vec3_dot(surface_normal, light_direction);
  float n_dot_v = vec3_dot(surface_normal, view_direction);

  if (n_dot_l < 0)
    n_dot_l = 0.0;
  if (n_dot_l > 1)
    n_dot_l = 1.0;
  if (n_dot_v < 0)
    n_dot_v = 0.0;
  if (n_dot_v > 1)
    n_dot_v = 1.0;

  float two_n_dot_l_times_n_dot_v = 2.0 * n_dot_l * n_dot_v;
  float n_dot_l_sum_n_dot_v = n_dot_l + n_dot_v;
  float lerped = lerp(two_n_dot_l_times_n_dot_v, n_dot_l_sum_n_dot_v, ALPHA);
  return 0.5 / (lerped + 1e-7f);
}

float normal_distribution(vec3_t surface_normal, vec3_t halfway_vector) {
  // We use the GGX normal distribution
  //  D(h) = (APLHA^2) / PI* (1 + (n.h)^2 * ((APLHA^2)-1)) ^ 2

  float n_dot_h = vec3_dot(surface_normal, halfway_vector);
  if (n_dot_h < 0)
    return 0.0;
  if (n_dot_h > 1)
    n_dot_h = 1.0;

  float n_dot_h_squared = n_dot_h * n_dot_h;

  float alpha_squared = ALPHA * ALPHA;

  float numerator = alpha_squared;

  float denominator =
      M_PI * powf(1.0 + (n_dot_h_squared * (alpha_squared - 1.0)), 2.0);

  return numerator / (denominator + 1e-7f);
}

float fresnel_specular_component(float fresnel_term, vec3_t surface_normal,
                                 vec3_t light_direction, vec3_t view_direction,
                                 vec3_t halfway_vector) {

  float visibility_term =
      visibility(surface_normal, view_direction, light_direction);
  float normal_distribution_term =
      normal_distribution(surface_normal, halfway_vector);

  return fresnel_term * visibility_term * normal_distribution_term;
}

float fresnel_diffuse_component(float fresnel_term) { return 1 - fresnel_term; }

// Find the UV coordinates from a skybox/cubemap based on the surface normal
// direction
vec2_t uv_from_surface_normal(vec3_t surface_normal, texture_t *texture_data) {
  vec2_t current_face = {0, 0};
  vec2_t final_uv = {0, 0};

  // Get the absoulte values of the surface normal
  float absX = fabsf(surface_normal.x);
  float absY = fabsf(surface_normal.y);
  float absZ = fabsf(surface_normal.z);

  // Determine which absoulte value from X Y Z is largest
  // the largest value will point towards the skybox face that the
  // surface_normal is pointing towards
  // For ex: if absZ is largest and surface_normal.z is +ve then surface_normal
  // is facing the front
  if (absX >= absY && absX >= absZ) {
    // then surface normal is pointing in either LEFT/RIGHT

    // project into the current face which will give us a value between [-1,1]
    final_uv.x = surface_normal.y / absX;
    final_uv.y = surface_normal.z / absX;

    // also determine the current face
    if (surface_normal.x < 0) {
      // then we are facing the LEFT side
      current_face.x = 0;
      current_face.y = 1;
      // change the signs to match the current face coordinate space
      final_uv.x *= 1.0;
      final_uv.y *= -1.0;
    } else if (surface_normal.x >= 0) {
      // then we are facing the RIGHT side
      current_face.x = 2;
      current_face.y = 1;
      // change the signs to match the current face coordinate space
      final_uv.x *= -1.0;
      final_uv.y *= -1.0;
    }
  } else if (absY >= absX && absY >= absZ) {
    // then surface normal is pointing in either TOP/BOTTOM

    // project into the current face which will give us a value between [-1,1]
    final_uv.x = surface_normal.x / absY;
    final_uv.y = surface_normal.z / absY;

    // also determine the current face
    if (surface_normal.y < 0) {
      // then we are facing the BOTTOM side
      current_face.x = 1;
      current_face.y = 0;
      // change the signs to match the current face coordinate space
      final_uv.x *= 1.0;
      final_uv.y *= -1.0;
    } else if (surface_normal.y >= 0) {
      // then we are facing the TOP side
      current_face.x = 1;
      current_face.y = 2;
      // change the signs to match the current face coordinate space
      final_uv.x *= 1.0;
      final_uv.y *= 1.0;
    }
  } else {
    // then surface normal is pointing in either FRONT/BACK

    // project into the current face which will give us a value between [-1,1]
    final_uv.x = surface_normal.x / absZ;
    final_uv.y = surface_normal.y / absZ;

    // also determine the current face
    if (surface_normal.z < 0) {
      // then we are facing the BACK side
      current_face.x = 3;
      current_face.y = 1;
      // change the signs to match the current face coordinate space
      final_uv.x *= -1.0;
      final_uv.y *= -1.0;
    } else if (surface_normal.z >= 0) {
      // then we are facing the FRONT side
      current_face.x = 1;
      current_face.y = 1;
      // change the signs to match the current face coordinate space
      final_uv.x *= 1.0;
      final_uv.y *= -1.0;
    }
  }

  // convert the UV from [-1,1] to [0,1] range
  final_uv.x = (final_uv.x + 1.0) / 2.0;
  final_uv.y = (final_uv.y + 1.0) / 2.0;

  // shift UV to the correct face
  // add to the UV's X&Y the currect face column and row values and divide by
  // 4 and 3 respectively as the cubemap is divided into 4Columns and 3Rows
  final_uv.x = (final_uv.x + current_face.x) / 4.0;
  final_uv.y = (final_uv.y + current_face.y) / 3.0;

  // finally bring this texture into the texture width and height range
  final_uv.x = (int)(final_uv.x * texture_data->width) % texture_data->width;
  final_uv.y = (int)(final_uv.y * texture_data->height) % texture_data->height;

  return final_uv;
}

uint32_t light_pbr(light_t lights[], int total_lights_in_scene,
                   vec3_t vertex_position, vec3_t camera_position,
                   vec3_t surface_normal, uint32_t vertex_color,
                   texture_t *irradiance_texture_data) {
  float final_r = 0.0;
  float final_g = 0.0;
  float final_b = 0.0;

  vec2_t uv = uv_from_surface_normal(surface_normal, irradiance_texture_data);
  vertex_color =
      irradiance_texture_data
          ->data[abs((int)(uv.x + (uv.y * irradiance_texture_data->width)))];

  // extract the R G B A from the vertex color
  // this vertex colors are in the gamma corrected space which is non linear
  uint32_t tex_color_a = (vertex_color >> 24) & 0xFF;
  uint32_t tex_color_r = (vertex_color >> 16) & 0xFF;
  uint32_t tex_color_g = (vertex_color >> 8) & 0xFF;
  uint32_t tex_color_b = (vertex_color >> 0) & 0xFF;

  // bring the vertex color to the linear space for any further calculations
  float tex_color_linear_r = powf((tex_color_r / 255.0), 2.2);
  float tex_color_linear_g = powf((tex_color_g / 255.0), 2.2);
  float tex_color_linear_b = powf((tex_color_b / 255.0), 2.2);

  // loop through all the lights in the scene and accumulate them in the
  // variables light_total_r/g/b
  for (int l = 0; l < total_lights_in_scene; ++l) {
    // current light
    light_t light = lights[l];

    // extract light color's R G B vallues as float in the range [0,1]
    float light_color_r = ((light.color >> 16) & 0xFF) / 255.0;
    float light_color_g = ((light.color >> 8) & 0xFF) / 255.0;
    float light_color_b = (light.color & 0xFF) / 255.0;

    // a light_direction vector pointing from the surface to the light
    vec3_t light_direction = vec3_sub(light.position, vertex_position);
    vec3_normalize(&light_direction);

    // a view direction vector that points from the surface to the camera
    vec3_t view_direction = vec3_sub(camera_position, vertex_position);
    vec3_normalize(&view_direction);

    // a halfway_vector that lies smack in between the view_direction and
    // light_direction
    vec3_t halfway_vector = vec3_add(light_direction, view_direction);
    vec3_normalize(&halfway_vector);

    // we will use the lighting equation
    // Lo(v)=sum_across_all_lights_in_scene(f(l,v)*Li*(n.l))
    // where
    // The BRDF term is
    // f(l,v) = (f_spec * light_color) + (f_diffuse * (albedo/PI))
    // The Incoming Light
    // Li = FOR NOW its a constant value
    // The cosine term
    // (n.l)

    //////////////////// BRDF Term ///////////////////////////////
    // The Fresnel Term
    float fresnel_term = fresnel_reflectance(halfway_vector, light_direction);
    // Specular Component of the fresnel equation
    float f_specular = fresnel_specular_component(
        fresnel_term, surface_normal, light_direction, view_direction,
        halfway_vector);
    // apply the specular term
    // f_spec = specular_term * light_color
    float specular_r = f_specular * light_color_r;
    float specular_g = f_specular * light_color_g;
    float specular_b = f_specular * light_color_b;

    // diffuse term of the fresnel equation
    // f_diffuse = diffuse_term * (albedo/PI) * light_color
    float f_diffuse = fresnel_diffuse_component(fresnel_term);
    float diffuse_r = f_diffuse * (tex_color_linear_r / M_PI) * light_color_r;
    float diffuse_g = f_diffuse * (tex_color_linear_g / M_PI) * light_color_g;
    float diffuse_b = f_diffuse * (tex_color_linear_b / M_PI) * light_color_b;

    // final_output(Lo)=(specular + diffuse) * Li * (n.l)
    float n_dot_l = vec3_dot(surface_normal, light_direction);
    if (n_dot_l <= 0)
      continue;
    final_r += (specular_r + diffuse_r) * Li * n_dot_l;
    final_g += (specular_g + diffuse_g) * Li * n_dot_l;
    final_b += (specular_b + diffuse_b) * Li * n_dot_l;
  }

  // Add ambient strength
  final_r += (tex_color_linear_r * PBR_AMBIENT_STRENGTH);
  final_g += (tex_color_linear_g * PBR_AMBIENT_STRENGTH);
  final_b += (tex_color_linear_b * PBR_AMBIENT_STRENGTH);

  // apply gamma correction back again to the linear light
  final_r = powf(final_r, GAMMA_INVERSE);
  final_g = powf(final_g, GAMMA_INVERSE);
  final_b = powf(final_b, GAMMA_INVERSE);

  // convert to uint32_t and bring it from range [0,1] to [0,255]
  uint32_t r = (uint32_t)(final_r * 255.0);
  uint32_t g = (uint32_t)(final_g * 255.0);
  uint32_t b = (uint32_t)(final_b * 255.0);
  // clamp the final color to range 255.0
  if (r > 255)
    r = 255;
  if (g > 255)
    g = 255;
  if (b > 255)
    b = 255;

  // combine the modified r g b values into the final color for the vertex
  uint32_t final_color = (tex_color_a << 24) | (r << 16) | (g << 8) | b;

  return final_color;
}
