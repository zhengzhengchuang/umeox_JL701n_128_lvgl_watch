#ifndef __QURAN_PLAY_FUNC_H__
#define __QURAN_PLAY_FUNC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

#define QpMaxNum (QpAudioNum)

#define QpSelIdxInvalid (-1)//无效

enum
{
    dir_type_folder,
    dir_type_file,
};

enum
{
    Qp_Mor_R = 0x7e,//早上祈祷词
    Qp_Eve_R = 0x7f,//晚上祈祷词
};

typedef struct  
{
    FILE *file;
    int total_num;
    FS_DIR_INFO *dir_buf;
}QpFsPara_t;

typedef struct
{
    u16 vm_mask;

    s8 SelIdx;//选中

    bool FavFlag[QpMaxNum];
}QpUserPara_t;
extern QpUserPara_t QpUser;

int QpIsRunning(void);

void QpUpdateSelIdx(void);

int QpGetTotalNum(void);
FS_DIR_INFO *QpGetFsDirInfo(void);

int QpGetDecCurTime(void);
int QpGetDecTotalTime(void);

u8 QpGetCycleMode(void);
void QpSetCycleMode(u8 mode);

void QpPPHandle(void);
void QpPrevHandle(void);
void QpNextHandle(void);
void QpAudioBySclust(FS_DIR_INFO *info);

void QpOpenFile(void);
void QpCloseFile(void);
void QpFileRead(u8 idx);

void QpUserInfoParaRead(void);
void QpUserInfoParaWrite(void);
void QpUserInfoParaReset(void);
void QpUserInfoParaUpdate(void);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
