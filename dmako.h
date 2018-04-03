//Demo.h
#ifndef _DEMO_H_
#define _DEMO_H_
#include <linux/ioctl.h>
#include <linux/slab.h>
#include "head.h"
//#include "myDma.h"
#include "xaxidma.h"
#include "xparameters.h"
#include "xil_cache_l.h"
/*Macros to help debuging*/
#undef PDEBUG
#ifdef DEMO_DEBUG
	#ifdef __KERNEL__
		#define PDEBUG(fmt, args...) printk(KERN_DEBUG "DEMO:" fmt,## args) 
	#else
		#define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
	#endif
#else
#define PDEBUG(fmt, args...) 
#endif

#define DEMO_MAJOR 224
#define DEMO_MINOR 0


struct demo_dev {
	struct cdev cdev;
};

void testFunc(void);
int demo_release(struct inode *inode, struct file *filp);
int demo_open(struct inode *inode, struct file *filp);
ssize_t demo_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
ssize_t demo_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);
loff_t demo_llseek(struct file *filp, loff_t off, int whence);
int demo_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
int xil_DmaInit(u16);
int CheckData(void);
int demo_mmap (struct file *filp, struct vm_area_struct *vma);
unsigned int comFunc(unsigned int hi, unsigned int mid, unsigned int lo);//1 2 3-> 0x1203
#endif
