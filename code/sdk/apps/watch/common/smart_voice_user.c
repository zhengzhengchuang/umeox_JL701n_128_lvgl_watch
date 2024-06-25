#include "includes.h"
#include "app_config.h"
#include "asm/power_interface.h"
#include "jl_kws.h"
#include "smart_voice/smart_voice.h"

#if TCFG_VAD_LP_CLOSE

static char kws_model = -1;

extern u32 check_ram1_size(void);
extern void *malloc_vlt(size_t size);
extern void free_vlt(void *pv);

void *wake_word_zalloc(int size)
{
    if (check_ram1_size() == 0) {
        return zalloc(size);
    } else {
        void *p = malloc_vlt(size);
        if (p) {
            memset(p, 0, size);
        }
        return p;
    }
}

void wake_word_free(void *p)
{
    if (check_ram1_size() == 0) {
        free(p);
    } else {
        free_vlt(p);
    }
}

void *multi_keyword_zalloc(int size)
{
    if (check_ram1_size() == 0) {
        return zalloc(size);
    } else {
        void *p = malloc_vlt(size);
        if (p) {
            memset(p, 0, size);
        }
        return p;
    }
}

void multi_keyword_free(void *p)
{
    if (check_ram1_size() == 0) {
        free(p);
    } else {
        free_vlt(p);
    }
}

static void kws_poweroff(int priv)
{
    int model = audio_smart_voice_get_model();
    if (model >= 0) {
        audio_smart_voice_detect_close();
        kws_model = model;
    }
}

AT(.volatile_ram_code)
static u8 extern_kws_handler(u32 timeout)
{
    int msg[3];
    int model = audio_smart_voice_get_model();
    if (model < 0) {
        return 0;
    }
    msg[0] = (int)kws_poweroff;
    msg[1] = 1;
    msg[2] = (int)NULL;
    os_taskq_post_type("app_core", Q_CALLBACK, 3, msg);
    return 1;
}

static u8 exit_kws_handler(u32 timeout)
{
    return 0;
}

//低功耗线程请求所有模块关闭，由对应线程处理
REGISTER_LP_REQUEST(kws_target) = {
    .name = "extern_kws",
    .request_enter = extern_kws_handler,
    .request_exit = exit_kws_handler,
};


int smart_voice_reset(void)
{
    int model = audio_smart_voice_get_model();
    if (model >= 0) {
        printf("smart_voice is open \n");
    } else {
        if (kws_model >= 0) {
            audio_smart_voice_detect_open(kws_model);
            kws_model = -1;
            return true;
        }
    }
    return false;
}

#if TCFG_UI_ENABLE
#include "ui/ui_api.h"

static void val_lcd_sleep_exit(void)
{
    smart_voice_reset();
}

REGISTER_LCD_SLEEP_HEADLER(vad_lcd_sleep) = {
    .name = "vad",
    .exit = val_lcd_sleep_exit,
};

#endif

#endif /* #if TCFG_VAD_LP_CLOSE */

