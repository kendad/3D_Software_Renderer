#pragma once

#include "texture.h"
#include "triangle.h"
#include "vector.h"
#define MAX_NUM_POLYGON_VERTICES 10

typedef struct {
  vec4_t vertices[MAX_NUM_POLYGON_VERTICES];
  vec4_t view_space_vertices[MAX_NUM_POLYGON_VERTICES];
  vec3_t normals[MAX_NUM_POLYGON_VERTICES];
  tex2_t texcoords[MAX_NUM_POLYGON_VERTICES];
  int num_vertices;
} polygon_t;

polygon_t create_polygon_from_triangle(triangle_t triangle);
void clip_polygon(polygon_t *polygon);
void triangle_from_polygon(polygon_t *polygon, triangle_t triangles[],
                           int *number_of_triangles_from_polygon);
