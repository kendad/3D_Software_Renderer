#include "clipping.h"
#include "texture.h"
#include "utilities.h"
#include "vector.h"
#include <stdbool.h>

typedef enum { TOP, BOTTOM, LEFT, RIGHT, NEAR, FAR } clipping_axis;

polygon_t create_polygon_from_triangle(triangle_t triangle) {
  polygon_t polygon = {.vertices = {triangle.vertices[0], triangle.vertices[1],
                                    triangle.vertices[2]},
                       .texcoords = {triangle.texcoords[0],
                                     triangle.texcoords[1],
                                     triangle.texcoords[2]},
                       .num_vertices = 3};
  return polygon;
}

bool is_vertex_inside(vec4_t *v, clipping_axis axis) {
  switch (axis) {
  case TOP:
    return v->y <= v->w;
  case BOTTOM:
    return v->y >= -v->w;
  case LEFT:
    return v->x >= -v->w;
  case RIGHT:
    return v->x <= v->w;
  case NEAR:
    return v->z >= -v->w;
  case FAR:
    return v->z <= v->w;
  }
}

float get_lerp_factor(vec4_t *v0, vec4_t *v1, clipping_axis axis) {
  // the interpolation factor
  float t = 0;

  switch (axis) {
  case TOP:
    t = (v0->w - v0->y) / ((v1->y - v0->y) - (v1->w - v0->w));
    break;

  case BOTTOM:
    t = (-v0->y - v0->w) / ((v1->y - v0->y) + (v1->w - v0->w));
    break;

  case LEFT:
    t = (-v0->x - v0->w) / ((v1->x - v0->x) + (v1->w - v0->w));
    break;

  case RIGHT:
    t = (v0->w - v0->x) / ((v1->x - v0->x) - (v1->w - v0->w));
    break;

  case NEAR:
    t = (-v0->z - v0->w) / ((v1->z - v0->z) + (v1->w - v0->w));
    break;

  case FAR:
    t = (v0->w - v0->z) / ((v1->z - v0->z) - (v1->w - v0->w));
    break;
  }

  return t;
}

void clip_polygon_against_axis(polygon_t *polygon, clipping_axis axis) {
  vec4_t inside_vertices[MAX_NUM_POLYGON_VERTICES];
  tex2_t inside_texcoords[MAX_NUM_POLYGON_VERTICES];
  int num_inside_vertices = 0;
  // Loop through all the vertices in the polygon vertices and check if they
  // are inside/outside the [-w to w] range
  vec4_t *current_vertex = &polygon->vertices[0];
  tex2_t *current_texcoord = &polygon->texcoords[0];
  vec4_t *previous_vertex = &polygon->vertices[polygon->num_vertices - 1];
  tex2_t *previous_texcoord = &polygon->texcoords[polygon->num_vertices - 1];

  while (current_vertex != &polygon->vertices[polygon->num_vertices]) {
    bool is_current_vertex_inside = is_vertex_inside(current_vertex, axis);
    bool is_previous_vertex_inside = is_vertex_inside(previous_vertex, axis);

    // we create a new vertex[intersection point on the frustum axis] only if
    // the vertices are moving from inside to outside of the view frustum or
    // vice-versa
    if ((is_current_vertex_inside && !is_previous_vertex_inside) ||
        (!is_current_vertex_inside && is_previous_vertex_inside)) {
      // the interpolation
      float t = get_lerp_factor(previous_vertex, current_vertex, axis);

      // Interpolated intersection point
      vec4_t intersection_point = {
          .x = lerp(previous_vertex->x, current_vertex->x, t),
          .y = lerp(previous_vertex->y, current_vertex->y, t),
          .z = lerp(previous_vertex->z, current_vertex->z, t),
          .w = lerp(previous_vertex->w, current_vertex->w, t),
      };

      // perspective corrected interpolated texture coord
      tex2_t interpolated_texture_coord = {
          .u = lerp(previous_texcoord->u, current_texcoord->u, t),
          .v = lerp(previous_texcoord->v, current_texcoord->v, t)};

      // put the new itersection point inside the list
      inside_vertices[num_inside_vertices] = intersection_point;
      inside_texcoords[num_inside_vertices] = interpolated_texture_coord;
      num_inside_vertices++;
    }

    // if the current vertex is inside then put it in the list
    if (is_current_vertex_inside) {
      inside_vertices[num_inside_vertices] = vec4_clone(current_vertex);
      inside_texcoords[num_inside_vertices] = tex2_clone(current_texcoord);
      num_inside_vertices++;
    }

    // Move to the next vertex
    previous_vertex = current_vertex;
    current_vertex++;
  }

  // at last update the polygon with the inside list of vertices
  for (int i = 0; i < num_inside_vertices; ++i) {
    polygon->vertices[i] = inside_vertices[i];
  }
  polygon->num_vertices = num_inside_vertices;
}

void clip_polygon(polygon_t *polygon) {
  clip_polygon_against_axis(polygon, TOP);
  clip_polygon_against_axis(polygon, BOTTOM);
  clip_polygon_against_axis(polygon, LEFT);
  clip_polygon_against_axis(polygon, RIGHT);
  clip_polygon_against_axis(polygon, NEAR);
  clip_polygon_against_axis(polygon, FAR);
}

void triangle_from_polygon(polygon_t *polygon, triangle_t *triangles,
                           int *number_of_triangles_from_polygon) {

  for (int i = 0; i < polygon->num_vertices - 2; ++i) {
    int index0 = 0;
    int index1 = i + 1;
    int index2 = i + 2;

    triangles[i].vertices[0] = polygon->vertices[index0];
    triangles[i].vertices[1] = polygon->vertices[index1];
    triangles[i].vertices[2] = polygon->vertices[index2];

    triangles[i].texcoords[0] = polygon->texcoords[index0];
    triangles[i].texcoords[1] = polygon->texcoords[index1];
    triangles[i].texcoords[2] = polygon->texcoords[index2];
  }
  *number_of_triangles_from_polygon = polygon->num_vertices - 2;
}
