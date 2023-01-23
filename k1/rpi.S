	.arch armv8-a+crc
	.file	"rpi.cc"
	.text
.Ltext0:
	.cfi_sections	.debug_frame
	.file 0 "/u2/t28cai/cs452/A1/k1" "rpi.cc"
	.section	.rodata
	.align	3
	.type	_ZL9MMIO_BASE, %object
	.size	_ZL9MMIO_BASE, 8
_ZL9MMIO_BASE:
	.xword	4261412864
	.align	3
	.type	_ZL9GPIO_BASE, %object
	.size	_ZL9GPIO_BASE, 8
_ZL9GPIO_BASE:
	.xword	4263510016
	.align	3
	.type	_ZL8AUX_BASE, %object
	.size	_ZL8AUX_BASE, 8
_ZL8AUX_BASE:
	.xword	4263596032
	.text
	.align	2
	.type	_ZL4gpiov, %function
_ZL4gpiov:
.LFB0:
	.file 1 "rpi.cc"
	.loc 1 64 36
	.cfi_startproc
	.loc 1 65 38
	mov	x0, 4263510016
	.loc 1 66 1
	ret
	.cfi_endproc
.LFE0:
	.size	_ZL4gpiov, .-_ZL4gpiov
	.align	2
	.type	_ZL3auxv, %function
_ZL3auxv:
.LFB1:
	.loc 1 68 34
	.cfi_startproc
	.loc 1 69 36
	mov	x0, 20480
	movk	x0, 0xfe21, lsl 16
	.loc 1 70 1
	ret
	.cfi_endproc
.LFE1:
	.size	_ZL3auxv, .-_ZL3auxv
	.align	2
	.type	_ZL3spii, %function
_ZL3spii:
.LFB2:
	.loc 1 72 45
	.cfi_startproc
	sub	sp, sp, #16
	.cfi_def_cfa_offset 16
	str	w0, [sp, 12]
	.loc 1 73 23
	ldr	w0, [sp, 12]
	cmp	w0, 0
	bne	.L6
	.loc 1 73 23 is_stmt 0 discriminator 1
	mov	x0, 20608
	movk	x0, 0xfe21, lsl 16
	.loc 1 73 91 is_stmt 1 discriminator 1
	b	.L8
.L6:
	.loc 1 73 23 discriminator 2
	mov	x0, 20672
	movk	x0, 0xfe21, lsl 16
.L8:
	.loc 1 74 1 discriminator 5
	add	sp, sp, 16
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE2:
	.size	_ZL3spii, .-_ZL3spii
	.section	.rodata
	.align	2
	.type	_ZL10GPIO_INPUT, %object
	.size	_ZL10GPIO_INPUT, 4
_ZL10GPIO_INPUT:
	.zero	4
	.align	2
	.type	_ZL11GPIO_OUTPUT, %object
	.size	_ZL11GPIO_OUTPUT, 4
_ZL11GPIO_OUTPUT:
	.word	1
	.align	2
	.type	_ZL11GPIO_ALTFN0, %object
	.size	_ZL11GPIO_ALTFN0, 4
_ZL11GPIO_ALTFN0:
	.word	4
	.align	2
	.type	_ZL11GPIO_ALTFN1, %object
	.size	_ZL11GPIO_ALTFN1, 4
_ZL11GPIO_ALTFN1:
	.word	5
	.align	2
	.type	_ZL11GPIO_ALTFN2, %object
	.size	_ZL11GPIO_ALTFN2, 4
_ZL11GPIO_ALTFN2:
	.word	6
	.align	2
	.type	_ZL11GPIO_ALTFN3, %object
	.size	_ZL11GPIO_ALTFN3, 4
_ZL11GPIO_ALTFN3:
	.word	7
	.align	2
	.type	_ZL11GPIO_ALTFN4, %object
	.size	_ZL11GPIO_ALTFN4, 4
_ZL11GPIO_ALTFN4:
	.word	3
	.align	2
	.type	_ZL11GPIO_ALTFN5, %object
	.size	_ZL11GPIO_ALTFN5, 4
_ZL11GPIO_ALTFN5:
	.word	2
	.align	2
	.type	_ZL9GPIO_NONE, %object
	.size	_ZL9GPIO_NONE, 4
_ZL9GPIO_NONE:
	.zero	4
	.align	2
	.type	_ZL8GPIO_PUP, %object
	.size	_ZL8GPIO_PUP, 4
_ZL8GPIO_PUP:
	.word	1
	.align	2
	.type	_ZL8GPIO_PDP, %object
	.size	_ZL8GPIO_PDP, 4
_ZL8GPIO_PDP:
	.word	2
	.text
	.align	2
	.type	_ZL10setup_gpiojjj, %function
_ZL10setup_gpiojjj:
.LFB3:
	.loc 1 92 75
	.cfi_startproc
	stp	x29, x30, [sp, -64]!
	.cfi_def_cfa_offset 64
	.cfi_offset 29, -64
	.cfi_offset 30, -56
	mov	x29, sp
	str	x19, [sp, 16]
	.cfi_offset 19, -48
	str	w0, [sp, 44]
	str	w1, [sp, 40]
	str	w2, [sp, 36]
	.loc 1 93 12
	ldr	w1, [sp, 44]
	mov	w0, 52429
	movk	w0, 0xcccc, lsl 16
	umull	x0, w1, w0
	lsr	x0, x0, 32
	lsr	w0, w0, 3
	str	w0, [sp, 60]
	.loc 1 94 25
	ldr	w2, [sp, 44]
	mov	w0, 52429
	movk	w0, 0xcccc, lsl 16
	umull	x0, w2, w0
	lsr	x0, x0, 32
	lsr	w1, w0, 3
	mov	w0, w1
	lsl	w0, w0, 2
	add	w0, w0, w1
	lsl	w0, w0, 1
	sub	w1, w2, w0
	.loc 1 94 12
	mov	w0, w1
	lsl	w0, w0, 1
	add	w0, w0, w1
	str	w0, [sp, 56]
	.loc 1 95 25
	bl	_ZL4gpiov
	mov	x1, x0
	.loc 1 95 39
	ldr	w0, [sp, 60]
	ldr	w0, [x1, x0, lsl 2]
	str	w0, [sp, 52]
	.loc 1 96 18
	ldr	w0, [sp, 56]
	mov	w1, 7
	lsl	w0, w1, w0
	.loc 1 96 13
	mvn	w0, w0
	.loc 1 96 10
	ldr	w1, [sp, 52]
	and	w0, w1, w0
	str	w0, [sp, 52]
	.loc 1 97 23
	ldr	w0, [sp, 56]
	ldr	w1, [sp, 40]
	lsl	w0, w1, w0
	.loc 1 97 10
	ldr	w1, [sp, 52]
	orr	w0, w1, w0
	str	w0, [sp, 52]
	.loc 1 98 23
	ldr	w19, [sp, 52]
	.loc 1 98 7
	bl	_ZL4gpiov
	mov	x1, x0
	.loc 1 98 23
	ldr	w0, [sp, 60]
	str	w19, [x1, x0, lsl 2]
	.loc 1 100 9
	ldr	w0, [sp, 44]
	lsr	w0, w0, 4
	str	w0, [sp, 60]
	.loc 1 101 16
	ldr	w0, [sp, 44]
	and	w0, w0, 15
	.loc 1 101 9
	lsl	w0, w0, 1
	str	w0, [sp, 56]
	.loc 1 102 16
	bl	_ZL4gpiov
	mov	x1, x0
	.loc 1 102 10
	ldr	w0, [sp, 60]
	add	x0, x0, 56
	lsl	x0, x0, 2
	add	x0, x1, x0
	ldr	w0, [x0, 4]
	str	w0, [sp, 52]
	.loc 1 103 18
	ldr	w0, [sp, 56]
	mov	w1, 3
	lsl	w0, w1, w0
	.loc 1 103 13
	mvn	w0, w0
	.loc 1 103 10
	ldr	w1, [sp, 52]
	and	w0, w1, w0
	str	w0, [sp, 52]
	.loc 1 104 24
	ldr	w0, [sp, 56]
	ldr	w1, [sp, 36]
	lsl	w0, w1, w0
	.loc 1 104 10
	ldr	w1, [sp, 52]
	orr	w0, w1, w0
	str	w0, [sp, 52]
	.loc 1 105 34
	ldr	w19, [sp, 52]
	.loc 1 105 7
	bl	_ZL4gpiov
	mov	x1, x0
	.loc 1 105 34
	ldr	w0, [sp, 60]
	add	x0, x0, 56
	lsl	x0, x0, 2
	add	x0, x1, x0
	str	w19, [x0, 4]
	.loc 1 106 1
	nop
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 64
	.cfi_restore 30
	.cfi_restore 29
	.cfi_restore 19
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE3:
	.size	_ZL10setup_gpiojjj, .-_ZL10setup_gpiojjj
	.align	2
	.global	_Z9init_gpiov
	.type	_Z9init_gpiov, %function
_Z9init_gpiov:
.LFB4:
	.loc 1 108 18
	.cfi_startproc
	stp	x29, x30, [sp, -16]!
	.cfi_def_cfa_offset 16
	.cfi_offset 29, -16
	.cfi_offset 30, -8
	mov	x29, sp
	.loc 1 109 13
	mov	w2, 0
	mov	w1, 3
	mov	w0, 18
	bl	_ZL10setup_gpiojjj
	.loc 1 110 13
	mov	w2, 0
	mov	w1, 3
	mov	w0, 19
	bl	_ZL10setup_gpiojjj
	.loc 1 111 13
	mov	w2, 0
	mov	w1, 3
	mov	w0, 20
	bl	_ZL10setup_gpiojjj
	.loc 1 112 13
	mov	w2, 0
	mov	w1, 3
	mov	w0, 21
	bl	_ZL10setup_gpiojjj
	.loc 1 113 1
	nop
	ldp	x29, x30, [sp], 16
	.cfi_restore 30
	.cfi_restore 29
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE4:
	.size	_Z9init_gpiov, .-_Z9init_gpiov
	.section	.rodata
	.align	2
	.type	_ZL25SPI_CNTL0_DOUT_HOLD_SHIFT, %object
	.size	_ZL25SPI_CNTL0_DOUT_HOLD_SHIFT, 4
_ZL25SPI_CNTL0_DOUT_HOLD_SHIFT:
	.word	12
	.align	2
	.type	_ZL18SPI_CNTL0_CS_SHIFT, %object
	.size	_ZL18SPI_CNTL0_CS_SHIFT, 4
_ZL18SPI_CNTL0_CS_SHIFT:
	.word	17
	.align	2
	.type	_ZL21SPI_CNTL0_SPEED_SHIFT, %object
	.size	_ZL21SPI_CNTL0_SPEED_SHIFT, 4
_ZL21SPI_CNTL0_SPEED_SHIFT:
	.word	20
	.align	2
	.type	_ZL19SPI_CNTL0_POSTINPUT, %object
	.size	_ZL19SPI_CNTL0_POSTINPUT, 4
_ZL19SPI_CNTL0_POSTINPUT:
	.word	65536
	.align	2
	.type	_ZL16SPI_CNTL0_VAR_CS, %object
	.size	_ZL16SPI_CNTL0_VAR_CS, 4
_ZL16SPI_CNTL0_VAR_CS:
	.word	32768
	.align	2
	.type	_ZL19SPI_CNTL0_VAR_WIDTH, %object
	.size	_ZL19SPI_CNTL0_VAR_WIDTH, 4
_ZL19SPI_CNTL0_VAR_WIDTH:
	.word	16384
	.align	2
	.type	_ZL16SPI_CNTL0_Enable, %object
	.size	_ZL16SPI_CNTL0_Enable, 4
_ZL16SPI_CNTL0_Enable:
	.word	2048
	.align	2
	.type	_ZL19SPI_CNTL0_In_Rising, %object
	.size	_ZL19SPI_CNTL0_In_Rising, 4
_ZL19SPI_CNTL0_In_Rising:
	.word	1024
	.align	2
	.type	_ZL21SPI_CNTL0_Clear_FIFOs, %object
	.size	_ZL21SPI_CNTL0_Clear_FIFOs, 4
_ZL21SPI_CNTL0_Clear_FIFOs:
	.word	512
	.align	2
	.type	_ZL20SPI_CNTL0_Out_Rising, %object
	.size	_ZL20SPI_CNTL0_Out_Rising, 4
_ZL20SPI_CNTL0_Out_Rising:
	.word	256
	.align	2
	.type	_ZL20SPI_CNTL0_Invert_CLK, %object
	.size	_ZL20SPI_CNTL0_Invert_CLK, 4
