	.arch armv8-a+crc
	.file	"main.cc"
	.text
.Ltext0:
	.cfi_sections	.debug_frame
	.file 0 "/u2/t28cai/cs452/A1/k1" "main.cc"
	.align	2
	.global	main
	.type	main, %function
main:
.LFB0:
	.file 1 "main.cc"
	.loc 1 19 12
	.cfi_startproc
	mov	x12, 11024
	sub	sp, sp, x12
	.cfi_def_cfa_offset 11024
	stp	x29, x30, [sp]
	.cfi_offset 29, -11024
	.cfi_offset 30, -11016
	mov	x29, sp
	.loc 1 24 10
	add	x0, sp, 4096
	add	x0, x0, 1024
	stp	xzr, xzr, [x0, -96]
	add	x0, sp, 4096
	add	x0, x0, 944
	mov	x1, 5984
	mov	x2, x1
	mov	w1, 0
	bl	memset
	.loc 1 25 10
	str	xzr, [sp, 24]
	str	xzr, [sp, 32]
	add	x0, sp, 40
	mov	x1, 4984
	mov	x2, x1
	mov	w1, 0
	bl	memset
	.loc 1 28 24
	mov	w0, 35
	add	x1, sp, 8192
	strb	w0, [x1, 1831]
	.loc 1 30 8
	add	x0, sp, 4096
	add	x0, x0, 928
	bl	foo
	.loc 1 31 10
	add	x0, sp, 4096
	add	x0, x0, 928
	bl	setSP
	.loc 1 32 8
	bl	bar
	.loc 1 54 12
	mov	w0, 0
	.loc 1 55 1
	ldp	x29, x30, [sp]
	mov	x12, 11024
	add	sp, sp, x12
	.cfi_restore 29
	.cfi_restore 30
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
.Letext0:
	.file 2 "context_switch.h"
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.4byte	0x10d
	.2byte	0x5
	.byte	0x1
	.byte	0x8
	.4byte	.Ldebug_abbrev0
	.uleb128 0x5
	.4byte	.LASF15
	.byte	0x21
	.4byte	.LASF0
	.4byte	.LASF1
	.8byte	.Ltext0
	.8byte	.Letext0-.Ltext0
	.4byte	.Ldebug_line0
	.uleb128 0x1
	.byte	0x1
	.byte	0x6
	.4byte	.LASF2
	.uleb128 0x1
	.byte	0x2
	.byte	0x5
	.4byte	.LASF3
	.uleb128 0x6
	.byte	0x4
	.byte	0x5
	.string	"int"
	.uleb128 0x1
	.byte	0x8
	.byte	0x5
	.4byte	.LASF4
	.uleb128 0x1
	.byte	0x1
	.byte	0x8
	.4byte	.LASF5
	.uleb128 0x1
	.byte	0x2
	.byte	0x7
	.4byte	.LASF6
	.uleb128 0x1
	.byte	0x4
	.byte	0x7
	.4byte	.LASF7
	.uleb128 0x1
	.byte	0x8
	.byte	0x7
	.4byte	.LASF8
	.uleb128 0x1
	.byte	0x8
	.byte	0x5
	.4byte	.LASF9
	.uleb128 0x1
	.byte	0x10
	.byte	0x4
	.4byte	.LASF10
	.uleb128 0x7
	.4byte	.LASF16
	.uleb128 0x1
	.byte	0x1
	.byte	0x8
	.4byte	.LASF11
	.uleb128 0x8
	.string	"bar"
	.byte	0x2
	.byte	0x5
	.byte	0x11
	.uleb128 0x9
	.4byte	.LASF12
	.byte	0x2
	.byte	0x6
	.byte	0x11
	.4byte	0x9a
	.uleb128 0x2
	.4byte	0x9a
	.byte	0
	.uleb128 0xa
	.byte	0x8
	.4byte	0x79
	.uleb128 0xb
	.string	"foo"
	.byte	0x2
	.byte	0x4
	.byte	0x11
	.4byte	0xb2
	.uleb128 0x2
	.4byte	0x9a
	.byte	0
	.uleb128 0xc
	.4byte	.LASF17
	.byte	0x1
	.byte	0x13
	.byte	0x5
	.4byte	0x3c
	.8byte	.LFB0
	.8byte	.LFE0-.LFB0
	.uleb128 0x1
	.byte	0x9c
	.4byte	0xf2
	.uleb128 0x3
	.4byte	.LASF13
	.byte	0x18
	.4byte	0xf2
	.uleb128 0x3
	.byte	0x91
	.sleb128 -6000
	.uleb128 0x3
	.4byte	.LASF14
	.byte	0x19
	.4byte	0x103
	.uleb128 0x4
	.byte	0x91
	.sleb128 -11000
	.byte	0
	.uleb128 0xd
	.4byte	0x79
	.4byte	0x103
	.uleb128 0x4
	.4byte	0x5f
	.2byte	0x176f
	.byte	0
	.uleb128 0xe
	.4byte	0x79
	.uleb128 0x4
	.4byte	0x5f
	.2byte	0x1387
	.byte	0
	.byte	0
	.section	.debug_abbrev,"",@progbits
