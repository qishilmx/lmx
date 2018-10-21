/**
 * @Author: qlc
 * @Date:   2018-10-21T01:16:41+08:00
 * @Email:  qlcx@tom.com
 * @Filename: buzzer.h
 * @Last modified by:   qlc
 * @Last modified time: 2018-10-21T14:26:50+08:00
 * @License: GPL
 */
#ifndef __S5P6818_BUZZER_H_
#define __S5P6818_BUZZER_H_

#include <asm/io.h>
#include <linux/proc_fs.h>

#define GPIO_BASE_ADDR_C_BUZZER 0xC001C000
#define GPIO_OUTENB_C_BUZZER 0x04
#define GPIO_OUT_C_BUZZER 0x00
#define GPIO_PULLENB_C_BUZZER 0x60
#define GPIO_PULLENB_DISABLE_DEFAULT_C_BUZZER 0x64

void __iomem *buzzer_init(void);
void buzzer_exit(void __iomem *addr);

void buzzer_on(void __iomem *addr);
void buzzer_off(void __iomem *addr);

uint8_t buzzer_on_or_off(void __iomem *addr);

#endif
