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
	// more to come
};

static volatile GICD* const gicd = (GICD*)(GIC_BASE + 0x1000);
static volatile GICC* const gicc = (GICC*)(GIC_BASE + 0x2000);

void Interrupt::init_interrupt() {
	gicd->GICD_CTLR = 1; // GICD
	gicc->GICC_CTLR = 1; // GICC

	Clock::enable_clock_one_interrupts();
}
void Interrupt::enable_interrupt_for(uint32_t id) {

	gicd->GICD_ISENABLERN[id / 32] = 1 << (id % 32);
	// also setup GICD ITARGETSRn to route to cpu 0
	gicd->GICD_ITARGETSRN[id / 4] = 1 << (8 * (id % 4));
}