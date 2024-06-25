/*****************************************************************
>file name : audio_bt_emitter_dma.c
>Description : Audio bt_emitter dma模块
    Audio bt_emitter dma 属于bt_emitter纯数字DMA，硬件上通过DMA访问上层传输数据，
DMA速度根据bt_emitter采样率决定，将采样点传输至codec进行数模转换处理。
*****************************************************************/
#define LOG_TAG     "[BT_EMITTER-DMA]"
#define LOG_ERROR_ENABLE
#define LOG_INFO_ENABLE
#define LOG_DEBUG_ENABLE
/* #define LOG_DUMP_ENABLE */
#include "debug.h"
#include "audio_bt_emitter.h"
#include "sound/pcm.h"
/* #include "audio_sbc_codec.h" */
/* #include "system/includes.h" */


#define substream_to_bt_emitter_context(substream) ((struct audio_bt_emitter_context *)substream->private_data)
static void audio_bt_emitter_dma_fifo_sync(struct sound_pcm_substream *substream, u8 flag);

int audio_bt_emitter_dma_register(struct audio_bt_emitter_context *bt_emitter, struct sound_pcm_platform_data *data)
{
    /*
     * PCM fifo设置;
     * 根据dma的模式配置fifo与dma复用，或配置pingpong buffer的dma地址与周期帧点数
     */
    INIT_LIST_HEAD(&bt_emitter->stream_list);

    bt_emitter->fifo.addr = data->dma_addr;
    bt_emitter->fifo.bytes = data->fifo_bytes;
    bt_emitter->fifo.frame_len = data->fifo_bytes / 2 / bt_emitter->hw_params.channels;
    bt_emitter->fifo.sync = audio_bt_emitter_dma_fifo_sync;

    return 0;
}

void audio_bt_emitter_dma_unregister(struct audio_bt_emitter_context *bt_emitter)
{
}

/*************************************************************************
 * BT_EMITTER DMA与复用fifo以及substream同步
 *
 * Input    :  substream  - sound_pcm_subtream结构指针，pcm子流
 *             flag - 同步读/写的标志.
 * Output   :  无.
 * Notes    :  该函数对于CFIFO方式的DMA与pcm子流的关联非常重要，
 *             同步了中间层所需要的DMA运行信息,中断模式则无需配置该函数.
 *=======================================================================*/
static void audio_bt_emitter_dma_fifo_sync(struct sound_pcm_substream *substream, u8 flag)
{
    struct audio_bt_emitter_context *bt_emitter = substream_to_bt_emitter_context(substream);
    struct sound_pcm_substream *substr1;

    if (flag & DMA_SYNC_R) {
        u32 read_ptr = audio_cfifo_get_hw_ptr(&bt_emitter->fifo.cfifo);
        bt_emitter->dma.hw_ptr = bt_emitter->dma.sw_ptr - audio_bt_emitter_hw_fifo_get_buff_len();
        u32 read_frames = bt_emitter->dma.hw_ptr - read_ptr;
        list_for_each_entry(substr1, &bt_emitter->stream_list, entry) {
            substr1->runtime->hw_ptr += read_frames;
        }
        audio_cfifo_read_update(&bt_emitter->fifo.cfifo, read_frames);
    }

    if (flag & DMA_SYNC_W) {
        u32 write_ptr = audio_cfifo_get_sw_ptr(&bt_emitter->fifo.cfifo);
        u32 write_frames = write_ptr - bt_emitter->dma.sw_ptr;
        audio_bt_emitter_hw_fifo_write_update(write_frames);
        list_for_each_entry(substr1, &bt_emitter->stream_list, entry) {
            substr1->runtime->sw_ptr += write_frames;
        }
        bt_emitter->dma.sw_ptr += write_frames;
    }
}

/*************************************************************************
 * 初始化runtime与硬件关联的参数
 *
 * Input    :  runtime  - pcm runtime结构指针
 *             bt_emitter - audio_bt_emitter_context结构指针.
 * Output   :  无.
 * Notes    :  无.
 *=======================================================================*/
static void audio_bt_emitter_set_runtime_params(struct sound_pcm_runtime *runtime, struct audio_bt_emitter_context *bt_emitter)
{
    runtime->fifo = &bt_emitter->fifo;
    runtime->status = &bt_emitter->status;
    runtime->channels = bt_emitter->hw_params.channels;
    runtime->sample_bits = bt_emitter->hw_params.sample_bits;
    runtime->hw_ptr = 0;
    runtime->sw_ptr = 0;
    runtime->hw_ptr_jiffies = 0;
}

