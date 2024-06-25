#include "product_test.h"
#include "pt_motor.h"

#if PT_MOTOR_ENABLE && TCFG_UI_ENABLE_MOTO

static struct pt_motor motor_info = {
    .type = PT_MOTOR_TYPE_LOOP,
    .loop_cnt = 1,
    .step = 30,
    .level_cur = 100,
    .level_max = 100,
};

extern void ui_moto_run_test(u8 percent);

static u8 pt_motor_busy = 0; // 忙碌标记
static u8 pt_motor_res = 0;      		// 测试结果

static int pt_motor_test(int priv)
{
    u32 result = PT_E_OK; // 人工判断，此处返回正常

    if (motor_info.type == PT_MOTOR_TYPE_LOOP) {
        // 循环振动
        u8 cnt = 0;
        u8 dir = 1;
        u8 level = 0;//motor_info.level_cur;
        u8 dly = motor_info.step / 10;
        if (motor_info.step % 10) {
            dly++;
        }
        while (cnt < motor_info.loop_cnt) {
            ui_moto_run_test(level);
            os_time_dly(dly);
            if (dir) { // 增强
                if (level < motor_info.level_max) {
                    level++;
                } else {
                    dir = 0;
                }
            } else { // 减弱
                if (level) {
                    level--;
                } else {
                    dir = 1;
                    cnt ++; // 一个循环
                }
            }
            if (pt_motor_busy != 1) {
                break;
            }
        }
        // 关闭循环振动
        pt_motor_busy = 0;
    } else {
        // 按振动强度固定振动
        ui_moto_run_test(motor_info.level_cur);
    }

    pt_motor_res = result;

    return 0;
}

static int pt_motor_test_stop(int priv)
{
    // 停止马达振动
    ui_moto_run_test(0);
    pt_motor_busy = 0;
    return 0;
}

int pt_motor_getinfo(struct pt_motor_result *res)
{
    res->result = 0;
    memcpy(&res->motor, &motor_info, sizeof(struct pt_motor));
    return 0;
}

int pt_motor_setinfo(struct pt_motor *mot)
{
    memcpy(&motor_info, mot, sizeof(struct pt_motor));
    return 0;
}

int pt_motor_init(void)
{
    return 0;
}

int pt_motor_start(void)
{
    if (pt_motor_busy) {
        return PT_E_MOD_RUN;
    }
    pt_motor_res = PT_E_MOD_RUN;
    int msg[3] = {0};
    msg[0] = (int)pt_motor_test;
    msg[1] = 1;
    msg[2] = (int)0;
    do {
        int os_err = os_taskq_post_type("app_core", Q_CALLBACK, 3, msg);
        if (os_err == OS_ERR_NONE) {
            break;
        }
        if (os_err != OS_Q_FULL) {
            pt_motor_res = PT_E_SYS_ERROR;
            return PT_E_SYS_ERROR;
        }
        os_time_dly(1);
    } while (1);

    pt_motor_busy = 1;

    return 0;
}

int pt_motor_stop(void)
{
    if (pt_motor_busy) {
        pt_motor_busy = 2;
        int msg[3] = {0};
        msg[0] = (int)pt_motor_test_stop;
        msg[1] = 1;
        msg[2] = (int)0;
        do {
            int os_err = os_taskq_post_type("app_core", Q_CALLBACK, 3, msg);
            if (os_err == OS_ERR_NONE) {
                break;
            }
            if (os_err != OS_Q_FULL) {
                if (pt_motor_res == PT_E_MOD_RUN) {
                    pt_motor_res = PT_E_MOD_STOP_NO_END;
                }
                return PT_E_MOD_CANT_STOP;//PT_E_SYS_ERROR;
            }
            os_time_dly(1);
        } while (1);
    }
    if (pt_motor_res == PT_E_MOD_RUN) {
        pt_motor_res = PT_E_MOD_STOP_NO_END;
    }
    return 0;
}

int pt_motor_ioctrl(u32 order, int len, void *param)
{
    u32 result = 0;
    switch (PT_ORDER_C_GET(order)) {
    case PT_N_C_START:
        result = pt_motor_start();
        break;
    case PT_N_C_STOP:
        result = pt_motor_stop();
        break;
    case PT_N_C_SETINFO:
        result = pt_motor_setinfo((struct pt_motor *)param);
        break;
    case PT_N_C_GETINFO:
        struct pt_motor_result res;
        pt_motor_getinfo(&res);
        product_test_push_data(order, sizeof(struct pt_motor_result), &res);
        return 0;
    case PT_N_C_GET_RESULT:
        result = pt_motor_res;
        break;
    default:
        result = PT_E_PARAM;
        break;
    }
    product_test_push_data(order, 4, &result);
    return result;
}


REGISTER_PT_MODULE(motor) = {
    .module = PT_M_MOTOR,
    .attr	= PT_ATTR_USER_JUDGE | PT_ATTR_ENV_OUT, // | PT_ATTR_EXTRA_DEV, // 需要外接马达
    .init	= pt_motor_init,
    .ioctrl	= pt_motor_ioctrl,
};

#endif /* #if PT_MOTOR_ENABLE */

