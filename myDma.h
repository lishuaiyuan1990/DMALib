#ifndef _MYDMA_H_
#define _MYDMA_H_
//#include "dmatypes.h"
#include <linux/string.h>
//#include <stdint.h>
#define XPAR_AXI_DMA_0_DEVICE_ID 0
#define XPAR_AXI_DMA_0_BASEADDR 0x40400000//DMA Control
//#define XPAR_AXI_DMA_0_HIGHADDR 0x4040FFFF
#define XPS_L2CC_CNTRL_OFFSET		0x0100U
#define XPAR_AXI_DMA_0_SG_INCLUDE_STSCNTRL_STRM 0
#define XPAR_AXI_DMA_0_INCLUDE_MM2S_DRE 0
#define XPAR_AXI_DMA_0_INCLUDE_S2MM_DRE 1
#define XPAR_AXI_DMA_0_INCLUDE_MM2S 0
#define XPAR_AXI_DMA_0_INCLUDE_S2MM 1
#define XPAR_AXI_DMA_0_M_AXI_MM2S_DATA_WIDTH 32
#define XPAR_AXI_DMA_0_M_AXI_S2MM_DATA_WIDTH 32
#define XPAR_AXI_DMA_0_INCLUDE_SG 0
#define XPAR_AXI_DMA_0_ENABLE_MULTI_CHANNEL 0
#define XPAR_AXI_DMA_0_NUM_MM2S_CHANNELS 1
#define XPAR_AXI_DMA_0_NUM_S2MM_CHANNELS 1
#define XPAR_AXI_DMA_0_M_AXIS_MM2S_TDATA_WIDTH 64
#define XPAR_AXI_DMA_0_S_AXIS_S2MM_TDATA_WIDTH 64
#define XPS_L2CC_CACHE_INV_CLN_WAY_OFFSET	0x07FCU
#define XAXIDMA_MICROMODE_MIN_BUF_ALIGN	0xFFF	/**< Minimum byte alignment*/
#define XAXIDMA_CR_OFFSET	0x00000000   /**< Channel control */
#define XAXIDMA_CR_RESET_MASK	0x00000004 /**< Reset DMA engine */
#define XAXIDMA_IDLE_MASK		0x00000002  /**< DMA channel idle */
#define XAXIDMA_IRQ_ALL_MASK		0x00007000 /**< All interrupts */
#define XAXIDMA_SRCADDR_OFFSET	0x00000018   /**< Simple mode source address*/
#define XPAR_XAXIDMA_NUM_INSTANCES 1
#define XST_DMA_ERROR                   9L	/* a DMA error occurred, this error*/
#define XST_SUCCESS                     0L
#define XST_FAILURE                     1L
#define XST_INVALID_PARAM               15L	/* an invalid parameter was passed*/
#define XAXIDMA_BD_NUM_WORDS		16  /**< Total number of words for*/
#define XAXIDMA_MAX_TRANSFER_LEN	0x7FFFFF  /* Max length hw supports */
#define XAXIDMA_MCHAN_MAX_TRANSFER_LEN	0x00FFFF  /* Max length MCDMA*/
#define AXIDMA_CHANNEL_NOT_HALTED	1
#define AXIDMA_CHANNEL_HALTED		2
#define XAXIDMA_BUFFLEN_OFFSET	0x00000028   /**< Tail descriptor pointer */
#define XAXIDMA_CDESC_OFFSET	0x00000008   /**< Current descriptor pointer */
#define XAXIDMA_RX_CDESC0_OFFSET	0x00000040   /**< Rx Current Descriptor 0 */
#define XAXIDMA_RX_NDESC_OFFSET		0x00000020   /**< Rx Next Descriptor Offset */
#define XAXIDMA_DESTADDR_OFFSET 	0x00000018
#define XAXIDMA_CR_RUNSTOP_MASK	0x00000001 /**< Start/stop DMA channel */
#define XAXIDMA_TX_OFFSET	0x00000000 /**< TX channel registers base
					     *  offset */
#define XAXIDMA_RX_OFFSET	0x00000030 /**< RX channel registers base*/
#define XAXIDMA_RESET_TIMEOUT	500
#define XAXIDMA_CR_OFFSET	0x00000000   /**< Channel control */
#define XAXIDMA_SR_OFFSET	0x00000004   /**< Status */
#define XAXIDMA_HALTED_MASK		0x00000001  /**< DMA channel halted */
#define XAXIDMA_DMA_TO_DEVICE		0x00
#define XAXIDMA_DEVICE_TO_DMA		0x01
#define XREG_CP15_CONTROL_C_BIT			0x00000004U

