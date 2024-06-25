
#include "app_config.h"
#include "media/includes.h"
#include "audio_way_dac.h"
#include "audio_config.h"

#ifndef AUDIO_OUT_WAY_TYPE
#error "no defined AUDIO_OUT_WAY_TYPE"
#endif

#if (AUDIO_OUT_WAY_TYPE & AUDIO_WAY_TYPE_DAC)

#if (AUDIO_OUT_WAY_TYPE & AUDIO_WAY_TYPE_DONGLE)
s16 dac_buff[1 * 1024] SEC(.dac_buff);
#elif (AUDIO_OUT_WAY_TYPE & AUDIO_WAY_TYPE_IIS)
s16 dac_buff[1 * 1024] SEC(.dac_buff);
#elif (AUDIO_OUT_WAY_TYPE & AUDIO_WAY_TYPE_DAC)
s16 dac_buff[4 * 1024] SEC(.dac_buff);
#endif

void *audio_dac_get_buf(int *len)
{
    if (len) {
        *len = sizeof(dac_buff);
    }
    return dac_buff;
}

static int audio_dac_trim_value(struct audio_dac_trim *trim)
{
    int len = syscfg_read(CFG_DAC_TRIM_INFO, (void *)trim, sizeof(struct audio_dac_trim));
    if (len != sizeof(struct audio_dac_trim) || audio_dac_trim_value_check(trim)) {
        return -EINVAL;
    }
    return 0;
}

static void audio_dac_trim_end_handler(struct audio_dac_trim *trim)
{
    syscfg_write(CFG_DAC_TRIM_INFO, (void *)trim, sizeof(struct audio_dac_trim));
}

const struct audio_dac_platform_data sound_dac_data = {
    .output = TCFG_AUDIO_DAC_CONNECT_MODE,
    .mode = TCFG_AUDIO_DAC_MODE,
    .vcm_cap_en = 1,
    .power = {
        .ldo_isel = 3,
        .ldo_fb_isel = 3,
        .lpf_isel   = 0xf,//0x8,
        .vcmo_en = 0,//1,
        .vcmo_always_on = 0,
        .vcm_risetime = 0,
        .trim_poweron_time = 500,
        .trim_value = audio_dac_trim_value,
        .trim_begin = NULL,
        .trim_end   = audio_dac_trim_end_handler,
    },
    .codec = {
        .dsm_clk = DAC_DSM_6MHz,
    },
};

void audio_way_dac_init(void)
{
    struct sound_pcm_platform_data data;
    data.dma_addr = NULL;//(void *)dac_buff;
    data.dma_bytes = sizeof(dac_buff);
    data.fifo_bytes = sizeof(dac_buff);
    data.private_data = (void *)&sound_dac_data;
    sound_platform_load("dac", &data);
}

__attribute__((weak))
void audio_dac_wakeup_irq_handler(void *priv)
{
    /* putchar('R'); */
    audio_way_resume();
}

struct audio_way *audio_way_dac_open(void)
{
    struct audio_way *audio_hdl;
    struct sound_pcm_stream *dac = NULL;
    int err = sound_pcm_create(&dac, "dac", 0);
    if (err) {
        log_e("Create dac sound pcm error.\n");
        return NULL;
    }
    sound_pcm_ctl_ioctl(dac, SNDCTL_IOCTL_SET_BIAS_TRIM, 0);
    sound_pcm_set_irq_handler(dac, NULL, audio_dac_wakeup_irq_handler);
    audio_hdl = zalloc(sizeof(struct audio_way));
    ASSERT(audio_hdl);
    audio_hdl->way_type = AUDIO_WAY_TYPE_DAC;
    audio_hdl->stream = dac;
    audio_hdl->state = AUDIO_WAY_STATE_IDLE;
    switch (sound_dac_data.output) {
    case DAC_OUTPUT_MONO_L:
    case DAC_OUTPUT_MONO_R:
        audio_hdl->out_ch = 1;
        break;
    default :
        audio_hdl->out_ch = 2;
        break;
    }
    return audio_hdl;
}


#endif /*(AUDIO_OUT_WAY_TYPE & AUDIO_WAY_TYPE_DAC)*/