_ZL20SPI_CNTL0_Invert_CLK:
	.word	128
	.align	2
	.type	_ZL20SPI_CNTL0_SO_MSB_FST, %object
	.size	_ZL20SPI_CNTL0_SO_MSB_FST, 4
_ZL20SPI_CNTL0_SO_MSB_FST:
	.word	64
	.align	2
	.type	_ZL19SPI_CNTL0_MAX_SHIFT, %object
	.size	_ZL19SPI_CNTL0_MAX_SHIFT, 4
_ZL19SPI_CNTL0_MAX_SHIFT:
	.word	63
	.align	2
	.type	_ZL23SPI_CNTL1_CS_HIGH_SHIFT, %object
	.size	_ZL23SPI_CNTL1_CS_HIGH_SHIFT, 4
_ZL23SPI_CNTL1_CS_HIGH_SHIFT:
	.word	8
	.align	2
	.type	_ZL20SPI_CNTL1_Keep_Input, %object
	.size	_ZL20SPI_CNTL1_Keep_Input, 4
_ZL20SPI_CNTL1_Keep_Input:
	.word	1
	.align	2
	.type	_ZL20SPI_CNTL1_SI_MSB_FST, %object
	.size	_ZL20SPI_CNTL1_SI_MSB_FST, 4
_ZL20SPI_CNTL1_SI_MSB_FST:
	.word	2
	.align	2
	.type	_ZL18SPI_CNTL1_Done_IRQ, %object
	.size	_ZL18SPI_CNTL1_Done_IRQ, 4
_ZL18SPI_CNTL1_Done_IRQ:
	.word	64
	.align	2
	.type	_ZL19SPI_CNTL1_TX_EM_IRQ, %object
	.size	_ZL19SPI_CNTL1_TX_EM_IRQ, 4
_ZL19SPI_CNTL1_TX_EM_IRQ:
	.word	128
	.align	2
	.type	_ZL21SPI_STAT_TX_FIFO_MASK, %object
	.size	_ZL21SPI_STAT_TX_FIFO_MASK, 4
_ZL21SPI_STAT_TX_FIFO_MASK:
	.word	-16777216
	.align	2
	.type	_ZL21SPI_STAT_RX_FIFO_MASK, %object
	.size	_ZL21SPI_STAT_RX_FIFO_MASK, 4
_ZL21SPI_STAT_RX_FIFO_MASK:
	.word	16711680
	.align	2
	.type	_ZL16SPI_STAT_TX_FULL, %object
	.size	_ZL16SPI_STAT_TX_FULL, 4
_ZL16SPI_STAT_TX_FULL:
	.word	1024
	.align	2
	.type	_ZL17SPI_STAT_TX_EMPTY, %object
	.size	_ZL17SPI_STAT_TX_EMPTY, 4
_ZL17SPI_STAT_TX_EMPTY:
	.word	512
	.align	2
	.type	_ZL16SPI_STAT_RX_FULL, %object
	.size	_ZL16SPI_STAT_RX_FULL, 4
_ZL16SPI_STAT_RX_FULL:
	.word	256
	.align	2
	.type	_ZL17SPI_STAT_RX_EMPTY, %object
	.size	_ZL17SPI_STAT_RX_EMPTY, 4
_ZL17SPI_STAT_RX_EMPTY:
	.word	128
	.align	2
	.type	_ZL13SPI_STAT_BUSY, %object
	.size	_ZL13SPI_STAT_BUSY, 4
_ZL13SPI_STAT_BUSY:
	.word	64
	.align	2
	.type	_ZL21SPI_STAT_BIT_CNT_MASK, %object
	.size	_ZL21SPI_STAT_BIT_CNT_MASK, 4
_ZL21SPI_STAT_BIT_CNT_MASK:
	.word	63
	.text
	.align	2
	.global	_Z8init_spij
	.type	_Z8init_spij, %function
_Z8init_spij:
.LFB5:
	.loc 1 147 33
	.cfi_startproc
	stp	x29, x30, [sp, -64]!
	.cfi_def_cfa_offset 64
	.cfi_offset 29, -64
	.cfi_offset 30, -56
	mov	x29, sp
	str	x19, [sp, 16]
	.cfi_offset 19, -48
	str	w0, [sp, 44]
	.loc 1 148 21
	bl	_ZL3auxv
	.loc 1 148 25
	ldr	w0, [x0, 4]
	str	w0, [sp, 60]
	.loc 1 149 13
	ldr	w0, [sp, 44]
	mov	w1, 2
	lsl	w0, w1, w0
	mov	w1, w0
	.loc 1 149 7
	ldr	w0, [sp, 60]
	orr	w0, w0, w1
	str	w0, [sp, 60]
	.loc 1 150 18
	ldr	w19, [sp, 60]
	.loc 1 150 6
	bl	_ZL3auxv
	.loc 1 150 18
	str	w19, [x0, 4]
	.loc 1 151 6
	ldr	w0, [sp, 44]
	bl	_ZL3spii
	mov	x1, x0
	.loc 1 151 23
	mov	w0, 512
	str	w0, [x1]
	.loc 1 152 12
	mov	w0, 82
	str	w0, [sp, 56]
	.loc 1 153 32
	ldr	w0, [sp, 56]
	lsl	w1, w0, 20
	.loc 1 157 23
	mov	w0, 19520
	orr	w19, w1, w0
	.loc 1 153 6
	ldr	w0, [sp, 44]
	bl	_ZL3spii
	.loc 1 153 23
	str	w19, [x0]
	.loc 1 158 6
	ldr	w0, [sp, 44]
	bl	_ZL3spii
	mov	x1, x0
	.loc 1 158 23
	mov	w0, 2
	str	w0, [x1, 4]
	.loc 1 159 1
	nop
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 64
	.cfi_restore 30
	.cfi_restore 29
	.cfi_restore 19
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE5:
	.size	_Z8init_spij, .-_Z8init_spij
	.align	2
	.type	_ZL13spi_send_recvjPKcmPcm, %function
_ZL13spi_send_recvjPKcmPcm:
.LFB6:
	.loc 1 161 113
	.cfi_startproc
	stp	x29, x30, [sp, -128]!
	.cfi_def_cfa_offset 128
	.cfi_offset 29, -128
	.cfi_offset 30, -120
	mov	x29, sp
	str	x19, [sp, 16]
	.cfi_offset 19, -112
	str	w0, [sp, 76]
	str	x1, [sp, 64]
	str	x2, [sp, 56]
	str	x3, [sp, 48]
	str	x4, [sp, 40]
	.loc 1 162 10
	str	xzr, [sp, 120]
	.loc 1 163 10
	str	xzr, [sp, 112]
	.loc 1 164 3
	b	.L13
.L26:
.LBB2:
	.loc 1 165 14
	str	wzr, [sp, 108]
	.loc 1 166 12
	str	xzr, [sp, 96]
	.loc 1 169 5
	b	.L14
.L16:
	.loc 1 170 31 discriminator 3
	ldr	x1, [sp, 64]
	ldr	x0, [sp, 120]
	add	x0, x1, x0
	ldrb	w0, [x0]
	mov	w1, w0
	.loc 1 170 40 discriminator 3
	ldr	x0, [sp, 96]
	mov	w2, w0
	mov	w0, 16
	sub	w0, w0, w2
	.loc 1 170 33 discriminator 3
	lsl	w0, w1, w0
	mov	w1, w0
	.loc 1 170 12 discriminator 3
	ldr	w0, [sp, 108]
	orr	w0, w0, w1
	str	w0, [sp, 108]
	.loc 1 169 53 discriminator 3
	ldr	x0, [sp, 120]
	add	x0, x0, 1
	str	x0, [sp, 120]
	.loc 1 169 65 discriminator 3
	ldr	x0, [sp, 96]
	add	x0, x0, 8
	str	x0, [sp, 96]
.L14:
	.loc 1 169 30 discriminator 1
	ldr	x1, [sp, 120]
	ldr	x0, [sp, 56]
	cmp	x1, x0
	bcs	.L15
	.loc 1 169 30 is_stmt 0 discriminator 2
	ldr	x0, [sp, 96]
	cmp	x0, 23
	bls	.L16
.L15:
	.loc 1 172 20 is_stmt 1
	ldr	x0, [sp, 96]
	lsl	w0, w0, 24
	.loc 1 172 10
	ldr	w1, [sp, 108]
	orr	w0, w1, w0
	str	w0, [sp, 108]
	.loc 1 175 5
	b	.L17
.L18:
	.loc 1 175 51 discriminator 2
	// Start of user assembly
// 175 "rpi.cc" 1
	yield
// 0 "" 2
	// End of user assembly
.L17:
	.loc 1 175 15 discriminator 1
	ldr	w0, [sp, 76]
	bl	_ZL3spii
	.loc 1 175 26 discriminator 1
	ldr	w0, [x0, 8]
	.loc 1 175 31 discriminator 1
	and	w0, w0, 1024
	cmp	w0, 0
	cset	w0, ne
	and	w0, w0, 255
	cmp	w0, 0
	bne	.L18
	.loc 1 176 5
	ldr	x1, [sp, 120]
	ldr	x0, [sp, 56]
	cmp	x1, x0
	bcs	.L19
	.loc 1 177 33
	ldr	w19, [sp, 108]
	.loc 1 177 10
	ldr	w0, [sp, 76]
	bl	_ZL3spii
	.loc 1 177 33
	str	w19, [x0, 48]
	b	.L21
.L19:
	.loc 1 179 29
	ldr	w19, [sp, 108]
	.loc 1 179 10
	ldr	w0, [sp, 76]
	bl	_ZL3spii
	.loc 1 179 29
	str	w19, [x0, 32]
	.loc 1 183 5
	b	.L21
.L22:
	.loc 1 183 52 discriminator 2
	// Start of user assembly
// 183 "rpi.cc" 1
	yield
// 0 "" 2
	// End of user assembly
.L21:
	.loc 1 183 15 discriminator 1
	ldr	w0, [sp, 76]
	bl	_ZL3spii
	.loc 1 183 26 discriminator 1
	ldr	w0, [x0, 8]
	.loc 1 183 31 discriminator 1
	and	w0, w0, 128
	cmp	w0, 0
	cset	w0, ne
	and	w0, w0, 255
	cmp	w0, 0
	bne	.L22
	.loc 1 184 15
	ldr	w0, [sp, 76]
	bl	_ZL3spii
	.loc 1 184 10
	ldr	w0, [x0, 32]
	str	w0, [sp, 108]
	.loc 1 187 27
	ldr	x1, [sp, 40]
	ldr	x0, [sp, 112]
	sub	x0, x1, x0
	.loc 1 187 12
	lsl	x0, x0, 3
	str	x0, [sp, 88]
	.loc 1 188 5
	ldr	x1, [sp, 96]
	ldr	x0, [sp, 88]
	cmp	x1, x0
	bls	.L24
	.loc 1 188 28 discriminator 1
	ldr	x0, [sp, 88]
	str	x0, [sp, 96]
	.loc 1 189 5 discriminator 1
	b	.L24
.L25:
	.loc 1 190 42 discriminator 2
	ldr	x0, [sp, 96]
	sub	w0, w0, #8
	.loc 1 190 32 discriminator 2
	ldr	w1, [sp, 108]
	lsr	w2, w1, w0
	.loc 1 190 22 discriminator 2
	ldr	x1, [sp, 48]
	ldr	x0, [sp, 112]
	add	x0, x1, x0
	.loc 1 190 24 discriminator 2
	and	w1, w2, 255
	strb	w1, [x0]
	.loc 1 189 31 discriminator 2
	ldr	x0, [sp, 112]
	add	x0, x0, 1
	str	x0, [sp, 112]
	.loc 1 189 43 discriminator 2
	ldr	x0, [sp, 96]
	sub	x0, x0, #8
	str	x0, [sp, 96]
.L24:
	.loc 1 189 18 discriminator 1
	ldr	x0, [sp, 96]
	cmp	x0, 0
	bne	.L25
.L13:
.LBE2:
	.loc 1 164 28
	ldr	x1, [sp, 120]
	ldr	x0, [sp, 56]
	cmp	x1, x0
	bcc	.L26
	.loc 1 164 28 is_stmt 0 discriminator 1
	ldr	x1, [sp, 112]
	ldr	x0, [sp, 40]
	cmp	x1, x0
	bcc	.L26
	.loc 1 193 1 is_stmt 1
	nop
	nop
	ldr	x19, [sp, 16]
	ldp	x29, x30, [sp], 128
	.cfi_restore 30
	.cfi_restore 29
	.cfi_restore 19
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE6:
	.size	_ZL13spi_send_recvjPKcmPcm, .-_ZL13spi_send_recvjPKcmPcm
	.section	.rodata
	.type	_ZL8UART_RHR, %object
	.size	_ZL8UART_RHR, 1
