#include "sys_timer.h"
#include "jiffies.h"
#include "spinlock.h"
//#include "cpu.h"

#define TIMER_US_ENABLE		0

void (*sys_timer_delay_handler)();
struct list_head timer_head;
struct list_head timer_us_head;
volatile unsigned long jiffies;
volatile unsigned long jiffies_us;

extern void timer1_init(void (*cb)(void));

void *get_sys_timer_head()
{
    return &timer_head;
}

u32 get_jiffies(u8 mode, u32 timer_ms)
{
    if (mode == 1) {
        jiffies += timer_ms / 10;

    }
    return jiffies;
}

bool __timer_find(struct sys_timer *timer)
{
    struct sys_timer *p;

    list_for_each_entry(p, &timer_head, entry) {
        if (p == timer) {
            return TRUE;
        }
    }

    return FALSE;
}

#if TIMER_US_ENABLE
bool __timer_us_find(struct sys_timer *timer)
{
    struct sys_timer *p;

    list_for_each_entry(p, &timer_us_head, entry) {
        if (p == timer) {
            return TRUE;
        }
    }
    return FALSE;
}
#endif

void __timer_insert(struct sys_timer *timer)
{
    struct sys_timer *p;
    list_for_each_entry(p, &timer_head, entry) {
        if (p == timer) {
            list_del(&p->entry);
            break;
        }
    }

    list_for_each_entry(p, &timer_head, entry) {
        if (p->jiffies > timer->jiffies) {
            __list_add(&timer->entry, p->entry.prev, &p->entry);
            return;
        }
    }

    list_add_tail((struct list_head *)&timer->entry, (struct list_head *)&timer_head);
}

#if TIMER_US_ENABLE
void __timer_us_insert(struct sys_timer *timer)
{
    struct sys_timer *p;
    list_for_each_entry(p, &timer_us_head, entry) {
        if (p == timer) {
            list_del(&p->entry);
            break;
        }
    }

    list_for_each_entry(p, &timer_us_head, entry) {
        if (p->jiffies > timer->jiffies) {
            __list_add(&timer->entry, p->entry.prev, &p->entry);
            return;
        }
    }

    list_add_tail((struct list_head *)&timer->entry, (struct list_head *)&timer_us_head);
}
#endif

#if TIMER_US_ENABLE
void sys_timer_us_register(struct sys_timer *timer, u32 us_sec,
                           void (*fun)(struct sys_timer *timer), u8 delay_do)
{
    local_irq_disable();

    timer->loop = 0;
    timer->delay_do = delay_do;
    timer->fun = fun;
    timer->jiffies = jiffies_us + msecs_to_jiffies_10(us_sec);
    //printf("timer->jiffies=%d,%d\n", jiffies_us, timer->jiffies);
    __timer_us_insert(timer);

    local_irq_enable();
}
#endif

void sys_hi_timer_schedule()
{
    struct sys_timer *p, *n;

    local_irq_disable();

    jiffies++;
    list_for_each_entry_safe(p, n, &timer_head, entry) {
        if (time_before(jiffies, p->jiffies)) {
            break;
        }
        //putchar('s');
        if (sys_timer_delay_handler) {
            sys_timer_delay_handler();
        }

    }

    local_irq_enable();
}

#if TIMER_US_ENABLE
static void sys_timer_us_schedule()
{
    struct sys_timer *p, *n;
    u8 do_fun_flag = 0;

    local_irq_disable();

    jiffies_us++;
    list_for_each_entry_safe(p, n, &timer_us_head, entry) {
        if (time_before(jiffies_us, p->jiffies)) {
            break;
        }
        if (sys_timer_delay_handler) {
            sys_timer_delay_handler();
        }
    }
    local_irq_enable();
}
void loop_timer_us_schedule()
{
    struct sys_timer *p, *n;
    u8 do_fun_flag = 0;

    local_irq_disable();

    list_for_each_entry_safe(p, n, &timer_us_head, entry) {
        if (time_before(jiffies_us, p->jiffies)) {
            break;
        }
        sys_timer_us_remove(p);
        do_fun_flag = 0xaa;
        break;
    }

    local_irq_enable();
    if (do_fun_flag == 0xaa) {
        ASSERT(p->fun != NULL);
        p->fun(p);
    }
}
#endif


static DEFINE_SPINLOCK(lock);
#define TIMER_POOL_NUM_CONFIG	10
static struct sys_timer timer_pool[TIMER_POOL_NUM_CONFIG] = {0};
static u16 global_id = 0;


static struct sys_timer *__sys_timer_get(void *priv, void (*func)(void *priv),
        u32 msec, int timeout)
{
    struct sys_timer *t;

    spin_lock(&lock);
    int i;
    for (i = 0; i < ARRAY_SIZE(timer_pool); i++) {
        if (timer_pool[i].used == 0) {
            timer_pool[i].used = 1;
            t = &timer_pool[i];
            spin_unlock(&lock);
            goto __next;
        }
    }

    spin_unlock(&lock);
    if (i == ARRAY_SIZE(timer_pool)) {
        /* puts("<<<tiemr pool full>>>\n"); */
    }

__next:
    t->priv = priv;
    t->func = func;
    t->msec = msec;
    t->del 	= 0;
    //t->posting = 0;
    t->timeout = timeout;
    t->jiffies = jiffies + msecs_to_jiffies(msec);

    t->id = ++global_id;
    if (t->id == 0) {
        t->id = (u16)func;
    }

    return t;
}

static int __timer_put(struct sys_timer *timer)
{
    if (timer >= timer_pool && timer < timer_pool + ARRAY_SIZE(timer_pool)) {
        timer->used = 0;
        return 1;
    }
    return 0;
}

