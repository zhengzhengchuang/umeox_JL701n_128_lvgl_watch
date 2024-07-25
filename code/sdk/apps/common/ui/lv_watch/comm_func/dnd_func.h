#ifndef __DND_FUNC_H__
#define __DND_FUNC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

typedef struct
{
    u16 vm_mask;

    bool main_sw;

    //定时勿扰
    bool enable; 
    u8 dnd_start_hour;
    u8 dnd_start_minute;
    u8 dnd_end_hour;
    u8 dnd_end_minute;
    u8 dnd_repeat;
}DndParaInfo_t;
extern DndParaInfo_t Dnd_Info;

void DndInfoParaRead(void);
void DndInfoParaWrite(void);
void DndInfoParaReset(void);
void DndInfoParaUpdate(void);

void DndManualUpdate(void);//手动立即刷新
void DndProcess(struct sys_time *ptime);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
