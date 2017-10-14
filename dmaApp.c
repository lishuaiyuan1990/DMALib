#include "dmaApp.h"
#include <unistd.h>
#include <math.h>
#include <memory.h>
#include <time.h>
#include <pthread.h>
#include "head.h"
#include <sys/ioctl.h>
#include <sys/select.h>
#define MAX_LINKEDNUM 10
unsigned int* gdata;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t pthread_id;
pthread_t transpthread_id[2];
struct TDPara td_para;
volatile int sys_start;
volatile int fd_accept[MAX_LINKEDNUM];
int coder;
int codermark = 0;

int gatestart = 0;
int gateend = 500;
int endsign = 0xEEEE;
volatile int mark = 1;
int main()
{
	gdata = (unsigned int*)malloc(MAX_PKT_LEN);
	struct DMAPara para = openDMA();
	int fd = para.fd;
	int* pdata[2];
	pdata[0] = para.pdata[0];
	pdata[1] = para.pdata[1];
	sockFunc(fd, pdata);
	closeDMA(fd, pdata);
	pthread_mutex_destroy(&mutex);
	return 0;
}
struct DMAPara openDMA()
{
	struct DMAPara para;
	int fd;
	int* pdata;	
	int* pdata_2;
	fd = open("/dev/dma", O_RDWR);
	if (fd == 1)
	{
		perror("open dma error\n");
		exit(-1);
	}
	printf("open dama successfully\n");
	pdata = (int*)mmap(0, MAX_PKT_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, fd, RX_BUFFER_BASE); 
	pdata_2 = (int*)mmap(0, MAX_PKT_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, fd, RX_BUFFER_BASE_2); 
	para.fd = fd;
	para.pdata[0] = pdata;
	para.pdata[1] = pdata_2;
	//ioctl(fd, CONFIG_PARA_DMA);
	return para;
}

