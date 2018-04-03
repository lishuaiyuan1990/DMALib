#include "myDma.h"

#define NUMBER_OF_TRANSFERS	10
#define XPAR_PS7_DDR_0_S_AXI_BASEADDR 0x00100000
#define RX_BUFFER_BASE		(XPAR_PS7_DDR_0_S_AXI_BASEADDR + 0x01000000)

#define REG_BASE_PADDR 0x43C00000
#define XPAR_AXIDMA_0_DEVICE_ID XPAR_AXI_DMA_0_DEVICE_ID
#define DMA_DEV_ID XPAR_AXIDMA_0_DEVICE_ID
#define TEST_START_VALUE 0xC


XAxiDma_Config XAxiDma_ConfigTable[] =
{
	{
		XPAR_AXI_DMA_0_DEVICE_ID,
		XPAR_AXI_DMA_0_BASEADDR,
		XPAR_AXI_DMA_0_SG_INCLUDE_STSCNTRL_STRM,
		XPAR_AXI_DMA_0_INCLUDE_MM2S,
		XPAR_AXI_DMA_0_INCLUDE_MM2S_DRE,
		XPAR_AXI_DMA_0_M_AXIS_MM2S_TDATA_WIDTH,
		XPAR_AXI_DMA_0_INCLUDE_S2MM,
		XPAR_AXI_DMA_0_INCLUDE_S2MM_DRE,
		XPAR_AXI_DMA_0_S_AXIS_S2MM_TDATA_WIDTH,
		XPAR_AXI_DMA_0_INCLUDE_SG,
		XPAR_AXI_DMA_0_NUM_MM2S_CHANNELS,
		XPAR_AXI_DMA_0_NUM_S2MM_CHANNELS
	}
};


void Xil_Out32(INTPTR Addr, u32 Value)
{
	u32 *LocalAddr = (u32 *)Addr;
	*LocalAddr = Value;
}
u32 Xil_In32(INTPTR Addr)
{
	u32 ret = *(volatile u32 *) Addr;
	return ret;
}

