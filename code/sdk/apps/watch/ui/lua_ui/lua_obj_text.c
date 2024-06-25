#include "ui/ui.h"
#include "ui/ui_core.h"

#include "lua/lauxlib.h"
#include "lua/lualib.h"

#include "lua_module.h"
#include "lua_obj.h"
#include "lua_gui.h"


#if (TCFG_LUA_ENABLE)


#define ui_text_for_id(id) \
	({ \
		struct element *elm = ui_core_get_element_by_id(id); \
	 	elm ? container_of(elm, struct ui_text, elm): NULL; \
	 })



// 设置文本控件内容
// 参数：
// (string) --> 默认显示内码字符串
// (string, redraw) --> 显示内码字符串，可选是否redraw
// (string, strlen) --> 默认显示内码字符串，指定长度
// (string, strlen, redraw) --> 默认显示内码字符串，指定长度，并可选redraw
// (string, redraw, encode) --> 根据格式显示字符串
// (string, redraw, encode, scroll) --> 滚动显示指定格式字符串
// (string, redraw, encode, scroll, endian) --> 滚动显示指定格式字符串，区分大小端
// (string, redraw, encode, scroll, endian, strlen) --> 滚动显示指定格式字符串，区分大小端，并指定字符串长度
// 返回：无
static int obj_setText(lua_State *L)
{
    int id = 0;
    int redraw = true;
    int encode = FONT_ENCODE_ANSI;
    int scroll = FONT_DEFAULT;
    int endian = FONT_ENDIAN_SMALL;
    int msglen = -1;
    int date_type = LUA_TNONE;
    const char *msg = NULL;
    struct ui_text *text = NULL;

    int n = lua_gettop(L);
    switch (n) {
    case 2:
        date_type = lua_type(L, -1);
        if (date_type == LUA_TNUMBER) {
            msg		= (char *)lua_tointeger(L, -1);
        } else {
            msg		= lua_tostring(L, -1);
        }
        break;
    case 3:
        date_type = lua_type(L, -1);
        /* stackDump(L); */
        if (date_type == LUA_TNUMBER) {	// 如果是数据类型表示字符串长度
            msglen = lua_tointeger(L, -1);
        } else if (date_type == LUA_TBOOLEAN) {	// 如果是布尔型，表示redraw标志
            redraw	= lua_toboolean(L, -1);
        }
        /* stackDump(L); */
        date_type = lua_type(L, -2);
        /* stackDump(L); */
        if (date_type == LUA_TNUMBER) {
            msg		= (char *)lua_tointeger(L, -2);
        } else {
            msg		= lua_tostring(L, -2);
        }
        break;
    case 4:
        printf("1");
        /* stackDump(L); */
        date_type = lua_type(L, -1);
        printf("date_type == %d\n", date_type);
        printf("2");
        /* stackDump(L); */
        if (date_type == LUA_TNUMBER) {		// 如果最后一个参数是数值，表明传递的是编码格式
            encode	= lua_tointeger(L, -1);
            redraw	= lua_toboolean(L, -2);
        } else if (date_type == LUA_TBOOLEAN) {	// 如果最后一个参数是布尔型，说明传递的是redraw标志
            redraw = lua_toboolean(L, -1);
            msglen = lua_tointeger(L, -2);
        }
        printf("encode is -> %d\n", encode);
        printf("redraw is -> %d\n", redraw);
        date_type = lua_type(L, -3);
        printf("date_type is -> %d\n", date_type);
        put_buf((char *)lua_tointeger(L, -3), 106);
        /* stackDump(L); */
        if (date_type == LUA_TNUMBER) {
            msg		= (char *)lua_tointeger(L, -3);
        } else {
            msg		= lua_tostring(L, -3);
        }
        break;
    case 5:
        scroll	= lua_tointeger(L, -1);
        encode	= lua_tointeger(L, -2);
        redraw	= lua_toboolean(L, -3);
        date_type = lua_type(L, -4);
        if (date_type == LUA_TNUMBER) {
            msg		= (char *)lua_tointeger(L, -4);
        } else {
            msg		= lua_tostring(L, -4);
        }
        break;
    case 6:
        endian	= lua_tointeger(L, -1);
        scroll	= lua_tointeger(L, -2);
        encode	= lua_tointeger(L, -3);
        redraw	= lua_toboolean(L, -4);
        date_type = lua_type(L, -5);
        if (date_type == LUA_TNUMBER) {
            msg		= (char *)lua_tointeger(L, -5);
        } else {
            msg		= lua_tostring(L, -5);
        }
        break;
    case 7:
        printf("1");
        /* stackDump(L); */
        msglen  = lua_tointeger(L, -1);
        printf("2");
        /* stackDump(L); */
        endian	= lua_tointeger(L, -2);
        printf("3");
        /* stackDump(L); */
        scroll	= lua_tointeger(L, -3);
        printf("4");
        /* stackDump(L); */
        encode	= lua_tointeger(L, -4);
        printf("5");
        /* stackDump(L); */
        redraw	= lua_toboolean(L, -5);
        printf("6");
        /* stackDump(L); */
        date_type = lua_type(L, -6);
        printf("7");
        /* stackDump(L); */
        put_buf((char *)lua_tointeger(L, -6), msglen);
        printf("date_type == %d\n", date_type);
        if (date_type == LUA_TNUMBER) {
            msg		= (char *)lua_tointeger(L, -6);
            y_printf(">>>> msg == 0x%x\n", msg);
        } else {
            msg		= lua_tostring(L, -6);
            g_printf("<<<< msg == 0x%x\n", msg);
        }
        break;
    default:
        printf("ERROR, arg number err!");
        return 0;
    }

    if (msglen == -1) {
        /* 如果字符串长度为-1，说明没有指定字符串长度，要主动计算 */
        msglen = strlen(msg);
    }

    lua_db("%s(msg=%s, redraw=%d, encode=%d, scroll=%d, endian=%d)\n", __FUNCTION__, msg, redraw, encode, scroll, endian);

    id = lua_get_self_id(L);
    text = ui_text_for_id(id);
    if (!text) {
        return 0;
    }
    text->attrs.offset = 0;
    text->attrs.format = "text";
    text->attrs.str	   = msg;
    text->attrs.strlen = msglen;
    text->attrs.encode = encode;//FONT_ENCODE_UNICODE;
    text->attrs.endian = endian;//FONT_ENDIAN_SMALL;
    text->attrs.flags  = scroll;//FONT_DEFAULT;
    printf("format:%s, str:%s, strlen:%d, encode:%d, endian:%d, flags:%d\n", text->attrs.format, text->attrs.str, text->attrs.strlen, text->attrs.encode, text->attrs.endian, text->attrs.flags);

    if (redraw) {
        ui_core_redraw(&text->elm);
    }
    /* ui_text_set_str_by_id(id, "ascii", msg); */

    return 0;
}


