/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include "aos_system.h"
#include "aos/kernel.h"
/* #include "os/os_api.h" */
#include "app_config.h"

#if TCFG_PAY_ALIOS_ENABLE && (TCFG_PAY_ALIOS_WAY_SEL==TCFG_PAY_ALIOS_WAY_ALIYUN)

#define LOG_TAG "AOS"
#include "debug.h"

#define _SYSINFO_DEVICE_NAME "aos-xxx"
#define SYSINFO_VERSION      "0.0.1"

#ifndef LOG_I
#define LOG_I   log_i
#endif

AOS_API_NO_INLNE
void aos_putchar(char c)
{
    //TODO:
    // aos_printf("%c",c);
    putchar(c);
}

AOS_API_NO_INLNE
void aos_reboot(void)
{
    LOG_I("aos_reboot");
    AOS_API_CALL_DEBUG("0x%x", 1);
#ifndef WIN32
    //TODO:
    cpu_reset();
#endif // !WIN32

}

AOS_API_NO_INLNE
int32_t aos_get_hz(void)
{
    //TODO:
    // return RT_TICK_PER_SECOND;
#ifdef configTICK_RATE_HZ
    return configTICK_RATE_HZ;
#elif defined OS_TICKS_PER_SEC
    return OS_TICKS_PER_SEC;
#else
#error "error os ticks"
#endif
}



AOS_API_NO_INLNE
uint32_t aos_ms_to_tick(uint32_t ms)
{
    ms = ms < 10 ? 10 : ms;
    return (ms * (u32)aos_get_hz()) / 1000;
}

AOS_API_NO_INLNE
const char *aos_version_get(void)
{
    AOS_API_CALL_DEBUG("0x%x", 1);
    return SYSINFO_VERSION;
}



#if 0 //Remove useless APIs
int aos_task_key_create(aos_task_key_t *key)
{
    return 0;
}

void aos_task_key_delete(aos_task_key_t key)
{
}

int aos_task_setspecific(aos_task_key_t key, void *vp)
{
    return 0;
}

void *aos_task_getspecific(aos_task_key_t key)
{
    return 0;
}
#endif




// static void timer_common_cb(void * arg)
// {
// 	adpt_timer_t p_atimer = (adpt_timer_t) arg;

// 	p_atimer->fn((void *) p_atimer, p_atimer->arg);
// }


AOS_API_NO_INLNE
void aos_msleep(uint32_t ms)
{
    //TODO:
    // rt_err_t err;
    // err = rt_thread_mdelay((rt_int32_t) ms);
    // RT_ASSERT(RT_EOK == err);
    AOS_API_CALL_DEBUG("ms: %d", ms);
    os_time_dly(aos_ms_to_tick(ms));
}

AOS_API_NO_INLNE
void aos_init(void)
{
}

AOS_API_NO_INLNE
void aos_start(void)
{
}

AOS_API_NO_INLNE
void dumpsys_task_func(void)
{
    ASSERT(0);
}

extern int ___printf(const char *format, va_list args);

AOS_API_NO_INLNE
void __attribute__((weak)) aos_printf(const char *fmt, ...)
{
    int ret = 0;
    va_list args;
    va_start(args, fmt);

    ___printf(fmt, args);
}

/* void ALIPAY_log_ext(const char *fmt, ...) */
/* { */
/*     va_list args; */
/*     va_start(args, fmt); */
/*     ___printf(fmt, args); */
/* } */


AOS_API_NO_INLNE
int aos_snprintf(char *str, const int len, const char *fmt, ...)
{
    //TODO:
    // va_list args;
    // va_start(args, fmt);

    // rt_snprintf(str, len, fmt, args);

    // va_end(args);
    va_list param;

    va_start(param, fmt);

    return vsnprintf(str, len, fmt, param);
}

AOS_API_NO_INLNE
int aos_vsnprintf(char *str, const int len, const char *format, va_list ap)
{
    //TODO:
    if (len == 0) {
        return 0;
    }

    return print(&str, str + len - 1, format, ap);
}

AOS_API_NO_INLNE
const char *aos_get_system_version(void)
{
    //TODO:
    AOS_API_CALL_DEBUG("0x%x", 1);
    return SYSINFO_VERSION;
}

AOS_API_NO_INLNE
const char *aos_get_platform_type(void)
{
    //TODO:
    AOS_API_CALL_DEBUG("0x%x", 1);
    return _SYSINFO_DEVICE_NAME;
}

