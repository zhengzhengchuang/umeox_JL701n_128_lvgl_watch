#include "sport_info_sync.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"

#include "lua_module.h"
#include "lua_device.h"
#include "sport_data/watch_common.h"


#if (TCFG_LUA_ENABLE)


#define DEVICE_LUA_TEST 1
#define abs(x)  ((x)>0?(x):-(x) )


static struct watch_execise *__execise_hd = NULL;

// 查找数组中的第一个最小值
// 数组指针，数组长度，最小值存储地址，最小值索引
static void get_min_data(int *array[], int size, int *min, int *index)
{
    *index = 0;
    *min = array[0];

    for (int i = 0; i < size; i++) {
        /* printf("data:%d\n", array[i]); */
        if (*min > array[i]) {
            *min = array[i];
            *index = i;
        }
    }
}

// 查找数组中的第一个最大值
// 数组指针，数组长度，最大值存储地址，最大值索引
static void get_max_data(int *array[], int size, int *max, int *index)
{
    *index = 0;
    *max = array[0];

    for (int i = 0; i < size; i++) {
        /* printf("data:%d\n", array[i]); */
        if (*max < array[i]) {
            *max = array[i];
            *index = i;
        }
    }
}


// 获取步数
static int device_getStepNumber(lua_State *L)
{
    int step_number = 0;

#if DEVICE_LUA_TEST
    step_number = 100;
    lua_db(">> device << %s, get number: %d\n", __FUNCTION__, step_number);
    // 把获取到的步数压栈并返回
    lua_pushinteger(L, step_number);
    return 1;
#endif
    step_number = step_number;//get_step();//实时步数
    lua_pushinteger(L, step_number);

    return 1;
}


static int device_setSportCtrl(lua_State *L) //device:setSportStatus(ctrl_mode,sport_type)
{
    printf("%s %d", __func__, __LINE__);
    int sport_type = 0;
    int ctrl_mode = 0;
    sport_type = lua_tointeger(L, -1);
    lua_pop(L, 1);
    ctrl_mode = lua_tointeger(L, -1);
    lua_pop(L, 1);
    printf("sport_type=%d  ctrl_mode=%d", sport_type, ctrl_mode);

    if (__execise_hd == NULL) {
        __execise_hd = zalloc(sizeof(struct watch_execise));
        watch_execise_handle_get(__execise_hd);
    }

    u8 sport_ctrl_status = __execise_hd->execise_ctrl_status_get();
    switch (ctrl_mode) {
    case DEVICE_SPORT_CTRL_CLR:
        printf("DEVICE_SPORT_CTRL_CLR");
        __execise_hd->execise_ctrl_status_clr();
        break;
    case DEVICE_SPORT_CTRL_START:
        if (sport_ctrl_status == 0) {
            if (sport_type == 0) {
                /* assert("sport_type=0 but sport will start"); */
            }
            __execise_hd->execise_ctrl_status_set(sport_type, SPORT_STATUS_START);
            sport_info_sync_start_exercise_resp(NULL, NULL);//向APP发送运动命令
            extern int watch_sport_start(u8 type); //开始运动
            watch_sport_start(sport_type);//记录运动文件
        } else {
            printf("recall sport_start");
        }
        break;
    case DEVICE_SPORT_CTRL_PAUSE:
        if ((sport_ctrl_status == 1) | (sport_ctrl_status == 3)) {
            __execise_hd->execise_ctrl_status_set(SPORT_MODE_NOT_TRANS, SPORT_STATUS_PAUSE);
            sport_info_sync_pause_exercise_resp();
            extern int watch_sport_pause();
            watch_sport_pause();
        } else {
            printf("recall sport_pause");
        }

        break;
    case DEVICE_SPORT_CTRL_KEEP:
        if (sport_ctrl_status == 2) {
            __execise_hd->execise_ctrl_status_set(SPORT_MODE_NOT_TRANS, SPORT_STATUS_CONTINNUE);
            sport_info_sync_keep_exercise_resp();
            extern int watch_sport_restart();
            watch_sport_restart();
        } else {
            printf("recall sport_keep");
        }
        break;
    case DEVICE_SPORT_CTRL_STOP:
        if (sport_ctrl_status != 0) {
            __execise_hd->execise_ctrl_status_set(SPORT_MODE_NOT_TRANS, SPORT_STATUS_STOP);
            extern int watch_sport_stop();
            watch_sport_stop();//停止文件记录
            sport_info_sync_end_exercise_by_fw();//向APP发送停止运动命令
            __execise_hd->execise_info_clr();//清除运动模式
        } else {
            printf("recall sport_stop");
        }
        break;
    case DEVICE_SPORT_CTRL_STATUS:
        ctrl_mode = __execise_hd->execise_ctrl_status_get();
        lua_pushinteger(L, ctrl_mode);
        return 1;
    }

    return 0;
}


