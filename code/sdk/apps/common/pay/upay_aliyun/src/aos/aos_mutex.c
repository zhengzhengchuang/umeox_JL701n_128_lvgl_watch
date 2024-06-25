/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include "aos_system.h"
#include "aos/kernel.h"
/* #include "os/os_api.h" */
#include "app_config.h"

#if TCFG_PAY_ALIOS_ENABLE && (TCFG_PAY_ALIOS_WAY_SEL==TCFG_PAY_ALIOS_WAY_ALIYUN)
#undef AOS_API_CALL_DEBUG
#define AOS_API_CALL_DEBUG

AOS_API_NO_INLNE
aos_status_t aos_mutex_new(aos_mutex_t *mutex)
{
    AOS_API_CALL_DEBUG("mutex: 0x%x", mutex);

    //TODO:
    // RT_ASSERT(mutex!=RT_NULL);
    // rt_mutex_t handle = rt_mutex_create("AOS_mtx", RT_IPC_FLAG_FIFO);

    // if(RT_NULL == handle)
    // {
    // 	return 1;
    // }
    // else
    // {
    // 	*mutex = (aos_mutex_t) handle;
    //     return 0;
    // }
    OS_MUTEX *mutex_ptr;

    if (mutex) {
        mutex_ptr = malloc(sizeof(OS_MUTEX));
        *mutex = mutex_ptr;
        if (mutex_ptr == NULL) {
            return 2;
        }
        os_mutex_create(mutex_ptr);
    } else {
        return 1;
    }

    return 0;
}

AOS_API_NO_INLNE
void aos_mutex_free(aos_mutex_t *mutex)
{
    //TODO:
    // rt_err_t err;

    // RT_ASSERT(mutex!=RT_NULL);
    // err = rt_mutex_delete((rt_mutex_t) *mutex);
    // RT_ASSERT(RT_EOK == err);

    if (mutex && (*mutex)) {
        os_mutex_del((OS_MUTEX *)(*mutex), 0);
        free((void *)(*mutex));
        *mutex = NULL;
    }
}

AOS_API_NO_INLNE
int aos_mutex_lock(aos_mutex_t *mutex, unsigned int timeout)
{
    //TODO:
    // rt_err_t err;

    // RT_ASSERT(mutex!=RT_NULL);
    // err = rt_mutex_take((rt_mutex_t) *mutex, timeout);
    // return (RT_EOK == err) ? 0 : 1;

    ASSERT(mutex && (*mutex));

    AOS_API_CALL_DEBUG("mutex: 0x%x, *mutex: 0x%x, timeout: 0x%x", mutex, *mutex, timeout);

    int err;
    if (timeout == AOS_WAIT_FOREVER) {
        err = os_mutex_pend((OS_MUTEX *)(*mutex), 0);
    } else if (timeout == AOS_NO_WAIT) {
        //err = OSMutexAccept((OS_EVENT *)(*mutex));
        err = os_mutex_pend((OS_MUTEX *)(*mutex), -1);
        if (err == 0) {
            err = OS_NO_ERR;
        } else {
            err = -1;
        }
    } else {
        err = os_mutex_pend((OS_MUTEX *)(*mutex), aos_ms_to_tick(timeout));
    }

    return (err == OS_NO_ERR) ? 0 : -ETIMEDOUT;
}

AOS_API_NO_INLNE
int aos_mutex_unlock(aos_mutex_t *mutex)
{
    //TODO:
    // rt_err_t err;

    // RT_ASSERT(mutex!=RT_NULL);
    // err = rt_mutex_release((rt_mutex_t) *mutex);

    // return (RT_EOK == err) ? 0 : 1;

    ASSERT(mutex && (*mutex));

    AOS_API_CALL_DEBUG("mutex: 0x%x, *mutex: 0x%x", mutex, *mutex);

    int err = os_mutex_post((OS_MUTEX *)(*mutex));

    return (err == OS_NO_ERR) ? 0 : -1;
}

AOS_API_NO_INLNE
bool aos_mutex_is_valid(aos_mutex_t *mutex)
{
    //TODO:
    // return (mutex&&(*mutex));
    AOS_API_CALL_DEBUG("mutex: 0x%x", mutex);

    if ((mutex == NULL) || (*mutex == NULL)) {
        return false;
    }

    return true;
}

#endif /* #if TCFG_PAY_ALIOS_ENABLE && (TCFG_PAY_ALIOS_WAY_SEL==TCFG_PAY_ALIOS_WAY_ALIYUN) */
