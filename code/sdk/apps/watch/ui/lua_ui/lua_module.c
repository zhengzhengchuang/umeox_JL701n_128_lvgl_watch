#include "system/includes.h"

#include "lua/lauxlib.h"
#include "lua/lualib.h"

#include "lua_module.h"
#include "lua_gui.h"
#include "lua_utils.h"

#if (TCFG_LUA_ENABLE)

/*
** lua状态机
*/
lua_State *L = NULL;

/*
 * 接管触摸消息标志, 高四位默认设置，低四位主动设置
 * */
char lua_touch_event_takeover = 0x00;


static int touch_event_take_over()
{
    if (lua_touch_event_takeover & 0x02) {
        // 有设置，返回设置值
        if (lua_touch_event_takeover & 0x01) {
            return true;
        } else {
            return false;
        }
    } else {
        // 没有设置，返回默认
        return lua_touch_event_takeover;
    }
}


/*
** 以下注册的lib由init加载，可供任何lua程序使用
*/
const luaL_Reg loadedlibs[] = {
    // 内部基本库，修改会影响使用
    {"_G", luaopen_base},
    {LUA_STRLIBNAME, luaopen_string},

    // 添加用户自定义库
    {LUA_GUINAME,		luaopen_ui    },	// ui
    {LUA_UTILSNAME,		luaopen_utils },	// utils
    {LUA_BSPNAME,		luaopen_bsp   },	// bsp
    {LUA_DEVICENAME,	luaopen_device},	// device
    {LUA_BTNAME,		luaopen_bt    },	// bt
    {LUA_MUSICNAME,		luaopen_music },	// music
    {LUA_SYSNAME,		luaopen_sys   },	// sys

    // 必须以NULL结尾
    {NULL, NULL}
};


/*
** 打印栈信息
*/
void stackDump(lua_State *L)
{
    int top = lua_gettop(L);

    printf("\n\n--------stack top (%d)--------:\n", top);
    for (int i = top; i > 0; --i) {
        int t = lua_type(L, i);
        printf("index:%d --> '%s'\t( %s )\n", i, lua_typename(L, t), lua_tostring(L, i));
#if 0
        if (t == LUA_TTABLE) {
            lua_pushnil(L);
            while (lua_next(L, -2)) {
                stackDump(L);
                lua_pop(L, 1);
            }
        }
#endif
    }
    mem_stats();
    printf("--------stack bottom---------\n\n\n");
}


/*
** 生成随机数
*/
unsigned int myrand(unsigned int min, unsigned int max)
{
    unsigned int rand = (JL_RAND->R64H << 32) | JL_RAND->R64L;
    return ((rand % (max - min)) + min);
}


/*
** lua执行字符串指令
*/
int run_lua_string(int len, const char *string)
{
    int error = 0;

    lua_touch_event_takeover &= 0xf0; /* 默认设置是不接管 */
    if (!L) {
        /* 状态机未初始化 */
        printf("error! Lua state not init\n");
        return -1;
    }
    lua_db(">>> lua code:\n\n%s\n\n", string);

    /* 执行字符串代码 */
    error = luaL_dostring(L, string);
    lua_gc(L, LUA_GCCOLLECT, 0);
    lua_db("run lua code error = %d\n", error);
    if (error) {
        stackDump(L);
    }
    ASSERT(!error);

    lua_db("lua touch event takeover: 0x%x, %d\n", lua_touch_event_takeover, touch_event_take_over());
    return touch_event_take_over();
}


/*
** lua解释器初始化
*/
lua_State *lua_interperter_init(void)
{
    int error = 0;

    /* 打印提示信息 */
    lua_db("\n\n>>> lua interperter init <<< \n\n");
    mem_stats();

    /* 创建lua状态机 */
    L = luaL_newstate();
    if (L == NULL) {
        printf(">>> lua init failed\n");
        return NULL;
    }

    /* 解释器版本检查，并打印版本号 */
    luaL_checkversion(L);
    lua_writestring(LUA_COPYRIGHT, strlen(LUA_COPYRIGHT));
    lua_writeline();

    /* 强制设定堆栈大小 */
    /* lua_checkstack(L, 1000); */

    /* 注册库模块 */
    luaL_openlibs(L);

    mem_stats();
    lua_db("run lua main = %d\n", error);

    return L;
}


/*
** 获取lua状态机
*/
lua_State *getLuaState(void)
{
    return L;
}



/*
** 手动关闭lua解释器
*/
int lua_interperter_uninit(lua_State *parm_L)
{
    lua_close(parm_L); // close lua environment
    if (parm_L == L) {
        L = NULL;
    }

    return 0;
}


/*
** 把lua解释器初始化注册到系统初始化
*/
void lua_late_init(void)
{
    lua_State *L0 = lua_interperter_init();
}
/* late_initcall(lua_late_init); */



#endif /* #if (TCFG_LUA_ENABLE) */

