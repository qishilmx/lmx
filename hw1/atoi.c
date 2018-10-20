/**
 * @Author: qlc
 * @Date:   2018-10-20T19:19:29+08:00
 * @Email:  qlcx@tom.com
 * @Filename: atoi.c
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-20T22:29:26+08:00
 * @License: GPL
 */

#include "atoi.h"
#include <linux/proc_fs.h>
#include <linux/string.h>

#define PERR(fmt, args...)                                                     \
  do {                                                                         \
    printk(KERN_ERR "[%s:%d]" fmt, __FUNCTION__, __LINE__, ##args);            \
  } while (0)

/**
 * [my_atoi 把字符串中的数字转换成int型]
 * @param  str [字符串]
 * @return     [转换后的数字]
 */
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

/**
 * [str_rts 反转一个字符串]
 */
void str_rts(char *rts) {
  int i = 0, n = 0;
  char tmp;
  if (IS_ERR_OR_NULL(rts))
    return;

  n = strlen(rts);
  for (i = 0; i < (n / 2); i++) {
    tmp = rts[i];
    rts[i] = rts[n - i - 1];
    rts[n - i - 1] = tmp;
  }
}
