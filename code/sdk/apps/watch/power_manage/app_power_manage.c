#include "system/includes.h"
#include "app_power_manage.h"
#include "app_main.h"
#include "app_config.h"
#include "app_action.h"
#include "asm/charge.h"
#include "ui_manage.h"
#include "tone_player.h"
#include "asm/adc_api.h"
#include "btstack/avctp_user.h"
#include "user_cfg.h"
#include "bt.h"
#include "asm/charge.h"
#include "ui/ui_api.h"
#include "../../include/key_event_deal.h"

#if TCFG_USER_TWS_ENABLE
#include "bt_tws.h"
#endif

#define LOG_TAG_CONST       APP_POWER
#define LOG_TAG             "[APP_POWER]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"

#define TCFG_BATTERY_POWER_MANAGE_ENABLE    ENABLE_THIS_MOUDLE
#define VBAT_DETECT_CNT     3   //连续3次检测

enum {
    VBAT_NORMAL = 0,
    VBAT_WARNING,
    VBAT_LOWPOWER,
} VBAT_STATUS;

static int vbat_slow_timer = 0;
static int vbat_fast_timer = 0;
static int lowpower_timer = 0;
static u8 old_battery_level = 9;
static u16 bat_val = 0;
static u16 avr_bat_val = 0;
static u8 avr_bat_percent_val = 0;
static volatile u8 cur_battery_level = 0;
static u16 battery_full_value = 0;
static u8 tws_sibling_bat_level = 0xff;
static u8 tws_sibling_bat_percent_level = 0xff;
static u8 cur_bat_st = VBAT_NORMAL;



static int vm_vbat_value=0;
void write_vm_vbat_value(int vm_value){
    if(vm_value<0){
        vm_value=0;
    }else if(vm_value>100){
        vm_value=100;
    }
    syscfg_write(VM_BAT_PRESENT, &vm_value, sizeof(vm_value));
}

void read_vm_vbat_value()
{
    int vm_value=0;
    int ret = syscfg_read(VM_BAT_PRESENT, &vm_value, sizeof(vm_value));
    if(ret > 0) 
    {
        if(vm_value<0){
            vm_value=0;
        }
        else if(vm_value>100){
            vm_value=100;
        }
        vm_vbat_value = vm_value;
        //log_sylon(">>>>>>>>> VM init val is %d", vm_vbat_value);
    } 
}


/************************************************************************************
1、需要在user_cfg_id.h添加保存电量的ID
     #define     VM_BAT_PRESENT          xx id范围[1 ~ 60] 
2、BATTERY_CAPACITY 宏定义是电池电量设定
3、open_circuit_voltage（开路电压数组）capacity（电池容量）数组值根据电池曲线值，修改采样（目前是百分之五采样一次） 
4、BATTERY_RESISTOR 宏定义是电池内阻，根据实际电池内阻定 
5、vbat_calculate_loop() 此函数传充电前后的差值，估算电流值，计算vbat的线性值
time:2023 05 24 <<<<<<<<<<<<<<<<<<<
************************************************************************************/
const float charge_k = 0.6f;//充电电流系数。需要测量实际电流大小与打印值的差异
#define BATTERY_CAPACITY    390.0f // 电池总容量，单位mAh
static float BATTERY_RESISTOR  =  0.360f;   //电池内阻

#define BATTERY_MAX_I	+300.0f	//最大充电电流
#define BATTERY_MIN_I	-200.0f	//最大放电电流
#define BATTERY_INVAID_I		(50.0f)	//电流校验失效电流
#define BATTERY_CHARGE_SMALL_I	(30.0f)	//充电最小电流

// 实际应用的电池曲线
#define NEW_NUM_POINTS 20
#define abs(x) ((x)>0?(x):-(x))

const float capacity[NEW_NUM_POINTS] = 
{
    1.0000, 0.9500, 0.9000, 0.8501, 0.8000, 0.7502, 0.7003, 0.6316, 0.5789, 0.5263,
    0.4737, 0.4211, 0.3684, 0.3158, 0.2632, 0.2105, 0.1579, 0.1053, 0.0526, 0.0000,
};

