#include "ui/ui.h"
#include "ui/ui_number.h"

#include "lua/lauxlib.h"
#include "lua/lualib.h"

#include "lua_module.h"
#include "lua_obj.h"
#include "lua_gui.h"

#if (TCFG_LUA_ENABLE)


#define ui_number_for_id(id) \
    ({ \
        struct element *elm = ui_core_get_element_by_id(id); \
        elm ? (struct ui_number *)elm: NULL; \
     })



// 设置数字控件内容
// 参数：
// (num0)
// (num0, redraw)
// (num0, num1)
// (num0, num1, redraw)
// (string)
// (string, redraw)
// 返回：无
static int obj_setNumber(lua_State *L)
{
    int id = 0;
    int redraw = true;
    struct unumber numb = {0};
    struct ui_number *number = NULL;

    int type = lua_type(L, -1);
    if (type == LUA_TBOOLEAN) {
        /* 如果最后一个参数是boolean值，说明是redraw标志 */
        redraw = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }

    id = lua_get_self_id(L);
    number = ui_number_for_id(id);

    int n = lua_gettop(L);
    if (3 == n) {
        numb.numbs = 2;
        numb.type = TYPE_NUM;
        numb.number[1] = lua_tointeger(L, -1);
        numb.number[0] = lua_tointeger(L, -2);
    } else if (2 == n) {
        type = lua_type(L, -1);
        if (type == LUA_TNUMBER) {
            numb.numbs = 1;
            numb.type = TYPE_NUM;
            numb.number[0] = lua_tointeger(L, -1);
            numb.number[1] = 0;
        } else {
            numb.type = TYPE_STRING;
            char *str = lua_tostring(L, -1);
            int str_len = strlen(str) + 1;
            if (!number->temp_str) {
                number->temp_str = (u8 *)malloc(str_len);
                number->temp_str_len = str_len;
            } else {
                if (str_len > number->temp_str_len) {
                    free(number->temp_str);
                    number->temp_str = (u8 *)malloc(str_len);
                    number->temp_str_len = str_len;
                }
            }
            strcpy(number->temp_str, str);
            numb.num_str = number->temp_str;
            //printf("numb.num_str : %s\n", number->num_str);
            lua_db("[ %s ], string:%d\n", __FUNCTION__, numb.num_str);
        }
    } else {
        printf("error! argument is not number or string!\n");
        return 0;
    }

    ui_number_update(number, &numb);
    if (redraw) {
        ui_core_redraw(&number->text.elm);
    }

    return 0;
}


// 获取数字控件显示的数字
// 参数：无
// 返回：number
static int obj_getNumber(lua_State *L)
{
    int id = 0;
    int type = 0;
    struct ui_number *numb;

    id = lua_get_self_id(L);
    numb = ui_number_for_id(id);

    if (numb->type == TYPE_NUM) {
        if (numb->nums == 1) {
            lua_pushinteger(L, numb->number[0]);
            return 1;
        } else if (numb->nums == 2) {
            lua_pushinteger(L, numb->number[0]);
            lua_pushinteger(L, numb->number[1]);
            return 2;
        } else {
            printf("error!, number->nums is 0\n");
            return 0;
        }
    } else if (numb->type == TYPE_STRING) {
        lua_pushstring(L, numb->num_str);
        return 1;
    }

    return 0;
}

// num: num0, num1
// str: string
static int obj_updateNumber(lua_State *L)
{
    int id = 0;
    int top = 0;
    int num0 = 0, num1 = 0;
    struct unumber numb;

    id = lua_get_self_id(L);

    // 通过传入的参数数量来确定什么类型
    // 两个参数为number，一个参数为string
    top = lua_gettop(L);
    if (2 ==  top) {
        numb.type = TYPE_STRING;
        numb.numbs = 1;
        numb.num_str = lua_tostring(L, -1);
        lua_db("upateValue str:%s\n", numb.num_str);
    } else if (3 == top) {
        numb.type = TYPE_NUM;

        if (lua_isnil(L, -1)) {
            // 如果num1是空的
            num1 = -1;
        } else {
            numb.number[1] = lua_tointeger(L, -1);
        }

        if (lua_isnil(L, -2)) {
            num0 = -1;
        } else {
            numb.number[0] = lua_tointeger(L, -2);
        }

        if (num0 && num1) {
            // 两个参数都是nil
            numb.numbs = 0;
        } else if (!num0 && !num1) {
            // 两个参数都不是nil
            numb.numbs = 2;
        } else {
            // 只有一个有效参数
            numb.numbs = 1;
        }

        lua_db("upateValue %d, num0:%d, num1:%d\n", numb.numbs, numb.number[0], numb.number[1]);
    } else {
        log_e("UpdateValue argument number err!");
        return 0;
    }

    if (numb.numbs) {
        /* 有参数才update */
        ui_number_update_by_id(id, &numb);
    }

    return 0;
}




const luaL_Reg obj_NumberMethod[] = {
    {"setNumber",		obj_setNumber},
    {"getNumber",		obj_getNumber},

    // update 数字时：obj:updateValue(num0, num1)
    // update 字符串：obj:updateValue('string')
    /* {"updateNumber",	obj_updateNumber}, */

    {NULL, NULL}
};

#endif /* #if (TCFG_LUA_ENABLE) */

