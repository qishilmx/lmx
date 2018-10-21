/**
 * @Author: qlc
 * @Date:   2018-10-20T23:32:06+08:00
 * @Email:  qlcx@tom.com
 * @Filename: x5p6818.c
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-21T13:21:21+08:00
 * @License: GPL
 */
#include "led.h"

void __iomem *led_init(void) {
  unsigned int reg = 0;
  void __iomem *led_v = NULL;
  led_v = ioremap(LED_GPIO_BASE_ADDR, SZ_64K);
  if (IS_ERR_OR_NULL(led_v))
    goto ioremap_err;

  reg = ioread32(led_v + GPIO_PULLENB_DISABLE_DEFAULT_B);
  reg |= (1 << 26);
  iowrite32(reg, led_v + GPIO_PULLENB_DISABLE_DEFAULT_B);
  reg = ioread32(led_v + GPIO_PULLENB_B);
  reg &= ~(1 << 26);
  iowrite32(reg, led_v + GPIO_PULLENB_B);
  reg = ioread32(led_v + GPIO_OUTENB_B);
  reg |= (1 << 26);
  iowrite32(reg, led_v + GPIO_OUTENB_B);
  reg = ioread32(led_v + GPIO_OUT_B);
  reg |= (1 << 26);
  iowrite32(reg, led_v + GPIO_OUT_B);

  reg = ioread32(led_v + GPIO_PULLENB_DISABLE_DEFAULT_C);
  reg |= (1 << 11) | (1 << 7) | (1 << 12);
  iowrite32(reg, led_v + GPIO_PULLENB_DISABLE_DEFAULT_C);
  reg = ioread32(led_v + GPIO_PULLENB_C);
  reg &= ~((1 << 11) | (1 << 7) | (1 << 12));
  iowrite32(reg, led_v + GPIO_PULLENB_C);
  reg = ioread32(led_v + GPIO_OUTENB_C);
  reg |= (1 << 11) | (1 << 7) | (1 << 12);
  iowrite32(reg, led_v + GPIO_OUTENB_C);
  reg = ioread32(led_v + GPIO_OUT_C);
  reg |= (1 << 11) | (1 << 7) | (1 << 12);
  iowrite32(reg, led_v + GPIO_OUT_C);

  return led_v;
ioremap_err:
  return NULL;
}
void led_exit(void __iomem *addr) {
  unsigned reg = 0;
  reg = ioread32(addr + GPIO_OUT_B);
  reg |= (1 << 26);
  iowrite32(reg, addr + GPIO_OUT_B);

  reg = ioread32(addr + GPIO_OUT_C);
  reg |= (1 << 11) | (1 << 7) | (1 << 12);
  iowrite32(reg, addr + GPIO_OUT_C);

  iounmap(addr);
}

void led_on(void __iomem *addr, unsigned int led_num) {
  led_set_status(addr, led_num, 1);
}
void led_off(void __iomem *addr, unsigned int led_num) {
  led_set_status(addr, led_num, 0);
}

void led_set_status(void __iomem *addr, unsigned int led_num, int status) {
  unsigned int reg = 0;

  switch (led_num) {
  case 0:
    reg = ioread32(addr + GPIO_OUT_B);
    if (status)
      reg &= ~(1 << 26);
    else
      reg |= (1 << 26);
    iowrite32(reg, addr + GPIO_OUT_B);
    break;
  case 1:
    reg = ioread32(addr + GPIO_OUT_C);
    if (status)
      reg &= ~(1 << 11);
    else
      reg |= (1 << 11);
    iowrite32(reg, addr + GPIO_OUT_C);
    break;
  case 2:
    reg = ioread32(addr + GPIO_OUT_C);
    if (status)
      reg &= ~(1 << 7);
    else
      reg |= (1 << 7);
    iowrite32(reg, addr + GPIO_OUT_C);
    break;
  case 3:
    reg = ioread32(addr + GPIO_OUT_C);
    if (status)
      reg &= ~(1 << 12);
    else
      reg |= (1 << 12);
    iowrite32(reg, addr + GPIO_OUT_C);
    break;
  }
}
void led_get_status(void __iomem *addr, uint8_t *status) {
  unsigned int reg = 0;
  (*status) = 0;

  reg = ioread32(addr + GPIO_OUT_B);
  if (!(reg & (1 << 26)))
    (*status) |= (1 << 0);
  reg = ioread32(addr + GPIO_OUT_C);
  if (!(reg & (1 << 11)))
    (*status) |= (1 << 1);
  reg = ioread32(addr + GPIO_OUT_C);
  if (!(reg & (1 << 7)))
    (*status) |= (1 << 2);
  reg = ioread32(addr + GPIO_OUT_C);
  if (!(reg & (1 << 12)))
    (*status) |= (1 << 3);
}
