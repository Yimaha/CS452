
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

.global foo
foo:
	debug_print x0 // print
	ret

.global bar
bar:
	debug_print sp // print
	ret

.global setSP
setSP:
	// the goal is just to set the sp to the right value
	// debug_print x0 // print
	mov sp, x0
	ret

.global el0_entry
el0_entry:

	msr SP_EL0, x0 // assuming stack address is passed in x0, save it

    mrs x0, CurrentEL
    cmp x0, #0b100  // checks if we are in EL1
    beq in_el1
    blo in_el0
	cmp x0, #0b1000 // checks if we are in EL2
	beq in_el2

in_el3:
in_el2:
in_el1:

    // Switch to EL0
	adr x0, in_el0
	msr ELR_EL1, x0

	mov x0, #0b1111000000
	msr SPSR_EL1, x0

	eret

in_el0:
	// already done, maybe
	ret
	