XAxiDma_Config *XAxiDma_LookupConfig(u32 DeviceId)
{
	//extern XAxiDma_Config XAxiDma_ConfigTable[];
	XAxiDma_Config *CfgPtr;
	u32 Index;

	CfgPtr = NULL;

	for (Index = 0; Index < XPAR_XAXIDMA_NUM_INSTANCES; Index++) {
		if (XAxiDma_ConfigTable[Index].DeviceId == DeviceId) {

			CfgPtr = &XAxiDma_ConfigTable[Index];
			break;
		}
	}

	return CfgPtr;
}
int XAxiDma_CfgInitialize(XAxiDma * InstancePtr, XAxiDma_Config *Config, u32 vBaseAddr)
{
	u32 BaseAddr;
	int TimeOut;
	int Index;
	u32 MaxTransferLen;

	InstancePtr->Initialized = 0;

	if(!Config) {
		return XST_INVALID_PARAM;
	}
	Config->BaseAddr = vBaseAddr;
	BaseAddr = vBaseAddr;

	/* Setup the instance */
	memset(InstancePtr, 0, sizeof(XAxiDma));
	InstancePtr->RegBase = BaseAddr;

	/* Get hardware setting information from the configuration structure
	 */
	InstancePtr->HasMm2S = Config->HasMm2S;
	InstancePtr->HasS2Mm = Config->HasS2Mm;

	InstancePtr->HasSg = Config->HasSg;

	InstancePtr->MicroDmaMode = Config->MicroDmaMode;

	/* Get the number of channels */
	InstancePtr->TxNumChannels = Config->Mm2sNumChannels;
	InstancePtr->RxNumChannels = Config->S2MmNumChannels;

	/* This condition is for IP version < 6.00a */
	if (!InstancePtr->TxNumChannels)
		InstancePtr->TxNumChannels = 1;
	if (!InstancePtr->RxNumChannels)
		InstancePtr->RxNumChannels = 1;

	if ((InstancePtr->RxNumChannels > 1) ||
		(InstancePtr->TxNumChannels > 1)) {
		MaxTransferLen =
			XAXIDMA_MCHAN_MAX_TRANSFER_LEN;
	}
	else {
		MaxTransferLen =
			XAXIDMA_MAX_TRANSFER_LEN;
	}

	/* Initialize the ring structures */
	InstancePtr->TxBdRing.RunState = AXIDMA_CHANNEL_HALTED;
	InstancePtr->TxBdRing.IsRxChannel = 0;
	if (!InstancePtr->MicroDmaMode) {
		InstancePtr->TxBdRing.MaxTransferLen = MaxTransferLen;
	}
	else {
		/* In MicroDMA mode, Maximum length that can be transferred
		 * is '(Memory Data Width / 4) * Burst Size'
		 */
		InstancePtr->TxBdRing.MaxTransferLen =
				((Config->Mm2SDataWidth / 4) *
				 Config->Mm2SBurstSize);
	}
	InstancePtr->TxBdRing.RingIndex = 0;

	for (Index = 0; Index < InstancePtr->RxNumChannels; Index++) {
		InstancePtr->RxBdRing[Index].RunState
						 = AXIDMA_CHANNEL_HALTED;
		InstancePtr->RxBdRing[Index].IsRxChannel = 1;
		InstancePtr->RxBdRing[Index].RingIndex = Index;
	}

	if (InstancePtr->HasMm2S) {
		InstancePtr->TxBdRing.ChanBase =
				BaseAddr + XAXIDMA_TX_OFFSET;
		InstancePtr->TxBdRing.HasStsCntrlStrm =
					Config->HasStsCntrlStrm;
		InstancePtr->TxBdRing.HasDRE = Config->HasMm2SDRE;
		InstancePtr->TxBdRing.DataWidth =
			((unsigned int)Config->Mm2SDataWidth >> 3);
	}

	if (InstancePtr->HasS2Mm) {
		for (Index = 0;
			Index < InstancePtr->RxNumChannels; Index++) {
			InstancePtr->RxBdRing[Index].ChanBase =
					BaseAddr + XAXIDMA_RX_OFFSET;
			InstancePtr->RxBdRing[Index].HasStsCntrlStrm =
					Config->HasStsCntrlStrm;
			InstancePtr->RxBdRing[Index].HasDRE =
					Config->HasS2MmDRE;
			InstancePtr->RxBdRing[Index].DataWidth =
			((unsigned int)Config->S2MmDataWidth >> 3);

			if (!InstancePtr->MicroDmaMode) {
				InstancePtr->RxBdRing[Index].MaxTransferLen =
							MaxTransferLen;
			}
			else {
			/* In MicroDMA mode, Maximum length that can be transferred
			 * is '(Memory Data Width / 4) * Burst Size'
			 */
				InstancePtr->RxBdRing[Index].MaxTransferLen =
						((Config->S2MmDataWidth / 4) *
						Config->S2MmBurstSize);
			}
		}
	}

	/* Reset the engine so the hardware starts from a known state
	 */
	XAxiDma_Reset(InstancePtr);

	/* At the initialization time, hardware should finish reset quickly
	 */
	TimeOut = XAXIDMA_RESET_TIMEOUT;

	while (TimeOut) {

		if(XAxiDma_ResetIsDone(InstancePtr)) {
			break;
		}

		TimeOut -= 1;

	}

	if (!TimeOut) {
		//xdbg_printf(XDBG_DEBUG_ERROR, "Failed reset in" "initialize\r\n");

		/* Need system hard reset to recover
		 */
		InstancePtr->Initialized = 0;
		return XST_DMA_ERROR;
	}

	/* Initialization is successful
	 */
	InstancePtr->Initialized = 1;

	return XST_SUCCESS;
}