// 获取运动信息
static int device_getStepInfo(lua_State *L)
{
    printf("%s ", __func__);
    int mode = 0x00;
    int sportinfo_data;
    mode = lua_tointeger(L, -1);
    lua_pop(L, 1);
    printf("%s mode=%d", __func__, mode);
    if (__execise_hd == NULL) {
        __execise_hd = zalloc(sizeof(struct watch_execise));
        watch_execise_handle_get(__execise_hd);
    }
    struct motion_info  __execise_info;
    __execise_hd->execise_info_get(&__execise_info);
    switch (mode) {
    case DEVICE_SPORT_INFO_ALL://
        printf("index=motion_intensity,step_freq, pace,motion_kcal,motion_distance,motion_step");//索引说明
        return 0;
    case DEVICE_SPORT_INFO_STEP:
        //sportinfo_data = 100;
        sportinfo_data = __execise_info.step; //运动步数
        break;
    case DEVICE_SPORT_INFO_DISTANCE:
        //sportinfo_data = 10;
        sportinfo_data = __execise_info.step_distance;
        break;
    case DEVICE_SPORT_INFO_KCAL :
        /* sportinfo_data = 50; */
        sportinfo_data = __execise_info.caloric;
        break;
    case DEVICE_SPORT_INFO_MOTIONSTATUS:
        /* sportinfo_data = 1; */
        sportinfo_data = 0;
        break;
    case DEVICE_SPORT_INFO_MOTIONTIME:
        /* sportinfo_data = 10086; */
        sportinfo_data = __execise_info.motion_time;
        break;
    case DEVICE_SPORT_INFO_SPEED:
        /* sportinfo_data = 80; */
        sportinfo_data = __execise_info.motion_speed;
        break;
    case DEVICE_SPORT_INFO_PACE:
        /* sportinfo_data = 680; */
        sportinfo_data = __execise_info.motion_pace;
        break;
    case DEVICE_SPORT_INFO_STEPFREQ:
        /* sportinfo_data = 5; */
        sportinfo_data = __execise_info.motion_step_freq;
        break;
    case DEVICE_SPORT_INFO_STEPSTRIDE:
        /* sportinfo_data = 10; */
        sportinfo_data = __execise_info.motion_step_stride;
        break;
    case DEVICE_SPORT_INFO_INTENSITY:
        /* sportinfo_data = 1; */
        sportinfo_data = __execise_info.intensity;
        break;
    case DEVICE_SPORT_INFO_RECOVERYTIME:
        /* sportinfo_data=70; */
        sportinfo_data = __execise_info.recoverytime_min + 60 * __execise_info.recoverytime_hour;
        break;
    case DEVICE_SPORT_INFO_INTENSITYTIME:
        int sportinfo_buf[5] = {0};
        memcpy(sportinfo_buf, __execise_info.intensity_time, 20);
        lua_newtable(L);
        lua_pushstring(L, "intensity5");
        lua_pushinteger(L, sportinfo_buf[4]);
        lua_settable(L, 2);
        lua_pushstring(L, "intensity4");
        lua_pushinteger(L, sportinfo_buf[3]);
        lua_settable(L, 2);
        lua_pushstring(L, "intensity3");
        lua_pushinteger(L, sportinfo_buf[2]);
        lua_settable(L, 2);
        lua_pushstring(L, "intensity2");
        lua_pushinteger(L, sportinfo_buf[1]);
        lua_settable(L, 2);
        lua_pushstring(L, "intensity1");
        lua_pushinteger(L, sportinfo_buf[0]);
        lua_settable(L, 2);
        printf("intensitytable ok");
        return 1;
    default:
        sportinfo_data = 0;
        break;
    }
    printf("%s mode=%d  data=%d ", __func__, mode, sportinfo_data);
    lua_pushinteger(L, sportinfo_data);

    return 1;
}


