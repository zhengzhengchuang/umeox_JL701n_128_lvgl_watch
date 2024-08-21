#include "../lv_watch.h"

static void printf_buf(u8 *buf, u8 len)
{
    printf("LE GET (%d):", len);
    put_buf(buf, len);

    return;
}

u8 calc_crc(u8 *buf, u8 crc_idx)
{
    u32 verify_crc = 0;
    for(u8 i = 0; i < crc_idx; i++)
        verify_crc += buf[i];

    u8 crc = \
        (u8)(verify_crc&(0xff));

    return crc;
}

//获取设备类型、型号、版本
void RemoteGetDeviceInfo(u8 *buf, u8 len)
{
    u8 nfy_buf[Cmd_Pkt_Len];
    memset(nfy_buf, 0x00, Cmd_Pkt_Len);

    const uint8_t *ble_mac = GetDevBleMac();
    static u8 dst_ble_mac[6];
    memset(dst_ble_mac, 0, 6);
    swapX(ble_mac, dst_ble_mac, 6);
 
    le_cmd_t cmd = buf[0];

    u8 idx = 0;
    nfy_buf[idx++] = cmd;
    nfy_buf[idx++] = Qwatch;
    nfy_buf[idx++] = Model_QW02;
    nfy_buf[idx++] = Version_Release_1;
    nfy_buf[idx++] = Version_Release_0;
    nfy_buf[idx++] = Version_Internal;
    nfy_buf[idx++] = dst_ble_mac[0];
    nfy_buf[idx++] = dst_ble_mac[1];
    nfy_buf[idx++] = dst_ble_mac[2];
    nfy_buf[idx++] = dst_ble_mac[3];
    nfy_buf[idx++] = dst_ble_mac[4];
    nfy_buf[idx++] = dst_ble_mac[5];


    u8 crc_idx = Cmd_Pkt_Len - 1;
    nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

    printf_buf(nfy_buf, Cmd_Pkt_Len);

    umeox_common_le_notify_data(nfy_buf, Cmd_Pkt_Len);

    return;
}


//获取单位格式
void RemoteGetUnitFormat(u8 *buf, u8 len)
{
    u8 nfy_buf[Cmd_Pkt_Len];
    memset(nfy_buf, 0x00, Cmd_Pkt_Len);

    le_cmd_t cmd = buf[0];
    u8 type = buf[1];

    int unit_para = 0;
    if(type == 0x00)
        unit_para = GetVmParaCacheByLabel(vm_label_time_format);
    else if(type == 0x01)
        unit_para = GetVmParaCacheByLabel(vm_label_unit_temperature);
    else if(type == 0x02)
        unit_para = GetVmParaCacheByLabel(vm_label_unit_distance);
        
    u8 idx = 0;
    nfy_buf[idx++] = cmd;
    nfy_buf[idx++] = type;
    nfy_buf[idx++] = (u8)unit_para;
    
    u8 crc_idx = Cmd_Pkt_Len - 1;
    nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

    printf_buf(nfy_buf, Cmd_Pkt_Len);

    umeox_common_le_notify_data(nfy_buf, Cmd_Pkt_Len);

    return;
}

//同步电量
void RemoteGetBatteryPower(u8 *buf, u8 len)
{
    u8 nfy_buf[Cmd_Pkt_Len];
    memset(nfy_buf, 0x00, Cmd_Pkt_Len);

    le_cmd_t cmd = buf[0];

    u8 power = GetBatteryPower();

    u8 idx = 0;
    nfy_buf[idx++] = cmd;
    nfy_buf[idx++] = power;

    u8 crc_idx = Cmd_Pkt_Len - 1;
    nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

    printf_buf(nfy_buf, Cmd_Pkt_Len);

    umeox_common_le_notify_data(nfy_buf, Cmd_Pkt_Len);

    return;
}

