#include "../lv_watch.h"
#include "../../../../../include_lib/btstack/avctp_user.h"

#define VM_MASK (0x55ac)

TwsInfoPara_t TwsPara;
TwsScanPara_t TwsScan;

static const TwsInfoPara_t Init = { 
    .num = 0, .Paired = {0},
};

static bool ScanComplete = false;
bool GetTwsScanComplete(void)
{
    return ScanComplete;
}

void SetTwsScanComplete(bool f)
{
    ScanComplete = f;
    return;
}

static bool ConnComplete = false;
bool GetTwsConnComplete(void)
{
    return ConnComplete;
}

void SetTwsConnComplete(bool f)
{
    ConnComplete = f;
    return;
}

extern u8 *get_cur_connect_emitter_mac_addr(void); 
static void TwsBtEmitterDisc(void)
{
    u8 *mac_addr = get_cur_connect_emitter_mac_addr();
    if(mac_addr != NULL)
        user_emitter_cmd_prepare(USER_CTRL_DISCONNECTION_HCI, 0, NULL);

    return;
}

void TwsBtEmitterStart(void)
{
    TwsBtEmitterDisc();

    memset(&TwsScan, 0, sizeof(TwsScanPara_t));

    SetTwsScanComplete(false);
    SetTwsConnComplete(false);

    bt_close_bredr_timeout_start();

    bt_emitter_start_search_device();

    return;
}

void TwsBtEmitterConnByMac(u8 *mac)
{
    emitter_bt_connect(mac);

    return;
}

void TwsBtScanResultParse(u8 *buf, u8 len)
{
    if(buf == NULL || len == 0)
        return;

    u8 scan_num = TwsScan.num;
    if(scan_num >= Tws_Scan_Num)
        return;

    memcpy(TwsScan.Para[scan_num].mac, &buf[4], 6);
    put_buf(TwsScan.Para[scan_num].mac, 6);

    u8 name_len = buf[11];
    if(name_len > Tws_Name_Len)
        name_len = Tws_Name_Len;
    if(name_len == 0) return;

    printf("name_len = %d\n", name_len);
    memcpy(TwsScan.Para[scan_num].name, &buf[12], name_len);

    TwsScan.num += 1;

    printf("num = %d\n", TwsScan.num);

    return;
}

void TwsInfoParaRead(void)
{
    int vm_op_len = \
        sizeof(TwsInfoPara_t);

    int ret = syscfg_read(CFG_TWS_PARA_INFO, \
        &TwsPara, vm_op_len);
    if(ret != vm_op_len || TwsPara.vm_mask != VM_MASK)
        TwsInfoParaReset();

    return;
}

void TwsInfoParaWrite(void)
{
    int vm_op_len = \
        sizeof(TwsInfoPara_t);
    
    for(u8 i = 0; i < 3; i++)
    {
        int ret = syscfg_write(CFG_TWS_PARA_INFO, \
            &TwsPara, vm_op_len);
        if(ret == vm_op_len)
            break;
    }

    return;
}

void TwsInfoParaReset(void)
{
    int vm_op_len = \
        sizeof(TwsInfoPara_t);

    memcpy(&TwsPara, &Init, vm_op_len);

    User_Info.vm_mask = VM_MASK;

    TwsInfoParaUpdate();

    return;
}

void TwsInfoParaUpdate(void)
{
    TwsInfoParaWrite();

    return;
}