/*************************************************************************
 * 获取BT_EMITTER的PCM关联硬件参数
 *
 * Input    :  sound_pcm_substream  - pcm子流结构指针
 *             params - pcm硬件参数指针.
 * Output   :  0 - 暂时默认返回0.
 * Notes    :  属于sound_pcm_ops的ioctl命令case.
 *=======================================================================*/
static int audio_bt_emitter_get_pcm_hw_params(struct sound_pcm_substream *substream, struct sound_pcm_hw_params *params)
{
    struct audio_bt_emitter_context *bt_emitter = substream_to_bt_emitter_context(substream);

    memcpy(params, &bt_emitter->hw_params, sizeof(bt_emitter->hw_params));

    if (bt_emitter->status.state == SOUND_PCM_STATE_IDLE || bt_emitter->status.state == SOUND_PCM_STATE_SUSPENDED) {
        params->sample_rate = 0;
    }
    return 0;
}

/*************************************************************************
 * 设置BT_EMITTER的PCM关联硬件参数
 *
 * Input    :  sound_pcm_substream  - pcm子流结构指针
 *             params - pcm硬件参数指针.
 * Output   :  0 - 暂时默认返回0.
 * Notes    :  属于sound_pcm_ops的ioctl命令case.
 *=======================================================================*/
static int audio_bt_emitter_set_pcm_hw_params(struct sound_pcm_substream *substream, struct sound_pcm_hw_params *params)
{
    struct audio_bt_emitter_context *bt_emitter = substream_to_bt_emitter_context(substream);

    bt_emitter->hw_params.sample_rate = params->sample_rate;

    return 0;
}

/*************************************************************************
 * BT_EMITTER DMA(PCM Stream)打开接口
 *
 * Input    :  sound_pcm_substream  - pcm子流结构指针
 *             params - pcm硬件参数指针.
 * Output   :  0 - 暂时默认返回0.
 * Notes    :  .
 *=======================================================================*/
static int audio_bt_emitter_dma_open(struct sound_pcm_substream *substream)
{
    struct audio_bt_emitter_context *bt_emitter = substream_to_bt_emitter_context(substream);

    log_info("bt_emitter context : 0x%x\n", (u32)bt_emitter);
    if (!substream->runtime) {
        return -EINVAL;
    }

    audio_bt_emitter_set_runtime_params(substream->runtime, bt_emitter);

    list_add(&substream->entry, &bt_emitter->stream_list);

    return 0;
}

/*************************************************************************
 * BT_EMITTER DMA(PCM Stream)准备接口
 *
 * Input    :  sound_pcm_substream  - pcm子流结构指针
 * Output   :  0 - 暂时默认返回0.
 * Notes    :  初始化硬件FIFO和状态以及硬件采样率.
 *=======================================================================*/
static int audio_bt_emitter_dma_prepare(struct sound_pcm_substream *substream)
{
    struct audio_bt_emitter_context *bt_emitter = substream_to_bt_emitter_context(substream);

    if ((bt_emitter->status.state != SOUND_PCM_STATE_PREPARED) && (bt_emitter->status.state != SOUND_PCM_STATE_RUNNING)) {
        bt_emitter->status.state = SOUND_PCM_STATE_PREPARED;
        bt_emitter->dma.sw_ptr = 0;
        bt_emitter->dma.hw_ptr = 0;
        audio_bt_emitter_hw_init(bt_emitter);
    }

    bt_emitter->status.ref_count++;

    return 0;
}

/*************************************************************************
 * BT_EMITTER DMA(runtime)硬件指针查询接口
 *
 * Input    :  sound_pcm_substream  - pcm子流结构指针
 * Output   :  runtime硬件实时指针.
 * Notes    :  该接口一边用于给音频同步查询来用.
 *=======================================================================*/
static int audio_bt_emitter_dma_pointer(struct sound_pcm_substream *substream)
{
    return 0;
}

/*************************************************************************
 * BT_EMITTER DMA中断请求接口
 *
 * Input    :  sound_pcm_substream  - pcm子流结构指针
 * Output   :  0 - 成功，非0 - 失败.
 * Notes    :  请求硬件访问到dma_irq_ptr位置时的中断
 * History  :  2021/03/11 by Lichao.
 *=======================================================================*/
static int audio_bt_emitter_dma_irq_request(struct sound_pcm_substream *substream)
{
    struct audio_bt_emitter_context *bt_emitter = substream_to_bt_emitter_context(substream);
    struct sound_pcm_runtime *runtime = substream->runtime;

    local_irq_disable();
    if (bt_emitter->status.state == SOUND_PCM_STATE_RUNNING) {
        audio_bt_emitter_hw_dma_enable();
    }
    local_irq_enable();
    return 0;
}

