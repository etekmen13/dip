#include "image_util.h"
char output_filename[BUFFER_SIZE];

int main(int argc, char **argv) {
  if (argc < 3) {
    fprintf(stderr,
            "Usage: %s\n <output_filepath> <input_filepath_1> "
            "<opt_input_filepath_2> <>\n",
            argv[0]);
    return 1;
  }
  int fd1 = open(argv[2], O_RDWR);

  if (errno != 0) {
    printf("failed to open file from argv[1]. errno: %s\n", strerror(errno));
    return 1;
  }
  int fd2 = -1;

  if (argc > 3) {
    fd2 = open(argv[2], O_RDWR);
    if (errno != 0) {
      printf("failed to open file from argv[2]. errno: %s\n", strerror(errno));
      return 1;
    }
  }
  char buf[BUFFER_SIZE];
  read_input(buf, "Enter Size of Image(s)\n");
  int size;
  str2int(&size, buf);
  if (size <= 0) {
    printf("input valid size\n");
    return 1;
  }
  read_input(buf, "Enter Bit-Depth\n");
  int bit_depth;
  str2int(&bit_depth, buf);

  if (bit_depth <= 0) {
    printf("input valid bit-depth\n");
    return 1;
  }
  read_input(buf, "Select Transformation:\n1. Subtraction\n2. Negative\n3. "
                  "Translation (Forward Mapping)\n4. Intensity Slicing\n");
  int transformation;
  str2int(&transformation, buf);

  if (transformation < 1 || transformation > 4) {
    printf("input valid transformation\n");
    return 1;
  }

  image_t img1;
  img1.bit_depth = bit_depth;
  img1.size = size * size * bit_depth / 8;
  img1.data = read_image(fd1, img1.size);

  if (!img1.data) {
    printf("read image failed\n");
    return 1;
  }

  image_t img2;
  img2.data = NULL;
  if (argc > 2) {
    img2.bit_depth = bit_depth;
    img2.size = size * size * bit_depth / 8;
    img2.data = read_image(fd2, img2.size);
    if (!img2.data) {
      printf("read image2 failed\n");
      return 1;
    }
  }

  int bytes_read = read_input(buf, "Enter output filename:\n");

  strncpy(output_filename, buf, bytes_read);

  if (output_filename[bytes_read - 1] == '\n') {
    output_filename[bytes_read - 1] = '\0';
  }

  switch (transformation) {
  case 2:
    negative(img1);
    break;
  case 1:
    if (fd2 == -1) {
      printf("Please provide valid path to second image.\n");
      return 1;
    }
    subtraction(img1, img2);
    break;
  case 3:
    read_input(buf, "Enter X offset:\n");
    int x;
    str2int(&x, buf);

    translation(img1, );
    break;
  case 4:
    read_input(buf, "Enter Input Intensity Min:\n");
    int a;
    str2int(&a, buf);
    if (a < 0 || a >= (1 << bit_depth)) {
      printf("Invalid Input Intensity\n");
      return 1;
    }

    read_input(buf, "Enter Input Intensity Max :\n");
    int b;
    str2int(&b, buf);
    if (b < 0 || b >= (1 << bit_depth)) {
      printf("Invalid Input Intensity\n");
      return 1;
    }
    read_input(buf, "Enter Output Intensity");
    int level;
    str2int(&level, buf);
    if (level < 0 || level >= (1 << bit_depth)) {
      printf("Invalid Output Intensity\n");
      return 1;
    }
    intensity_slice(img1, a, b, level);
  }

  // Clean up
  if (img1.data) {
    free(img1.data);
  }
  if (img2.data) {
    free(img2.data);
  }
  if (fd1 != -1) {
    close(fd1);
  }
  if (fd2 != -1) {
    close(fd2);
  }
  return 0;
}

enum STR2INT_ERROR str2int(int *i, char const *s) {
  char *end;
  long l;
  errno = 0;
  l = strtol(s, &end, 0);
  if ((errno == ERANGE && l == LONG_MAX) || l > INT_MAX) {
    return OVERFLOW;
  }
  if ((errno == ERANGE && l == LONG_MIN) || l < INT_MIN) {
    return UNDERFLOW;
  }
  if (*s == '\0' || *end != '\0') {
    return INCONVERTIBLE;
  }
  *i = l;
  return SUCCESS;
}

void intensity_slice(image_t img, int a, int b, int level) {

  for (int i = 0; i < img.size; i++) {
    if (img.data[i] >= a && img.data[i] <= b) {
      img.data[i] = level;
    }
  }
  write_image(img, output_filename);
}

void translation(image_t img, int x_offset, int y_offset) {}
ssize_t read_input(char *buf, const char *msg) {
  ssize_t bytes_read;
  do {

    printf("%s\n", msg);
    fflush(stdout);

    bytes_read = read(STDIN_FILENO, buf, BUFFER_SIZE);

    if (bytes_read == -1) {
      printf("Error reading from stdin, errno: %s\n", strerror(errno));
    }

  } while (bytes_read == -1);
  buf[bytes_read] = '\0';
  return bytes_read;
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
void subtraction(image_t img, image_t img2) {
  image_t out;
  out.size = img.size;
  out.bit_depth = img.bit_depth;
  out.data = malloc(out.size);
  short temp[out.size];
  for (int i = 0; i < out.size; i++) {
    temp[i] = (short)img.data[i] - (short)img2.data[i];

    printf("%d - %d = %d\n", img.data[i], img2.data[i], temp[i]);
  }
  // normalize(temp, out.size);
  for (int i = 0; i < out.size; i++) {
    out.data[i] = temp[i];
  }
  write_image(out, output_filename);
  free(out.data);
}

void negative(image_t img) {
  for (int i = 0; i < img.size; i++) {
    img.data[i] = ((1 << img.bit_depth) - 1) - img.data[i];
  }
  write_image(img, output_filename);
}
void write_image(image_t img, char *filename) {
  int fd = open(filename, O_CREAT | O_RDWR);
  if (errno != 0) {
    printf("failed to create fd. errno: %s\n", strerror(errno));
  }
  write(fd, img.data, img.size);
  if (errno != 0) {
    printf("failed to write image to fd. errno: %s\n", strerror(errno));
  }
  close(fd);
}

unsigned char *read_image(int fd, int size) {
  unsigned char *image = malloc(size);
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
