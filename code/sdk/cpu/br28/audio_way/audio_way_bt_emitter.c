
#include "app_config.h"
#include "media/includes.h"
#include "audio_way_bt_emitter.h"
#include "audio_sbc_codec.h"


#ifndef AUDIO_OUT_WAY_TYPE
#error "no defined AUDIO_OUT_WAY_TYPE"
#endif

#if (AUDIO_OUT_WAY_TYPE & AUDIO_WAY_TYPE_BT)

s16 bt_emitter_buff[1 * 1024];


void audio_way_bt_emitter_init(void)
{
    struct sound_pcm_platform_data data;
    data.dma_addr = NULL;//(void *)bt_emitter_buff;
    data.dma_bytes = sizeof(bt_emitter_buff);
    data.fifo_bytes = sizeof(bt_emitter_buff);
    sound_platform_load("bt_emitter", &data);
}

__attribute__((weak))
void audio_bt_emitter_wakeup_irq_handler(void *priv)
{
    /* putchar('R'); */
    audio_way_resume();
}

struct audio_way *audio_way_bt_emitter_open(void)
{
    struct audio_way *audio_hdl;
    struct sound_pcm_stream *bt_emitter = NULL;
    int err = sound_pcm_create(&bt_emitter, "bt_emitter", 0);
    if (err) {
        log_e("Create bt_emitter sound pcm error.\n");
        return NULL;
    }
    sound_pcm_set_irq_handler(bt_emitter, NULL, audio_bt_emitter_wakeup_irq_handler);
    audio_hdl = zalloc(sizeof(struct audio_way));
    ASSERT(audio_hdl);
    audio_hdl->way_type = AUDIO_WAY_TYPE_BT;
    audio_hdl->stream = bt_emitter;
    audio_hdl->state = AUDIO_WAY_STATE_IDLE;
    audio_hdl->out_ch = audio_sbc_enc_get_channel_num();
    return audio_hdl;
}


#endif /*(AUDIO_OUT_WAY_TYPE & AUDIO_WAY_TYPE_BT)*/

