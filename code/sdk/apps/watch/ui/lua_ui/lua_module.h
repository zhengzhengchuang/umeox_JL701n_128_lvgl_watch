#ifndef __LUA_MODULE_H__
#define __LUA_MODULE_H__


#include "lua/lua.h"
#include "generic/typedef.h"
#include "app_config.h"


// lua状态机，供其它文件使用
extern lua_State *L;

// touch消息接管标志
extern char lua_touch_event_takeover;

// lua数据类型没有整形，只有number字符形，补充定义int形数据
// 注意：不能与 lua.h 定义的数据类型冲突
#define LUA_TINTEGER	10



// gui模块
#define LUA_GUINAME		"gui"
LUALIB_API int (luaopen_ui)(lua_State *L);


// utils模块
#define LUA_UTILSNAME	"utils"
LUALIB_API int (luaopen_utils)(lua_State *L);


// bsp模块
#define LUA_BSPNAME		"bsp"
LUALIB_API int (luaopen_bsp)(lua_State *L);


// device模块
#define LUA_DEVICENAME	"device"
LUALIB_API int (luaopen_device)(lua_State *L);


// bluetooth模块
#define LUA_BTNAME		"bt"
LUALIB_API int (luaopen_bt)(lua_State *L);


// music模块
#define LUA_MUSICNAME	"music"
LUALIB_API int (luaopen_music)(lua_State *L);


// music模块
#define LUA_SYSNAME		"sys"
LUALIB_API int luaopen_sys(lua_State *L);



/*
** 保存整数数组到table中，该数组key为正数，用tab[x]使用；
** L 		  --> lua状态机
** array 	  --> 待保存数组
** array_size --> 待保存数组的大小
*/
static inline void set_integer_array_to_table(lua_State *L, const char *array, int array_size)
{
    for (int i = 1; i <= array_size; i++) {
        lua_pushinteger(L, array[i - 1]);
        lua_rawseti(L, -2, i);
        // lua_setfield(L, -2, i);
    }
}

/*
** 从table读取数组，该数组key为正数，用tab[x]使用；
** L 		  --> lua状态机
** array 	  --> 数组缓存
** array_size --> 待读取数组的大小
*/
static inline void get_integer_array_from_table(lua_State *L, char *array, int array_size)
{
    for (int i = 0; i < array_size; i++) {
        lua_pushinteger(L, i + 1);
        lua_gettable(L, -2);
        array[i] = lua_tointeger(L, -1);
        lua_pop(L, 1);
    }
}

// 保存一个整形值到table
static inline void set_value_to_table(lua_State *L, const char *k, int value)
{
    lua_pushstring(L, k);
    lua_pushinteger(L, value);
    lua_settable(L, -3);
}


// 保存一个table到table
static inline void set_table_to_table(lua_State *L, const char *new_table_k, int table_index)
{
    if (table_index != -1) {
        lua_pushvalue(L, table_index); //将table 所在index处压栈
    }
    lua_setfield(L, -2, new_table_k);
}




// 函数声明
void stackDump(lua_State *L);

unsigned int myrand(unsigned int min, unsigned int max);

int run_lua_string(int len, const char *string);

lua_State *lua_interperter_init(void);

lua_State *getLuaState(void);

int lua_interperter_uninit(lua_State *parm_L);




// 调试打印开关
#if 1
// #define lua_db(...) printf(__VA_ARGS__)
#define lua_db(fmt, ...) \
	printf("%s "fmt"", "[LUA DEBUG]: ", ##__VA_ARGS__)
#else
#define lua_db(...)
#endif



#endif //__LUA_MODULE_H__