_ZL8UART_RHR:
	.zero	1
	.type	_ZL8UART_THR, %object
	.size	_ZL8UART_THR, 1
_ZL8UART_THR:
	.zero	1
	.type	_ZL8UART_IER, %object
	.size	_ZL8UART_IER, 1
_ZL8UART_IER:
	.byte	1
	.type	_ZL8UART_IIR, %object
	.size	_ZL8UART_IIR, 1
_ZL8UART_IIR:
	.byte	2
	.type	_ZL8UART_FCR, %object
	.size	_ZL8UART_FCR, 1
_ZL8UART_FCR:
	.byte	2
	.type	_ZL8UART_LCR, %object
	.size	_ZL8UART_LCR, 1
_ZL8UART_LCR:
	.byte	3
	.type	_ZL8UART_MCR, %object
	.size	_ZL8UART_MCR, 1
_ZL8UART_MCR:
	.byte	4
	.type	_ZL8UART_LSR, %object
	.size	_ZL8UART_LSR, 1
_ZL8UART_LSR:
	.byte	5
	.type	_ZL8UART_MSR, %object
	.size	_ZL8UART_MSR, 1
_ZL8UART_MSR:
	.byte	6
	.type	_ZL8UART_SPR, %object
	.size	_ZL8UART_SPR, 1
_ZL8UART_SPR:
	.byte	7
	.type	_ZL10UART_TXLVL, %object
	.size	_ZL10UART_TXLVL, 1
_ZL10UART_TXLVL:
	.byte	8
	.type	_ZL10UART_RXLVL, %object
	.size	_ZL10UART_RXLVL, 1
_ZL10UART_RXLVL:
	.byte	9
	.type	_ZL10UART_IODir, %object
	.size	_ZL10UART_IODir, 1
_ZL10UART_IODir:
	.byte	10
	.type	_ZL12UART_IOState, %object
	.size	_ZL12UART_IOState, 1
_ZL12UART_IOState:
	.byte	11
	.type	_ZL13UART_IOIntEna, %object
	.size	_ZL13UART_IOIntEna, 1
_ZL13UART_IOIntEna:
	.byte	12
	.type	_ZL13UART_reserved, %object
	.size	_ZL13UART_reserved, 1
_ZL13UART_reserved:
	.byte	13
	.type	_ZL14UART_IOControl, %object
	.size	_ZL14UART_IOControl, 1
_ZL14UART_IOControl:
	.byte	14
	.type	_ZL9UART_EFCR, %object
	.size	_ZL9UART_EFCR, 1
_ZL9UART_EFCR:
	.byte	15
	.type	_ZL8UART_DLL, %object
	.size	_ZL8UART_DLL, 1
_ZL8UART_DLL:
	.zero	1
	.type	_ZL8UART_DLH, %object
	.size	_ZL8UART_DLH, 1
_ZL8UART_DLH:
	.byte	1
	.type	_ZL8UART_EFR, %object
	.size	_ZL8UART_EFR, 1
_ZL8UART_EFR:
	.byte	2
	.type	_ZL8UART_TCR, %object
	.size	_ZL8UART_TCR, 1
_ZL8UART_TCR:
	.byte	6
	.type	_ZL8UART_TLR, %object
	.size	_ZL8UART_TLR, 1
_ZL8UART_TLR:
	.byte	7
	.type	_ZL18UART_CHANNEL_SHIFT, %object
	.size	_ZL18UART_CHANNEL_SHIFT, 1
_ZL18UART_CHANNEL_SHIFT:
	.byte	1
	.type	_ZL15UART_ADDR_SHIFT, %object
	.size	_ZL15UART_ADDR_SHIFT, 1
_ZL15UART_ADDR_SHIFT:
	.byte	3
	.type	_ZL16UART_READ_ENABLE, %object
	.size	_ZL16UART_READ_ENABLE, 1
_ZL16UART_READ_ENABLE:
	.byte	-128
	.type	_ZL22UART_FCR_TX_FIFO_RESET, %object
	.size	_ZL22UART_FCR_TX_FIFO_RESET, 1
_ZL22UART_FCR_TX_FIFO_RESET:
	.byte	4
	.type	_ZL22UART_FCR_RX_FIFO_RESET, %object
	.size	_ZL22UART_FCR_RX_FIFO_RESET, 1
_ZL22UART_FCR_RX_FIFO_RESET:
	.byte	2
	.type	_ZL16UART_FCR_FIFO_EN, %object
	.size	_ZL16UART_FCR_FIFO_EN, 1
_ZL16UART_FCR_FIFO_EN:
	.byte	1
	.type	_ZL21UART_LCR_DIV_LATCH_EN, %object
	.size	_ZL21UART_LCR_DIV_LATCH_EN, 1
_ZL21UART_LCR_DIV_LATCH_EN:
	.byte	-128
	.type	_ZL28UART_EFR_ENABLE_ENHANCED_FNS, %object
	.size	_ZL28UART_EFR_ENABLE_ENHANCED_FNS, 1
_ZL28UART_EFR_ENABLE_ENHANCED_FNS:
	.byte	16
	.type	_ZL20UART_IOControl_RESET, %object
	.size	_ZL20UART_IOControl_RESET, 1
_ZL20UART_IOControl_RESET:
	.byte	8
	.text
	.align	2
	.type	_ZL19uart_write_registermmcc, %function
_ZL19uart_write_registermmcc:
.LFB7:
	.loc 1 233 93
	.cfi_startproc
	stp	x29, x30, [sp, -64]!
	.cfi_def_cfa_offset 64
	.cfi_offset 29, -64
	.cfi_offset 30, -56
	mov	x29, sp
	str	x0, [sp, 40]
	str	x1, [sp, 32]
	strb	w2, [sp, 31]
	strb	w3, [sp, 30]
	.loc 1 234 8
	strh	wzr, [sp, 56]
	.loc 1 235 25
	ldr	x0, [sp, 32]
	and	w0, w0, 255
	ubfiz	w0, w0, 1, 7
	and	w1, w0, 255
	.loc 1 235 55
	ldrb	w0, [sp, 31]
	ubfiz	w0, w0, 3, 5
	and	w0, w0, 255
	.loc 1 235 48
	orr	w0, w1, w0
	and	w0, w0, 255
	.loc 1 235 10
	strb	w0, [sp, 56]
	.loc 1 236 10
	ldrb	w0, [sp, 30]
	strb	w0, [sp, 57]
	.loc 1 237 16
	ldr	x0, [sp, 40]
	mov	w5, w0
	add	x0, sp, 56
	mov	x4, 0
	mov	x3, 0
	mov	x2, 2
	mov	x1, x0
	mov	w0, w5
	bl	_ZL13spi_send_recvjPKcmPcm
	.loc 1 238 1
	nop
	ldp	x29, x30, [sp], 64
	.cfi_restore 30
	.cfi_restore 29
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE7:
	.size	_ZL19uart_write_registermmcc, .-_ZL19uart_write_registermmcc
	.align	2
	.type	_ZL18uart_read_registermmc, %function
_ZL18uart_read_registermmc:
.LFB8:
	.loc 1 240 82
	.cfi_startproc
	stp	x29, x30, [sp, -64]!
	.cfi_def_cfa_offset 64
	.cfi_offset 29, -64
	.cfi_offset 30, -56
	mov	x29, sp
	str	x0, [sp, 40]
	str	x1, [sp, 32]
	strb	w2, [sp, 31]
	.loc 1 241 8
	strh	wzr, [sp, 56]
	.loc 1 242 8
	strh	wzr, [sp, 48]
	.loc 1 243 25
	ldr	x0, [sp, 32]
	and	w0, w0, 255
	ubfiz	w0, w0, 1, 7
	and	w1, w0, 255
	.loc 1 243 55
	ldrb	w0, [sp, 31]
	ubfiz	w0, w0, 3, 5
	and	w0, w0, 255
	.loc 1 243 48
	orr	w0, w1, w0
	and	w0, w0, 255
	.loc 1 243 75
	orr	w0, w0, -128
	and	w0, w0, 255
	.loc 1 243 10
	strb	w0, [sp, 56]
	.loc 1 244 16
	ldr	x0, [sp, 40]
	mov	w5, w0
	add	x1, sp, 48
	add	x0, sp, 56
	mov	x4, 2
	mov	x3, x1
	mov	x2, 2
	mov	x1, x0
	mov	w0, w5
	bl	_ZL13spi_send_recvjPKcmPcm
	.loc 1 245 15
	ldrb	w0, [sp, 49]
	.loc 1 246 1
	ldp	x29, x30, [sp], 64
	.cfi_restore 30
	.cfi_restore 29
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE8:
	.size	_ZL18uart_read_registermmc, .-_ZL18uart_read_registermmc
	.align	2
	.type	_ZL17uart_init_channelmmm, %function
_ZL17uart_init_channelmmm:
.LFB9:
	.loc 1 248 87
	.cfi_startproc
	stp	x29, x30, [sp, -64]!
	.cfi_def_cfa_offset 64
	.cfi_offset 29, -64
	.cfi_offset 30, -56
	mov	x29, sp
	str	x0, [sp, 40]
	str	x1, [sp, 32]
	str	x2, [sp, 24]
	.loc 1 250 22
	mov	w3, -128
	mov	w2, 3
	ldr	x1, [sp, 32]
	ldr	x0, [sp, 40]
	bl	_ZL19uart_write_registermmcc
	.loc 1 251 43
	ldr	x0, [sp, 24]
	lsl	x0, x0, 4
	.loc 1 251 31
	mov	x1, 14745600
	udiv	x0, x1, x0
	.loc 1 251 12
	str	w0, [sp, 56]
	.loc 1 252 77
	ldr	w0, [sp, 56]
	lsr	w0, w0, 4
	.loc 1 252 22
	and	w0, w0, 255
	and	w0, w0, -16
	and	w0, w0, 255
	mov	w3, w0
	mov	w2, 1
	ldr	x1, [sp, 32]
	ldr	x0, [sp, 40]
	bl	_ZL19uart_write_registermmcc
	.loc 1 253 22
	ldr	w0, [sp, 56]
	and	w0, w0, 255
	mov	w3, w0
	mov	w2, 0
	ldr	x1, [sp, 32]
	ldr	x0, [sp, 40]
	bl	_ZL19uart_write_registermmcc
	.loc 1 256 22
	mov	w3, 3
	mov	w2, 3
	ldr	x1, [sp, 32]
	ldr	x0, [sp, 40]
	bl	_ZL19uart_write_registermmcc
	.loc 1 259 22
	mov	w3, 7
	mov	w2, 2
	ldr	x1, [sp, 32]
	ldr	x0, [sp, 40]
	bl	_ZL19uart_write_registermmcc
.LBB3:
	.loc 1 260 12
	str	wzr, [sp, 60]
	.loc 1 260 3
	b	.L31
.L32:
	.loc 1 260 35 discriminator 3
	// Start of user assembly
// 260 "rpi.cc" 1
	yield
// 0 "" 2
	.loc 1 260 3 discriminator 3
	// End of user assembly
	ldr	w0, [sp, 60]
	add	w0, w0, 1
	str	w0, [sp, 60]
.L31:
	.loc 1 260 21 discriminator 1
	ldr	w1, [sp, 60]
	mov	w0, 65534
	cmp	w1, w0
	ble	.L32
.LBE3:
	.loc 1 261 1
	nop
	nop
	ldp	x29, x30, [sp], 64
	.cfi_restore 30
	.cfi_restore 29
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE9:
	.size	_ZL17uart_init_channelmmm, .-_ZL17uart_init_channelmmm
	.align	2
	.global	_Z9init_uartj
	.type	_Z9init_uartj, %function
_Z9init_uartj:
.LFB10:
	.loc 1 263 37
	.cfi_startproc
	stp	x29, x30, [sp, -32]!
	.cfi_def_cfa_offset 32
	.cfi_offset 29, -32
	.cfi_offset 30, -24
	mov	x29, sp
	str	w0, [sp, 28]
	.loc 1 264 22
	ldr	w0, [sp, 28]
	mov	w3, 8
	mov	w2, 14
	mov	x1, 0
	bl	_ZL19uart_write_registermmcc
	.loc 1 265 20
	ldr	w0, [sp, 28]
	mov	x2, 49664
	movk	x2, 0x1, lsl 16
	mov	x1, 0
	bl	_ZL17uart_init_channelmmm
	.loc 1 266 20
	ldr	w0, [sp, 28]
	mov	x2, 2400
	mov	x1, 1
	bl	_ZL17uart_init_channelmmm
	.loc 1 267 1
	nop
	ldp	x29, x30, [sp], 32
	.cfi_restore 30
	.cfi_restore 29
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE10:
	.size	_Z9init_uartj, .-_Z9init_uartj
	.align	2
	.global	_Z9uart_getcmm
	.type	_Z9uart_getcmm, %function
