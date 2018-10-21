/**
 * @Author: qlc
 * @Date:   2018-10-20T10:54:04+08:00
 * @Email:  qlcx@tom.com
 * @Filename: chardev.c
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-21T17:00:53+08:00
 * @License: GPL
 */
#include "atoi.h"
#include "buzzer.h"
#include "led.h"
#include "stack_r.h"
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/uaccess.h>

#define PERR(fmt, args...)                                                     \
  do {                                                                         \
    printk(KERN_ERR "[%s:%d]" fmt, __FUNCTION__, __LINE__, ##args);            \
  } while (0)

MODULE_LICENSE("GPL");
/*----------------------------开始添加----------------------------*/

#define ST_SIZE 512

/*定义字符设备相关结构体*/
typedef struct {
  void __iomem *led_value;
  void __iomem *buzzer_value;
  dev_t chardev_r_num;
  struct cdev chardev_r_dev;
  struct file_operations chardev_r_ops;
  struct class *chardev_r_class;
  struct device *chardev_r_device;
} CHAR_DEV_R;

CHAR_DEV_R *save = NULL;
/*创建一个栈指针*/
STACK_R *stacks;
/*定义一个proc文件结构体指针*/
struct proc_dir_entry *proc_r;
STACK_R *proc_stacks_r;

int follow[5] = {0};

/*proc操作函数*/
int open_r(struct inode *inode, struct file *file) { return 0; }
int release_r(struct inode *inode, struct file *file) { return 0; }
ssize_t read_r(struct file *file, char __user *ubuf, size_t size, loff_t *pos) {
  return stack_pop(proc_stacks_r, ubuf, size);
}
ssize_t write_r(struct file *file, const char __user *ubuf, size_t size,
                loff_t *pos) {
  int ret = 0, sret = 0;
  CHAR_DEV_R *p = NULL;
  LED_STATUS get_d = {0};
  int led0 = 0, led1 = 0, led2 = 0, led3 = 0, buzzer = 0;
  char *show_data_p = "show status";
  char tmp[ST_SIZE] = {'\0'};

  if (IS_ERR_OR_NULL(save))
    return size;

  p = save;

  proc_stacks_r->S_D_TOP = 0;
  proc_stacks_r->S_D_NUM = 0;
  ret = stack_push(proc_stacks_r, ubuf, size);

  sret = strncmp(show_data_p, proc_stacks_r->S_DATA, strlen(show_data_p));
  PERR("sert:%d\n", sret);
  if (!sret) {
    led_get_status(p->led_value, &get_d.status);
    if (get_d.status & (1 << 0))
      led0 = 1;
    else if (get_d.status & (1 << 1))
      led1 = 1;
    else if (get_d.status & (1 << 2))
      led2 = 1;
    else if (get_d.status & (1 << 3))
      led3 = 1;
    if (buzzer_on_or_off(p->buzzer_value))
      buzzer = 1;
    snprintf(tmp, sizeof(tmp), "\n%s%d--%s%d--%s%d--%s%d--%s%d\n", "led0:",
             led0, "led1:", led1, "led2:", led2, "led3:", led3, "buzzer:",
             buzzer);
  }
  PERR("tmp:%s\n", tmp);
  proc_stacks_r->S_D_TOP = strlen(tmp);
  proc_stacks_r->S_D_NUM = strlen(tmp);
  str_rts(tmp);
  PERR("tmp:%s\n", tmp);
  strncpy(proc_stacks_r->S_DATA, tmp, strlen(tmp));
  PERR("S_DATA:%s\n", proc_stacks_r->S_DATA);
  return ret;
}

/*定义file_operations结构体,并初始化,在该结构体上面实现相应函数*/
struct file_operations fops = {
    .open = open_r, .release = release_r, .read = read_r, .write = write_r,
};

struct proc_dir_entry *proc_r_create(const char *name, umode_t mode,
                                     struct proc_dir_entry *parent,
                                     const struct file_operations *proc_fops,
                                     void *data) {
  struct proc_dir_entry *proc_c;
  proc_stacks_r = stack_create(ST_SIZE);
  if (IS_ERR_OR_NULL(proc_stacks_r))
    goto stack_create_proc_err;
  proc_c = proc_create_data(name, mode, parent, proc_fops, data);
  if (IS_ERR_OR_NULL(proc_c))
    goto proc_create_data_err;
  return proc_c;
proc_create_data_err:
  stack_destroy(proc_stacks_r);
stack_create_proc_err:
  return NULL;
}

void proc_r_remove(const char *name, struct proc_dir_entry *parent) {
  remove_proc_entry(name, parent);
}

/*定义ioctl相关结构体*/
typedef struct {
  unsigned int led_num_on;  /*1,2,3,4对应相应的led灯，5为全部打开*/
  unsigned int led_num_off; /*1,2,3,4对应相应的led灯，5为全部关闭*/
  unsigned int buzzer_follow; /*1为跟随0为不跟随*/
} LED_OR_BUZZER_SET;
#define SET_GET_VALUE _IOWR('L', 1, LED_OR_BUZZER_SET *)

/*定义相应操作函数*/
int chardev_r_open(struct inode *inode, struct file *file) {
  /*保存不会被内核改变的数据*/
  CHAR_DEV_R *c = container_of(inode->i_cdev, CHAR_DEV_R, chardev_r_dev);
  file->private_data = c;
  save = c;
  return 0;
}
int chardev_r_release(struct inode *inode, struct file *file) { return 0; }
ssize_t chardev_r_read(struct file *file, char __user *buffer, size_t size,
                       loff_t *pos) {
  int ret = 0;
  ret = stack_pop(stacks, buffer, size);
  stacks->S_D_TOP = 0;
  stacks->S_D_NUM = 0;
  return ret;
}
ssize_t chardev_r_write(struct file *file, const char __user *buffer,
                        size_t size, loff_t *pos) {
  CHAR_DEV_R *c = file->private_data;
  /*定义命令字符串*/
  char *set_led_on = "set led on";
  char *set_led_off = "set led off";
  char *show_status = "show status";
  char *set_buzzer_on = "set buzzer on";
  char *set_buzzer_off = "set buzzer off";

  int ret = 0, str = 0, num = 0;
  int led0 = 0, led1 = 0, led2 = 0, led3 = 0, buzzer = 0;
  char tmp[ST_SIZE] = {'\0'};
  LED_STATUS get_d = {0};

  /*每次写之前清空一次*/
  stacks->S_D_TOP = 0;
  stacks->S_D_NUM = 0;
  ret = stack_push(stacks, buffer, size);
  /*判断命令行*/
  /*led*/
  str = strncmp(set_led_on, stacks->S_DATA, strlen(set_led_on));
  if (!str) {
    num = my_atoi(stacks->S_DATA, stacks->S_D_NUM);
    led_on(c->led_value, num);
    follow[num] = 1;
  }
  str = strncmp(set_led_off, stacks->S_DATA, strlen(set_led_off));
  if (!str) {
    num = my_atoi(stacks->S_DATA, stacks->S_D_NUM);
    led_off(c->led_value, num);
    follow[num] = 0;
  }
  /*buzzer*/
  str = strncmp(set_buzzer_on, stacks->S_DATA, strlen(set_buzzer_on));
  if (!str) {
    num = my_atoi(stacks->S_DATA, stacks->S_D_NUM);
    buzzer_on(c->buzzer_value);
  }
  str = strncmp(set_buzzer_off, stacks->S_DATA, strlen(set_buzzer_off));
  if (!str) {
    num = my_atoi(stacks->S_DATA, stacks->S_D_NUM);
    buzzer_off(c->buzzer_value);
    follow[0] = 0;
    follow[1] = 0;
    follow[2] = 0;
    follow[3] = 0;
    follow[4] = 0;
  }
  if (follow[0] == 1 || follow[1] == 1 || follow[2] == 1 || follow[3] == 1 ||
      follow[4] == 1) {
    buzzer_on(c->buzzer_value);
  } else {
    buzzer_off(c->buzzer_value);
  }
  /*显示状态,写命令到设备后，必须查看设备，中间不能进行其他操作，否则数据会被清空*/
  str = strncmp(show_status, stacks->S_DATA, strlen(show_status));
  if (!str) {
    led_get_status(c->led_value, &get_d.status);
    if (get_d.status & (1 << 0))
      led0 = 1;
    else if (get_d.status & (1 << 1))
      led1 = 1;
    else if (get_d.status & (1 << 2))
      led2 = 1;
    else if (get_d.status & (1 << 3))
      led3 = 1;
    if (buzzer_on_or_off(c->buzzer_value))
      buzzer = 1;
    snprintf(tmp, sizeof(tmp), "\n%s%d--%s%d--%s%d--%s%d--%s%d\n", "led0:",
             led0, "led1:", led1, "led2:", led2, "led3:", led3, "buzzer:",
             buzzer);
  }

  stacks->S_D_TOP = strlen(tmp);
  stacks->S_D_NUM = strlen(tmp);
  str_rts(tmp);
  strncpy(stacks->S_DATA, tmp, strlen(tmp));
  return ret;
}
long chardev_r_unlocked_ioctl(struct file *file, unsigned int cmd,
                              unsigned long data) {
  int ret = 0;
  CHAR_DEV_R *c = file->private_data;
  LED_OR_BUZZER_SET or_set;

  switch (cmd) {
  case SET_GET_VALUE:
    ret = copy_from_user(&or_set, (void *)data, sizeof(or_set));
    if (or_set.led_num_on) {
      switch (or_set.led_num_on) {
      case 1:
        led_on(c->led_value, 0);
        follow[0] = 1;
        break;
      case 2:
        led_on(c->led_value, 1);
        follow[1] = 1;
        break;
      case 3:
        led_on(c->led_value, 2);
        follow[2] = 1;
        break;
      case 4:
        led_on(c->led_value, 3);
        follow[3] = 1;
        break;
      case 5:
        led_on(c->led_value, 0);
        led_on(c->led_value, 1);
        led_on(c->led_value, 2);
        led_on(c->led_value, 3);
        follow[4] = 1;
        break;
      }
    }
    if (or_set.led_num_off) {
      switch (or_set.led_num_off) {
      case 1:
        led_off(c->led_value, 0);
        follow[0] = 0;
        break;
      case 2:
        led_off(c->led_value, 1);
        follow[1] = 0;
        break;
      case 3:
        led_off(c->led_value, 2);
        follow[2] = 0;
        break;
      case 4:
        led_off(c->led_value, 3);
        follow[3] = 0;
        break;
      case 5:
        led_off(c->led_value, 0);
        led_off(c->led_value, 1);
        led_off(c->led_value, 2);
        led_off(c->led_value, 3);
        follow[4] = 0;
        break;
      }
    }
    if (or_set.buzzer_follow) {
      if (follow[0] == 1 || follow[1] == 1 || follow[2] == 1 ||
          follow[3] == 1 || follow[4] == 1) {
        buzzer_on(c->buzzer_value);
      } else {
        buzzer_off(c->buzzer_value);
      }
    }
    break;
  }

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
  cdr->led_value = led_init(); /*需要修改*/
  if (IS_ERR_OR_NULL(cdr->led_value))
    goto led_init_err;
  cdr->buzzer_value = buzzer_init(); /*需要修改*/
  if (IS_ERR_OR_NULL(cdr->buzzer_value))
    goto buzzer_init_err;
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
                                        cdr->chardev_r_num, NULL, "mydev");
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
  buzzer_exit(cdr->buzzer_value);
buzzer_init_err:
  led_exit(cdr->led_value);
led_init_err: /*需要修改*/
  kfree(cdr);
kzalloc_err:
  return NULL;
}

void chardev_r_destroy(CHAR_DEV_R *cdr) {
  device_destroy(cdr->chardev_r_class, cdr->chardev_r_num);
  class_destroy(cdr->chardev_r_class);
  cdev_del(&cdr->chardev_r_dev);
  unregister_chrdev_region(cdr->chardev_r_num, 1);
  buzzer_exit(cdr->buzzer_value);
  led_exit(cdr->led_value); /*需要修改*/
  kfree(cdr);
}

/*----------------------------结束添加----------------------------*/
static __init int Basics_init(void) {
  /*开始添加*/
  proc_r = proc_r_create("proc_r", 0644, NULL, &fops, NULL);
  if (IS_ERR_OR_NULL(proc_r))
    goto proc_r_create_err;
  stacks = stack_create(ST_SIZE);
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

static __exit void Basics_exit(void) {
  /*开始添�����*/
  chardev_r_destroy(chardev_r);
  stack_destroy(stacks);
  proc_r_remove("proc_r", NULL);
  stack_destroy(proc_stacks_r);
  /*结束添加*/
  PERR("EXIT\n");
  return;
}

module_init(Basics_init);
module_exit(Basics_exit);
