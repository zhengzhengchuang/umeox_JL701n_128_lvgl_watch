#ifndef __POWER_API_H__
#define __POWER_API_H__

#include "gpio.h"

enum LOW_POWER_LEVEL {
    LOW_POWER_MODE_IDLE = 0,
    LOW_POWER_MODE_STANDBY,
    LOW_POWER_MODE_SLEEP,
    LOW_POWER_MODE_DEEP_SLEEP,
};

//typedef u8(*idle_handler_t)(void);
typedef void(*low_power_enter)(enum LOW_POWER_LEVEL level);
typedef void(*low_power_exit)(enum LOW_POWER_LEVEL level);
typedef enum LOW_POWER_LEVEL(*level_handler_t)(void);

struct low_power_target {
    char *name;
    level_handler_t level;
    low_power_enter enter;
    low_power_exit exit;
    //idle_handler_t is_idle;
};

extern const struct low_power_target low_power_target_begin[];
extern const struct low_power_target low_power_target_end[];

#define list_for_each_low_power_target(p) \
    for (p = low_power_target_begin; p < low_power_target_end; p++)

//priv: 2 > 1 > 0
#define REGISTER_LOWPOWER_TARGET(target, priv) \
        static const struct low_power_target target SEC_USED(.lp_target.priv.ops)

enum LOW_POWER_LEVEL p11_low_power_target_query(void);

void p11_low_power_target_enter(enum LOW_POWER_LEVEL level);

void p11_low_power_target_exit(enum LOW_POWER_LEVEL level);

extern volatile u8 p2m_message[];
extern volatile u8 m2p_message[];

void power_init(void);

void __power_recover(void);

void p11_power_hw_manage(void);

#endif /* #ifndef __POWER_HW_H__ */