.Ldebug_abbrev0:
	.uleb128 0x1
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.byte	0
	.byte	0
	.uleb128 0x2
	.uleb128 0x5
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x3
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 1
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0x21
	.sleb128 10
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x4
	.uleb128 0x21
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0x5
	.byte	0
	.byte	0
	.uleb128 0x5
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x1f
	.uleb128 0x1b
	.uleb128 0x1f
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.uleb128 0x10
	.uleb128 0x17
	.byte	0
	.byte	0
	.uleb128 0x6
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0
	.byte	0
	.uleb128 0x7
	.uleb128 0x3b
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.byte	0
	.byte	0
	.uleb128 0x8
	.uleb128 0x2e
	.byte	0
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x3c
	.uleb128 0x19
	.byte	0
	.byte	0
	.uleb128 0x9
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x3c
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xa
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xb
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x3c
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xc
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x7c
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xd
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xe
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.byte	0
	.section	.debug_aranges,"",@progbits
	.4byte	0x2c
	.2byte	0x2
	.4byte	.Ldebug_info0
	.byte	0x8
	.byte	0
	.2byte	0
	.2byte	0
	.8byte	.Ltext0
	.8byte	.Letext0-.Ltext0
	.8byte	0
	.8byte	0
	.section	.debug_line,"",@progbits
.Ldebug_line0:
	.section	.debug_str,"MS",@progbits,1
.LASF13:
	.string	"kernel_stack"
.LASF15:
	.string	"GNU C++17 11.3.1 20220712 -mcpu=cortex-a72 -mstrict-align -mgeneral-regs-only -mlittle-endian -mabi=lp64 -g -ffreestanding -fno-builtin -fno-rtti -fno-exceptions"
.LASF14:
	.string	"user_stack"
.LASF16:
	.string	"decltype(nullptr)"
.LASF5:
	.string	"unsigned char"
.LASF8:
	.string	"long unsigned int"
.LASF6:
	.string	"short unsigned int"
.LASF17:
	.string	"main"
.LASF7:
	.string	"unsigned int"
.LASF11:
	.string	"char"
.LASF9:
	.string	"long long int"
.LASF3:
	.string	"short int"
.LASF4:
	.string	"long int"
.LASF12:
	.string	"setSP"
.LASF10:
	.string	"long double"
.LASF2:
	.string	"signed char"
	.section	.debug_line_str,"MS",@progbits,1
.LASF0:
	.string	"main.cc"
.LASF1:
	.string	"/u2/t28cai/cs452/A1/k1"
	.ident	"GCC: (Arm GNU Toolchain 11.3.Rel1) 11.3.1 20220712"
