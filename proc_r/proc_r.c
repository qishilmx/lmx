/**
 * @Author: qlc
 * @Date:   2018-10-19T09:20:39+08:00
 * @Email:  qlcx@tom.com
 * @Filename: proc_r.c
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-19T11:38:22+08:00
 * @License: GPL
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>

#define PERR(fmt, args...)                                                     \
  do {                                                                         \
    printk(KERN_ERR "[%s:%d]" fmt, __FUNCTION__, __LINE__, ##args);            \
  } while (0)

MODULE_LICENSE("GPL");
/*----------------------------开始添加----------------------------*/
/*proc操作函数*/
int open_r(struct inode *inode, struct file *file) { return 0; }
int release_r(struct inode *inode, struct file *file) { return 0; }
ssize_t read_r(struct file *file, char __user *ubuf, size_t size, loff_t *pos) {
  return 0;
}
ssize_t write_r(struct file *file, const char __user *ubuf, size_t size,
                loff_t *pos) {
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
  proc_r = proc_create_data(name, mode, parent, proc_fops, data);
  if (IS_ERR_OR_NULL(proc_r))
    goto proc_create_data_err;
  return proc_r;
proc_create_data_err:
  return NULL;
}
void proc_r_remove(const char *name, struct proc_dir_entry *parent) {
  remove_proc_entry(name, parent);
}
/*----------------------------结束添加----------------------------*/
static __init int proc_r_init(void) {
  /*开始添加*/
  proc_r = proc_r_create("proc_r", 0644, NULL, &fops, NULL);
  if (IS_ERR_OR_NULL(proc_r))
    goto proc_r_create_err;
  /*暂停添加*/
  PERR("INIT\n");
  return 0;
/*继续添加*/
proc_r_create_err:
  return -ENOMEM;
  /*结束添加*/
}

static __exit void proc_r_exit(void) {
  /*开始添加*/
  proc_r_remove("proc_r", NULL);
  /*结束添加*/
  PERR("EXIT\n");
  return;
}

module_init(proc_r_init);
module_exit(proc_r_exit);