//获取勿扰模式
void RemoteGetDndMode(u8 *buf, u8 len)
{
    u8 nfy_buf[Cmd_Pkt_Len];
    memset(nfy_buf, 0x00, Cmd_Pkt_Len);

    le_cmd_t cmd = buf[0];

    bool enable = \
        Dnd_Info.enable;
    u8 start_hour = \
        Dnd_Info.dnd_start_hour;
    u8 start_minute = \
        Dnd_Info.dnd_start_minute;
    u8 end_hour = \
        Dnd_Info.dnd_end_hour;
    u8 end_minute = \
        Dnd_Info.dnd_end_minute;
    u8 repeat = \
        Dnd_Info.dnd_repeat;

    u8 idx = 0;
    nfy_buf[idx++] = cmd;
    nfy_buf[idx++] = enable;
    nfy_buf[idx++] = start_hour;
    nfy_buf[idx++] = start_minute;
    nfy_buf[idx++] = end_hour;
    nfy_buf[idx++] = end_minute;
    nfy_buf[idx++] = repeat;

    u8 crc_idx = Cmd_Pkt_Len - 1;
    nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

    printf_buf(nfy_buf, Cmd_Pkt_Len);

    umeox_common_le_notify_data(nfy_buf, Cmd_Pkt_Len);

    return;
}

//读取闹钟
void RemoteGetAlarmInfo(u8 *buf, u8 len)
{
    u8 nfy_buf[Cmd_Pkt_Len];

    le_cmd_t cmd = buf[0];

    u8 idx;
    u8 alarm_num = \
        Alarm_Info.alarm_num;
    if(alarm_num == 0)
    {
        memset(nfy_buf, 0x00, Cmd_Pkt_Len);

        idx = 0;
        nfy_buf[idx++] = cmd;
        nfy_buf[idx++] = alarm_num;   

        u8 crc_idx = Cmd_Pkt_Len - 1;
        nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

        printf_buf(nfy_buf, Cmd_Pkt_Len);

        umeox_common_le_notify_data(nfy_buf, Cmd_Pkt_Len);
    }else
    {
        u8 alarm_id;
        u8 alarm_enable;
        u8 alarm_hour;
        u8 alarm_minute;
        u8 alarm_repeat;
        alarm_union_t alarm_info;

        for(u8 i = 0; i < alarm_num; i++)
        {
            alarm_info.info = \
                GetUserAlarmInfo(i);
            alarm_id = \
                alarm_info.bit_field.alarm_id;
            alarm_enable = \
                alarm_info.bit_field.alarm_enable;
            alarm_hour = \
                alarm_info.bit_field.alarm_hour;
            alarm_minute = \
                alarm_info.bit_field.alarm_minute;
            alarm_repeat = \
                alarm_info.bit_field.alarm_repeat;

            memset(nfy_buf, 0x00, Cmd_Pkt_Len);

            idx = 0;
            nfy_buf[idx++] = cmd;
            nfy_buf[idx++] = alarm_num;
            nfy_buf[idx++] = alarm_id;
            nfy_buf[idx++] = alarm_enable;
            nfy_buf[idx++] = alarm_hour;
            nfy_buf[idx++] = alarm_minute;
            nfy_buf[idx++] = alarm_repeat;

            u8 crc_idx = Cmd_Pkt_Len - 1;
            nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

            printf_buf(nfy_buf, Cmd_Pkt_Len);

            umeox_common_le_notify_data(nfy_buf, Cmd_Pkt_Len);
        }
    }

    return;
}

//获取历史心率数据
void RemoteGetHistoryHrData(u8 *buf, u8 len)
{
    int le_msg[6];
    le_msg[0] = Le_History_Hr_Data;
    le_msg[1] = buf[0];
    le_msg[2] = buf[1] + 2000;
    le_msg[3] = buf[2];
    le_msg[4] = buf[3];
    post_le_task_msg(le_msg, 5);

    return;
}

//获取历史血氧数据
void RemoteGetHistoryBoData(u8 *buf, u8 len)
{
    int le_msg[6];
    le_msg[0] = Le_History_Bo_Data;
    le_msg[1] = buf[0];
    le_msg[2] = buf[1] + 2000;
    le_msg[3] = buf[2];
    le_msg[4] = buf[3];
    post_le_task_msg(le_msg, 5);

    return;
}


//获取历史活动数据
void RemoteGetHistoryPedoData(u8 *buf, u8 len)
{
    int le_msg[6];
    le_msg[0] = Le_History_Pedo_Data;
    le_msg[1] = buf[0];
    le_msg[2] = buf[1] + 2000;
    le_msg[3] = buf[2];
    le_msg[4] = buf[3];
    post_le_task_msg(le_msg, 5);

    return;
}

