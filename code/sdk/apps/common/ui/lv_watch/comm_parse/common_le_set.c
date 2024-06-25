#include "../lv_watch.h"

static void printf_buf(u8 *buf, u8 len)
{
    printf("LE SET (%d):", len);
    put_buf(buf, len);

    return;
}

//设置时间
void RemoteSetUtcTime(u8 *buf, u8 len)
{
    u8 ctx_idx = 1;
    
    s8 pn = 1;
    int time_zone;
    u8 tz = buf[ctx_idx++];
    if(tz&(0x80)) pn = -1;
    time_zone = (tz&(0x7f))*pn;
    SetVmParaCacheByLabel(vm_label_time_zone, \
        time_zone);
    
    struct sys_time utc_time;
    utc_time.year = \
        buf[ctx_idx++] + 2000;
    utc_time.month = \
        buf[ctx_idx++];
    utc_time.day = \
        buf[ctx_idx++];
    utc_time.hour = \
        buf[ctx_idx++];
    utc_time.min = \
        buf[ctx_idx++];
    utc_time.sec = \
        buf[ctx_idx++];
    SetUtcTime(&utc_time);

    le_task_post(Le_Set_UtcTime, NULL, 0);
    
    umeox_common_le_notify_data(buf, len);

    return;
}

//设置单位格式
void RemoteSetUnitFormat(u8 *buf, u8 len)
{
    u8 type_idx = 1;
    u8 para_idx = 2;

    if(buf[type_idx] == 0x00)
    {
        int time_format = buf[para_idx];
        SetVmParaCacheByLabel(vm_label_time_format, \
            time_format);
    }else if(buf[type_idx] == 0x01)
    {
        int temper_unit = buf[para_idx];
        SetVmParaCacheByLabel(vm_label_unit_temperature, \
            temper_unit);
    }else if(buf[type_idx] == 0x02)
    {
        int distance_unit = buf[para_idx];
        SetVmParaCacheByLabel(vm_label_unit_distance, \
            distance_unit);
    }

    umeox_common_le_notify_data(buf, len);

    return;
}


//远程设备操作
void RemoteDeviceOpHandle(u8 *buf, u8 len)
{
    u8 op_type = buf[1];
    
    switch(op_type)
    {
        case 0x01:
            le_task_post(Le_Set_Shutdown, NULL, 0);
            break;

        case 0x02:
            le_task_post(Le_Set_Restart, NULL, 0);
            break;
        
        case 0x03:
            le_task_post(Le_Set_Reset, NULL, 0);
            break;

        default:
            break;
    }

    umeox_common_le_notify_data(buf, len);

    return;
}

//设置勿扰模式
void RemoteSetDndMode(u8 *buf, u8 len)
{
    bool *enable = \
        &(Dnd_Info.enable);
    u8 *start_hour = \
        &(Dnd_Info.dnd_start_hour);
    u8 *start_minute = \
        &(Dnd_Info.dnd_start_minute);
    u8 *end_hour = \
        &(Dnd_Info.dnd_end_hour);
    u8 *end_minute = \
        &(Dnd_Info.dnd_end_minute);
    u8 *repeat = \
        &(Dnd_Info.dnd_repeat);

    *enable = !!buf[1];
    *start_hour = buf[2];
    *start_minute = buf[3];
    *end_hour = buf[4];
    *end_minute = buf[5];
    *repeat = buf[6];

    le_task_post(Le_Dnd_Update, NULL, 0);

    umeox_common_le_notify_data(buf, len);

    return;
}