_Z9uart_getcmm:
.LFB11:
	.loc 1 269 55
	.cfi_startproc
	stp	x29, x30, [sp, -32]!
	.cfi_def_cfa_offset 32
	.cfi_offset 29, -32
	.cfi_offset 30, -24
	mov	x29, sp
	str	x0, [sp, 24]
	str	x1, [sp, 16]
	.loc 1 270 3
	b	.L35
.L36:
	.loc 1 270 72 discriminator 2
	// Start of user assembly
// 270 "rpi.cc" 1
	yield
// 0 "" 2
	// End of user assembly
.L35:
	.loc 1 270 28 discriminator 1
	mov	w2, 9
	ldr	x1, [sp, 16]
	ldr	x0, [sp, 24]
	bl	_ZL18uart_read_registermmc
	and	w0, w0, 255
	.loc 1 270 66 discriminator 1
	cmp	w0, 0
	cset	w0, eq
	and	w0, w0, 255
	cmp	w0, 0
	bne	.L36
	.loc 1 271 28
	mov	w2, 0
	ldr	x1, [sp, 16]
	ldr	x0, [sp, 24]
	bl	_ZL18uart_read_registermmc
	and	w0, w0, 255
	.loc 1 272 1
	ldp	x29, x30, [sp], 32
	.cfi_restore 30
	.cfi_restore 29
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE11:
	.size	_Z9uart_getcmm, .-_Z9uart_getcmm
	.align	2
	.global	_Z9uart_putcmmc
	.type	_Z9uart_putcmmc, %function
_Z9uart_putcmmc:
.LFB12:
	.loc 1 274 63
	.cfi_startproc
	stp	x29, x30, [sp, -48]!
	.cfi_def_cfa_offset 48
	.cfi_offset 29, -48
	.cfi_offset 30, -40
	mov	x29, sp
	str	x0, [sp, 40]
	str	x1, [sp, 32]
	strb	w2, [sp, 31]
	.loc 1 275 3
	b	.L39
.L40:
	.loc 1 275 72 discriminator 2
	// Start of user assembly
// 275 "rpi.cc" 1
	yield
// 0 "" 2
	// End of user assembly
.L39:
	.loc 1 275 28 discriminator 1
	mov	w2, 8
	ldr	x1, [sp, 32]
	ldr	x0, [sp, 40]
	bl	_ZL18uart_read_registermmc
	and	w0, w0, 255
	.loc 1 275 66 discriminator 1
	cmp	w0, 0
	cset	w0, eq
	and	w0, w0, 255
	cmp	w0, 0
	bne	.L40
	.loc 1 276 22
	ldrb	w3, [sp, 31]
	mov	w2, 0
	ldr	x1, [sp, 32]
	ldr	x0, [sp, 40]
	bl	_ZL19uart_write_registermmcc
	.loc 1 277 1
	nop
	ldp	x29, x30, [sp], 48
	.cfi_restore 30
	.cfi_restore 29
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE12:
	.size	_Z9uart_putcmmc, .-_Z9uart_putcmmc
	.section	.rodata
	.align	3
	.type	_ZZ9uart_putsmmPKcmE3max, %object
	.size	_ZZ9uart_putsmmPKcmE3max, 8
_ZZ9uart_putsmmPKcmE3max:
	.xword	32
	.text
	.align	2
	.global	_Z9uart_putsmmPKcm
	.type	_Z9uart_putsmmPKcm, %function
_Z9uart_putsmmPKcm:
.LFB13:
	.loc 1 279 85
	.cfi_startproc
	stp	x29, x30, [sp, -112]!
	.cfi_def_cfa_offset 112
	.cfi_offset 29, -112
	.cfi_offset 30, -104
	mov	x29, sp
	str	x0, [sp, 40]
	str	x1, [sp, 32]
	str	x2, [sp, 24]
	str	x3, [sp, 16]
	.loc 1 282 26
	ldr	x0, [sp, 32]
	and	w0, w0, 255
	ubfiz	w0, w0, 1, 7
	and	w0, w0, 255
	.loc 1 282 11
	strb	w0, [sp, 56]
	.loc 1 283 35
	mov	w2, 8
	ldr	x1, [sp, 32]
	ldr	x0, [sp, 40]
	bl	_ZL18uart_read_registermmc
	and	w0, w0, 255
	.loc 1 283 71
	and	x0, x0, 255
	str	x0, [sp, 104]
	.loc 1 284 3
	ldr	x0, [sp, 104]
	cmp	x0, 32
	bls	.L42
	.loc 1 284 24 discriminator 1
	mov	x0, 32
	str	x0, [sp, 104]
.L42:
.LBB4:
	.loc 1 285 15
	str	xzr, [sp, 96]
	.loc 1 285 25
	mov	x0, 1
	str	x0, [sp, 88]
.L48:
	.loc 1 286 5
	ldr	x1, [sp, 88]
	ldr	x0, [sp, 104]
	cmp	x1, x0
	bcs	.L43
	.loc 1 286 21 discriminator 1
	ldr	x1, [sp, 96]
	ldr	x0, [sp, 16]
	cmp	x1, x0
	bcs	.L43
	.loc 1 287 28
	ldr	x1, [sp, 24]
	ldr	x0, [sp, 96]
	add	x0, x1, x0
	ldrb	w2, [x0]
	.loc 1 287 18
	ldr	x0, [sp, 88]
	add	x1, sp, 56
	strb	w2, [x1, x0]
	.loc 1 288 12
	ldr	x0, [sp, 96]
	add	x0, x0, 1
	str	x0, [sp, 96]
	.loc 1 289 12
	ldr	x0, [sp, 88]
	add	x0, x0, 1
	str	x0, [sp, 88]
	b	.L44
.L43:
	.loc 1 291 20
	ldr	x0, [sp, 40]
	mov	w5, w0
	add	x0, sp, 56
	mov	x4, 0
	mov	x3, 0
	ldr	x2, [sp, 88]
	mov	x1, x0
	mov	w0, w5
	bl	_ZL13spi_send_recvjPKcmPcm
	.loc 1 292 7
	ldr	x1, [sp, 96]
	ldr	x0, [sp, 16]
	cmp	x1, x0
	beq	.L50
	.loc 1 293 32
	mov	w2, 8
	ldr	x1, [sp, 32]
	ldr	x0, [sp, 40]
	bl	_ZL18uart_read_registermmc
	and	w0, w0, 255
	.loc 1 293 12
	and	x0, x0, 255
	str	x0, [sp, 104]
	.loc 1 294 7
	ldr	x0, [sp, 104]
	cmp	x0, 32
	bls	.L47
	.loc 1 294 28 discriminator 1
	mov	x0, 32
	str	x0, [sp, 104]
.L47:
	.loc 1 295 12
	mov	x0, 1
	str	x0, [sp, 88]
.L44:
	.loc 1 286 5
	b	.L48
.L50:
	.loc 1 292 25
	nop
.LBE4:
	.loc 1 298 1
	nop
	ldp	x29, x30, [sp], 112
	.cfi_restore 30
	.cfi_restore 29
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE13:
	.size	_Z9uart_putsmmPKcm, .-_Z9uart_putsmmPKcm
	.align	2
	.global	memset
	.type	memset, %function
memset:
.LFB14:
	.loc 1 301 51
	.cfi_startproc
	sub	sp, sp, #48
	.cfi_def_cfa_offset 48
	str	x0, [sp, 24]
	str	w1, [sp, 20]
	str	x2, [sp, 8]
.LBB5:
	.loc 1 302 14
	ldr	x0, [sp, 24]
	str	x0, [sp, 40]
	.loc 1 302 3
	b	.L52
.L53:
	.loc 1 302 47 discriminator 3
	ldr	w0, [sp, 20]
	and	w2, w0, 255
	.loc 1 302 44 discriminator 3
	ldr	x0, [sp, 40]
	add	x1, x0, 1
	str	x1, [sp, 40]
	.loc 1 302 47 discriminator 3
	mov	w1, w2
	strb	w1, [x0]
	.loc 1 302 3 discriminator 3
	ldr	x0, [sp, 8]
	sub	x0, x0, #1
	str	x0, [sp, 8]
.L52:
	.loc 1 302 31 discriminator 1
	ldr	x0, [sp, 8]
	cmp	x0, 0
	bne	.L53
.LBE5:
	.loc 1 303 10
	ldr	x0, [sp, 24]
	.loc 1 304 1
	add	sp, sp, 48
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE14:
	.size	memset, .-memset
	.align	2
	.global	memcpy
	.type	memcpy, %function
memcpy:
.LFB15:
	.loc 1 307 90
	.cfi_startproc
	sub	sp, sp, #64
	.cfi_def_cfa_offset 64
	str	x0, [sp, 24]
	str	x1, [sp, 16]
	str	x2, [sp, 8]
	.loc 1 308 11
	ldr	x0, [sp, 16]
	str	x0, [sp, 56]
	.loc 1 309 11
	ldr	x0, [sp, 24]
	str	x0, [sp, 48]
.LBB6:
	.loc 1 310 17
	str	xzr, [sp, 40]
	.loc 1 310 5
	b	.L56
.L57:
	.loc 1 310 54 discriminator 3
	ldr	x0, [sp, 56]
	add	x1, x0, 1
	str	x1, [sp, 56]
	.loc 1 310 49 discriminator 3
	ldrb	w2, [x0]
	.loc 1 310 43 discriminator 3
	ldr	x0, [sp, 48]
	add	x1, x0, 1
	str	x1, [sp, 48]
	.loc 1 310 47 discriminator 3
	mov	w1, w2
	strb	w1, [x0]
	.loc 1 310 5 discriminator 3
	ldr	x0, [sp, 40]
	add	x0, x0, 1
	str	x0, [sp, 40]
.L56:
	.loc 1 310 26 discriminator 1
	ldr	x1, [sp, 40]
	ldr	x0, [sp, 8]
	cmp	x1, x0
	bcc	.L57
.LBE6:
	.loc 1 311 12
	ldr	x0, [sp, 24]
	.loc 1 312 1
	add	sp, sp, 64
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE15:
	.size	memcpy, .-memcpy
	.section	.rodata
	.align	3
.LC0:
	.string	"\r\n"
	.text
	.align	2
	.global	debug
	.type	debug, %function
debug:
.LFB16:
	.loc 1 314 31
	.cfi_startproc
	stp	x29, x30, [sp, -32]!
	.cfi_def_cfa_offset 32
	.cfi_offset 29, -32
	.cfi_offset 30, -24
	mov	x29, sp
	strb	w0, [sp, 31]
	.loc 1 315 12
	ldrb	w2, [sp, 31]
	mov	x1, 0
	mov	x0, 0
	bl	_Z9uart_putcmmc
	.loc 1 316 12
	mov	x3, 2
	adrp	x0, .LC0
	add	x2, x0, :lo12:.LC0
	mov	x1, 0
	mov	x0, 0
	bl	_Z9uart_putsmmPKcm
	.loc 1 317 1
	nop
	ldp	x29, x30, [sp], 32
	.cfi_restore 30
	.cfi_restore 29
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE16:
	.size	debug, .-debug
