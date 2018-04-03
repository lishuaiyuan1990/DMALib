#include <linux/tcp.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <linux/rtc.h>
#include <linux/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <strings.h>
#define STOP_DMA 0
#define AD_LENGTH 2000
#define START_DMA 1
#define NUMBER_OF_TRANSFERS	1
#define XPAR_PS7_DDR_0_S_AXI_BASEADDR 0x00100000
#define RX_BUFFER_BASE	(XPAR_PS7_DDR_0_S_AXI_BASEADDR + 0x01000000)
#define XPAR_PS7_DDR_0_S_AXI_BASEADDR_2 0x00500000
#define RX_BUFFER_BASE_2	(XPAR_PS7_DDR_0_S_AXI_BASEADDR_2 + 0x01000000)
#define SVRPORT 6869
struct DMAPara
{
	int fd;
	int* pdata[2];
};
struct TDPara
{
	int index;
	struct DMAPara para;
};
struct TransPara
{
	struct TDPara td;
	int clock;
};
void controlFunc(const int sock, int sign, int fd, int *pdata[]); 
void sockFunc(int fd, int *pdata[]);
struct DMAPara openDMA();
void *startDMA(void *args);
void *transData(void *args);
void closeDMA(int fd, int *pdata[]);
void transFocusLaw(int sock, int fd);
void readCtlMsg(int sock, int fd, int *pdata[]);
