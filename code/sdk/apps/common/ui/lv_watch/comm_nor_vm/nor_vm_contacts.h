#ifndef __NOR_VM_CONTACTS_H__
#define __NOR_VM_CONTACTS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"
#include "../include/comm_call.h"

#define Contacts_Max_Num (10)

typedef struct
{
    u16 check_code;
    
    char name_str[Call_Name_Max_Len + 1];
    char number_str[Call_Number_Max_Len + 1];
}vm_contacts_ctx_t;
extern vm_contacts_ctx_t w_contacts;
extern vm_contacts_ctx_t r_contacts;

void VmContactsCtxClear(void);
u8 VmContactsItemNum(void);
bool VmContactsCtxByIdx(u8 idx);
void VmContactsCtxFlashSave(u8 idx, void *p);
#ifdef __cplusplus
}
#endif

#endif
