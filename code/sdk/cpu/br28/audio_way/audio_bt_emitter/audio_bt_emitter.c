/*****************************************************************
>file name : audio_bt_emitter.c
*****************************************************************/
#define LOG_TAG     "[BT_EMITTER-audio]"
#define LOG_ERROR_ENABLE
#define LOG_INFO_ENABLE
#define LOG_DEBUG_ENABLE
/* #define LOG_DUMP_ENABLE */
#include "debug.h"
#include "audio_bt_emitter.h"
#include "sound/pcm.h"
#include "audio_sbc_codec.h"

extern const struct sound_drv_controller audio_bt_emitter_controller;
extern const struct sound_pcm_ops bt_emitter_dma_ops;

extern u32 audio_output_channel_num(void);

/*************************************************************************
 * BT_EMITTER 硬件(支持配置)参数初始化
 *
 * Input    :  ctx  - audio_bt_emitter_context指针
 * Output   :  0 - 成功，非0 - 失败.
 * Notes    :
 *=======================================================================*/
static int audio_bt_emitter_init_hw_params(struct audio_bt_emitter_context *ctx)
{
    /* u8 channel = audio_sbc_enc_get_channel_num(); */
    u8 channel = audio_output_channel_num();
    int sr = audio_sbc_enc_get_rate();
    if (channel == 0) {
        channel = audio_sbc_enc_get_channel_num();
    }

    log_info("bt emitter ch:%d, sr:%d \n", channel, sr);

    ctx->hw_params.channels = channel;
    ctx->hw_params.rates = sound_pcm_match_standard_rate(sr);
    ctx->hw_params.sample_rate = 0;
    ctx->hw_params.sample_bits = 16;

    return 0;
}

/*************************************************************************
 * BT_EMITTER创建设备context以及初始化
 *
 * Input    :  device  - device的二级指针（用于赋值）
 *             data - 设备平台配置数据.
 * Output   :  *device - audio_bt_emitter_context结构指针
 *             -ENOMEM - 内存申请失败，0 - 初始化成功.
 * Notes    :
 *=======================================================================*/
static int audio_bt_emitter_new_context(void **device, struct sound_pcm_platform_data *data)
{
    struct audio_bt_emitter_context *ctx = NULL;

    ctx = (struct audio_bt_emitter_context *)zalloc(sizeof(struct audio_bt_emitter_context));
    if (!ctx) {
        return -ENOMEM;
    }

    *device = ctx;

    log_info("malloc bt_emitter context : 0x%x\n", (u32)ctx);

    audio_bt_emitter_init_hw_params(ctx);

    audio_bt_emitter_dma_register(ctx, data);

    audio_bt_emitter_controller_register(ctx, data);

    return 0;
}

/*************************************************************************
 * BT_EMITTER设备释放及注销
 *
 * Input    :  device  - audio_bt_emitter_context指针
 * Output   :  无.
 * Notes    :
 *=======================================================================*/
static void audio_bt_emitter_free_context(void *device)
{
    struct audio_bt_emitter_context *ctx = (struct audio_bt_emitter_context *)device;

    audio_bt_emitter_dma_unregister(ctx);

    audio_bt_emitter_controller_unregister(ctx);

    free(ctx);
}

SOUND_PLATFORM_DRIVER(audio_bt_emitter_driver) = {
    .name = "bt_emitter",
    .ops = &bt_emitter_dma_ops,
    .controller = &audio_bt_emitter_controller,
    .create = audio_bt_emitter_new_context,
    .free = audio_bt_emitter_free_context,
};

