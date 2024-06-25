/*********************************************************************************************
    *   Filename        : main.c

    *   Description     : P11系统Main循环

    *   Author          : MoZhiYe

    *   Email           : mozhiye@zh-jieli.com

    *   Last modifiled  : 2021-06-23 09:00

    *   Copyright:(c)JIELI  2021-2029  @ , All Rights Reserved.
*********************************************************************************************/
#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(".app.bss")
#pragma data_seg(".app.data")
#pragma const_seg(".app.const")
#pragma code_seg(".app.text")
#pragma str_literal_override(".app.text")
#endif /* #ifdef SUPPORT_MS_EXTENSIONS */

#include "includes.h"
#include "uart.h"
#include "debug.h"
#include "app_config.h"
#include "gpio.h"
#include "ram_share.h"

#include "msg.h"
#include "timer.h"
#include "sensor_api.h"
#include "hrSensor_manage.h"

#include "bank_switch.h"

#include "gSensor_manage.h"
#include "lp_timer_api.h"

#include "usr_timer.h"

/* #pragma bss_seg(".main_bss") */
/* #pragma const_seg(".main_const") */
/* #pragma code_seg(".main_code") */
/*  */


const struct bank_info bank_info_table[] = {
    {BANK_DEMO_TAG, BANK_CODE_DEMO_BANK_MAX, (u32)overlay_demo_begin},
    {BANK_SENSOR_TAG,  BANK_CODE_SENSOR_BANK_MAX, (u32)overlay_sensor_begin},
    {NULL, 0}
};



#if 0
static void test_get_cur_500ms_tick()
{
    printf("\n tick \n");

}
LOOP_DETECT_REGISTER(NULL, 500, test_get_cur_500ms_tick);
#endif




void user_main_init()
{
#if CONFIG_SENSOR_ALG_ENABLE
    user_p11_p2m_post_init();
    task_message_init();
    timer_tick_init();
    sensor_init();
    extern int vir_p11_i2c_init();
    vir_p11_i2c_init();
#endif
    extern void user_lptime_init(void);
    user_lptime_init();
}



extern void iic_host_check();



extern int sensor_hub_gsensor_enable(u8 on);

extern int sensor_hub_gsensor_check_len();

extern int sensor_hub_gsensor_read(u8 *data, u16 len);

extern int sensor_hub_hrsensor_check_len();

extern int sensor_hub_hrsensor_read(u8 *data, u16 len);

extern int sensor_hub_hrsensor_enable(u8 on);


extern int sensor_hub_spo2sensor_enable(u8 on);


extern int gravity_sensor_write(u8 w_chip_id, u8 *data, u8 length);
extern int _gravity_sensor_get_ndata(u8 r_chip_id, u8 register_address, u8 *buf, int data_len);



#if CONFIG_SENSOR_ALG_ENABLE

volatile u8 vir_iic_m2p_wbuf[256];

int vir_p11_i2c_init()
{
    int msg[2];
    msg[0] = sizeof(vir_iic_m2p_wbuf);
    msg[1] = vir_iic_m2p_wbuf;
    user_p11_post_to_main_system(P11_SYS_IIC_INIT_SYNC, sizeof(msg), (u8 *)msg, 1); //配置主核共享空间
    return 0;
}

//iic 地址是7bit地址
int vir_p11_i2c_write_regs(u8 dev, u8 address,
                           u8 *data, size_t length)
{
    int ret  = gravity_sensor_write(address << 1, data, length);
    if (ret != length) {
        return 0;
    }
    return length;
}



//iic 地址是7bit地址
int vir_p11_i2c_read_regs(u8 dev, u16 address, u8 reg,
                          u8 *data, u16 length)
{
    int ret;
    ret = _gravity_sensor_get_ndata(((address << 1) | 1), reg, data, length);
    return ret;
}

#endif