const float open_circuit_voltage[NEW_NUM_POINTS] = 
{
    4.3477, 4.2916, 4.2283, 4.1793, 4.1322, 4.0776, 4.0398, 3.9837, 3.9428, 3.9012,
    3.8668, 3.8423, 3.8191, 3.8014, 3.7981, 3.7614, 3.7406, 3.7183, 3.6914, 3.4400,
};

// const float capacity[NEW_NUM_POINTS] = {
//  1.0000, 0.9474, 0.8947, 0.8421, 0.7895, 0.7368, 0.6842, 0.6316, 0.5789, 0.5263,
//  0.4737, 0.4211, 0.3684, 0.3158, 0.2632, 0.2105, 0.1579, 0.1053, 0.0526, 0.0000,
// };

// const float open_circuit_voltage[NEW_NUM_POINTS] = {
//  4.2000, 4.1500, 4.1000, 4.0500, 4.0000, 3.9500, 3.9000, 3.8500, 3.8000, 3.7500,
//  3.7000, 3.6500, 3.6000, 3.5500, 3.5000, 3.4500, 3.4000, 3.4000, 3.4000, 3.4000,
// };

#if 1
static int is_sleep = 0;
void sleep_check()
{
    int value;
    struct lp_target *p;
    list_for_each_lp_target(p){
        value = 1 & (p->is_idle());
    }
    if(value){
        ++is_sleep;
    }   
    else{
        is_sleep = 0; 
    }
}
#endif

/* 线性差值算法 */
/*
线性差值:即根据给定的一组数据点，通过线性函数来估算在某个输入值处的输出值。具体来说，
这个函数接受四个参数：输入值x、输出值y、数据点的x坐标数组x_values、数据点的y坐标数组y_values，
以及数据点的数量num_points。函数首先遍历所有的数据点，找到输入值x所在的区间，然后根据该区间
内的两个数据点，计算出线性函数的斜率和截距，从而得到在输入值x处的输出值y。如果输入值x超出了
数据点的范围，则函数会输出一个错误信息，并将输出值y设置为最接近输入值x的数据点的输出值。需要
注意的是，由于浮点数的精度问题，当输入值x非常接近数据点的边界时，可能会出现精度丢失的情况，
因此函数在边界处进行了特殊处理。
*/
void linear_interpolation(float x, float *y, float *x_values, float *y_values, int num_points) 
{
    for (int i = 0; i < num_points - 1; i++) 
    {
        if (x <= x_values[i] && x >= x_values[i+1]) 
        {
            float x1 = x_values[i];
            float y1 = y_values[i];
            float x2 = x_values[i+1];
            float y2 = y_values[i+1];
            if(x2-x1==0)
            {
                printf("%s div0 error",__func__);
                return;
            }
            *y = y1 + ((y2 - y1) / (x2 - x1)) * (x - x1);
            break;
        }
    }

    //FLOAT边界精度丢失处理
    if(x<x_values[num_points-1])
    {
        *y = y_values[num_points-1];
    }else if(x>x_values[0]){
        *y = y_values[0];
    }
}

// 实时电池虚拟开路电压函数 OCV(SOC)
float OCV(float soc) 
{
    float open_circuit_voltage_value=0.0;
    linear_interpolation(soc, &open_circuit_voltage_value, capacity, open_circuit_voltage, NEW_NUM_POINTS);
    return open_circuit_voltage_value;
}

float SOC(float ocv){
    float soc;
    linear_interpolation(ocv, &soc, open_circuit_voltage, capacity, NEW_NUM_POINTS);
    return soc; 

}

// 预测电流函数 I(real_vbat_value, SOC)
float predicted_current(float real_vbat_value, float soc) {

    float ocv = OCV(soc);
    printf("%s:ocv = %f\n", __func__, ocv);
    //printf("real_vbat_value:%f soc:%f", real_vbat_value,soc);
    return (real_vbat_value - ocv) / BATTERY_RESISTOR * 1000; // 10 is battery resistor
}

