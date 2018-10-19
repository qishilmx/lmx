/**
 * @Author: qlc
 * @Date:   2018-10-19T17:26:24+08:00
 * @Email:  qlcx@tom.com
 * @Filename: stack_r.c
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-19T18:11:16+08:00
 * @License: GPL
 */
#include "stack_r.h"

STACK_R *stack_create(unsigned int size) {
  STACK_R *stack = NULL;
  stack = kmalloc(sizeof(*stack), GFP_KERNEL);
  if (IS_ERR_OR_NULL(stack))
    goto stack_kmalloc_err;
  stack->S_DATA = kmalloc(size, GFP_KERNEL);
  if (IS_ERR_OR_NULL(stack->S_DATA))
    goto S_DATA_kmalloc_err;
  stack->S_D_SIZE = size;
  return stack;
S_DATA_kmalloc_err:
  kfree(stack);
stack_kmalloc_err:
  return NULL;
}

void stack_destroy(STACK_R *stack) {
  if (IS_ERR_OR_NULL(stack))
    goto stack_is_null;
  if (IS_ERR_OR_NULL(stack->S_DATA))
    goto S_DATA_is_null;
  kfree(stack->S_DATA);
  kfree(stack);
  return;
S_DATA_is_null:
  kfree(stack);
stack_is_null:
  return;
}

int stack_push(STACK_R *stack, const char __user *data, size_t size) {
  int i = 0, ret = 0;
  if (IS_ERR_OR_NULL(stack) || IS_ERR_OR_NULL(data) || !size)
    goto Conditions_are_not_satisfied;
  if (IS_ERR_OR_NULL(stack->S_DATA))
    goto Conditions_are_not_satisfied;

  if (size > ((stack->S_D_SIZE) - (stack->S_D_NUM)))
    size = ((stack->S_D_SIZE) - (stack->S_D_NUM));

  for (i = 0; i < size; i++) {
    ret = copy_from_user(&(stack->S_DATA[(stack->S_D_TOP) + i]), &(data[i]), 1);
    if (ret)
      break;
  }
  stack->S_D_TOP += i;
  stack->S_D_NUM += i;

  return i;
Conditions_are_not_satisfied:
  return 0;
}

int stack_pop(STACK_R *stack, char __user *data, size_t size) {
  int i = 0, ret = 0;
  if (IS_ERR_OR_NULL(stack) || IS_ERR_OR_NULL(data) || !size)
    goto Conditions_are_not_satisfied;
  if (IS_ERR_OR_NULL(stack->S_DATA))
    goto Conditions_are_not_satisfied;

  if (size > (stack->S_D_NUM))
    size = (stack->S_D_NUM);

  for (i = 0; i < size; i++) {
    ret =
        copy_to_user(&(data[i]), &(stack->S_DATA[(stack->S_D_TOP) - 1 - i]), 1);
    if (ret)
      break;
  }
  stack->S_D_TOP -= i;
  stack->S_D_NUM -= i;

  return i;
Conditions_are_not_satisfied:
  return 0;
}