int user_run_loop()
{
    int msg = NO_MSG;
    int error = MSG_NO_ERROR;
    int data = 0;
#if CONFIG_SENSOR_ALG_ENABLE
__again:
    while (1) {
        error = task_get_msg(0, 1, &msg);
        if (NO_MSG == msg) {
            return 0;
        }
        switch (msg) {
        case P11_SYS_KICK:
            iic_host_check();
            break;
        case P11_SYS_IIC_R_SYNC:
            ASSERT(vir_iic_m2p_wbuf[0] & BIT(7));
            ASSERT((vir_iic_m2p_wbuf[0] & 3) == 1); //暂时读支持u8 寄存器
            vir_iic_m2p_wbuf[1] = vir_p11_i2c_read_regs(vir_iic_m2p_wbuf[1], vir_iic_m2p_wbuf[2], vir_iic_m2p_wbuf[4], (u8 *)&vir_iic_m2p_wbuf[5], vir_iic_m2p_wbuf[3]);
            vir_iic_m2p_wbuf[0] = 0;
            break;
        case P11_SYS_IIC_W_SYNC:
            ASSERT(vir_iic_m2p_wbuf[0] == BIT(6));
            vir_iic_m2p_wbuf[1] = vir_p11_i2c_write_regs(vir_iic_m2p_wbuf[1], vir_iic_m2p_wbuf[2], (u8 *)&vir_iic_m2p_wbuf[4], vir_iic_m2p_wbuf[3]);
            vir_iic_m2p_wbuf[0] = 0;
            break;


        case P11_SYS_GSENSOR_START:
            printf("P11_SYS_GSENSOR_START\n");
            sensor_hub_gsensor_enable(1);
            break;
        case P11_SYS_GSENSOR_STOP:
            printf("P11_SYS_GSENSOR_STOP\n");
            sensor_hub_gsensor_enable(0);
            break;
        case P11_SYS_HRSENSOR_START:
            printf("P11_SYS_HRSENSOR_START\n");
            sensor_hub_hrsensor_enable(1);
            break;
        case P11_SYS_HRSENSOR_STOP:
            printf("P11_SYS_HRSENSOR_STOP\n");
            sensor_hub_hrsensor_enable(0);
            break;
        case P11_SYS_SPO2_START:
            printf("P11_SYS_SPO2SENSOR_START\n");
            sensor_hub_spo2sensor_enable(1);
            break;
        case P11_SYS_SPO2_STOP:
            printf("P11_SYS_SPO2SENSOR_STOP\n");
            sensor_hub_spo2sensor_enable(0);
            break;
            /* case P11_SYS_GSENSOR_SEARCH: */
            /* case P11_SYS_HRSENSOR_SEARCH:	 */
            /* printf("P11_SYS_SENSOR_SEARCH\n"); */
            /* break; */
        }
    }
    goto __again;
#endif
    return 0;
}



/*========================================================================
 * @brief: p11主循环
 *
 * @param:
 * @return:
 ========================================================================*/
static void p11_run_loop(void)
{
    while (1) {
        p11_power_hw_manage();
        user_run_loop();
    }
}
/*========================================================================
 * @brief: Main函数
 *
 * @param:
 * @return:
 ========================================================================*/

#include "iic_soft.h"

static void usr_timer_test(void *priv)
{
    printf("fun:%s, %d\n", __FUNCTION__, __LINE__);
}

int main(void)
{
    local_irq_disable();

#if CONFIG_UART_DEBUG_ENABLE
    debug_uart_init(IO_PORTB_09);
#endif /* #if CONFIG_UART_DEBUG_ENABLE */

    printf("\n============ Hello BR28 P11 ============\n");

    p11_q32s(0)->PMU_CON |= BIT(2); //P11 WKUP EN
    printf("p11_q32s(0)->PMU_CON = 0x%x\n", p11_q32s(0)->PMU_CON);

    debug_init(); //异常检测初始化

#if CONFIG_SENSOR_ALG_ENABLE
    soft_iic_init(0);//some sensor need wait iic idle little ms
#endif
    usr_timer_init();

    power_init();

    iovdd2_bypass_iovdd(1);

    local_irq_enable();
    printf("[boot]Interrupt ready \n");

    mdelay(100);

    //usr_timer_add(NULL, usr_timer_test, 100, 0);

    user_main_init();
    p11_run_loop();

}







