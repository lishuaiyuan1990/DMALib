//demo.c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/cdev.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/version.h>
#include <linux/vmalloc.h>
#include <linux/ctype.h>
#include <linux/pagemap.h>
#include <linux/unistd.h> 
#include <linux/mman.h>
#include <linux/mm_types.h>  
#include <linux/mm.h> 
#include <linux/fcntl.h> 
#include <linux/mman.h>

#include <linux/time.h>  

#include <linux/stat.h>
#include "dmako.h"

#define TEST_PARA_POS   	0x0C
#define READ_POS   		0x08
#define XPAR_PS7_DDR_0_S_AXI_BASEADDR   0x00100000
#define XAXIDMA_RX_OFFSET	0x00000030 /**< RX channel registers base*/
#define XPAR_PS7_DDR_0_S_AXI_BASEADDR_2 0x00500000
#define RX_BUFFER_BASE		(XPAR_PS7_DDR_0_S_AXI_BASEADDR + 0x01000000)
#define RX_BUFFER_BASE_2	(XPAR_PS7_DDR_0_S_AXI_BASEADDR_2 + 0x01000000)
#define XPAR_AXIDMA_0_DEVICE_ID XPAR_AXI_DMA_0_DEVICE_ID
#define DMA_DEV_ID		XPAR_AXIDMA_0_DEVICE_ID
#define START_DMA 1
#define STOP_DMA 0
#define REG_BASE_PADDR 0x43C00000
MODULE_AUTHOR("Li");
MODULE_LICENSE("GPL");


struct  timeval g_start;
struct  timeval g_end;
struct  timeval g_start_0;

struct demo_dev *demo_devices;
static unsigned char demo_inc = 0;//全局变量，每次只能打开一个设备
u32 XPAR_AXI_DMA_0_VBASEADDR;
u32 VXPS_L2CC_BASEADDR;
u32 VREG_BASE_ADDR;
XAxiDma AxiDma;

int demo_open(struct inode *inode, struct file *filp)
{
	struct demo_dev *dev;
	
	if (demo_inc > 0) return -ERESTARTSYS;
	demo_inc++;
	dev = container_of(inode->i_cdev, struct demo_dev, cdev);
 	filp->private_data = dev;
    	return 0;
}

int demo_release(struct inode *inode, struct file *filp)
{	
	demo_inc--;
	return 0;
}

ssize_t demo_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	//XAxiDma_WriteReg(VREG_BASE_ADDR, 0, 1);
	//while (XAxiDma_Busy(&AxiDma,XAXIDMA_DEVICE_TO_DMA));
	int ret = XAxiDma_Busy(&AxiDma,XAXIDMA_DEVICE_TO_DMA);
	//unsigned long arg = 0;
	//unsigned int para = (3 << 16) + arg;
	//XAxiDma_WriteReg(VREG_BASE_ADDR, TEST_PARA_POS, para);
	//printk("dmako...... read XAxiDma_Busy %d\n", ret);
	if (ret == 0)
	{
		do_gettimeofday(&g_end);
		unsigned long diff;
		diff = 1000000 * (g_end.tv_sec-g_start.tv_sec)+ g_end.tv_usec-g_start.tv_usec;
        printk("dmako not busy from configSimple time(us) %ld\n",diff);
		// XAxiDma_WriteReg(VREG_BASE_ADDR, 0, 0);
	}
	return ret;
}

ssize_t  demo_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	return 0;
}