/*************************************************************************
 * BT_EMITTER DMA停止接口
 *
 * Input    :  sound_pcm_substream  - pcm子流结构指针
 * Output   :  0 - 成功，非0 - 失败.
 * Notes    :
 *=======================================================================*/
static int audio_bt_emitter_dma_stop(struct sound_pcm_substream *substream)
{
    struct audio_bt_emitter_context *bt_emitter = substream_to_bt_emitter_context(substream);

    if (!AUDIO_BT_EMITTER_STREAM_RUNNING(bt_emitter)) {
        /*log_warn("Audio BT_EMITTER maybe stop once more.\n");*/
        return 0;
    }

    bt_emitter->status.ref_count--;
    if (!AUDIO_BT_EMITTER_STREAM_RUNNING(bt_emitter)) {
        audio_bt_emitter_hw_dma_disable();
        bt_emitter->status.state = SOUND_PCM_STATE_SUSPENDED;
    }
    return 0;
}

/*************************************************************************
 * BT_EMITTER DMA开启接口
 *
 * Input    :  sound_pcm_substream  - pcm子流结构指针
 * Output   :  无.
 * Notes    :  需要先将硬件prepare完成后才可以执行打开DMA的操作
 *=======================================================================*/
static void audio_bt_emitter_dma_start(struct sound_pcm_substream *substream)
{
    struct audio_bt_emitter_context *bt_emitter = substream_to_bt_emitter_context(substream);
    struct sound_pcm_runtime *runtime = substream->runtime;

    if (bt_emitter->status.state == SOUND_PCM_STATE_PREPARED) {
        bt_emitter->status.state = SOUND_PCM_STATE_RUNNING;
        audio_bt_emitter_hw_dma_enable();
    }
}

/*************************************************************************
 * BT_EMITTER DMA trigger接口
 *
 * Input    :  sound_pcm_substream  - pcm子流结构指针
 *             cmd - trigger命令: START/STOP/IRQ
 * Output   :  0 - 成功，1 - 命令执行失败.
 * Notes    :  无.
 *=======================================================================*/
static int audio_bt_emitter_dma_trigger(struct sound_pcm_substream *substream, int cmd)
{
    int err = 0;

    switch (cmd) {
    case SOUND_PCM_TRIGGER_START:
        audio_bt_emitter_dma_start(substream);
        break;
    case SOUND_PCM_TRIGGER_STOP:
        err = audio_bt_emitter_dma_stop(substream);
        break;
    case SOUND_PCM_TRIGGER_IRQ:
        err = audio_bt_emitter_dma_irq_request(substream);
        break;
    default:
        break;
    }
    return 0;
}

/*************************************************************************
 * BT_EMITTER DMA ioctl接口
 *
 * Input    :  sound_pcm_substream  - pcm子流结构指针
 *             cmd - IOCTL命令
 *             args - 参数指针
 * Output   :  0 - 成功，1 - 命令执行失败.
 * Notes    :  无.
 *=======================================================================*/
static int audio_bt_emitter_pcm_ioctl(struct sound_pcm_substream *substream, u32 cmd, void *args)
{
    int err = 0;

    switch (cmd) {
    case SOUND_PCM_GET_HW_PARAMS:
        err = audio_bt_emitter_get_pcm_hw_params(substream, (struct sound_pcm_hw_params *)args);
        break;
    case SOUND_PCM_SET_HW_PARAMS:
        err = audio_bt_emitter_set_pcm_hw_params(substream, (struct sound_pcm_hw_params *)args);
        break;
    default:
        break;
    }
    return 0;
}

/*************************************************************************
 * BT_EMITTER DMA(PCM Stream)关闭接口
 *
 * Input    :  sound_pcm_substream  - pcm子流结构指针
 * Output   :  0 - 暂时默认返回0.
 * Notes    :  .
 *=======================================================================*/
static int audio_bt_emitter_dma_close(struct sound_pcm_substream *substream)
{
    struct audio_bt_emitter_context *bt_emitter = substream_to_bt_emitter_context(substream);

    if (substream->runtime) {
        /*sound_pcm_free_runtime(substream->runtime);*/
        /*substream->runtime = NULL;*/
    }
    list_del(&substream->entry);
    return 0;
}

const struct sound_pcm_ops bt_emitter_dma_ops = {
    .open       = audio_bt_emitter_dma_open,
    .close      = audio_bt_emitter_dma_close,
    .ioctl      = audio_bt_emitter_pcm_ioctl,
    .prepare    = audio_bt_emitter_dma_prepare,
    .trigger    = audio_bt_emitter_dma_trigger,
    .pointer    = audio_bt_emitter_dma_pointer,
};


