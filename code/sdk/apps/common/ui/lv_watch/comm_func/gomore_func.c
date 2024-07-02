#include "../lv_watch.h"

#define VM_MASK (0x55aa)

GMalgokeyInfo_t Galgo_info;

static const GMalgokeyInfo_t InitInfo = {
    .algo_key = {0},
};

void GalgoInfoParaRead(void)
{
    int vm_op_len = \
        sizeof(GMalgokeyInfo_t);

    int ret = syscfg_read(CFG_GM_ALGO_KEY_INFO, \
        &Galgo_info, vm_op_len);
    if(ret != vm_op_len || Galgo_info.vm_mask != VM_MASK)
        GalgoInfoParaReset();

    return;
}

void GalgoInfoParaWrite(void)
{
    int vm_op_len = \
        sizeof(GMalgokeyInfo_t);
    
    for(u8 i = 0; i < 3; i++)
    {
        int ret = syscfg_write(CFG_GM_ALGO_KEY_INFO, \
            &Galgo_info, vm_op_len);
        if(ret == vm_op_len)
            break;
    }

    return;
}

void GalgoInfoParaReset(void)
{
    int vm_op_len = \
        sizeof(GMalgokeyInfo_t);

    memcpy(&Galgo_info, &InitInfo, vm_op_len);

    Galgo_info.vm_mask = VM_MASK;

    GalgoInfoParaUpdate();
        
    return;
}

void GalgoInfoParaUpdate(void)
{
    GalgoInfoParaWrite();

    return;
}