//获取历史睡眠数据
void RemoteGetHistorySleepData(u8 *buf, u8 len)
{
    int le_msg[6];
    le_msg[0] = Le_History_Sleep_Data;
    le_msg[1] = buf[0];
    le_msg[2] = buf[1] + 2000;
    le_msg[3] = buf[2];
    le_msg[4] = buf[3];
    post_le_task_msg(le_msg, 5);

    return;
}

//同步实时活动数据
void RemoteGetPedoData(u8 *buf, u8 len)
{
    u8 nfy_buf[Cmd_Pkt_Len];
    memset(nfy_buf, 0x00, Cmd_Pkt_Len);

    u32 steps = GetPedoDataSteps();
    u32 kcal = GetPedoDataKcal();
    u32 dis_m = GetPedoDataDisM();

    le_cmd_t cmd = buf[0];

    u8 idx = 0;
    nfy_buf[idx++] = cmd;
    nfy_buf[idx++] = (steps>>16)&(0xff);
    nfy_buf[idx++] = (steps>>8)&(0xff);
    nfy_buf[idx++] = (steps>>0)&(0xff);
    nfy_buf[idx++] = (kcal>>8)&(0xff);
    nfy_buf[idx++] = (kcal>>0)&(0xff);
    nfy_buf[idx++] = (dis_m>>16)&(0xff);
    nfy_buf[idx++] = (dis_m>>8)&(0xff);
    nfy_buf[idx++] = (dis_m>>0)&(0xff);

    u8 crc_idx = Cmd_Pkt_Len - 1;
    nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

    printf_buf(nfy_buf, Cmd_Pkt_Len);
    
    umeox_common_le_notify_data(nfy_buf, Cmd_Pkt_Len);
    
    return;
}

//获取指定功能开关
void RemoteGetSpecFuncSw(u8 *buf, u8 len)
{
    u8 nfy_buf[Cmd_Pkt_Len];
    memset(nfy_buf, 0x00, Cmd_Pkt_Len);

    le_cmd_t cmd = buf[0];
    u8 sw_type = buf[1];
    u8 sw_enable = 0;

    switch(sw_type)
    {
        case 0x00:/*低电提醒*/
            sw_enable = 0;
            break;

        case 0x01:/*自动呼吸*/
            sw_enable = 0;
            break;

        case 0x02:/*设置睡眠时间*/
            sw_enable = 0;
            break;

        case 0x03:/*高精度睡眠监测*/
            sw_enable = 0;
            break;

        case 0x04:/*自动心率*/
            sw_enable = \
                GetVmParaCacheByLabel(vm_label_auto_hr_sw);
            break;

        case 0x05:/*自动血氧*/
            sw_enable = \
                GetVmParaCacheByLabel(vm_label_auto_bo_sw);
            break;

        default:
            break;
    }

    u8 idx = 0;
    nfy_buf[idx++] = cmd;
    nfy_buf[idx++] = sw_type;
    nfy_buf[idx++] = sw_enable;

    u8 crc_idx = Cmd_Pkt_Len - 1;
    nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

    printf_buf(nfy_buf, Cmd_Pkt_Len);
    
    umeox_common_le_notify_data(nfy_buf, Cmd_Pkt_Len);

    return;
}

//设备主动上报事件
void RemoteGetDevEvents(le_event_t event)
{
    u8 nfy_buf[Cmd_Pkt_Len];
    memset(nfy_buf, 0x00, Cmd_Pkt_Len);

    if(event >= Le_Event_Num)
        return;

    u8 idx = 0;
    nfy_buf[idx++] = \
        Cmd_Device_Events;
    nfy_buf[idx++] = event;

    u8 crc_idx = Cmd_Pkt_Len - 1;
    nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

    printf_buf(nfy_buf, Cmd_Pkt_Len);
    
    umeox_common_le_notify_data(nfy_buf, Cmd_Pkt_Len);

    return;
}