#if DEVICE_LUA_TEST
// 获取心率
static int test_heartrate[] = {65, 72, 83, 96, 76, 72, 67, 91, 85, 70};
static int device_getHeartRate(lua_State *L)
{
    int mode = 0x00;
    int heartrate = 0;
    int time = 0;
    static u8 index = 0;
    int data_number = get_array_num(test_heartrate);

    int n = lua_gettop(L);
    if (3 == n) {
        index = lua_tointeger(L, -1);
        if (index < 0) {
            index = data_number - abs(index);
        }
        lua_pop(L, 1);
        n = n - 1;
        if ((index >= data_number) || (index < 0)) {
            printf("ERROR! %s, set index err, reset index to 0.\n", __FUNCTION__);
            index = 0;
            lua_pop(L, 1);	/* 把模式设置出栈 */
            lua_pushnil(L);
            lua_pushnil(L);
            return 2;
        }
    }

    if (2 == n) {
        mode = lua_tointeger(L, -1);
        lua_pop(L, 1);
    }

    switch (mode) {
    case DEVICE_GET_NEXT_DATA:
        /* 获取下一个数据 */
        heartrate = test_heartrate[index];
        time = index;
        if (++index >= data_number) {
            index = data_number - 1;	// 超过最后一个，只获取最后一个
            lua_pushnil(L);
            lua_pushnil(L);
            return 2;
        }
        break;

    case DEVICE_GET_PREV_DATA:
        /* 获取上一个数据 */
        heartrate = test_heartrate[index];
        time = index;
        if (--index < 0) {
            index = 0;
            lua_pushnil(L);
            lua_pushnil(L);
            return 2;
        }
        break;

    case DEVICE_GET_ALL_DATA:
        /* 获取全部数据 */
        lua_newtable(L);
        return 1;	// 返回一个table

    case DEVICE_GET_IND_DATA:
        /* 获取指定索引的数据 */
        heartrate = test_heartrate[index];
        time = index;
        break;

    case DEVICE_GET_MAX_DATA:
        /* 获取最大值 */
        get_max_data(test_heartrate, data_number, &heartrate, &time);
        break;

    case DEVICE_GET_MIN_DATA:
        /* 获取最小值 */
        get_min_data(test_heartrate, data_number, &heartrate, &time);
        break;

    case DEVICE_GET_DATA_NUM:
        /* 获取数据个数 */
        lua_pushinteger(L, data_number);
        return 1;

    default:
        heartrate = test_heartrate[data_number - 1];
        time = data_number - 1;
        break;
    }

    lua_db("%s, heartrate:%d, time:%d\n", __FUNCTION__, heartrate, time);
    lua_pushinteger(L, heartrate);
    lua_pushinteger(L, time);

    return 2;
}

#else

static int device_getHeartRate(lua_State *L)
{
    int mode = 0x00;
    int heartrate = 0;
    int time = 0;
    static u8 index = 0;
    int data_number = get_array_num(test_heartrate);
    int n = lua_gettop(L);
    if (3 == n) {
        index = lua_tointeger(L, -1);
        if (index < 0) {
            index = data_number - abs(index);
        }
        lua_pop(L, 1);
        n = n - 1;
        if ((index >= data_number) || (index < 0)) {
            printf("ERROR! %s, set index err, reset index to 0.\n", __FUNCTION__);
            index = 0;
            lua_pop(L, 1);	/* 把模式设置出栈 */
            lua_pushnil(L);
            lua_pushnil(L);
            return 2;
        }
    }
    if (2 == n) {
        mode = lua_tointeger(L, -1);
        lua_pop(L, 1);
    }
    switch (mode) {
    case DEVICE_GET_REAL_DATA:/* 获取实时值*/
        heartrate = get_real_heart_data();
        lua_pushinteger(L, heartrate);
        return 1;
        break;
    case DEVICE_GET_MAX_DATA:/* 获取最大值 */
        heartrate = get_max_heart_rate();
        lua_pushinteger(L, heartrate);
        return 1;
        break;
    case DEVICE_GET_MIN_DATA:/* 获取最小值 */
        heartrate = get_min_heart_rate();
        lua_pushinteger(L, heartrate);
        return 1;
        break;
    case DEVICE_GET_DATA_NUM:
        return 0;
        break;
    case DEVICE_GET_NEXT_DATA:
        return 0;
        break;
    case DEVICE_GET_PREV_DATA:
        return 0;
        break;
    case DEVICE_GET_ALL_DATA:
        return 0;
        break;
    case DEVICE_GET_IND_DATA://索引值
        return 0;
        break;
    default:
        break;
    }
    return 0;
}

#endif

// 获取静息心率
static int device_getRestingHeartRate(lua_State *L)
{
    // 用平均值模拟静息心率
    int sum = 0;
    int resting_heartrate = 0;
    int data_number = get_array_num(test_heartrate);
    for (int i = 0; i < data_number; i++) {
        sum += test_heartrate[i];
    }
    resting_heartrate = sum / data_number;
    lua_db("%s, get resting heart rate: %d\n", __FUNCTION__, resting_heartrate);
    lua_pushinteger(L, resting_heartrate);
    return 1;
}