#define Vbat_Log_En (0)
static float current_soc;
void vbat_calculate_loop() 
{
    printf("____%s\n", __func__);

    // 模拟每 8 秒更新一次电池电压 real_vbat_value
    int real_vbat_value;
    float current_vbat;
    // 空载状态下，直接根据 SOC-OCV 反向函数计算 SOC
    current_vbat = (float)(adc_get_voltage(AD_CH_VBAT) * 4)/1000.0f;
    printf("current_vbat = %f\n", current_vbat);
#if Vbat_Log_En
    printf("is_sleep = %d 实际检测出vbat值>>>>>>>>>>>> current_vbat = %f,current_soc = %.2f",is_sleep,current_vbat,current_soc);
#endif
    if(is_sleep>2000 && !LVCMP_DET_GET() && 0) { //长时间休眠且非充电状态直接校准 is_sleep累加作为休眠时间设置
        if (current_vbat >= OCV(1.0)) {
            current_soc = 1.0;
        } else if (current_vbat <= OCV(0.0)) {
            current_soc = 0.0;
        } else {
            current_soc = SOC(current_vbat);
        }
    } else 
    {
		float I = predicted_current(current_vbat, current_soc);
        printf("%s:I = %f\n", __func__, I);
		if(I < BATTERY_MIN_I){
			printf(">>>>>>BATTERY_MIN_I");
			I = BATTERY_MIN_I;
		} else if (I > BATTERY_MAX_I){
			printf(">>>>>>BATTERY_MAX_I");
			I = BATTERY_MAX_I;
		}
        // 根据电池状态进行放电或充电运算
        if (!LVCMP_DET_GET()) { // 放电状态        
            current_soc += I * 8 / 3600 / BATTERY_CAPACITY; // 积分运算，单位转换为 mAh
            #if Vbat_Log_En
            printf("Down 放电状态  %.2f current_soc = %.2f \n", I,current_soc);//电流值
            #endif
        } else { // 充电状态
			if(I < BATTERY_CHARGE_SMALL_I){
				I = BATTERY_CHARGE_SMALL_I;
			}
            current_soc += I * charge_k * 8 / 3600 / BATTERY_CAPACITY; // 积分运算，单位转换为 mAh
            #if Vbat_Log_En
            printf("Up 充电状态 Current I: %.2f current_soc = %.2f \n", I,current_soc);//电流值
            #endif
        }
    }

    // 防止 SOC 值超出范围
    if (current_soc < 0.0) {
        current_soc = 0.0;
    } else if (current_soc > 1.0/*BATTERY_CAPACITY*/) {
        current_soc = 1.0/*BATTERY_CAPACITY*/;
    }
    
    // 输出当前 SOC 值
    real_vbat_value = (int)(current_soc*100);
    #if Vbat_Log_En
    printf("经过算法后的curr_present_k = %d,vm_vbat_value  = %dcurrent_soc = %.2f",real_vbat_value,vm_vbat_value,current_soc);
    #endif
    
     // 防止电压回弹
     u8 update = 0;
    if(get_charge_online_flag()) {
        //充电过程中电量下降使用之前电量
        if(real_vbat_value <= vm_vbat_value) {  
            #if Vbat_Log_En
            printf("充电过程中电量下降使用之前电量");
            #endif
            // real_vbat_value = vm_vbat_value;
        } else {
            update = 1;
        }
    } else {
        //未充电过程中电量上升使用之前电量
        if(real_vbat_value >= vm_vbat_value) {  
            #if Vbat_Log_En
            printf("未充电过程中电量上升使用之前电量");
            #endif
            // real_vbat_value= vm_vbat_value; 
        } else {
            update = 1;
        }
    }

    if(update){
        #if Vbat_Log_En
	    printf("储存到vm的电量值 vm_vbat_value is %d \n",vm_vbat_value);
        #endif
        vm_vbat_value = real_vbat_value;
		int argv[3];
		argv[0] = (int)write_vm_vbat_value;
		argv[1] = 1;
		argv[2] = vm_vbat_value;
		int ret = os_taskq_post_type("app_core", Q_CALLBACK, 3, argv);
		if (ret) {
			printf("vbat change post ret:%d \n", ret);
		}
    }
    #if Vbat_Log_En
    printf("sylon debug:>>>>>>>>>real_vbat_value = %d 实际显示电量:vm_vbat_value=%d is_charge=%d",real_vbat_value,vm_vbat_value, get_charge_online_flag());
    #endif
    avr_bat_percent_val = vm_vbat_value; //real_vbat_value;
    printf("Current SOC: %d\n", avr_bat_percent_val);
}
/************************************************************************************
time: 2023 05 24 >>>>>>>>>>>>>>>>>>>
************************************************************************************/

