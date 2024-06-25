/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include "aos_system.h"
#include "aos/kernel.h"
/* #include "os/os_api.h" */
#include "app_config.h"

#if TCFG_PAY_ALIOS_ENABLE && (TCFG_PAY_ALIOS_WAY_SEL==TCFG_PAY_ALIOS_WAY_ALIYUN)

extern int os_queue_create(OS_QUEUE **queue, u32 qsize);
extern int os_queue_del(OS_QUEUE **queue, int opt);
extern int os_queue_post(OS_QUEUE **queue, int *msg, u32 cnt);
extern int os_queue_pend(OS_QUEUE **queue, int *msg, u32 *cnt, uint32_t timeout);

AOS_API_NO_INLNE
aos_status_t aos_queue_new(aos_queue_t *queue, void *buf, size_t size, size_t max_msg)
{
    AOS_API_CALL_DEBUG("queue: 0x%x, buf: 0x%x, size: 0x%x, max_msg: 0x%x", queue, buf, size, max_msg);
    //TODO:
    // rt_err_t err;

    // RT_ASSERT(queue!=RT_NULL);

    // rt_mq_t  rt_mq = (rt_mq_t) rt_malloc(sizeof(struct rt_messagequeue));

    // if(RT_NULL == rt_mq)
    // {
    // 	return 1;
    // }
    // else
    // {
    // 	(void)(max_msg);//Unused
    //     buf=rt_malloc(size);
    //     RT_ASSERT(buf!=RT_NULL);
    // 	err = rt_mq_init(rt_mq,"Aos_q",buf,size/max_msg,size,RT_IPC_FLAG_FIFO);
    // 	if(RT_EOK == err)
    // 	{
    // 		*queue = (aos_queue_t) rt_mq;
    // 		return 0;
    // 	}
    // 	else
    // 	{
    // 		return 1;
    // 	}
    // }
    int err = os_queue_create((OS_QUEUE **)queue, size);

    return (err == OS_ERR_NONE) ? 0 : 1;
}

AOS_API_NO_INLNE
void aos_queue_free(aos_queue_t *queue)
{
    //TODO:
    // rt_err_t err;
    // rt_mq_t rt_q;

    // RT_ASSERT(queue!=RT_NULL);
    // rt_q=(rt_mq_t)(*queue);
    // err = rt_mq_detach(rt_q);
    // RT_ASSERT(RT_EOK == err);

    // if (rt_q->msg_pool)
    //     rt_free(rt_q->msg_pool);
    // rt_free((void *) *queue);
    ASSERT(queue && (*queue));

    AOS_API_CALL_DEBUG("queue: 0x%x, *queue: 0x%x", queue, *queue);

    int err = os_queue_del((OS_QUEUE **)queue, OS_DEL_ALWAYS);

    /* return (err == OS_ERR_NONE) ? 0 : 1; */
}

AOS_API_NO_INLNE
aos_status_t aos_queue_send(aos_queue_t *queue, void *msg, size_t size)
{
    //TODO:
    // rt_err_t err;

    // RT_ASSERT((queue!=RT_NULL)&&(msg!=RT_NULL));

    // err = rt_mq_send((rt_mq_t) *queue, msg, (rt_size_t) size);
    // return (RT_EOK == err) ? 0 : 1;

    ASSERT(queue && (*queue));
    ASSERT((size % 4) == 0);

    AOS_API_CALL_DEBUG("queue: 0x%x, *queue: 0x%x, msg: 0x%x, size: 0x%x", queue, *queue, msg, size);

    int err = os_queue_post((OS_QUEUE **)queue, msg, size / 4);

    return (err == OS_ERR_NONE) ? 0 : 1;
}

AOS_API_NO_INLNE
aos_status_t aos_queue_recv(aos_queue_t *queue, uint32_t ms, void *msg, size_t *size)
{
    //TODO:
    // rt_err_t err;

    // RT_ASSERT((queue!=RT_NULL)&&(msg!=RT_NULL));

    // err = rt_mq_recv((rt_mq_t) *queue, msg, (rt_size_t) size, rt_tick_from_millisecond((rt_int32_t) ms));
    // return (RT_EOK == err) ? 0 : 1;


    u32 cnt;

    ASSERT(queue && (*queue));

    AOS_API_CALL_DEBUG("queue: 0x%x, *queue: 0x%x, msg: 0x%x, size: 0x%x, ms: %d", queue, *queue, msg, size, ms);

    int err = os_queue_pend((OS_QUEUE **)queue, msg, &cnt, aos_ms_to_tick(ms));
    *size = cnt * 4;

    return (err == OS_ERR_NONE) ? 0 : 1;
}

AOS_API_NO_INLNE
bool aos_queue_is_valid(aos_queue_t *queue)
{
    //TODO:
    if ((queue == NULL) || (*queue == NULL)) {
        return false;
    }

    return true;
}


#endif /* #if TCFG_PAY_ALIOS_ENABLE && (TCFG_PAY_ALIOS_WAY_SEL==TCFG_PAY_ALIOS_WAY_ALIYUN) */
