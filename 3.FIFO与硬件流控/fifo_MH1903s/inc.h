#ifndef INC_H
#define INC_H

#define uint32_t unsigned int
#define uint16_t unsigned short int
#define uint8_t  unsigned char

typedef   signed          char int8_t;
typedef   signed short     int int16_t;
typedef   signed           int int32_t;

#define     __IO    volatile
#define     __O     volatile
#define     __I     volatile const

#define NULL                      0
#define __NVIC_PRIO_BITS          3
#define GPIO_GROUP_NUM            6

typedef struct
{
    union
    {
        __I  uint32_t RBR;
        __O  uint32_t THR;
        __IO uint32_t DLL;
    } OFFSET_0;
    union
    {
        __IO uint32_t DLH;
        __IO uint32_t IER;
    } OFFSET_4;
    union
    {
        __I uint32_t IIR;
        __O uint32_t FCR;
    } OFFSET_8;
    __IO uint32_t LCR;
    __IO uint32_t MCR;
    __I  uint32_t LSR;
    __I  uint32_t MSR;
    __IO uint32_t SCR;
    __IO uint32_t LPDLL;
    __IO uint32_t LPDLH;
    __I  uint32_t RES0[2];
    union
    {
        __I  uint32_t SRBR[16];
        __O  uint32_t STHR[16];
    } OFFSET_48;
    __IO uint32_t FAR;
    __I  uint32_t TFR;
    __O  uint32_t RFW;
    __I  uint32_t USR;
    __I  uint32_t TFL;
    __I  uint32_t RFL;
    __O  uint32_t SRR;
    __IO uint32_t SRTS;
    __IO uint32_t SBCR;
    __IO uint32_t SDMAM;
    __IO uint32_t SFE;
    __IO uint32_t SRT;
    __IO uint32_t STET;
    __IO uint32_t HTX;
    __O uint32_t DMASA;
    __I  uint32_t RES1[18];
    __I  uint32_t CPR;
    __I  uint32_t UCV;
    __I  uint32_t CTR;
    
} UART_TypeDef;

typedef struct
{
    __IO uint32_t FREQ_SEL;
    __IO uint32_t CG_CTRL1;
    __IO uint32_t CG_CTRL2;
    __O uint32_t  SOFT_RST1;
    __O uint32_t  SOFT_RST2;
    __IO uint32_t LOCK_R;
    __IO uint32_t PHER_CTRL;
	__I uint32_t  SYS_RSVD[(0x2C-0x1C) >> 2];
    __I uint32_t  HCLK_1MS_VAL;
    __I uint32_t  PCLK_1MS_VAL;
    __IO uint32_t ANA_CTRL;
    __IO uint32_t DMA_CHAN;
    __IO uint32_t SCI0_GLF;
    __IO uint32_t SW_RSV1;
    __IO uint32_t SW_RSV2;
    __IO uint32_t CARD_RSVD;
    __IO uint32_t LDO25_CR;
	__IO uint32_t DMA_CHAN1;
    __I uint32_t  SYS_RSVD2[(0x100-0x54) >> 2];
    __IO uint32_t MSR_CR1;
    __IO uint32_t MSR_CR2;
    __IO uint32_t USBPHY_CR1;
    __IO uint32_t USBPHY_CR2;
    __IO uint32_t USBPHY_CR3;
	__IO uint32_t ISO7816_CR;
	__IO uint32_t LDO_CR;
	__IO uint32_t CHG_CSR;
    __I uint32_t  SYS_RSVD3[(0x204-0x120) >> 2];
    __IO uint32_t RSVD_POR;
    __I uint32_t  SYS_RSVD4[(0x3EC-0x208) >> 2];
    __IO uint32_t PM2_WK_FLAG;
    __IO uint32_t CALIB_CSR;
    __IO uint32_t DBG_CR;
    __IO uint32_t CHIP_ID;
} SYSCTRL_TypeDef;

typedef struct
{
    __IO uint32_t IODR;
    __IO uint32_t BSRR;
    __IO uint32_t OEN;
    __IO uint32_t PUE;
} GPIO_TypeDef;