#if TCFG_BATTERY_POWER_MANAGE_ENABLE
u8 get_vbat_averge_percent(void);
u8 battery_precent_lel = 100;
#endif

void vbat_check(void *priv);
void sys_enter_soft_poweroff(void *priv);
void clr_wdt(void);
void power_event_to_user(u8 event);

u8 get_tws_sibling_bat_level(void)
{
#if TCFG_USER_TWS_ENABLE
    /* log_info("***********get_tws_sibling_bat_level: %2x", tws_sibling_bat_percent_level); */
    return tws_sibling_bat_level & 0x7f;
#endif
    return 0xff;
}

u8 get_tws_sibling_bat_persent(void)
{
#if TCFG_USER_TWS_ENABLE
    /* log_info("***********get_tws_sibling_bat_level: %2x", tws_sibling_bat_percent_level); */
    return tws_sibling_bat_percent_level;
#endif
    return 0xff;
}

void app_power_set_tws_sibling_bat_level(u8 vbat, u8 percent)
{
#if TCFG_USER_TWS_ENABLE
    tws_sibling_bat_level = vbat;
    tws_sibling_bat_percent_level = percent;
    /*
     ** 发出电量同步事件进行进一步处理
     **/
    power_event_to_user(POWER_EVENT_SYNC_TWS_VBAT_LEVEL);

    log_info("set_sibling_bat_level: %d, %d\n", vbat, percent);
#endif
}


static void set_tws_sibling_bat_level(void *_data, u16 len, bool rx)
{
    u8 *data = (u8 *)_data;

    if (rx) {
        app_power_set_tws_sibling_bat_level(data[0], data[1]);
    }
}

#if TCFG_USER_TWS_ENABLE
REGISTER_TWS_FUNC_STUB(vbat_sync_stub) = {
    .func_id = TWS_FUNC_ID_VBAT_SYNC,
    .func    = set_tws_sibling_bat_level,
};
#endif

void tws_sync_bat_level(void)
{
#if (TCFG_USER_TWS_ENABLE && BT_SUPPORT_DISPLAY_BAT)
    u8 battery_level = cur_battery_level;
#if CONFIG_DISPLAY_DETAIL_BAT
    u8 percent_level = get_vbat_percent();
#else
    u8 percent_level = get_self_battery_level() * 10 + 10;
#endif
    if (get_charge_online_flag()) {
        percent_level |= BIT(7);
    }

    u8 data[2];
    data[0] = battery_level;
    data[1] = percent_level;
    tws_api_send_data_to_sibling(data, 2, TWS_FUNC_ID_VBAT_SYNC);

    log_info("tws_sync_bat_level: %d,%d\n", battery_level, percent_level);
#endif
}

void power_event_to_user(u8 event)
{
    struct sys_event e;
    e.type = SYS_DEVICE_EVENT;
    e.arg  = (void *)DEVICE_EVENT_FROM_POWER;
    e.u.dev.event = event;
    e.u.dev.value = 0;
    sys_event_notify(&e);
}

