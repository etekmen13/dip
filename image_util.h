#ifndef IMAGE_UTIL_H
#define IMAGE_UTIL_H

#include "util.h"
#define HISTOGRAM_COLUMN_SIZE 100
typedef struct image {
  unsigned char *data;
  int width;
  int height;
  int size;
  int bit_depth;
} image_t;

enum FILTER_TYPE { BOX, GAUSSIAN, MEDIAN, LAPLACE4, LAPLACE8 };
enum TRANSFORMATION { SUBTRACT, NEGATE, TRANSLATE, INTENSITY_SLICE };
int save_image(image_t *img, char *filename);

void save_histogram(int *h, int n);

void intensity_slice(image_t *img, int a, int b, int level);

void normalize_short(short *data, int size, int depth);

void normalize_double(double *data, int size, int depth);

void subtraction(image_t *img, image_t *img2);

void translation_forward(image_t *img, int x_offset, int y_offset);

void negative(image_t *img);

void histogram_spec(image_t *img, double *h_target);

int *get_histogram(image_t *img);

unsigned char *read_image(int fd, int size);

void free_image(image_t *img);
int filter(image_t *img, enum FILTER_TYPE type, unsigned int kernel_width);
double *get_normalized_histogram(const image_t *img);
#endif
