/*
 * @Description:
 * @Author: HJY
 * @Date: 2022-12-07 15:20:43
 * @LastEditTime: 2023-01-10 11:05:53
 * @LastEditors: HJY
 */
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
void *aos_zalloc(size_t size)
{
    AOS_API_CALL_DEBUG("size: 0x%x", size);
    //TODO:
    // void *ptr;
    // ptr = rt_malloc((rt_size_t) size);

    // if(RT_NULL != ptr)
    // {
    // 	memset(ptr, 0, size);
    // }
    // return ptr;
    return zalloc(size);
}

AOS_API_NO_INLNE
void *aos_malloc(size_t size)
{
    AOS_API_CALL_DEBUG("size: 0x%x", size);
    //TODO:
    // return rt_malloc((rt_size_t) size);;
    return malloc(size);
}

AOS_API_NO_INLNE
void *aos_calloc(size_t nitems, size_t size)
{
    AOS_API_CALL_DEBUG("nitems: 0x%x, size: 0x%x", nitems, size);
    //TODO:
    // return rt_calloc((rt_size_t) nitems, (rt_size_t) size);
    return zalloc(nitems * size);
}

AOS_API_NO_INLNE
void *_calloc_r(struct _reent *ptr, size_t size, size_t len)
{
    return aos_calloc(size, len);
}

#if 0
void *
calloc(size_t size, size_t len)
{
    size_t cnt = size * len;
    if (cnt == 0) {
        return (void *)0;
    }
    void *p = malloc(size * len);
    memset(p, 0, cnt);
    return p;
}

#endif



AOS_API_NO_INLNE
void *aos_realloc(void *mem, size_t size)
{
    AOS_API_CALL_DEBUG("mem: 0x%x, size: 0x%x", mem, size);
    //TODO:
    // return rt_realloc(mem, (rt_size_t) size);
    return realloc(mem, size);
}

AOS_API_NO_INLNE
void aos_alloc_trace(void *addr, size_t allocator) {}

AOS_API_NO_INLNE
void aos_free(void *mem)
{
    AOS_API_CALL_DEBUG("mem: 0x%x", mem);
    //TODO:
    // rt_free(mem);
    free(mem);
}

#endif /* #if TCFG_PAY_ALIOS_ENABLE && (TCFG_PAY_ALIOS_WAY_SEL==TCFG_PAY_ALIOS_WAY_ALIYUN) */
