#include "../lv_watch.h"

static bool umeox_common_le_crc(u8 *buf, u8 len)
{ 
    u32 verify_crc = 0;
    u8 verify_crc_low8bit = 0;

    for(u8 i = 0; i < len - 1; i++)
        verify_crc += buf[i];

    verify_crc_low8bit = (u8)(verify_crc&(0xff));

    if(verify_crc_low8bit == buf[len - 1])
        return true;

    return false;
}

void umeox_common_le_cmd_parse(u8 *buf, u8 len)
{
    le_cmd_t cmd = buf[0];

    /* 充电中、升级中、crc检验均回复app失败指令 */
    u8 charge_state = GetChargeState();
    u8 upgrade_state = GetOtaUpgradeState();
    bool le_crc = umeox_common_le_crc(buf, len);
    if(le_crc == false || charge_state == 1 || \
        upgrade_state != upgrade_none)
    {
        umeox_common_le_reply_fail(buf, len);

        return;
    }

    switch(cmd)
    {
        case Cmd_Get_Device_Info:
            RemoteGetDeviceInfo(buf, len);
            break;

        case Cmd_Set_UtcTime:
            RemoteSetUtcTime(buf, len);
            break;

        case Cmd_Set_UnitFormat:
            RemoteSetUnitFormat(buf, len);
            break;

        case Cmd_Get_UnitFormat:
            RemoteGetUnitFormat(buf, len);
            break;

        case Cmd_Get_Battery_Power:
            RemoteGetBatteryPower(buf, len);
            break;

        case Cmd_Remote_Device_Op:
            RemoteDeviceOpHandle(buf, len);
            break;

        case Cmd_Set_Dnd_Mode:
            RemoteSetDndMode(buf, len);
            break;

        case Cmd_Get_Dnd_Mode:
            RemoteGetDndMode(buf, len);
            break;

        case Cmd_Set_Alarm_Info:
            RemoteSetAlarmInfo(buf, len);
            break;

        case Cmd_Get_Alarm_Info:
            RemoteGetAlarmInfo(buf, len);
            break;

        case Cmd_Find_Dev:
            RemoteFindDev(buf, len);
            break;

        case Cmd_Set_User_Info:
            RemoteSetUserInfo(buf, len);
            break;

        case Cmd_Get_History_Hr_Data:
            RemoteGetHistoryHrData(buf, len);
            break;

        case Cmd_Get_History_Bo_Data:
            RemoteGetHistoryBoData(buf, len);
            break;

        case Cmd_Get_History_Pedo_Data:
            RemoteGetHistoryPedoData(buf, len);
            break;

        case Cmd_Get_History_Sleep_Data:
            RemoteGetHistorySleepData(buf, len);
            break;

        case Cmd_Get_Pedo_Data:
            RemoteGetPedoData(buf, len);
            break;

        case Cmd_Set_Spec_Func_Sw:
            RemoteSetSpecFuncSw(buf, len);
            break;

        case Cmd_Get_Spec_Func_Sw:
            RemoteGetSpecFuncSw(buf, len);
            break;

        case Cmd_Modify_Galgo_Key:
            RemoteModifyGalgoKey(buf, len);
            break;

        case Cmd_Set_Health_Monitor_Para:
            RemoteSetHealthMonitorPara(buf, len);
            break;

        case Cmd_Get_Health_Monitor_Para:
            RemoteGetHealthMonitorPara(buf, len);
            break;
        
        case Cmd_Dev_Bond:
            RemoteDevBond(buf, len);
            break;

        case Cmd_Dev_UnBond:
            RemoteDevUnBond(buf, len);
            break;

        case Cmd_Msg_Notify_Push:
            RemoteMsgNotifyPush(buf, len);
            break;

        case Cmd_Set_ANCS_Sw:
            RemoteSetANCSSw(buf, len);
            break;

        case Cmd_Get_ANCS_Sw:
            RemoteGetANCSSw(buf, len);
            break;

        case Cmd_Android_Phone:
            break;

        case Cmd_Set_LL_Info:
            RemoteSetLLInfo(buf, len);
            break;

        case Cmd_Set_Weather_Data:
            RemoteSetWeatherData(buf, len);
            break;

        case Cmd_Set_Sedentary_Info:
            RemoteSetSedentaryInfo(buf, len);
            break;

        case Cmd_Get_Sedentary_Info:
            RemoteGetSedentaryInfo(buf, len);
            break;

        case Cmd_Camera:
            RemoteSetCameraOpCmd(buf, len);
            break;

        case Cmd_Sync_Remote_Music:
            RemoteSetRmusicPara(buf, len);
            break;

        case Cmd_Set_PTime_Cfg_Para:
            RemoteSetPTimeCfgPara(buf, len);
            break;

        case Cmd_Update_Call_Contacts:
            RemoteUpdateCallContacts(buf, len);
            break;
        default:
            break;
    }

    return;
}
