
#include "hs_private.h"
#include "iotsec.h"
#include "system/includes.h"

//--------------------------------------------------------------------------
//
//									版本V1.0.02
//
//--------------------------------------------------------------------------


#undef _DEBUG_H_
#define LOG_TAG_CONST       ALIPAY_SSP          //修改成文件名
#define LOG_TAG             "[ALIPAY_SSP]"
#include "debug.h"
#define LOG_v(t)  log_tag_const_v_ ## t
#define LOG_i(t)  log_tag_const_i_ ## t
#define LOG_d(t)  log_tag_const_d_ ## t
#define LOG_w(t)  log_tag_const_w_ ## t
#define LOG_e(t)  log_tag_const_e_ ## t
#define LOG_c(t)  log_tag_const_c_ ## t
#define LOG_tag(tag, n) n(tag)
const char LOG_tag(LOG_TAG_CONST, LOG_v) AT(.LOG_TAG_CONST) = 1;
const char LOG_tag(LOG_TAG_CONST, LOG_i) AT(.LOG_TAG_CONST) = 1; //log_info
const char LOG_tag(LOG_TAG_CONST, LOG_d) AT(.LOG_TAG_CONST) = 1; //log_debug
const char LOG_tag(LOG_TAG_CONST, LOG_w) AT(.LOG_TAG_CONST) = 1;
const char LOG_tag(LOG_TAG_CONST, LOG_e) AT(.LOG_TAG_CONST) = 1;
const char LOG_tag(LOG_TAG_CONST, LOG_c) AT(.LOG_TAG_CONST) = 1;

#include "app_config.h"
#if TCFG_PAY_ALIOS_ENABLE && (TCFG_PAY_ALIOS_WAY_SEL==TCFG_PAY_ALIOS_WAY_T_HEAD)

/*○ 功能描述
    ■ 获取读取资产的文件句柄
  ○ 接口参数
    ■ fd  - 存放资产句柄
    ■ asset_name - 资产名字
  ○ 返回值
    ■ 0表示成功，非0表示失败*/
csi_error_t csi_open_asset(void **fd, char asset_name[36])
{
    unsigned char tmpBuf[10];
    unsigned char i;

    log_info("%s\n", __func__);
    for (i = 0; i < 36; ++i) {
        if (asset_name[i] == 0) {
            break;
        }
    }

    if ((fd == 0) || (i == 0)) {
        return CSI_ERROR;
    }

    g_HSI2CBuf[0] = 0x00;
    g_HSI2CBuf[1] = i + 5;
    g_HSI2CBuf[2] = 0x80;
    g_HSI2CBuf[3] = 0xE0;
    g_HSI2CBuf[4] = 0x00;
    g_HSI2CBuf[5] = 0x00;
    g_HSI2CBuf[6] = i;
    lmemcpy(g_HSI2CBuf + 7, (unsigned char *)asset_name, i);

    ComputeCRC(g_HSI2CBuf, i + 7, g_HSI2CBuf + i + 7);
    HS_IIC_Send_Cmd(g_HSI2CBuf, i + 9);
    if (0 != HS_IIC_Recv_Resp(tmpBuf, 2)) {
        return CSI_ERROR;
    }

    *fd = (void *)(tmpBuf[0] * 256 + tmpBuf[1]);	//文件地址
    return CSI_OK;
}

/*○ 功能描述
    ■ 写入指定的资产
  ○ 接口参数
    ■ fd  - 存放资产句柄
    ■ offset - 资产数据偏移
    ■ buffer - 存放资产数据缓冲区
    ■ data_len - 写入资产数据个数
  ○ 返回值
    ■ 0表示成功，非0表示失败*/
