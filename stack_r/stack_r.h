/**
 * @Author: qlc
 * @Date:   2018-10-19T17:26:39+08:00
 * @Email:  qlcx@tom.com
 * @Filename: stack_r.h
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-19T18:09:40+08:00
 * @License: GPL
 */
#ifndef __STACK_R_H_
#define __STACK_R_H_
#include <asm/uaccess.h>
#include <linux/proc_fs.h>

typedef struct {
  char *S_DATA;
  unsigned int S_D_TOP;
  unsigned int S_D_NUM;
  unsigned int S_D_SIZE;
} STACK_R;

STACK_R *stack_create(unsigned int size);
void stack_destroy(STACK_R *stack);

int stack_push(STACK_R *stack, const char __user *data, size_t size);
int stack_pop(STACK_R *stack, char __user *data, size_t size);
#endif
