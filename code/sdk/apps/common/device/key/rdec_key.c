#include "key_driver.h"
#include "gpio.h"
#include "system/event.h"
#include "app_config.h"
#include "audio_config.h"
#include "rdec_key.h"
#include "asm/rdec_soft.h"
#include "timer.h"
#include "../../ui/lv_watch/comm_key/common_rdec.h"
#include "../../../../cpu/br28/ui_driver/lvgl/lvgl_main.h"

#if TCFG_RDEC_KEY_ENABLE

static volatile u8 is_key_active = 0;
static const struct rdec_platform_data *__this = NULL;

u8 rdec_get_key_value(void);

struct key_driver_para rdec_key_scan_para = {
    .scan_time 	  	  = 10,				//按键扫描频率, 单位: ms
    .last_key 		  = NO_KEY,  		//上一次get_value按键值, 初始化为NO_KEY;
    .filter_time  	  = 0,				//按键消抖延时;
    .long_time 		  = 75,  			//按键判定长按数量
    .hold_time 		  = (75 + 15),  	//按键判定HOLD数量
    .click_delay_time = 0,				//按键被抬起后等待连击延时数量
    .key_type		  = KEY_DRIVER_TYPE_RDEC,
    .get_value 		  = rdec_get_key_value,
};

extern u32 timer_get_ms(void);
u8 rdec_get_key_value(void)
{
    int i;
    s8 rdec_data;
    u8 key_value = 0;
    if (!__this->enable) {
        return NO_KEY;
    }
    for (i = 0; i < 1; i++) 
    {
        rdec_data = get_rdec_rdat(i);
        if (rdec_data < 0) {
            key_value = __this->rdec[i].key_value0;
            return key_value;
        } else if (rdec_data > 0) {
            key_value = __this->rdec[i].key_value1;
            return key_value;
        }
    }
    return NO_KEY;
}

static void rdec_key_scan(void *priv)
{
    struct key_driver_para *scan_para = (struct key_driver_para *)priv;

    // u8 key_event = 0;
    // u8 key_value = 0;

    static u8 cnt = 0;
    static u8 rdec_filter = 0;
    u8 cur_key_value = NO_KEY;
    

    if(lcd_sleep_status())
    {
        is_key_active = 0;
        return;
    }

    cur_key_value = scan_para->get_value();

    if (cur_key_value != NO_KEY) {
        is_key_active = 35;      //35*10Ms
    }else if (is_key_active) {
        is_key_active--;
    }

    if(cur_key_value != NO_KEY)
    {
        cnt = 0;

        rdec_filter++;
        rdec_filter %= 2;
        if(rdec_filter)
            goto __scan_end;

        int rdec_msg[2];
        rdec_msg[0] = ui_msg_rdec_handle;
        //1：前进 0：后退
        if(cur_key_value == TCFG_RDEC0_KEY1_VALUE)
        {
            rdec_msg[1] = Rdec_Forward;
            post_ui_msg(rdec_msg, 2);
        }else if(cur_key_value == TCFG_RDEC0_KEY0_VALUE)
        {
            rdec_msg[1] = Rdec_Backward;
            post_ui_msg(rdec_msg, 2);
        }
    }

__scan_end:
    if(cur_key_value == NO_KEY && rdec_filter)
    {
        cnt++;
        if(cnt > 30)
        {
            cnt = 0;
            rdec_filter = 0;
        }
    }

    return;
}

int rdec_key_init(const struct rdec_platform_data *rdec_key_data)
{
    __this = rdec_key_data;
    if (!__this) {
        return -EINVAL;
    }
    if (!__this->enable) {
        return KEY_NOT_SUPPORT;
    }
    printf("rdec_key_init >>>> ");
    rdec_init(rdec_key_data);

    sys_s_hi_timer_add((void *)&rdec_key_scan_para, rdec_key_scan, rdec_key_scan_para.scan_time); //注册按键扫描定时器
}

static u8 rdec_key_idle_query(void)
{
    return !is_key_active;
}

REGISTER_LP_TARGET(rdec_key_lp_target) = {
    .name = "rdec_key",
    .is_idle = rdec_key_idle_query,
};

#endif  /* #if TCFG_RDEC_KEY_ENABLE */