// 获取血氧
#if DEVICE_LUA_TEST
static int device_getBloodOxygen(lua_State *L)
{

    int oxygen = 98;
    lua_pushinteger(L, oxygen);
    return 1;
}
#else
static int device_getBloodOxygen(lua_State *L)
{
    int mode = 0x00;
    int bloodoxygen = 0;
    int time = 0;
    static u8 index = 0;
    int data_number = get_array_num(test_heartrate);
    int n = lua_gettop(L);
    if (3 == n) {
        index = lua_tointeger(L, -1);
        if (index < 0) {
            index = data_number - abs(index);
        }
        lua_pop(L, 1);
        n = n - 1;
        if ((index >= data_number) || (index < 0)) {
            printf("ERROR! %s, set index err, reset index to 0.\n", __FUNCTION__);
            index = 0;
            lua_pop(L, 1);	/* 把模式设置出栈 */
            lua_pushnil(L);
            lua_pushnil(L);
            return 2;
        }
    }
    if (2 == n) {
        mode = lua_tointeger(L, -1);
        lua_pop(L, 1);
    }
    struct watch_blood_oxygen __bloodoxygen_hd;
    watch_blood_oxygen_handle_get(&__bloodoxygen_hd);
    switch (mode) {
    case DEVICE_GET_REAL_DATA:/* 获取实时值*/
        u8 wear;
        bloodoxygen = __bloodoxygen_hd.blood_oxygen_get(LAST_DATA, NULL);
        lua_pushinteger(L, bloodoxygen);
        return 1;
        break;
    case DEVICE_GET_MAX_DATA:/* 获取最大值 */
        bloodoxygen = __bloodoxygen_hd.blood_oxygen_get(MAX_DATA, NULL);
        lua_pushinteger(L, bloodoxygen);
        return 1;
        break;
    case DEVICE_GET_MIN_DATA:/* 获取最小值 */
        bloodoxygen = __bloodoxygen_hd.blood_oxygen_get(MIN_DATA, NULL);
        lua_pushinteger(L, bloodoxygen);
        return 1;
        break;
    case DEVICE_GET_DATA_NUM:
        return 0;
        break;
    case DEVICE_GET_NEXT_DATA:
        return 0;
        break;
    case DEVICE_GET_PREV_DATA:
        return 0;
        break;
    case DEVICE_GET_ALL_DATA:
        return 0;
        break;
    case DEVICE_GET_IND_DATA://索引值
        return 0;
        break;
    default:
        break;
    }
    return 0;
}

#endif

// ~~~~~~~~~~~~~~~~~~~~~~~ 大气压强和海拔高度
// ---------> 返回值均为：(数值，时间)或对应列表
// 获取大气压强
// 无参数：当前的大气压强，返回（压强，时间）
// (NEXT/PREV/ALL)：获取下一个/上一个数据，从index计算
// (NEXT/PREV/ALL, index)： 获取第index个数据
// 注意：传入了index之后，相当于设置游标，前一个参数不起作用, 对于index, 0表示第一个，-1表示最后一个
static int test_pressure[] = {990, 995, 992, 996, 993, 1000, 1001, 998, 996, 994};
static int device_getPressure(lua_State *L)
{
    int mode = 0x00;
    int pressure = 0;
    int time = 0;
    static u8 index = 0; /* 索引游标 */
    int data_number = get_array_num(test_pressure);

    int n = lua_gettop(L);
    if (3 == n) {
        /* 两个参数时，最后一个参数是设置游标 */
        index = lua_tointeger(L, -1);
        if (index < 0) {
            /* 设置的游标为负数时，表示从设置为数据倒数第index个 */
            index = data_number - abs(index);
        }
        lua_pop(L, 1);
        n = n - 1;

        if ((index >= data_number) || (index < 0)) {
            /* 如果游标不在数据范围内，重置为 0 并返回nil，*/
            printf("ERROR! %s, set index err, reset index to 0.\n", __FUNCTION__);
            index = 0;
            lua_pop(L, 1);	/* 把模式设置出栈 */
            lua_pushnil(L);
            lua_pushnil(L);
            return 2;
        }
    }

    if (2 == n) {
        /* 获取参数的模式 */
        mode = lua_tointeger(L, -1);
        lua_pop(L, 1);
    }

    switch (mode) {
    case DEVICE_GET_NEXT_DATA:
        /* 获取下一个数据 */
        pressure = test_pressure[index];
        time = index;
        if (++index >= data_number) {
            index = data_number - 1;	// 超过最后一个，只获取最后一个
            lua_pushnil(L);
            lua_pushnil(L);
            return 2;
        }
        break;

    case DEVICE_GET_PREV_DATA:
        /* 获取上一个数据 */
        pressure = test_pressure[index];
        time = index;
        if (--index < 0) {
            index = 0;
            lua_pushnil(L);
            lua_pushnil(L);
            return 2;
        }
        break;

    case DEVICE_GET_ALL_DATA:
        /* 获取全部数据 */
        lua_newtable(L);
        return 1;	// 返回一个table

    case DEVICE_GET_IND_DATA:
        /* 获取指定索引的数据 */
        pressure = test_pressure[index];
        time = index;
        break;

    case DEVICE_GET_MAX_DATA:
        /* 获取最大值 */
        get_max_data(test_pressure, data_number, &pressure, &time);
        break;

    case DEVICE_GET_MIN_DATA:
        /* 获取最小值 */
        get_min_data(test_pressure, data_number, &pressure, &time);
        break;

    case DEVICE_GET_DATA_NUM:
        /* 获取数据个数 */
        lua_pushinteger(L, data_number);
        return 1;

    default:
        pressure = test_pressure[data_number - 1];
        time = data_number - 1;
        break;
    }

    lua_db("%s, pressure:%d, time:%d\n", __FUNCTION__, pressure, time);
    lua_pushinteger(L, pressure);
    lua_pushinteger(L, time);
    return 2;
}