// 获取文本控件的文本
// 参数：无
// 返回：string
static int obj_getText(lua_State *L)
{
    int id = 0;
    char *str = NULL;
    struct ui_text *text = NULL;

    id = lua_get_self_id(L);
    text = ui_text_for_id(id);
    str = text->attrs.str;

    lua_db("id:0x%x, get Text: %s\n", id, str);
    if (str) {
        lua_pushstring(L, str);
        return 1;
    } else {
        printf("error! text is nil\n");
        return 0;
    }
}


// 设置ASCII字符串
/* static int obj_setAnsiText(lua_State *L) */
/* { */
/* return 0; */
/* } */


// 设置UTF8字符
// (string)
// (string, redraw)
// (string, strlen, redraw)
// (string, strlen, scroll, redraw)
static int obj_setUtf8Text(lua_State *L)
{
    int id = 0;
    int redraw = true;
    int scroll = FONT_DEFAULT;
    int strlen = -1;
    char *str = NULL;
    int date_type = LUA_TNONE;

    int n = lua_gettop(L);

    if (2 == n) {
        date_type = lua_type(L, 2);
        if (date_type == LUA_TNUMBER) {
            str		= (char *)lua_tointeger(L, 2);
        } else {
            str		= lua_tostring(L, 2);
        }
    } else if (3 == n) {
        redraw = lua_toboolean(L, -1);
        date_type = lua_type(L, 2);
        if (date_type == LUA_TNUMBER) {
            str		= (char *)lua_tointeger(L, 2);
        } else {
            str		= lua_tostring(L, 2);
        }
    } else if (4 == n) {
        redraw = lua_toboolean(L, -1);
        strlen = lua_tointeger(L, -2);
        date_type = lua_type(L, 2);
        if (date_type == LUA_TNUMBER) {
            str		= (char *)lua_tointeger(L, 2);
        } else {
            str		= lua_tostring(L, 2);
        }
    } else if (5 == n) {
        redraw = lua_toboolean(L, -1);
        scroll = lua_tointeger(L, -2);
        strlen = lua_tointeger(L, -3);
        date_type = lua_type(L, 2);
        if (date_type == LUA_TNUMBER) {
            str		= (char *)lua_tointeger(L, 2);
        } else {
            str		= lua_tostring(L, 2);
        }
    } else {
        printf("ERROR! %s arg number err!\n", __FUNCTION__);
        return 0;
    }

    struct ui_text *text = NULL;
    id = lua_get_self_id(L);
    text = ui_text_for_id(id);
    if (!text) {
        return 0;
    }

    text->attrs.offset = 0;
    text->attrs.format = "text";
    text->attrs.str	   = str;
    text->attrs.strlen = strlen;
    text->attrs.encode = FONT_ENCODE_UTF8;
    text->attrs.endian = FONT_ENDIAN_SMALL;
    text->attrs.flags  = scroll;

    if (redraw) {
        ui_core_redraw(&text->elm);
    }

    return 0;
}