void XAxiDma_Reset(XAxiDma * InstancePtr)
{
	u32 RegBase;
	XAxiDma_BdRing *TxRingPtr;
	XAxiDma_BdRing *RxRingPtr;
	int RingIndex;

	TxRingPtr = XAxiDma_GetTxRing(InstancePtr);

	/* Save the locations of current BDs both rings are working on
	 * before the reset so later we can resume the rings smoothly.
	 */
	if(XAxiDma_HasSg(InstancePtr)){
		XAxiDma_BdRingSnapShotCurrBd(TxRingPtr);

		for (RingIndex = 0; RingIndex < InstancePtr->RxNumChannels;
						RingIndex++) {
			RxRingPtr = XAxiDma_GetRxIndexRing(InstancePtr,
						RingIndex);
			XAxiDma_BdRingSnapShotCurrBd(RxRingPtr);
		}
	}

	/* Reset
	 */
	if (InstancePtr->HasMm2S) {
		RegBase = InstancePtr->RegBase + XAXIDMA_TX_OFFSET;
	}
	else {
		RegBase = InstancePtr->RegBase + XAXIDMA_RX_OFFSET;
	}

	XAxiDma_WriteReg(RegBase, XAXIDMA_CR_OFFSET, XAXIDMA_CR_RESET_MASK);

	/* Set TX/RX Channel state */
	if (InstancePtr->HasMm2S) {
		TxRingPtr->RunState = AXIDMA_CHANNEL_HALTED;
	}

	for (RingIndex = 0; RingIndex < InstancePtr->RxNumChannels;
					RingIndex++) {
		RxRingPtr = XAxiDma_GetRxIndexRing(InstancePtr, RingIndex);
		if (InstancePtr->HasS2Mm) {
			RxRingPtr->RunState = AXIDMA_CHANNEL_HALTED;
		}
	}
}
int XAxiDma_SimpleTransfer(XAxiDma *InstancePtr, u32 BuffAddr, u32 Length, int Direction, u32 vaddr)
{
	static int first_time_run = 1;
	u32 WordBits;
	int RingIndex = 0;

	/* If Scatter Gather is included then, cannot submit
	 */
	/*if (XAxiDma_HasSg(InstancePtr)) {
		//xdbg_printf(XDBG_DEBUG_ERROR, "Simple DMA mode is not"" supported\r\n");
		return XST_FAILURE;
	}*/

	//if(Direction == XAXIDMA_DEVICE_TO_DMA){
	if (first_time_run)
	{
		first_time_run = 0;	
		if ((Length < 1) ||
			(Length >
			InstancePtr->RxBdRing[RingIndex].MaxTransferLen)) {
			return XST_INVALID_PARAM;
		}


		if (!InstancePtr->HasS2Mm) {
			//xdbg_printf(XDBG_DEBUG_ERROR, "S2MM channel is not"" supported\r\n");

			return XST_FAILURE;
		}

		if(!(XAxiDma_ReadReg(InstancePtr->RxBdRing[RingIndex].ChanBase,
				XAXIDMA_SR_OFFSET) & XAXIDMA_HALTED_MASK)) {
			if (XAxiDma_Busy(InstancePtr,Direction)) {
				//xdbg_printf(XDBG_DEBUG_ERROR,"Engine is busy\r\n");
				return XST_FAILURE;
			}
		}

		if (!InstancePtr->MicroDmaMode) {
			WordBits =
			 (u32)((InstancePtr->RxBdRing[RingIndex].DataWidth) - 1);
		}
		else {
			WordBits = XAXIDMA_MICROMODE_MIN_BUF_ALIGN;
		}

		if ((BuffAddr & WordBits)) {

			if (!InstancePtr->RxBdRing[RingIndex].HasDRE) {
				//xdbg_printf(XDBG_DEBUG_ERROR,"Unaligned transfer without"" DRE %x\r\n", (unsigned int)BuffAddr);

				return XST_INVALID_PARAM;
			}
		}

		//XAxiDma_WriteReg(0x43C00000,4,Length/4);
		XAxiDma_WriteReg(vaddr, 4, Length/2);
	}

	XAxiDma_WriteReg(InstancePtr->RxBdRing[RingIndex].ChanBase, XAXIDMA_DESTADDR_OFFSET, BuffAddr);
	XAxiDma_WriteReg(InstancePtr->RxBdRing[RingIndex].ChanBase,
			XAXIDMA_CR_OFFSET,
		XAxiDma_ReadReg(InstancePtr->RxBdRing[RingIndex].ChanBase,
		XAXIDMA_CR_OFFSET)| XAXIDMA_CR_RUNSTOP_MASK);
	/* Writing to the BTT register starts the transfer
	 */
	XAxiDma_WriteReg(InstancePtr->RxBdRing[RingIndex].ChanBase,
				XAXIDMA_BUFFLEN_OFFSET, Length);
	//}

	return XST_SUCCESS;
}

