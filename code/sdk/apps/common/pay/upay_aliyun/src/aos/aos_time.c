#include "aos_system.h"
#include "aos/kernel.h"
/* #include "os/os_api.h" */
#include "app_config.h"

#if TCFG_PAY_ALIOS_ENABLE && (TCFG_PAY_ALIOS_WAY_SEL==TCFG_PAY_ALIOS_WAY_ALIYUN)

AOS_API_NO_INLNE
uint64_t aos_now_ms(void)
{
    //TODO:
    // return (uint64_t) rt_tick_get_millisecond();

    AOS_API_CALL_DEBUG("%x", 1);

    return (uint64_t)jiffies_msec();
}

AOS_API_NO_INLNE
uint64_t aos_now(void)
{
    //TODO:
    // return (uint64_t) (rt_tick_get() / RT_TICK_PER_SECOND);

    AOS_API_CALL_DEBUG("%x", 1);
    return ((uint64_t)jiffies_msec()) * 1000LL * 1000LL;
}


//不实现
AOS_API_NO_INLNE
aos_status_t aos_now_time_str(char *buffer, size_t len)
{
    ASSERT(0);

    return 1;
}

#endif /* #if TCFG_PAY_ALIOS_ENABLE && (TCFG_PAY_ALIOS_WAY_SEL==TCFG_PAY_ALIOS_WAY_ALIYUN) */