typedef struct
{
    __IO uint32_t INTP_TYPE;
    __IO uint32_t INTP_STA;
} GPIO_INTP_TypeDef;

typedef struct
{
    GPIO_TypeDef GPIO[GPIO_GROUP_NUM];
    __I  uint32_t RSVD0[(0x114 - 0x060)>>2];
    __I  uint32_t INTP[GPIO_GROUP_NUM];
    __I  uint32_t RSVD1[(0x180 - 0x12C)>>2];
    __IO uint32_t ALT[GPIO_GROUP_NUM];
    __I  uint32_t RSVD2[(0x200 - 0x198)>>2];
    __IO uint32_t SYS_CR1;
    __I  uint32_t RSVD3[(0x220 - 0x204)>>2];
    __IO uint32_t WAKE_TYPE_EN;
    __IO uint32_t WAKE_P0_EN;
    __IO uint32_t WAKE_P1_EN;
    __IO uint32_t WAKE_P2_EN;
    __IO uint32_t WAKE_P3_EN;
    __I  uint32_t RSVD5[(0x800 - 0x234)>>2];
    GPIO_INTP_TypeDef INTP_TYPE_STA[GPIO_GROUP_NUM];
} GPIO_MODULE_TypeDef;

typedef struct
{
    uint32_t PLL_Frequency;     
    uint32_t CPU_Frequency;     
    uint32_t HCLK_Frequency;    
    uint32_t PCLK_Frequency;   
} SYSCTRL_ClocksTypeDef;

typedef struct
{
  __I  uint32_t CPUID;                   /*!< Offset: 0x000 (R/ )  CPUID Base Register                                   */
  __IO uint32_t ICSR;                    /*!< Offset: 0x004 (R/W)  Interrupt Control and State Register                  */
  __IO uint32_t VTOR;                    /*!< Offset: 0x008 (R/W)  Vector Table Offset Register                          */
  __IO uint32_t AIRCR;                   /*!< Offset: 0x00C (R/W)  Application Interrupt and Reset Control Register      */
  __IO uint32_t SCR;                     /*!< Offset: 0x010 (R/W)  System Control Register                               */
  __IO uint32_t CCR;                     /*!< Offset: 0x014 (R/W)  Configuration Control Register                        */
  __IO uint8_t  SHP[12];                 /*!< Offset: 0x018 (R/W)  System Handlers Priority Registers (4-7, 8-11, 12-15) */
  __IO uint32_t SHCSR;                   /*!< Offset: 0x024 (R/W)  System Handler Control and State Register             */
  __IO uint32_t CFSR;                    /*!< Offset: 0x028 (R/W)  Configurable Fault Status Register                    */
  __IO uint32_t HFSR;                    /*!< Offset: 0x02C (R/W)  HardFault Status Register                             */
  __IO uint32_t DFSR;                    /*!< Offset: 0x030 (R/W)  Debug Fault Status Register                           */
  __IO uint32_t MMFAR;                   /*!< Offset: 0x034 (R/W)  MemManage Fault Address Register                      */
  __IO uint32_t BFAR;                    /*!< Offset: 0x038 (R/W)  BusFault Address Register                             */
  __IO uint32_t AFSR;                    /*!< Offset: 0x03C (R/W)  Auxiliary Fault Status Register                       */
  __I  uint32_t PFR[2];                  /*!< Offset: 0x040 (R/ )  Processor Feature Register                            */
  __I  uint32_t DFR;                     /*!< Offset: 0x048 (R/ )  Debug Feature Register                                */
  __I  uint32_t ADR;                     /*!< Offset: 0x04C (R/ )  Auxiliary Feature Register                            */
  __I  uint32_t MMFR[4];                 /*!< Offset: 0x050 (R/ )  Memory Model Feature Register                         */
  __I  uint32_t ISAR[5];                 /*!< Offset: 0x060 (R/ )  Instruction Set Attributes Register                   */
       uint32_t RESERVED0[5];
  __IO uint32_t CPACR;                   /*!< Offset: 0x088 (R/W)  Coprocessor Access Control Register                   */
} SCB_Type;

