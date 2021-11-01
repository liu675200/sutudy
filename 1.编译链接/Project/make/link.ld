
SECTIONS
{
. = 0xD0024000;
.text : { * (.text)}

.data : {* (.data)}

bss_start = .;
.bss : {* (.bss)}
bss_end = .;

}


SECTIONS          /*设置段链接分布*/
{
	.text :
	{
		__text_start__ = .;        /*记录段开始地址*/
		*(.text*)            /*将所有.o文件的(.text*)段汇集成一段*/
		KEEP(*(.init))
		KEEP(*(.fini))
		*(.rodata*)
		__text_end__ = .;        /*记录段结束位置*/
	}                    /*保存内存属性*/

	/*加一条AT使其虚拟地址直接连在.text段后面*/
	.data :AT (__text_end__)
	{
		__data_start__ = .;    
        /*记录data段实际物理地址，启动后需将数据复制到实际地址中*/
		*(.data*)
		/* All data end */
		__data_end__ = .;
	}
	
	/*禁止bss段产生内容，直接连在data段后面*/
	.bss (NOLOAD):
	{
		. = ALIGN(4);    /*四字节对齐*/
		__bss_start__ = .;    /*bss段初始化后被分配的地址空间的地址*/
		*(.bss*)
		. = ALIGN(4);    /*四字节对齐*/
		__bss_end__ = .;
	}
} 
		




