// #include "IIC_Master_Driver.h"
#include "asm/iic_hw.h"
#include "asm/iic_soft.h"
#include "app_config.h"
#include "system/includes.h"
#if TCFG_PAY_ALIOS_ENABLE && (TCFG_PAY_ALIOS_WAY_SEL==TCFG_PAY_ALIOS_WAY_T_HEAD)

const char log_tag_const_v_ALIPAY_HSP AT(.LOG_TAG_CONST) = 1;//LIB_DEBUG & FALSE;
const char log_tag_const_i_ALIPAY_HSP AT(.LOG_TAG_CONST) = 1;//LIB_DEBUG & TRUE;
const char log_tag_const_d_ALIPAY_HSP AT(.LOG_TAG_CONST) = 1;//LIB_DEBUG & TRUE;
const char log_tag_const_w_ALIPAY_HSP AT(.LOG_TAG_CONST) = 1;//LIB_DEBUG & TRUE;
const char log_tag_const_e_ALIPAY_HSP AT(.LOG_TAG_CONST) = 1;//LIB_DEBUG & TRUE;

#define LOG_TAG_CONST       ALIPAY_HSP
#define LOG_TAG             "[ALIPAY_HSP]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"

#if TCFG_TOUCH_USER_IIC_TYPE
#define iic_init(iic)                       hw_iic_init(iic)
#define iic_uninit(iic)                     hw_iic_uninit(iic)
#define iic_start(iic)                      hw_iic_start(iic)
#define iic_stop(iic)                       hw_iic_stop(iic)
#define iic_tx_byte(iic, byte)              hw_iic_tx_byte(iic, byte)
#define iic_rx_byte(iic, ack)               hw_iic_rx_byte(iic, ack)
#define iic_read_buf(iic, buf, len)         hw_iic_read_buf(iic, buf, len)
#define iic_write_buf(iic, buf, len)        hw_iic_write_buf(iic, buf, len)
#define iic_suspend(iic)                    hw_iic_suspend(iic)
#define iic_resume(iic)                     hw_iic_resume(iic)
#else
#define iic_init(iic)                       soft_iic_init(iic)
#define iic_uninit(iic)                     soft_iic_uninit(iic)
#define iic_start(iic)                      soft_iic_start(iic)
#define iic_stop(iic)                       soft_iic_stop(iic)
#define iic_tx_byte(iic, byte)              soft_iic_tx_byte(iic, byte)
#define iic_rx_byte(iic, ack)               soft_iic_rx_byte(iic, ack)
#define iic_read_buf(iic, buf, len)         soft_iic_read_buf(iic, buf, len)
#define iic_write_buf(iic, buf, len)        soft_iic_write_buf(iic, buf, len)
#define iic_suspend(iic)                    soft_iic_suspend(iic)
#define iic_resume(iic)                     soft_iic_resume(iic)
#endif

typedef struct {
    u8 init;
    hw_iic_dev iic_hdl;
} ft_param;

static ft_param module_param = {
    .init = 0,
    .iic_hdl = 1,
};
#define __this   (&module_param)

#define HOST_ID        1

extern const struct soft_iic_config soft_iic_cfg[];


unsigned char g_HSI2CBuf[4096];

//extern unsigned char * IIC_Master_Init(void);

//extern void IIC_Master_Send(unsigned char byAddr, unsigned char *pData, unsigned short wLen);

//extern void IIC_Master_Receive(unsigned char byAddr, unsigned char *pData, unsigned short wLen);

//CRC单字节计算——循环左移，先移最高bit
unsigned short LeftShift_CRC(unsigned char byValue, unsigned short *lpwCrc)
{
    // log_info("%s\n", __func__);
    byValue ^= (unsigned char)(*lpwCrc >> 8);
    byValue ^= (byValue >> 4);

    *lpwCrc = (*lpwCrc << 8) ^ (unsigned short)(byValue);
    *lpwCrc ^= (unsigned short)byValue << 12;
    *lpwCrc ^= (unsigned short)byValue << 5;

    return (*lpwCrc);
}

void ComputeCRC(unsigned char *Data, unsigned short Length, unsigned char *pbyOut)
{
    unsigned char chBlock;
    unsigned short wCrc;
    log_info("%s\n", __func__);

    wCrc = 0x0000; // ITU-V.41

    while (Length--) {
        chBlock = *Data++;
        LeftShift_CRC(chBlock, &wCrc);
    }

    pbyOut[0] = (unsigned char)((wCrc >> 8) & 0xFF);
    pbyOut[1] = (unsigned char)(wCrc & 0xFF);

    return;
}

void lmemset(unsigned char *pBuf, unsigned char byVal, unsigned short wLen)
{
    unsigned short i;
    log_info("%s\n", __func__);

    for (i = 0; i < wLen; ++i) {
        pBuf[i] = byVal;
    }
    return ;
}

