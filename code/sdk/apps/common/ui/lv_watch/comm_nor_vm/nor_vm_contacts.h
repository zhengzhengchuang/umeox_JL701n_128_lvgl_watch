#ifndef __NOR_VM_CONTACTS_H__
#define __NOR_VM_CONTACTS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"
#include "../include/comm_call.h"

/*********************************************************************************
                                  联系人最大个数                                  
*********************************************************************************/
#define Contacts_Max_Num (10)

/*********************************************************************************
                                  联系人内容                                  
*********************************************************************************/
typedef struct
{
    uint16_t check_code;
    
    char name_str[Call_Name_Max_Len + 1];
    char number_str[Call_Number_Max_Len + 1];
}vm_contacts_ctx_t;
extern vm_contacts_ctx_t w_contacts;
extern vm_contacts_ctx_t r_contacts;

/*********************************************************************************
                                  联系人接口                                       
*********************************************************************************/
void VmContactsCtxClear(void);
uint8_t VmContactsItemNum(void);
bool VmContactsCtxByIdx(uint8_t idx);
void VmContactsCtxFlashSave(uint8_t idx, void *p);
#ifdef __cplusplus
}
#endif

#endif
