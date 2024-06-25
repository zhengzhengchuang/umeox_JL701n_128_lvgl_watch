#ifndef __LUA_GUI_H__
#define __LUA_GUI_H__


#include "lua/lua.h"


int lua_get_interget_from_self(lua_State *L, char *name);

int lua_set_interget_to_self(lua_State *L, char *key, int val);

int lua_get_self_id(lua_State *L);


void ui_css_get_abs_rect(struct element_css *css, struct rect *rect);


#endif
