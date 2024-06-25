#ifndef SYS_SPINLOCK_H
#define SYS_SPINLOCK_H

#include "typedef.h"
//#include "cpu.h"
//#include "irq.h"

struct __spinlock {
    volatile u32 rwlock;
};

typedef struct __spinlock spinlock_t;

extern void local_irq_disable(void);
extern void local_irq_enable(void);


#define preempt_disable() \
	local_irq_disable()

#define preempt_enable() \
	local_irq_enable()


#if CPU_CORE_NUM > 1

#define spin_acquire(lock) 	\
	do { \
		arch_spin_lock(lock); \
	}while(0)

#define spin_release(lock) \
	do { \
		arch_spin_unlock(lock); \
	}while(0)

#else

#define spin_acquire(lock) 	\
	do { \
	}while(0)


#define spin_release(lock) \
	do { \
	}while(0)

#endif


#define DEFINE_SPINLOCK(x) \
	spinlock_t x = { .rwlock = 0 }


static inline void spin_lock_init(spinlock_t *lock)
{
    lock->rwlock = 0;
}

static inline void spin_lock(spinlock_t *lock)
{
    preempt_disable();
    spin_acquire(lock);
}


static inline void spin_unlock(spinlock_t *lock)
{
    spin_release(lock);
    preempt_enable();
}

#endif



