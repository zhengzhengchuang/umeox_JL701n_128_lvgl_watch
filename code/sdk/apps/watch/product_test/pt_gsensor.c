#include "product_test.h"
#include "pt_gsensor.h"
#include "gSensor/gSensor_manage.h"
#include "asm/math_fast_function.h"
#if PT_GSENSOR_ENABLE
/*****************************************************/
//				PT_GSENSOR_CONFIG
/*****************************************************/

#define GSENSOR_CHECK_DATA_LEN 	(3*32) 	// 三轴*32组数据
#define GRAVITY_THR_MAX			(10)		// 静态重力最大阈值
#define GRAVITY_THR_MIN			(1)		// 静态重力最小阈值

static u8 pt_gsensor_busy = 0;      	// 忙碌标记
static u8 pt_gsensor_res = 0;      		// 测试结果
static u8 pt_gsensor_id[4] = {0};		// ID
/*****************************************************/
//				PT_SENSOR_DEBUG
/*****************************************************/
#define PT_GSENSOR_DEBUG 			1		// DEBUG
#define PT_GSENSOR_ERR_DEBUG		1		// DEBUG
#define PT_GSENSOR_DEBUG_TEST		0		// DEBUG

#if PT_GSENSOR_DEBUG
#define pt_log_i(format, ...) printf("[PT_SENSOR(INFO)]@FUNC<%s>"format"\n", __func__, ##__VA_ARGS__)
#else
#define pt_log_i(format, ...)
#endif
#if PT_GSENSOR_ERR_DEBUG
#define pt_log_e(format, ...) printf("[PT_SENSOR(ERROR)]@FUNC<%s>"format"\n", __func__, ##__VA_ARGS__)
#else
#define pt_log_e(format, ...)
#endif
//***********************************************************//
//				获取传感器数据
// @ func:		pt_xxx_data_get
// @ return: 	0 		获取不到数据
//				other	数据长度
//***********************************************************//
static int pt_gsensor_data_get(void *data_buf, int data_len)
{
    return read_gsensor_nbuf(data_buf, data_len);
}
//***********************************************************//
//				校验数据是否合法
// @ func:		pt_xxx_data_check
// @ return: 	PT_E_OK(0) 	数据属于正常值
//				other		数据异常
// @ note：		正常数据范围参考sensor厂提供的数据手册/量产测试说明
//***********************************************************//
static int pt_gsensor_data_check(void *data_buf, int data_len)
{
    int *sensor_buf = data_buf;
    u16 gravity = 0;
    u16 avg_cnt = 0;
    //计算重力均值 gravity = (x^2+y^2+z^2)^(1/2)
    for (int i = 0; i < data_len; i += 3) {
        u16 gravity_x = sensor_buf[3 * i];
        u16 gravity_y = sensor_buf[3 * i + 1];
        u16 gravity_z = sensor_buf[3 * i + 2];

        u16 gravity_tmp = root_float((float)(gravity_x * gravity_x + gravity_y * gravity_y + gravity_z * gravity_z));

        if (!gravity_tmp) {
            gravity += gravity_tmp;
            avg_cnt ++;
        }
        pt_log_i("[%d] gavityg:%d gravity_tmp%d |x,y,z|=|%d,%d,%d|\n", i, (gravity / avg_cnt), gravity_tmp, gravity_x, gravity_y, gravity_z);
    }
    u16 gravity_avg = gravity / avg_cnt;
    pt_log_i("gravity_avg:%d", gravity_avg);
    // 判断数据是否异常
    if (gravity_avg <= GRAVITY_THR_MIN) {
        pt_log_e("gravity_avg <= GRAVITY_THR_MIN\n");
        return PT_E_MOD_ERROR;
    } else if (gravity_avg >= GRAVITY_THR_MAX) {
        pt_log_e("gravity_avg >= GRAVITY_THR_MAX\n");
        return PT_E_MOD_ERROR;
    }
    pt_log_i("gravity_avg is normal");

    return PT_E_OK;

}
static int pt_gsensor_test(int priv)
{

    u32 result = PT_E_OK;


    // 获取gsensor数据
    u16 sensor_data_point = GSENSOR_CHECK_DATA_LEN;
    u16 sensor_data_len = sensor_data_point * sizeof(u16);
    u16 *sensor_data = (u16 *)zalloc(sensor_data_len);
    ASSERT(sensor_data);

    u16 read_buf_len = pt_gsensor_data_get(sensor_data, sensor_data_len);
    if (!read_buf_len) {//数据为空
        pt_log_e("GSENSOR_read_buf_len\n");
        result = PT_E_MOD_ERROR;
    } else {
        result = pt_gsensor_data_check(sensor_data, sensor_data_len);
        if (result !=  PT_E_OK) {
            pt_log_e("pt_gsensor_data_check err");
        }

    }
    if (sensor_data) {
        free(sensor_data);
        sensor_data = NULL;
    }

    pt_gsensor_busy = 0;
    pt_gsensor_res = result;

    return 0;
}

