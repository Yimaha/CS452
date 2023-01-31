
# Much of this context switch was assisted by Mike Krinkin's excellent blog post on ARMv8:
# https://krinkinmu.github.io/2021/01/10/aarch64-interrupt-handling.html

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

.macro save_regs
	// Save kernel registers
	sub sp, sp, 16*16

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
.endm

.macro load_regs

	// Load kernel registers
	ldp x0, x1, [sp, 0*16]
	ldp x2, x3, [sp, 1*16]
	ldp x4, x5, [sp, 2*16]
	ldp x6, x7, [sp, 3*16]
	ldp x8, x9, [sp, 4*16]
	ldp x10, x11, [sp, 5*16]
	ldp x12, x13, [sp, 6*16]
	ldp x14, x15, [sp, 7*16]
	ldp x16, x17, [sp, 8*16]
	ldp x18, x19, [sp, 9*16]
	ldp x20, x21, [sp, 10*16]
	ldp x22, x23, [sp, 11*16]
	ldp x24, x25, [sp, 12*16]
	ldp x26, x27, [sp, 13*16]
	ldp x28, x29, [sp, 14*16]
	ldp x30, xzr, [sp, 15*16]
	
	add sp, sp, 16*16
.endm


.global first_el0_entry
first_el0_entry:

	// x0 = function pc
	// x1 = stack frame
	// Switch to EL0

	msr SP_EL0, x0 // assuming stack address is passed in x0
    msr elr_el1, x1 // we need another thing when the function get back since it is bidirectional, but this will do for now

	// save registers
	save_regs

	mov x0, #0b1111000000
	msr SPSR_EL1, x0

	eret // ready to jump


.global to_kernel
to_kernel:


	save_regs

	// does not need to save sp since sp is banked
	svc 0x1

.global handle_syscall
handle_syscall:

	load_regs

	mrs x0, SP_EL0
	ret
	
.global to_user
to_user:

	// x0 = stack address
	// x1 = return 8 bytes
	// Switch to EL0

	msr SP_EL0, x0 // assuming stack address is passed in x0
	ldr x0, =handle_return_to_user
	msr elr_el1, x0 // we need another thing when the function get back since it is bidirectional, but this will do for now
	
	// no need to worry about function pc anymore
	save_regs

	// need storing and restore
	mov x0, #0b1111000000
	msr SPSR_EL1, x0

	mov x0, x1 // prepare x1 since x1 is the returned value

	eret // ready to jump

// this is a private function
// unlike handle_syscall which is needed by the exception table
// there is no reason why this label exists outside of this file
handle_return_to_user:

	ldp xzr, x1, [sp, 0*16]
	ldp x2, x3, [sp, 1*16]
	ldp x4, x5, [sp, 2*16]
	ldp x6, x7, [sp, 3*16]
	ldp x8, x9, [sp, 4*16]
	ldp x10, x11, [sp, 5*16]
	ldp x12, x13, [sp, 6*16]
	ldp x14, x15, [sp, 7*16]
	ldp x16, x17, [sp, 8*16]
	ldp x18, x19, [sp, 9*16]
	ldp x20, x21, [sp, 10*16]
	ldp x22, x23, [sp, 11*16]
	ldp x24, x25, [sp, 12*16]
	ldp x26, x27, [sp, 13*16]
	ldp x28, x29, [sp, 14*16]
	ldp x30, xzr, [sp, 15*16]

	add sp, sp, 16*16

	// x0 should already be filled
	ret
