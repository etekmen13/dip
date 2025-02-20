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
void swap(unsigned char *arr, int l, int r) {
  unsigned char temp = arr[l];
  arr[l] = arr[r];
  arr[r] = temp;
}
int partition(unsigned char *arr, int i, int r) {
  unsigned char pivot = arr[i];
  int l = i + 1;
  while (l < r) {
    while (arr[l] < pivot)
      ++l;

    while (arr[r] >= pivot)
      --r;

    swap(arr, l, r);
  }
  swap(arr, i, r);
  return r;
}
void quick_sort_helper(unsigned char *arr, int l, int r) {
  if (r <= l) {
    return;
  }
  int pivot_idx = partition(arr, l, r);
  quick_sort_helper(arr, l, pivot_idx);
  quick_sort_helper(arr, pivot_idx, r);
}
void quick_sort(unsigned char *arr, unsigned int n) {
  quick_sort_helper(arr, 0, n - 1);
}

void insertion_sort(unsigned char *arr, unsigned int n) {
  for (int i = 0; i < n; ++i) {
    int temp = arr[i];
    int j = 0;
    while (j < i && arr[j] < arr[i])
      ++j;
    for (int k = i - 1; k >= j; --k) {
      arr[k + 1] = arr[k];
    }
    arr[j] = temp;
  }
}

void sort(unsigned char *arr, unsigned int n) {
  if (n > 20) {
    quick_sort(arr, n);
  } else {
    insertion_sort(arr, n);
  }
}
