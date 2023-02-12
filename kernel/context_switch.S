
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
	// Save registers
	sub sp, sp, 17*16

	stp x0, x1, [sp, 0*16]
	stp x2, x3, [sp, 1*16]
	stp x4, x5, [sp, 2*16]
	stp x30, x19, [sp, 9*16]
	stp x20, x21, [sp, 10*16]
	stp x22, x23, [sp, 11*16]
	stp x24, x25, [sp, 12*16]
	stp x26, x27, [sp, 13*16]
	stp x28, x29, [sp, 14*16]

	// does not need to save sp since sp is banked
.endm

.macro load_regs

	// Load kernel registers
	ldp x0, x1, [sp, 0*16]
	ldp x2, x3, [sp, 1*16]
	ldp x4, x5, [sp, 2*16]
	ldp x30, x19, [sp, 9*16]
	ldp x20, x21, [sp, 10*16]
	ldp x22, x23, [sp, 11*16]
	ldp x24, x25, [sp, 12*16]
	ldp x26, x27, [sp, 13*16]
	ldp x28, x29, [sp, 14*16]
	
	add sp, sp, 17*16
.endm


.global first_el0_entry
first_el0_entry:

	// x0 = function pc
	// x1 = stack frame
	// Switch to EL0

	msr SP_EL0, x0 // assuming stack address is passed in x0
	msr elr_el1, x1 // we need another thing when the function get back since it is bidirectional, but this will do for now

	// save kernel registers
	save_regs

	// mov x0, #0b1111000000
	msr SPSR_EL1, xzr

	eret // ready to jump


.global to_kernel
to_kernel:
	
	save_regs

	// does not need to save sp since sp is banked in SP_EL0
	svc 0x1

.global handle_syscall
handle_syscall:

	load_regs

	mrs x0, SP_EL0
	ret
	
.global to_user
to_user:

	// x0 = syscall return value
	// x1 = stack address of user task
	// x2 = SPSR_EL1
	// Switch to EL0

	msr SP_EL0, x1 // assuming stack address is passed in x1
	ldr x1, =handle_return_to_user
	msr elr_el1, x1 // we need another thing when the function get back since it is bidirectional, but this will do for now
	
	// no need to worry about function pc anymore
	save_regs

	// need storing and restore
	// orr x2, x2, #0b1111000000
	msr SPSR_EL1, x2

	eret // ready to jump

.global to_user_interrupted
to_user_interrupted:

	// x0 = stack address of user task
	// x1 = SPSR_EL1
	// x2 = program counter
	// Switch to EL0

	save_regs

	msr SPSR_EL1, x1
	msr elr_el1, x2

	ldp x2, x3, [x0, 1*16]
	ldp x4, x5, [x0, 2*16]
	ldp x6, x7, [x0, 3*16]
	ldp x8, x9, [x0, 4*16]
	ldp x10, x11, [x0, 5*16]
	ldp x12, x13, [x0, 6*16]
	ldp x14, x15, [x0, 7*16]
	ldp x16, x17, [x0, 8*16]
	ldp x18, x19, [x0, 9*16]
	ldp x20, x21, [x0, 10*16]
	ldp x22, x23, [x0, 11*16]
	ldp x24, x25, [x0, 12*16]
	ldp x26, x27, [x0, 13*16]
	ldp x28, x29, [x0, 14*16]
	ldp x30, xzr, [x0, 15*16]

	add x0, x0, 17*16
	msr SP_EL0, x0 // assuming stack address is passed in x0

	ldp x0, x1, [x0, -17*16]

	eret // ready to jump


// these are private functions
// unlike handle_syscall which is needed by the exception table
// there is no reason why these label exists outside of this file
handle_return_to_user:

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

	// x0 should already be filled
	ret


# I want to set up MAIR such that the entry is following
# |outer write back transient, inner write back transient|outer write back non-transient, inner write back non-transient|nGnRnE|not cachable|
#define MAIR_TABLE  0b01110111111111110000000001000100
# we want to run 32 bit addressing since we won't go above 4gb even with all the device address
#define DISABLE_TTBR1_EL1 (1 << 23)
#define TTBR0_EL1_OUTER_SHARABLE (2 << 12)
#define TTBR0_EL1_ORGN_0 (1<<10)
#define TTBR0_EL1_IRGN_0 (1<<8)
#define T0SZ 32
#define TCR_ENABLE (DISABLE_TTBR1_EL1 | TTBR0_EL1_OUTER_SHARABLE | TTBR0_EL1_ORGN_0 | TTBR0_EL1_IRGN_0 | T0SZ)

.global mmu_registers
mmu_registers:
    msr TTBR0_EL1, x0
    ldr x1, =MAIR_TABLE
    msr MAIR_EL1, x1
    ldr x1, =TCR_ENABLE
    msr TCR_EL1, x1
    ISB
    MRS x0, SCTLR_EL1
    ORR x0, x0, #1
    MSR SCTLR_EL1, x0
    ISB
	ret