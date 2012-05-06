ifneq ($(KERNELRELEASE),)

	obj-m := lska.o
	lska-objs := util.o proc_file.o kernel_file_op.o new_syscall.o hijack.o driver.o saudit.o

else

	KERNELDIR:=/lib/modules/$(shell uname -r)/build
	PWD:=$(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	rm -rf *.mod.c *.o *.order *.symvers .*.cmd .tmp_versions

endif

.PHONY:clean
clean:
	-rm *.mod.c *.o *.order *.symvers *.ko .*
