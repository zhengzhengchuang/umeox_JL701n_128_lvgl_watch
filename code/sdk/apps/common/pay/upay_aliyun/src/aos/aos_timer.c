/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "aos_system.h"
#include "aos/kernel.h"
/* #include "os/os_api.h" */
#include "app_config.h"

#if TCFG_PAY_ALIOS_ENABLE && (TCFG_PAY_ALIOS_WAY_SEL==TCFG_PAY_ALIOS_WAY_ALIYUN)

struct aos_timer_apapter {
    struct list_head entry;
    u32 id;
    void *arg;
    void (*fn)(void *, void *);
    u32 ms;
    u8 repeat;
    u8 run;
};

static struct list_head aos_timer_head = LIST_HEAD_INIT(aos_timer_head);

static spinlock_t aos_timer_lock = {.rwlock = 0};

static void aos_timer_apapter_handler(void *arg)
{
    void (*func)(void *, void *) = NULL;
    struct aos_timer_apapter *p;
    void *arg0;
    void *arg1;

    spin_lock(&aos_timer_lock);
    list_for_each_entry(p, &aos_timer_head, entry) {
        if ((void *)p == (void *)arg) {
            func = p->fn;
            arg0 = (void *)p;
            arg1 = p->arg;
            if (p->repeat == false) {
                p->run = false;
            }
            break;
        }
    }
    spin_unlock(&aos_timer_lock);

    if (func) {
        func(arg0, arg1);
    }
}

AOS_API_NO_INLNE
aos_status_t aos_timer_create(aos_timer_t *timer, void (*fn)(void *, void *), void *arg,
                              uint32_t ms, uint32_t options)
{
    //TODO:
    // rt_err_t err;

    // //rt_timer_t  handle;
    // RT_ASSERT((timer!=RT_NULL)&&(fn!=RT_NULL));




    // adpt_timer_t p_atimer = (adpt_timer_t) rt_malloc(sizeof(struct adpt_timer));

    // if(RT_NULL == p_atimer)
    // {
    // 	return 1;
    // }
    // else
    // {

    // 	rt_uint8_t flag = RT_TIMER_FLAG_SOFT_TIMER; //Use soft timer defaultly.

    // 	if(AOS_TIMER_REPEAT&options)
    // 	{

    // 		flag |= RT_TIMER_FLAG_PERIODIC;
    // 	}


    // 	p_atimer->arg = arg;
    // 	p_atimer->fn  = fn;
    // 	rt_timer_init(&p_atimer->handle,
    // 					"AosTim",
    // 					timer_common_cb,
    // 					(void *) p_atimer,
    // 					rt_tick_from_millisecond((rt_int32_t) ms), flag);

    // 	*timer = (aos_timer_t) p_atimer;

    // 	if(AOS_TIMER_AUTORUN&options)
    // 	{
    // 		err = rt_timer_start(&p_atimer->handle);
    // 	}
    // 	else
    // 	{
    // 		err = RT_EOK;
    // 	}
    // 	return (RT_EOK == err) ? 0 : 1;
    // }

    bool repeat, auto_run;
    int err = 0;

    struct aos_timer_apapter *p;

    if (timer == NULL) {
        return -EINVAL;
    }

    AOS_API_CALL_DEBUG("timer: 0x%x, fn: 0x%x, arg: 0x%x, ms: %d, options: 0x%x", (u32)timer, fn, arg, ms, options);

    repeat = (options & AOS_TIMER_REPEAT) ? true : false;
    auto_run = (options & AOS_TIMER_AUTORUN) ? true : false;

    struct aos_timer_apapter *aos_t = (struct aos_timer_apapter *)zalloc(sizeof(struct aos_timer_apapter));

    if (aos_t == NULL) {
        return 1;
    }

    aos_t->arg = arg;
    aos_t->ms = ms;
    aos_t->fn = fn;
    aos_t->repeat = repeat;
    aos_t->run = false;


    spin_lock(&aos_timer_lock);
    printf("timer: add tail: 0x%x", aos_t);
    list_add_tail(&(aos_t->entry), &(aos_timer_head));
    spin_unlock(&aos_timer_lock);

    *timer = (aos_timer_t)aos_t;

    if (auto_run) {
        err = aos_timer_start(timer);
    }

    return err;
}


