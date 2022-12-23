#ifndef _EVI_H
#define _EVI_H

// Defines the 16 entries in the exception vector table.
// See page 1876 of aarch64 manual.

// Row 1 - Current Exception level with SP_EL0.
#define SYNCHRONOUS_EL1_SP_EL0  0 
#define IRQ_EL1_SP_EL0          1 
#define FIQ_EL1_SP_EL0          2 
#define SERROR_EL1_SP_EL0       3 

// Row 2 - Current Exception level with SP_ELx, x>0.
#define SYNCHRONOUS_EL1_SP_EL1 4 
#define IRQ_EL1_SP_EL1          5 
#define FIQ_EL1_SP_EL1 6 
#define SERROR_EL1_SP_EL1 7 

// Row 3 - Lower Exception level, where the implemented level immediately lower than the target level is using AArch64.
#define SYNCHRONOUS_EL0_AARCH64 8 
#define IRQ_EL0_AARCH64 9 
#define FIQ_EL0_AARCH64 10 
#define SERROR_EL0_AARCH64 11 

// Row 4 - Lower Exception level, where the implemented level immediately lower than the target level is using AArch32.
#define SYNCHRONOUS_EL0_AARCH32 12 
#define IRQ_EL0_AARCH32 13 
#define FIQ_EL0_AARCH32 14 
#define SERROR_EL0_AARCH32 15 

#endif