#include "../lv_watch.h"

#define VM_MASK (0x55ae)

ll_info_t ll_info;

static const ll_info_t init_info = {
    .vm_latitude = 22.57390f, \
    .vm_longitude = 113.94300f, 
    .position_valid = false,
};

void LLInfoParaRead(void)
{
    int vm_op_len = sizeof(ll_info_t);

    int ret = syscfg_read(CFG_LL_PARA_INFO, \
        &ll_info, vm_op_len);
    if(ret != vm_op_len || ll_info.vm_mask != VM_MASK)
        LLInfoParaReset();
    
    return;
}

void LLInfoParaWrite(void)
{
    int vm_op_len = sizeof(ll_info_t);
    
    for(u8 i = 0; i < 3; i++)
    {
        int ret = syscfg_write(CFG_LL_PARA_INFO, \
            &ll_info, vm_op_len);
        if(ret == vm_op_len)
            break;
    }

    return;
}

void LLInfoParaReset(void)
{
    int vm_op_len = sizeof(ll_info_t);

    memcpy(&ll_info, &init_info, vm_op_len);

    ll_info.vm_mask = VM_MASK;

    LLInfoParaUpdate();

    return;
}

void LLInfoParaUpdate(void)
{
    LLInfoParaWrite();

    return;
}