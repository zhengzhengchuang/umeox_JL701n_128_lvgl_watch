#include "audio_iis.h"
#include "audio_link.h"
#include "debug.h"
#include "sound/sound.h"
#include "circular_buf.h"
#include "gpio.h"


#define IIS_OUTPUT_CBUF_LEN  (8 * 1024)

struct new_iis_hdl {
    ALINK_PARM alink_parm;
    cbuffer_t iis_cbuffer;
    u8 *iis_buf;
    u8 init;
    u8 start;
    u8 module_idx;
    u8 alink_tx_ch;	//iis 有四个通道，ch_idx的值可以为0,1,2,3, 设置为0代表iis有1个通道
    u8 alink_rx_ch;
};

struct new_iis_hdl *iis = NULL;

void alink_iis_init(int dma_size, int sr)
{
    if (iis) {
        return;
    }

    struct new_iis_hdl *iis_hdl = zalloc(sizeof(struct new_iis_hdl));
    if (iis_hdl) {
        iis = iis_hdl;
        iis->module_idx = 0;


        iis->iis_buf = malloc(IIS_OUTPUT_CBUF_LEN);
        cbuf_init(&iis->iis_cbuffer, iis->iis_buf, IIS_OUTPUT_CBUF_LEN);

        iis->alink_tx_ch = 0;
        iis->alink_rx_ch = 1;

        iis->alink_parm.mode = ALINK_MD_IIS;
        iis->alink_parm.clk_mode = ALINK_CLK_FALL_UPDATE_RAISE_SAMPLE;
        iis->alink_parm.bitwide = ALINK_LEN_16BIT;
        iis->alink_parm.sclk_per_frame = ALINK_FRAME_32SCLK;
        iis->alink_parm.buf_mode = ALINK_BUF_CIRCLE;
        iis->alink_parm.dma_len = dma_size;
        iis->alink_parm.sample_rate = sr;
        iis->alink_parm.module = iis->module_idx;
        iis->alink_parm.mclk_io = IO_PORTA_15;
        iis->alink_parm.sclk_io = IO_PORTA_14;
        iis->alink_parm.lrclk_io = IO_PORTA_13;
        iis->alink_parm.ch_cfg[iis->alink_tx_ch].data_io = IO_PORTA_12;
        iis->alink_parm.ch_cfg[iis->alink_rx_ch].data_io = IO_PORTA_05;
        iis->alink_parm.role = ALINK_ROLE_SLAVE;		// 从机
        /* iis->alink_parm.role = ALINK_ROLE_MASTER;		// 主机 */
    }
    iis->init = 1;
    iis->start = 0;
}

/*
 * note: 进来的数据是双声道的数据
 *       读寄存器的值的单位为每个声道各自有多少个点
 */
static int __audio_iis_tx_write(s16 *data, u32 len, u8 module_idx, ALINK_CH alink_ch)
{
    u32 swp = 0;
    s16 *wptr = NULL;
    u32 free_points = 0;
    u32 each_ch_points = 0;
    u32 alink_len = 0;
    int wlen = 0;
    u8 ch = 0;

    struct alnk_hw_ch *hw_channel_parm = (struct alnk_hw_ch *)(&(iis->alink_parm.ch_cfg[alink_ch]));

    swp = alink_get_swptr(hw_channel_parm);
    each_ch_points = len / 2 / 2;	//保存每个声道有多少个点数
    free_points = alink_get_shn(hw_channel_parm);		//读出 循环buff 还有N点数据可以写入
    alink_len = alink_get_len(hw_channel_parm);
    if (swp + free_points > alink_len) {
        free_points = alink_len - swp;
    }
    if (each_ch_points > free_points) {
        each_ch_points = free_points;
    }
    wptr = (s16 *)(alink_get_addr(hw_channel_parm) + swp * 2 * 2);	//IIS 数据为双声道数据
    wlen = each_ch_points * 2 * 2;
    memcpy(wptr, data, wlen);
    alink_set_shn(hw_channel_parm, each_ch_points);

    return wlen;
}

void alink_iis_start(void *rx_priv, void *tx_priv, void (*rx_handle)(void *priv, void *addr, int len), void (*tx_handle)(void *priv, void *addr, int len))
{
    if (iis) {
        if (iis->start == 0 && iis->init == 1) {
            void *hw_alink1 = alink_init(&iis->alink_parm);
            if (rx_handle != NULL) {
                alink_channel_init(hw_alink1, iis->alink_rx_ch, ALINK_DIR_RX, rx_priv, rx_handle);
            }
            if (tx_handle != NULL) {
                alink_channel_init(hw_alink1, iis->alink_tx_ch, ALINK_DIR_TX, tx_priv, tx_handle);
            }
            alink_start(hw_alink1);
            iis->start = 1;
        }
    }
}

void alink_rx_set_shn(int len)
{
    alink_set_shn(&iis->alink_parm.ch_cfg[iis->alink_rx_ch], len / 4);
}

int audio_iis_output_write(s16 *data, u32 len)
{
    int wlen = 0;
    u8 *u8_data = (u8 *)data;
    if (iis) {
        wlen = cbuf_write(&(iis->iis_cbuffer), u8_data, len);
    }
    return wlen;
}


//////////// ******************* iis rx/tx test demo ******************** ///////////////////
static u8 cache_flag = 0;
static void iis_rx_handle_demo(void *priv, void *buf, int len)
{
    s16 *data = (s16 *)buf;
    int wlen = 0;
    wlen = audio_iis_output_write(data, len);

    // 告诉寄存器收到了len个数据
    alink_rx_set_shn(len);

    // 缓存
    if (cache_flag == 0 && cbuf_get_data_len(&iis->iis_cbuffer) >= (IIS_OUTPUT_CBUF_LEN * 3 / 4)) {
        cache_flag = 1;
    }
}


static void handle_tx_demo(void *priv, void *buf, int len)
{
    if (iis) {

        int rlen = 0;
        if (cache_flag == 1) {
            rlen = cbuf_read(&iis->iis_cbuffer, buf, len);
        }
        alink_set_shn(&iis->alink_parm.ch_cfg[iis->alink_tx_ch], rlen / 4);
    }
}

/* test demo */
void iis_test_demo(void)
{
#if 1
    alink_iis_init(1024, 8000);
    alink_iis_start(NULL, NULL, iis_rx_handle_demo, handle_tx_demo);

    extern void wdt_clear(void);
    extern void wdt_close(void);
    wdt_close();
    while (1) {
        os_time_dly(100);
        putchar('S');
    }
#endif
}