#define XPS_L2CC_BASEADDR			0xF8F02000U
#define XPS_L2CC_DEBUG_CTRL_OFFSET		0x0F40U		/* Debug Control Register */
#define XPS_L2CC_CACHE_CLEAN_PA_OFFSET		0x07B0U		/* Cache Clean by PA */
#define XPS_L2CC_CACHE_CLEAN_INDX_OFFSET	0x07B8U		/* Cache Clean by Index */
#define XPS_L2CC_CACHE_INVLD_PA_OFFSET		0x0770U		/* Cache Invalid by PA */
#define XPS_L2CC_CACHE_INV_CLN_PA_OFFSET	0x07F0U		/* Cache Invalidate and Clean by PA */
#define XPS_L2CC_CACHE_SYNC_OFFSET		0x0730U		/* Cache Sync */
#define XPS_L2CC_DUMMY_CACHE_SYNC_OFFSET 	0x0740U

#if defined (__GNUC__) || defined (__ICCARM__)
#define XREG_CP15_SYS_CONTROL			"p15, 0, %0,  c1,  c0, 0"
#define XREG_CP15_CACHE_SIZE_ID			"p15, 1, %0,  c0,  c0, 0"
#else
#define XREG_CP15_CACHE_SIZE_ID			"cp15:1:c0:c0:0"
#define XREG_CP15_SYS_CONTROL			"cp15:0:c1:c0:0"
#endif

#define CONFIG_PL310_ERRATA_588369 1
#define CONFIG_PL310_ERRATA_727915 1
#define CONFIG_PL310_ERRATA_753970 1

#define XREG_CP15_INVAL_DC_LINE_MVA_POC		"p15, 0, %0,  c7,  c6, 1"
#define XREG_CP15_CACHE_SIZE_SEL		"p15, 2, %0,  c0,  c0, 0"
#define XREG_CP15_CLEAN_INVAL_DC_LINE_MVA_POC	"p15, 0, %0,  c7, c14, 1"

//#define XREG_CP15_INVAL_DC_LINE_MVA_POC		"cp15:0:c7:c6:1"
//#define XREG_CP15_CACHE_SIZE_SEL		"cp15:2:c0:c0:0"
//#define XREG_CP15_CLEAN_INVAL_DC_LINE_MVA_POC	"cp15:0:c7:c14:1"
//#define XREG_CP15_INVAL_DC_LINE_MVA_POC		"P15,0,R4,C7,C6,1"
//#define XREG_CP15_CACHE_SIZE_SEL		"P15,2,R4,C0,C0,0"
//#define XREG_CP15_CLEAN_INVAL_DC_LINE_MVA_POC	"P15,0,R4,C7,C14,1"
/* pseudo assembler instructions */
#define mfcpsr()	({u32 rval; \
			  __asm__ __volatile__(\
			    "mrs	%0, cpsr\n"\
			    : "=r" (rval)\
			  );\
			  rval;\
			 })
#define mtcpsr(v)	__asm__ __volatile__(\
			  "msr	cpsr,%0\n"\
			  : : "r" (v)\
			)
#define IRQ_FIQ_MASK 0xC0U	/* Mask IRQ and FIQ interrupts in cpsr */
/* CP15 operations */
#define mtcp(rn, v)	__asm__ __volatile__(\
			 "mcr " rn "\n"\
			 : : "r" (v)\
			);
/* Data Synchronization Barrier */
#define dsb() __asm__ __volatile__ ("DSB" : : : "memory")

typedef unsigned long Xuint32;
typedef unsigned char Xuint8;
//typedef Xuint32 u32;
typedef Xuint8  u8;
typedef int INTPTR;
typedef struct {
	u32 DeviceId;
	u32 BaseAddr;
	int HasStsCntrlStrm;
	int HasMm2S;
	int HasMm2SDRE;
	int Mm2SDataWidth;
	int HasS2Mm;
	int HasS2MmDRE;
	int S2MmDataWidth;
	int HasSg;
	int Mm2sNumChannels;
	int S2MmNumChannels;
	int Mm2SBurstSize;
	int S2MmBurstSize;
	int MicroDmaMode;
} XAxiDma_Config;

typedef u32 XAxiDma_Bd[XAXIDMA_BD_NUM_WORDS];
typedef struct {
	u32 ChanBase;		/**< physical base address*/

	int IsRxChannel;	/**< Is this a receive channel */
	volatile int RunState;	/**< Whether channel is running */
	int HasStsCntrlStrm; 	/**< Whether has stscntrl stream */
	int HasDRE;
	int DataWidth;
	u32 MaxTransferLen;

	u32 FirstBdPhysAddr;	/**< Physical address of 1st BD in list */
	u32 FirstBdAddr;	/**< Virtual address of 1st BD in list */
	u32 LastBdAddr;		/**< Virtual address of last BD in the list */
	u32 Length;		/**< Total size of ring in bytes */
	u32 Separation;		/**< Number of bytes between the starting
				     address of adjacent BDs */
	XAxiDma_Bd *FreeHead;	/**< First BD in the free group */
	XAxiDma_Bd *PreHead;	/**< First BD in the pre-work group */
	XAxiDma_Bd *HwHead;	/**< First BD in the work group */
	XAxiDma_Bd *HwTail;	/**< Last BD in the work group */
	XAxiDma_Bd *PostHead;	/**< First BD in the post-work group */
	XAxiDma_Bd *BdaRestart;	/**< BD to load when channel is started */
	int FreeCnt;		/**< Number of allocatable BDs in free group */
	int PreCnt;		/**< Number of BDs in pre-work group */
	int HwCnt;		/**< Number of BDs in work group */
	int PostCnt;		/**< Number of BDs in post-work group */
	int AllCnt;		/**< Total Number of BDs for channel */
	int RingIndex;		/**< Ring Index */
} XAxiDma_BdRing;

