#include "argparser.h"
#include "util.h"

struct argp_option options[] = {
    {"help", 'h', 0, 0, "Show this Screen"},
    {"version", 'v', 0, 0, "Show version"},
    {"output", 'o', "FILE", 0, "Define output filepath. Default = ./a.raw"},
    {"transformation", 't', "TYPE", 0, "Define transformation type"},
    {0}};

struct argp argp = {options, parse_opt, "DEPTH WIDTH HEIGHT FILE1 [FILE2]",
                    HELP_MSG};
int parse_opt(int key, char *arg, struct argp_state *state) {
  struct arguments *arguments = state->input;

  switch (key) {
  case 'h':
    fprintf(stdout, HELP_MSG);
    argp_usage(state);
    break;
  case 'v':
    fprintf(stdout, "%s\n", VERSION);
    argp_usage(state);
    break;
  case 'o':
    arguments->output_file = arg;
    break;
  case 't': {
    enum STR2INT_ERROR err = str2int(&arguments->transformation, arg);
    if (err != SUCCESS || arguments->transformation < TRANSFORMATION_MIN ||
        arguments->transformation > TRANSFORMATION_MAX) {
      argp_error(state, "Invalid Transformation Type. \n\n");
    }
    break;
  }
  case ARGP_KEY_ARG:
    switch (state->arg_num) {
    case 0: {
      enum STR2INT_ERROR err = str2int(&arguments->bit_depth, arg);
      if (err != SUCCESS || arguments->bit_depth < 0) {
        argp_error(state, "Invalid Bit-Depth. \n\n");
      }
    }
    case 1: {
      enum STR2INT_ERROR err = str2int(&arguments->width, arg);
      if (err != SUCCESS || arguments->width < 0) {
        argp_error(state, "Invalid Width. \n\n");
      }
      break;
    }
    case 2: {
      enum STR2INT_ERROR err = str2int(&arguments->height, arg);
      if (err != SUCCESS || arguments->height < 0) {
        argp_error(state, "Invalid Height. \n\n");
      }
      break;
    }
    case 3:
      arguments->input_file1 = arg;
      break;
    case 4:
      arguments->input_file2 = arg;
      break;
    default:
      argp_error(state, "Too many arguments.");
      break;
    }
    break;
  case ARGP_KEY_END:
    if (state->arg_num < 4) {
      argp_error(
          state,
          "Bit-Depth, Width, Height, and First input file are required.");
    }
    break;
  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}