typedef struct
{
  __IO uint32_t ISER[8];                 /*!< Offset: 0x000 (R/W)  Interrupt Set Enable Register           */
       uint32_t RESERVED0[24];
  __IO uint32_t ICER[8];                 /*!< Offset: 0x080 (R/W)  Interrupt Clear Enable Register         */
       uint32_t RSERVED1[24];
  __IO uint32_t ISPR[8];                 /*!< Offset: 0x100 (R/W)  Interrupt Set Pending Register          */
       uint32_t RESERVED2[24];
  __IO uint32_t ICPR[8];                 /*!< Offset: 0x180 (R/W)  Interrupt Clear Pending Register        */
       uint32_t RESERVED3[24];
  __IO uint32_t IABR[8];                 /*!< Offset: 0x200 (R/W)  Interrupt Active bit Register           */
       uint32_t RESERVED4[56];
  __IO uint8_t  IP[240];                 /*!< Offset: 0x300 (R/W)  Interrupt Priority Register (8Bit wide) */
       uint32_t RESERVED5[644];
  __O  uint32_t STIR;                    /*!< Offset: 0xE00 ( /W)  Software Trigger Interrupt Register     */
}  NVIC_Type;

typedef enum IRQn
{
/******  Cortex-M# Processor Exceptions Numbers ***************************************************/

/* ToDo: use this Cortex interrupt numbers if your device is a CORTEX-M3 / Cortex-M4 device       */
	NonMaskableInt_IRQn           = -14,      /*!<  2 Non Maskable Interrupt                      */
	MemoryManagement_IRQn         = -12,      /*!<  4 Memory Management Interrupt                 */
	BusFault_IRQn                 = -11,      /*!<  5 Bus Fault Interrupt                         */
	UsageFault_IRQn               = -10,      /*!<  6 Usage Fault Interrupt                       */
	SVCall_IRQn                   = -5,       /*!< 11 SV Call Interrupt                           */
	DebugMonitor_IRQn             = -4,       /*!< 12 Debug Monitor Interrupt                     */
	PendSV_IRQn                   = -2,       /*!< 14 Pend SV Interrupt                           */
	SysTick_IRQn                  = -1,       /*!< 15 System Tick Interrupt                       */

/******  Device Specific Interrupt Numbers ********************************************************/
/* ToDo: add here your device specific external interrupt numbers
         according the interrupt handlers defined in startup_Device.s
         eg.: Interrupt for Timer#1       TIM1_IRQHandler   ->   TIM1_IRQn                        */
    DMA_IRQn                                        = 0,
    USB_IRQn                                        = 1,
    USBDMA_IRQn                                     = 2,
    LCD_IRQn                                        = 3,
    SCI0_IRQn                                       = 4,
    UART0_IRQn                                      = 5,
    UART1_IRQn                                      = 6,
    SPI0_IRQn                                       = 7,
    CRYPT0_IRQn                                     = 8,
    TIM0_0_IRQn                                     = 9,
    TIM0_1_IRQn                                     = 10,
    TIM0_2_IRQn                                     = 11,
    TIM0_3_IRQn                                     = 12,
    EXTI0_IRQn                                      = 13,
    EXTI1_IRQn                                      = 14,
    EXTI2_IRQn                                      = 15,
    RTC_IRQn                                        = 16,
    SENSOR_IRQn                                     = 17,
    TRNG_IRQn                                       = 18,
    ADC0_IRQn                                       = 19,
    SSC_IRQn                                        = 20,
    TIM0_4_IRQn                                     = 21,
    TIM0_5_IRQn                                     = 22,
    KBD_IRQn                                        = 23,
    MSR_IRQn                                        = 24,
    EXTI3_IRQn                                      = 25,
    SPI1_IRQn                                       = 26,
    SPI2_IRQn                                       = 27,
    
    SCI2_IRQn                                       = 29,
   
    UART2_IRQn                                      = 32,
    UART3_IRQn                                      = 33,
    QSPI_IRQn                                       = 35,
    I2C0_IRQn                                       = 36,
    EXTI4_IRQn                                      = 37,
    EXTI5_IRQn                                      = 38,
    TIM0_6_IRQn                                     = 39,
    TIM0_7_IRQn                                     = 40,
    DCMI_IRQn                                       = 42,
   
    QR_IRQn                                         = 46,
	GPU_IRQn										= 47,

	AWD_IRQn										= 49,
	DAC_IRQn										= 50,
	SPI5_IRQn										= 51
} IRQn_Type;

