/**
 * @Author: qlc
 * @Date:   2018-10-20T10:53:20+08:00
 * @Email:  qlcx@tom.com
 * @Filename: debug.c
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-21T01:07:27+08:00
 * @License: GPL
 */
#if 0
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
#endif
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct user_led_con_t {
  uint8_t power;
  uint8_t num;
  uint8_t status;
};

int main(void) {
  int fd = 0;
  int ret = 0;
  int i = 0;
  struct user_led_con_t con = {0};
  fd = open("/dev/mydev", O_RDWR);
  if (fd < 0) {
    ret = fd;
    goto open_myled_err;
  }
  for (i = 0; i < 15; i++) {
    if ((i % 4) == 0)
      con.power = !con.power;
    con.num = i % 4;
    ret = write(fd, &con, sizeof(con));
    if (ret < 0) {
      perror("write");
    }
    sleep(1);
  }

  ret = read(fd, &con, sizeof(con));
  if (ret < 0) {
    perror("read");
  }

  printf("%#x\n", con.status);
  close(fd);
  return 0;
open_myled_err:
  return ret;
}
