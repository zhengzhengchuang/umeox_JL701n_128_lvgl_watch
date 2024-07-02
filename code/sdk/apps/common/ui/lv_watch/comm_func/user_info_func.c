#include "../lv_watch.h"

#define VM_MASK (0x66aa)

UserInfoPara_t User_Info;

static const UserInfoPara_t Init = {
    .age = 25,
    .gender = Gender_Male,
    .height = 170,
    .weight = 60,
};

void UserInfoParaRead(void)
{
    int vm_op_len = \
        sizeof(UserInfoPara_t);

    int ret = syscfg_read(CFG_USER_PERSONAL_INFO, \
        &User_Info, vm_op_len);
    if(ret != vm_op_len || User_Info.vm_mask != VM_MASK)
        UserInfoParaReset();

    return;
}

void UserInfoParaWrite(void)
{
    int vm_op_len = \
        sizeof(UserInfoPara_t);
    
    for(u8 i = 0; i < 3; i++)
    {
        int ret = syscfg_write(CFG_USER_PERSONAL_INFO, \
            &User_Info, vm_op_len);
        if(ret == vm_op_len)
            break;
    }

    return;
}

void UserInfoParaReset(void)
{
    int vm_op_len = \
        sizeof(UserInfoPara_t);

    memcpy(&User_Info, &Init, vm_op_len);

    User_Info.vm_mask = VM_MASK;

    UserInfoParaUpdate();

    return;
}

void UserInfoParaUpdate(void)
{
    UserInfoParaWrite();

    return;
}
