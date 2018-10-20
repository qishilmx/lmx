/**
 * @Author: qlc
 * @Date:   2018-10-20T19:19:29+08:00
 * @Email:  qlcx@tom.com
 * @Filename: atoi.c
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-20T19:27:44+08:00
 * @License: GPL
 */

#include "atoi.h"

int my_atoi(const char *str) {
  int flag = 1, ret = 0;
  while (*str == ' ')
    str++;
  if (*str == '+')
    str++;
  else if (*str == '-') {
    flag = -1;
    str++;
  }
  while (*str >= '0' || *str <= '9') {
    ret = ret * 10 + *str - '0';
    str++;
    if (ret < 0) {
      ret = 2147483647;
      break;
    }
  }
  return (ret * (flag ? -1 : 1));
}
