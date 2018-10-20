/**
 * @Author: qlc
 * @Date:   2018-10-19T22:49:18+08:00
 * @Email:  qlcx@tom.com
 * @Filename: char_proc_ioctl.c
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-20T22:08:36+08:00
 * @License: GPL
 */
#include "stack_r.h"
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <linux/module.h>
//#include <linux/proc_fs.h>
#include "atoi.h"
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/uaccess.h>

#define PERR(fmt, args...)                                                     \
  do {                                                                         \
    printk(KERN_ERR "[%s:%d]" fmt, __FUNCTION__, __LINE__, ##args);            \
  } while (0)

MODULE_LICENSE("GPL");
/*----------------------------开始添加----------------------------*/
#define BUF_SIZE 128
/*定义一个全局大小变量*/
static unsigned int SIZE_VALUE = 512;

/*定义一个栈指针*/
STACK_R *stacks;
/*定义一个栈指针*/
STACK_R *proc_stacks_r;

/*定义字符设备相关结构体*/
typedef struct {
  dev_t chardev_r_num;
  struct cdev chardev_r_dev;
  struct file_operations chardev_r_ops;
  struct class *chardev_r_class;
  struct device *chardev_r_device;
} CHAR_DEV_R;

#if 1
/*proc相关操作*/

/*proc操作函数*/
int open_r(struct inode *inode, struct file *file) { return 0; }
int release_r(struct inode *inode, struct file *file) { return 0; }
ssize_t read_r(struct file *file, char __user *ubuf, size_t size, loff_t *pos) {
  return stack_pop(proc_stacks_r, ubuf, size);
}
ssize_t write_r(struct file *file, const char __user *ubuf, size_t size,
                loff_t *pos) {
  /*写操作*/
  int ret = 0, str = 0, sret = 0, num = 0;
  char *set_clean = "set clear stack";
  char *set_sta_size = "set stack size";
  char *show_sta_da = "show stack data";
  char buf[BUF_SIZE] = {'\0'};
  STACK_R *stacks_bck = NULL;

  /*每次写之前清空一次*/
  proc_stacks_r->S_D_TOP = 0;
  proc_stacks_r->S_D_NUM = 0;
  ret = stack_push(proc_stacks_r, ubuf, size);
  /*进行字符判断并进行相关操作*/
  if (ret > strlen(set_clean)) {
    str = strncmp(set_clean, proc_stacks_r->S_DATA, strlen(set_clean));
    if (str == 0) {
      stacks->S_D_TOP = 0;
      stacks->S_D_NUM = 0;
    }
    str = strncmp(set_sta_size, proc_stacks_r->S_DATA, strlen(set_sta_size));
    if (str == 0) {
      /*设置大小*/
      num = my_atoi(proc_stacks_r->S_DATA);
      PERR("num = %d\n", num);
      if (num > 0) {
        stacks_bck = stack_create(num);
        if (IS_ERR_OR_NULL(stacks_bck))
          goto stack_bck_create_err;
        if (num < stacks->S_D_NUM) {
          stacks_bck->S_D_TOP = num;
          stacks_bck->S_D_NUM = num;
          strncpy(stacks_bck->S_DATA, stacks->S_DATA, num);
        } else {
          stacks_bck->S_D_TOP = stacks->S_D_TOP;
          stacks_bck->S_D_NUM = stacks->S_D_NUM;
          strncpy(stacks_bck->S_DATA, stacks->S_DATA, stacks->S_D_NUM);
        }
        stack_destroy(stacks);
        stacks = stacks_bck;
        stacks_bck = NULL;
      }
    }
    str = strncmp(show_sta_da, proc_stacks_r->S_DATA, strlen(show_sta_da));
    if (str == 0) {
      /*设置显示数据*/
      sret = snprintf(buf, sizeof(buf), "%s%d%s%d\n%s", "NUM:", stacks->S_D_NUM,
                      "SIZE", stacks->S_D_SIZE, "DATA:");
      strncat(buf, stacks->S_DATA, stacks->S_D_NUM);
      proc_stacks_r->S_D_TOP = strlen(buf);
      proc_stacks_r->S_D_NUM = strlen(buf);
      proc_stacks_r->S_D_SIZE = stacks->S_D_SIZE;
      str_rts(buf);
      strncpy(proc_stacks_r->S_DATA, buf, strlen(buf));
    }
  }
  return ret;
stack_bck_create_err:
  return 0;
}

/*定义一个proc文件结构体指针*/
struct proc_dir_entry *proc_r;
/*定义file_operations结构体,并初始化,在该结构体上面实现相应函数*/
struct file_operations fops = {
    .open = open_r, .release = release_r, .read = read_r, .write = write_r,
};

struct proc_dir_entry *proc_r_create(const char *name, umode_t mode,
                                     struct proc_dir_entry *parent,
                                     const struct file_operations *proc_fops,
                                     void *data) {
  proc_stacks_r = stack_create(BUF_SIZE);
  if (IS_ERR_OR_NULL(proc_stacks_r))
    goto stack_create_proc_err;
  proc_r = proc_create_data(name, mode, parent, proc_fops, data);
  if (IS_ERR_OR_NULL(proc_r))
    goto proc_create_data_err;
  return proc_r;
proc_create_data_err:
  stack_destroy(proc_stacks_r);
stack_create_proc_err:
  return NULL;
}

void proc_r_remove(const char *name, struct proc_dir_entry *parent) {
  remove_proc_entry(name, parent);
}

#endif

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
/*定义相应操作函数*/
int chardev_r_open(struct inode *inode, struct file *file) {
  /*保存不会被内核改变的数据*/
  CHAR_DEV_R *c = container_of(inode->i_cdev, CHAR_DEV_R, chardev_r_dev);
  file->private_data = c;
  return 0;
}
int chardev_r_release(struct inode *inode, struct file *file) { return 0; }
ssize_t chardev_r_read(struct file *file, char __user *buffer, size_t size,
                       loff_t *pos) {
  return stack_pop(stacks, buffer, size);
}
ssize_t chardev_r_write(struct file *file, const char __user *buffer,
                        size_t size, loff_t *pos) {
  int ret = 0;
  ret = stack_push(stacks, buffer, size);
  str_rts(stacks->S_DATA);
  return ret;
}
long chardev_r_unlocked_ioctl(struct file *file, unsigned int cmd,
                              unsigned long data) {
  int ret = 0;
  // CHAR_DEV_R *c = file->private_data; /*暂时未使用*/
  CHAR_DEV_SET value_S;
  CHAR_DEV_GET value_G;
  STACK_R *stacks_bck = NULL;

  switch (cmd) {
  case SET_VALUE:
    ret = copy_from_user(&value_S, (void *)data, sizeof(value_S));
    if (value_S.set_clear) {
      stacks->S_D_TOP = 0;
      stacks->S_D_NUM = 0;
      value_S.set_clear = 0;
    }
    if (value_S.set_size > 0) {
      stacks_bck = stack_create(value_S.set_size);
      if (IS_ERR_OR_NULL(stacks_bck))
        goto stack_bck_create_err;
      if (value_S.set_size < stacks->S_D_NUM) {
        stacks_bck->S_D_TOP = value_S.set_size;
        stacks_bck->S_D_NUM = value_S.set_size;
        strncpy(stacks_bck->S_DATA, stacks->S_DATA, value_S.set_size);
      } else {
        stacks_bck->S_D_TOP = stacks->S_D_TOP;
        stacks_bck->S_D_NUM = stacks->S_D_NUM;
        strncpy(stacks_bck->S_DATA, stacks->S_DATA, stacks->S_D_NUM);
      }
      stack_destroy(stacks);
      stacks = stacks_bck;
      stacks_bck = NULL;
    }
    break;
  case GET_VALUE:
    value_G.show_num = stacks->S_D_NUM;
    value_G.show_size = stacks->S_D_SIZE;
    ret = copy_to_user((void *)data, &value_G, sizeof(value_G));
    break;
  }

  return 0;
stack_bck_create_err:
  return 0;
}

/*定义字符设备结构体指针*/
CHAR_DEV_R *chardev_r = NULL;

CHAR_DEV_R *chardev_r_create(void) {
  int ret = 0;
  CHAR_DEV_R *cdr;
  cdr = kzalloc(sizeof(*cdr), GFP_KERNEL);
  if (IS_ERR_OR_NULL(cdr))
    goto kzalloc_err;
  /*      申请设备号并注册    */
  ret = alloc_chrdev_region(&cdr->chardev_r_num, 0, 1, "chardev_r");
  if (ret) {
    PERR("alloc chardev_r chrdev region err\n");
    goto alloc_chardev_r_chrdev_region_err;
  }
  /*初始化设备*/
  cdev_init(&cdr->chardev_r_dev, &cdr->chardev_r_ops);

  /*初始化相应操作函数*/
  cdr->chardev_r_ops.open = chardev_r_open;
  cdr->chardev_r_ops.release = chardev_r_release;
  cdr->chardev_r_ops.read = chardev_r_read;
  cdr->chardev_r_ops.write = chardev_r_write;
  cdr->chardev_r_ops.unlocked_ioctl = chardev_r_unlocked_ioctl;

  /*添加设备*/
  ret = cdev_add(&cdr->chardev_r_dev, cdr->chardev_r_num, 1);
  if (ret) {
    PERR("cdev_add_chardev_r_device_err\n");
    goto cdev_add_chardev_r_device_err;
  }

  cdr->chardev_r_class = class_create(THIS_MODULE, "chardev_r");
  if (IS_ERR_OR_NULL(cdr->chardev_r_class)) {
    PERR("chardev_r_class_create_err\n");
    ret = -ENOMEM;
    goto chardev_r_class_create_err;
  }
  cdr->chardev_r_device = device_create(cdr->chardev_r_class, NULL,
                                        cdr->chardev_r_num, NULL, "chardev_r");
  if (IS_ERR_OR_NULL(cdr->chardev_r_device)) {
    PERR("chardev_r_device_create_err\n");
    ret = -ENOMEM;
    goto chardev_r_device_create_err;
  }

  return cdr;
chardev_r_device_create_err:
  class_destroy(cdr->chardev_r_class);
chardev_r_class_create_err:
  cdev_del(&cdr->chardev_r_dev);
cdev_add_chardev_r_device_err:
  unregister_chrdev_region(cdr->chardev_r_num, 1);
alloc_chardev_r_chrdev_region_err:
  kfree(cdr);
kzalloc_err:
  return NULL;
}

void chardev_r_destroy(CHAR_DEV_R *cdr) {
  device_destroy(cdr->chardev_r_class, cdr->chardev_r_num);
  class_destroy(cdr->chardev_r_class);
  cdev_del(&cdr->chardev_r_dev);
  unregister_chrdev_region(cdr->chardev_r_num, 1);
  kfree(cdr);
}

/*----------------------------结束添加----------------------------*/
static __init int char_proc_ioctl_init(void) {
  /*开始添加*/
  /*proc*/
  proc_r = proc_r_create("proc_r", 0644, NULL, &fops, NULL);
  if (IS_ERR_OR_NULL(proc_r))
    goto proc_r_create_err;

  stacks = stack_create(SIZE_VALUE);
  if (IS_ERR_OR_NULL(stacks))
    goto stack_create_err;
  chardev_r = chardev_r_create();
  if (IS_ERR_OR_NULL(chardev_r))
    goto chardev_r_create_err;
  /*暂停添加*/
  PERR("INIT\n");
  return 0;
/*继续添加*/
chardev_r_create_err:
  stack_destroy(stacks);
stack_create_err:
  proc_r_remove("proc_r", NULL);
proc_r_create_err:
  return -ENOMEM;
  /*结束添加*/
}

static __exit void char_proc_ioctl_exit(void) {
  /*开始添加*/
  chardev_r_destroy(chardev_r);
  proc_r_remove("proc_r", NULL);
  stack_destroy(proc_stacks_r);
  stack_destroy(stacks);
  /*结束添加*/
  PERR("EXIT\n");
  return;
}

module_init(char_proc_ioctl_init);
module_exit(char_proc_ioctl_exit);
