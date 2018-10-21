# @Author: qlc
# @Date:   2018-10-20T10:52:44+08:00
# @Email:  qlcx@tom.com
# @Filename: Makefile
# @Last modified by:   qlc
# @Last modified time: 2018-10-21T14:17:15+08:00
# @License: GPL
TAGET=mydev
TM=debug
obj-m=$(TAGET).o
$(TAGET)-objs=chardev.o led.o atoi.o stack_r.o buzzer.o
KERN_PATH=/opt/UEA_10_08/s5p6818sdk_lzy1/Linux/src/linux-3.4
PWD=$(shell pwd)
CROSS_COMPILE=/opt/UEA_10_08/s5p6818sdk_lzy1/Linux/src/buildroot/output/host/usr/bin/arm-cortex_a9-linux-gnueabi-
all:
	make -C $(KERN_PATH) M=$(PWD) modules
	cp $(PWD)/$(TAGET).ko /rootfs
clean:
	make -C  $(KERN_PATH) M=$(PWD) clean
debug:
	$(CROSS_COMPILE)gcc $(TM).c -o $(TM)
	cp $(TM) /rootfs
debug_clean:
	rm -rf $(TM)
