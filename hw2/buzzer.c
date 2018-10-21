/**
 * @Author: qlc
 * @Date:   2018-10-21T01:16:27+08:00
 * @Email:  qlcx@tom.com
 * @Filename: buzzer.c
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-21T14:28:18+08:00
 * @License: GPL
 */
#include "buzzer.h"

void __iomem *buzzer_init(void) {
  unsigned int reg;
  void __iomem *buzzer_v = NULL;
  buzzer_v = ioremap(GPIO_BASE_ADDR_C_BUZZER, SZ_4K);
  if (IS_ERR_OR_NULL(buzzer_v))
    goto ioremap_gpio_c_err;

  reg = ioread32(buzzer_v + GPIO_PULLENB_DISABLE_DEFAULT_C_BUZZER);
  reg |= (1 << 14);
  iowrite32(reg, buzzer_v + GPIO_PULLENB_DISABLE_DEFAULT_C_BUZZER);
  reg = ioread32(buzzer_v + GPIO_PULLENB_C_BUZZER);
  reg &= ~((1 << 14));
  iowrite32(reg, buzzer_v + GPIO_PULLENB_C_BUZZER);
  reg = ioread32(buzzer_v + GPIO_OUTENB_C_BUZZER);
  reg |= (1 << 14);
  iowrite32(reg, buzzer_v + GPIO_OUTENB_C_BUZZER);
  reg = ioread32(buzzer_v + GPIO_OUT_C_BUZZER);
  reg &= ~(1 << 14);
  iowrite32(reg, buzzer_v + GPIO_OUT_C_BUZZER);

  return buzzer_v;
ioremap_gpio_c_err:
  return NULL;
}
void buzzer_exit(void __iomem *addr) {
  unsigned int reg = 0;
  reg = ioread32(addr + GPIO_OUT_C_BUZZER);
  reg &= ~(1 << 14);
  iowrite32(reg, addr + GPIO_OUT_C_BUZZER);
  iounmap(addr);
}

void buzzer_on(void __iomem *addr) {
  unsigned int reg = 0;
  reg = ioread32(addr + GPIO_OUT_C_BUZZER);
  reg |= (1 << 14);
  iowrite32(reg, addr + GPIO_OUT_C_BUZZER);
}
void buzzer_off(void __iomem *addr) {
  unsigned int reg = 0;
  reg = ioread32(addr + GPIO_OUT_C_BUZZER);
  reg &= ~(1 << 14);
  iowrite32(reg, addr + GPIO_OUT_C_BUZZER);
}

uint8_t buzzer_on_or_off(void __iomem *addr) {
  unsigned int reg = 0;
  reg = ioread32(addr + GPIO_OUT_C_BUZZER);
  return ((reg >> 14) & 1);
}