u32 XAxiDma_Busy(XAxiDma *InstancePtr, int Direction)
{
	u32 data = 123;
	data = ((XAxiDma_ReadReg(InstancePtr->RegBase +
				(XAXIDMA_RX_OFFSET * Direction),
				XAXIDMA_SR_OFFSET) &
				XAXIDMA_IDLE_MASK) ? 0 : 1);
	return data;
}

int XAxiDma_ResetIsDone(XAxiDma * InstancePtr)
{
	u32 RegisterValue;
	XAxiDma_BdRing *TxRingPtr;
	XAxiDma_BdRing *RxRingPtr;

	TxRingPtr = XAxiDma_GetTxRing(InstancePtr);
	RxRingPtr = XAxiDma_GetRxRing(InstancePtr);

	/* Check transmit channel
	 */
	if (InstancePtr->HasMm2S) {
		RegisterValue = XAxiDma_ReadReg(TxRingPtr->ChanBase,
			XAXIDMA_CR_OFFSET);

		/* Reset is done when the reset bit is low
		 */
		if(RegisterValue & XAXIDMA_CR_RESET_MASK) {

			return 0;
		}
	}

	/* Check receive channel
	 */
	if (InstancePtr->HasS2Mm) {
		RegisterValue = XAxiDma_ReadReg(RxRingPtr->ChanBase,
				XAXIDMA_CR_OFFSET);

		/* Reset is done when the reset bit is low
		 */
		if(RegisterValue & XAXIDMA_CR_RESET_MASK) {

			return 0;
		}
	}

	return 1;
}
void Xil_L1DCacheFlushLine(u32 adr)
{
	mtcp(XREG_CP15_CACHE_SIZE_SEL, 0U);
	mtcp(XREG_CP15_CLEAN_INVAL_DC_LINE_MVA_POC, (adr & (~0x1FU)));

	/* Wait for L1 flush to complete */
	dsb();
}
void Xil_L2CacheSync(u32 vaddr)
{
	Xil_Out32(vaddr + XPS_L2CC_DUMMY_CACHE_SYNC_OFFSET, 0x0U);
}
void Xil_L2CacheFlushLine(u32 adr, u32 vaddr)
{
	Xil_Out32(vaddr + XPS_L2CC_CACHE_CLEAN_PA_OFFSET, adr);
	Xil_Out32(vaddr + XPS_L2CC_CACHE_INVLD_PA_OFFSET, adr);
	/* synchronize the processor */
	dsb();
}
void Xil_L2WriteDebugCtrl(u32 Value, u32 vaddr)
{
	Xil_Out32(vaddr + XPS_L2CC_DEBUG_CTRL_OFFSET, Value);
}

