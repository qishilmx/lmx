/**
 * @Author: qlc
 * @Date:   2018-10-19T09:14:22+08:00
 * @Email:  qlcx@tom.com
 * @Filename: Basics.c
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-19T13:14:37+08:00
 * @License: GPL
 */
#include <linux/init.h>
#include <linux/module.h>

#define PERR(fmt, args...)                                                     \
  do {                                                                         \
    printk(KERN_ERR "[%s:%d]" fmt, __FUNCTION__, __LINE__, ##args);            \
  } while (0)

MODULE_LICENSE("GPL");
/*----------------------------开始添加----------------------------*/
/*----------------------------结束添加----------------------------*/
static __init int Basics_init(void) {
  /*开始添加*/
  /*暂停添加*/
  PERR("INIT\n");
  return 0;
  /*继续添加*/
  /*结束添加*/
}

static __exit void Basics_exit(void) {
  /*开始添加*/
  /*结束添加*/
  PERR("EXIT\n");
  return;
}

module_init(Basics_init);
module_exit(Basics_exit);