.Letext0:
	.file 2 "/u3/cs452/public/xdev/lib/gcc/aarch64-none-elf/11.3.1/include/stdint-gcc.h"
	.file 3 "/u3/cs452/public/xdev/lib/gcc/aarch64-none-elf/11.3.1/include/stddef.h"
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.4byte	0xed6
	.2byte	0x5
	.byte	0x1
	.byte	0x8
	.4byte	.Ldebug_abbrev0
	.uleb128 0x17
	.4byte	.LASF164
	.byte	0x21
	.4byte	.LASF0
	.4byte	.LASF1
	.8byte	.Ltext0
	.8byte	.Letext0-.Ltext0
	.4byte	.Ldebug_line0
	.uleb128 0x4
	.byte	0x1
	.byte	0x6
	.4byte	.LASF2
	.uleb128 0x4
	.byte	0x2
	.byte	0x5
	.4byte	.LASF3
	.uleb128 0x18
	.byte	0x4
	.byte	0x5
	.string	"int"
	.uleb128 0x4
	.byte	0x8
	.byte	0x5
	.4byte	.LASF4
	.uleb128 0x4
	.byte	0x1
	.byte	0x8
	.4byte	.LASF5
	.uleb128 0x4
	.byte	0x2
	.byte	0x7
	.4byte	.LASF6
	.uleb128 0x13
	.4byte	.LASF9
	.byte	0x2
	.byte	0x34
	.byte	0x19
	.4byte	0x69
	.uleb128 0xe
	.4byte	0x58
	.uleb128 0x4
	.byte	0x4
	.byte	0x7
	.4byte	.LASF7
	.uleb128 0x4
	.byte	0x8
	.byte	0x7
	.4byte	.LASF8
	.uleb128 0x13
	.4byte	.LASF10
	.byte	0x3
	.byte	0xd1
	.byte	0x17
	.4byte	0x70
	.uleb128 0xe
	.4byte	0x77
	.uleb128 0x4
	.byte	0x8
	.byte	0x5
	.4byte	.LASF11
	.uleb128 0x4
	.byte	0x10
	.byte	0x4
	.4byte	.LASF12
	.uleb128 0x19
	.4byte	.LASF165
	.uleb128 0x1a
	.4byte	.LASF36
	.byte	0xf4
	.byte	0x1
	.byte	0x3
	.byte	0x8
	.4byte	0x1a6
	.uleb128 0x2
	.4byte	.LASF13
	.byte	0x4
	.4byte	0x1ab
	.byte	0
	.uleb128 0x2
	.4byte	.LASF14
	.byte	0x6
	.4byte	0x58
	.byte	0x1c
	.uleb128 0x2
	.4byte	.LASF15
	.byte	0x7
	.4byte	0x58
	.byte	0x20
	.uleb128 0x2
	.4byte	.LASF16
	.byte	0x9
	.4byte	0x58
	.byte	0x28
	.uleb128 0x2
	.4byte	.LASF17
	.byte	0xa
	.4byte	0x58
	.byte	0x2c
	.uleb128 0x2
	.4byte	.LASF18
	.byte	0xc
	.4byte	0x58
	.byte	0x34
	.uleb128 0x2
	.4byte	.LASF19
	.byte	0xd
	.4byte	0x58
	.byte	0x38
	.uleb128 0x2
	.4byte	.LASF20
	.byte	0xf
	.4byte	0x58
	.byte	0x40
	.uleb128 0x2
	.4byte	.LASF21
	.byte	0x10
	.4byte	0x58
	.byte	0x44
	.uleb128 0x2
	.4byte	.LASF22
	.byte	0x12
	.4byte	0x58
	.byte	0x4c
	.uleb128 0x2
	.4byte	.LASF23
	.byte	0x13
	.4byte	0x58
	.byte	0x50
	.uleb128 0x2
	.4byte	.LASF24
	.byte	0x15
	.4byte	0x58
	.byte	0x58
	.uleb128 0x2
	.4byte	.LASF25
	.byte	0x16
	.4byte	0x58
	.byte	0x5c
	.uleb128 0x2
	.4byte	.LASF26
	.byte	0x18
	.4byte	0x58
	.byte	0x64
	.uleb128 0x2
	.4byte	.LASF27
	.byte	0x19
	.4byte	0x58
	.byte	0x68
	.uleb128 0x2
	.4byte	.LASF28
	.byte	0x1b
	.4byte	0x58
	.byte	0x70
	.uleb128 0x2
	.4byte	.LASF29
	.byte	0x1c
	.4byte	0x58
	.byte	0x74
	.uleb128 0x2
	.4byte	.LASF30
	.byte	0x1e
	.4byte	0x58
	.byte	0x7c
	.uleb128 0x2
	.4byte	.LASF31
	.byte	0x1f
	.4byte	0x58
	.byte	0x80
	.uleb128 0x2
	.4byte	.LASF32
	.byte	0x21
	.4byte	0x58
	.byte	0x88
	.uleb128 0x2
	.4byte	.LASF33
	.byte	0x22
	.4byte	0x58
	.byte	0x8c
	.uleb128 0x2
	.4byte	.LASF34
	.byte	0x23
	.4byte	0x1bb
	.byte	0x90
	.uleb128 0x2
	.4byte	.LASF35
	.byte	0x24
	.4byte	0x1cb
	.byte	0xe4
	.byte	0
	.uleb128 0x10
	.4byte	0x9b
	.uleb128 0x9
	.4byte	0x58
	.4byte	0x1bb
	.uleb128 0xa
	.4byte	0x70
	.byte	0x5
	.byte	0
	.uleb128 0x9
	.4byte	0x58
	.4byte	0x1cb
	.uleb128 0xa
	.4byte	0x70
	.byte	0x14
	.byte	0
	.uleb128 0x9
	.4byte	0x58
	.4byte	0x1db
	.uleb128 0xa
	.4byte	0x70
	.byte	0x3
	.byte	0
	.uleb128 0x14
	.string	"AUX"
	.byte	0x8
	.byte	0x27
	.4byte	0x1ff
	.uleb128 0x1b
	.string	"IRQ"
	.byte	0x1
	.byte	0x28
	.byte	0xc
	.4byte	0x58
	.byte	0
	.uleb128 0x2
	.4byte	.LASF37
	.byte	0x29
	.4byte	0x58
	.byte	0x4
	.byte	0
	.uleb128 0x10
	.4byte	0x1db
	.uleb128 0x14
	.string	"SPI"
	.byte	0x40
	.byte	0x2c
	.4byte	0x29f
	.uleb128 0x2
	.4byte	.LASF38
	.byte	0x2d
	.4byte	0x58
	.byte	0
	.uleb128 0x2
	.4byte	.LASF39
	.byte	0x2e
	.4byte	0x58
	.byte	0x4
	.uleb128 0x2
	.4byte	.LASF40
	.byte	0x2f
	.4byte	0x58
	.byte	0x8
	.uleb128 0x2
	.4byte	.LASF41
	.byte	0x30
	.4byte	0x58
	.byte	0xc
	.uleb128 0x2
	.4byte	.LASF34
	.byte	0x31
	.4byte	0x1cb
	.byte	0x10
	.uleb128 0x2
	.4byte	.LASF42
	.byte	0x32
	.4byte	0x58
	.byte	0x20
	.uleb128 0x2
	.4byte	.LASF43
	.byte	0x33
	.4byte	0x58
	.byte	0x24
	.uleb128 0x2
	.4byte	.LASF44
	.byte	0x34
	.4byte	0x58
	.byte	0x28
	.uleb128 0x2
	.4byte	.LASF45
	.byte	0x35
	.4byte	0x58
	.byte	0x2c
	.uleb128 0x2
	.4byte	.LASF46
	.byte	0x36
	.4byte	0x58
	.byte	0x30
	.uleb128 0x2
	.4byte	.LASF47
	.byte	0x37
	.4byte	0x58
	.byte	0x34
	.uleb128 0x2
	.4byte	.LASF48
	.byte	0x38
	.4byte	0x58
	.byte	0x38
	.uleb128 0x2
	.4byte	.LASF49
	.byte	0x39
	.4byte	0x58
	.byte	0x3c
	.byte	0
	.uleb128 0x10
	.4byte	0x204
	.uleb128 0x1
	.4byte	.LASF51
	.byte	0x3c
	.byte	0x14
	.4byte	0x2be
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL9MMIO_BASE
	.uleb128 0x7
	.4byte	0x2c3
	.uleb128 0xe
	.4byte	0x2b9
	.uleb128 0x4
	.byte	0x1
	.byte	0x8
	.4byte	.LASF50
	.uleb128 0xe
	.4byte	0x2c3
	.uleb128 0x1
	.4byte	.LASF52
	.byte	0x3d
	.byte	0x14
	.4byte	0x2be
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL9GPIO_BASE
	.uleb128 0x1
	.4byte	.LASF53
	.byte	0x3e
	.byte	0x15
	.4byte	0x2be
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL8AUX_BASE
	.uleb128 0x1
	.4byte	.LASF54
	.byte	0x4f
	.byte	0x17
	.4byte	0x64
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL10GPIO_INPUT
	.uleb128 0x1
	.4byte	.LASF55
	.byte	0x50
	.byte	0x17
	.4byte	0x64
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL11GPIO_OUTPUT
	.uleb128 0x1
	.4byte	.LASF56
	.byte	0x51
	.byte	0x17
	.4byte	0x64
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL11GPIO_ALTFN0
	.uleb128 0x1
	.4byte	.LASF57
	.byte	0x52
	.byte	0x17
	.4byte	0x64
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL11GPIO_ALTFN1
	.uleb128 0x1
	.4byte	.LASF58
	.byte	0x53
	.byte	0x17
	.4byte	0x64
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL11GPIO_ALTFN2
	.uleb128 0x1
	.4byte	.LASF59
	.byte	0x54
	.byte	0x17
	.4byte	0x64
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL11GPIO_ALTFN3
	.uleb128 0x1
	.4byte	.LASF60
	.byte	0x55
	.byte	0x17
	.4byte	0x64
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL11GPIO_ALTFN4
	.uleb128 0x1
	.4byte	.LASF61
	.byte	0x56
	.byte	0x17
	.4byte	0x64
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL11GPIO_ALTFN5
	.uleb128 0x1
	.4byte	.LASF62
	.byte	0x58
	.byte	0x17
	.4byte	0x64
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL9GPIO_NONE
	.uleb128 0x1
	.4byte	.LASF63
	.byte	0x59
	.byte	0x17
	.4byte	0x64
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL8GPIO_PUP
	.uleb128 0x1
	.4byte	.LASF64
	.byte	0x5a
	.byte	0x17
	.4byte	0x64
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL8GPIO_PDP
	.uleb128 0x1
	.4byte	.LASF65
	.byte	0x73
	.byte	0x17
	.4byte	0x64
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL25SPI_CNTL0_DOUT_HOLD_SHIFT
	.uleb128 0x1
	.4byte	.LASF66
	.byte	0x74
	.byte	0x17
	.4byte	0x64
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL18SPI_CNTL0_CS_SHIFT
	.uleb128 0x1
	.4byte	.LASF67
	.byte	0x75
	.byte	0x17
	.4byte	0x64
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL21SPI_CNTL0_SPEED_SHIFT
	.uleb128 0x1
	.4byte	.LASF68
	.byte	0x77
	.byte	0x17
	.4byte	0x64
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL19SPI_CNTL0_POSTINPUT
	.uleb128 0x1
	.4byte	.LASF69
	.byte	0x78
	.byte	0x17
	.4byte	0x64
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL16SPI_CNTL0_VAR_CS
	.uleb128 0x1
	.4byte	.LASF70
	.byte	0x79
	.byte	0x17
	.4byte	0x64
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL19SPI_CNTL0_VAR_WIDTH
	.uleb128 0x1
	.4byte	.LASF71
	.byte	0x7a
	.byte	0x17
	.4byte	0x64
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL16SPI_CNTL0_Enable
	.uleb128 0x1
	.4byte	.LASF72
	.byte	0x7b
	.byte	0x17
	.4byte	0x64
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL19SPI_CNTL0_In_Rising
	.uleb128 0x1
	.4byte	.LASF73
	.byte	0x7c
	.byte	0x17
	.4byte	0x64
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL21SPI_CNTL0_Clear_FIFOs
	.uleb128 0x1
	.4byte	.LASF74
	.byte	0x7d
	.byte	0x17
	.4byte	0x64
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL20SPI_CNTL0_Out_Rising
	.uleb128 0x1
	.4byte	.LASF75
	.byte	0x7e
	.byte	0x17
	.4byte	0x64
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL20SPI_CNTL0_Invert_CLK
	.uleb128 0x1
	.4byte	.LASF76
	.byte	0x7f
	.byte	0x17
	.4byte	0x64
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL20SPI_CNTL0_SO_MSB_FST
	.uleb128 0x1
	.4byte	.LASF77
	.byte	0x80
	.byte	0x17
	.4byte	0x64
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL19SPI_CNTL0_MAX_SHIFT
	.uleb128 0x1
	.4byte	.LASF78
	.byte	0x82
	.byte	0x17
	.4byte	0x64
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL23SPI_CNTL1_CS_HIGH_SHIFT
	.uleb128 0x1
	.4byte	.LASF79
	.byte	0x84
	.byte	0x17
	.4byte	0x64
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL20SPI_CNTL1_Keep_Input
	.uleb128 0x1
	.4byte	.LASF80
	.byte	0x85
	.byte	0x17
	.4byte	0x64
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL20SPI_CNTL1_SI_MSB_FST
	.uleb128 0x1
	.4byte	.LASF81
	.byte	0x86
	.byte	0x17
	.4byte	0x64
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL18SPI_CNTL1_Done_IRQ
	.uleb128 0x1
	.4byte	.LASF82
	.byte	0x87
	.byte	0x17
	.4byte	0x64
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL19SPI_CNTL1_TX_EM_IRQ
	.uleb128 0x1
	.4byte	.LASF83
	.byte	0x89
	.byte	0x17
	.4byte	0x64
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL21SPI_STAT_TX_FIFO_MASK
	.uleb128 0x1
	.4byte	.LASF84
	.byte	0x8a
	.byte	0x17
	.4byte	0x64
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL21SPI_STAT_RX_FIFO_MASK
	.uleb128 0x1
	.4byte	.LASF85
	.byte	0x8b
	.byte	0x17
	.4byte	0x64
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL16SPI_STAT_TX_FULL
	.uleb128 0x1
	.4byte	.LASF86
	.byte	0x8c
	.byte	0x17
	.4byte	0x64
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL17SPI_STAT_TX_EMPTY
	.uleb128 0x1
	.4byte	.LASF87
	.byte	0x8d
	.byte	0x17
	.4byte	0x64
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL16SPI_STAT_RX_FULL
	.uleb128 0x1
	.4byte	.LASF88
	.byte	0x8e
	.byte	0x17
	.4byte	0x64
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL17SPI_STAT_RX_EMPTY
	.uleb128 0x1
	.4byte	.LASF89
	.byte	0x8f
	.byte	0x17
	.4byte	0x64
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL13SPI_STAT_BUSY
	.uleb128 0x1
	.4byte	.LASF90
	.byte	0x90
	.byte	0x17
	.4byte	0x64
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL21SPI_STAT_BIT_CNT_MASK
	.uleb128 0x1
	.4byte	.LASF91
	.byte	0xc5
	.byte	0x13
	.4byte	0x2ca
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL8UART_RHR
	.uleb128 0x1
	.4byte	.LASF92
	.byte	0xc6
	.byte	0x13
	.4byte	0x2ca
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL8UART_THR
	.uleb128 0x1
	.4byte	.LASF93
	.byte	0xc7
	.byte	0x13
	.4byte	0x2ca
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL8UART_IER
	.uleb128 0x1
	.4byte	.LASF94
	.byte	0xc8
	.byte	0x13
	.4byte	0x2ca
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL8UART_IIR
	.uleb128 0x1
	.4byte	.LASF95
	.byte	0xc9
	.byte	0x13
	.4byte	0x2ca
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL8UART_FCR
	.uleb128 0x1
	.4byte	.LASF96
	.byte	0xca
	.byte	0x13
	.4byte	0x2ca
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL8UART_LCR
	.uleb128 0x1
	.4byte	.LASF97
	.byte	0xcb
	.byte	0x13
	.4byte	0x2ca
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL8UART_MCR
	.uleb128 0x1
	.4byte	.LASF98
	.byte	0xcc
	.byte	0x13
	.4byte	0x2ca
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL8UART_LSR
	.uleb128 0x1
	.4byte	.LASF99
	.byte	0xcd
	.byte	0x13
	.4byte	0x2ca
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL8UART_MSR
	.uleb128 0x1
	.4byte	.LASF100
	.byte	0xce
	.byte	0x13
	.4byte	0x2ca
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL8UART_SPR
	.uleb128 0x1
	.4byte	.LASF101
	.byte	0xcf
	.byte	0x13
	.4byte	0x2ca
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL10UART_TXLVL
	.uleb128 0x1
	.4byte	.LASF102
	.byte	0xd0
	.byte	0x13
	.4byte	0x2ca
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL10UART_RXLVL
	.uleb128 0x1
	.4byte	.LASF103
	.byte	0xd1
	.byte	0x13
	.4byte	0x2ca
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL10UART_IODir
	.uleb128 0x1
	.4byte	.LASF104
	.byte	0xd2
	.byte	0x13
	.4byte	0x2ca
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL12UART_IOState
	.uleb128 0x1
	.4byte	.LASF105
	.byte	0xd3
	.byte	0x13
	.4byte	0x2ca
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL13UART_IOIntEna
	.uleb128 0x1
	.4byte	.LASF106
	.byte	0xd4
	.byte	0x13
	.4byte	0x2ca
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL13UART_reserved
	.uleb128 0x1
	.4byte	.LASF107
	.byte	0xd5
	.byte	0x13
	.4byte	0x2ca
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL14UART_IOControl
	.uleb128 0x1
	.4byte	.LASF108
	.byte	0xd6
	.byte	0x13
	.4byte	0x2ca
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL9UART_EFCR
	.uleb128 0x1
	.4byte	.LASF109
	.byte	0xd8
	.byte	0x13
	.4byte	0x2ca
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL8UART_DLL
	.uleb128 0x1
	.4byte	.LASF110
	.byte	0xd9
	.byte	0x13
	.4byte	0x2ca
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL8UART_DLH
	.uleb128 0x1
	.4byte	.LASF111
	.byte	0xda
	.byte	0x13
	.4byte	0x2ca
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL8UART_EFR
	.uleb128 0x1
	.4byte	.LASF112
	.byte	0xdb
	.byte	0x13
	.4byte	0x2ca
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL8UART_TCR
	.uleb128 0x1
	.4byte	.LASF113
	.byte	0xdc
	.byte	0x13
	.4byte	0x2ca
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL8UART_TLR
	.uleb128 0x1
	.4byte	.LASF114
	.byte	0xdf
	.byte	0x13
	.4byte	0x2ca
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL18UART_CHANNEL_SHIFT
	.uleb128 0x1
	.4byte	.LASF115
	.byte	0xe0
	.byte	0x13
	.4byte	0x2ca
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL15UART_ADDR_SHIFT
	.uleb128 0x1
	.4byte	.LASF116
	.byte	0xe1
	.byte	0x13
	.4byte	0x2ca
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL16UART_READ_ENABLE
	.uleb128 0x1
	.4byte	.LASF117
	.byte	0xe2
	.byte	0x13
	.4byte	0x2ca
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL22UART_FCR_TX_FIFO_RESET
	.uleb128 0x1
	.4byte	.LASF118
	.byte	0xe3
	.byte	0x13
	.4byte	0x2ca
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL22UART_FCR_RX_FIFO_RESET
	.uleb128 0x1
	.4byte	.LASF119
	.byte	0xe4
	.byte	0x13
	.4byte	0x2ca
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL16UART_FCR_FIFO_EN
	.uleb128 0x1
	.4byte	.LASF120
	.byte	0xe5
	.byte	0x13
	.4byte	0x2ca
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL21UART_LCR_DIV_LATCH_EN
	.uleb128 0x1
	.4byte	.LASF121
	.byte	0xe6
	.byte	0x13
	.4byte	0x2ca
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL28UART_EFR_ENABLE_ENHANCED_FNS
	.uleb128 0x1
	.4byte	.LASF122
	.byte	0xe7
	.byte	0x13
	.4byte	0x2ca
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZL20UART_IOControl_RESET
	.uleb128 0x1c
	.4byte	.LASF166
	.byte	0x1
	.2byte	0x13a
	.byte	0x11
	.8byte	.LFB16
	.8byte	.LFE16-.LFB16
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x8cf
	.uleb128 0x6
	.string	"c"
	.2byte	0x13a
	.byte	0x1c
	.4byte	0x2c3
	.uleb128 0x2
	.byte	0x91
	.sleb128 -1
	.byte	0
	.uleb128 0x15
	.4byte	.LASF125
	.2byte	0x133
	.4byte	0x959
	.8byte	.LFB15
	.8byte	.LFE15-.LFB15
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x959
	.uleb128 0x5
	.4byte	.LASF123
	.2byte	0x133
	.byte	0x2c
	.4byte	0x95b
	.uleb128 0x2
	.byte	0x91
	.sleb128 -40
	.uleb128 0x6
	.string	"src"
	.2byte	0x133
	.byte	0x4b
	.4byte	0x965
	.uleb128 0x2
	.byte	0x91
	.sleb128 -48
	.uleb128 0x6
	.string	"n"
	.2byte	0x133
	.byte	0x57
	.4byte	0x77
	.uleb128 0x2
	.byte	0x91
	.sleb128 -56
	.uleb128 0xb
	.string	"sit"
	.2byte	0x134
	.byte	0xb
	.4byte	0x2b9
	.uleb128 0x2
	.byte	0x91
	.sleb128 -8
	.uleb128 0xc
	.4byte	.LASF124
	.2byte	0x135
	.byte	0xb
	.4byte	0x2b9
	.uleb128 0x2
	.byte	0x91
	.sleb128 -16
	.uleb128 0xd
	.8byte	.LBB6
	.8byte	.LBE6-.LBB6
	.uleb128 0xb
	.string	"i"
	.2byte	0x136
	.byte	0x11
	.4byte	0x77
	.uleb128 0x2
	.byte	0x91
	.sleb128 -24
	.byte	0
	.byte	0
	.uleb128 0x1d
	.byte	0x8
	.uleb128 0x16
	.4byte	0x959
	.uleb128 0x7
	.4byte	0x96a
	.uleb128 0x16
	.4byte	0x960
	.uleb128 0x1e
	.uleb128 0x15
	.4byte	.LASF126
	.2byte	0x12d
	.4byte	0x959
	.8byte	.LFB14
	.8byte	.LFE14-.LFB14
	.uleb128 0x1
	.byte	0x9c
	.4byte	0x9d4
	.uleb128 0x6
	.string	"s"
	.2byte	0x12d
	.byte	0x1f
	.4byte	0x959
	.uleb128 0x2
	.byte	0x91
	.sleb128 -24
	.uleb128 0x6
	.string	"c"
	.2byte	0x12d
	.byte	0x26
	.4byte	0x3c
	.uleb128 0x2
	.byte	0x91
	.sleb128 -28
	.uleb128 0x6
	.string	"n"
	.2byte	0x12d
	.byte	0x30
	.4byte	0x77
	.uleb128 0x2
	.byte	0x91
	.sleb128 -40
	.uleb128 0xd
	.8byte	.LBB5
	.8byte	.LBE5-.LBB5
	.uleb128 0xb
	.string	"it"
	.2byte	0x12e
	.byte	0xe
	.4byte	0x2b9
	.uleb128 0x2
	.byte	0x91
	.sleb128 -8
	.byte	0
	.byte	0
	.uleb128 0x11
	.4byte	.LASF127
	.2byte	0x117
	.4byte	.LASF136
	.8byte	.LFB13
	.8byte	.LFE13-.LFB13
	.uleb128 0x1
	.byte	0x9c
	.4byte	0xa9a
	.uleb128 0x5
	.4byte	.LASF128
	.2byte	0x117
	.byte	0x17
	.4byte	0x77
	.uleb128 0x3
	.byte	0x91
	.sleb128 -72
	.uleb128 0x5
	.4byte	.LASF129
	.2byte	0x117
	.byte	0x2a
	.4byte	0x77
	.uleb128 0x3
	.byte	0x91
	.sleb128 -80
	.uleb128 0x6
	.string	"buf"
	.2byte	0x117
	.byte	0x43
	.4byte	0xa9a
	.uleb128 0x3
	.byte	0x91
	.sleb128 -88
	.uleb128 0x5
	.4byte	.LASF130
	.2byte	0x117
	.byte	0x4f
	.4byte	0x77
	.uleb128 0x3
	.byte	0x91
	.sleb128 -96
	.uleb128 0xb
	.string	"max"
	.2byte	0x118
	.byte	0x17
	.4byte	0x83
	.uleb128 0x9
	.byte	0x3
	.8byte	_ZZ9uart_putsmmPKcmE3max
	.uleb128 0xc
	.4byte	.LASF131
	.2byte	0x119
	.byte	0x8
	.4byte	0xa9f
	.uleb128 0x2
	.byte	0x91
	.sleb128 -56
	.uleb128 0xc
	.4byte	.LASF132
	.2byte	0x11b
	.byte	0xa
	.4byte	0x77
	.uleb128 0x2
	.byte	0x91
	.sleb128 -8
	.uleb128 0xd
	.8byte	.LBB4
	.8byte	.LBE4-.LBB4
	.uleb128 0xc
	.4byte	.LASF133
	.2byte	0x11d
	.byte	0xf
	.4byte	0x77
	.uleb128 0x2
	.byte	0x91
	.sleb128 -16
	.uleb128 0xc
	.4byte	.LASF134
	.2byte	0x11d
	.byte	0x19
	.4byte	0x77
	.uleb128 0x2
	.byte	0x91
	.sleb128 -24
	.byte	0
	.byte	0
	.uleb128 0x7
	.4byte	0x2ca
	.uleb128 0x9
	.4byte	0x2c3
	.4byte	0xaaf
	.uleb128 0xa
	.4byte	0x70
	.byte	0x1f
	.byte	0
	.uleb128 0x11
	.4byte	.LASF135
	.2byte	0x112
	.4byte	.LASF137
	.8byte	.LFB12
	.8byte	.LFE12-.LFB12
	.uleb128 0x1
	.byte	0x9c
	.4byte	0xafc
	.uleb128 0x5
	.4byte	.LASF128
	.2byte	0x112
	.byte	0x17
	.4byte	0x77
	.uleb128 0x2
	.byte	0x91
	.sleb128 -8
	.uleb128 0x5
	.4byte	.LASF129
	.2byte	0x112
	.byte	0x2a
	.4byte	0x77
	.uleb128 0x2
	.byte	0x91
	.sleb128 -16
	.uleb128 0x6
	.string	"c"
	.2byte	0x112
	.byte	0x3c
	.4byte	0x2c3
	.uleb128 0x2
	.byte	0x91
	.sleb128 -17
	.byte	0
	.uleb128 0x1f
	.4byte	.LASF167
	.byte	0x1
	.2byte	0x10d
	.byte	0x6
	.4byte	.LASF168
	.4byte	0x2c3
	.8byte	.LFB11
	.8byte	.LFE11-.LFB11
	.uleb128 0x1
	.byte	0x9c
	.4byte	0xb42
	.uleb128 0x5
	.4byte	.LASF128
	.2byte	0x10d
	.byte	0x17
	.4byte	0x77
	.uleb128 0x2
	.byte	0x91
	.sleb128 -8
	.uleb128 0x5
	.4byte	.LASF129
	.2byte	0x10d
	.byte	0x2a
	.4byte	0x77
	.uleb128 0x2
	.byte	0x91
	.sleb128 -16
	.byte	0
	.uleb128 0x11
	.4byte	.LASF138
	.2byte	0x107
	.4byte	.LASF139
	.8byte	.LFB10
	.8byte	.LFE10-.LFB10
	.uleb128 0x1
	.byte	0x9c
	.4byte	0xb73
	.uleb128 0x5
	.4byte	.LASF128
	.2byte	0x107
	.byte	0x19
	.4byte	0x58
	.uleb128 0x2
	.byte	0x91
	.sleb128 -4
	.byte	0
	.uleb128 0xf
	.4byte	.LASF142
	.byte	0xf8
	.8byte	.LFB9
	.8byte	.LFE9-.LFB9
	.uleb128 0x1
	.byte	0x9c
	.4byte	0xbe7
	.uleb128 0x3
	.4byte	.LASF128
	.byte	0xf8
	.byte	0x26
	.4byte	0x77
	.uleb128 0x2
	.byte	0x91
	.sleb128 -24
	.uleb128 0x3
	.4byte	.LASF129
	.byte	0xf8
	.byte	0x39
	.4byte	0x77
	.uleb128 0x2
	.byte	0x91
	.sleb128 -32
	.uleb128 0x3
	.4byte	.LASF140
	.byte	0xf8
	.byte	0x4d
	.4byte	0x77
	.uleb128 0x2
	.byte	0x91
	.sleb128 -40
	.uleb128 0x1
	.4byte	.LASF141
	.byte	0xfb
	.byte	0xc
	.4byte	0x58
	.uleb128 0x2
	.byte	0x91
	.sleb128 -8
	.uleb128 0xd
	.8byte	.LBB3
	.8byte	.LBE3-.LBB3
	.uleb128 0xb
	.string	"i"
	.2byte	0x104
	.byte	0xc
	.4byte	0x3c
	.uleb128 0x2
	.byte	0x91
	.sleb128 -4
	.byte	0
	.byte	0
	.uleb128 0x20
	.4byte	.LASF161
	.byte	0x1
	.byte	0xf0
	.byte	0xd
	.4byte	0x2c3
	.8byte	.LFB8
	.8byte	.LFE8-.LFB8
	.uleb128 0x1
	.byte	0x9c
	.4byte	0xc50
	.uleb128 0x3
	.4byte	.LASF128
	.byte	0xf0
	.byte	0x28
	.4byte	0x77
	.uleb128 0x2
	.byte	0x91
	.sleb128 -24
	.uleb128 0x3
	.4byte	.LASF129
	.byte	0xf0
	.byte	0x3b
	.4byte	0x77
	.uleb128 0x2
	.byte	0x91
	.sleb128 -32
	.uleb128 0x12
	.string	"reg"
	.byte	0xf0
	.byte	0x4d
	.4byte	0x2c3
	.uleb128 0x2
	.byte	0x91
	.sleb128 -33
	.uleb128 0x8
	.string	"req"
	.byte	0xf1
	.byte	0x8
	.4byte	0xc50
	.uleb128 0x2
	.byte	0x91
	.sleb128 -8
	.uleb128 0x8
	.string	"res"
	.byte	0xf2
	.byte	0x8
	.4byte	0xc50
	.uleb128 0x2
	.byte	0x91
	.sleb128 -16
	.byte	0
	.uleb128 0x9
	.4byte	0x2c3
	.4byte	0xc60
	.uleb128 0xa
	.4byte	0x70
	.byte	0x1
	.byte	0
	.uleb128 0xf
	.4byte	.LASF143
	.byte	0xe9
	.8byte	.LFB7
	.8byte	.LFE7-.LFB7
	.uleb128 0x1
	.byte	0x9c
	.4byte	0xcc3
	.uleb128 0x3
	.4byte	.LASF128
	.byte	0xe9
	.byte	0x28
	.4byte	0x77
	.uleb128 0x2
	.byte	0x91
	.sleb128 -24
	.uleb128 0x3
	.4byte	.LASF129
	.byte	0xe9
	.byte	0x3b
	.4byte	0x77
	.uleb128 0x2
	.byte	0x91
	.sleb128 -32
	.uleb128 0x12
	.string	"reg"
	.byte	0xe9
	.byte	0x4d
	.4byte	0x2c3
	.uleb128 0x2
	.byte	0x91
	.sleb128 -33
	.uleb128 0x3
	.4byte	.LASF144
	.byte	0xe9
	.byte	0x57
	.4byte	0x2c3
	.uleb128 0x2
	.byte	0x91
	.sleb128 -34
	.uleb128 0x8
	.string	"req"
	.byte	0xea
	.byte	0x8
	.4byte	0xc50
	.uleb128 0x2
	.byte	0x91
	.sleb128 -8
	.byte	0
	.uleb128 0xf
	.4byte	.LASF145
	.byte	0xa1
	.8byte	.LFB6
	.8byte	.LFE6-.LFB6
	.uleb128 0x1
	.byte	0x9c
	.4byte	0xd81
	.uleb128 0x3
	.4byte	.LASF146
	.byte	0xa1
	.byte	0x24
	.4byte	0x58
	.uleb128 0x2
	.byte	0x91
	.sleb128 -52
	.uleb128 0x3
	.4byte	.LASF147
	.byte	0xa1
	.byte	0x39
	.4byte	0xa9a
	.uleb128 0x2
	.byte	0x91
	.sleb128 -64
	.uleb128 0x3
	.4byte	.LASF148
	.byte	0xa1
	.byte	0x49
	.4byte	0x77
	.uleb128 0x3
	.byte	0x91
	.sleb128 -72
	.uleb128 0x3
	.4byte	.LASF149
	.byte	0xa1
	.byte	0x58
	.4byte	0x2b9
	.uleb128 0x3
	.byte	0x91
	.sleb128 -80
	.uleb128 0x3
	.4byte	.LASF150
	.byte	0xa1
	.byte	0x68
	.4byte	0x77
	.uleb128 0x3
	.byte	0x91
	.sleb128 -88
	.uleb128 0x1
	.4byte	.LASF151
	.byte	0xa2
	.byte	0xa
	.4byte	0x77
	.uleb128 0x2
	.byte	0x91
	.sleb128 -8
	.uleb128 0x1
	.4byte	.LASF152
	.byte	0xa3
	.byte	0xa
	.4byte	0x77
	.uleb128 0x2
	.byte	0x91
	.sleb128 -16
	.uleb128 0xd
	.8byte	.LBB2
	.8byte	.LBE2-.LBB2
	.uleb128 0x1
	.4byte	.LASF144
	.byte	0xa5
	.byte	0xe
	.4byte	0x58
	.uleb128 0x2
	.byte	0x91
	.sleb128 -20
	.uleb128 0x1
	.4byte	.LASF153
	.byte	0xa6
	.byte	0xc
	.4byte	0x77
	.uleb128 0x2
	.byte	0x91
	.sleb128 -32
	.uleb128 0x8
	.string	"max"
	.byte	0xbb
	.byte	0xc
	.4byte	0x77
	.uleb128 0x2
	.byte	0x91
	.sleb128 -40
	.byte	0
	.byte	0
	.uleb128 0x21
	.4byte	.LASF154
	.byte	0x1
	.byte	0x93
	.byte	0x6
	.4byte	.LASF169
	.8byte	.LFB5
	.8byte	.LFE5-.LFB5
	.uleb128 0x1
	.byte	0x9c
	.4byte	0xdce
	.uleb128 0x3
	.4byte	.LASF146
	.byte	0x93
	.byte	0x18
	.4byte	0x58
	.uleb128 0x2
	.byte	0x91
	.sleb128 -20
	.uleb128 0x8
	.string	"reg"
	.byte	0x94
	.byte	0xc
	.4byte	0x58
	.uleb128 0x2
	.byte	0x91
	.sleb128 -4
	.uleb128 0x1
	.4byte	.LASF155
	.byte	0x98
	.byte	0xc
	.4byte	0x58
	.uleb128 0x2
	.byte	0x91
	.sleb128 -8
	.byte	0
	.uleb128 0x22
	.4byte	.LASF170
	.byte	0x1
	.byte	0x6c
	.byte	0x6
	.4byte	.LASF171
	.8byte	.LFB4
	.8byte	.LFE4-.LFB4
	.uleb128 0x1
	.byte	0x9c
	.uleb128 0xf
	.4byte	.LASF156
	.byte	0x5c
	.8byte	.LFB3
	.8byte	.LFE3-.LFB3
	.uleb128 0x1
	.byte	0x9c
	.4byte	0xe5d
	.uleb128 0x12
	.string	"pin"
	.byte	0x5c
	.byte	0x21
	.4byte	0x58
	.uleb128 0x2
	.byte	0x91
	.sleb128 -20
	.uleb128 0x3
	.4byte	.LASF157
	.byte	0x5c
	.byte	0x2f
	.4byte	0x58
	.uleb128 0x2
	.byte	0x91
	.sleb128 -24
	.uleb128 0x3
	.4byte	.LASF158
	.byte	0x5c
	.byte	0x41
	.4byte	0x58
	.uleb128 0x2
	.byte	0x91
	.sleb128 -28
	.uleb128 0x8
	.string	"reg"
	.byte	0x5d
	.byte	0xc
	.4byte	0x58
	.uleb128 0x2
	.byte	0x91
	.sleb128 -4
	.uleb128 0x1
	.4byte	.LASF159
	.byte	0x5e
	.byte	0xc
	.4byte	0x58
	.uleb128 0x2
	.byte	0x91
	.sleb128 -8
	.uleb128 0x1
	.4byte	.LASF160
	.byte	0x5f
	.byte	0xc
	.4byte	0x58
	.uleb128 0x2
	.byte	0x91
	.sleb128 -12
	.byte	0
	.uleb128 0x23
	.string	"spi"
	.byte	0x1
	.byte	0x48
	.byte	0x1d
	.4byte	0xe8e
	.8byte	.LFB2
	.8byte	.LFE2-.LFB2
	.uleb128 0x1
	.byte	0x9c
	.4byte	0xe8e
	.uleb128 0x3
	.4byte	.LASF162
	.byte	0x48
	.byte	0x26
	.4byte	0x3c
	.uleb128 0x2
	.byte	0x91
	.sleb128 -4
	.byte	0
	.uleb128 0x7
	.4byte	0x29f
	.uleb128 0x24
	.string	"aux"
	.byte	0x1
	.byte	0x44
	.byte	0x1d
	.4byte	0xeb1
	.8byte	.LFB1
	.8byte	.LFE1-.LFB1
	.uleb128 0x1
	.byte	0x9c
	.uleb128 0x7
	.4byte	0x1ff
	.uleb128 0x25
	.4byte	.LASF163
	.byte	0x1
	.byte	0x40
	.byte	0x1e
	.4byte	0xed4
	.8byte	.LFB0
	.8byte	.LFE0-.LFB0
	.uleb128 0x1
	.byte	0x9c
	.uleb128 0x7
	.4byte	0x1a6
	.byte	0
	.section	.debug_abbrev,"",@progbits
