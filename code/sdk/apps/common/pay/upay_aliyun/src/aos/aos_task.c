/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */


#include "aos_system.h"
#include "aos/kernel.h"
/* #include "os/os_api.h" */
#include "app_config.h"

#if TCFG_PAY_ALIOS_ENABLE && (TCFG_PAY_ALIOS_WAY_SEL==TCFG_PAY_ALIOS_WAY_ALIYUN)

AOS_API_NO_INLNE
int aos_task_new(const char *name, void (*fn)(void *), void *arg,
                 size_t stack_size)
{

    //TODO:

    // rt_thread_t handle = rt_thread_create(name,fn,arg,
    // 			(rt_uint32_t)stack_size,
    // 		    RT_THREAD_PRIORITY_APP,
    // 		    10);
    // rt_thread_startup(handle);
    // return (NULL == handle) ? 1 : 0;
    AOS_API_CALL_DEBUG("name: %s, fn = 0x%x, arg: 0x%x, stack_size: 0x%x", name, fn, arg, stack_size);

    stack_size = (stack_size + 3) / 4;
    stack_size = (stack_size + 31) / 32 * 32;
    int err = os_task_create(fn, arg, 1, stack_size, 0, (s8 *)name);

    return (err == OS_ERR_NONE) ? 0 : 1;
}

AOS_API_NO_INLNE
aos_status_t aos_task_new_ext(aos_task_t *task, const char *name, void (*fn)(void *),
                              void *arg, size_t stack_size, int32_t prio)
{
    AOS_API_CALL_DEBUG("taskp: 0x%x, name: %s, fn = 0x%x, arg: 0x%x, stack_size: 0x%x", task, name, fn, arg, stack_size);
    //TODO:
    //  rt_thread_t handle;

    //  // TODO: Check compatibility with RT-Thread, pri max(RT_THREAD_PRIORITY_IDLE) is set to 32.
    //  prio=RT_THREAD_PRIORITY_MIDDLE;
    //  RT_ASSERT((prio < RT_THREAD_PRIORITY_IDLE) && (prio > RT_THREAD_PRIORITY_CRITIAL));

    //  handle = rt_thread_create(name,fn,arg,
    // 			 (rt_uint32_t)stack_size,
    // 			 prio,
    // 			 10);
    //  rt_thread_startup(handle);
    //  return (NULL == handle) ? 1 : 0;

    stack_size = (stack_size + 3) / 4;
    stack_size = (stack_size + 31) / 32 * 32;
    int err = os_task_create(fn, arg, prio, stack_size, 0, (s8 *)name);
    if (err == OS_ERR_NONE) {
        *task = os_task_get_handle(name);
    } else {
        *task = NULL;
    }

    return (err == OS_ERR_NONE) ? 0 : 1;
}

extern const char *os_current_task_rom();

static void ___aos_task_kill(char *name)
{
    if (name) {
        AOS_API_CALL_DEBUG("kill task: %s: 0x%x", name);
        task_kill(name);
        free(name);
    }
}

AOS_API_NO_INLNE
void aos_task_exit(int code)
{
    AOS_API_CALL_DEBUG("code: 0x%x", code);
    // LOG_I("aos_task_exit %s",&rt_thread_self()->name[0]);
    // rt_thread_exit();
    //TODO:
    int err = 0;
    const char *name = os_current_task_rom();

    char *kill_name = (char *)zalloc(64);
    u32 len = strlen(name);
    ASSERT(len < 64);
    memcpy(kill_name, name, len);

    //task_kill(name);
    //TODO: 使用发消息方式删除任务

    int msg[4];
    msg[0] = (int)___aos_task_kill;
    msg[1] = 1;
    msg[2] = (int)kill_name;
    err = os_taskq_post_type((char *)"app_core", Q_CALLBACK, 3, msg);

    while (1) {
        os_time_dly(1000);
    }
}


AOS_API_NO_INLNE
const char *aos_task_name(void)
{
    //TODO:
    // return &rt_thread_self()->name[0];
    AOS_API_CALL_DEBUG("0x%x", 1);

    return os_current_task_rom();
}

#endif /* #if TCFG_PAY_ALIOS_ENABLE && (TCFG_PAY_ALIOS_WAY_SEL==TCFG_PAY_ALIOS_WAY_ALIYUN) */