csi_error_t csi_write_asset(void *fd, uint32_t offset, void *data, uint32_t data_len)
{
    unsigned int dwBufOff;
    unsigned char abyBuf[10];
    log_info("%s\n", __func__);

    if (offset + data_len > 4096) {
        return CSI_ERROR;
    }
    if (data == 0) {
        return CSI_ERROR;
    }

    //先根据fd选择文件
    g_HSI2CBuf[0] = 0x00;
    g_HSI2CBuf[1] = 0x07;
    g_HSI2CBuf[2] = 0x00;
    g_HSI2CBuf[3] = 0xA4;
    g_HSI2CBuf[4] = 0x00;
    g_HSI2CBuf[5] = 0x00;
    g_HSI2CBuf[6] = 0x02;
    g_HSI2CBuf[7] = (unsigned char)((unsigned int)fd / 256);
    g_HSI2CBuf[8] = (unsigned int)fd % 256;

    ComputeCRC(g_HSI2CBuf, 9, g_HSI2CBuf + 9);
    HS_IIC_Send_Cmd(g_HSI2CBuf, 11);
    if (0 != HS_IIC_Recv_Resp(abyBuf, 2)) {
        return CSI_ERROR;
    }

    if (data_len == 0) {
        return CSI_OK;
    }

    dwBufOff = 0;
    while (data_len > 1024) {
        g_HSI2CBuf[0] = 0x04;
        g_HSI2CBuf[1] = 0x07;
        g_HSI2CBuf[2] = 0x00;
        g_HSI2CBuf[3] = 0xd6;
        g_HSI2CBuf[4] = (unsigned char)(offset / 256);
        g_HSI2CBuf[5] = (unsigned char)(offset % 256);
        g_HSI2CBuf[6] = 0x00;
        g_HSI2CBuf[7] = 0x04;
        g_HSI2CBuf[8] = 0x00;
        lmemcpy(g_HSI2CBuf + 9, (unsigned char *)data + dwBufOff, 1024);

        ComputeCRC(g_HSI2CBuf, 1033, g_HSI2CBuf + 1033);
        HS_IIC_Send_Cmd(g_HSI2CBuf, 1035);
        if (0 != HS_IIC_Recv_Resp(abyBuf, 0)) {
            return CSI_ERROR;
        }
        offset += 1024;
        data_len -= 1024;
        dwBufOff += 1024;
    }

    g_HSI2CBuf[0] = (unsigned char)((data_len + 7) / 256);
    g_HSI2CBuf[1] = (unsigned char)((data_len + 7) % 256);
    g_HSI2CBuf[2] = 0x80;
    g_HSI2CBuf[3] = 0xd6;
    g_HSI2CBuf[4] = (unsigned char)(offset / 256);
    g_HSI2CBuf[5] = (unsigned char)(offset % 256);
    g_HSI2CBuf[6] = 0x00;
    g_HSI2CBuf[7] = (unsigned char)(data_len / 256);
    g_HSI2CBuf[8] = (unsigned char)(data_len % 256);
    lmemcpy(g_HSI2CBuf + 9, (unsigned char *)data + dwBufOff, data_len);

    ComputeCRC(g_HSI2CBuf, data_len + 9, g_HSI2CBuf + data_len + 9);
    HS_IIC_Send_Cmd(g_HSI2CBuf, data_len + 11);
    if (0 != HS_IIC_Recv_Resp(abyBuf, 0)) {
        return CSI_ERROR;
    }

    return CSI_OK;
}

/*○ 功能描述
    ■ 读取指定的资产
  ○ 接口参数
    ■ fd  - 存放资产句柄
    ■ offset - 资产数据偏移
    ■ buffer - 存放资产数据缓冲区
    ■ data_len - 返回读取资产数据个数
  ○ 返回值
    ■ 0表示成功，非0表示失败*/