.Ldebug_abbrev0:
	.uleb128 0x1
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
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x2
	.uleb128 0xd
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
	.sleb128 12
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x3
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 1
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x4
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
	.uleb128 0x5
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 1
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x6
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 1
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x7
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0x21
	.sleb128 8
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x8
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 1
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x9
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xa
	.uleb128 0x21
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0xb
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 1
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0xc
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 1
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0xd
	.uleb128 0xb
	.byte	0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.byte	0
	.byte	0
	.uleb128 0xe
	.uleb128 0x26
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xf
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 1
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0x21
	.sleb128 13
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
	.uleb128 0x10
	.uleb128 0x35
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x11
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 1
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0x21
	.sleb128 6
	.uleb128 0x6e
	.uleb128 0xe
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
	.uleb128 0x12
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 1
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x18
	.byte	0
	.byte	0
	.uleb128 0x13
	.uleb128 0x16
	.byte	0
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
	.byte	0
	.byte	0
	.uleb128 0x14
	.uleb128 0x13
	.byte	0x1
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 1
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0x21
	.sleb128 8
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x15
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0x21
	.sleb128 1
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0x21
	.sleb128 18
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x7a
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x16
	.uleb128 0x37
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x17
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
	.uleb128 0x18
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
	.uleb128 0x19
	.uleb128 0x3b
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.byte	0
	.byte	0
	.uleb128 0x1a
	.uleb128 0x13
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x1b
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x1c
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
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
	.uleb128 0x1d
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x1e
	.uleb128 0x26
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x1f
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0x19
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x39
	.uleb128 0xb
	.uleb128 0x6e
	.uleb128 0xe
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
	.uleb128 0x20
	.uleb128 0x2e
	.byte	0x1
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
	.uleb128 0x21
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
	.uleb128 0x6e
	.uleb128 0xe
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
	.uleb128 0x22
	.uleb128 0x2e
	.byte	0
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
	.uleb128 0x6e
	.uleb128 0xe
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x7
	.uleb128 0x40
	.uleb128 0x18
	.uleb128 0x7c
	.uleb128 0x19
	.byte	0
	.byte	0
	.uleb128 0x23
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3
	.uleb128 0x8
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
	.uleb128 0x7a
	.uleb128 0x19
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x24
	.uleb128 0x2e
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
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
	.uleb128 0x7a
	.uleb128 0x19
	.byte	0
	.byte	0
	.uleb128 0x25
	.uleb128 0x2e
	.byte	0
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
	.uleb128 0x7a
	.uleb128 0x19
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
.LASF145:
	.string	"spi_send_recv"