//设置闹钟
void RemoteSetAlarmInfo(u8 *buf, u8 len)
{
    u8 alarm_num = \
        Alarm_Info.alarm_num;

    u8 op_type = buf[1];
    u8 alarm_id = buf[2];
    u8 alarm_enable = !!buf[3];
    u8 alarm_hour = buf[4];
    u8 alarm_minute = buf[5];
    u8 alarm_repeat = buf[6];
    
    if(op_type == 0x01)
    {
        if(alarm_num >= Alarm_Max_Num)
            goto __reply_end;

        //增加
        alarm_union_t alarm_info;
        alarm_info.info = No_Alarm_Info;

        alarm_info.bit_field.alarm_id = \
            alarm_num;
        alarm_info.bit_field.alarm_enable = \
            alarm_enable;
        alarm_info.bit_field.alarm_hour = \
            alarm_hour;
        alarm_info.bit_field.alarm_minute = \
            alarm_minute;
        alarm_info.bit_field.alarm_repeat = \
            alarm_repeat&(0x7f);

        UserAlarmAdd(alarm_info.info);
    }else if(op_type == 0x02)
    {
        if(alarm_id >= alarm_num)
            goto __reply_end;

        //修改
        alarm_union_t alarm_info;
        alarm_info.info = No_Alarm_Info;

        alarm_info.bit_field.alarm_id = \
            alarm_id;
        alarm_info.bit_field.alarm_enable = \
            alarm_enable;
        alarm_info.bit_field.alarm_hour = \
            alarm_hour;
        alarm_info.bit_field.alarm_minute = \
            alarm_minute;
        alarm_info.bit_field.alarm_repeat = \
            alarm_repeat&(0x7f);

        UserAlarmAllModify(alarm_info.info);
    }else if(op_type == 0x03)
    {
        //删除某一个
        if(alarm_id >= alarm_num)
            goto __reply_end;

        UserAlarmDeleteOne(alarm_id);
    }else if(op_type == 0x04)
    {
        //全删
        UserAlarmDeleteAll();
    }

    SetAlarmRemoteUpdate(true);

__reply_end:
    umeox_common_le_notify_data(buf, len);
        
    return;
}

//查找设备
void RemoteFindDev(u8 *buf, u8 len)
{
    u8 enable = buf[1];

    int para = (int)enable;
    le_task_post(Le_Find_Dev, &para, 1);

    umeox_common_le_notify_data(buf, len);

    return;
}

//设置用户信息
void RemoteSetUserInfo(u8 *buf, u8 len)
{
    User_Info.age = buf[1];
    User_Info.gender = buf[2];
    User_Info.height = buf[3];
    User_Info.weight = buf[4];

    le_task_post(Le_User_Info_Update, NULL, 0);

    umeox_common_le_notify_data(buf, len);

    return;
}

//设置指定功能开关（手表只支持自动心率、自动血氧开关）
void RemoteSetSpecFuncSw(u8 *buf, u8 len)
{
    u8 sw_type = buf[1];
    u8 sw_enable = !!buf[2];

    switch(sw_type)
    {
        case 0x00:/*低电提醒*/
            break;

        case 0x01:/*自动呼吸*/
            break;

        case 0x02:/*设置睡眠时间*/
            break;

        case 0x03:/*高精度睡眠监测*/
            break;

        case 0x04:/*自动心率*/
            SetVmParaCacheByLabel(\
                vm_label_auto_hr_sw, sw_enable);
            break;

        case 0x05:/*自动血氧*/
            SetVmParaCacheByLabel(\
                vm_label_auto_bo_sw, sw_enable);
            break;

        default:
            break;
    }

    umeox_common_le_notify_data(buf, len);

    return;
}


//修改GoMore算法密钥
void RemoteModifyGalgoKey(u8 *buf, u8 len)
{  
    static u8 pkt_num;
    static u8 pkt_sum;
    static u8 ctx_offset;
    static u8 GalgoKeyCache[GalgoKeyLen];

    u8 pkt_id = buf[1];

    u8 *algo_key = \
        Galgo_info.algo_key;

    if(pkt_id == 0x00)
    {
        pkt_sum = 0;
        ctx_offset = 0;
        pkt_num = buf[2];

        memset(algo_key, 0, GalgoKeyLen);
        memset(GalgoKeyCache, 0, GalgoKeyLen);
    }else if(pkt_id == 0x01)
    {
        u8 ctx_len = 12;

        u8 len;
        if(ctx_offset + ctx_len > GalgoKeyLen)
            len = GalgoKeyLen - ctx_offset;
        else
            len = ctx_len;

        pkt_sum++;
        if(pkt_sum < pkt_num)
        {
            if(len > 0)
            {
                memcpy(&GalgoKeyCache[ctx_offset], &buf[3], len);
                ctx_offset += len;
            }
        }else if(pkt_sum == pkt_num)
        {
            if(len > 0)
                memcpy(&GalgoKeyCache[ctx_offset], &buf[3], len);

            memcpy(algo_key, GalgoKeyCache, GalgoKeyLen);
            le_task_post(Le_GalgoKey_Update, NULL, 0);

            goto __reply;
        }
    }

    goto __end;

__reply:
    u8 nfy_buf[Cmd_Pkt_Len];
    memset(nfy_buf, 0x00, Cmd_Pkt_Len);

    u8 idx = 0;
    nfy_buf[idx++] = buf[0];

    u8 crc_idx = Cmd_Pkt_Len - 1;
    nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

    printf_buf(nfy_buf, Cmd_Pkt_Len);
    
    umeox_common_le_notify_data(nfy_buf, Cmd_Pkt_Len);

__end:
    return;
}

