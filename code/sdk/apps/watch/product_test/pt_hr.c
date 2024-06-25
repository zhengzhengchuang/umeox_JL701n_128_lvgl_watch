#include "product_test.h"
#include "pt_hr.h"
#include "hr_sensor/hrSensor_manage.h"
#if PT_HR_ENABLE
/*****************************************************/
//				PT_SENSOR_CONFIG
/*****************************************************/
#define PT_HR_MODE_HEART_RATE		(0)
#define PT_HR_MODE_SPO2				(1)

#define SPO2_CHECK_DATA_LEN 		(4*1)		// 四轴数据
#define HR_SENSOR_CHECK_DATA_LEN 	(3*1) 		// 三轴数据
#define HR_THR_IR					(1000)		// 红外阈值
#define HR_THR_RED					(1000)		// 红光阈值
#define HR_THR_GREEN				(1000)		// 绿光阈值
#define HR_THR_ALS					(270000)	// ALS参数阈值

static u8 pt_hr_busy = 0; // 忙碌标记
static u8 pt_hr_res = 0;      		// 测试结果
static u8 pt_hr_id[4] = {0};
/*****************************************************/
//				PT_SENSOR_DEBUG
/*****************************************************/
#define PT_HRSNESOR_DEBUG 			1			// DEBUG
#define PT_HRSENSOR_ERR_DEBUG		1			// DEBUG
#define PT_HRSENSOR_DEBUG_TEST		0			// DEBUG

#if PT_HRSNESOR_DEBUG
#define pt_log_i(format, ...) printf("[PT_SENSOR(INFO)]@FUNC<%s>"format"\n", __func__, ##__VA_ARGS__)
#else
#define pt_log_i(format, ...)
#endif
#if PT_HRSENSOR_ERR_DEBUG
#define pt_log_e(format, ...) printf("[PT_SENSOR(ERROR)]@FUNC<%s>"format"\n", __func__, ##__VA_ARGS__)
#else
#define pt_log_e(format, ...)
#endif

