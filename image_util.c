#include "image_util.h"
#include <stdio.h>
#include <stdlib.h>

int save_image(image_t *img, char *filename) {
  int fd = open(filename, O_CREAT | O_RDWR, 0644);
  if (fd == -1 && errno != 0) {
    printf("failed to create fd. errno: %s\n", strerror(errno));
    return -1;
  }
  int bytes_written = write(fd, img->data, img->size);
  if (bytes_written == -1 && errno != 0) {
    printf("failed to write image to fd. errno: %s\n", strerror(errno));
    close(fd);
    return -1;
  }
  close(fd);
  return 0;
}

unsigned char *read_image(int fd, int size) {
  unsigned char *image = (unsigned char *)malloc(size);
  if (!image) {
    return NULL;
  }
  int bytes_read = read(fd, image, size);
  if (bytes_read == -1 && errno != 0) {
    printf("failed to read fd to memory. errno: %s\n", strerror(errno));
    return NULL;
  }
  return image;
}

void intensity_slice(image_t *img, int a, int b, int level) {
  int thresh =
      level >= (1 << img->bit_depth) ? (1 << img->bit_depth) - 1 : level;
  for (int i = 0; i < img->size; i++) {
    if (img->data[i] >= a && img->data[i] <= b) {
      img->data[i] = thresh;
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

void normalize_short(short *data, int size, int depth) {
  short min = SHRT_MAX;
  for (int i = 0; i < size; i++) {
    min = data[i] < min ? data[i] : min;
  }
  printf("min: %d\n", min);
  for (int i = 0; i < size; i++) {
    data[i] -= min;
  }
  short max = SHRT_MIN;
  for (int i = 0; i < size; i++) {
    max = data[i] > max ? data[i] : max;
  }
  printf("max: %d\n", max);
  for (int i = 0; i < size; i++) {
    data[i] = (depth * data[i]) / max;
  }
}
void normalize_double(double *data, int size, int depth) {
  double min = DBL_MAX;
  for (int i = 0; i < size; i++) {
    min = data[i] < min ? data[i] : min;
  }
  printf("min: %f\n", min);
  for (int i = 0; i < size; i++) {
    data[i] -= min;
  }
  double max = DBL_MIN;
  for (int i = 0; i < size; i++) {
    max = data[i] > max ? data[i] : max;
  }
  printf("max: %f\n", max);
  for (int i = 0; i < size; i++) {
    data[i] = (depth * data[i]) / max;
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
  }
  short *temp = (short *)malloc(img->size * sizeof(short));
  for (int i = 0; i < img->size; i++) {
    temp[i] = (short)img->data[i] - (short)img2->data[i];
  }
  normalize_short(temp, img->size, 1 << img->bit_depth);
  for (int i = 0; i < img->size; ++i) {
    img->data[i] = temp[i];
  }
  free(temp);
}

void negative(image_t *img) {
  for (int i = 0; i < img->size; i++) {
    img->data[i] = ((1 << img->bit_depth) - 1) - img->data[i];
  }
}

void histogram_spec(image_t *img, double *h_target) {
  const int n = 1 << img->bit_depth;
  const double identity_budget = 1.0 / n;
  const double *h = get_normalized_histogram(img);
  if (!h) {
    fprintf(stderr, "get_normalized_histogram returned NULL\n");
    return;
  }
  int j = 1;
  int f[n];
  double count = 0;
  double budget = 0;
  for (int i = 0; i < n; ++i) {
    budget += h_target ? h_target[i] : identity_budget;
    while (j < n && count + h[j] < budget) {
      count += h[j];
      f[j++] = i;
    }
  }
  for (int i = 0; i < img->size; ++i) {
    img->data[i] = f[img->data[i]];
  }
}

void save_histogram(int *h, int n) {

  int fd = open("histogram", O_CREAT | O_RDWR, 0644);
  if (errno != 0) {
    printf("failed to create fd. errno: %s\n", strerror(errno));
  }
  write(fd, h, n * sizeof(int));
  if (errno != 0) {
    printf("failed to write image to fd. errno: %s\n", strerror(errno));
  }
  close(fd);
}

int *get_histogram(image_t *img) {
  if (!img || !img->data) {
    fprintf(stderr, "img or img->data is nullptr\n");
    return NULL;
  }
  int *h = malloc((1 << img->bit_depth) * sizeof(int));
  for (int i = 0; i < img->size; ++i) {
    h[img->data[i]]++;
  }
  return h;
}
double gaussian(int s, int t, double std) {
  double d = 2 * std * std;
  double k = 1.0 / (d * M_PI);
  return k * exp(-(s * s + t * t) / d);
}
void set_kernel(unsigned int kernel_width,
                double kernel[kernel_width][kernel_width],
                enum FILTER_TYPE type) {
  int h_width = kernel_width / 2;
  switch (type) {
  case BOX: {
    double weight = 1.0 / (kernel_width * kernel_width);
    printf("Kernel: ");
    for (int i = 0; i < kernel_width; ++i) {
      for (int j = 0; j < kernel_width; ++j) {
        kernel[i][j] = weight;
        printf("%f, ", kernel[i][j]);
      }
    }
    printf("\n");
  } break;
  case GAUSSIAN: {
    for (int i = -h_width; i <= h_width; ++i) {
      for (int j = -h_width; j <= h_width; ++j) {
        kernel[i + h_width][j + h_width] = gaussian(j, i, 5.0);
      }
    }
  } break;
  case LAPLACE4: {
    memset(kernel, 0, kernel_width * kernel_width * sizeof(double));
    for (int i = 0; i < kernel_width; ++i) {
      kernel[i][h_width] = 1;
      kernel[h_width][i] = 1;
    }
    kernel[h_width][h_width] = -4;
    printf("Kernel: ");
    for (int i = 0; i < kernel_width; ++i) {
      for (int j = 0; j < kernel_width; ++j) {
        printf("%f, ", kernel[i][j]);
      }
    }
    printf("\n");

  } break;
  case LAPLACE8: {
    printf("Kernel: ");
    for (int i = 0; i < kernel_width; ++i) {
      for (int j = 0; j < kernel_width; ++j) {
        kernel[j][i] = 1;
      }
    }
    kernel[h_width][h_width] = -8;
    for (int i = 0; i < kernel_width; ++i) {
      for (int j = 0; j < kernel_width; ++j) {
        printf("%f, ", kernel[i][j]);
      }
    }
    printf("\n");

  } break;
  default:
    break;
  }
}
void apply_kernel(image_t *img, unsigned int kernel_width,
                  double kernel[kernel_width][kernel_width]) {
  int h_width = kernel_width / 2;

  double *temp = malloc(img->size * sizeof(double));
  memset(temp, 0, img->size * sizeof(double));

  for (int i = h_width; i < img->height - h_width; ++i) {
    for (int j = h_width; j < img->width - h_width; ++j) {
      int idx = i * img->height + j;
      double res = 0.0;
      for (int y = i - h_width; y <= i + h_width; ++y) {
        for (int x = j - h_width; x <= j + h_width; ++x) {
          int neighbor_idx = y * img->height + x;
          res += img->data[neighbor_idx] *
                 kernel[y - i + h_width][x - j + h_width];
        }
      }

      temp[idx] = res;
    }
  }

  for (int i = 0; i < img->size; ++i) {
    // printf("%f, %d\n", temp[i], img->data[i]);
  }
  normalize_double(temp, img->size, (1 << img->bit_depth));
  for (int i = 0; i < img->size; ++i) {
    img->data[i] = (unsigned char)temp[i];
  }
  free(temp);
}

void apply_median(image_t *img, unsigned int kernel_width) {
  int kwidth2 = kernel_width * kernel_width;
  int h_width2 = kwidth2 / 2;
  int h_width = kernel_width / 2;
  int border = h_width * (img->width + 1);
  unsigned char *temp = malloc(img->size);
  for (int i = border; i < img->size - border; ++i) {
    int x0 = i % img->width;
    int y0 = i / img->width;

    if (x0 - h_width < 0 || x0 + h_width >= img->width)
      continue;
    if (y0 - h_width < 0 || y0 + h_width >= img->width)
      continue;

    unsigned char arr[kwidth2];

    int k = 0;

    for (int x = -h_width; x <= h_width; ++x) {
      for (int y = -h_width; y <= h_width; ++y) {
        int j = i + x + y * img->width;
        arr[k++] = img->data[j];
      }
    }
    sort(arr, kwidth2);
    temp[i] = arr[h_width2];
  }
  for (int i = 0; i < img->size; ++i) {
    img->data[i] = temp[i];
  }
  free(temp);
}
int filter(image_t *img, enum FILTER_TYPE type, unsigned int kernel_width) {
  if (kernel_width % 2 == 0) {
    fprintf(stderr, "kernel dimensions must be odd.\n");
    return 1;
  }
  if (type == MEDIAN) {
    apply_median(img, kernel_width);
    return 0;
  }
  double kernel[kernel_width][kernel_width];
  set_kernel(kernel_width, kernel, type);
  apply_kernel(img, kernel_width, kernel);
  return 0;
}
double *get_normalized_histogram(const image_t *img) {
  // Validate inputs
  if (!img || !img->data) {
    fprintf(stderr, "Error: Invalid image (NULL pointer)\n");
    return NULL;
  }

  if (img->size <= 0) {
    fprintf(stderr, "Error: Empty image (size=%d)\n", img->size);
    return NULL;
  }

  const int bins = 1 << img->bit_depth;
  const double total_pixels = (double)img->size;

  // Allocate and initialize histogram
  double *hist = calloc(bins, sizeof(double));
  if (!hist) {
    fprintf(stderr, "Error: Failed to allocate %d histogram bins\n", bins);
    return NULL;
  }

  // Build histogram with bounds checking
  for (int i = 0; i < img->size; ++i) {
    const unsigned char val = img->data[i];
    if (val >= bins) {
      fprintf(stderr, "Error: Pixel value %d exceeds %d-bit depth\n", val,
              img->bit_depth);
      free(hist);
      return NULL;
    }
    hist[val] += 1.0;
  }

  // Normalize using precise floating-point division
  for (int i = 0; i < bins; ++i) {
    hist[i] /= total_pixels;
  }

  return hist;
}

void free_image(image_t *img) {
  if (img != NULL && img->data != NULL) {
    free(img->data);
    img->data = NULL;
  }
}
