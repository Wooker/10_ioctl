#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define IOC_MAGIC 'k'

#define SUM_LENGTH _IOWR(IOC_MAGIC, 1, char *)
#define SUM_CONTENT _IOWR(IOC_MAGIC, 2, char *)

int main() {
  int fd, value, number;
  long res;
  char s[20];
  int a;

  printf("\nOpening Driver\n");
  fd = open("/dev/solution_node", O_RDWR);
  if (fd < 0) {
    printf("Cannot open device file...\n");
    return 0;
  }

  printf("Enter the IOCTL to send (0 length, 1 content)\n");
  scanf("%d", &a);
  printf("Enter the Value to send\n");
  scanf("%s", s);
  printf("Value is: %s\n", s);

  if (a == 0) {
    printf("Calculating the length\n");
    res = ioctl(fd, SUM_LENGTH, (char *) &s);
    printf("Length is: %lu\n", res);
  } else {
    printf("Calculating the content\n");
    res = ioctl(fd, SUM_CONTENT, (char *) &s);
    printf("Content is: %lu\n", res);
  }

  printf("Closing Driver\n");
  close(fd);
}