static void __id_check(struct sys_timer *t, struct list_head *head)
{
    struct sys_timer *p;

__again:
    list_for_each_entry(p, head, entry) {
        if (t->id == p->id) {
            t->id++;
            goto __again;
        }
    }
}

static u16 __sys_timer_add(void *priv, void (*func)(void *priv), u32 msec, int timeout)
{
    struct sys_timer *t;

    t = __sys_timer_get(priv, func, msec, timeout);
    if (!t) {
        return 0;
    }

    spin_lock(&lock);

    __id_check(t, &timer_head);

    struct sys_timer *p;
    list_for_each_entry(p, &timer_head, entry) {
        //printf("p->jiffies:%x t->jiffies:%x\n",p-jiffies,t->jiffies);
        if (p->jiffies > t->jiffies) {
            __list_add(&t->entry, p->entry.prev, &p->entry);
            goto _LOOP_RET;
        }
    }

    list_add_tail(&t->entry, &timer_head);
_LOOP_RET:
    spin_unlock(&lock);

    /* if(func == lmp_connection_timeout){ */
    /*     mpu_set(0, (u32)t + sizeof(struct list_head), (u32)t + sizeof(struct list_head) + 4 - 1, 0,  "Cxr"); */
    /* } */
    //log_debug("add id : %d", t->id);

    //os_sem_post(&sys_timer_sem);

    return t->id;
}
static void __timer_del(struct sys_timer *timer)
{
    spin_lock(&lock);

    __list_del_entry(&timer->entry);

    int ret = __timer_put(timer);

    spin_unlock(&lock);

    /* if (!ret) {
        free(timer);
    } */
}

static void __sys_timer_del(struct list_head *head, u16 id)
{
    const char *task;
    struct sys_timer *p;

    spin_lock(&lock);

    list_for_each_entry(p, head, entry) {
        if (p->id == id) {
            __list_del_entry(&p->entry);
            __timer_put(p);
            //p->del = 1;
            break;
        }
    }

    spin_unlock(&lock);
}

u16 sys_timer_add(void *priv, void (*func)(void *priv), u32 msec)
{
    int rets;
    __asm__ volatile("%0 = rets" :"=r"(rets));
    /* printf("add rts : 0x%x / %d", rets, msec); */
    return __sys_timer_add(priv, func, msec, 0);
}

u16 sys_timeout_add(void *priv, void (*func)(void *priv), u32 msec)
{
    int rets;
    __asm__ volatile("%0 = rets" :"=r"(rets));
    /* printf("add ot rts : 0x%x / %d", rets, msec); */
    return __sys_timer_add(priv, func, msec, 1);
}

void sys_timer_del(u16 t)
{
    __sys_timer_del(&timer_head, t);
}

void sys_timeout_del(u16 t)
{
    __sys_timer_del(&timer_head, t);
}

int sys_timer_modify(u16 id, u32 msec)
{
    struct sys_timer *p;

    spin_lock(&lock);

    list_for_each_entry(p, &timer_head, entry) {
        if (p->id == id) {
            p->msec = msec;
            p->jiffies = jiffies + msecs_to_jiffies(msec);
            break;
        }
    }

    spin_unlock(&lock);

    //os_sem_post(&sys_timer_sem);

    return 0;
}
void sys_timer_set_user_data(u16 id, void *priv)
{
    struct sys_timer *p;

    spin_lock(&lock);

    list_for_each_entry(p, &timer_head, entry) {
        if (p->id == id) {
            p->priv = priv;
        }
    }

    spin_unlock(&lock);
}

sys_timer sys_timer_register(u32 msec, void (*callback)(void *))
{
    return sys_timeout_add(NULL, callback, msec);
}

void sys_timer_change_period(sys_timer timer, u32 msec)
{
    sys_timer_modify(timer, msec);
}

void sys_timer_set_context(sys_timer timer, void *context)
{
    sys_timer_set_user_data(timer, context);
}

sys_timer sys_timer_register_periodic(u32 msec, void (*callback)(void *))
{
    return sys_timer_add(NULL, callback, msec);
}

void sys_timer_re_run(u16 id)
{
    struct sys_timer *p;

    spin_lock(&lock);

    list_for_each_entry(p, &timer_head, entry) {
        if (p->id == id) {
            p->jiffies = jiffies + msecs_to_jiffies(p->msec);
            break;
        }
    }

    spin_unlock(&lock);

    //os_sem_post(&sys_timer_sem);
}

void sys_timer_reset(sys_timer timer)
{
    sys_timer_re_run(timer);
}

void sys_timer_remove(sys_timer timer)
{
    sys_timeout_del(timer);
}


void sys_timer_schedule()
{
    struct sys_timer *p, *n;
    u8 do_fun_flag = 0;

#if TIMER_US_ENABLE
    loop_timer_us_schedule();
#endif

    jiffies++;
    spin_lock(&lock);
    list_for_each_entry_safe(p, n, &timer_head, entry) {
        if (time_after(jiffies, p->jiffies)) {
            spin_unlock(&lock);
            if (p->func) {
                p->func(p->priv);
            }

            spin_lock(&lock);
            if (p->timeout) {
                spin_unlock(&lock);
                __timer_del(p);
                spin_lock(&lock);
            } else {
                p->jiffies = jiffies + msecs_to_jiffies(p->msec);
            }
        }
    }
    spin_unlock(&lock);
}

void sys_timer_init()
{
    INIT_LIST_HEAD(&timer_head);
#if TIMER_US_ENABLE
    INIT_LIST_HEAD(&timer_us_head);
#endif

    timer1_init(sys_timer_schedule);
}