int app_power_event_handler(struct device_event *dev)
{
    int ret = false;

#if(TCFG_SYS_LVD_EN == 1)
    switch (dev->event) {
    case POWER_EVENT_POWER_NORMAL:
        ui_update_status(STATUS_EXIT_LOWPOWER);
        break;
    case POWER_EVENT_POWER_WARNING:
        /* ui_update_status(STATUS_LOWPOWER); */
        if (lowpower_timer == 0) {
            lowpower_timer = sys_timer_add((void *)POWER_EVENT_POWER_WARNING, (void (*)(void *))power_event_to_user, LOW_POWER_WARN_TIME);
        }
        break;
    case POWER_EVENT_POWER_LOW:
        r_printf(" POWER_EVENT_POWER_LOW");
        vbat_timer_delete();
        if (lowpower_timer) {
            sys_timer_del(lowpower_timer);
            lowpower_timer = 0 ;
        }
#if TCFG_APP_BT_EN
#if (RCSP_ADV_EN)
        extern u8 adv_tws_both_in_charge_box(u8 type);
        adv_tws_both_in_charge_box(1);
#endif
        soft_poweroff_mode(1);  ///强制关机
        sys_enter_soft_poweroff(NULL);
#else
        void app_entry_idle() ;
        app_entry_idle() ;
#endif
        break;
#if TCFG_APP_BT_EN
#if TCFG_USER_TWS_ENABLE
    case POWER_EVENT_SYNC_TWS_VBAT_LEVEL:
        if (tws_api_get_role() == TWS_ROLE_MASTER) {
            user_send_cmd_prepare(USER_CTRL_HFP_CMD_UPDATE_BATTARY, 0, NULL);
        }
        break;
#endif

    case POWER_EVENT_POWER_CHANGE:
        /* log_info("POWER_EVENT_POWER_CHANGE\n"); */
#if TCFG_USER_TWS_ENABLE
        if (tws_api_get_tws_state() & TWS_STA_SIBLING_CONNECTED) {
            if (tws_api_get_tws_state()&TWS_STA_ESCO_OPEN) {
                break;
            }
            tws_sync_bat_level();
        }
#endif
        UI_MSG_POST("bat_status:event=%4", dev->event);
        user_send_cmd_prepare(USER_CTRL_HFP_CMD_UPDATE_BATTARY, 0, NULL);
#endif
        break;
    case POWER_EVENT_POWER_CHARGE:
        UI_MSG_POST("bat_status:event=%4", dev->event);
        if (lowpower_timer) {
            sys_timer_del(lowpower_timer);
            lowpower_timer = 0 ;
        }
        break;
    default:
        break;
    }
#endif

    return ret;
}

#define SAMPLE_POINT      20
#define FILTER_CNT        5
#define abs(x)  ((x)>0?(x):-(x))
u16 vbat_fifo[SAMPLE_POINT];
u8 vbat_percent_fifo[SAMPLE_POINT];

static void vbat_averge_filter(u16 val)
{
    u8 i = 0, k = 0;
    u32 sum = 0;
    int val_diff = avr_bat_val - val;
    static u8 filter_cnt = 0;

    //电量值与平均值超过0.05v，默认为无效数据
    if (abs(val_diff) > 5 && (avr_bat_val != 0)) {
        filter_cnt++;
        if (filter_cnt < FILTER_CNT) {
            return;
        }
        /* printf("filter_cnt >= FILTER_CNT"); */
    }

    k = SAMPLE_POINT;
    filter_cnt = 0;

    if (vbat_fifo[0] == 0) {
        for (i = 0; i < k; i++) {
            vbat_fifo[i] = val;
        }
    } else {
        for (i = 0; i < k - 1; i++) {
            vbat_fifo[i] = vbat_fifo[i + 1];
        }
        vbat_fifo[k - 1] = val;
    }

    for (i = 0; i < k; i++) {
        sum += vbat_fifo[i];
    }

    avr_bat_val = sum / k;
    //log_sylon("avr_bat_val:%d %d", avr_bat_val, val); 
}

u16 get_vbat_level(void)
{
    return (adc_get_voltage(AD_CH_VBAT) * 4 / 10);
}

__attribute__((weak)) u8 remap_calculate_vbat_percent(u16 bat_val)
{
    return 0;
}

u16 get_vbat_value(void)
{
    return bat_val;
}

