#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#define BUFFER_SIZE 1024
#define SHORT_MIN
enum STR2INT_ERROR { SUCCESS, OVERFLOW, UNDERFLOW, INCONVERTIBLE };
typedef struct image {
  unsigned char *data;
  int size;
  int bit_depth;
} image_t;

enum STR2INT_ERROR str2int(int *i, char const *s);

ssize_t read_input(char *buf, const char *msg);
#endif
