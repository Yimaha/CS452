
// print a char to the display
.macro debug_print, c
	sub sp, sp, 16
	str x30, [sp]
	str x0, [sp, 8]
	mov x0, \c
	bl print
	ldr x0, [sp, 8]
	ldr x30, [sp]
	add sp, sp, 16
.endm

.macro debug_val_print, c
	sub sp, sp, 16
	str x30, [sp]
	str x0, [sp, 8]
	mov x0, \c
	bl val_print
	ldr x0, [sp, 8]
	ldr x30, [sp]
	add sp, sp, 16
.endm


.global check_sp
check_sp:
	// the goal is just to set the sp to the right value
	// debug_print x0 // print
	mov x0, sp
	debug_val_print x0
	ret



.global first_el0_entry
first_el0_entry:

	// x0 = function pc
	// x1 = stack frame
	// Switch to EL0

	msr SP_EL0, x0 // assuming stack address is passed in x0

	// Save kernel registers
	sub sp, sp, 11*16

	stp x0, x1, [sp, 0*16]
	stp x2, x3, [sp, 1*16]
	stp x4, x5, [sp, 2*16]
	stp x6, x7, [sp, 3*16]
	stp x8, x9, [sp, 4*16]
	stp x10, x11, [sp, 5*16]
	stp x12, x13, [sp, 6*16]
	stp x14, x15, [sp, 7*16]
	stp x16, x17, [sp, 8*16]
	stp x18, x19, [sp, 9*16]
	stp x20, x21, [sp, 10*16]
	stp x22, x23, [sp, 11*16]
	stp x24, x25, [sp, 12*16]
	stp x26, x27, [sp, 13*16]
	stp x28, x29, [sp, 14*16]
	stp x30, xzr, [sp, 15*16]
	// does not need to save sp since sp is banked

    msr elr_el1, x1 // we need another thing when the function get back since it is bidirectional, but this will do for now

	mov x0, #0b1111000000
	msr SPSR_EL1, x0

	eret // ready to jump

