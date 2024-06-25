#ifndef  __IRQ_H__
#define  __IRQ_H__

#include "hwi.h"

#define ___interrupt    __attribute__((interrupt("")))

void irq_set_pending(u32 irq_idx);

void irq_common_handler(u32 irq_idx);

#endif