void lmemcpy(unsigned char *pDst, unsigned char *pSrc, unsigned short wLen)
{
    unsigned short i;
    // log_info("%s\n", __func__);

    if ((pSrc == pDst) || (wLen == 0)) {
        return;
    }

    if (pSrc < pDst) {
        for (i = wLen - 1; i != 0; --i) {
            pDst[i] = pSrc[i];
        }
        pDst[0] = pSrc[0];
    } else {
        for (i = 0; i < wLen; ++i) {
            pDst[i] = pSrc[i];
        }
    }

    return ;
}

//相同返回0，不同返回1
unsigned char lmemcmp(unsigned char *pBuf1, unsigned char *pBuf2, unsigned short wLen)
{
    unsigned short i;
    log_info("%s\n", __func__);

    for (i = 0; i < wLen; ++i) {
        if (pBuf1[i] != pBuf2[i]) {
            return 1;
        }
    }

    return 0;
}

#define TEST_1		0

#define FT6336G_IIC_ADDR  0xC8
#define FT6336G_IIC_DELAY   1

#define SLAVE_ADDR      0x50

#define WRITE_ADDR      SLAVE_ADDR
#define READ_ADDR       (WRITE_ADDR|BIT(0))

#define TCFG_ALIPAY_TP_IO_MUL	0

#if  TCFG_ALIPAY_TP_IO_MUL
extern  spinlock_t tp_lock;
#endif
static int hs_i2c_write(u8 *buf, int len)
{

#if TEST_1
    int ret = 0;
    log_info("%s[len:%d]", __func__, len);
    soft_iic_start(__this->iic_hdl);
    ret = soft_iic_tx_byte(__this->iic_hdl, WRITE_ADDR);
    if (0 == ret) {
        ret = -1;
        goto __end;
    }
    delay(soft_iic_cfg[HOST_ID].delay);
    ret = soft_iic_tx_byte(__this->iic_hdl, 0x55);
    if (0 == ret) {
        ret = -2;
        goto __end;
    }
    delay(soft_iic_cfg[__this->iic_hdl].delay);
    ret = soft_iic_write_buf(__this->iic_hdl, buf, len);
    if (0 == ret) {
        ret = -3;
        goto __end;
    }
__end:
    soft_iic_stop(__this->iic_hdl);
    delay(soft_iic_cfg[__this->iic_hdl].delay);
    log_info("%s[ret:%d]", __func__, ret);
    return ret;
#else

    int ret = 0;
    log_info("%s[len:%d]", __func__, len);
#if  TCFG_ALIPAY_TP_IO_MUL
    spin_lock(&tp_lock);
#endif
    soft_iic_start(__this->iic_hdl);
    ret = soft_iic_tx_byte(__this->iic_hdl, FT6336G_IIC_ADDR);
    if (0 == ret) {
        ret = -1;
        goto __end;
    }
    delay(soft_iic_cfg[__this->iic_hdl].delay);
    ret = soft_iic_write_buf(__this->iic_hdl, buf, len);
    if (0 == ret) {
        ret = -3;
        goto __end;
    }
__end:
    soft_iic_stop(__this->iic_hdl);
    delay(soft_iic_cfg[__this->iic_hdl].delay);
    log_info("%s[ret:%d]", __func__, ret);
#if  TCFG_ALIPAY_TP_IO_MUL
    spin_unlock(&tp_lock);
#endif
    return ret;
#endif
}

static u8 hs_i2c_read(u8 addr, u8 *buf, unsigned short len)
{
#if TEST_1
    int ret = 0;
    log_info("%s[len:%d]", __func__, len);
    soft_iic_start(__this->iic_hdl);
    ret = soft_iic_tx_byte(__this->iic_hdl, WRITE_ADDR);
    if (0 == ret) {
        ret = -1;
        goto __end;
    }
    delay(soft_iic_cfg[__this->iic_hdl].delay);
    ret = soft_iic_tx_byte(__this->iic_hdl, 0xAA);
    if (0 == ret) {
        ret = -2;
        goto __end;
    }
    delay(soft_iic_cfg[__this->iic_hdl].delay);
    soft_iic_start(__this->iic_hdl);
    ret = soft_iic_tx_byte(__this->iic_hdl, READ_ADDR);
    if (0 == ret) {
        ret = -3;
        goto __end;
    }
    delay(soft_iic_cfg[HOST_ID].delay);
    ret = soft_iic_read_buf(__this->iic_hdl, buf, len);
    if (0 == ret) {
        ret = -4;
        goto __end;
    }
__end:
    soft_iic_stop(__this->iic_hdl);
    delay(soft_iic_cfg[HOST_ID].delay);
    log_info("%s[ret:0x%x]", __func__, ret);
    return ret;
#else
    int ret = 0;
#if  TCFG_ALIPAY_TP_IO_MUL
    spin_lock(&tp_lock);
#endif
    log_info("%s[len:%d]", __func__, len);

    soft_iic_start(__this->iic_hdl);
    ret = soft_iic_tx_byte(__this->iic_hdl, addr);
    if (0 == ret) {
        ret = -3;
        goto __end;
    }
    delay(soft_iic_cfg[__this->iic_hdl].delay);
    ret = soft_iic_read_buf(__this->iic_hdl, buf, len);
    if (0 == ret) {
        ret = -4;
        goto __end;
    }
__end:
    soft_iic_stop(__this->iic_hdl);
    delay(soft_iic_cfg[__this->iic_hdl].delay);
    put_buf(buf, len);
    log_info("%s[ret:0x%x]", __func__, ret);
#if  TCFG_ALIPAY_TP_IO_MUL
    spin_unlock(&tp_lock);
#endif
    return ret;
#endif
}

