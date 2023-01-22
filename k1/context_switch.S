
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

