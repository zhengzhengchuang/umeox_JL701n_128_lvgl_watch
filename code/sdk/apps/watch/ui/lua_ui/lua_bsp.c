#include "system/includes.h"
#include "app_config.h"
#include "gpio.h"
#include "list.h"
#include "event.h"
#include "ui/ui_sys_param.h"
#include "audio_config.h"

#include "lua/lauxlib.h"
#include "lua/lualib.h"

#include "lua_module.h"
#include "lua_bsp.h"
#include "lua_gui.h"



#if (TCFG_LUA_ENABLE)

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ gpio
// get gpio
static u32 bsp_get_gpio(lua_State *L)
{
    u32 gpio = 0;

    gpio = lua_get_interget_from_self(L, "_gpio");

    return gpio;
}


static int bsp_gpio_outputHigh(lua_State *L)
{
    u32 gpio;

    gpio =  bsp_get_gpio(L);
    lua_db("%s, gpio:%d\n", __FUNCTION__, gpio);
    gpio_direction_output(gpio, 1);

    return 0;
}

static int bsp_gpio_outputLow(lua_State *L)
{
    u32 gpio;

    gpio =  bsp_get_gpio(L);
    lua_db("%s, gpio:%d\n", __FUNCTION__, gpio);
    gpio_direction_output(gpio, 0);

    return 0;
}

static int bsp_gpio_setOutput(lua_State *L)
{
    u32 gpio;

    gpio =  bsp_get_gpio(L);
    lua_db("%s, gpio:%d\n", __FUNCTION__, gpio);
    gpio_set_direction(gpio, 1);

    return 0;
}

static int bsp_gpio_setInput(lua_State *L)
{
    u32 gpio;

    gpio =  bsp_get_gpio(L);
    lua_db("%s, gpio:%d\n", __FUNCTION__, gpio);
    gpio_direction_input(gpio);

    return 0;
}

static int bsp_gpio_read(lua_State *L)
{
    u32 gpio;
    int value;

    gpio =  bsp_get_gpio(L);
    value = gpio_read(gpio);
    lua_db("%s, gpio:%d, value:%d\n", __FUNCTION__, gpio, value);
    lua_pushinteger(L, value);

    return 1;
}

static int bsp_gpio_setPullUp(lua_State *L)
{
    u32 gpio;
    int onoff;

    onoff = lua_tointeger(L, -1);
    gpio =  bsp_get_gpio(L);
    lua_db("%s, gpio:%d, nooff:%d\n", __FUNCTION__, gpio, onoff);
    gpio_set_pull_up(gpio, onoff);

    return 0;
}

static int bsp_gpio_setPullDown(lua_State *L)
{
    u32 gpio;
    int onoff;

    onoff = lua_tointeger(L, -1);
    gpio =  bsp_get_gpio(L);
    lua_db("%s, gpio:%d, nooff:%d\n", __FUNCTION__, gpio, onoff);
    gpio_set_pull_down(gpio, onoff);

    return 0;
}

static int bsp_gpio_setDie(lua_State *L)
{
    u32 gpio;
    int onoff;

    onoff = lua_tointeger(L, -1);
    gpio =  bsp_get_gpio(L);
    lua_db("%s, gpio:%d, nooff:%d\n", __FUNCTION__, gpio, onoff);
    gpio_set_die(gpio, onoff);

    return 0;
}


static const luaL_Reg gpio_Method[] = {
    {"outputHigh", bsp_gpio_outputHigh},
    {"outputLow",  bsp_gpio_outputLow},
    {"setOutput",  bsp_gpio_setOutput},

    {"setInput",   bsp_gpio_setInput},
    {"read",       bsp_gpio_read},

    {"setPullUp",  bsp_gpio_setPullUp},
    {"setPullDown", bsp_gpio_setPullDown},
    {"setDie",     bsp_gpio_setDie},
    {NULL, NULL}
};

// 获取gpio对象
static int bsp_getGpioControl(lua_State *L)
{
    char *grop;
    int prot = 0;
    u32 gpio = 0;

    prot = lua_tointeger(L, -1);
    grop = lua_tostring(L, -2);
    gpio = (grop[0] - 'A') * IO_GROUP_NUM + prot;

    lua_db("set gpio: %s, %d, %d\n", grop, prot, gpio);

    lua_pop(L, 2);		// 将两个传入的参数出栈
    lua_newtable(L);	// 创建一个新table

    lua_pushstring(L, "_gpio");
    lua_pushinteger(L, gpio);
    lua_settable(L, 2);	// 把_gpio保存到table中
    luaL_setfuncs(L, gpio_Method, 0);	// 加载gpio的操作方法到table中

    return 1;	// 返回这个table
}
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end gpio



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ start iic
static int bsp_getIICControl(lua_State *L)
{
    return 0;
}
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end iic



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ start spi
static int bsp_getSPIControl(lua_State *L)
{
    return 0;
}
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end spi



// bsp module
const luaL_Reg bsp_method[] = {
    // 获取gpio对象
    {"getGpioControl",	bsp_getGpioControl},

    // 获取IIC对象
    {"getIICControl",	bsp_getIICControl},

    // 获取SPI对象
    {"getSPIControl",	bsp_getSPIControl},

    {NULL, NULL}
};


// 注册bsp模块
LUALIB_API int luaopen_bsp(lua_State *L)
{
    luaL_newlib(L, bsp_method);
    return 1;
}

#endif /* #if (TCFG_LUA_ENABLE) */