void Xil_DCacheInvalidateRange(INTPTR adr, u32 len, u32 vaddr)
{
	const u32 cacheline = 32U;
	u32 end;
	u32 tempadr = adr;
	u32 tempend;
	u32 currmask;
	volatile u32 *L2CCOffset = (volatile u32 *)(vaddr + XPS_L2CC_CACHE_INVLD_PA_OFFSET);
	currmask = mfcpsr();
	mtcpsr(currmask | IRQ_FIQ_MASK);

	if (len != 0U) {
		end = tempadr + len;
		tempend = end;
		/* Select L1 Data cache in CSSR */
		mtcp(XREG_CP15_CACHE_SIZE_SEL, 0U);

		if ((tempadr & (cacheline-1U)) != 0U) {
			tempadr &= (~(cacheline - 1U));

			//Xil_L1DCacheFlushLine(tempadr);
			/* Disable Write-back and line fills */
			Xil_L2WriteDebugCtrl(0x3U, vaddr);
			Xil_L2CacheFlushLine(tempadr, vaddr);
			/* Enable Write-back and line fills */
			Xil_L2WriteDebugCtrl(0x0U, vaddr);
			Xil_L2CacheSync(vaddr);
			tempadr += cacheline;
		}
		if ((tempend & (cacheline-1U)) != 0U) {
			tempend &= (~(cacheline - 1U));

			//Xil_L1DCacheFlushLine(tempend);
			/* Disable Write-back and line fills */
			Xil_L2WriteDebugCtrl(0x3U, vaddr);
			Xil_L2CacheFlushLine(tempend, vaddr);
			/* Enable Write-back and line fills */
			Xil_L2WriteDebugCtrl(0x0U, vaddr);
			Xil_L2CacheSync(vaddr);
		}

		while (tempadr < tempend) {
			/* Invalidate L2 cache line */
			*L2CCOffset = tempadr;
			Xil_L2CacheSync(vaddr);
			/* Invalidate L1 Data cache line */
			__asm__ __volatile__("mcr " \
			XREG_CP15_INVAL_DC_LINE_MVA_POC :: "r" (tempadr));
			tempadr += cacheline;
		}
	}
	dsb();
	mtcpsr(currmask);
}

void Xil_L2CacheDisable(u32 vaddr)
{
    register u32 L2CCReg;


    L2CCReg = XAxiDma_ReadReg(vaddr, XPS_L2CC_CNTRL_OFFSET);


    if((L2CCReg & 0x1U) != 0U) {
        /* Clean and Invalidate L2 Cache */
        Xil_L2CacheFlush(vaddr);
	    /* Disable the L2CC */
	L2CCReg = Xil_In32(vaddr + XPS_L2CC_CNTRL_OFFSET);
	    Xil_Out32(vaddr + XPS_L2CC_CNTRL_OFFSET,
		      (L2CCReg & (~0x01U)));
		/* Wait for the cache operations to complete */

		dsb();
    }
}

void Xil_L2CacheFlush(u32 vaddr)
{
	u32 ResultL2Cache;

	/* Flush the caches */

	/* Disable Write-back and line fills */
	Xil_L2WriteDebugCtrl(0x3U, vaddr);

	Xil_Out32(vaddr + XPS_L2CC_CACHE_INV_CLN_WAY_OFFSET,
		  0x0000FFFFU);
	ResultL2Cache = Xil_In32(vaddr + XPS_L2CC_CACHE_INV_CLN_WAY_OFFSET)
							& 0x0000FFFFU;

	while(ResultL2Cache != (u32)0U) {
		ResultL2Cache = Xil_In32(vaddr + XPS_L2CC_CACHE_INV_CLN_WAY_OFFSET)
									& 0x0000FFFFU;
	}

	Xil_L2CacheSync(vaddr);
	/* Enable Write-back and line fills */
	Xil_L2WriteDebugCtrl(0x0U, vaddr);

	/* synchronize the processor */
	dsb();
}


void Xil_L2CacheInvalidateLine(u32 adr)
{
	Xil_Out32(XPS_L2CC_BASEADDR + XPS_L2CC_CACHE_INVLD_PA_OFFSET, (u32)adr);
	/* synchronize the processor */
	dsb();
}





