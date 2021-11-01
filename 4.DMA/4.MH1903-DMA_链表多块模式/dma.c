#include "inc.h"

extern uint32_t src_Buf0[MULTIBLOCK_BLOCKSIZE];
extern uint32_t src_Buf1[MULTIBLOCK_BLOCKSIZE];
extern uint32_t src_Buf2[MULTIBLOCK_BLOCKSIZE];
extern uint32_t src_Buf3[MULTIBLOCK_BLOCKSIZE];

extern uint32_t dst_Buf0[MULTIBLOCK_BLOCKSIZE];
extern uint32_t dst_Buf1[MULTIBLOCK_BLOCKSIZE];
extern uint32_t dst_Buf2[MULTIBLOCK_BLOCKSIZE];
extern uint32_t dst_Buf3[MULTIBLOCK_BLOCKSIZE];

extern void UART_Send(UART_TypeDef* UARTx,uint8_t ch);

LLI llilist[MULTIBLOCK_BLOCKCOUNTER];


void DMA_Config_M2M(void)
{
	SYSCTRL->CG_CTRL2 |= ((uint32_t)0x20000000);//DMA的AHB时钟总线使能
    SYSCTRL->LOCK_R &= ~(((uint32_t)0x20000000) & 0xF0000000);
    SYSCTRL->SOFT_RST2 |= ((uint32_t)0x20000000);
    SYSCTRL->LOCK_R |= (((uint32_t)0x20000000) & 0xF0000000);

	DMA->ChEnReg_L =  ((1<<channel)<<8);//关DMA线
	if(DMA->ChEnReg_L & (1<<channel))
		UART_Send(UART0,0x32);
	//清除位
	DMA->ClearTfr_L = (1<<channel);
	DMA->ClearBlock_L = (1<<channel);
	DMA->ClearSrcTran_L = (1<<channel);
	DMA->ClearDstTran_L = (1<<channel);
	DMA->ClearErr_L = (1<<channel);

	DMA_Channelx->SAR_L = (uint32_t)&src_Buf0[0];//源地址
	DMA_Channelx->DAR_L = (uint32_t)&dst_Buf0[0];//目的地址

	//内存
	DMA_Channelx->CTL_H &= ~(0x0fffU);
	DMA_Channelx->CTL_H |= MULTIBLOCK_BLOCKSIZE;//传输数据块大小单位
	
	DMA_Channelx->CTL_L &= ~(0xFFFFFFFF);
	//地址多块、传输模式、数据突发量、地址自加、数据位宽，中断总控
	DMA_Channelx->CTL_L |=  (LLP_SRC_EN<<28) | (LLP_DST_EN<<27) | (TT_FC<<20) | (SRC_MSIZE<<14) | (DEST_MSIZE<<11) | (SINC<<9) | (DINC<<7) | (SRC_TR_WIDTH<<4) | (DST_TR_WIDTH<<1) | (INT_EN<<0);
	//硬握手
	DMA_Channelx->CFG_L &= ~(0xFFFFFFFF);
	DMA_Channelx->CFG_L |= (HS_SEL_SRC<<19) | (HS_SEL_DST<<18);

	//设置指向的链表
	DMA_Channelx->LLP_L &= (0x3);//清零LLP寄存器

	DMA_Channelx->LLP_L |= ((uint32_t)&llilist[0]) & ~(0x3);//奇怪的是这个地方地址不用左移2位，低2位是否不起作用
	DMA_Channelx->CFG_L &= ~(1 << 30);//清零自动加载源地址初值
	DMA_Channelx->CFG_L &= ~(0x80000000);//清零自动加载目的地址初值
	DMA_Channelx->CTL_L |= 1 << 28;//使能多块源数据
	DMA_Channelx->CTL_L |= 1 << 27;//使能多块目的地址
	DMA_Channelx->CTL_H |= 1 << 12;//使能块传输完成标志位

	//DMA使能
	DMA->DmaCfgReg_L = 1;
	UART_Send(UART0,0x33);

	DMA->ClearBlock_L = (1<<channel);//清除块完成标志位
	DMA->ClearErr_L = (1<<channel);//清除传输错误位

	DMA_Channelx->CTL_L |= ((uint32_t)0x01);//开启中断总控
	DMA->MaskBlock_L = (((1<<channel) << 8 ) | (1<<channel));//使能块传输完成中断
	DMA->MaskErr_L = (((1<<channel) << 8) | (1<<channel));//使能传输错误中断

}





void DMA_ChannelCmd(void)
{
	//DMA通道使能
	DMA->ChEnReg_L |= (1<<channel) | ((1<<channel)<<8);
	UART_Send(UART0,0x55);
}	



void DMA_InitLLI(LLI *lli, LLI *next_lli, void *src_addr, void *dest_addr, uint16_t btsize)
{
	lli->SAR = (uint32_t)src_addr;
	lli->DAR = (uint32_t)dest_addr;
	
	lli->LLP = DMA_Channelx->LLP_L & (0x3);
	lli->LLP |= ((uint32_t)next_lli) & ~(0x3);
	lli->CTL_L = DMA_Channelx->CTL_L;
	lli->CTL_H &= ~(0x00001FFF);
	lli->CTL_H = btsize;
//	lli->CTL_H |= 1 << 12;
	lli->DSTAT = 0;
}


void LLI_Init(void)
{
	DMA_InitLLI(&llilist[0],&llilist[1], (void*)(uint32_t)src_Buf0, (void*)((uint32_t)dst_Buf0), MULTIBLOCK_BLOCKSIZE);
	DMA_InitLLI(&llilist[1],&llilist[2], (void*)(uint32_t)src_Buf1, (void*)((uint32_t)dst_Buf1), MULTIBLOCK_BLOCKSIZE);
	DMA_InitLLI(&llilist[2],&llilist[3], (void*)(uint32_t)src_Buf2, (void*)((uint32_t)dst_Buf2), MULTIBLOCK_BLOCKSIZE);
	DMA_InitLLI(&llilist[3],&llilist[4], (void*)(uint32_t)src_Buf3, (void*)((uint32_t)dst_Buf3), MULTIBLOCK_BLOCKSIZE);
}



