//设置健康监测参数
void RemoteSetHealthMonitorPara(u8 *buf, u8 len)
{
    u8 sw_type = buf[1];
    u8 sw_enable = !!buf[2];

    u8 para_type = buf[3];
    u8 para_val = buf[4];

    switch(sw_type)
    {
        case 0x01:/*全天测量开关*/
            break;

        case 0x02:/*心率过高开关*/
            SetVmParaCacheByLabel(\
                vm_label_hr_high_remind_sw, sw_enable);
            break;

        case 0x03:/*心率过低开关*/
            SetVmParaCacheByLabel(\
                vm_label_hr_low_remind_sw, sw_enable);
            break;
        
        default:
            break;
    }

    switch(para_type)
    {
        case 0x01:/*全天测量间隔(单位：min)*/
            break;

        case 0x02:/*心率过高提醒参数*/
            SetVmParaCacheByLabel(\
                vm_label_hr_high_val, para_val);
            break;

        case 0x03:/*心率过低提醒参数*/
            SetVmParaCacheByLabel(\
                vm_label_hr_low_val, para_val);
            break;
        
        default:
            break;
    }

    umeox_common_le_notify_data(buf, len);

    return;
}

//设备绑定
void RemoteDevBond(u8 *buf, u8 len)
{
    static u8 pkt_num;
    static u8 pkt_sum;
    static u8 ctx_offset;

    u8 pkt_id = buf[1];
    bool *bc_valid = \
        &(BondCode_Info.bc_valid);
    u8 *bond_code = \
        BondCode_Info.bond_code;

    if(pkt_id == 0x00)
    {
        pkt_sum = 0;
        ctx_offset = 0;
        pkt_num = buf[2];

        memset(new_bond_code, 0x00, DevBondCodeLen);
    }else if(pkt_id == 0x01)
    {
        u8 ctx_len = 12;

        u8 len;
        if(ctx_offset + ctx_len > DevBondCodeLen)
            len = DevBondCodeLen - ctx_offset;
        else
            len = ctx_len;

        pkt_sum++;
        if(pkt_sum < pkt_num)
        {
            if(len > 0)
            {
                memcpy(&new_bond_code[ctx_offset], &buf[3], len);
                ctx_offset += len;
            }
        }else if(pkt_sum == pkt_num)
        {
            if(len > 0)
                memcpy(&new_bond_code[ctx_offset], &buf[3], len);

            u8 bond_type = 0x02;

            if(*bc_valid == false)
            {
                //首次绑定
                bond_type = 0x01;
                le_task_post(Le_First_Dev_Bond, NULL, 0);
            }else
            {
                bool com_ret = \
                    BondCodeCompare(bond_code, new_bond_code, DevBondCodeLen);
                if(com_ret == true)
                {
                    //原绑定
                    bond_type = 0x02;
                    le_task_post(Le_Ori_Dev_Bond, NULL, 0);
                }else
                {
                    bond_type = 0x01;
                    le_task_post(Le_New_Dev_Bond, NULL, 0);
                }
            }

            goto __reply;
        }  
    }

    goto __end;

__reply:
    u8 nfy_buf[Cmd_Pkt_Len];
    memset(nfy_buf, 0x00, Cmd_Pkt_Len);

    u8 idx = 0;
    nfy_buf[idx++] = buf[0];

    u8 crc_idx = Cmd_Pkt_Len - 1;
    nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

    printf_buf(nfy_buf, Cmd_Pkt_Len);
    
    umeox_common_le_notify_data(nfy_buf, Cmd_Pkt_Len);

__end:
    return;
}


//设备解绑
void RemoteDevUnBond(u8 *buf, u8 len)
{
    umeox_common_le_notify_data(buf, len);

    //解绑操作
    le_task_post(Le_Dev_UnBond, NULL, 0);

    return;
}