int  demo_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	unsigned int para;
	int Status;
	static int first_time_run = 1;
	printk("dmako......ioctl cmd: %0#8x arg: %0#8x \n", cmd, arg);
	u16 data = cmd & 0x0000FFFF;
	u8 nr = data & 0x00FF;
	u8 nr_lo = nr & 0x0F;
	u8 nr_hi = (nr & 0xF0) >> 4;
	u8 magic = (data & 0xFF00) >> 8; 
	if (cmd == CONFIG_PARA_DMA)
	{		
		do_gettimeofday(&g_start_0);
		u8 *RxBufferPtr;
		//printk("dmako xxxxxxxxxxxxxxxxxxxxxx ioctl CONFIG_PARA_DMA : %d\n", arg);
		if (arg == 2)
		{
			printk("dmako configDMA XAxiDma_Reset...");
			XAxiDma_Reset(&AxiDma);
			/* At the initialization time, hardware should finish reset quickly
			 */
			int TimeOut = 500;
			while (TimeOut) {
				if(XAxiDma_ResetIsDone(&AxiDma)) {
					break;
				}
				TimeOut -= 1;
			}
			if (!TimeOut)
				return XST_FAILURE;
			printk("dmako configDMA XAxiDma_Reset Success...");
			arg = 0;
		}
		arg == 0 ? (RxBufferPtr = (u8 *)RX_BUFFER_BASE) : (RxBufferPtr = (u8 *)RX_BUFFER_BASE_2);
		Status = XAxiDma_SimpleTransfer(&AxiDma,(u32) RxBufferPtr, MAX_PKT_LEN, XAXIDMA_DEVICE_TO_DMA,VREG_BASE_ADDR);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}
		/* Invalidate the DestBuffer before receiving the data, in case the
		 * Data Cache is enabled
		 */
		u32 *RxPacket;
		if(arg == 0)
			RxPacket = (u32 *) RX_BUFFER_BASE;
		else
			RxPacket = (u32 *) RX_BUFFER_BASE_2;
		//XAxiDma_WriteReg(VXPS_L2CC_BASEADDR, XPS_L2CC_CACHE_INVLD_PA_OFFSET, (u32)RxPacket);
		//Xil_L2CacheInvalidateLine(VXPS_L2CC_BASEADDR);
		//Xil_DCacheInvalidateRange((u32)RxPacket, MAX_PKT_LEN / 100, VXPS_L2CC_BASEADDR);
		XAxiDma_WriteReg(VREG_BASE_ADDR, 0, 1);
		XAxiDma_WriteReg(VREG_BASE_ADDR, 0, 0);
		do_gettimeofday(&g_start);
		unsigned long diff;
		diff = 1000000 * (g_start.tv_sec-g_start_0.tv_sec)+ g_start.tv_usec-g_start_0.tv_usec;
        printk("dmako just configDMA time (T3/us) %ld\n",diff);
		return XST_SUCCESS;
	}
	else if (cmd == REG_SET_PARA)
	{
		XAxiDma_WriteReg(VREG_BASE_ADDR, TEST_PARA_POS, arg);
		return 0;
	}
	printk("ioctl error\n");
	return -EFAULT;
}
unsigned int comFunc(unsigned int hi, unsigned int mid, unsigned int lo)//1 2 3-> 0x1203
{
	unsigned int si = (hi << 12) + (mid << 8) + lo;
	return si;
}
loff_t demo_llseek(struct file *filp, loff_t off, int whence)
{
	loff_t pos;
	
	pos = filp->f_pos;
	switch (whence) {
	case 0:
		pos = off;
		break;
	case 1:
		pos += off;
		break;
	case 2:
	default:
		return -EINVAL; 
	}
	
	if ((pos > 256) || (pos < 0))
		return -EINVAL;
	
	return filp->f_pos = pos;
}
int demo_mmap (struct file *filp, struct vm_area_struct *vma)  
{  
  int err = 0;  
  unsigned long start = vma->vm_start;  
  unsigned long size = vma->vm_end - vma->vm_start;  
  err = remap_pfn_range (vma, start, vma->vm_pgoff, size, vma->vm_page_prot);  
  return err;  
}  
struct file_operations demo_fops = {
	.owner = THIS_MODULE,
	.llseek = demo_llseek,
	.read = demo_read,
	.write = demo_write,
	.unlocked_ioctl = demo_ioctl,
	.open = demo_open,
	.release = demo_release,
	.mmap = demo_mmap, 
};

void demo_cleanup_module(void)
{
	dev_t devno = MKDEV(DEMO_MAJOR, DEMO_MINOR);
	
	if (demo_devices) {
		cdev_del(&demo_devices->cdev);
		kfree(demo_devices);
	}
	unregister_chrdev_region(devno, 1);
}