// 设置unicode字符
// (string)
// (string, redraw)
// (string, strlen, redraw)
// (string, strlen, scroll, redraw)
// (string, strlen, scroll, endian, redraw)
static int obj_setUnicodeText(lua_State *L)
{
    int id = 0;
    int redraw = true;
    int scroll = FONT_DEFAULT;
    int endian = FONT_ENDIAN_SMALL;
    int strlen = -1;
    char *str = NULL;
    int date_type = LUA_TNONE;

    int n = lua_gettop(L);

    if (2 == n) {
        date_type = lua_type(L, 2);
        if (date_type == LUA_TNUMBER) {
            str		= (char *)lua_tointeger(L, 2);
        } else {
            str		= lua_tostring(L, 2);
        }
    } else if (3 == n) {
        redraw = lua_toboolean(L, -1);
        date_type = lua_type(L, 2);
        if (date_type == LUA_TNUMBER) {
            str		= (char *)lua_tointeger(L, 2);
        } else {
            str		= lua_tostring(L, 2);
        }
    } else if (4 == n) {
        redraw = lua_toboolean(L, -1);
        strlen = lua_tointeger(L, -2);
        date_type = lua_type(L, 2);
        if (date_type == LUA_TNUMBER) {
            str		= (char *)lua_tointeger(L, 2);
        } else {
            str		= lua_tostring(L, 2);
        }
    } else if (5 == n) {
        redraw = lua_toboolean(L, -1);
        scroll = lua_tointeger(L, -2);
        strlen = lua_tointeger(L, -3);
        date_type = lua_type(L, 2);
        if (date_type == LUA_TNUMBER) {
            str		= (char *)lua_tointeger(L, 2);
        } else {
            str		= lua_tostring(L, 2);
        }
    } else if (6 == n) {
        redraw = lua_toboolean(L, -1);
        endian = lua_tointeger(L, -2);
        scroll = lua_tointeger(L, -3);
        strlen = lua_tointeger(L, -4);
        date_type = lua_type(L, 2);
        if (date_type == LUA_TNUMBER) {
            str		= (char *)lua_tointeger(L, 2);
        } else {
            str		= lua_tostring(L, 2);
        }
    } else {
        printf("ERROR! %s arg number err!\n", __FUNCTION__);
        return 0;
    }

    struct ui_text *text = NULL;
    id = lua_get_self_id(L);
    text = ui_text_for_id(id);
    if (!text) {
        return 0;
    }

    text->attrs.offset = 0;
    text->attrs.format = "text";
    text->attrs.str	   = str;
    text->attrs.strlen = strlen;
    text->attrs.encode = FONT_ENCODE_UNICODE;
    text->attrs.endian = endian;
    text->attrs.flags  = scroll;

    if (redraw) {
        ui_core_redraw(&text->elm);
    }

    return 0;
}