.LASF136:
	.string	"_Z9uart_putsmmPKcm"
.LASF106:
	.string	"UART_reserved"
.LASF10:
	.string	"size_t"
.LASF52:
	.string	"GPIO_BASE"
.LASF82:
	.string	"SPI_CNTL1_TX_EM_IRQ"
.LASF154:
	.string	"init_spi"
.LASF83:
	.string	"SPI_STAT_TX_FIFO_MASK"
.LASF109:
	.string	"UART_DLL"
.LASF99:
	.string	"UART_MSR"
.LASF130:
	.string	"blen"
.LASF133:
	.string	"bidx"
.LASF152:
	.string	"recvidx"
.LASF77:
	.string	"SPI_CNTL0_MAX_SHIFT"
.LASF89:
	.string	"SPI_STAT_BUSY"
.LASF161:
	.string	"uart_read_register"
.LASF11:
	.string	"long long int"
.LASF2:
	.string	"signed char"
.LASF72:
	.string	"SPI_CNTL0_In_Rising"
.LASF156:
	.string	"setup_gpio"
.LASF96:
	.string	"UART_LCR"
.LASF115:
	.string	"UART_ADDR_SHIFT"
.LASF78:
	.string	"SPI_CNTL1_CS_HIGH_SHIFT"
.LASF13:
	.string	"GPFSEL"
