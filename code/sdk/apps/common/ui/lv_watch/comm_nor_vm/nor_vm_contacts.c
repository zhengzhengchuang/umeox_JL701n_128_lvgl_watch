#include "nor_vm_main.h"

vm_contacts_ctx_t w_contacts;
vm_contacts_ctx_t r_contacts;
static const nor_vm_type_t nor_vm_type = \
    nor_vm_type_contacts;

/*********************************************************************************
                              清除                                         
*********************************************************************************/
void VmContactsCtxClear(void)
{
    void *nor_vm_file = nor_flash_vm_file(nor_vm_type);
    if(!nor_vm_file) return;

    u8 num = VmContactsItemNum();

    while(num)
    {
        flash_common_delete_by_index(nor_vm_file, 0);
        num--;
    }
    
    return;
}

/*********************************************************************************
                              存储数量                                         
*********************************************************************************/
u8 VmContactsItemNum(void)
{
    u8 num = 0;

    void *nor_vm_file = nor_flash_vm_file(nor_vm_type);
    if(!nor_vm_file) return num;

    num = flash_common_get_total(nor_vm_file);

    if(num > Contacts_Max_Num)
        num = Contacts_Max_Num;

    return num;
}

/*********************************************************************************
                              通过下标获取联系人内容                                        
*********************************************************************************/
bool VmContactsCtxByIdx(u8 idx)
{
    u8 num = VmContactsItemNum();
    if(idx >= num) return false;
    
    void *nor_vm_file = nor_flash_vm_file(nor_vm_type);
    int ctx_len = sizeof(vm_contacts_ctx_t);

    if(!nor_vm_file) return false;
    
    memset(&r_contacts, 0, ctx_len);
    flash_common_read_by_index(nor_vm_file, idx, 0, \
        ctx_len, (u8 *)&r_contacts);

    if(r_contacts.check_code != Nor_Vm_Check_Code)
        return false;

    return true;
}
 
/*********************************************************************************
                              联系人存储                                   
*********************************************************************************/
void VmContactsCtxFlashSave(u8 idx, void *p)
{
    bool BondFlag = GetDevBondFlag();
    if(BondFlag == false) return;

    if(!p) return;

    void *nor_vm_file = nor_flash_vm_file(nor_vm_type);
    int ctx_len = sizeof(vm_contacts_ctx_t);

    if(!nor_vm_file) return;

    flash_common_write_file(nor_vm_file, 0, ctx_len, (u8 *)p);

    return;
}