int xil_DmaInit(u16 DeviceId)
{
	XAxiDma_Config *CfgPtr;
	int Status;

	/* Initialize the XAxiDma device.
	 */
	CfgPtr = XAxiDma_LookupConfig(DeviceId);
	if (!CfgPtr) {
		return XST_FAILURE;
	}

	Status = XAxiDma_CfgInitialize(&AxiDma, CfgPtr, XPAR_AXI_DMA_0_VBASEADDR);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	if(XAxiDma_HasSg(&AxiDma)){
		//xil_printf("Device configured as SG mode \r\n");
		return XST_FAILURE;
	}
	XAxiDma_IntrDisable(&AxiDma, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DEVICE_TO_DMA);
	Xil_L2CacheDisable(VXPS_L2CC_BASEADDR);
	return XST_SUCCESS;
}
int CheckData(void)
{
	u32 *RxPacket;
	int Index = 0;

	RxPacket = (u32 *) RX_BUFFER_BASE;

	/* Invalidate the DestBuffer before receiving the data, in case the
	 * Data Cache is enabled
	 */
	Xil_DCacheInvalidateRange((u32)RxPacket, MAX_PKT_LEN, VXPS_L2CC_BASEADDR);

	printk("Data received: ");
	for(Index = 0; Index < 8; Index++) {
		printk("0x%X ", (unsigned int)RxPacket[Index]);
	}
	printk("\r\n");
	return XST_SUCCESS;
}
//Init module流程：
//1）注册设备号MKDEV；
//2）注册设备驱动程序,即初始化cdev结构（嵌入到demo_devices结构中）
int demo_init_module(void)
{
	int result;
	dev_t dev = 0;
	ssleep(3);
	dev = MKDEV(DEMO_MAJOR, DEMO_MINOR);
	result = register_chrdev_region(dev, 1, "dma");
	if (result < 0) {
		printk(KERN_WARNING "dma: can't get major %d\n", DEMO_MAJOR);
		return result;
	}
	demo_devices = kmalloc(sizeof(struct demo_dev), GFP_KERNEL);
	if (!demo_devices) {
		result = -ENOMEM;
		goto fail;
	}
	VREG_BASE_ADDR = ioremap(REG_BASE_PADDR, 0xFFFF);
	XPAR_AXI_DMA_0_VBASEADDR = ioremap(XPAR_AXI_DMA_0_BASEADDR, 0xFFFF);
	VXPS_L2CC_BASEADDR = ioremap(XPS_L2CC_BASEADDR, 0xFFFF);
	printk("REG_BASE_PADDR: %x\n", REG_BASE_PADDR);
	printk("VREG_BASE_ADDR: %x\n", VREG_BASE_ADDR);
	printk("VXPS_L2CC_BASEADDR: %x\n", VXPS_L2CC_BASEADDR);
	printk("XPS_L2CC_BASEADDR: %x\r\n", XPS_L2CC_BASEADDR);
	printk("XPAR_AXI_DMA_0_VBASEADDR: %x\n", XPAR_AXI_DMA_0_VBASEADDR);
	printk("XPAR_AXI_DMA_0_BASEADDR: %x\r\n", XPAR_AXI_DMA_0_BASEADDR);

	memset(demo_devices, 0, sizeof(struct demo_dev));
	cdev_init(&demo_devices->cdev, &demo_fops);	
	demo_devices->cdev.owner = THIS_MODULE;
	demo_devices->cdev.ops = &demo_fops; //将创建的字符设备与file_operations中各函数操作连接起来
	result = cdev_add(&demo_devices->cdev, dev, 1);
	
	
	if (result) {
		printk(KERN_NOTICE "error %d adding demo\n", result);
		goto fail;
	}
	xil_DmaInit(DMA_DEV_ID);
	return 0;
fail:
	demo_cleanup_module();
	return result;
}

module_init(demo_init_module);
module_exit(demo_cleanup_module);

