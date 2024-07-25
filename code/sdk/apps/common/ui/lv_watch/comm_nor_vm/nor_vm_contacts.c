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
    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);

    if(!nor_vm_file)
        return;

    uint8_t contacts_num = \
        VmContactsItemNum();

    while(contacts_num)
    {
        flash_common_delete_by_index(nor_vm_file, 0);

        contacts_num--;
    }
    
    return;
}

/*********************************************************************************
                              存储数量                                         
*********************************************************************************/
uint8_t VmContactsItemNum(void)
{
    uint8_t contacts_num = 0;

    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);

    if(!nor_vm_file)
        return contacts_num;

    contacts_num = \
        flash_common_get_total(nor_vm_file);

    if(contacts_num > Contacts_Max_Num)
        contacts_num = Contacts_Max_Num;

    return contacts_num;
}

/*********************************************************************************
                              通过下标获取联系人内容                                        
*********************************************************************************/
bool VmContactsCtxByIdx(uint8_t idx)
{
    uint8_t contacts_num = \
        VmContactsItemNum();

    if(idx >= contacts_num)
        return false;
    
    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);
    int ctx_len = \
        sizeof(vm_contacts_ctx_t);
    
    if(!nor_vm_file)
        return false;
    
    memset(&r_contacts, 0, ctx_len);
    flash_common_read_by_index(nor_vm_file, idx, 0, \
        ctx_len, (uint8_t *)&r_contacts);

    if(r_contacts.check_code != \
        Nor_Vm_Check_Code)
        return false;

    return true;
}
 
/*********************************************************************************
                              联系人存储                                   
*********************************************************************************/
void VmContactsCtxFlashSave(uint8_t idx, void *p)
{
    bool BondFlag = GetDevBondFlag();
    if(BondFlag == false)
        return;

    if(!p) return;

    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);
    int ctx_len = \
        sizeof(vm_contacts_ctx_t);

    if(!nor_vm_file)
        return;

    flash_common_write_file(nor_vm_file, 0, ctx_len, (u8 *)p);

    return;
}
