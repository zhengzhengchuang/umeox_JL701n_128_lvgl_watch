#ifndef __TWS_FUNC_H__
#define __TWS_FUNC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

#define TwsNone        (0xff)
#define Tws_Scan_Num   (5)
#define Tws_Paired_Num (5)
#define Tws_Name_Len   (32)

enum
{
    TWS_SCAN_STOP,    /*扫描停止*/
    TWS_SCANNING,     /*扫描中*/  
    TWS_SCAN_TIMEOUT, /*扫描超时*/
};

enum
{
    TWS_CONNECTING, /*连接中*/
    TWS_CONNECTED,  /*连接完成*/
    TWS_DISCONNECT,  /*断连*/
    TWS_CONNECT_TIMEOUT,  /*连接超时*/
};

typedef struct
{
    u8 mac[6];
    char name[Tws_Name_Len + 1];
}TwsDsc_t;

typedef struct
{
    u16 vm_mask;

    u8 num;
    TwsDsc_t Paired[Tws_Paired_Num];
}TwsInfoPara_t;
extern TwsInfoPara_t TwsPara;

typedef struct
{
    u8 num;
    u8 state;
    TwsDsc_t Para[Tws_Scan_Num];
}TwsScanPara_t;
extern TwsScanPara_t TwsScan;

typedef struct
{
    u8 dev;
    u8 state;
    TwsDsc_t Conn;
}TwsConnPara_t;
extern TwsConnPara_t TwsConn;

u8 get_unpair_dev(void);
void set_unpair_dev(u8 dev);
bool GetNewPairedDevFlag(void);
void SetNewPairedDevFlag(bool f);

void TwsBtUserDiscHandle(void);
void TwsBtEmitterStart(void);
void TwsBtEmitterConnByMac(u8 *mac);
void TwsBtScanResultParse(u8 *buf, u8 len);

void TwsBtUserUnpair(u8 dev);

void TwsConnCompleteHandle(u8 *mac);
void TwsDisConnCompleteHandle(u8 *mac);

void TwsInfoParaRead(void);
void TwsInfoParaWrite(void);
void TwsInfoParaReset(void);
void TwsInfoParaUpdate(void);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
