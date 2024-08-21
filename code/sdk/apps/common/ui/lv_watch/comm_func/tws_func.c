#include "../lv_watch.h"
#include "../../../../../include_lib/btstack/avctp_user.h"

#define VM_MASK (0x55bb)

TwsInfoPara_t TwsPara;
TwsScanPara_t TwsScan;
TwsConnPara_t TwsConn;

static const TwsInfoPara_t Init = { 
    .num = 0, .Paired = {0},
};

extern void delete_link_key(bd_addr_t *bd_addr, u8 id);

static u8 unpair_dev = TwsNone;
u8 get_unpair_dev(void)
{
    return unpair_dev;
}

void set_unpair_dev(u8 dev)
{
    unpair_dev = dev;
    return;
}

static bool NewDevFlag;
bool GetNewPairedDevFlag(void)
{
    return NewDevFlag;
}

void SetNewPairedDevFlag(bool f)
{
    NewDevFlag = f;
    return;
}

static bool MacAddrIsSame(u8 *mac0, u8 *mac1)
{
    put_buf(mac0, 6);
    put_buf(mac1, 6);

    bool IsSame = false;

    if(mac0 == NULL || mac1 == NULL)
        goto __end;

    u8 i;
    for(i = 0; i < 6; i++)
    {
        if(mac0[i] != mac1[i])
            break;
    }  

    if(i >= 6)
      IsSame = true;  

__end:
    return IsSame;
}

extern u8 *get_cur_connect_emitter_mac_addr(void); 
static void TwsBtEmitterDisc(void)
{
    u8 *mac_addr = get_cur_connect_emitter_mac_addr();
    if(mac_addr != NULL)
        user_emitter_cmd_prepare(USER_CTRL_DISCONNECTION_HCI, 0, NULL);

    return;
}

void TwsBtUserDiscHandle(void)
{
    TwsBtEmitterDisc();

    /*初始化连接设备*/
    TwsConn.dev = TwsNone;
    TwsConn.state = TWS_DISCONNECT;
    memset(&TwsConn.Conn, 0, sizeof(TwsDsc_t));

    return;
}

void TwsBtUserCloseHandle(void)
{
    /*初始化连接设备*/
    TwsConn.dev = TwsNone;
    TwsConn.state = TWS_DISCONNECT;
    memset(&TwsConn.Conn, 0, sizeof(TwsDsc_t));
    
    return;
}

void TwsBtEmitterStart(void)
{
    TwsBtEmitterDisc();

    /*初始化扫描设备*/
    TwsScan.num = 0;
    TwsScan.state = TWS_SCANNING;
    memset(TwsScan.Para, 0, sizeof(TwsDsc_t)*Tws_Scan_Num);

    /*初始化连接设备*/
    TwsConn.dev = TwsNone;
    TwsConn.state = TWS_DISCONNECT;
    memset(&TwsConn.Conn, 0, sizeof(TwsDsc_t));

    bt_close_bredr_timeout_start();

    bt_emitter_start_search_device();

    return;
}

void TwsBtEmitterConnByMac(u8 *mac)
{
    if(mac == NULL) return;

    TwsBtEmitterDisc();

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

void TwsConnCompleteHandle(u8 *mac)
{
    if(mac == NULL) return;

    put_buf(mac, 6);

    uint8_t *num = &TwsPara.num;

    /*先查找是否配对过*/
    for(uint8_t i = 0; i < *num; i++)
    {  
        if(MacAddrIsSame(TwsPara.Paired[i].mac, mac) == true)
        {
            printf("___Paired Dev__\n");
            TwsConn.dev = i;
            SetNewPairedDevFlag(false);
            goto __end;
        }
    }

    /*配对设备已满*/
    if(*num >= Tws_Paired_Num)
    {
        /*配对设备满，去掉最旧的那个*/
        for(uint8_t i = 0; i < Tws_Paired_Num - 1; i++)
        {
            memcpy(TwsPara.Paired[i].mac, TwsPara.Paired[i+1].mac, 6);
            memcpy(TwsPara.Paired[i].name, TwsPara.Paired[i+1].name, Tws_Name_Len + 1);
        }

        TwsDsc_t *conn = &TwsConn.Conn;
        uint8_t idx = Tws_Paired_Num - 1;
        memset(TwsPara.Paired[idx].mac, 0, 6);
        memset(TwsPara.Paired[idx].name, 0, Tws_Name_Len + 1);
        memcpy(TwsPara.Paired[idx].mac, conn->mac, 6);
        memcpy(TwsPara.Paired[idx].name, conn->name, Tws_Name_Len + 1);

        TwsConn.dev = idx;
        SetNewPairedDevFlag(true);
        TwsInfoParaUpdate();
        goto __end;
    }

    /*添加新的配对设备*/
    uint8_t idx = *num;
    TwsDsc_t *conn = &TwsConn.Conn;
    memset(TwsPara.Paired[idx].mac, 0, 6);
    memset(TwsPara.Paired[idx].name, 0, Tws_Name_Len + 1);
    memcpy(TwsPara.Paired[idx].mac, conn->mac, 6);
    memcpy(TwsPara.Paired[idx].name, conn->name, Tws_Name_Len + 1);
    TwsConn.dev = idx;
    *num += 1;
    SetNewPairedDevFlag(true);
    TwsInfoParaUpdate();

    printf("__%s__:idx = %d\n", __func__, idx);
    printf("__%s__:num = %d\n", __func__, *num);
    put_buf(TwsPara.Paired[idx].mac, 6);

__end:
    /*初始化扫描设备*/
    TwsScan.num = 0;
    TwsScan.state = TWS_SCAN_STOP;
    memset(TwsScan.Para, 0, sizeof(TwsDsc_t)*Tws_Scan_Num);

    TwsConn.state = TWS_CONNECTED;

    return;
}

void TwsDisConnCompleteHandle(u8 *mac)
{
    printf("%s\n", __func__);

    if(MacAddrIsSame(TwsConn.Conn.mac, mac))
    {
        TwsConn.dev = TwsNone;
        TwsConn.state = TWS_DISCONNECT;
        memset(&TwsConn.Conn, 0, sizeof(TwsDsc_t));
    }

    return;
}

void TwsBtUserUnpair(u8 dev)
{
    if(dev == TwsNone)
        return;

    printf("%s\n", __func__);

    TwsBtEmitterDisc();

    u8 *mac = TwsPara.Paired[dev].mac;
    put_buf(mac, 6);
    delete_link_key((bd_addr_t *)mac, 1);

    u8 num = TwsPara.num;

    for(u8 i = dev; i < num - 1; i++)
    {
        memcpy(TwsPara.Paired[i].mac, TwsPara.Paired[i+1].mac, 6);
        memcpy(TwsPara.Paired[i].name, TwsPara.Paired[i+1].name, Tws_Name_Len + 1);
    }

    TwsPara.num -= 1;

    TwsInfoParaUpdate();
    
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

    /*初始化扫描设备*/
    TwsScan.num = 0;
    TwsScan.state = TWS_SCAN_STOP;
    memset(TwsScan.Para, 0, sizeof(TwsDsc_t)*Tws_Scan_Num);

    /*初始化连接设备*/
    TwsConn.dev = TwsNone;
    TwsConn.state = TWS_DISCONNECT;
    memset(&TwsConn.Conn, 0, sizeof(TwsDsc_t));

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

    TwsPara.vm_mask = VM_MASK;

    TwsInfoParaUpdate();

    return;
}

void TwsInfoParaUpdate(void)
{
    TwsInfoParaWrite();

    return;
}