void HS_IIC_Init(void)
{
    log_info("%s[hd:%d]\n", __func__, __this->iic_hdl);
    // g_HSI2CBuf = IIC_Master_Init();
    iic_init(__this->iic_hdl);
    printf("delay:%d\n", soft_iic_cfg[__this->iic_hdl].delay);

}

void HS_IIC_Uninit(void)
{
    log_info("%s[hd:%d]\n", __func__, __this->iic_hdl);
#if TCFG_ALIPAY_TP_IO_MUL
    log_info("%s is NULL\n", __func__);
#else
    iic_uninit(__this->iic_hdl);
#endif
    printf("delay:%d\n", soft_iic_cfg[__this->iic_hdl].delay);
}

void HS_IIC_Send_Cmd(unsigned char *pData, unsigned short wLen)
{
    // IIC_Master_Send(0xC8, pData, wLen);
    log_info("%s[len:%d]\n", __func__, wLen);
    put_buf(pData, wLen);
    int ret = hs_i2c_write(pData, wLen);
    log_debug("%s[ret:%d]\n", __func__, ret);
}

//功能:	接收wLen长度的数据，到pData缓冲区中
//返回:
//			0 表示正确接收
//			1 表示接收数据CRC错误
//			2 表示实际接收数据长度域不是wLen
//			3 表示指令返回状态字不是9000
//			4 表示指令超时未响应
unsigned char HS_IIC_Recv_Resp(unsigned char *pData, unsigned short wLen)
{
    unsigned short wRspLen;
    unsigned char crc[2];
    unsigned int dwCnt;
    log_info("%s[len:%d]\n", __func__, wLen);
    int ret = 0;

    g_HSI2CBuf[0] = 0x00;
    dwCnt = 0;
    while (g_HSI2CBuf[0] != 0x55) {
        ++dwCnt;
        if (dwCnt > 0x1000) {

            return 4;
        }
        ret = hs_i2c_read(0xC9, g_HSI2CBuf, 1);
        // r_printf("%s[ret1:%d]\n", __func__, ret);
        // put_buf(g_HSI2CBuf,1);
        // g_printf("g_HSI2CBuf:0x%x\n", g_HSI2CBuf[0]);

    }

    int rlen = wLen + 6;

    hs_i2c_read(0xC9, g_HSI2CBuf, rlen);

    wRspLen = g_HSI2CBuf[0];
    wRspLen <<= 8;
    wRspLen |= g_HSI2CBuf[1];


    if (wRspLen != wLen + 2) {


        return 2;
    }
    if ((g_HSI2CBuf[2 + wLen] != 0x90) || (g_HSI2CBuf[3 + wLen] != 0x00)) { //状态字


        pData[0] = g_HSI2CBuf[2 + wLen];
        pData[1] = g_HSI2CBuf[3 + wLen];

        return 3;
    }



    ComputeCRC(g_HSI2CBuf + 2, wLen + 2, crc);


    if ((g_HSI2CBuf[wLen + 4] != crc[0]) || (g_HSI2CBuf[wLen + 5] != crc[1])) {


        return 1;
    }

    lmemcpy(pData, g_HSI2CBuf + 2, wLen);



    return 0;
}

static u8 write[16];
static u8 read[16];
static u8 receive[16];
void iic_master_test(void)
{

    int ret = 0;
    int ret1 = 0;
    // clk_set("sys", (48 * 1000000L));
    memset(read, 0x00, sizeof(read));
    write[0]++;
    for (u8 i = 0; i < sizeof(write); i++) {
        write[i] = i;
    }
    write[0] = 1;
    write[sizeof(write) - 1] = 0xff; //包结束符 //视情况修改
    HS_IIC_Send_Cmd(write, sizeof(write));
    /* delay(soft_iic_cfg[HOST_ID].delay); */
    ret1 = hs_i2c_read(0xAA, read, sizeof(read));

    log_info("%s[send 0x%x  0x%x]", __func__, write[0], ret);
    put_buf(receive, sizeof(receive));
    log_info("%s[read 0x%x]", __func__, ret1);
    if (ret1 > 0) {
        put_buf(read, ret1);
    }
}

#endif
