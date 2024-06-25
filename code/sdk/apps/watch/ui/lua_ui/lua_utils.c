#include "system/includes.h"
#include "app_config.h"

#include "lua/lauxlib.h"
#include "lua/lualib.h"

#include "lua_module.h"
#include "lua_utils.h"



#if (TCFG_LUA_ENABLE)



static int utils_print(lua_State *L)
{
    int num = 0;
    const char *s = NULL;
    int n = lua_gettop(L);// 获取参数个数
    int type = lua_type(L, -1);
    lua_db(">>>>> type:%d\n", type);

    switch (type) {
    case LUA_TNONE:
    case LUA_TNIL:
        printf("[lua utils](nil): nil(none)");
        break;
    case LUA_TBOOLEAN:
        num = lua_toboolean(L, 2);
        printf("[lua utils](boolean): %s", (num ? "true" : "false"));
        break;
    case LUA_TLIGHTUSERDATA:
        printf("[lua utils](lightuserdata): ");
        break;
    case LUA_TNUMBER:
        num = luaL_checkinteger(L, 2);
        printf("[lua utils](number): %d", num);
        break;
    case LUA_TSTRING:
        s = luaL_checkstring(L, 2);
        printf("[lua utils](string): %s", s);
        break;
    case LUA_TTABLE:
        lua_pushnil(L);
        printf("[lua utils](table %d) {\n", n);
        while (lua_next(L, -2) != 0) {
            //value
            switch (lua_type(L, -2)) {
            case LUA_TNUMBER:
                printf("%d = ", lua_tonumber(L, -2));
                break;
            case LUA_TSTRING:
                printf("%s = ", lua_tostring(L, -2));
                break;
            }
            //key
            switch (lua_type(L, -1)) {
            case LUA_TTABLE:
                utils_print(L);
                break;
            case LUA_TNUMBER:
                printf("%d, ", lua_tointeger(L, -1));
                break;
            case LUA_TSTRING:
                printf("%s, ", lua_tostring(L, -1));
                break;
            }
            lua_pop(L, 1);
        }
        printf("}\n");
        break;
    case LUA_TFUNCTION:
        s = lua_tocfunction(L, 2);
        printf("[lua utils](function): %s", s);
        break;
    case LUA_TUSERDATA:
        printf("[lua utils](userdata):");
        break;
    case LUA_TTHREAD:
        break;
    case LUA_NUMTAGS:
    default:
        break;
    }
    return 0;
}


static int utils_version(lua_State *L)
{
    printf("Lua Module Version: 0.0.1\n");
    return 0;
}


static int utils_help(lua_State *L)
{
    printf(">>> lua help!\n");
    return 0;
}


// utils module
const luaL_Reg utils_method[] = {
    {"print",	utils_print},
    {"version",	utils_version},
    {"help",	utils_help},

    {NULL, NULL}
};


// 注册utils模块
LUALIB_API int luaopen_utils(lua_State *L)
{
    luaL_newlib(L, utils_method);
    return 1;
}



#endif /* #if (TCFG_LUA_ENABLE) */


