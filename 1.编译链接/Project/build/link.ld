MEMORY
{
  FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 0x10000   /* 256k */
  RAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 0x10000   /* 32k,总40k,其中8K留空 */
}

_estack = 0x20010000;

SECTIONS
{
	.text :
	{
		__text_start__ = .;
		KEEP(start.o(*.cortex_vectors))
		KEEP(start.o(.text.Reset_Handler))
		*(.text*)
		KEEP(*(.init))
		KEEP(*(.fini))
		*(.rodata*)
		__text_end__ = .;
	}> FLASH

	__text = .;

	/*加一条AT使其虚拟地址直接连在.text段后面，*/
	.data :AT (__text)
	{
		__data_start__ = .;
		*(vtable)
		*(.data*)

		. = ALIGN(4);
		/* preinit data */
		PROVIDE_HIDDEN (__preinit_array_start = .);
		KEEP(*(.preinit_array))
		PROVIDE_HIDDEN (__preinit_array_end = .);

		. = ALIGN(4);
		/* init data */
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP(*(SORT(.init_array.*)))
		KEEP(*(.init_array))
		PROVIDE_HIDDEN (__init_array_end = .);

		. = ALIGN(4);
		/* finit data */
		PROVIDE_HIDDEN (__fini_array_start = .);
		KEEP(*(SORT(.fini_array.*)))
		KEEP(*(.fini_array))
		PROVIDE_HIDDEN (__fini_array_end = .);

		. = ALIGN(4);
		/* All data end */
		__data_end__ = .;
	}> RAM
	

	/*禁止bss段产生内容，直接连在data段后面*/
	.bss (NOLOAD):
	{
		__bss_start__ = .;
		*(.bss*)
		__bss_end__ = .;
	}> RAM
	
} 
		