//消息推送（安卓）
void RemoteMsgNotifyPush(u8 *buf, u8 len)
{
    static u8 pkt_num;
    static u8 pkt_sum;
    static u8 ctx_offset;

    u8 pkt_id = buf[1];

    const u16 max_len = Msg_Ctx_Len;
    char *w_ctx = w_message.msg_ctx;
    u8 *type = &(w_message.message_type);
    u16 *check_code = &(w_message.check_code);
    struct sys_time *time = &(w_message.message_time);

    if(pkt_id == 0x00)
    {
        pkt_sum = 0;
        ctx_offset = 0;
        *type = buf[2];
        pkt_num = buf[3];

        memset(w_ctx, 0, max_len + 1);
    }else if(pkt_id == 0x01)
    {
        u8 ctx_len = 12;

        u8 len;
        if(ctx_offset + ctx_len > max_len)
            len = max_len - ctx_offset;
        else
            len = ctx_len;

        pkt_sum++;
        if(pkt_sum < pkt_num)
        {
            if(len > 0)
            {
                memcpy(&w_ctx[ctx_offset], &buf[3], len);
                ctx_offset += len;
            }
            
        }else if(pkt_sum == pkt_num)
        {
            if(len > 0)
                memcpy(&w_ctx[ctx_offset], &buf[3], len);

            *check_code = Nor_Vm_Check_Code;
            GetUtcTime(time);
            le_task_post(Le_Notify_Push, NULL, 0);

            goto __reply;
        }
    }

    goto __end;

__reply:
    u8 nfy_buf[Cmd_Pkt_Len];
    memset(nfy_buf, 0x00, Cmd_Pkt_Len);

    u8 idx = 0;
    nfy_buf[idx++] = buf[0];

    u8 crc_idx = Cmd_Pkt_Len - 1;
    nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

    printf_buf(nfy_buf, Cmd_Pkt_Len);
    
    umeox_common_le_notify_data(nfy_buf, Cmd_Pkt_Len);

__end:
    return;
}


//设置ANCS开关
void RemoteSetANCSSw(u8 *buf, u8 len)
{
    u8 sw_0 = buf[1];
    u8 sw_1 = buf[2];

    printf("sw_0 = %d, sw_1 = %d\n", sw_0, sw_1);

    ancs_sw_union_t *sw_union = 
        &(Notify_Info.sw_union);

    sw_union->bit_field.other = \
        (sw_0>>0)&(0x1);
    sw_union->bit_field.facebook = \
        (sw_0>>1)&(0x1);
    sw_union->bit_field.vkontakte = \
        (sw_0>>2)&(0x1);
    sw_union->bit_field.instagram = \
        (sw_0>>3)&(0x1);
    sw_union->bit_field.linkedin = \
        (sw_0>>4)&(0x1);
    sw_union->bit_field.tiktok = \
        (sw_0>>5)&(0x1);
    sw_union->bit_field.whatsapp = \
        (sw_0>>6)&(0x1);
    sw_union->bit_field.wechat = \
        (sw_0>>7)&(0x1);

    sw_union->bit_field.snapchat = \
        (sw_1>>0)&(0x1);
    sw_union->bit_field.telegram = \
        (sw_1>>1)&(0x1);
    sw_union->bit_field.twitter = \
        (sw_1>>2)&(0x1);
    sw_union->bit_field.skype = \
        (sw_1>>3)&(0x1);
    sw_union->bit_field.line = \
        (sw_1>>4)&(0x1);
    sw_union->bit_field.gmail = \
        (sw_1>>5)&(0x1);
    sw_union->bit_field.sms = \
        (sw_1>>6)&(0x1);
    sw_union->bit_field.phone = \
        (sw_1>>7)&(0x1);

    le_task_post(Le_ANCSSw_Update, NULL, 0);

    umeox_common_le_notify_data(buf, len);

    return;
}

//设置经纬度信息
void RemoteSetLLInfo(u8 *buf, u8 len)
{
    u8 tmp0 = buf[1];
    u8 tmp1 = buf[2];
    u8 tmp2 = buf[3];
    u8 tmp3 = buf[4];
    s32 longitude = \
       (tmp0<<24)|(tmp1<<16)|(tmp2<<8)|(tmp3<<0);

    tmp0 = buf[5];
    tmp1 = buf[6];
    tmp2 = buf[7];
    tmp3 = buf[8];
    s32 latitude = \
       (tmp0<<24)|(tmp1<<16)|(tmp2<<8)|(tmp3<<0);

    printf("longitude = %d, latitude = %d\n", \
        longitude, latitude);

    ll_info.vm_longitude = \
        (float)((longitude*(1.0f))/(1000000));
    ll_info.vm_latitude = \
        (float)((latitude*(1.0f))/(1000000));
    ll_info.position_valid = true;

    le_task_post(Le_LL_Info_Update, NULL, 0);

    umeox_common_le_notify_data(buf, len);

    return;
}

