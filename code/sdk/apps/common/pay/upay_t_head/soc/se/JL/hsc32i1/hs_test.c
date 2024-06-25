/*
 * @Description:
 * @Author: HJY
 * @Date: 2022-11-16 17:46:07
 * @LastEditTime: 2022-12-15 17:31:19
 * @LastEditors: HJY
 */
// #include "type.h"
#include "hs_private.h"
#include "iotsec.h"
#include "app_config.h"
#if TCFG_PAY_ALIOS_ENABLE && (TCFG_PAY_ALIOS_WAY_SEL==TCFG_PAY_ALIOS_WAY_T_HEAD)
#undef _DEBUG_H_
#define LOG_TAG_CONST       HS_TEST          //修改成文件名
#define LOG_TAG             "[HS_TEST]"
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


extern csi_error_t csi_rng_get_data(void *data, uint32_t size);
extern csi_error_t csi_get_huk(uint8_t *uid, uint32_t *len);
extern csi_error_t csi_open_asset(void **fd, char asset_name[36]);
extern csi_error_t csi_write_asset(void *fd, uint32_t offset, void *data, uint32_t data_len);
extern csi_error_t csi_read_asset(void *fd, uint32_t offset, void *buffer, uint32_t *data_len);
extern csi_error_t csi_close_asset(void *fd);
extern csi_error_t csi_access_asset(uint32_t *access_flag, char asset_name[36]);
extern csi_error_t csi_remove_asset(char asset_name[36]);
extern csi_error_t csi_clear_assets(void);

extern void *Memory_Malloc(unsigned int);
extern void Memory_Free(void *pv);
extern void pringt_string_log(char *format, ...);
#define MyPrintf  printf



void IIC_TEST_DEMO(void)
{
    unsigned int i;
    unsigned int dwTmp;
    unsigned int dwReadLen;
    unsigned char *pTestBuf;
    unsigned int addrFile1;
    unsigned int addrFile2;
    unsigned short wCnt;


    //HS_IIC_Init();

    // extern void iic_master_test(void);
    // iic_master_test();
    // return;


    pTestBuf = malloc(4096);


    //测试串口打印数据是否正确
    for (i = 0; i < 256; ++i) {
        pTestBuf[i] = 255 - i;
    }
    MyPrintf("Uart Test,data = 0xFF - 0x00 :\n");
    for (i = 0; i < 256; ++i) {
        MyPrintf("%02x", pTestBuf[i]);
    }
    MyPrintf("\n");


    MyPrintf("IIC_TEST_DEMO test start:\n\n");


    for (wCnt = 0; wCnt < 2; ++wCnt) {
//		if(wCnt % 10 == 0)
        extern void wdt_clear(void);
        wdt_clear();
        {
            MyPrintf("IIC_TEST_DEMO %02x  ", wCnt);
        }

        //删除所有文件
        /*  pTestBuf[0] = csi_clear_assets();
          if(pTestBuf[0] != CSI_OK)
          {
              MyPrintf("csi_clear_assets ERROR! \n");
              while(1);
          }*/



        //创建测试文件1
        lmemset(pTestBuf, 0x00, 36);
        lmemset(pTestBuf + 1, 0x11, 32);
        MyPrintf("csi_open_asset1 is in-! \n");
        pTestBuf[0] = csi_open_asset((void **)(&addrFile1), (char *)(pTestBuf + 1));

        if (pTestBuf[0] != CSI_OK) {
            MyPrintf("csi_open_asset ERROR! \n");
            while (1);
        }


        //关闭文件1
        MyPrintf("csi_close_asset1 is in-! \n");
        pTestBuf[0] = csi_close_asset((void *)addrFile1);
        if (pTestBuf[0] != CSI_OK) {
            MyPrintf("csi_close_asset ERROR! \n");
            while (1);
        }



        //先读
        dwReadLen = 256;
        MyPrintf("csi_read_asset1 is in-! \n");
        pTestBuf[0] = csi_read_asset((void *)addrFile1, 128, pTestBuf + 1, &dwReadLen);
        if (pTestBuf[0] == CSI_OK) {
            MyPrintf("csi_read_asset read empty file ERROR! \n");
            while (1);
        }

        if (dwReadLen != 0) {
            MyPrintf("csi_read_asset, the return value of data_len ERROR! \n");
            while (1);
        }


        //写文件1
        for (i = 0; i < 256; ++i) {
            pTestBuf[i + 1] = i % 256;
        }
        MyPrintf("csi_write_asset1 is in-! \n");
        pTestBuf[0] = csi_write_asset((void *)addrFile1, 128, pTestBuf + 1, 256);
        if (pTestBuf[0] != CSI_OK) {
            MyPrintf("csi_write_asset ERROR! \n");
            while (1);
        }

        //读文件1
        dwReadLen = 256;
        MyPrintf("csi_read_asset2 is in-! \n");
        pTestBuf[0] = csi_read_asset((void *)addrFile1, 128, pTestBuf + 1, &dwReadLen);
        if (pTestBuf[0] != CSI_OK) {
            MyPrintf("csi_read_asset ERROR! \n");
            while (1);
        } else {
            for (i = 0; i < 256; ++i) {
                if (pTestBuf[i + 1] != i) {
                    break;
                }
            }
            if (i != 256) {
                MyPrintf("csi_read_asset -- Data Check ERROR \n");
                while (1);
            }
        }


        //打开测试文件1
        lmemset(pTestBuf, 0x00, 36);
        lmemset(pTestBuf + 1, 0x11, 32);
        MyPrintf("csi_open_asset2 is in-! \n");
        pTestBuf[0] = csi_open_asset((void **)(&addrFile1), (char *)(pTestBuf + 1));
        if (pTestBuf[0] != CSI_OK) {
            MyPrintf("csi_open_asset ERROR! \n");
            while (1);
        }


        //写文件1
        for (i = 0; i < 256; ++i) {
            pTestBuf[i + 1] = i % 256;
        }
        MyPrintf("csi_write_asset2 is in-! \n");
        pTestBuf[0] = csi_write_asset((void *)addrFile1, 128, pTestBuf + 1, 256);
        if (pTestBuf[0] != CSI_OK) {
            MyPrintf("csi_write_asset ERROR! \n");
            while (1);
        }


        //读文件1
        dwReadLen = 256;
        MyPrintf("csi_read_asset2 is in-! \n");
        pTestBuf[0] = csi_read_asset((void *)addrFile1, 128, pTestBuf + 1, &dwReadLen);
        if (pTestBuf[0] != CSI_OK) {
            MyPrintf("csi_read_asset ERROR! \n");
            while (1);
        } else {
            for (i = 0; i < 256; ++i) {
                if (pTestBuf[i + 1] != i) {
                    break;
                }
            }
            if (i != 256) {
                MyPrintf("csi_read_asset -- Data Check ERROR \n");
                while (1);
            }
        }
    }


    MyPrintf("IIC_TEST_DEMO test END:\n\n");

    free(pTestBuf);
}

#undef MyPrintf

#endif
