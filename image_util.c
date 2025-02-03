#include "image_util.h"
#include <stdlib.h>

void save_image(image_t *img, char *filename) {
  int fd = open(filename, O_CREAT | O_RDWR, 0644);
  if (errno != 0) {
    printf("failed to create fd. errno: %s\n", strerror(errno));
  }
  write(fd, img->data, img->size);
  if (errno != 0) {
    printf("failed to write image to fd. errno: %s\n", strerror(errno));
  }
  close(fd);
}

unsigned char *read_image(int fd, int size) {
  unsigned char *image = (unsigned char *)malloc(size);
  if (!image) {
    return NULL;
  }
  read(fd, image, size);
  if (errno != 0) {
    printf("failed to read fd to memory. errno: %s\n", strerror(errno));
    return NULL;
  }
  return image;
}

void intensity_slice(image_t *img, int a, int b, int level) {

  for (int i = 0; i < img->size; i++) {
    if (img->data[i] >= a && img->data[i] <= b) {
      img->data[i] = level;
    }
  }
}

void translation_forward(image_t *img, int x_offset, int y_offset) {
  // Forward Mapping:
  // x_1 = x_0 + dx
  // y_1 = y_0 + dy
  image_t out = {.bit_depth = img->bit_depth,
                 .size = img->size,
                 .data = (unsigned char *)malloc(img->size)};
  memset(out.data, 0, out.size);

  for (int i = 0; i < out.size; ++i) {
    int x0 = i % img->width;
    int y0 = i / img->width;

    int x1 = x0 + x_offset;
    if (x1 < 0 || x1 >= img->width)
      continue;

    int y1 = y0 + y_offset;

    if (y1 < 0 || y1 >= img->height)
      continue;

    int j = y1 * img->width + x1;

    out.data[j] = img->data[i];
  }
  free_image(img);
  img->data = out.data;
}

void normalize(short *data, int size) {
  short min = SHRT_MAX;
  for (int i = 0; i < size; i++) {
    min = data[i] < min ? data[i] : min;
  }
  printf("%d\n", min);
  for (int i = 0; i < size; i++) {
    data[i] -= min;
  }
  short max = SHRT_MIN;
  for (int i = 0; i < size; i++) {
    max = data[i] > max ? data[i] : max;
  }
  printf("max: %d\n", max);
  for (int i = 0; i < size; i++) {
    data[i] = (255 * data[i]) / max;
    printf("%d\n", data[i]);
  }
}

void subtraction(image_t *img, image_t *img2) {
  if (!img || !img2) {
    fprintf(stderr, "img1 or img2 is nullptr\n");
  }
  if (!img->data || !img2->data) {
    fprintf(stderr, "Error: One or both image data pointers are NULL.\n");
    return;
  }
  if (img->size != img2->size) {
    fprintf(stderr, "img1 doesn't match img2 size.\n");
    return;
  } // short *temp = (short *)malloc(img->size * sizeof(short));
  for (int i = 0; i < img->size; i++) {
    //   if (img->data[i] < img2->data[i]) {
    //    temp[i] = 0;
    //   continue;
    //}
    // temp[i] = (short)img->data[i] - (short)img2->data[i];
    img->data[i] = img->data[i] > img2->data[i] ? img->data[i] - img2->data[i]
                                                : img2->data[i] - img->data[i];

    // printf("%d - %d = %d\n", img->data[i], img2.data[i], temp[i]);
  }
  // normalize(temp, out.size);
  //  for (int i = 0; i < img->size; i++) {
  //    img->data[i] = temp[i] & 0xFF;
  // }
  // free(temp);
}

void negative(image_t *img) {
  for (int i = 0; i < img->size; i++) {
    img->data[i] = ((1 << img->bit_depth) - 1) - img->data[i];
  }
}

void free_image(image_t *img) {
  if (img != NULL && img->data != NULL) {
    free(img->data);
    img->data = NULL;
  }
}
