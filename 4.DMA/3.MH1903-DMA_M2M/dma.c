#include "inc.h"

extern uint32_t src_Buf[128];
extern uint32_t dst_Buf[128];
extern void UART_Send(UART_TypeDef* UARTx,uint8_t ch);


void DMA_Config_M2M(void)
{
	DMA->ChEnReg_L =  ((1<<channel)<<8);//关DMA线
	if(DMA->ChEnReg_L & (1<<channel))
		UART_Send(UART0,0x32);
	//清除位
	DMA->ClearTfr_L = (1<<channel);
	DMA->ClearBlock_L = (1<<channel);
	DMA->ClearSrcTran_L = (1<<channel);
	DMA->ClearDstTran_L = (1<<channel);
	DMA->ClearErr_L = (1<<channel);

	DMA_Channelx->SAR_L = (uint32_t)&src_Buf[0];//源地址
	DMA_Channelx->DAR_L = (uint32_t)&dst_Buf[0];//目的地址

	//内存
	DMA_Channelx->CTL_H &= ~(0x0fffU);
	DMA_Channelx->CTL_H |= sizeof(src_Buf) / 4;//传输数据块大小单位
	
	DMA_Channelx->CTL_L &= ~(0xFFFFFFFF);
	//地址多块、传输模式、数据突发量、地址自加、数据位宽，中断总控
	DMA_Channelx->CTL_L |=  (LLP_SRC_EN<<28) | (LLP_DST_EN<<27) | (TT_FC<<20) | (SRC_MSIZE<<14) | (DEST_MSIZE<<11) | (SINC<<9) | (DINC<<7) | (SRC_TR_WIDTH<<4) | (DST_TR_WIDTH<<1) | (INT_EN<<0);
	//握手
	DMA_Channelx->CFG_L &= ~(0xFFFFFFFF);
	DMA_Channelx->CFG_L |= (HS_SEL_SRC<<19) | (HS_SEL_DST<<18);

	//DMA使能
	DMA->DmaCfgReg_L = 1;
	UART_Send(UART0,0x33);
}

void DMA_ChannelCmd(void)
{
	//DMA通道使能
	DMA->ChEnReg_L |= (1<<channel) | ((1<<channel)<<8);
	UART_Send(UART0,0x55);
}	