//同步天气数据
void RemoteSetWeatherData(u8 *buf, u8 len)
{
    static u8 pkt_sum;

    u8 pkt_id = buf[1];

    u8 type = buf[2];
    int8_t real_temper = buf[3];
    int8_t min_temper = buf[4];
    int8_t max_temper = buf[5];

    uint16_t *check_code = \
        &(w_weather.check_code);
    struct sys_time *time = \
        &(w_weather.time);
    vm_weather_data_ctx_t *w_ctx = \
        w_weather.vm_ctx;

    if(pkt_id == 0x00)
        pkt_sum = 0;

    if(pkt_sum > \
        Weather_Sync_Days - 1)
        return;

    w_ctx[pkt_sum].type = type;
    w_ctx[pkt_sum].real_temper = real_temper;
    w_ctx[pkt_sum].min_temper = min_temper;
    w_ctx[pkt_sum].max_temper = max_temper;
    pkt_sum++;

    if(pkt_sum == Weather_Sync_Days)
    {
        *check_code = \
            Nor_Vm_Check_Code;
        GetUtcTime(time);
        le_task_post(Le_Weather_Update, NULL, 0);
   
        u8 nfy_buf[Cmd_Pkt_Len];
        memset(nfy_buf, 0x00, Cmd_Pkt_Len);

        u8 idx = 0;
        nfy_buf[idx++] = buf[0];

        u8 crc_idx = Cmd_Pkt_Len - 1;
        nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

        printf_buf(nfy_buf, Cmd_Pkt_Len);
        
        umeox_common_le_notify_data(nfy_buf, Cmd_Pkt_Len);
    }

    return;
}

//设置久坐提醒
void RemoteSetSedentaryInfo(u8 *buf, u8 len)
{
    bool enable = !!buf[1];
    u8 start_hour = buf[2];
    u8 start_minute = buf[3];
    u8 end_hour = buf[4];
    u8 end_minute = buf[5];
    u8 repeat = buf[6];

    Sed_Info.enable = enable;
    Sed_Info.start_hour = start_hour;
    Sed_Info.start_minute = start_minute;
    Sed_Info.end_hour = end_hour;
    Sed_Info.end_minute = end_minute;
    Sed_Info.repeat = repeat;
    
    le_task_post(Le_Sed_Info_Update, NULL, 0);

    umeox_common_le_notify_data(buf, len);

    return;
}


//远程操作相机
void RemoteSetCameraOpCmd(u8 *buf, u8 len)
{
    u8 OpCmd = buf[1];

    switch(OpCmd)
    {
        case RemoteReqEnterCamera:
            le_task_post(Le_Remote_Enter_Camera, NULL, 0);
            break;

        case RemoteReqExitCamera:
            le_task_post(Le_Remote_Exit_Camera, NULL, 0);
            break;
        
        default:
            break;
    }

    return;
}


//远程同步音乐参数
void RemoteSetRmusicPara(u8 *buf, u8 len)
{
    static u8 pkt_sum;
    static u8 pkt_num;
    static u8 ctx_offset;

    u8 pkt_id = buf[1];

    static char title[Rmusic_Title_Len];

    if(pkt_id == 0x00)
    {
        pkt_sum = 0;
        ctx_offset = 0;
        pkt_num = buf[4];
        u8 music_state = buf[2];
        u8 music_volume = buf[3];
        Rmusic_Info.state = music_state;
        Rmusic_Info.volume = music_volume;

        if(pkt_num == 0)
            goto __reply;
        else
            memset(title, 0, Rmusic_Title_Len);     
    }else if(pkt_id == 0x01)
    {
        u8 ctx_len = 12;

        u8 len;
        if(ctx_offset + ctx_len > Rmusic_Title_Len)
            len = Rmusic_Title_Len - ctx_offset;
        else
            len = ctx_len;

        pkt_sum++;
        if(pkt_sum < pkt_num)
        {
            if(len > 0)
            {
                memcpy(&title[ctx_offset], &buf[3], len);
                ctx_offset += len;
            }
        }else if(pkt_sum == pkt_num)
        {
            if(len > 0)
                memcpy(&title[ctx_offset], &buf[3], len);

            memcpy(Rmusic_Info.title, title, Rmusic_Title_Len);

            goto __reply;
        }
    }

    goto __end;

__reply:
    u8 nfy_buf[Cmd_Pkt_Len];
    memset(nfy_buf, 0x00, Cmd_Pkt_Len);

    u8 idx = 0;
    nfy_buf[idx++] = buf[0];

    u8 crc_idx = Cmd_Pkt_Len - 1;
    nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

    printf_buf(nfy_buf, Cmd_Pkt_Len);
    
    umeox_common_le_notify_data(nfy_buf, Cmd_Pkt_Len);

__end:
    return;
}

