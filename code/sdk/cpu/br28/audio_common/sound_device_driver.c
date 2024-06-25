#include "asm/includes.h"
#include "media/includes.h"
#include "system/includes.h"
#include "audio_config.h"
#include "asm/audio_adc.h"
#include "sound_device_driver.h"
#include "audio_enc/audio_enc.h"


//-----------------------------------------------------------------------------
// linein
//-----------------------------------------------------------------------------

struct broadcast_linein_hdl {
    struct audio_adc_output_hdl adc_output; //adc 输出
    struct adc_linein_ch linein_ch;      	//linein 通道
    u16 sample_rate;   					    //采样率
    u8 gain;								//增益
    u8 linein_ch_num;						//通道数
    u8 start;								//linein 状态
    void *output_priv;                      //数据接口私有参数
    int (*output_handler)(void *output_priv, s16 *data, int len);//输出接口

};

static struct broadcast_linein_hdl *linein_hdl = NULL;

static void linein_output_data(void *priv, s16 *data, int len)
{
    if (!linein_hdl || !linein_hdl->start) {
        return ;
    }
    int wlen = 0;
    if (linein_hdl->linein_ch_num >= 2) {
        /*输出两个linein的数据,默认输出第一个和第二个采样通道的数据*/
        if (linein_hdl->output_handler) {
            wlen = linein_hdl->output_handler(linein_hdl->output_priv, data, len * 2);
        }
        if (wlen < len * 2) {
            putchar('L');
        }
    } else {
        if (linein_hdl->output_handler) {
            wlen = linein_hdl->output_handler(linein_hdl->output_priv, data, len);
        }
        if (wlen < len) {
            putchar('L');
        }
    }
}

int broadcast_linein_open(struct broadcast_linein_params *params)
{
    struct broadcast_linein_params *broadcast_linein = (struct broadcast_linein_params *)params;
    linein_hdl = zalloc(sizeof(struct broadcast_linein_hdl));
    ASSERT(linein_hdl);
    linein_hdl->sample_rate = broadcast_linein->sample_rate;
    linein_hdl->gain = broadcast_linein->gain;
    linein_hdl->linein_ch_num = broadcast_linein->channel_num;
    linein_hdl->output_priv = broadcast_linein->output_priv;
    linein_hdl->output_handler = broadcast_linein->output_handler;

    if (audio_linein_open(&linein_hdl->linein_ch, linein_hdl->sample_rate, linein_hdl->gain) == 0) {
        linein_hdl->adc_output.handler = linein_output_data;
        linein_hdl->adc_output.priv = linein_hdl;
        if (linein_hdl->linein_ch_num != get_audio_linein_ch_num()) {
            printf("linein channel num err !!!\n");
            return -EINVAL;
        }
        printf("-------------linein->channel_num:%d ----------\n", linein_hdl->linein_ch_num);
        audio_linein_add_output(&linein_hdl->adc_output);
        audio_linein_start(&linein_hdl->linein_ch);
        linein_hdl->start = 1;
    }

    printf("audio_adc_linein_broadcast start succ\n");
    return 0;

}

void broadcast_linein_close(void)
{
    printf("audio_adc_linein_broadcast_close\n");

    if (!linein_hdl) {
        return ;
    }
    linein_hdl->start = 0;
    audio_linein_close(&linein_hdl->linein_ch, &linein_hdl->adc_output);
    local_irq_disable();
    free(linein_hdl);
    linein_hdl = NULL;
    local_irq_enable();
}