// 显示文本，设置显示文本列表中指定项的文本
// 参数：
// (index)
// (index, redraw)
// 返回：无
static int obj_showTextByIndex(lua_State *L)
{
    int id = 0;
    int index = 0;
    int redraw = true;
    struct ui_text *text = NULL;

    int n = lua_gettop(L);
    if (3 == n) {
        redraw = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }

    index = luaL_checkinteger(L, -1);
    id = lua_get_self_id(L);
    text = ui_text_for_id(id);
    ui_text_set_index(text, index);

    lua_db("id:0x%x, show Text: %d, redraw: %d\n", id, index, redraw);

    if (redraw) {
        if (text->elm.css.invisible) {
            ui_core_show(&text->elm, false);
        } else {
            ui_core_redraw(&text->elm);
        }
    }
    /* ui_text_show_index_by_id(id, index); */

    return 0;
}


// 显示文本，设置显示文本列表中指定项的文本
// 参数：
// (index_table, index_num)
// (index_table, index_num, redraw)
// 例：({1, 2, 3}, 3, false)
// 返回：无
static int obj_showMultiTextByIndex(lua_State *L)
{
    int id = 0;
    int index_num = 0;
    int redraw = true;
    struct ui_text *text = NULL;
    int i;

    int n = lua_gettop(L);
    if (4 == n) {
        redraw = lua_toboolean(L, -1);
        lua_pop(L, 1);
    }
    index_num = luaL_checkinteger(L, -1);
    lua_pop(L, 1);

    u8 *index_buf = (u8 *)malloc(index_num);
    for (i = 0; i < index_num; i++) {
        int val;
        lua_pushinteger(L, i + 1);
        lua_gettable(L, -2);
        val = lua_tointeger(L, -1);
        index_buf[i] = val;
        lua_db("index_buf[%d] = %d\n", i, val);
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    id = lua_get_self_id(L);
    text = ui_text_for_id(id);
    lua_db("id:0x%x, show Multi Text: %d, redraw: %d\n", id, index_num, redraw);
    ui_text_set_multi_text_index(text, index_buf, index_num);

    if (redraw) {
        if (text->elm.css.invisible) {
            ui_core_show(&text->elm, false);
        } else {
            ui_core_redraw(&text->elm);
        }
    }
    free(index_buf);

    return 0;
}

// 获取当前显示的文本列表索引
// 参数：无
// 返回：index
static int obj_getTextIndex(lua_State *L)
{
    int id = 0;
    int index = 0;
    struct ui_text *text = NULL;

    id = lua_get_self_id(L);
    text = ui_text_for_id(id);
    index = text->index;
    lua_db("id:0x%x, get Text: %d\n", id, index);
    lua_pushinteger(L, index);

    return 1;
}


// 获取文本列表中文本的数量
// 参数：无
// 返回：number
static int obj_getTextNumber(lua_State *L)
{
    int id = 0;
    int number = 0;
    struct ui_text *text = NULL;

    id = lua_get_self_id(L);
    text = ui_text_for_id(id);
    number = text->str_num;
    lua_db("id:0x%x, text number: %d\n", id, number);
    lua_pushinteger(L, number);

    return 1;
}


// 获取文本格式
// 参数：无
// 返回：format
static int obj_getTextFormat(lua_State *L)
{
    int id = 0;
    char *format = NULL;
    struct ui_text *text = NULL;

    id = lua_get_self_id(L);
    text = ui_text_for_id(id);
    format = text->attrs.format;
    lua_db("id:0x%x, text format: %d\n", id, format);

    if (format) {
        lua_pushstring(L, format);
        return 1;
    } else {
        printf("error! text format is nil\n");
        return 0;
    }
}



#if 0
// 设置对齐方式
static int obj_setTextAlignment(lua_State *L)
{
    int id = 0;
    const char *alignment = NULL;

    alignment = luaL_checkstring(L, -1);
    lua_db(">>>>> set text alignment:%s\n", alignment);

    id = lua_get_self_id(L);

    // 下面设置对齐方式

    return 0;
}
#endif


// text文本控件方法
const luaL_Reg obj_TextMethod[] = {

    {"setText",		obj_setText},
    {"getText",		obj_getText},

    {"setUtf8Text",			obj_setUtf8Text},
    {"setUnicodeText",		obj_setUnicodeText},

    {"showTextByIndex",		obj_showTextByIndex},
    {"showMultiTextByIndex",	obj_showMultiTextByIndex},

    {"getTextIndex",		obj_getTextIndex},
    {"getTextNumber",		obj_getTextNumber},
    {"getTextFormat",		obj_getTextFormat},

    {NULL, NULL}
};

#endif /* #if (TCFG_LUA_ENABLE) */

