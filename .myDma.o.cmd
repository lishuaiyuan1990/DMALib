cmd_/home/l/Desktop/mydma/myDma.o := arm-xilinx-linux-gnueabi-gcc -Wp,-MD,/home/l/Desktop/mydma/.myDma.o.d  -nostdinc -isystem /home/l/Zynq/ZynqTool/Toolchain/CodeSourcery/Sourcery_CodeBench_Lite_for_Xilinx_GNU_Linux/bin/../lib/gcc/arm-xilinx-linux-gnueabi/4.6.1/include -I/home/l/Zynq/ZynqTool/Kernel/linux-xlnx/arch/arm/include -Iarch/arm/include/generated  -Iinclude -I/home/l/Zynq/ZynqTool/Kernel/linux-xlnx/arch/arm/include/uapi -Iarch/arm/include/generated/uapi -I/home/l/Zynq/ZynqTool/Kernel/linux-xlnx/include/uapi -Iinclude/generated/uapi -include /home/l/Zynq/ZynqTool/Kernel/linux-xlnx/include/linux/kconfig.h -D__KERNEL__ -mlittle-endian -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -Os -fno-dwarf2-cfi-asm -mabi=aapcs-linux -mno-thumb-interwork -mfpu=vfp -funwind-tables -marm -D__LINUX_ARM_ARCH__=7 -march=armv7-a -msoft-float -Uarm -Wframe-larger-than=1024 -fno-stack-protector -Wno-unused-but-set-variable -fomit-frame-pointer -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -Werror=implicit-int -Werror=strict-prototypes  -DMODULE  -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(myDma)"  -D"KBUILD_MODNAME=KBUILD_STR(mydmako)" -c -o /home/l/Desktop/mydma/myDma.o /home/l/Desktop/mydma/myDma.c

source_/home/l/Desktop/mydma/myDma.o := /home/l/Desktop/mydma/myDma.c

deps_/home/l/Desktop/mydma/myDma.o := \
  /home/l/Desktop/mydma/myDma.h \
    $(wildcard include/config/pl310/errata/588369.h) \
    $(wildcard include/config/pl310/errata/727915.h) \
    $(wildcard include/config/pl310/errata/753970.h) \
  include/linux/string.h \
    $(wildcard include/config/binary/printf.h) \
  include/linux/compiler.h \
    $(wildcard include/config/sparse/rcu/pointer.h) \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
    $(wildcard include/config/kprobes.h) \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/arch/supports/optimized/inlining.h) \
    $(wildcard include/config/optimize/inlining.h) \
  include/linux/compiler-gcc4.h \
    $(wildcard include/config/arch/use/builtin/bswap.h) \
  include/linux/types.h \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/lbdaf.h) \
    $(wildcard include/config/arch/dma/addr/t/64bit.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/64bit.h) \
  include/uapi/linux/types.h \
  /home/l/Zynq/ZynqTool/Kernel/linux-xlnx/arch/arm/include/asm/types.h \
  include/asm-generic/int-ll64.h \
  include/uapi/asm-generic/int-ll64.h \
  arch/arm/include/generated/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/uapi/asm-generic/bitsperlong.h \
  /home/l/Zynq/ZynqTool/Kernel/linux-xlnx/include/uapi/linux/posix_types.h \
  include/linux/stddef.h \
  include/uapi/linux/stddef.h \
  /home/l/Zynq/ZynqTool/Kernel/linux-xlnx/arch/arm/include/uapi/asm/posix_types.h \
  /home/l/Zynq/ZynqTool/Kernel/linux-xlnx/include/uapi/asm-generic/posix_types.h \
  /home/l/Zynq/ZynqTool/Toolchain/CodeSourcery/Sourcery_CodeBench_Lite_for_Xilinx_GNU_Linux/bin/../lib/gcc/arm-xilinx-linux-gnueabi/4.6.1/include/stdarg.h \
  include/uapi/linux/string.h \
  /home/l/Zynq/ZynqTool/Kernel/linux-xlnx/arch/arm/include/asm/string.h \

/home/l/Desktop/mydma/myDma.o: $(deps_/home/l/Desktop/mydma/myDma.o)

$(deps_/home/l/Desktop/mydma/myDma.o):
