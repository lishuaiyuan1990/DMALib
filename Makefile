ifeq ($(KERNELRELEASE),)   
KERNELDIR ?= ~/Desktop/Zynq/ZynqTool/Kernel/linux-xlnx
PWD := $(shell pwd) 
CROSS_COMPILER := arm-xilinx-linux-gnueabi-
modules:
		$(MAKE) -C $(KERNELDIR) ARCH=arm CROSS_COMPILE=arm-xilinx-linux-gnueabi- M=$(PWD) modules
modules_install:
		$(MAKE) -C $(KERNELDIR) ARCH=arm CROSS_COMPILE=arm-xilinx-linux-gnueabi- M=$(PWD) modules_install
clean:
		rm -rRf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions modules.order Module.symvers

.PHONY: modules modules_install clean

else  
#MODULE_NAME := pns610
#PNS610_OBJS := driver.o dma.o data.o goose.o fpga.o ft3.o state.o
#$(MODULE_NAME)-objs:= $(PNS610_OBJS)
#obj-m := pns610.o

MODULE_NAME := mydmako
DMA_OBJS := myDma.o dmako.o
$(MODULE_NAME)-objs:= $(DMA_OBJS)
obj-m := mydmako.o

endif

