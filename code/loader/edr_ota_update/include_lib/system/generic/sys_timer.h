#define SYS_TIMER_H


#include "generic/typedef.h"
#include "list.h"

typedef int sys_timer;

struct sys_timer {
    struct list_head entry;
    void (*func)(void *priv);
    void *priv;
    u32 jiffies;
    u32 msec: 24;
    u32 del: 1;
    u32 timeout: 1;
    u16 id;
    u8 used;
};

void sys_timer_init();
bool __timer_find(struct sys_timer *timer);
u16 sys_timer_add(void *priv, void (*func)(void *priv), u32 msec);
void sys_timer_schedule(void);
void sys_timer_delay_schedule(void);
void sys_timer_set_user(struct sys_timer *timer, u32 user);
u32 sys_timer_get_user(struct sys_timer *timer);
void sys_timer_del_schedule(void);
void loop_timer_schedule(void);
void sys_timer_re_run(u16 id);
void sys_timer_del(u16 t);
int sys_timer_modify(u16 id, u32 msec);
u16 sys_timeout_add(void *priv, void (*func)(void *priv), u32 msec);
void sys_timeout_del(u16 t);
/*
 *  For Compatible
 */
#define sys_hi_timer_schedule()\
    usr_timer_schedule()

#define sys_hi_timer_add(a, b, c)\
    sys_timer_add(a, b, c)

#define sys_hi_timeout_add(a, b, c)\
    sys_timeout_add(a, b, c)
// usr_timeout_add(a, b, c, 1)

#define sys_hi_timer_modify(a, b)\
    sys_timer_modify(a, b)

#define sys_hi_timeout_modify(a, b)\
    sys_timer_modify(a, b)

#define sys_hi_timer_del(a)\
    sys_timer_del(a)

#define sys_hi_timeout_del(a)\
    sys_timer_del(a)

#define sys_s_hi_timer_add(a, b, c)\
    sys_timer_add(a, b, c, 0)

#define sys_s_hi_timerout_add(a, b, c)\
    sys_timeout_add(a, b, c, 0)

#define sys_s_hi_timer_modify(a, b)\
    sys_timer_modify(a, b)

#define sys_s_hi_timeout_modify(a, b)\
    sys_timer_modify(a, b)

#define sys_s_hi_timer_del(a)\
    sys_timer_del(a)

#define sys_s_hi_timeout_del(a)\
    sys_timeout_del(a)
