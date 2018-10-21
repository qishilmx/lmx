/**
 * @Author: qlc
 * @Date:   2018-10-20T23:32:15+08:00
 * @Email:  qlcx@tom.com
 * @Filename: x5p6818.h
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-21T12:59:30+08:00
 * @License: GPL
 */
#ifndef __S5P6818_LED_H_
#define __S5P6818_LED_H_

#include <asm/io.h>
#include <linux/proc_fs.h>

#define LED_GPIO_BASE_ADDR 0xC0010000

/*设置输入输出功能*/
#define GPIO_OUTENB_B 0xB004
/*设置输出电平*/
#define GPIO_OUT_B 0xB000
/*设置是否启用开漏输出或推挽输出*/
#define GPIO_PULLENB_B 0xB060
/*设置是否启用GPIOB_PULLENB的设置*/
#define GPIO_PULLENB_DISABLE_DEFAULT_B 0xB064

#define GPIO_OUTENB_C 0xC004
#define GPIO_OUT_C 0xC000
#define GPIO_PULLENB_C 0xC060
#define GPIO_PULLENB_DISABLE_DEFAULT_C 0xC064

typedef struct {
  uint8_t on_off;
  uint8_t led_num;
  uint8_t status;
} LED_STATUS;

void __iomem *led_init(void);
void led_exit(void __iomem *addr);

void led_on(void __iomem *addr, unsigned int led_num);
void led_off(void __iomem *addr, unsigned int led_num);

void led_set_status(void __iomem *addr, unsigned int led_num, int status);
void led_get_status(void __iomem *addr, uint8_t *status);

#endif
