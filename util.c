#include "util.h"

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

void close_fd(int fd) {
  if (fd != -1) {
    close(fd);
  }
}

enum STR2INT_ERROR str2int(int *i, char const *s) {
  char *end;
  long l;
  errno = 0;
  // printf("str2int: %s\n", s);
  l = strtol(s, &end, 0);
  if ((errno == ERANGE && l == LONG_MAX) || l > INT_MAX) {
    return OVERFLOW;
  }
  if ((errno == ERANGE && l == LONG_MIN) || l < INT_MIN) {
    return UNDERFLOW;
  }
  if (*s == '\0' || (*end != '\0' && *end != '\n')) {
    return INCONVERTIBLE;
  }
  *i = l;
  return SUCCESS;
}
