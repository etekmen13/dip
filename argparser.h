#ifndef ARGPARSER_H
#define ARGPARSER_H
#include "util.h"
#include <argp.h>
struct arguments {
  char *input_file1;
  char *input_file2;
  char *output_file;
  int width;
  int height;
  int bit_depth;
  int transformation;
};

extern struct argp_option options[];
int parse_opt(int key, char *arg, struct argp_state *state);

extern struct argp argp;
#endif
