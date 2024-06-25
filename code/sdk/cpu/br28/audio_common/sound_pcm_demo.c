/*****************************************************************
>file name : sound_pcm_demo.c
>create time : Mon 29 Mar 2021 08:57:09 AM CST
>description : Sound pcm设备使用示例
>编译配置说明：
1、Makefile config需加入以下配置选项：
    export CONFIG_SOUND_PLATFORM ?= y
    CC_DEFINE += \
    -DCONFIG_SOUND_PLATFORM_ENABLE=1
   或将该配置放置单独的audio_cfg文件由外部include进行定制化编译
   例如：-include lib$(SLASH)media$(SLASH)audio_cfg

2、sound_pcm_demo.c需添加到Makfile文件中进行编译
   使用audio_sound_playback_init()、audio_sound_playback_test()
   进行sound_pcm接口测试

3、添加驱动程序需在sdk_used_list.c中加入驱动的链接符号，如DAC驱动：
   audio_dac_driver

*****************************************************************/
#include "app_config.h"
#include "media/includes.h"
#include "system/includes.h"
#include "sound/sound.h"
const unsigned char sin44K[88] ALIGNED(4) = {
    0x00, 0x00, 0x45, 0x0E, 0x41, 0x1C, 0xAA, 0x29, 0x3B, 0x36, 0xB2, 0x41, 0xD5, 0x4B, 0x6E, 0x54,
    0x51, 0x5B, 0x5A, 0x60, 0x70, 0x63, 0x82, 0x64, 0x8A, 0x63, 0x8E, 0x60, 0x9D, 0x5B, 0xD1, 0x54,
    0x4D, 0x4C, 0x3D, 0x42, 0xD5, 0x36, 0x50, 0x2A, 0xF1, 0x1C, 0xFB, 0x0E, 0xB7, 0x00, 0x70, 0xF2,
    0x6E, 0xE4, 0xFD, 0xD6, 0x60, 0xCA, 0xD9, 0xBE, 0xA5, 0xB4, 0xF7, 0xAB, 0xFC, 0xA4, 0xDA, 0x9F,
    0xAB, 0x9C, 0x7F, 0x9B, 0x5E, 0x9C, 0x3F, 0x9F, 0x19, 0xA4, 0xCE, 0xAA, 0x3D, 0xB3, 0x3A, 0xBD,
    0x92, 0xC8, 0x0A, 0xD5, 0x60, 0xE2, 0x50, 0xF0
};

int read_44k_sine_data(void *buf, int bytes, int offset, u8 channel)
{
    s16 *sine = (s16 *)sin44K;
    s16 *data = (s16 *)buf;
    int frame_len = (bytes >> 1) / channel;
    int sin44k_frame_len = sizeof(sin44K) / 2;
    int i, j;

    offset = offset % sin44k_frame_len;

    for (i = 0; i < frame_len; i++) {
        for (j = 0; j < channel; j++) {
            *data++ = sine[offset];
        }
        if (++offset >= sin44k_frame_len) {
            offset = 0;
        }
    }

    return i * 2 * channel;
}

static int audio_dac_trim_value(struct audio_dac_trim *trim)
{
    int len = syscfg_read(CFG_DAC_TRIM_INFO, (void *)trim, sizeof(struct audio_dac_trim));
    if (len != sizeof(struct audio_dac_trim) || trim->left == 0 || trim->right == 0) {
        return -EINVAL;
    }
    return 0;
}

static void audio_dac_trim_end_handler(struct audio_dac_trim *trim)
{
    syscfg_write(CFG_DAC_TRIM_INFO, (void *)trim, sizeof(struct audio_dac_trim));
}