AOS_API_NO_INLNE
void aos_timer_free(aos_timer_t *timer)
{
    AOS_API_CALL_DEBUG("timer: 0x%x, *timer: 0x%x", timer, *timer);
    //TODO:
    // rt_err_t err;
    // adpt_timer_t p_atimer = (adpt_timer_t) timer;
    // RT_ASSERT(timer!=RT_NULL);

    // err = rt_timer_detach(&p_atimer->handle);
    // RT_ASSERT(RT_EOK == err);

    // rt_free(p_atimer);

    struct aos_timer_apapter *p;

    spin_lock(&aos_timer_lock);

    if ((*timer) == NULL) {
        spin_unlock(&aos_timer_lock);
        return;
    }

    list_for_each_entry(p, &aos_timer_head, entry) {
        if ((void *)p == (void *)(*timer)) {
            if (p->run == true) {
                usr_timer_del(p->id);
                p->run = false;
            }
            list_del(&(p->entry));
            free(p);
            *timer = NULL;
            break;
        }
    }
    spin_unlock(&aos_timer_lock);
}

AOS_API_NO_INLNE
int aos_timer_start(aos_timer_t *timer)
{
    AOS_API_CALL_DEBUG("timer: 0x%x, *timer: 0x%x", timer, *timer);
    //TODO:
    // rt_err_t err;
    // adpt_timer_t p_atimer = (adpt_timer_t) timer;

    // RT_ASSERT(timer!=RT_NULL);
    // err = rt_timer_start(&p_atimer->handle);
    // return (RT_EOK == err) ? 0 : 1;

    struct aos_timer_apapter *p;

    spin_lock(&aos_timer_lock);

    if ((*timer) == NULL) {
        spin_unlock(&aos_timer_lock);
        return 1;
    }

    list_for_each_entry(p, &aos_timer_head, entry) {
        printf("p: 0x%x, timer: 0x%x", p, *timer);
        if ((void *)p == (void *)(*timer)) {
            if ((p->run == false) && (p->ms)) {
                if (p->repeat) {
                    printf("usr timer add");
                    p->id = usr_timer_add(p, aos_timer_apapter_handler, p->ms, 0);
                } else {
                    printf("usr timeout add");
                    p->id = usr_timeout_add(p, aos_timer_apapter_handler, p->ms, 0);
                }
                p->run = true;
            }
            break;
        }
    }
    spin_unlock(&aos_timer_lock);

    return 0;
}

AOS_API_NO_INLNE
int aos_timer_stop(aos_timer_t *timer)
{
    AOS_API_CALL_DEBUG("timer: 0x%x, *timer: 0x%x", timer, *timer);
    //TODO:
    // rt_err_t err;
    // adpt_timer_t p_atimer = (adpt_timer_t) timer;
    // RT_ASSERT(timer!=RT_NULL);

    // err = rt_timer_stop(&p_atimer->handle);
    // return (RT_EOK == err) ? 0 : 1;

    struct aos_timer_apapter *p;

    spin_lock(&aos_timer_lock);

    if ((*timer) == NULL) {
        spin_unlock(&aos_timer_lock);
        return 1;
    }

    list_for_each_entry(p, &aos_timer_head, entry) {
        if ((void *)p == (void *)(*timer)) {
            if (p->run == true) {
                usr_timer_del(p->id);
                p->run = false;
            }
            break;
        }
    }
    spin_unlock(&aos_timer_lock);

    return 0;
}

#endif /* #if TCFG_PAY_ALIOS_ENABLE && (TCFG_PAY_ALIOS_WAY_SEL==TCFG_PAY_ALIOS_WAY_ALIYUN) */