//***********************************************************//
//				获取传感器数据
// @ func:		pt_xxx_data_get
// @ return: 	len 	数据长度
//				0		获取不到数据
//***********************************************************//
static int pt_hr_data_get(void *data_buf, int data_len, int mode)
{
    if (PT_HR_MODE_HEART_RATE == mode) {
        return hr_sensor_io_ctl(HR_SENSOR_READ_DATA, data_buf);
    } else if (PT_HR_MODE_SPO2 == mode) {
        return hr_sensor_io_ctl(SPO2_SENSOR_READ_DATA, data_buf);
    }
    return 0;
}
//***********************************************************//
//				校验数据是否合法
// @ func:		pt_hr_data_check
// @ return: 	PT_E_OK(0) 	数据正常
//				other		数据异常
// @ note：		根据sensor厂提供的数据手册/量产测试说明进行修改
//***********************************************************//
static int pt_hr_data_check(void *data_buf, int data_len, int mode)
{
    int *sensor_buf = data_buf;

    switch (mode) {
    case PT_HR_MODE_HEART_RATE:
        int als_data = 0;
        int hrm_data = 0;

        for (int i = 0; i < data_len; i += 3) {
            hrm_data = sensor_buf[3 * i];
            als_data = sensor_buf[3 * i + 1];

            u8 check1 = ((hrm_data - als_data) < HR_THR_GREEN) ? 0 : 1;
            u8 check2 = (als_data < HR_THR_ALS) ? 0 : 1;
            pt_log_i("[%d]hrm:%d als:%d check_result(%d,%d)\n", i, hrm_data, als_data, check1, check2);
            //0值判断
            if ((!hrm_data) && (!als_data)) {
                return PT_E_MOD_ERROR;
            }
            //阈值判断
            if (((hrm_data - als_data) > HR_THR_GREEN) || (als_data > HR_THR_ALS)) {
                return PT_E_MOD_ERROR;
            }
        }
        break;
    case PT_HR_MODE_SPO2 :
        int ir_data = 0;
        int ir_als = 0;
        int red_data = 0;
        int red_als = 0;

        for (int i = 0; i < data_len; i += 4) {
            red_data = sensor_buf[4 * i];
            red_als	= sensor_buf[4 * i + 1];
            ir_data = sensor_buf[4 * i + 2];
            ir_als 	= sensor_buf[4 * i + 3];
            u8 check1 = (red_data - red_als < HR_THR_RED) ? 0 : 1;
            u8 check2 = (ir_data - ir_als < HR_THR_IR) ? 0 : 1;
            u8 check3 = (red_als < HR_THR_ALS) ? 0 : 1;
            u8 check4 = (ir_als < HR_THR_ALS) ? 0 : 1;
            pt_log_i("[%d]red_als(%d %d)ir_als(%d %d) check_result(%d:%d:%d:%d)\n", i, red_data, red_als, ir_data, ir_als, check1, check2, check3, check4);
            //0值判断
            if (((!red_data) && (!red_als) && (!ir_data) && (!ir_als))) {
                return PT_E_MOD_ERROR;
            }
            //阈值判断
            if (check1 + check2 + check3 + check4) {
                return PT_E_MOD_ERROR;
            }
        }
        break;
    default:
        pt_log_e("mode%d err", mode);
        return PT_E_MOD_ERROR;
    }
    return PT_E_OK;
}
//***********************************************************//
//				心率模式测试
// @ func:		pt_hr_hrmode_test
// @ return: 	PT_E_OK(0) 	数据正常
//				other		数据异常
//***********************************************************//
static int pt_hr_hrmode_test(void)
{
    int err = PT_E_OK;

    u16 sensor_data_point = HR_SENSOR_CHECK_DATA_LEN;
    u16 sensor_data_len = sizeof(u32) * sensor_data_point;
    u32 *sensor_buf = NULL;
    if (!sensor_buf) {
        sensor_buf = (u32 *)zalloc(sensor_data_len);
        ASSERT(sensor_buf);
    }

    hr_sensor_io_ctl(HR_SENSOR_ENABLE, &sensor_data_len);
    os_time_dly(6);
    // 获取hr数据
    if (!pt_hr_data_get(sensor_buf, sensor_data_len, PT_HR_MODE_HEART_RATE)) { //数据为空
        pt_log_e("SENSOR_read_buf_len\n");
        err = PT_E_MOD_ERROR;
    } else {
        // 判断数据是否正常
        err = pt_hr_data_check(sensor_buf, sensor_data_len, PT_HR_MODE_HEART_RATE);
        if (err) {
            pt_log_e("pt_data_check err");
        }
    }
    hr_sensor_io_ctl(HR_SENSOR_DISABLE, NULL);
    if (sensor_buf) {
        free(sensor_buf);
        sensor_buf = NULL;
    }

    return err;
}
//***********************************************************//
//				血氧模式测试
// @ func:		pt_hr_spo2mode_test
// @ return: 	PT_E_OK(0) 	数据正常
//				other		数据异常
//***********************************************************//
static int pt_hr_spo2mode_test(void)
{
    int err = PT_E_OK;

    u16 sensor_data_point = SPO2_CHECK_DATA_LEN;
    u16 sensor_data_len = sizeof(u32) * sensor_data_point;
    u32 *sensor_buf = NULL;

    if (!sensor_buf) {
        sensor_buf = (u32 *)zalloc(sensor_data_len);
        ASSERT(sensor_buf);
    }

    hr_sensor_io_ctl(SPO2_SENSOR_ENABLE, &sensor_data_len);
    os_time_dly(6);
    // 获取hr数据
    if (!pt_hr_data_get(sensor_buf, sensor_data_len, PT_HR_MODE_SPO2)) { //数据为空
        pt_log_e("SENSOR_read_buf_len\n");
        err = PT_E_MOD_ERROR;
    } else {
        // 判断数据是否正常
        err = pt_hr_data_check(sensor_buf, sensor_data_len, PT_HR_MODE_SPO2);
        if (err) {
            pt_log_e("pt_data_check err");
        }
    }
    hr_sensor_io_ctl(HR_SENSOR_DISABLE, NULL);
    if (sensor_buf) {
        free(sensor_buf);
        sensor_buf = NULL;
    }

    return err;
}
//***********************************************************//
//				心率传感器模块测试
// @ func:		pt_hr_test
// @ note:		根据项目需要增删
//***********************************************************//
static int pt_hr_test(int priv)
{
    u32 result = PT_E_OK;
    //心率测试
    result = pt_hr_hrmode_test();
    if (result !=  PT_E_OK) {
        pt_log_e("pt_hr_hrmode_test err");
    }
    //血氧测试
    result = pt_hr_spo2mode_test();
    if (result !=  PT_E_OK) {
        pt_log_e("pt_hr_spo2mode_test err");
    }

    pt_hr_busy = 0;
    pt_hr_res = result;

    return 0;
}