const struct audio_dac_platform_data sound_dac_data1 = {
    .output = TCFG_AUDIO_DAC_CONNECT_MODE,
    .power = {
        .ldo_isel = 3,
        .ldo_fb_isel = 3,
        .lpf_isel   = 0x8,
        .vcmo_en = 1,
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

extern s16 dac_buff[4 * 1024];
int audio_sound_playback_init(void)
{

    sound_platform_init();

    /***********DAC platform load***********/
    struct sound_pcm_platform_data data;
    data.dma_addr = (void *)dac_buff;
    data.dma_bytes = sizeof(dac_buff);
    data.fifo_bytes = sizeof(dac_buff);
    data.private_data = (void *)&sound_dac_data1;
    sound_platform_load("dac", &data);

#if 0
    /************i2s platform load demo**************/
    data.dma_addr = i2s_dma_buffer;
    data.dma_bytes = sizeof(i2s_dma_buffer);
    data.private_data = i2s_platform_data,
         sound_platform_load("iis", &data);

    /************PCM device platform load*************/
    sound_platform_load("pcm", &data);
#endif

    return 0;
}

static void sound_dac_wakeup_irq_handler(void *priv)
{
    /*putchar('R');*/
    os_taskq_post_msg("app_core", 1, 0x9);
}

static void sound_dac_power_setup(struct sound_pcm_stream *dac)
{
    sound_pcm_ctl_ioctl(dac, SNDCTL_IOCTL_POWER_ON, NULL);
    struct sound_volume volume = {
        .chmap = SOUND_CHMAP_FL | SOUND_CHMAP_FR,
        .volume = {10, 10},
    };
    sound_pcm_ctl_ioctl(dac, SNDCTL_IOCTL_SET_ANA_GAIN, &volume);
    volume.volume[0] = 16384;
    volume.volume[1] = 16384;
    sound_pcm_ctl_ioctl(dac, SNDCTL_IOCTL_SET_DIG_GAIN, &volume);
    printf("JL_AUDIO->DAC_VL0 : 0x%x, JL_ADDA->DAA_CON1 : 0x%x\n", JL_AUDIO->DAC_VL0, JL_ADDA->DAA_CON1);
}

struct sound_pcm_stream *dac = NULL;
int audio_sound_playback_test(void)
{
    struct sound_pcm_hw_params *hw_params;
    int err = sound_pcm_create(&dac, "dac", 0);
    if (err) {
        printf("Create sound pcm error.\n");
        return -ENODEV;
    }
    sound_pcm_ctl_ioctl(dac, SNDCTL_IOCTL_SET_BIAS_TRIM, 0);

    sound_pcm_set_irq_handler(dac, NULL, sound_dac_wakeup_irq_handler);

    sound_pcm_prepare(dac, 44100, 50, WRITE_MODE_BLOCK);

    sound_dac_power_setup(dac);
    /*sound_pcm_write_slience(dac, sound_pcm_current_ptr(dac), 1024);*/
    sound_pcm_start(dac);

#define TEST_PCM_SAMPLES (128 * 2)

    s16 pcm_frames[TEST_PCM_SAMPLES];
    u16 frames_offset = 0;
    s16 frame_bytes = 0;
    s16 write_len = 0;
    s16 write_offset = 0;
    int res = 0;
    int msg[16];
    u8  read = 1;
    u8  pend = 0;

    while (1) {
        if (pend) {
            res = os_taskq_pend(NULL, msg, ARRAY_SIZE(msg));
        } else {
            res = os_taskq_accept(ARRAY_SIZE(msg), msg);
        }

        if (res == OS_TASKQ) {
            switch (msg[1]) {
            case 0x9:
                break;
            default:
                continue;
            }
        }

        if (read) {
            frame_bytes = read_44k_sine_data(pcm_frames, TEST_PCM_SAMPLES * 2, frames_offset, 2);
            frames_offset += (frame_bytes >> 1) / 2;
        }

        s16 write_bytes = frame_bytes - write_offset * 2;
        write_len = sound_pcm_write(dac, pcm_frames + write_offset, write_bytes);

        if (write_len < write_bytes) {
            /*overrun*/
            sound_pcm_trigger_interrupt(dac, SOUND_TIME_US, 2500);
            pend = 1;
        } else {
            pend = 0;
        }

        write_offset += (write_len >> 1);
        /*printf("%d - %d - %d\n", write_len, write_bytes, write_offset);*/
        if (write_offset >= TEST_PCM_SAMPLES) {
            write_offset = 0;
            read = 1;
        } else {
            read = 0;
        }
    }
}
