#ifndef IMAGE_UTIL_H
#define IMAGE_UTIL_H

#include "util.h"
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/io.h>
#include <unistd.h>

void save_image(image_t img, char *filename);

void intensity_slice(image_t img, int a, int b, int level);

void normalize(short *data, int size);

void subtraction(image_t img, image_t img2);

void translation(image_t img, int x_offset, int y_offset);

void negative(image_t img);

unsigned char *read_image(int fd, int size);

#endif
