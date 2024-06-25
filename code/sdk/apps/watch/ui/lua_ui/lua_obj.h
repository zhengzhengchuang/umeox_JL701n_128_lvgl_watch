#ifndef __LUA_OBJ_H__
#define __LUA_OBJ_H__


#include "lua/lua.h"
#include "res/resfile.h"

typedef struct viewfile {
    RESFILE *file;
    struct flash_file_info info;
} VIEWFILE;


void ui_get_coordinate(int id, int *x, int *y, int *w, int *h);

void set_func_to_self_table(lua_State *L, unsigned char index, char *func_name);

void run_func_from_self_table(lua_State *L, char *func_name);



/* 方法列表声明 */
extern const luaL_Reg obj_method[];			/* gui 公共方法 */
extern const luaL_Reg obj_DrawMethod[];		/* 自定义绘图 */
extern const luaL_Reg obj_ImageMethod[];	/* 图片控件 */
extern const luaL_Reg obj_TextMethod[];		/* 文本控件 */
extern const luaL_Reg obj_TimeMethod[];		/* 时间控件 */
extern const luaL_Reg obj_BatteryMethod[];	/* 电池电量控件 */
extern const luaL_Reg obj_NumberMethod[];	/* 数字控件 */
extern const luaL_Reg obj_ListMethod[];		/* 表格和列表控件 */
extern const luaL_Reg obj_WatchMethod[];	/* 手表、表盘控件 */
extern const luaL_Reg obj_SliderMethod[];	/* 进度条控件 */
extern const luaL_Reg obj_CompassMethod[];	/* 指南针控件 */
extern const luaL_Reg obj_ProgressMethod[];	/* 圆弧进度条控件 */
extern const luaL_Reg obj_MultiProgressMethod[];	/* 多重圆弧进度条控件 */



#endif