int pt_gsensor_init(void)
{
    // 获取id
    gsensor_io_ctl(SEARCH_SENSOR, pt_gsensor_id);
    // 判断id是否非法
    if (pt_gsensor_id[0] == 0x0) {
        pt_log_e("pt_gsensor_not_find\n");
        return PT_E_DEV_ID;
    } else {
        gsensor_enable();
        pt_log_e("pt_gsensor_id:%x\n", pt_gsensor_id[0]);
    }
    return PT_E_OK;
}

int pt_gsensor_start(void)
{
    if (pt_gsensor_busy) {
        return PT_E_MOD_RUN;
    }
    pt_gsensor_res = PT_E_MOD_RUN;
    int msg[3] = {0};
    msg[0] = (int)pt_gsensor_test;
    msg[1] = 1;
    msg[2] = (int)0;
    do {
        int os_err = os_taskq_post_type("app_core", Q_CALLBACK, 3, msg);
        if (os_err == OS_ERR_NONE) {
            break;
        }
        if (os_err != OS_Q_FULL) {
            pt_gsensor_res = PT_E_SYS_ERROR;
            return PT_E_SYS_ERROR;
        }
        os_time_dly(1);
    } while (1);

    pt_gsensor_busy = 1;

    return 0;
}

int pt_gsensor_stop(void)
{
    if (pt_gsensor_busy) {
        return PT_E_MOD_CANT_STOP;
    }
    gsensor_disable();
    if (pt_gsensor_res == PT_E_MOD_RUN) {
        pt_gsensor_res = PT_E_MOD_STOP_NO_END;
    }
    return 0;
}

int pt_gsensor_ioctrl(u32 order, int len, void *param)
{
    u32 result = 0;
    switch (PT_ORDER_C_GET(order)) {
    case PT_N_C_START:
        result = pt_gsensor_start();
        break;
    case PT_N_C_STOP:

        result = pt_gsensor_stop();
        break;
    case PT_N_C_GETINFO:
        u8 info[8];
        memcpy(&info[0], result, 4);
        memcpy(&info[4], pt_gsensor_id, 4);
        product_test_push_data(order, sizeof(info), info);
        return 0;
    case PT_N_C_GET_RESULT:
        result = pt_gsensor_res;
        product_test_push_data(order, 4, &result);
        return 0;
    default:

        result = PT_E_PARAM;
        break;
    }
    product_test_push_data(order, 4, &result);
    return result;
}

REGISTER_PT_MODULE(gsensor) = {
    .module = PT_M_GSENSOR,
    .attr	= PT_ATTR_SELF,
    .init	= pt_gsensor_init,
    .ioctrl	= pt_gsensor_ioctrl,
};

#if PT_GSENSOR_DEBUG_TEST
int pt_gsensor_fast_test(void)
{
    pt_gsensor_init();
    pt_gsensor_start();
    os_time_dly(10);
    pt_gsensor_test(NULL);
    pt_gsensor_stop();
    return 0;
}
#endif

#endif /* #if PT_GSENSOR_ENABLE */

