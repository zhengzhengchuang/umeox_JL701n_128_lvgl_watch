#include "../lv_watch.h"

#define VM_MASK (0x55af)

DevBondCodeInfo_t BondCode_Info;
u8 new_bond_code[DevBondCodeLen];

static const DevBondCodeInfo_t InitInfo = {
    .bc_valid = false,
    .bond_code = {0},
};

//清除数据
void DevBondClearData(void)
{
    ResetAllVmData();
    ResetAllNorVmData();

    return;
}

//首次绑定处理
void FirstDevBondHandle(void)
{
    printf("____first dev bond\n");

    DevBondClearData();

    bool *bc_valid = \
        &(BondCode_Info.bc_valid);
    u8 *bond_code = \
        BondCode_Info.bond_code;

    *bc_valid = true;
    memcpy(bond_code, new_bond_code, \
        DevBondCodeLen);
    BondCodeInfoParaUpdate();

    SetDevBondFlag(1);

    /* 充电中 */
    u8 charge_state = GetChargeState();
    if(charge_state == 1) return;

    ui_act_id_t act_id = ui_act_id_watchface;
    ui_menu_jump(act_id);
 
    return;
}

//原绑定处理
void OriDevBondHandle(void)
{
    printf("____old dev bond\n");

    return;
}

//新绑定处理
void NewDevBondHandle(void)
{
    printf("____New dev bond\n");

    DevBondClearData();

    bool *bc_valid = \
        &(BondCode_Info.bc_valid);
    u8 *bond_code = \
        BondCode_Info.bond_code;

    *bc_valid = true;
    memcpy(bond_code, new_bond_code, \
        DevBondCodeLen);
    BondCodeInfoParaUpdate();

    SetDevBondFlag(1);

    /* 充电中 */
    u8 charge_state = GetChargeState();
    if(charge_state == 1) return;

    ui_act_id_t act_id = ui_act_id_new_bond;
    ui_menu_jump(act_id);

    return;
}

//解绑处理
void DevUnBondHandle(void)
{
    printf("____dev unbond\n");

    DevBondClearData();

    bool *bc_valid = \
        &(BondCode_Info.bc_valid);
    u8 *bond_code = \
        BondCode_Info.bond_code;

    *bc_valid = false;
    memset(bond_code, 0x00, \
        DevBondCodeLen);
    BondCodeInfoParaUpdate();

    SetDevBondFlag(0);

    /* 充电中 */
    u8 charge_state = GetChargeState();
    if(charge_state == 1) return;

    ui_act_id_t act_id = ui_act_id_dev_bond;
    ui_menu_jump(act_id);

    //断开蓝牙
    //app主动断链，设备无需做操作
 
    return;
}

//相同返回true，不同返回false
bool BondCodeCompare(u8 *old_code, u8 *new_code, u8 len)
{
    if(old_code == NULL || new_code == NULL || \
        len == 0)
        return false;

    int ret = strncmp((const char *)old_code, (const char *)new_code, len);
    if(ret == 0) return true;

    return false;
}

void BondCodeInfoParaRead(void)
{
    int vm_op_len = \
        sizeof(DevBondCodeInfo_t);

    int ret = syscfg_read(CFG_DEV_BOND_CODE_INFO, \
        &BondCode_Info, vm_op_len);
    if(ret != vm_op_len || BondCode_Info.vm_mask != VM_MASK)
        BondCodeInfoParaReset();

    memset(new_bond_code, 0x00, DevBondCodeLen);

    return;
}

void BondCodeInfoParaWrite(void)
{
    int vm_op_len = \
        sizeof(DevBondCodeInfo_t);
    
    for(u8 i = 0; i < 3; i++)
    {
        int ret = syscfg_write(CFG_DEV_BOND_CODE_INFO, \
            &BondCode_Info, vm_op_len);
        if(ret == vm_op_len)
            break;
    }

    return;
}

void BondCodeInfoParaReset(void)
{
    int vm_op_len = \
        sizeof(DevBondCodeInfo_t);

    memcpy(&BondCode_Info, &InitInfo, vm_op_len);

    BondCode_Info.vm_mask = VM_MASK;

    BondCodeInfoParaUpdate();

    return;
}

void BondCodeInfoParaUpdate(void)
{
    BondCodeInfoParaWrite();

    return;
}
