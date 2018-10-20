/**
 * @Author: qlc
 * @Date:   2018-10-20T10:53:20+08:00
 * @Email:  qlcx@tom.com
 * @Filename: debug.c
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-20T10:53:32+08:00
 * @License: GPL
 */
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*定义ioctl相关结构体*/
typedef struct {
  unsigned int set_size;
  int set_clear;
} CHAR_DEV_SET;
typedef struct {
  int show_num;
  int show_size;
} CHAR_DEV_GET;
#define SET_VALUE _IOW('L', 1, CHAR_DEV_SET *)
#define GET_VALUE _IOR('L', 2, CHAR_DEV_GET *)

int main(void) {
  CHAR_DEV_SET value_S = {0};
  CHAR_DEV_GET value_G = {0};
  int fd = 0;
  int ret = 0;
  fd = open("/dev/chardev_r", O_RDWR);
  if (fd < 0) {
    return fd;
  }
  value_S.set_size = 100;
  value_S.set_clear = 0;
  ret = ioctl(fd, SET_VALUE, &value_S);
  printf("ret=%d\n", ret);

  ret = ioctl(fd, GET_VALUE, &value_G);
  printf("ret=%d\n", ret);
  printf("NUM:%d--SIZE:%d\n", value_G.show_num, value_G.show_size);

  close(fd);
  return 0;
}
