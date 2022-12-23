#ifndef _SYSR_INTERRUPT_H
#define _SYSR_INTERRUPT_H

#include "mmio_base.h"

#define IRQ_ER_1 (MMIO_BASE + 0xB210) // See page 141 of BCM Manual
#define IRQ_PENDING_1 (MMIO_BASE + 0xB204) // See page 141

#endif