#include "texture.h"
#include <stdint.h>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

texture_t load_texture_data(char *filename) {
  texture_t texture_data;
  unsigned char *data =
      stbi_load(filename, &texture_data.width, &texture_data.height,
                &texture_data.no_of_channels, 4);
  if (!data) {
    fprintf(stderr, "Error loading texture data\n");
    exit(1);
  }

  texture_data.data = (uint32_t *)data;
  return texture_data;
}

tex2_t tex2_clone(tex2_t *t) {
  tex2_t new_tex_coord = {.u = t->u, .v = t->v};
  return new_tex_coord;
}