csi_error_t csi_read_asset(void *fd, uint32_t offset, void *buffer, uint32_t *data_len)
{
    unsigned int dwOff;
    unsigned char abyBuf[10];
    unsigned short wFileLen;
    uint32_t dwTmpLen;
    log_info("%s[data_len:%d]\n", __func__, *data_len);


    if (*data_len == 0) {
        return CSI_ERROR;
    }

    if (buffer == 0) {
        return CSI_ERROR;
    }


    //先根据fd选择文件
    g_HSI2CBuf[0] = 0x00;
    g_HSI2CBuf[1] = 0x07;
    g_HSI2CBuf[2] = 0x00;
    g_HSI2CBuf[3] = 0xA4;
    g_HSI2CBuf[4] = 0x00;
    g_HSI2CBuf[5] = 0x00;
    g_HSI2CBuf[6] = 0x02;
    g_HSI2CBuf[7] = (unsigned char)((unsigned int)fd / 256);
    g_HSI2CBuf[8] = (unsigned int)fd % 256;

    ComputeCRC(g_HSI2CBuf, 9, g_HSI2CBuf + 9);
    HS_IIC_Send_Cmd(g_HSI2CBuf, 11);

    if (0 != HS_IIC_Recv_Resp(abyBuf, 2)) {
        return CSI_ERROR;
    }

    wFileLen = abyBuf[0] * 256 + abyBuf[1];


    if (wFileLen == 0) {
        *data_len = 0;


        return CSI_ERROR;
    }



    if (offset >= wFileLen) {


        return CSI_ERROR;
    }



    if (offset + *data_len > wFileLen) {


        *data_len = wFileLen - offset;
    }



    dwOff = 0;
    dwTmpLen = *data_len;
    while (dwTmpLen > 1024) {


        g_HSI2CBuf[0] = 0x00;
        g_HSI2CBuf[1] = 0x07;
        g_HSI2CBuf[2] = 0x00;
        g_HSI2CBuf[3] = 0xB0;
        g_HSI2CBuf[4] = (unsigned char)(offset / 256);
        g_HSI2CBuf[5] = (unsigned char)(offset % 256);
        g_HSI2CBuf[6] = 0x00;
        g_HSI2CBuf[7] = 0x04;
        g_HSI2CBuf[8] = 0x00;

        ComputeCRC(g_HSI2CBuf, 9, g_HSI2CBuf + 9);
        HS_IIC_Send_Cmd(g_HSI2CBuf, 11);
        if (0 == HS_IIC_Recv_Resp((unsigned char *)buffer + dwOff, 1024)) {
            offset += 1024;
            dwOff += 1024;
            dwTmpLen -= 1024;
        } else {


            return CSI_ERROR;
        }
    }



    g_HSI2CBuf[0] = 0x00;
    g_HSI2CBuf[1] = 0x07;
    g_HSI2CBuf[2] = 0x00;
    g_HSI2CBuf[3] = 0xB0;
    g_HSI2CBuf[4] = (unsigned char)(offset / 256);
    g_HSI2CBuf[5] = (unsigned char)(offset % 256);
    g_HSI2CBuf[6] = 0x00;
    g_HSI2CBuf[7] = (unsigned char)(dwTmpLen / 256);
    g_HSI2CBuf[8] = (unsigned char)(dwTmpLen % 256);


    ComputeCRC(g_HSI2CBuf, 9, g_HSI2CBuf + 9);
    HS_IIC_Send_Cmd(g_HSI2CBuf, 11);
    if (0 != HS_IIC_Recv_Resp((unsigned char *)buffer + dwOff, dwTmpLen)) {


        return CSI_ERROR;
    }

    return CSI_OK;
}

/*○ 功能描述
    ■ 关闭指定的资产句柄
  ○ 接口参数
    ■ fd  - 存放资产句柄
  ○ 返回值
    ■ 0表示成功，非0表示失败*/
csi_error_t csi_close_asset(void *fd)
{
    unsigned char abySW[2];
    log_info("%s\n", __func__);


    g_HSI2CBuf[0] = 0x00;
    g_HSI2CBuf[1] = 0x07;
    g_HSI2CBuf[2] = 0x80;
    g_HSI2CBuf[3] = 0xA6;
    g_HSI2CBuf[4] = 0x00;
    g_HSI2CBuf[5] = 0x00;
    g_HSI2CBuf[6] = 0x02;
    g_HSI2CBuf[7] = (unsigned char)((unsigned int)fd / 256);
    g_HSI2CBuf[8] = (unsigned int)fd % 256;

    ComputeCRC(g_HSI2CBuf, 9, g_HSI2CBuf + 9);
    HS_IIC_Send_Cmd(g_HSI2CBuf, 11);
    if (0 != HS_IIC_Recv_Resp(abySW, 0)) {
        return CSI_ERROR;
    }

    return CSI_OK;
}

/*○ 功能描述
    ■ 确认指定的资产是否可访问
  ○ 接口参数
    ■ access-flag - 资产访问结果标识: 资产存在设置为1，资产不存在设置为0
    ■ asset_name - 资产名字
  ○ 返回值
    ■ 0表示成功，非0表示失败*/
