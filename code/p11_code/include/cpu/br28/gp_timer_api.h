#ifndef __GP_TIMER_API_H__
#define __GP_TIMER_API_H__


void gp_timer_init(P11_GPTMR_TypeDef *gp_timer, u32 us);

void gp_timer_close(P11_GPTMR_TypeDef *gp_timer);

void gp_timer_register_irq(P11_GPTMR_TypeDef *gp_timer, void (*handle)(void));

void gp_timer_unregister_irq(P11_GPTMR_TypeDef *gp_timer);

void gp_timer_output_pwm(P11_GPTMR_TypeDef *gp_timer, u32 gpio, u32 freq, u32 duty);

enum GP_TIMER_CAPTURE_EDGE_TABLE {
    GP_TIMER_CAPTURE_EDGE_UP = 0,
    GP_TIMER_CAPTURE_EDGE_DOWN,
};
void gp_timer_capture(P11_GPTMR_TypeDef *gp_timer, u32 gpio, enum GP_TIMER_CAPTURE_EDGE_TABLE edge, u8 irflt_enable);

#endif /* #ifndef __GP_TIMER_API_H__ */