//设置祈祷时间运算参数
void RemoteSetPTimeCfgPara(u8 *buf, u8 len)
{
    u8 conv = buf[1];
    PT_Cfg.conv = conv;

    if(conv == Conv_Common)
    {
        u8 fajr_angle = buf[2];
        u16 isha_value = (buf[3]<<8)|(buf[4]);
        u8 maghrib_value = buf[5];
        u8 isha_is_minutes = buf[6];
        u8 maghrib_is_minutes = buf[7];
        JuriMethod_t juri = buf[8];

        PT_Cfg.fajr_angle = fajr_angle*(1.0f)/(10.0f);
        PT_Cfg.isha_value = isha_value*(1.0f)/(10.0f);
        PT_Cfg.maghrib_value = maghrib_value*(1.0f)/(10.0f);
        PT_Cfg.isha_is_minutes = !!isha_is_minutes;
        PT_Cfg.maghrib_is_minutes = !!maghrib_is_minutes;
        PT_Cfg.juri = juri;
    }

    le_task_post(Le_PTime_Cfg_Para_Update, NULL, 0);
  
    umeox_common_le_notify_data(buf, len);

    return;
}

//更新通话通讯录
void RemoteUpdateCallContacts(u8 *buf, u8 len)
{
    static u8 ctx_idx;
    static u8 ctx_num;
    static u8 name_pkt_num;
    static u8 name_pkt_sum;
    static u8 name_ctx_offset;
    static u8 number_pkt_num;
    static u8 number_pkt_sum;
    static u8 number_ctx_offset;
    
    static char *name;
    static char *number;

    u8 pkt_id = buf[1];

    if(pkt_id == 0x00)
    {   
        ctx_num = buf[2];
        ctx_idx = buf[3];

        if(ctx_num == 0)
        {
            le_task_post(Le_Call_Contacts_Clear, NULL, 0);
            goto __reply;
        }
            
        name_pkt_sum = 0;
        name_ctx_offset = 0;
        name_pkt_num = buf[4];

        number_pkt_sum = 0;
        number_ctx_offset = 0;
        number_pkt_num = buf[5];

        name = GetContactsName(ctx_idx);
        memset(name, 0, Call_Name_Max_Len);

        number = GetContactsNumber(ctx_idx);   
        memset(number, 0, Call_Number_Max_Len);
    }else if(pkt_id == 0x01)
    {
        u8 ctx_len = 12;

        if(buf[2] == 0x01)
        {
            name_pkt_sum++;

            u8 len;
            if(name_ctx_offset + ctx_len > Call_Name_Max_Len)
                len = Call_Name_Max_Len - name_ctx_offset;
            else
                len = ctx_len;

            if(name_pkt_sum < name_pkt_num)
            {
                if(len > 0)
                {
                    memcpy(&name[name_ctx_offset], &buf[3], len);
                    name_ctx_offset += len;
                }
            }else
            {
                if(len > 0)
                    memcpy(&name[name_ctx_offset], &buf[3], len);
            }
        }else if(buf[2] == 0x02)
        {
            number_pkt_sum++;

            u8 len;
            if(number_ctx_offset + ctx_len > Call_Number_Max_Len)
                len = Call_Number_Max_Len - number_ctx_offset;
            else
                len = ctx_len;

            if(number_pkt_sum < number_pkt_num)
            {
                if(len > 0)
                {
                    memcpy(&number[number_ctx_offset], &buf[3], len);
                    number_ctx_offset += len;
                }
                
            }else
            {
                if(len > 0)
                    memcpy(&number[number_ctx_offset], &buf[3], len);

                if(ctx_idx >= ctx_num - 1)
                {
                    le_task_post(Le_Call_Contacts_Update, NULL, 0);
                    goto __reply;
                }
                    
            }
        }
    }

    goto __end;

__reply:
    u8 nfy_buf[Cmd_Pkt_Len];
    memset(nfy_buf, 0x00, Cmd_Pkt_Len);

    u8 idx = 0;
    nfy_buf[idx++] = buf[0];

    u8 crc_idx = Cmd_Pkt_Len - 1;
    nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

    printf_buf(nfy_buf, Cmd_Pkt_Len);
    
    umeox_common_le_notify_data(nfy_buf, Cmd_Pkt_Len);

__end:
    return;
}