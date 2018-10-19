/**
 * @Author: qlc
 * @Date:   2018-10-19T18:13:27+08:00
 * @Email:  qlcx@tom.com
 * @Filename: queue_r.c
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-19T19:01:18+08:00
 * @License: GPL
 */
#include "queue_r.h"

QUEUE_R *queue_create(unsigned int size) {
  QUEUE_R *queue = NULL;
  queue = kmalloc(sizeof(*queue), GFP_KERNEL);
  if (IS_ERR_OR_NULL(queue))
    goto queue_kmalloc_err;
  queue->Q_DATA = kmalloc(size, GFP_KERNEL);
  if (IS_ERR_OR_NULL(queue))
    goto Q_DATA_kmlloc_err;
  return queue;
Q_DATA_kmlloc_err:
  kfree(queue);
queue_kmalloc_err:
  return NULL;
}
void queue_destroy(QUEUE_R *queue) {
  if (IS_ERR_OR_NULL(queue))
    goto Conditions_are_not_satisfied;
  if (IS_ERR_OR_NULL(queue->Q_DATA))
    goto Conditions_are_not_satisfied;
  kfree(queue->Q_DATA);
  kfree(queue);
  return;
Conditions_are_not_satisfied:
  return;
}

int queue_en(QUEUE_R *queue, const char __user *data, size_t size) {
  int i = 0, ret = 0;
  if (IS_ERR_OR_NULL(queue) || IS_ERR_OR_NULL(data) || !size)
    goto Conditions_are_not_satisfied;
  if (IS_ERR_OR_NULL(queue->Q_DATA))
    goto Conditions_are_not_satisfied;

  /*判断队列是否为满*/
  if ((((queue->Q_D_TAILE) + 1) % (queue->Q_D_SIZE)) ==
      ((queue->Q_D_HEAD) % (queue->Q_D_SIZE)))
    goto Conditions_are_not_satisfied;

  if (size > ((queue->Q_D_SIZE) - (queue->Q_D_NUM)))
    size = ((queue->Q_D_SIZE) - (queue->Q_D_NUM));

  for (i = 0; i < size; i++) {
    ret = copy_from_user(
        &(queue->Q_DATA[((queue->Q_D_TAILE) % (queue->Q_D_SIZE))]), &(data[i]),
        1);
    if (ret)
      break;
    queue->Q_D_TAILE++;
  }
  queue->Q_D_NUM += i;
  return 0;
Conditions_are_not_satisfied:
  return 0;
}
int queue_de(QUEUE_R *queue, char __user *data, size_t size) {
  int i = 0, ret = 0;
  if (IS_ERR_OR_NULL(queue) || IS_ERR_OR_NULL(data) || !size)
    goto Conditions_are_not_satisfied;
  if (IS_ERR_OR_NULL(queue->Q_DATA))
    goto Conditions_are_not_satisfied;

  /*判断队列是否为空*/
  if (((queue->Q_D_TAILE) % (queue->Q_D_SIZE)) ==
      ((queue->Q_D_HEAD) % (queue->Q_D_SIZE)))
    goto Conditions_are_not_satisfied;

  if (size > ((queue->Q_D_SIZE) - (queue->Q_D_NUM)))
    size = ((queue->Q_D_SIZE) - (queue->Q_D_NUM));

  for (i = 0; i < size; i++) {
    ret = copy_to_user(
        &(data[i]), &(queue->Q_DATA[((queue->Q_D_HEAD) % (queue->Q_D_SIZE))]),
        1);
    if (ret)
      break;
    queue->Q_D_HEAD++;
  }
  queue->Q_D_NUM -= i;
  return 0;
Conditions_are_not_satisfied:
  return 0;
}
