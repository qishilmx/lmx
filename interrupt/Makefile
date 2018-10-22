# @Author: qlc
# @Date:   2018-10-19T16:05:46+08:00
# @Email:  qlcx@tom.com
# @Filename: Makefile
# @Last modified by:   qlc
# @Last modified time: 2018-10-19T16:10:57+08:00
# @License: GPL
TAGET=interrupt
obj-m=$(TAGET).o
KERN_PATH=/opt/UEA_10_08/s5p6818sdk_lzy1/Linux/src/linux-3.4
PWD=$(shell pwd)
all:
	make -C $(KERN_PATH) M=$(PWD) modules
	cp $(PWD)/$(TAGET).ko /rootfs
clean:
	make -C  $(KERN_PATH) M=$(PWD) clean