csi_error_t csi_access_asset(uint32_t *access_flag, char asset_name[36])
{
    unsigned char i;
    unsigned char abyBuf[10];
    log_info("%s\n", __func__);

    for (i = 0; i < 36; ++i) {
        if (asset_name[i] == 0) {
            break;
        }
    }
    if (i == 0) {
        return CSI_ERROR;
    }

    g_HSI2CBuf[0] = 0x00;
    g_HSI2CBuf[1] = i + 5;
    g_HSI2CBuf[2] = 0x80;
    g_HSI2CBuf[3] = 0xA4;
    g_HSI2CBuf[4] = 0x04;
    g_HSI2CBuf[5] = 0x00;
    g_HSI2CBuf[6] = i;
    lmemcpy(g_HSI2CBuf + 7, (unsigned char *)asset_name, i);

    ComputeCRC(g_HSI2CBuf, i + 7, g_HSI2CBuf + i + 7);
    HS_IIC_Send_Cmd(g_HSI2CBuf, i + 9);
    abyBuf[0] = 0x00;
    abyBuf[1] = 0x00;
    if (0 != HS_IIC_Recv_Resp(abyBuf, 0)) {
        if ((abyBuf[0] == 0x6A) && (abyBuf[1] == 0x82)) {
            *access_flag = 0;
            return CSI_OK;
        }
        return CSI_ERROR;
    }

    *access_flag = 1;

    return CSI_OK;
}

/*○ 功能描述
    ■ 清除指定的资产
  ○ 接口参数
    ■ asset_name - 资产名字
  ○ 返回值
    ■ 0表示成功，非0表示失败*/
csi_error_t csi_remove_asset(char asset_name[36])
{
    unsigned char i;
    unsigned char abySW[2];
    log_info("%s\n", __func__);

    for (i = 0; i < 36; ++i) {
        if (asset_name[i] == 0) {
            break;
        }
    }
    if (i == 0) {
        return CSI_ERROR;
    }

    g_HSI2CBuf[0] = 0x00;
    g_HSI2CBuf[1] = i + 5;
    g_HSI2CBuf[2] = 0x00;
    g_HSI2CBuf[3] = 0x0E;
    g_HSI2CBuf[4] = 0x00;
    g_HSI2CBuf[5] = 0x00;
    g_HSI2CBuf[6] = i;
    lmemcpy(g_HSI2CBuf + 7, (unsigned char *)asset_name, i);

    ComputeCRC(g_HSI2CBuf, i + 7, g_HSI2CBuf + i + 7);
    HS_IIC_Send_Cmd(g_HSI2CBuf, i + 9);
    if (0 != HS_IIC_Recv_Resp(abySW, 0)) {
        return CSI_ERROR;
    }

    return CSI_OK;
}

/*○ 功能描述
    ■ 清除所有资产
  ○ 接口参数
    ■ 无
  ○ 返回值
    ■ 0表示成功，非0表示失败*/
csi_error_t csi_clear_assets(void)
{
    unsigned char abySW[2];
    log_info("%s\n", __func__);


    g_HSI2CBuf[0] = 0x00;

    g_HSI2CBuf[1] = 0x05;
    g_HSI2CBuf[2] = 0x80;
    g_HSI2CBuf[3] = 0x0E;
    g_HSI2CBuf[4] = 0x00;
    g_HSI2CBuf[5] = 0x00;
    g_HSI2CBuf[6] = 0x00;


    ComputeCRC(g_HSI2CBuf, 7, g_HSI2CBuf + 7);

    HS_IIC_Send_Cmd(g_HSI2CBuf, 9);
    if (0 != HS_IIC_Recv_Resp(abySW, 0)) {
        return CSI_ERROR;
    }

    return CSI_OK;
}



/*○ 功能描述
    ■ 获取安全芯片固件版本号
  ○ 接口参数
    ■ ver - 存放版本号buffer
  ○ 返回值
    ■ 0表示成功，非0表示失败*/
csi_error_t csi_get_fw_version(char ver[8])
{
    g_HSI2CBuf[0] = 0x00;
    g_HSI2CBuf[1] = 0x05;
    g_HSI2CBuf[2] = 0x00;
    g_HSI2CBuf[3] = 0xBC;
    g_HSI2CBuf[4] = 0x00;
    g_HSI2CBuf[5] = 0x00;
    g_HSI2CBuf[6] = 0x08;
    ComputeCRC(g_HSI2CBuf, 7, g_HSI2CBuf + 7);
    HS_IIC_Send_Cmd(g_HSI2CBuf, 9);
    if (0 != HS_IIC_Recv_Resp(ver, 8)) {
        return CSI_ERROR;
    }

    return CSI_OK;

}


#endif
