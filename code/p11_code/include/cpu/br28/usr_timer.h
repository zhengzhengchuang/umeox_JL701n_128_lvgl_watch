#ifndef __USR_TIMER_H__
#define __USR_TIMER_H__

void usr_timer_init();

u16 usr_timer_add(void *priv, void (*func)(void *priv), u32 msec, u8 priority);

void usr_timer_del(u16 t);

#endif
