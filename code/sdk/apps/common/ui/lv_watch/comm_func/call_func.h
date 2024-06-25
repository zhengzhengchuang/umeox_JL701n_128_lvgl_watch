#ifndef __CALL_FUNC_H__
#define __CALL_FUNC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

char *GetContactsName(u8 idx);
char *GetContactsNumber(u8 idx);

void CtrlCallHangUp(void);
void CtrlCallAnswer(void);
char *GetCallNumName(void);
void CtrlCallOutByNum(char *num, u8 len);

u8 GetCallOutOrIn(void);
void SetCallOutOrIn(u8 val);
bool GetCallAnswerState(void);
void SetCallAnswerState(bool state);
void ClearThisCallProcess(void);
void UpdateCallLogVmFlash(void);
void VmCallLogCtxFlashWrite(void);

void UpdateContactsVmFlash(void);
char *GetContactsNameByNumber(char *number);

void CallOutOrInProcess(void);
void CallHangUpAfterHandle(void);
void CallAnswerAfterHandle(void);

u16 GetCallOnlineTimerId(void);
u32 GetCallOnlineDuration(void);
void CallOnlineDurationTimerCreate(void);
void CallOnlineDurationTimerDelete(void);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
