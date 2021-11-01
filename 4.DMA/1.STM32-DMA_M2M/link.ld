MEMORY
{
  FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 0x10000   
  RAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 0x10000
  /*定义内存布局与大小*/
  
}

_estack = 0x20010000;    /*指定内存外栈指针地址*/

SECTIONS          /*设置段链接分布*/
{
	.text :
	{
		. = ALIGN(4);    /*四字节对齐*/
		__text_start__ = .;        /*记录段开始地址*/
		KEEP(start.o(*.cortex_vectors))/*优先加载start.o中text段*/
		KEEP(start.o(.text.Reset_Handler))
		*(.text*)            /*将所有.o文件的(.text*)段汇集成一段*/
		KEEP(*(.init))
		KEEP(*(.fini))
		*(.rodata*)
		. = ALIGN(4);    /*四字节对齐*/
		__text_end__ = .;        /*记录段结束位置*/
	}> FLASH                    /*保存内存属性*/

	/*加一条AT使其虚拟地址直接连在.text段后面*/
	.data :AT (__text_end__)
	{
		. = ALIGN(4);    /*四字节对齐*/
		__data_start__ = .;    
        /*记录data段实际物理地址，启动后需将数据复制到实际地址中*/
		*(.data*)

		. = ALIGN(4);    /*四字节对齐*/
		/* All data end */
		__data_end__ = .;
	}> RAM
	
	/*禁止bss段产生内容，直接连在data段后面*/
	.bss (NOLOAD):
	{
		. = ALIGN(4);    /*四字节对齐*/
		__bss_start__ = .;    /*bss段初始化后被分配的地址空间的地址*/
		*(.bss*)
		. = ALIGN(4);    /*四字节对齐*/
		__bss_end__ = .;
	}> RAM
} 
		
