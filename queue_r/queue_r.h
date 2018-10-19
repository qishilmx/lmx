/**
 * @Author: qlc
 * @Date:   2018-10-19T18:13:37+08:00
 * @Email:  qlcx@tom.com
 * @Filename: queue_r.h
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-19T18:43:43+08:00
 * @License: GPL
 */
#ifndef __QUEUE_R_H_
#define __QUEUE_R_H_

#include <asm/uaccess.h>
#include <linux/proc_fs.h>

typedef struct {
  char *Q_DATA;
  unsigned int Q_D_HEAD;
  unsigned int Q_D_TAILE;
  unsigned int Q_D_NUM;
  unsigned int Q_D_SIZE;
} QUEUE_R;

QUEUE_R *queue_create(unsigned int size);
void queue_destroy(QUEUE_R *queue);

int queue_en(QUEUE_R *queue, const char __user *data, size_t size);
int queue_de(QUEUE_R *queue, char __user *data, size_t size);

#endif
