ENTRY(_start)           /* ELF entry symbol - no strictly needed */

SECTIONS {
  . = 0x80000;          /* load address */
	.text.boot : {        /* boot code must start at 0x80000 */
		KEEP(*(.text.boot))
	}
}
