#ifndef __TWS_FUNC_H__
#define __TWS_FUNC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

#define Tws_Scan_Num   (8)
#define Tws_Paired_Num (5)
#define Tws_Name_Len   (32)

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
    TwsDsc_t Para[Tws_Scan_Num];
}TwsScanPara_t;
extern TwsScanPara_t TwsScan;

void TwsBtEmitterStart(void);
void TwsBtEmitterConnByMac(u8 *mac);

bool GetTwsScanComplete(void);
void SetTwsScanComplete(bool f);

bool GetTwsConnComplete(void);
void SetTwsConnComplete(bool f);

void TwsBtScanResultParse(u8 *buf, u8 len);

void TwsInfoParaRead(void);
void TwsInfoParaWrite(void);
void TwsInfoParaReset(void);
void TwsInfoParaUpdate(void);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
