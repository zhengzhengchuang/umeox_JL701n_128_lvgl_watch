#ifndef __COMMON_CMD_H__
#define __COMMON_CMD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

#define Cmd_Pkt_Len (16) //指令包长度(cmd + 有效包 + crc)

enum
{
    Cmd_Get_Device_Info =             0x01,//获取设备类型、型号、版本
    Cmd_Set_UtcTime =                 0x02,//设置时间
    Cmd_Set_UnitFormat =              0x03,//设置单位格式
    Cmd_Get_UnitFormat =              0x04,//获取单位格式
    Cmd_Get_Battery_Power =           0x05,//同步电量
    Cmd_Remote_Device_Op =            0x06,//远程设备操作
    Cmd_Set_Dnd_Mode =                0x07,//设置勿扰模式
    Cmd_Get_Dnd_Mode =                0x08,//获取勿扰模式
    Cmd_Set_Alarm_Info =              0x09,//设置闹钟
    Cmd_Get_Alarm_Info =              0x0A,//读取闹钟
    Cmd_Find_Dev =                    0x0F,//查找设备
    Cmd_Set_User_Info =               0x15,//设置用户信息
    Cmd_Get_History_Hr_Data =         0x17,//获取历史心率数据
    Cmd_Get_History_Bo_Data =         0x18,//获取历史血氧数据
    Cmd_Get_History_Pedo_Data =       0x19,//获取历史活动数据
    Cmd_Get_History_Sleep_Data =      0x1C,//获取历史睡眠数据
    Cmd_Get_Pedo_Data =               0x1D,//同步实时活动数据
    Cmd_Set_Spec_Func_Sw =            0x1E,//设置指定功能开关
    Cmd_Get_Spec_Func_Sw =            0x1F,//获取指定功能开关
    Cmd_Device_Events =               0x20,//设备上报事件
    Cmd_Modify_Galgo_Key =            0x21,//修改GoMore算法密钥
    Cmd_Modify_Mac_Address =          0x22,//修改蓝牙地址
    Cmd_Set_Health_Monitor_Para =     0x27,//设置健康监测参数
    Cmd_Get_Health_Monitor_Para =     0x28,//获取健康监测参数
    Cmd_Dev_Bond =                    0x29,//设备绑定
    Cmd_Dev_UnBond =                  0x2A,//设备解绑
    Cmd_Msg_Notify_Push =             0x2B,//消息推送（安卓）
    Cmd_Set_ANCS_Sw =                 0x2C,//设置ANCS开关
    Cmd_Get_ANCS_Sw =                 0x2D,//获取ANCS开关
    Cmd_Android_Phone =               0x2E,//安卓电话//未做
    Cmd_Set_LL_Info =                 0x2F,//设置经纬度信息
    Cmd_Set_Weather_Data =            0x30,//同步天气数据
    Cmd_Set_Sedentary_Info =          0x31,//设置久坐提醒
    Cmd_Get_Sedentary_Info =          0x32,//获取久坐提醒
    Cmd_Camera =                      0x33,//拍照
    Cmd_Ctrl_Remote_Music =           0x34,//控制手机音乐(安卓)
    Cmd_Sync_Remote_Music =           0x35,//同步手机音乐参数(安卓)
    Cmd_Set_PTime_Cfg_Para =          0x36,//设置祈祷时间运算参数
    Cmd_Update_Call_Contacts =        0x38,//更新通话通讯录
};
typedef uint8_t le_cmd_t;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
