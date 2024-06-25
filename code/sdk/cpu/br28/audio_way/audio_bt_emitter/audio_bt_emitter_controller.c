/*****************************************************************
>file name : audio_bt_emitter_controller.c
>decription: Audio bt_emitter控制器
    audio bt_emitter控制器属于bt_emitter的控制模块，包括bt_emitter（audio）电源控制，
数字codec模块控制。
*****************************************************************/
#define LOG_TAG     "[BT_EMITTER-CTL]"
#define LOG_ERROR_ENABLE
#define LOG_INFO_ENABLE
#define LOG_DEBUG_ENABLE
/* #define LOG_DUMP_ENABLE */
#include "debug.h"
#include "audio_bt_emitter.h"

int audio_bt_emitter_controller_register(struct audio_bt_emitter_context *bt_emitter, struct sound_pcm_platform_data *data)
{
    return 0;
}

void audio_bt_emitter_controller_unregister(struct audio_bt_emitter_context *bt_emitter)
{
}

static int audio_bt_emitter_power_on(void *private_data)
{
    return 0;
}
static int audio_bt_emitter_power_off(void *private_data)
{
    return 0;
}

static int audio_bt_emitter_get_digital_gain(struct audio_bt_emitter_context *bt_emitter, struct sound_volume *sv)
{
    int i;
    for (i = 0; i < BT_EMITTER_CH_MAX; i++) {
        if (sv->chmap & BIT(i)) {
            sv->volume[i] = audio_bt_emitter_hw_get_digital_gain(i);
        }
    }
    return 0;
}

static int audio_bt_emitter_set_digital_gain(struct audio_bt_emitter_context *bt_emitter, struct sound_volume *sv)
{
    int i;
    for (i = 0; i < BT_EMITTER_CH_MAX; i++) {
        if (sv->chmap & BIT(i)) {
            audio_bt_emitter_hw_set_digital_gain(i, sv->volume[i]);
        }
    }
    return 0;
}

static int audio_bt_emitter_controller_ioctl(void *private_data, int cmd, void *args)
{
    struct audio_bt_emitter_context *bt_emitter = (struct audio_bt_emitter_context *)private_data;

    switch (cmd) {
    case SNDCTL_IOCTL_GET_DIG_GAIN:
        audio_bt_emitter_get_digital_gain(bt_emitter, (struct sound_volume *)args);
        break;
    case SNDCTL_IOCTL_SET_DIG_GAIN:
        audio_bt_emitter_set_digital_gain(bt_emitter, (struct sound_volume *)args);
        break;
    }
    return 0;
}

const struct sound_drv_controller audio_bt_emitter_controller = {
    .power_on   = audio_bt_emitter_power_on,
    .power_off  = audio_bt_emitter_power_off,
    .ioctl      = audio_bt_emitter_controller_ioctl,
};
