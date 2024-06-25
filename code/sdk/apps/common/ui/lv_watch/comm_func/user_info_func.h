#ifndef __USER_INFO_FUNC_H__
#define __USER_INFO_FUNC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

enum
{
    Gender_Female = 0x00,
    Gender_Male,
};

typedef struct
{
    u16 vm_mask;

    uint8_t age;
    uint8_t gender;
    uint8_t height;//cm
    uint8_t weight;//kg
}UserInfoPara_t;
extern UserInfoPara_t User_Info;

void UserInfoParaRead(void);
void UserInfoParaWrite(void);
void UserInfoParaReset(void);
void UserInfoParaUpdate(void);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