.LASF29:
	.string	"GPLEN1"
.LASF4:
	.string	"long int"
.LASF58:
	.string	"GPIO_ALTFN2"
.LASF59:
	.string	"GPIO_ALTFN3"
.LASF14:
	.string	"GPSET0"
.LASF15:
	.string	"GPSET1"
.LASF54:
	.string	"GPIO_INPUT"
.LASF125:
	.string	"memcpy"
.LASF138:
	.string	"init_uart"
.LASF18:
	.string	"GPLEV0"
.LASF19:
	.string	"GPLEV1"
.LASF95:
	.string	"UART_FCR"
.LASF159:
	.string	"shift"
.LASF139:
	.string	"_Z9init_uartj"
.LASF153:
	.string	"count"
.LASF98:
	.string	"UART_LSR"
.LASF76:
	.string	"SPI_CNTL0_SO_MSB_FST"
.LASF117:
	.string	"UART_FCR_TX_FIFO_RESET"
.LASF28:
	.string	"GPLEN0"
.LASF171:
	.string	"_Z9init_gpiov"
.LASF122:
	.string	"UART_IOControl_RESET"
.LASF131:
	.string	"temp"
.LASF7:
	.string	"unsigned int"
.LASF167:
	.string	"uart_getc"
.LASF148:
	.string	"sendlen"
.LASF8:
	.string	"long unsigned int"
.LASF116:
	.string	"UART_READ_ENABLE"
.LASF86:
	.string	"SPI_STAT_TX_EMPTY"
.LASF92:
	.string	"UART_THR"
.LASF64:
	.string	"GPIO_PDP"
.LASF144:
	.string	"data"
.LASF142:
	.string	"uart_init_channel"
.LASF147:
	.string	"sendbuf"
.LASF6:
	.string	"short unsigned int"
.LASF137:
	.string	"_Z9uart_putcmmc"
.LASF158:
	.string	"resistor"
.LASF67:
	.string	"SPI_CNTL0_SPEED_SHIFT"
.LASF41:
	.string	"PEEK"
.LASF134:
	.string	"tidx"
.LASF71:
	.string	"SPI_CNTL0_Enable"
.LASF121:
	.string	"UART_EFR_ENABLE_ENHANCED_FNS"
.LASF87:
	.string	"SPI_STAT_RX_FULL"
.LASF16:
	.string	"GPCLR0"
.LASF17:
	.string	"GPCLR1"
.LASF135:
	.string	"uart_putc"
.LASF165:
	.string	"decltype(nullptr)"
.LASF164:
	.string	"GNU C++17 11.3.1 20220712 -mcpu=cortex-a72 -mstrict-align -mgeneral-regs-only -mlittle-endian -mabi=lp64 -g -ffreestanding -fno-builtin -fno-rtti -fno-exceptions"
.LASF79:
	.string	"SPI_CNTL1_Keep_Input"
.LASF85:
	.string	"SPI_STAT_TX_FULL"
.LASF69:
	.string	"SPI_CNTL0_VAR_CS"
.LASF112:
	.string	"UART_TCR"
.LASF127:
	.string	"uart_puts"
.LASF118:
	.string	"UART_FCR_RX_FIFO_RESET"
.LASF94:
	.string	"UART_IIR"
.LASF170:
	.string	"init_gpio"
.LASF70:
	.string	"SPI_CNTL0_VAR_WIDTH"
.LASF12:
	.string	"long double"
.LASF66:
	.string	"SPI_CNTL0_CS_SHIFT"
.LASF42:
	.string	"IO_REGa"
.LASF43:
	.string	"IO_REGb"
.LASF44:
	.string	"IO_REGc"
.LASF45:
	.string	"IO_REGd"
.LASF22:
	.string	"GPREN0"
.LASF23:
	.string	"GPREN1"
.LASF120:
	.string	"UART_LCR_DIV_LATCH_EN"
.LASF46:
	.string	"TXHOLD_REGa"
.LASF47:
	.string	"TXHOLD_REGb"
.LASF48:
	.string	"TXHOLD_REGc"
.LASF49:
	.string	"TXHOLD_REGd"
.LASF68:
	.string	"SPI_CNTL0_POSTINPUT"
.LASF30:
	.string	"GPAREN0"
.LASF31:
	.string	"GPAREN1"
.LASF102:
	.string	"UART_RXLVL"
.LASF166:
	.string	"debug"
.LASF90:
	.string	"SPI_STAT_BIT_CNT_MASK"
.LASF55:
	.string	"GPIO_OUTPUT"
.LASF123:
	.string	"dest"
.LASF81:
	.string	"SPI_CNTL1_Done_IRQ"
.LASF100:
	.string	"UART_SPR"
.LASF63:
	.string	"GPIO_PUP"
.LASF75:
	.string	"SPI_CNTL0_Invert_CLK"
.LASF84:
	.string	"SPI_STAT_RX_FIFO_MASK"
.LASF26:
	.string	"GPHEN0"
.LASF27:
	.string	"GPHEN1"
.LASF5:
	.string	"unsigned char"
.LASF119:
	.string	"UART_FCR_FIFO_EN"
.LASF3:
	.string	"short int"
.LASF37:
	.string	"ENABLES"
.LASF140:
	.string	"baudRate"
.LASF151:
	.string	"sendidx"
.LASF143:
	.string	"uart_write_register"
.LASF149:
	.string	"recvbuf"
.LASF105:
	.string	"UART_IOIntEna"
.LASF53:
	.string	"AUX_BASE"
.LASF168:
	.string	"_Z9uart_getcmm"
.LASF35:
	.string	"PUP_PDN_CNTRL_REG"
.LASF9:
	.string	"uint32_t"
.LASF169:
	.string	"_Z8init_spij"
.LASF91:
	.string	"UART_RHR"
.LASF129:
	.string	"uartChannel"
.LASF80:
	.string	"SPI_CNTL1_SI_MSB_FST"
.LASF50:
	.string	"char"
.LASF124:
	.string	"cdest"
.LASF141:
	.string	"bauddiv"
.LASF110:
	.string	"UART_DLH"
.LASF65:
	.string	"SPI_CNTL0_DOUT_HOLD_SHIFT"
.LASF162:
	.string	"index"
.LASF20:
	.string	"GPEDS0"
.LASF21:
	.string	"GPEDS1"
.LASF150:
	.string	"recvlen"
.LASF107:
	.string	"UART_IOControl"
.LASF111:
	.string	"UART_EFR"
.LASF73:
	.string	"SPI_CNTL0_Clear_FIFOs"
.LASF51:
	.string	"MMIO_BASE"
.LASF101:
	.string	"UART_TXLVL"
.LASF56:
	.string	"GPIO_ALTFN0"
.LASF57:
	.string	"GPIO_ALTFN1"
.LASF24:
	.string	"GPFEN0"
.LASF25:
	.string	"GPFEN1"
.LASF60:
	.string	"GPIO_ALTFN4"
.LASF61:
	.string	"GPIO_ALTFN5"
.LASF40:
	.string	"STAT"
.LASF126:
	.string	"memset"
.LASF38:
	.string	"CNTL0"
.LASF39:
	.string	"CNTL1"
.LASF160:
	.string	"status"
.LASF62:
	.string	"GPIO_NONE"
.LASF108:
	.string	"UART_EFCR"
.LASF32:
	.string	"GPAFEN0"
.LASF33:
	.string	"GPAFEN1"
.LASF113:
	.string	"UART_TLR"
.LASF146:
	.string	"channel"
.LASF163:
	.string	"gpio"
.LASF74:
	.string	"SPI_CNTL0_Out_Rising"
.LASF34:
	.string	"_unused"
.LASF155:
	.string	"speed"
.LASF103:
	.string	"UART_IODir"
.LASF157:
	.string	"setting"
.LASF104:
	.string	"UART_IOState"
.LASF36:
	.string	"GPIO"
.LASF114:
	.string	"UART_CHANNEL_SHIFT"
.LASF128:
	.string	"spiChannel"
.LASF93:
	.string	"UART_IER"
.LASF88:
	.string	"SPI_STAT_RX_EMPTY"
.LASF97:
	.string	"UART_MCR"
.LASF132:
	.string	"tlen"
	.section	.debug_line_str,"MS",@progbits,1
.LASF0:
	.string	"rpi.cc"
.LASF1:
	.string	"/u2/t28cai/cs452/A1/k1"
	.ident	"GCC: (Arm GNU Toolchain 11.3.Rel1) 11.3.1 20220712"