typedef struct XAxiDma {
	u32 RegBase;		/* Virtual base address of DMA engine */

	int HasMm2S;		/* Has transmit channel */
	int HasS2Mm;		/* Has receive channel */
	int Initialized;	/* Driver has been initialized */
	int HasSg;

	XAxiDma_BdRing TxBdRing;     /* BD container management for TX channel */
	XAxiDma_BdRing RxBdRing[16]; /* BD container management for RX channel */
	int TxNumChannels;
	int RxNumChannels;
	int MicroDmaMode;
} XAxiDma;

#define XAxiDma_GetTxRing(InstancePtr) \
			(&((InstancePtr)->TxBdRing))
#define XAxiDma_GetRxRing(InstancePtr) \
			(&((InstancePtr)->RxBdRing[0]))
#define XAxiDma_GetRxIndexRing(InstancePtr, RingIndex) \
			(&((InstancePtr)->RxBdRing[RingIndex]))
#define XAxiDma_HasSg(InstancePtr)	((InstancePtr)->HasSg) ? 1 : 0
#define XAxiDma_IntrDisable(InstancePtr, Mask, Direction)	\
			XAxiDma_WriteReg((InstancePtr)->RegBase + \
			(XAXIDMA_RX_OFFSET * Direction), XAXIDMA_CR_OFFSET, \
			(XAxiDma_ReadReg((InstancePtr)->RegBase + \
			(XAXIDMA_RX_OFFSET * Direction), XAXIDMA_CR_OFFSET)) \
			& ~(Mask & XAXIDMA_IRQ_ALL_MASK))
#define XAxiDma_BdRingSnapShotCurrBd(RingPtr)		  \
	{								  \
		if (!RingPtr->IsRxChannel) {				  \
			(RingPtr)->BdaRestart = 			  \
			(XAxiDma_Bd *)XAxiDma_ReadReg((RingPtr)->ChanBase, \
					XAXIDMA_CDESC_OFFSET);		  \
		} else {						  \
			if (!RingPtr->RingIndex) {				  \
				(RingPtr)->BdaRestart = 		  \
				(XAxiDma_Bd *)XAxiDma_ReadReg(            \
					(RingPtr)->ChanBase, 		  \
					XAXIDMA_CDESC_OFFSET);		  \
			} else {					  \
				(RingPtr)->BdaRestart = 		  \
				(XAxiDma_Bd *)XAxiDma_ReadReg( 		  \
				(RingPtr)->ChanBase,                      \
				(XAXIDMA_RX_CDESC0_OFFSET +		  \
                                (RingPtr->RingIndex - 1) * 		  \
					XAXIDMA_RX_NDESC_OFFSET)); 	  \
			}						  \
		}							  \
	}
void Xil_Out32(INTPTR Addr, u32 Value);
u32 Xil_In32(INTPTR Addr);
#define XAxiDma_Out32	Xil_Out32
#define XAxiDma_WriteReg(BaseAddress, RegOffset, Data)          \
    XAxiDma_Out32((BaseAddress) + (RegOffset), (Data))
#define XAxiDma_ReadReg(BaseAddress, RegOffset)             \
    XAxiDma_In32((BaseAddress) + (RegOffset))
#define XAxiDma_In32	Xil_In32
XAxiDma_Config *XAxiDma_LookupConfig(u32 DeviceId);
int XAxiDma_CfgInitialize(XAxiDma * InstancePtr, XAxiDma_Config *Config, u32 vBaseAddr);
void XAxiDma_Reset(XAxiDma * InstancePtr);
int XAxiDma_SimpleTransfer(XAxiDma *InstancePtr, u32 BuffAddr, u32 Length, int Direction, u32 vaddr);
u32 XAxiDma_Busy(XAxiDma *InstancePtr, int Direction);
int XAxiDma_ResetIsDone(XAxiDma * InstancePtr);
void Xil_L1DCacheFlushLine(u32 adr);
void Xil_L2CacheSync(u32 vaddr);
void Xil_L2CacheFlushLine(u32 adr, u32 vaddr);
void Xil_L2WriteDebugCtrl(u32 Value, u32 vaddr);
void Xil_DCacheInvalidateRange(INTPTR adr, u32 len, u32 vaddr);

void Xil_L2CacheDisable(u32 vaddr);
void Xil_L2CacheFlush(u32 vaddr);


void Xil_L2CacheInvalidateLine(u32 adr);
//u32 XPAR_AXI_DMA_0_VBASEADDR;
#endif


