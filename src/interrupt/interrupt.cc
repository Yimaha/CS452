#include "interrupt.h"

struct GICD {
	uint32_t GICD_CTLR;
	uint32_t GICD_TYPER;
	uint32_t GICD_IIDR;
	uint32_t reserv1[29];
	uint32_t GICD_IGROUPRn[32];
	uint32_t GICD_ISENABLERN[32];
	uint32_t GICC_ICENABLERN[32];
	// Fill up space to 0x800
	uint32_t _unused2[384];
	uint32_t GICD_ITARGETSRN[254];
};

struct GICC {
	uint32_t GICC_CTLR;
	uint32_t GICC_PMR;
	uint32_t GICC_BPR;
	uint32_t GICC_IAR;
	uint32_t GICC_EOIR;
	uint32_t GICC_RPR;
	uint32_t GICC_HPPIR;
	uint32_t GICC_ABPR;
	uint32_t GICC_AIAR;
	uint32_t GICC_AEOIR;
	uint32_t GICC_AHPPIR;
};

static volatile GICD* const gicd = (GICD*)(GIC_BASE + 0x1000);
static volatile GICC* const gicc = (GICC*)(GIC_BASE + 0x2000);

void Interrupt::init_interrupt() {
	gicd->GICD_CTLR = 1; // GICD
	gicc->GICC_CTLR = 1; // GICC

	Clock::enable_clock_one_interrupts();
	UART::enable_uart_interrupt();
}

uint32_t Interrupt::get_interrupt_id() {
	return gicc->GICC_IAR;
}

bool Interrupt::is_interrupt_clear() {
	return (gicc->GICC_IAR & 0x3ff) == 1023; // when 1023 is read interrupt is cleared, I think?
}

void Interrupt::end_interrupt(uint32_t id) {
	gicc->GICC_EOIR = id;
}

void Interrupt::enable_interrupt_for(uint32_t id) {

	gicd->GICD_ISENABLERN[id / 32] = 1 << (id % 32);
	// also setup GICD ITARGETSRn to route to cpu 0
	gicd->GICD_ITARGETSRN[id / 4] = 1 << (8 * (id % 4));
}