AOS_API_NO_INLNE
int aos_get_ip(char *ip)
{
    LOG_I(__FUNCTION__);

    ASSERT(0);

    return 0;
}

AOS_API_NO_INLNE
int aos_get_mac_addr(unsigned char mac[8])
{
    // TODO: ?
    LOG_I(__FUNCTION__);
    ASSERT(0);

    return 0;
}


AOS_API_NO_INLNE
int aos_network_status_registercb(void (*cb)(int status, void *), void *arg)
{
    // TODO: ?
    LOG_I(__FUNCTION__);
    ASSERT(0);

    return 0;
}

AOS_API_NO_INLNE
int aos_get_network_status(void)
{
    // TODO: ?
    LOG_I(__FUNCTION__);

    ASSERT(0);

    return 1;
}

AOS_API_NO_INLNE
const char *aos_get_device_name(void)
{
    //TODO:
    return "AC701N-Watch";
}

#if 0
int amp_heap_memory_info(amp_heap_info_t *heap_info)
{
    LOG_I(__FUNCTION__);
    return 0;
}
#endif

AOS_API_NO_INLNE
int aos_system_init(void)
{
    LOG_I(__FUNCTION__);

    return 0;
}

AOS_API_NO_INLNE
const char *aos_get_module_hardware_version(void)
{
    return "xxxxx";
}

AOS_API_NO_INLNE
const char *aos_get_module_software_version(void)
{
    return "xxxx";
}

AOS_API_NO_INLNE
int aos_system_init_early(void)
{
    LOG_I(__FUNCTION__);

    return 0;
}

AOS_API_NO_INLNE
int32_t aos_rand(void)
{
    return rand32();
}

AOS_API_NO_INLNE
void aos_srand(uint32_t seed)
{
    /* srand(seed); */
}

#if 0
/*
	QuickJS for alipay
*/
typedef rt_uint32_t JSCFunctionType;    //Just for pass compile error

typedef struct JSCFunctionListEntry {
    const char *name;
    uint8_t prop_flags;
    uint8_t def_type;
    int16_t magic;
    union {
        struct {
            uint8_t length; /* XXX: should move outside union */
            uint8_t cproto; /* XXX: should move outside union */
            JSCFunctionType cfunc;
        } func;
        struct {
            JSCFunctionType get;
            JSCFunctionType set;
        } getset;
        struct {
            const char *name;
            int base;
        } alias;
        struct {
            const struct JSCFunctionListEntry *tab;
            int len;
        } prop_list;
        const char *str;
        int32_t i32;
        int64_t i64;
        double f64;
    } u;
} JSCFunctionListEntry;

/*Empty vaiables just for fix link error*/
/*
const JSCFunctionListEntry js_std_console_funcs[1];
const JSCFunctionListEntry js_std_dpe_method_router_funcs[1];
const JSCFunctionListEntry js_std_re_callback_funcs[2];
*/

int amp_test()
{
    extern void amp_main();

    aos_printf("Test amp\n");

    amp_main();
    return 0;
}
MSH_CMD_EXPORT(amp_test, Test AMP);

int amp_mount()
{
    /*  try to mount fs first to avoid mkfs each time */
    if (dfs_mount("flash2", "/", "elm", 0, 0) == 0) { // fs exist
        aos_printf("mount fs on flash to root success\n");
    } else {
        // auto mkfs, remove it if you want to mkfs manual
        aos_printf("mount fs on flash to root fail, try to make fs\n");
        if (dfs_mkfs("elm", "flash2") == 0) {
            aos_printf("make elm fs on flash sucess, mount again\n");
            if (dfs_mount("flash2", "/", "elm", 0, 0) == 0) {
                aos_printf("mount fs on flash success\n");
            } else {
                aos_printf("mount to fs on flash fail\n");
            }
        } else {
            aos_printf("dfs_mkfs elm flash fail\n");
        }
    }
    return 0;
}
MSH_CMD_EXPORT(amp_mount, Mount file system);

#endif

#if 0 //Fix make
void impl_sha1_init(void)
{
    return;
}

void impl_sha1_starts(void)
{
    return;
}

void impl_sha1_update(void)
{
    return;
}

void impl_sha1_finish(void)
{
    return;
}

#endif /* #if 0 //Fix make */

/* AOS_API_NO_INLNE */
/* bool bizbus_hal_tt_exist(void) */
/* { */
/* return 0; */
/* } */

#endif /* #if TCFG_PAY_ALIOS_ENABLE && (TCFG_PAY_ALIOS_WAY_SEL==TCFG_PAY_ALIOS_WAY_ALIYUN) */