u8 get_vbat_percent(void)
{
   printf("get_vbat_percent,bat_val %d\n" ,bat_val);
   
#if TCFG_BATTERY_POWER_MANAGE_ENABLE   
    if (bat_val <= app_var.poweroff_tone_v)
    {
        return 0;
    }
    return get_vbat_averge_percent();
#endif

    u16 tmp_bat_val;
    u16 bat_val = avr_bat_val;//get_vbat_level();
    if (battery_full_value == 0) {
#if TCFG_CHARGE_ENABLE
        battery_full_value = (get_charge_full_value() - 100) / 10; //防止部分电池充不了这么高电量，充满显示未满的情况
#else
        battery_full_value = 420;
#endif
    }

    if (bat_val <= app_var.poweroff_tone_v) {
        return 0;
    }

    tmp_bat_val = remap_calculate_vbat_percent(bat_val);
    if (!tmp_bat_val) {
        tmp_bat_val = ((u32)bat_val - app_var.poweroff_tone_v) * 100 / (battery_full_value - app_var.poweroff_tone_v);
        if (tmp_bat_val > 100) {
            tmp_bat_val = 100;
        }
    }
    return (u8)tmp_bat_val;
}

u8 get_vbat_averge_percent(void)
{
    return avr_bat_percent_val;
}

bool get_vbat_need_shutdown(void)
{
    if ((bat_val <= LOW_POWER_SHUTDOWN) || adc_check_vbat_lowpower()) {
        return TRUE;
    }
    return FALSE;
}

//将当前电量转换为1~9级发送给手机同步电量
u8  battery_value_to_phone_level(u16 bat_val)
{
    u8  battery_level = 0;
    u8 vbat_percent = get_vbat_percent();

    if (vbat_percent < 5) { //小于5%电量等级为0，显示10%
        return 0;
    }

    battery_level = (vbat_percent - 5) / 10;

    return battery_level;
}

//获取自身的电量
u8  get_self_battery_level(void)
{
    return cur_battery_level;
}

u8  get_cur_battery_level(void)
{
    u8 bat_lev = tws_sibling_bat_level & (~BIT(7));
#if TCFG_USER_TWS_ENABLE
    if (bat_lev == 0x7f) {
        return cur_battery_level;
    }

#if (CONFIG_DISPLAY_TWS_BAT_TYPE == CONFIG_DISPLAY_TWS_BAT_LOWER)
    return cur_battery_level < bat_lev ? cur_battery_level : bat_lev;
#elif (CONFIG_DISPLAY_TWS_BAT_TYPE == CONFIG_DISPLAY_TWS_BAT_HIGHER)
    return cur_battery_level < bat_lev ? bat_lev : cur_battery_level;
#elif (CONFIG_DISPLAY_TWS_BAT_TYPE == CONFIG_DISPLAY_TWS_BAT_LEFT)
    return tws_api_get_local_channel() == 'L' ? cur_battery_level : bat_lev;
#elif (CONFIG_DISPLAY_TWS_BAT_TYPE == CONFIG_DISPLAY_TWS_BAT_RIGHT)
    return tws_api_get_local_channel() == 'R' ? cur_battery_level : bat_lev;
#else
    return cur_battery_level;
#endif //END CONFIG_DISPLAY_TWS_BAT_TYPE

#else  //TCFG_USER_TWS_ENABLE == 0
    return cur_battery_level;
#endif
}

