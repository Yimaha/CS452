ENTRY(_start)           /* ELF entry symbol - no strictly needed */

/* mark BSS segment */
__bss_start = ADDR(.bss);
__bss_end   = ADDR(.bss) + SIZEOF(.bss);
__bss_size = (__bss_end - __bss_start) >> 3;

SECTIONS {
  . = 0x80000;          /* load address */
	.text.boot : {        /* boot code must start at 0x80000 */
		KEEP(*(.text.boot))
		*(.text*)
	}

	/* mark constructor initialization array */
	.init_array : ALIGN(4) {
		__init_array_start = .;
			*(.init_array)
			*(SORT_BY_INIT_PRIORITY(.init_array.*))
		__init_array_end = .;
	}


  . = 0x200000;
	.rodata : {
		*(.rodata*)
	}
}
