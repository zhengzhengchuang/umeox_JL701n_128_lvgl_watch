#ifndef __LP_TIMER_API_H__
#define __LP_TIMER_API_H__


void lp_timer_init(P11_LPTMR_TypeDef *lp_timer, u32 us);

void lp_timer_register_irq(P11_LPTMR_TypeDef *lp_timer, void (*handle)(void));

#endif /* #ifndef __LP_TIMER_API_H__ */