int pt_hr_init(void)
{
    hr_sensor_io_ctl(HR_SEARCH_SENSOR, &pt_hr_id);
    if (pt_hr_id[0] == 0x0) {
        pt_log_e("hrsensor_not_find\n");
        return PT_E_DEV_ID;
    } else {
        pt_log_i("hrsensor_id:%x\n", pt_hr_id[0]);
    }
    return PT_E_OK;
}

int pt_hr_start(void)
{
    if (pt_hr_busy) {
        return PT_E_MOD_RUN;
    }
    pt_hr_res = PT_E_MOD_RUN;
    int msg[3] = {0};
    msg[0] = (int)pt_hr_test;
    msg[1] = 1;
    msg[2] = (int)0;
    do {
        int os_err = os_taskq_post_type("app_core", Q_CALLBACK, 3, msg);
        if (os_err == OS_ERR_NONE) {
            break;
        }
        if (os_err != OS_Q_FULL) {
            pt_hr_res = PT_E_SYS_ERROR;
            return PT_E_SYS_ERROR;
        }
        os_time_dly(1);
    } while (1);
    pt_hr_busy = 1;

    return 0;
}

int pt_hr_stop(void)
{
    if (pt_hr_busy) {
        return PT_E_MOD_CANT_STOP;
    }
    if (pt_hr_res == PT_E_MOD_RUN) {
        pt_hr_res = PT_E_MOD_STOP_NO_END;
    }
    return 0;
}

int pt_hr_ioctrl(u32 order, int len, void *param)
{
    u32 result = 0;
    switch (PT_ORDER_C_GET(order)) {
    case PT_N_C_START:

        result = pt_hr_start();
        break;
    case PT_N_C_STOP:
        result = pt_hr_stop();
        break;
    case PT_N_C_GETINFO:

        u8 info[8];
        memcpy(&info[0], result, 4);
        memcpy(&info[4], pt_hr_id, 4);
        product_test_push_data(order, sizeof(info), info);
        return 0;

    case PT_N_C_GET_RESULT:
        result = pt_hr_res;

        break;
    default:
        result = PT_E_PARAM;
        break;
    }
    product_test_push_data(order, 4, &result);
    return result;
}

REGISTER_PT_MODULE(hr) = {
    .module = PT_M_HR,
    .attr	= PT_ATTR_SELF,
    .init	= pt_hr_init,
    .ioctrl	= pt_hr_ioctrl,
};


#if PT_HRSENSOR_DEBUG_TEST
int pt_hrsensor_fast_test(void)
{
    pt_hr_init();
    pt_hr_start();
    pt_hr_test(NULL);
    pt_hr_stop();
    return 0;
}
#endif
#endif /* #if PT_HR_ENABLE */