//获取健康监测参数
void RemoteGetHealthMonitorPara(u8 *buf, u8 len)
{
    u8 nfy_buf[Cmd_Pkt_Len];
    memset(nfy_buf, 0x00, Cmd_Pkt_Len);

    le_cmd_t cmd = buf[0];
    u8 sw_type = buf[1];
    u8 para_type = buf[2];

    u8 sw_enable = 0;
    u8 para_val = 0;

    switch(sw_type)
    {
        case 0x01:/*全天测量开关*/
            sw_enable = 0;
            break;

        case 0x02:/*心率过高开关*/
            sw_enable = HrPara.hr_high_sw;
            break;

        case 0x03:/*心率过低开关*/
            sw_enable = HrPara.hr_low_sw;
            break;
        
        default:
            break;
    }

    switch(para_type)
    {
        case 0x01:/*全天测量间隔(单位：min)*/
            para_val = 0;
            break;
            
        case 0x02:/*心率过高提醒参数*/
            para_val = HrPara.hr_high_val;
            break;

        case 0x03:/*心率过低提醒参数*/
            para_val = HrPara.hr_low_val;
            break;
        
        default:
            break;
    }

    u8 idx = 0;
    nfy_buf[idx++] = cmd;
    nfy_buf[idx++] = sw_type;
    nfy_buf[idx++] = sw_enable;
    nfy_buf[idx++] = para_type;
    nfy_buf[idx++] = para_val;

    u8 crc_idx = Cmd_Pkt_Len - 1;
    nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

    printf_buf(nfy_buf, Cmd_Pkt_Len);
    
    umeox_common_le_notify_data(nfy_buf, Cmd_Pkt_Len);

    return;
}

//获取ANCS开关
void RemoteGetANCSSw(u8 *buf, u8 len)
{
    u8 nfy_buf[Cmd_Pkt_Len];
    memset(nfy_buf, 0x00, Cmd_Pkt_Len);

    le_cmd_t cmd = buf[0];

    ancs_sw_union_t tmp;
    tmp.info = Notify_Info.sw_union.info;

    u8 sw_0 = (tmp.info>>0)&(0xff);
    u8 sw_1 = (tmp.info>>8)&(0xff);

    u8 idx = 0;
    nfy_buf[idx++] = cmd;
    nfy_buf[idx++] = sw_0;
    nfy_buf[idx++] = sw_1;

    u8 crc_idx = Cmd_Pkt_Len - 1;
    nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

    printf_buf(nfy_buf, Cmd_Pkt_Len);
    
    umeox_common_le_notify_data(nfy_buf, Cmd_Pkt_Len);

    return;
}

//获取久坐提醒
void RemoteGetSedentaryInfo(u8 *buf, u8 len)
{
    u8 nfy_buf[Cmd_Pkt_Len];
    memset(nfy_buf, 0x00, Cmd_Pkt_Len);

    le_cmd_t cmd = buf[0];
  
    u8 idx = 0;
    nfy_buf[idx++] = cmd;
    nfy_buf[idx++] = Sed_Info.enable;
    nfy_buf[idx++] = Sed_Info.start_hour;
    nfy_buf[idx++] = Sed_Info.start_minute;
    nfy_buf[idx++] = Sed_Info.end_hour;
    nfy_buf[idx++] = Sed_Info.end_minute;
    nfy_buf[idx++] = Sed_Info.repeat;

    u8 crc_idx = Cmd_Pkt_Len - 1;
    nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

    printf_buf(nfy_buf, Cmd_Pkt_Len);
    
    umeox_common_le_notify_data(nfy_buf, Cmd_Pkt_Len);

    return;
}

//设备操作相机
void DevSendCameraOpCmd(u8 OpCmd)
{
    u8 nfy_buf[Cmd_Pkt_Len];
    memset(nfy_buf, 0x00, Cmd_Pkt_Len);

    le_cmd_t cmd = Cmd_Camera;

    u8 idx = 0;
    nfy_buf[idx++] = cmd;
    nfy_buf[idx++] = OpCmd;

    u8 crc_idx = Cmd_Pkt_Len - 1;
    nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

    printf_buf(nfy_buf, Cmd_Pkt_Len);
    
    umeox_common_le_notify_data(nfy_buf, Cmd_Pkt_Len);

    return;
}

//设备控制远程音乐
void DevSendRmusicOpCmd(u8 OpCmd, u8 *para)
{
    u8 nfy_buf[Cmd_Pkt_Len];
    memset(nfy_buf, 0x00, Cmd_Pkt_Len);

    le_cmd_t cmd = Cmd_Ctrl_Remote_Music;
    
    u8 idx = 0;
    nfy_buf[idx++] = cmd;
    nfy_buf[idx++] = OpCmd;
    if(para)
        nfy_buf[idx++] = *para;

    u8 crc_idx = Cmd_Pkt_Len - 1;
    nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

    printf_buf(nfy_buf, Cmd_Pkt_Len);
    
    umeox_common_le_notify_data(nfy_buf, Cmd_Pkt_Len);

    return;
}