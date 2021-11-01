#include "inc.h"
extern void USART_SendData(uint16_t Data);
//解锁
void Flash_Unlock()
{
    FLASH_KEYR = FLASH_KEY1;
    FLASH_KEYR = FLASH_KEY2;
}

//上锁
void Flash_Lock()
{
    FLASH_CR |= ((uint32_t)0x00000080);
}

//清除标志位
void Flash_Init()
{
    FLASH_SR |= (1<<5);//EOP
    FLASH_SR |= (1<<4);//WRPRT
    FLASH_SR |= (1<<2);//PGERR
}

//查询状态
uint32_t Flash_status()
{
    uint32_t FLASHStatus = 1;
    uint32_t Timeout = 0x00002000;
    while((FLASH_SR & ((uint32_t)0x00000001)) == ((uint32_t)0x00000001) && (Timeout != 0x00))//BSY在忙状态
        {
            Timeout--;
            if((FLASH_SR & ((uint32_t)0x00000001)) == ((uint32_t)0x00000001))//还在忙
                {
                    FLASHStatus = 0;
                }
            else if((FLASH_SR & ((uint32_t)0x00000004)) != 0)//PGERR编程错误
                    {
                        FLASHStatus = 0;
                    }
                 else if ((FLASH_SR & ((uint32_t)0x00000010)) != 0)//WRPRTERR写保护错误
                        {
                            FLASHStatus = 0;
                        }
                       else
                        {
                            FLASHStatus = 1;
                        }
        }
    return FLASHStatus;
}

//全局擦除
void Flash_Erase()
{
    //计算要一共的页数
    uint32_t FLASHStatus = 1;
    uint32_t NbrOfPage = (WRITE_END_ADDR - WRITE_START_ADDR) / FLASH_PAGE_SIZE;
    Flash_Unlock();
    Flash_Init();

    FLASHStatus = Flash_status();
    uint32_t EraseCounter = 0;
    while ((EraseCounter < NbrOfPage) && (FLASHStatus == 1))
    {
        FLASH_CR |= (1<<1);//PER位，页擦除 
        FLASH_AR = (WRITE_START_ADDR + (FLASH_PAGE_SIZE * EraseCounter)); 
        FLASH_CR |= (1<<6);
        FLASHStatus = Flash_status();
        FLASH_CR &= ((uint32_t)0x00001FFD);//结束擦除
        EraseCounter++;
    }
    Flash_Lock();
}


//写入数据，写入地址
int Flash_Write(uint8_t Data[128],uint32_t NowAddr)
{
    uint32_t FLASHStatus;
    uint16_t data16_t;
    uint32_t Address;
    //解锁
    Flash_Unlock();
    //清除标志位
    Flash_Init();
    Address = NowAddr;
/*
    FLASHStatus = Flash_status();
    if(FLASHStatus == 1)
    {
        FLASH_CR |= (1<<1);//PER位，页擦除 
        FLASH_AR = Address;
        FLASH_CR |= (1<<6);
        FLASHStatus = Flash_status();
        FLASH_CR &= ((uint32_t)0x00001FFD);//结束擦除
    }
*/
    FLASHStatus = Flash_status();
    int num = 0;
    while ((num<128) && (FLASHStatus == 1))//在进行写的时候不能进行读操作，会上锁
    {
        data16_t = ((Data[num]) | (Data[num+1]<<8));
        FLASH_CR |= (1<<0);//开始编程
        *(volatile uint16_t*)Address = (uint16_t)data16_t;
        FLASHStatus = Flash_status();
        FLASH_CR &= ((uint32_t)0x00001FFE);
        Address = Address + 2;
        num = num + 2;
    }
    //上锁
    Flash_Lock();
    //检查写入正确
    Address = NowAddr;
    for(int num = 0;num<128;)
    {
        data16_t = ((Data[num]) | (Data[num+1]<<8));
        if((*(volatile uint16_t*)Address) != data16_t)
        {
            if(IF){USART_SendData(data16_t);}
            if(IF){USART_SendData(*(volatile uint16_t*)Address);}
            return 1;
        }
        Address = Address + 2;
        num = num + 2;
    }
    return 0;
}


               