/**
 * @Author: qlc
 * @Date:   2018-10-20T10:53:20+08:00
 * @Email:  qlcx@tom.com
 * @Filename: debug.c
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-21T18:02:26+08:00
 * @License: GPL
 */
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*定义ioctl相关结构体*/
typedef struct {
  unsigned int led_num_on;  /*1,2,3,4对应相应的led灯，5为全部打开*/
  unsigned int led_num_off; /*1,2,3,4对应相应的led灯，5为全部关闭*/
  unsigned int buzzer_follow; /*1为跟随0为不跟随*/
} LED_OR_BUZZER_SET;
#define SET_GET_VALUE _IOWR('L', 1, LED_OR_BUZZER_SET *)

int main(void) {
  int fd = 0;
  int ret = 0;
  LED_OR_BUZZER_SET or_set;

  fd = open("/dev/mydev", O_RDWR);
  if (fd < 0) {
    return fd;
  }
  or_set.led_num_on = 1;
  or_set.led_num_off = 0;
  or_set.buzzer_follow = 0;

  ret = ioctl(fd, SET_GET_VALUE, &or_set);
  printf("ret=%d\n", ret);

  close(fd);
  return 0;
}
