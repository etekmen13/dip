#ifndef UTIL_H
#define UTIL_H

#include <argp.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/io.h>
#include <unistd.h>
#define BUFFER_SIZE 1024

#define TRANSFORMATION_MAX 4
#define TRANSFORMATION_MIN 1
#define HELP_MSG                                                               \
  "Example Usage:\n"                                                           \
  "dip -t 1 -o subtract.raw 8 512 512 ~/input1.raw ~/input2.raw\n\n"           \
  "Options:\n"                                                                 \
  "-h --help                            Show this Screen.\n"                   \
  "-v --version                         Show version.\n"                       \
  "-o --output <output_filepath>        Define output filepath. Default = "    \
  "./a.raw\n"                                                                  \
  "-t --transformation <type>           Define transformation type.\n\n"       \
  "Transformation Types:\n"                                                    \
  "1: Subtract (Requires 2 Images)\n"                                          \
  "2: Negate\n"                                                                \
  "3: Translate\n"                                                             \
  "4: Intensity Slice\n"

#define VERSION "dip v1.0.0\n"

enum STR2INT_ERROR { SUCCESS, OVERFLOW, UNDERFLOW, INCONVERTIBLE };

enum STR2INT_ERROR str2int(int *i, char const *s);

ssize_t read_input(char *buf, const char *msg);

void close_fd(int fd);
#endif
