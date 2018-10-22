/**
 * @Author: qlc
 * @Date:   2018-10-19T16:10:45+08:00
 * @Email:  qlcx@tom.com
 * @Filename: interrupt.c
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-19T16:24:23+08:00
 * @License: GPL
 */
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <mach/platform.h>

#define PERR(fmt, args...)                                                     \
  do {                                                                         \
    printk(KERN_ERR "[%s:%d]" fmt, __FUNCTION__, __LINE__, ##args);            \
  } while (0)

MODULE_LICENSE("GPL");
/*----------------------------开始添加----------------------------*/
irqreturn_t key_1_irq_handler(int irq, void *dev) {

  PERR("key_1:irq=%d\n", irq);

  /*执行成功*/
  return IRQ_HANDLED;
  /*
   执行失败返回
   return IRQ_NONE
   */
}
irqreturn_t key_2_irq_handler(int irq, void *dev) {
  PERR("key_2:irq=%d\n", irq);
  return IRQ_HANDLED;
}
irqreturn_t key_3_irq_handler(int irq, void *dev) {
  PERR("key_3:irq=%d\n", irq);
  return IRQ_HANDLED;
}
irqreturn_t key_4_irq_handler(int irq, void *dev) {
  PERR("key_4:irq=%d\n", irq);
  return IRQ_HANDLED;
}
/*----------------------------结束添加----------------------------*/
static __init int interrupt_init(void) {
  /*开始添加*/
  int ret = 0;
  ret = request_irq(gpio_to_irq(PAD_GPIO_A + 28), key_1_irq_handler,
                    IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "key_1", NULL);
  if (ret < 0)
    goto key_1_request_irq_err;
  ret = request_irq(gpio_to_irq(PAD_GPIO_B + 30), key_2_irq_handler,
                    IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "key_2", NULL);
  if (ret < 0)
    goto key_2_request_irq_err;
  ret = request_irq(gpio_to_irq(PAD_GPIO_B + 31), key_3_irq_handler,
                    IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "key_3", NULL);
  if (ret < 0)
    goto key_3_request_irq_err;
  ret = request_irq(gpio_to_irq(PAD_GPIO_B + 9), key_4_irq_handler,
                    IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "key_4", NULL);
  if (ret < 0)
    goto key_4_request_irq_err;
  /*暂停添加*/
  PERR("INIT\n");
  return 0;
/*继续添加*/
key_4_request_irq_err:
  free_irq(IRQ_GPIO_B_START + 31, NULL);
key_3_request_irq_err:
  free_irq(IRQ_GPIO_B_START + 30, NULL);
key_2_request_irq_err:
  free_irq(IRQ_GPIO_A_START + 28, NULL);
key_1_request_irq_err:
  return ret;
  /*结束添加*/
}

static __exit void interrupt_exit(void) {
  /*开始添加*/
  free_irq(IRQ_GPIO_B_START + 9, NULL);
  free_irq(IRQ_GPIO_B_START + 31, NULL);
  free_irq(IRQ_GPIO_B_START + 30, NULL);
  free_irq(IRQ_GPIO_A_START + 28, NULL);
  /*结束添加*/
  PERR("EXIT\n");
  return;
}

module_init(interrupt_init);
module_exit(interrupt_exit);
