#include "argparser.h"
#include "image_util.h"
#include "util.h"

int main(int argc, char **argv) {
  struct arguments arguments = {.input_file1 = NULL,
                                .input_file2 = NULL,
                                .output_file = "a.raw",
                                .transformation = 0,
                                .width = 0,
                                .height = 0,
                                .bit_depth = 8};

  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  //  printf("%s %s %s %d", arguments.input_file1, arguments.input_file2,
  //        arguments.output_file, arguments.transformation);
  int input_fd1 = open(arguments.input_file1, O_RDWR);

  if (errno != 0) {
    printf("failed to open input file 1. errno: %s\n", strerror(errno));
    return 1;
  }

  int input_fd2 = -1;

  if (arguments.input_file2) {
    input_fd2 = open(arguments.input_file2, O_RDWR);
    if (errno != 0) {
      fprintf(stderr, "failed to open input file 2. errno: %s\n",
              strerror(errno));
      return 1;
    }
  }
  int size = arguments.width * arguments.height * (arguments.bit_depth / 8.0f);
  image_t img1 = {.bit_depth = arguments.bit_depth,
                  .width = arguments.width,
                  .height = arguments.height,
                  .size = size,
                  .data = read_image(input_fd1, size)};

  if (!img1.data) {
    fprintf(stderr, "read image failed\n");
    return 1;
  }
  image_t img2 = {.bit_depth = img1.bit_depth,
                  .width = img1.width,
                  .height = img1.height,
                  .size = img1.size,
                  .data = NULL};

  if (input_fd2 != -1) {
    img2.data = read_image(input_fd2, img1.size);
    if (!img2.data) {
      fprintf(stderr, "read image2 failed\n");
      return 1;
    }
  }

  char buf[BUFFER_SIZE];
  enum STR2INT_ERROR err;

  switch (arguments.transformation) {
  case 0:
    break;
  case 2:
    negative(&img1);
    break;
  case 1:
    if (input_fd2 == -1) {
      fprintf(stderr, "Please provide valid path to second image.\n");
      return 1;
    }
    subtraction(&img1, &img2);
    break;
  case 3: {
    read_input(buf, "Enter X offset:\n");
    int x;
    err = str2int(&x, buf);
    if (err != SUCCESS) {
      fprintf(stderr, "Please enter valid X offset.\n");
      return 1;
    }
    read_input(buf, "Enter Y offset:\n");
    int y;
    err = str2int(&y, buf);
    if (err != SUCCESS) {
      fprintf(stderr, "Please enter valid Y offset.\n");
      return 1;
    }
    translation_forward(&img1, x, y);
    break;
  }
  case 4: {
    read_input(buf, "Enter Input Intensity Min:\n");
    int a;
    str2int(&a, buf);
    if (a < 0 || a >= (1 << img1.bit_depth)) {
      printf("Invalid Input Intensity\n");
      return 1;
    }

    read_input(buf, "Enter Input Intensity Max :\n");
    int b;
    str2int(&b, buf);
    if (b < 0 || b >= (1 << img1.bit_depth)) {
      printf("Invalid Input Intensity\n");
      return 1;
    }
    read_input(buf, "Enter Output Intensity");
    int level;
    str2int(&level, buf);
    if (level < 0 || level >= (1 << img1.bit_depth)) {
      printf("Invalid Output Intensity\n");
      return 1;
    }
    intensity_slice(&img1, a, b, level);
  }
  case 5: {
    if (input_fd2 == -1)
      histogram_spec(&img1, NULL);
    else
      histogram_spec(&img1, get_normalized_histogram(&img2));
  }
  }
  save_image(&img1, arguments.output_file);

  // Clean up
  free_image(&img1);
  free_image(&img2);
  close_fd(input_fd1);
  close_fd(input_fd2);

  return 0;
}
