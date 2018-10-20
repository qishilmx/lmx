/**
 * @Author: qlc
 * @Date:   2018-10-20T19:19:29+08:00
 * @Email:  qlcx@tom.com
 * @Filename: atoi.c
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-20T21:29:52+08:00
 * @License: GPL
 */

#include "atoi.h"
#include <linux/proc_fs.h>

#define PERR(fmt, args...)                                                     \
  do {                                                                         \
    printk(KERN_ERR "[%s:%d]" fmt, __FUNCTION__, __LINE__, ##args);            \
  } while (0)

int my_atoi(const char *str) {
  int flag = 1, ret = 0, i = 0;
  for (i = 0; str[i] != '\0'; i++) {
    if (str[i] <= '9' && str[i] >= '0') {
      ret = ((ret * 10) + (str[i] - '0'));
    }
    if (str[i] == '+' || str[i] == '-') {
      if (str[i] == '-')
        flag = 0;
    }
  }
  return (ret * (flag ? 1 : -1));
}
