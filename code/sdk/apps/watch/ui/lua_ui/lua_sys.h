#ifndef __LUA_SYS_H__
#define __LUA_SYS_H__

#include "lua/lua.h"


// 回调函数结构体
typedef struct _LuaRegHandler_def {
    lua_State *lua;	/* lua状态机 */
    int func_index;	/* 回调函数索引 */
    int arg_number;	/* 传给回调函数的参数个数 */
} LuaRegHandler_def;


typedef struct _LuaHandlerTable_def {
    char *type;
    struct LuaRegHandler_def *LuaRegHandler;
} LuaHandlerTable_def;


LuaRegHandler_def *getLuaHandler(const char *type, int *type_len);
void callLuaHandler(LuaRegHandler_def *hdl);



int get_system_time(struct sys_time *time);

int set_system_time(struct sys_time *time);




// timer 句柄标志
#define BSP_TIMER_HANDLER		0x01

// timeout 句柄标志
#define BSP_TIMEOUT_HANDLER		0x02



// 操作联系人
#define SYS_OPERATE_CONTACTS	0
// 操作通话记录
#define SYS_OPERATE_CALLLOG		1


#define SYS_VM_READ_LIST_COUNT		0x01
#define SYS_VM_READ_LIST_INDEX		0x02
#define SYS_VM_READ_NAME_BY_NUMBER	0x03
#define SYS_VM_SAVE_LIST_BOOK		0x01


// 设置闹钟开关
#define SYS_ALARM_SET_ONOFF		0x01
// 设置闹钟时间
#define SYS_ALARM_SET_TIME		0x02
// 设置闹钟模式
#define SYS_ALARM_SET_MODE		0x04
// 设置闹钟贪睡
#define SYS_ALARM_SET_SNOOZE	0x08
// 暂时关闭闹钟
#define SYS_ALARM_SET_CLOSE		0x10





// 天气方法
// extern const luaL_Reg obj_WeatherMethod[];

#endif
