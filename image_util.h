#ifndef IMAGE_UTIL_H
#define IMAGE_UTIL_H

#include "util.h"

typedef struct image {
  unsigned char *data;
  int width;
  int height;
  int size;
  int bit_depth;
} image_t;
enum TRANSFORMATION { SUBTRACT, NEGATE, TRANSLATE, INTENSITY_SLICE };
void save_image(image_t *img, char *filename);

void intensity_slice(image_t *img, int a, int b, int level);

void normalize(short *data, int size);

void subtraction(image_t *img, image_t *img2);

void translation_forward(image_t *img, int x_offset, int y_offset);

void negative(image_t *img);

unsigned char *read_image(int fd, int size);

void free_image(image_t *img);
#endif
