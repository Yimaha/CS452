
.global interrupt_handler
interrupt_handler:
	// First off, have to save all the user level registers
	// because interrupts can happen at any time

	// Save x0 and x1, then save the stack pointer in x0
	stp x0, x1, [sp, -16]
	mrs x0, SP_EL0

	// Save user registers
	sub x0, x0, 17*16
	// stp x0, x1, [x2, 0*16]
	stp x2, x3, [x0, 1*16]
	stp x4, x5, [x0, 2*16]
	stp x6, x7, [x0, 3*16]
	stp x8, x9, [x0, 4*16]
	stp x10, x11, [x0, 5*16]
	stp x12, x13, [x0, 6*16]
	stp x14, x15, [x0, 7*16]
	stp x16, x17, [x0, 8*16]
	stp x18, x19, [x0, 9*16]
	stp x20, x21, [x0, 10*16]
	stp x22, x23, [x0, 11*16]
	stp x24, x25, [x0, 12*16]
	stp x26, x27, [x0, 13*16]
	stp x28, x29, [x0, 14*16]

	mrs x1, SPSR_EL1
	stp x30, x1, [x0, 15*16]

	// Fill the 'data' field with 1 (representing an interrupt)
	mov x1, 0b1
	mrs x2, ELR_EL1
	stp x1, x2, [x0, 16*16]

	// okay cool, now we can bring back x2 and x3
	mov x2, x0
	ldp x0, x1, [sp, -16]
	stp x0, x1, [x2, 0*16]
	mov x0, x2
	// At this point, x0 should contain the task stack pointer

	// Load kernel registers
	ldp xzr, x1, [sp, 0*16]
	ldp x2, x3, [sp, 1*16]
	ldp x4, x5, [sp, 2*16]
	ldp x30, x19, [sp, 9*16]
	ldp x20, x21, [sp, 10*16]
	ldp x22, x23, [sp, 11*16]
	ldp x24, x25, [sp, 12*16]
	ldp x26, x27, [sp, 13*16]
	ldp x28, x29, [sp, 14*16]
	add sp, sp, 17*16

	ret

.global read_esr
read_esr:
	mrs x0, ESR_EL1
	ret
