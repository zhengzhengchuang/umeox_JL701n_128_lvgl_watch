/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include "aos_system.h"
#include "aos/kernel.h"
/* #include "os/os_api.h" */
#include "app_config.h"


#if TCFG_PAY_ALIOS_ENABLE && (TCFG_PAY_ALIOS_WAY_SEL==TCFG_PAY_ALIOS_WAY_ALIYUN)

AOS_API_NO_INLNE
aos_status_t aos_sem_new(aos_sem_t *sem, uint32_t count)
{
    AOS_API_CALL_DEBUG("sem: 0x%x, count: 0x%x", sem, count);
    //TODO:
    // RT_ASSERT(sem!=RT_NULL);

    // rt_sem_t handle = rt_sem_create("Aos_sem", (rt_uint32_t) count, RT_IPC_FLAG_FIFO);

    // if(RT_NULL == handle)
    // {
    // 	return 1;
    // }
    // else
    // {
    // 	*sem = (aos_sem_t) handle;
    //     return 0;
    // }

    OS_SEM *sem_ptr;

    if (sem) {
        sem_ptr = malloc(sizeof(OS_MUTEX));
        *sem = sem_ptr;
        if (sem_ptr == NULL) {
            return 2;
        }
        os_sem_create(sem_ptr, (int)count);
    } else {
        return 1;
    }

    return 0;
}

AOS_API_NO_INLNE
void aos_sem_free(aos_sem_t *sem)
{
    //TODO:
    // rt_err_t err;

    // RT_ASSERT(sem!=RT_NULL);
    // err = rt_sem_delete((rt_sem_t) *sem);
    // RT_ASSERT(RT_EOK == err);

    if (sem && (*sem)) {
        AOS_API_CALL_DEBUG("sem: 0x%x, *sem: 0x%x", sem, *sem);
        os_mutex_del((OS_SEM *)(*sem), 0);
        free((void *)(*sem));
        *sem = NULL;
    }

}

AOS_API_NO_INLNE
int aos_sem_wait(aos_sem_t *sem, unsigned int timeout)
{
    //TODO:
    // rt_err_t err;
    // RT_ASSERT(sem!=RT_NULL);
    // err = rt_sem_take((rt_sem_t) *sem, (rt_int32_t) timeout);

    // return (RT_EOK == err) ? 0 : 1;

    int err;

    ASSERT(sem && (*sem));

    AOS_API_CALL_DEBUG("sem: 0x%x, *sem: 0x%x, timeout: 0x%x", sem, *sem, timeout);

    if (timeout == AOS_WAIT_FOREVER) {
        err = os_sem_pend((OS_SEM *)(*sem), 0);
    } else if (timeout == AOS_NO_WAIT) {
        err = os_sem_pend((OS_SEM *)(*sem), -1);
    } else {
        err = os_sem_pend((OS_SEM *)(*sem), aos_ms_to_tick(timeout));
    }

    return (err == OS_NO_ERR) ? 0 : -ETIMEDOUT;
}

AOS_API_NO_INLNE
void aos_sem_signal(aos_sem_t *sem)
{
    //TODO:

    // rt_err_t err;

    // RT_ASSERT(sem!=RT_NULL);
    // err = rt_sem_release((rt_sem_t) *sem);
    // RT_ASSERT(RT_EOK == err);

    ASSERT(sem && (*sem));

    AOS_API_CALL_DEBUG("sem: 0x%x, *sem: 0x%x", sem, *sem);

    os_sem_post((OS_SEM *)(*sem));
}

AOS_API_NO_INLNE
bool aos_sem_is_valid(aos_sem_t *sem)
{
    //TODO:
    // return (sem&&(*sem));

    if ((sem == NULL) || (*sem == NULL)) {
        return false;
    }

    return true;
}

AOS_API_NO_INLNE
void aos_sem_signal_all(aos_sem_t *sem)
{
    //TODO:
    // rt_err_t err;

    // RT_ASSERT(sem!=RT_NULL);
    // err = rt_sem_control((rt_sem_t) *sem, RT_IPC_CMD_RESET, 0);
    // RT_ASSERT(RT_EOK == err);

    ASSERT(sem && (*sem));

    AOS_API_CALL_DEBUG("sem: 0x%x, *sem: 0x%x", sem, *sem);

    aos_sem_signal(sem);
}

#endif /* #if TCFG_PAY_ALIOS_ENABLE && (TCFG_PAY_ALIOS_WAY_SEL==TCFG_PAY_ALIOS_WAY_ALIYUN) */
