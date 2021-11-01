#include "inc.h"

extern uint32_t src_Buf0[MULTIBLOCK_BLOCKSIZE];

extern uint32_t dst_Buf0[MULTIBLOCK_BLOCKSIZE];


extern void UART_Send(UART_TypeDef* UARTx,uint8_t ch);

LLI llilist[MULTIBLOCK_BLOCKCOUNTER];

#define LLP_SRC_EN 			0       //多块源地址使能 0:禁止 1：使能
#define LLP_DST_EN 			0       //多块目的地址使能
#define TT_FC 				0x2U    //传输模式  0：M2M  1：M2P  2：P2M
#define SRC_MSIZE 			0x0U    //源数据突发传输量 0:1		1:4		2:8
#define DEST_MSIZE 			0x0U    //目标数据突发传输量 0:1		1:4		2:8
#define SINC 				0x2U    //源地址 0:自加  1:自减  2/3:地址不变
#define DINC 				0x0U    //目标地址 0:自加  1:自减  2/3:地址不变
#define SRC_TR_WIDTH 		0x2U    //源数据位宽 0:8位  1:16位  2:32位
#define DST_TR_WIDTH 		0x2U    //目的数据位宽 0:8位  1:16位  2:32位
#define INT_EN 				0x1     //中断总控 0：关闭 1：开启所有中断

#define RELOAD_DST 			0//自动加载多块目的地址 1：使能  0：未使能
#define RELOAD_SRC 			0//自动加载多块源地址 1：使能  0：未使能
#define HS_SEL_SRC 			0//源设备握手 0：硬握手  1：软握手
#define HS_SEL_DST 			0//目标设备握手 0：硬握手  1：软握手

void DMA_Config_P2M(void)
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

	DMA_Channelx->SAR_L = (uint32_t)&(UART0->OFFSET_0.RBR);//源地址
	DMA_Channelx->DAR_L = (uint32_t)dst_Buf0;//目的地址
	SYSCTRL->DMA_CHAN = (SYSCTRL->DMA_CHAN & 0xFFFFFFE0) | 0x00000003;//选择外设UART0
	//内存
	DMA_Channelx->CTL_H &= ~(0x0fffU);
	DMA_Channelx->CTL_H |= MULTIBLOCK_BLOCKSIZE;//传输数据块大小单位
	
	DMA_Channelx->CTL_L &= ~(0xFFFFFFFF);
	//地址多块、传输模式、数据突发量、地址自加、数据位宽，中断总控
	DMA_Channelx->CTL_L |=  (LLP_SRC_EN<<28) | (LLP_DST_EN<<27) | (TT_FC<<20) | (SRC_MSIZE<<14) | (DEST_MSIZE<<11) | (SINC<<9) | (DINC<<7) | (SRC_TR_WIDTH<<4) | (DST_TR_WIDTH<<1) | (INT_EN<<0);
	//硬握手
	DMA_Channelx->CFG_L &= ~(0xFFFFFFFF);
	DMA_Channelx->CFG_L |= (HS_SEL_SRC<<19) | (HS_SEL_DST<<18);

 	//--------------------------多块传输配置---------------
	//设置指向的链表
	DMA_Channelx->LLP_L &= (0x3);//清零LLP寄存器
	DMA_Channelx->LLP_L |= ((uint32_t)&llilist[0]) & ~(0x3);//奇怪的是这个地方地址不用左移2位，低2位是否不起作用
	DMA_Channelx->CFG_L &= ~(1 << 30);//清零自动加载源地址初值
	DMA_Channelx->CFG_L &= ~(0x80000000);//清零自动加载目的地址初值
	DMA_Channelx->CTL_L |= 1 << 28;//使能多块源数据
	DMA_Channelx->CTL_L |= 1 << 27;//使能多块目的地址
	DMA_Channelx->CTL_H |= 1 << 12;//使能块传输完成标志位
	//---------------------------------------------------- 
	
	//DMA使能
	DMA->DmaCfgReg_L = 1;
	UART_Send(UART1,0x33);

 	DMA->ClearBlock_L = (1<<channel);//清除块完成标志位
	DMA->ClearErr_L = (1<<channel);//清除传输错误位
	DMA_Channelx->CTL_L |= ((uint32_t)0x01);//开启中断总控
	DMA->MaskBlock_L = (((1<<channel) << 8 ) | (1<<channel));//使能块传输完成中断
	DMA->MaskErr_L = (((1<<channel) << 8) | (1<<channel));//使能传输错误中断 

}





void DMA_ChannelCmd(void)
{
	//DMA通道使能
	UART_Send(UART1,0x55);
	DMA->ChEnReg_L |= (1<<channel) | ((1<<channel)<<8);
}	



void DMA_InitLLI(LLI *lli, LLI *next_lli, void *src_addr, void *dest_addr, uint16_t btsize)
{
	lli->SAR = (uint32_t)src_addr;
	lli->DAR = (uint32_t)dest_addr;
	
	lli->LLP = DMA_Channelx->LLP_L & (0x3);
	lli->LLP |= ((uint32_t)next_lli) & ~(0x3);
	lli->CTL_L = DMA_Channelx->CTL_L;
//	lli->CTL_H &= ~DMA_CTL_BLOCK_TS_Mask;
	lli->CTL_H = btsize;
//	lli->CTL_H |= 1 << 12;
	lli->DSTAT = 0;
}


void LLI_Init(void)
{
	DMA_InitLLI(&llilist[0],&llilist[0], (void*)(uint32_t)&(UART0->OFFSET_0.RBR), (void*)((uint32_t)dst_Buf0), MULTIBLOCK_BLOCKSIZE);
}



