void vbat_check_slow(void *priv)
{
    if (vbat_fast_timer == 0) {
        vbat_fast_timer = usr_timer_add(NULL, vbat_check, 10, 1);
    }
    if (get_charge_online_flag()) {
        sys_timer_modify(vbat_slow_timer, 30 * 1000);
    } else {
        sys_timer_modify(vbat_slow_timer, 15 * 1000);
    }
}
void get_real_vbat()
{
    current_soc = SOC((float)(adc_get_voltage(AD_CH_VBAT) * 4)/1000.0);
    vm_vbat_value = (int)(current_soc*100.0);
    write_vm_vbat_value(vm_vbat_value);
    vbat_calculate_loop();
    printf("current_soc = %.2f vm_vbat_value = %d  ",current_soc,vm_vbat_value);
}
extern u16 vbat_voltage_array[17];
void vbat_check_init(void)
{  
    printf("%s\n", __func__);

#if TCFG_BATTERY_POWER_MANAGE_ENABLE   
    read_vm_vbat_value();
    printf("vm_vbat_value = %d\n", vm_vbat_value);

	if(!get_charge_online_flag()) 
    {
		current_soc = (float)vm_vbat_value/100.0f;//1.0f
		float current_vbat;
		// 空载状态下，直接根据 SOC-OCV 反向函数计算 SOC
		current_vbat = (float)(adc_get_voltage(AD_CH_VBAT) * 4)/1000.0f;
		float I = predicted_current(current_vbat, current_soc);//4.12V  80
		printf("VBAT INIT I = %f", I);
		if(abs(I) > BATTERY_INVAID_I)
        {
			printf("BATTERY_INVAID_I REREAD VBAT P!!!!!!!");
			vm_vbat_value = 0;	//当前电流不可靠，重新读取
		}
    }
	
	printf("vm_vbat_value = %d ",vm_vbat_value);

    if((is_reset_source(P33_VDDIO_LVD_RST) | is_reset_source(P33_VDDIO_POR_RST)) || (vm_vbat_value == 0))
    {
        if(get_charge_online_flag()) 
        {
            memset(vbat_voltage_array, 0x0, sizeof(vbat_voltage_array));//清除adc数组里面的异常值
            sys_timeout_add(NULL, get_real_vbat, 1000);//电池进入保护状态或者第一次使用电池时,等待1s,电池激活后再获取电压值
        } else {
            get_real_vbat();
        }
    } else {
        current_soc = (float)vm_vbat_value/100.0;
        //log_sylon("current_soc = %.2f ",current_soc);
    }

    //usr_timer_add(NULL, sleep_check, 10, 0);//检查是否进入低功耗
    vbat_calculate_loop();
    sys_timer_add(NULL, vbat_calculate_loop, 8000);

#if 0
	if (vbat_slow_timer == 0) {
        vbat_slow_timer = sys_timer_add(NULL, vbat_check_slow, 10 * 1000);
    } else {
        sys_timer_modify(vbat_slow_timer, 10 * 1000);
    }
#endif
#else

    if (vbat_slow_timer == 0) {
        vbat_slow_timer = sys_timer_add(NULL, vbat_check_slow, 30 * 1000);
    } else {
        sys_timer_modify(vbat_slow_timer, 30 * 1000);
    }
#endif

#if 0
    if(vbat_fast_timer == 0) 
    {
        vbat_fast_timer = usr_timer_add(NULL, vbat_check, 10, 1);
    }
#endif
}

void vbat_timer_delete(void)
{
    if (vbat_slow_timer) {
        sys_timer_del(vbat_slow_timer);
        vbat_slow_timer = 0;
    }
    if (vbat_fast_timer) {
        usr_timer_del(vbat_fast_timer);
        vbat_fast_timer = 0;
    }
}