static int test_altitude[] = {6, 7, 9, 3, 2, 5, 10, 5, 3, 2};
// 获取当前的海拔高度
static int device_getAltitude(lua_State *L)
{
    int mode = 0x00;
    int altitude = 0;
    int time = 0;
    static u8 index = 0;
    int data_number = get_array_num(test_altitude);

    int n = lua_gettop(L);
    if (3 == n) {
        index = lua_tointeger(L, -1);
        if (index < 0) {
            index = data_number - abs(index);
        }
        lua_pop(L, 1);
        n = n - 1;
        if ((index >= data_number) || (index < 0)) {
            printf("ERROR! %s, set index err, reset index to 0.\n", __FUNCTION__);
            index = 0;
            lua_pop(L, 1);	/* 把模式设置出栈 */
            lua_pushnil(L);
            lua_pushnil(L);
            return 2;
        }
    }

    if (2 == n) {
        mode = lua_tointeger(L, -1);
        lua_pop(L, 1);
    }

    switch (mode) {
    case DEVICE_GET_NEXT_DATA:
        altitude = test_altitude[index];
        time = index;
        if (++index > data_number) {
            index = data_number - 1;	// 超过最后一个，只获取最后一个
            lua_pushnil(L);
            lua_pushnil(L);
            return 2;
        }
        break;

    case DEVICE_GET_PREV_DATA:
        altitude = test_altitude[index];
        time = index;
        if (--index < 0) {
            index = 0;
            lua_pushnil(L);
            lua_pushnil(L);
            return 2;
        }
        break;

    case DEVICE_GET_ALL_DATA:
        lua_newtable(L);
        return 1;	// 返回一个table

    case DEVICE_GET_IND_DATA:
        altitude = test_altitude[index];
        time = index;
        break;

    case DEVICE_GET_MAX_DATA:
        get_max_data(test_altitude, data_number, &altitude, &time);
        break;

    case DEVICE_GET_MIN_DATA:
        get_min_data(test_altitude, data_number, &altitude, &time);
        break;

    case DEVICE_GET_DATA_NUM:
        /* 获取数据个数 */
        lua_pushinteger(L, data_number);
        return 1;

    default:
        altitude = test_altitude[data_number - 1];
        time = data_number - 1;
        break;
    }

    lua_db("%s, altitude:%d, time:%d\n", __FUNCTION__, altitude, time);
    lua_pushinteger(L, altitude);
    lua_pushinteger(L, time);
    return 2;
}



// device module
const luaL_Reg device_method[] = {

    // 步数
    {"getStepNumber", 		device_getStepNumber},

    //运动信息
    {"getSportInfo",		device_getStepInfo},

    {"setSportStatus",		device_setSportCtrl},

    // 心率
    {"getHeartRate",		device_getHeartRate},

    // 获取静息心率
    {"getRestingHeartRate",	device_getRestingHeartRate},

    // 血氧
    {"getBloodOxygen",		device_getBloodOxygen},

    // 大气压强
    {"getPressure",			device_getPressure},

    // 海拔高度
    {"getAltitude",			device_getAltitude},

    {NULL, NULL}
};


// 注册utils模块
LUALIB_API int luaopen_device(lua_State *L)
{
    luaL_newlib(L, device_method);
    return 1;
}

#endif /* #if (TCFG_LUA_ENABLE) */