void closeDMA(int fd, int *pdata[])
{
	munmap(pdata[0], MAX_PKT_LEN);  
	munmap(pdata[1], MAX_PKT_LEN);  
	close(fd);
}
void sockFunc(int fd, int* pdata[])
{
	int server_sock = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(SVRPORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	int bind_ret = bind(server_sock, (struct sockaddr*)&addr, sizeof(addr));
	if (bind_ret < 0)
	{
		printf("bind error, errno: %d\n", errno);
		return;
	}
	int listen_ret = listen(server_sock, 20);
	fd_set fdset;
	while (1)
	{
		int maxfd;
		FD_ZERO(&fdset);
		FD_SET(server_sock, &fdset);
		maxfd = server_sock;
		for (int i = 0; i < MAX_LINKEDNUM; i++)
		{
			if (fd_accept[i] != 0)
			{
				FD_SET(fd_accept[i], &fdset);
				if (fd_accept[i] > maxfd)
					maxfd = fd_accept[i];
			}
		}
		struct timeval timeout = {1, 0};
		int res = select(maxfd + 1, &fdset, NULL, NULL, &timeout);
		
		if (res < 0)
		{
			printf("select error!\n");
			break;
		}
		else if (res == 0)
		{
			continue;
		}
		else
		{
			for (int i = 0; i < MAX_LINKEDNUM; i++)
			{
				if (fd_accept[i] != 0 &&  FD_ISSET(fd_accept[i], &fdset))
				{
					readCtlMsg(i, fd, pdata);
					break;
				}
			}
			if (FD_ISSET(server_sock, &fdset))
			{
				
				struct sockaddr_in cnt_addr;
				bzero(&cnt_addr, sizeof(cnt_addr));
				socklen_t slen = sizeof(cnt_addr);
				int cnt_sock = accept(server_sock, (struct sockaddr*)&cnt_addr, &slen);
				if (cnt_sock < 0)
				{
					printf("accept error, errno: %d\n", errno);
					continue;
				}
				int opt = 1;
				setsockopt(cnt_sock, SOL_SOCKET, SO_KEEPALIVE,(char*) &opt, sizeof(int));
				setsockopt(cnt_sock, IPPROTO_TCP, TCP_NODELAY,(char*)&opt, sizeof(int));
				//int send_buffer = 1024 * 1024 * 100;
				//setsockopt(cnt_sock, SOL_SOCKET, SO_SNDBUF, (const char*)(&send_buffer), sizeof(int));
				printf("recv from IP(%s) PORT(%d)\n", inet_ntoa(cnt_addr.sin_addr), cnt_addr.sin_port);
				for (int i = 0; i < MAX_LINKEDNUM; i++)
				{
					if (fd_accept[i] == 0)
					{
						fd_accept[i] = cnt_sock;
						break;
					}
				}
			}
		}
	}
	printf("break while\n");
	for (int i = 0; i < MAX_LINKEDNUM; i++)
        {
  		if (fd_accept[i] != 0)
			close(fd_accept[i]);
        }
}
void readCtlMsg(int index, int fd, int *pdata[])
{
	int cnt_sock = fd_accept[index];
	int rd_buffer;
	//printf("waiting for read...(main fork)\n");
	int rd_num = read(cnt_sock, &rd_buffer, 4);
	//printf("rec %0#8x\n", rd_buffer);
	if (rd_num == 0)
	{
  		close(fd_accept[index]);
		fd_accept[index] = 0;
		printf("read EOF\n");
	}
	else if (rd_num < 0)
	{
		close(fd_accept[index]);
		fd_accept[index] = 0;
		printf("read error, errno: %d\n", errno);
	}
	else if (rd_num > 0)
	{
		controlFunc(index, rd_buffer,fd, pdata);
	}
}

void controlFunc(const int index, int sign, int fd, int *pdata[])
{
	int sock = fd_accept[index];
	if (fd_accept[index] == 0)
		return;
	int hi16 = sign >> 16;
	int hi8 = hi16 >> 8;
	int lo = sign & 0x0000FFFF; 
	if (sign == 0xABCDEF && sys_start)//linked or disconnected
	{
		//coder = ioctl(fd, ENCODERRead_MAGIC);
		//printf("coder data: %d\n", coder);
		return;
	}
	if (sign == 0xDDDD)
	{
		mark = 1;
		return;
	}
	if (sign == 97)
	{
		printf("lishuaiyuan create thread DMA\n");
		td_para.para.fd = fd;
		td_para.para.pdata[0] = pdata[0];
		td_para.para.pdata[1] = pdata[1];
		td_para.index = index;
		if (pthread_create(&pthread_id, NULL, startDMA, &td_para) !=0)
		{
			printf("Create Thread Failed\n");
			return;
		}
		return;
	}
	else if (sign == 0x78)
	{
		//transFocusLaw(sock, fd);
		return;
	}
	else if (hi16 == 3)
	{
		if (lo == 0)
		{
			int data[4];
			while(read(fd, data, 10));
			sys_start = 0;
			//ioctl(fd, CONFIG_PARA_DMA);
		}
		else if (lo == 1)
		{
			sys_start = 1;
		}
		ioctl(fd, REG_SET_PARA, sign);
		return;
	}
	else if (sign == 0X4000000)
	{
		int data[4];
		while(read(fd, data, 10));
		ioctl(fd, REG_SET_PARA, sign);
		return;
	}
	if (hi8 == INIT_CMD)
		ioctl(fd, CONFIG_PARA_DMA);
	else
		ioctl(fd, REG_SET_PARA, sign);
}
void *transData(void *args)
{
	int len = gateend - gatestart;
	int index = *((int*)args);
	fd_set set;
	//debug
	/*int pnum = MAX_PKT_LEN / 4;
	for (int i = 0; i < pnum; i++)
	{
		gdata[i] = i % 1000;
	}*/
	//printf("start trans thread\n");
	int ret_write = 0;
	/*FILE *fd = fopen("./scan_data.txt", "w+");
	for (int i = 0; i < MAX_PKT_LEN/4; i++)
	{
		fprintf(fd, "%0#10x\n", gdata[i]);
	}*/
	while (ret_write < MAX_PKT_LEN && sys_start&& fd_accept[index] != 0)
	{
		FD_ZERO(&set);
		FD_SET(fd_accept[index], &set);
		struct timeval timeout = {5, 0};
		int res = select(fd_accept[index] + 1, &set, &set, NULL, &timeout);
		if (res < 0)
		{
			printf("select error\n");
			break;
		}
		else if (res == 0)
		{
			printf("write time out\n");
			break;
		}
		if (fd_accept[index] == 0)
			break;
		mark = 0;
		ret_write = ret_write + write(fd_accept[index], gdata + ret_write / 4,  MAX_PKT_LEN - ret_write);
		if (ret_write < 0)
		{
			mark = 1;
			printf("errno: %d\n", errno);
			break;
		}
		while (!mark&& fd_accept[index] != 0);
	}
	pthread_exit(NULL);
}
void *startDMA(void *args)
{
	if (pthread_mutex_trylock(&mutex) != 0) 
		return (void*)(1);
	static int first_to_run  = 1;
	struct TDPara* td_para = (struct TDPara*)(args);
	int fd = td_para->para.fd;
	int *pdata[2];
	pdata[0] = td_para->para.pdata[0];
	pdata[1] = td_para->para.pdata[1];
	int index = td_para->index;
        int sock = fd_accept[index];
	int data[4];
	int j = 0;
	int clock = 0;
	ioctl(fd, CONFIG_PARA_DMA, 0);//0
	printf("config_para_DMA 1 %d", clock);
	while(read(fd, data, 10)); 
	while (!sys_start);
	static int first = 1;
	while(sys_start && fd_accept[index] != 0)
	{
		while (sys_start && read(fd, data, 10)&& fd_accept[index] != 0); 
		if (!sys_start)
			break;
		clock = (clock + 1) % 2;//1
		ioctl(fd, CONFIG_PARA_DMA, clock);
		printf("config_para_DMA 2 %d", clock);
		clock = (clock + 1) % 2;//0
		if (first != 1)
			pthread_join(transpthread_id[0], NULL);//waiting 0
		else
			first = 0;
		memcpy(gdata, pdata[clock], MAX_PKT_LEN);
		printf("config_para_DMA memcpy %d", clock);
		if (pthread_create(&transpthread_id[1],NULL, transData, (void*)(&index)))//create 1
		{
			printf("Create Thread Failed\n");
		}
		while (sys_start && read(fd, data, 10)&& fd_accept[index] != 0); 
		ioctl(fd, CONFIG_PARA_DMA, clock);
		printf("config_para_DMA 2 %d", clock);
		clock = (clock + 1) % 2;//1
		pthread_join(transpthread_id[1], NULL);//waiting 1
		memcpy(gdata, pdata[clock], MAX_PKT_LEN);
		printf("config_para_DMA memcpy %d", clock);
		pthread_create(&transpthread_id[0], NULL,transData, (void*)(&index));//create 0*/
		clock = (clock + 1) % 2;//0
		if (fd_accept[index] == 0)
			break;
		printf("end\n");
	}
	printf("disconnected ...\n");
	pthread_mutex_unlock(&mutex);
	printf("unlock...\n");
	pthread_exit((void*)0);
}