void vbat_check(void *priv)
{
    static u8 unit_cnt = 0;
    static u8 low_warn_cnt = 0;
    static u8 low_off_cnt = 0;
    static u8 low_voice_cnt = 0;
    static u8 low_power_cnt = 0;
    static u8 power_normal_cnt = 0;
    static u8 charge_online_flag = 0;
    static u8 low_voice_first_flag = 1;//进入低电后先提醒一次
    if (!bat_val) {
        bat_val = get_vbat_level();
    } else {
        bat_val = (get_vbat_level() + bat_val) / 2;             //32s平均电压
    }
    cur_battery_level = battery_value_to_phone_level(bat_val);
    vbat_averge_filter(bat_val);    //bat_val计算出平均电压avr_bat_val
	////log_sylon(">>>>>>>>>>>>>> bat_val:%d, cur_battery_level:%d , adc_check_vbat_lowpower:%d, avr_bat_val = %d\n", bat_val, cur_battery_level, adc_check_vbat_lowpower(),avr_bat_val);
    unit_cnt++;

    /* if (bat_val < LOW_POWER_OFF_VAL) { */
    if (adc_check_vbat_lowpower() || (bat_val <= app_var.poweroff_tone_v)) {
        low_off_cnt++;
    }
    /* if (bat_val < LOW_POWER_WARN_VAL) { */
    if (bat_val <= app_var.warning_tone_v) {
        low_warn_cnt++;
    }

    /* log_info("unit_cnt:%d\n", unit_cnt); */

    if (unit_cnt >= VBAT_DETECT_CNT) 
    {

        if (get_charge_online_flag() == 0) {
            if (low_off_cnt > (VBAT_DETECT_CNT / 2)) { //低电关机
                low_power_cnt++;
                low_voice_cnt = 0;
                power_normal_cnt = 0;
                cur_bat_st = VBAT_LOWPOWER;
                if (low_power_cnt > 6) {
                    log_info("\n*******Low Power,enter softpoweroff********\n");
                    low_power_cnt = 0;
                    power_event_to_user(POWER_EVENT_POWER_LOW);
                    usr_timer_del(vbat_fast_timer);
                    vbat_fast_timer = 0;
                }
            } else if (low_warn_cnt > (VBAT_DETECT_CNT / 2)) { //低电提醒
                low_voice_cnt ++;
                low_power_cnt = 0;
                power_normal_cnt = 0;
                cur_bat_st = VBAT_WARNING;
                if ((low_voice_first_flag && low_voice_cnt > 1) || //第一次进低电10s后报一次
                    (!low_voice_first_flag && low_voice_cnt >= 5)) {
                    low_voice_first_flag = 0;
                    low_voice_cnt = 0;
                    if (!lowpower_timer) {
                        log_info("\n**Low Power,Please Charge Soon!!!**\n");
                        power_event_to_user(POWER_EVENT_POWER_WARNING);
                    }
                }
            } else {
                power_normal_cnt++;
                low_voice_cnt = 0;
                low_power_cnt = 0;
                if (power_normal_cnt > 2) {
                    if (cur_bat_st != VBAT_NORMAL) {
                        log_info("[Noraml power]\n");
                        cur_bat_st = VBAT_NORMAL;
                        power_event_to_user(POWER_EVENT_POWER_NORMAL);
                    }
                }
            }
        } else {
            power_event_to_user(POWER_EVENT_POWER_CHARGE);
        }

        unit_cnt = 0;
        low_off_cnt = 0;
        low_warn_cnt = 0;

        if (cur_bat_st != VBAT_LOWPOWER) {
            usr_timer_del(vbat_fast_timer);
            vbat_fast_timer = 0;
            cur_battery_level = battery_value_to_phone_level(bat_val);
            if (cur_battery_level != old_battery_level) {
                power_event_to_user(POWER_EVENT_POWER_CHANGE);
            } else {
                if (charge_online_flag != get_charge_online_flag()) {
                    //充电变化也要交换，确定是否在充电仓
                    power_event_to_user(POWER_EVENT_POWER_CHANGE);
                }
            }
            charge_online_flag =  get_charge_online_flag();
            old_battery_level = cur_battery_level;
        }
    }
}

bool vbat_is_low_power(void)
{
    return (cur_bat_st != VBAT_NORMAL);
}

#include "../../common/ui/lv_watch/comm_func/power_manage.h"
void check_power_on_voltage(void)
{
#if(TCFG_SYS_LVD_EN == 1)
    u16 val = 0;
    u8 normal_power_cnt = 0;
    u8 low_power_cnt = 0;

    while(1) 
    {
        clr_wdt();
        val = get_vbat_level();
        printf("%s:vbat: %d\n", __func__, val);
        if((val < app_var.poweroff_tone_v)/* || adc_check_vbat_lowpower()*/) 
        {
            low_power_cnt++;
            normal_power_cnt = 0;
            if(low_power_cnt > 10) 
            {
                printf("low power , enter softpoweroff!\n");
                BatPowerSetZero();
                power_set_soft_poweroff();
            }
        }else 
        {
            normal_power_cnt++;
            low_power_cnt = 0;
            if (normal_power_cnt > 10) {
                //vbat_check_init();
                BatPowerInit();
                return;
            }
        }
    }
#endif
}