typedef struct
{
    __IO uint32_t LoadCount;
    __I  uint32_t CurrentValue;
    __IO uint32_t ControlReg;
    __IO  uint32_t EOI;
    __I  uint32_t IntStatus;
} TIM_TypeDef;

#define MHSCPU_PERIPH_BASE (0x40000000UL)
#define SCS_BASE           (0xE000E000UL)    
#define MHSCPU_AHB_BASE    (MHSCPU_PERIPH_BASE)
#define MHSCPU_APB0_BASE   (MHSCPU_PERIPH_BASE + 0x10000)

#define SYSCTRL_BASE (MHSCPU_APB0_BASE + 0xF000)
#define UART0_BASE   (MHSCPU_APB0_BASE + 0x6000)
#define UART1_BASE   (MHSCPU_APB0_BASE + 0x7000)
#define GPIO_BASE    (MHSCPU_APB0_BASE + 0xD000)
#define TIMER_BASE   (MHSCPU_APB0_BASE + 0x3000)
#define SCB_BASE     (SCS_BASE +  0x0D00UL)
#define NVIC_BASE    (SCS_BASE +  0x0100UL)  

#define SYSCTRL        ((SYSCTRL_TypeDef *) SYSCTRL_BASE)
#define UART0          ((UART_TypeDef *) UART0_BASE)
#define UART1          ((UART_TypeDef *) UART1_BASE)
#define GPIO           ((GPIO_MODULE_TypeDef *)GPIO_BASE)
#define SCB            ((SCB_Type *)SCB_BASE)
#define NVIC           ((NVIC_Type*)NVIC_BASE)   /*!< NVIC configuration struct          */
#define TIM0           ((TIM_TypeDef *) TIMER_BASE)
#define GPIOA          ((GPIO_TypeDef *)GPIO_BASE)
#define GPIOB          ((GPIO_TypeDef *)(GPIO_BASE + 0x0010))
#define GPIOC          ((GPIO_TypeDef *)(GPIO_BASE + 0x0020))
#define GPIOD          ((GPIO_TypeDef *)(GPIO_BASE + 0x0030))
#define GPIOE          ((GPIO_TypeDef *)(GPIO_BASE + 0x0040))
#define GPIOF          ((GPIO_TypeDef *)(GPIO_BASE + 0x0050))

#define GPIO_PA_ALT    *(uint32_t*)(GPIO_BASE+0x180)
#define GPIO_PB_ALT    *(uint32_t*)(GPIO_BASE+0x184)
#define GPIO_PA_IODR   *(unsigned int*)(GPIO_BASE+0x00)
#define GPIO_PA_BSRR   *(unsigned int*)(GPIO_BASE+0x04)
#define GPIO_PA_OEN    *(unsigned int*)(GPIO_BASE+0x08)
#define GPIO_PA_PUE    *(unsigned int*)(GPIO_BASE+0x0C)

#define end            ((uint8_t)0x56)
#define nop            ((uint8_t)0x55)
#define SOH            ((uint8_t)0x01) 
#define STX            ((uint8_t)0x02)  
#define EOT            ((uint8_t)0x04)  
#define ACK            ((uint8_t)0x06)  
#define NAK            ((uint8_t)0x15)  
#define CAN            ((uint32_t)0x18)


#define RING_BUFFER_SIZE		1024

typedef struct RingBuffer
{
	uint8_t buffer[RING_BUFFER_SIZE];
	uint32_t put_index, get_index;
	uint32_t count;
}RingBufferTypeDef;



#define __STATIC_INLINE  static __inline
#define BIT(n)      (1UL << (n))



#endif //INC_H
