MEMORY
{

  FLASH (rx) : ORIGIN = 0x01001000, LENGTH = 0x4000  

  RAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 0x10000   

}

_estack = 0x20010000;

SECTIONS

{

	.text :
	{
		. = ALIGN(4);
		__text_start__ = .;
		KEEP(*(*.cortex_vectors))
		KEEP(*(*.Reset_Handler))
		*(.text*)
		KEEP(*(.init))
		KEEP(*(.fini))
		*(.rodata*)
		__text_end__ = .;
	}> FLASH


	.data : AT (__text_end__)
	{
		. = ALIGN(4);
		__data_start__ = .;
		*(.data*)
		. = ALIGN(4);
		/* All data end */
		__data_end__ = .;
	}> RAM

	

	.bss (NOLOAD) : 

	{
		__bss_start__ = .;
		*(.bss*)
		__bss_end__ = .;
	}> RAM

} 