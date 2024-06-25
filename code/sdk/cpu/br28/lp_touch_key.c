#include "includes.h"
#include "asm/power/p11.h"
#include "asm/power/p33.h"
#include "asm/lp_touch_key_tool.h"
#include "device/key_driver.h"
#include "key_event_deal.h"
#include "app_config.h"
#include "asm/lp_touch_key_api.h"

#define LOG_TAG_CONST       LP_KEY
#define LOG_TAG             "[LP_KEY]"
/* #define LOG_ERROR_ENABLE */
/* #define LOG_DEBUG_ENABLE */
#define LOG_INFO_ENABLE
#define LOG_DUMP_ENABLE
#define LOG_CLI_ENABLE
#include "debug.h"

#if TCFG_LP_TOUCH_KEY_ENABLE

/*************************************************************************************************************************
  										lp_touch_key driver
*************************************************************************************************************************/

#define LP_TOUCH_KEY_TIMER_MAGIC_NUM 		0xFFFF

struct ctmu_key _ctmu_key = {
    .slide_dir = TOUCH_KEY_EVENT_MAX,
    .click_cnt = {0, 0, 0, 0, 0},
    .last_key = {CTMU_KEY_NULL, CTMU_KEY_NULL, CTMU_KEY_NULL, CTMU_KEY_NULL, CTMU_KEY_NULL},
    .short_timer = {LP_TOUCH_KEY_TIMER_MAGIC_NUM, LP_TOUCH_KEY_TIMER_MAGIC_NUM, LP_TOUCH_KEY_TIMER_MAGIC_NUM, LP_TOUCH_KEY_TIMER_MAGIC_NUM, LP_TOUCH_KEY_TIMER_MAGIC_NUM},
};

#define __this      (&_ctmu_key)

static volatile u8 is_lpkey_active = 0;

//======================================================//
// 内置触摸灵敏度表, 由内置触摸灵敏度调试工具生成 		//
// 请将该表替换sdk中lp_touch_key.c中同名的参数表        //
//======================================================//
const static struct ch_adjust_table ch_sensitivity_table[] = {
    /*  cfg0 		cfg1 		cfg2 */
//ch0  PB0
    {  109,   131,  4318}, // level 0
    {  109,   131,  3982}, // level 1
    {  109,   131,  3646}, // level 2
    {  109,   131,  3310}, // level 3
    {  109,   131,  2974}, // level 4
    {  109,   131,  2638}, // level 5
    {  109,   131,  2303}, // level 6
    {  109,   131,  1967}, // level 7
    {  109,   131,  1631}, // level 8
    {  109,   131,  1295}, // level 9
//ch1  PB1
    /* cfg0  , cfg1  , cfg2  */
    {   22,    27,   122}, // level 0
    {   22,    27,   112}, // level 1
    {   22,    27,   103}, // level 2
    {   22,    27,    93}, // level 3
    {   22,    27,    84}, // level 4
    {   22,    27,    74}, // level 5
    {   22,    27,    65}, // level 6
    {   22,    27,    55}, // level 7
    {   22,    27,    46}, // level 8
    {   22,    27,    36}, // level 9
//ch2  PB2
    {   10,    15,   152}, // level 0
    {   10,    15,   140}, // level 1
    {   10,    15,   128}, // level 2
    {   10,    15,   116}, // level 3
    {   10,    15,   104}, // level 4
    {   10,    15,    92}, // level 5
    {   10,    15,    81}, // level 6
    {   10,    15,    69}, // level 7
    {   10,    15,    57}, // level 8
    {   10,    15,    45}, // level 9
//ch3  PB4
    {   10,    15,   152}, // level 0
    {   10,    15,   140}, // level 1
    {   10,    15,   128}, // level 2
    {   10,    15,   116}, // level 3
    {   10,    15,   104}, // level 4
    {   10,    15,    92}, // level 5
    {   10,    15,    81}, // level 6
    {   10,    15,    69}, // level 7
    {   10,    15,    57}, // level 8
    {   10,    15,    45}, // level 9
    //ch4  PB5
    {  109,   131,  4318}, // level 0
    {  109,   131,  3982}, // level 1
    {  109,   131,  3646}, // level 2
    {  109,   131,  3310}, // level 3
    {  109,   131,  2974}, // level 4
    {  109,   131,  2638}, // level 5
    {  109,   131,  2303}, // level 6
    {  109,   131,  1967}, // level 7
    {  109,   131,  1631}, // level 8
    {  109,   131,  1295}, // level 9
};


int __attribute__((weak)) lp_touch_key_event_remap(struct sys_event *e)
{
    return true;
}

static void __ctmu_notify_key_event(struct sys_event *event, u8 ch)
{

#if CFG_DISABLE_KEY_EVENT
    return;
#endif

    event->type = SYS_KEY_EVENT;
    event->u.key.type = KEY_DRIVER_TYPE_CTMU_TOUCH; 	//区分按键类型
    event->arg  = (void *)DEVICE_EVENT_FROM_KEY;

    if (__this->key_ch_msg_lock) {//锁住期间不能发消息
        return;
    }

#if TCFG_LP_TOUCH_KEY_BT_TOOL_ENABLE
    if (lp_touch_key_online_debug_key_event_handle(ch, event)) {
        return;
    }
#endif /* #if TCFG_LP_TOUCH_KEY_BT_TOOL_ENABLE */

    if (lp_touch_key_event_remap(event)) {
        sys_event_notify(event);
    }
}

__attribute__((weak)) u8 remap_ctmu_short_click_event(u8 ch, u8 click_cnt, u8 event)
{
    return event;
}

u8 __attribute__((weak)) ctmu_slide_direction_handle(u8 cur_ch, u8 pre_ch)
{
    if ((cur_ch == 0xff) || (pre_ch == 0xff)) {
        return TOUCH_KEY_EVENT_MAX;
    }
    u8 dir_case = (pre_ch << 4) | (cur_ch & 0xf);
    log_debug("ctmu_touch_key_slide_direction = 0x%x  :  %x -> %x\n", dir_case, pre_ch, cur_ch);
    switch (dir_case) {
    case 0x12:      //方向 1 -> 2
    case 0x23:      //方向 2 -> 3
    case 0x13:      //方向 1 -> 3
        return TOUCH_KEY_EVENT_SLIDE_UP;
        break;
    case 0x32:      //方向 3 -> 2
    case 0x21:      //方向 2 -> 1
    case 0x31:      //方向 3 -> 1
        return TOUCH_KEY_EVENT_SLIDE_DOWN;
        break;
    }
    return TOUCH_KEY_EVENT_MAX;
}

static void __ctmu_short_click_time_out_handle(void *priv)
{
    u8 ch = *((u8 *)priv);
    struct sys_event e;
    switch (__this->click_cnt[ch]) {
    case 0:
        return;
        break;
    case 1:
        e.u.key.event = KEY_EVENT_CLICK;
        break;
    case 2:
        e.u.key.event = KEY_EVENT_DOUBLE_CLICK;
        break;
    case 3:
        e.u.key.event = KEY_EVENT_TRIPLE_CLICK;
        break;
    default:
        e.u.key.event = KEY_EVENT_TRIPLE_CLICK;
        break;
    }

    e.u.key.event = remap_ctmu_short_click_event(ch, __this->click_cnt[ch], e.u.key.event);
    e.u.key.value = __this->config->ch[ch].key_value;

    if (__this->slide_dir < TOUCH_KEY_EVENT_MAX) {
        e.u.key.event = __this->slide_dir;
        e.u.key.value = __this->config->slide_mode_key_value;
        __this->slide_dir = ctmu_slide_direction_handle(0xff, 0xff);
    }
    log_debug("notify key%d short event, cnt: %d", ch, __this->click_cnt[ch]);
    __ctmu_notify_key_event(&e, ch);

    __this->short_timer[ch] = LP_TOUCH_KEY_TIMER_MAGIC_NUM;
    __this->last_key[ch] = CTMU_KEY_NULL;
    __this->click_cnt[ch] = 0;
}

static u8 _ch_priv[LP_CTMU_CHANNEL_SIZE];
static void ctmu_short_click_handle(u8 ch)
{
    static u8 pre_ch = 0xff;
    _ch_priv[ch] = ch;
    __this->slide_dir = TOUCH_KEY_EVENT_MAX;
    __this->last_key[ch] = CTMU_KEY_SHORT_CLICK;
    if (__this->short_timer[ch] == LP_TOUCH_KEY_TIMER_MAGIC_NUM) {
        if (__this->config->slide_mode_en) {
            if ((pre_ch != 0xff) && (pre_ch != ch) && (__this->click_cnt[pre_ch] == 1)) {
                usr_timeout_del(__this->short_timer[pre_ch]);
                __this->short_timer[pre_ch] = LP_TOUCH_KEY_TIMER_MAGIC_NUM;
                __this->click_cnt[pre_ch] = 0;
                __this->last_key[pre_ch] = CTMU_KEY_NULL;
                __this->slide_dir = ctmu_slide_direction_handle(ch, pre_ch);
            }
            pre_ch = ch;
        }
        __this->click_cnt[ch] = 1;
        __this->short_timer[ch] = usr_timeout_add((void *)&_ch_priv[ch], __ctmu_short_click_time_out_handle, CTMU_SHORT_CLICK_DELAY_TIME, 1);
    } else {
        __this->click_cnt[ch]++;
        usr_timer_modify(__this->short_timer[ch], CTMU_SHORT_CLICK_DELAY_TIME);
    }
}

static void ctmu_raise_click_handle(u8 ch)
{
    struct sys_event e = {0};
    if (__this->last_key[ch] >= CTMU_KEY_LONG_CLICK) {
        e.u.key.event = KEY_EVENT_UP;
        e.u.key.value = __this->config->ch[ch].key_value;
        __ctmu_notify_key_event(&e, ch);

        __this->last_key[ch] = CTMU_KEY_NULL;
        log_debug("notify key HOLD UP event");
    } else {
        ctmu_short_click_handle(ch);
    }
}

static void ctmu_long_click_handle(u8 ch)
{
    __this->last_key[ch] = CTMU_KEY_LONG_CLICK;

    struct sys_event e;
    e.u.key.event = KEY_EVENT_LONG;
    e.u.key.value = __this->config->ch[ch].key_value;

    __ctmu_notify_key_event(&e, ch);
}

static void ctmu_hold_click_handle(u8 ch)
{
    __this->last_key[ch] = CTMU_KEY_HOLD_CLICK;

    struct sys_event e;
    e.u.key.event = KEY_EVENT_HOLD;
    e.u.key.value = __this->config->ch[ch].key_value;

    __ctmu_notify_key_event(&e, ch);
}


#if TCFG_LP_EARTCH_KEY_ENABLE

__attribute__((weak))
void ear_lptouch_update_state(u8 state)
{
    return;
}

static void __ctmu_ear_in_timeout_handle(void *priv)
{
#if CFG_DISABLE_INEAR_EVENT
    return;
#endif
    if (__this->config->ch[__this->config->eartch_ch].key_value == 0xFF) {
        //使用外部自定义流程
        if (__this->eartch_last_state == EAR_IN) {
            ear_lptouch_update_state(0);
        } else {
            ear_lptouch_update_state(1);
        }
        return;
    }

#if TCFG_EARTCH_EVENT_HANDLE_ENABLE
    u8 state;
    if (__this->eartch_last_state == EAR_IN) {
        state = EARTCH_STATE_IN;
    } else if (__this->eartch_last_state == EAR_OUT) {
        state = EARTCH_STATE_OUT;
    } else {
        return;
    }
    eartch_state_update(state);
#endif /* #if TCFG_EARTCH_EVENT_HANDLE_ENABLE */
}

static void __ctmu_key_unlock_timeout_handle(void *priv)
{
    __this->key_ch_msg_lock = false;
    __this->key_ch_msg_lock_timer = 0;
}

static void ctmu_eartch_event_handle(u8 eartch_state)
{
    __this->eartch_last_state = eartch_state;

#if TCFG_LP_TOUCH_KEY_BT_TOOL_ENABLE
    struct sys_event event;
    if (eartch_state == EAR_IN) {
        event.u.key.event = 10;
    } else if (eartch_state == EAR_OUT) {
        event.u.key.event = 11;
    }
    if (lp_touch_key_online_debug_key_event_handle(__this->config->eartch_ch, &event)) {
        return;
    }
#endif /* #if TCFG_LP_TOUCH_KEY_BT_TOOL_ENABLE */

    __this->key_ch_msg_lock = true;

    if (__this->key_ch_msg_lock_timer == 0) {
        __this->key_ch_msg_lock_timer = sys_hi_timeout_add(NULL, __ctmu_key_unlock_timeout_handle, ERATCH_KEY_MSG_LOCK_TIME);
    } else {
        sys_hi_timer_modify(__this->key_ch_msg_lock_timer, ERATCH_KEY_MSG_LOCK_TIME);
    }
    __ctmu_ear_in_timeout_handle(NULL);
}

#endif


static void ctmu_port_init(u8 port)
{
    gpio_set_die(port, 0);
    gpio_set_dieh(port, 0);
    gpio_set_direction(port, 1);
    gpio_set_pull_down(port, 0);
    gpio_set_pull_up(port, 0);
}

extern u8 p11_wakeup_query(void);

void lp_touch_key_send_cmd(enum CTMU_M2P_CMD cmd)
{
    M2P_CTMU_CMD = cmd;
    P11_M2P_INT_SET = BIT(M2P_CTMU_INDEX);
}

#define IO_RESET_PORTB_01 			11
void lp_touch_key_init(const struct lp_touch_key_platform_data *config)
{
    log_info("%s >>>>", __func__);

    ASSERT(config && (__this->init == 0));
    __this->config = config;

    u8 pinr_io;
    if (P33_CON_GET(P3_PINR_CON) & BIT(0)) {
        pinr_io = P33_CON_GET(P3_PORT_SEL0);
        if (pinr_io == (IO_RESET_PORTB_01 - 1)) {
            P33_CON_SET(P3_PINR_CON, 0, 8, 0);
            P33_CON_SET(P3_PINR_CON1, 0, 8, 0x16);
            P33_CON_SET(P3_PINR_CON1, 0, 1, 1);
            log_info("reset pin change: old: %d, P3_PINR_CON1 = 0x%x", pinr_io, P33_CON_GET(P3_PINR_CON1));
        }
    }

    u8 ch_enable = 0;
    u8 ch_wakeup_en = 0;
    u8 ch_debug = 0;
    for (u8 ch = 0; ch < LP_CTMU_CHANNEL_SIZE; ch ++) {
        if (__this->config->ch[ch].enable) {
            ch_enable |= BIT(ch);
            if (__this->config->ch[ch].wakeup_enable) {
                ch_wakeup_en |= BIT(ch);
            }
            if (CFG_CHx_DEBUG_ENABLE & BIT(ch)) {
                ch_debug |= BIT(ch);
            }
        }
    }
    if (is_wakeup_source(PWR_WK_REASON_P11) == 0 || get_charge_online_flag() || (!(ch_wakeup_en))) {
        for (u32 m2p_addr = 0x18; m2p_addr < 0x56; m2p_addr ++) {
            M2P_MESSAGE_ACCESS(m2p_addr) = 0;
        }
        for (u32 p2m_addr = 0x10; p2m_addr < 0x24; p2m_addr ++) {
            P2M_MESSAGE_ACCESS(p2m_addr) = 0;
        }
    }

    M2P_CTMU_MSG = 0;
    M2P_CTMU_CH_CFG = 0;
    M2P_CTMU_TIME_BASE = CTMU_SAMPLE_RATE_PRD;

    M2P_CTMU_LONG_TIMEL = (CTMU_LONG_KEY_DELAY_TIME & 0xFF);
    M2P_CTMU_LONG_TIMEH = ((CTMU_LONG_KEY_DELAY_TIME >> 8) & 0xFF);
    M2P_CTMU_HOLD_TIMEL = (CTMU_HOLD_CLICK_DELAY_TIME & 0xFF);
    M2P_CTMU_HOLD_TIMEH = ((CTMU_HOLD_CLICK_DELAY_TIME >> 8) & 0xFF);

    M2P_CTMU_SOFTOFF_LONG_TIMEL = (CFG_M2P_CTMU_SOFTOFF_LONG_TIME & 0xFF);
    M2P_CTMU_SOFTOFF_LONG_TIMEH = ((CFG_M2P_CTMU_SOFTOFF_LONG_TIME >> 8) & 0xFF);

#if TCFG_LP_TOUCH_KEY_BT_TOOL_ENABLE
    M2P_CTMU_LONG_PRESS_RESET_TIME_VALUE_L = 0;
    M2P_CTMU_LONG_PRESS_RESET_TIME_VALUE_H = 0;
#else
    M2P_CTMU_LONG_PRESS_RESET_TIME_VALUE_L = (CTMU_RESET_TIME_CONFIG & 0xFF);
    M2P_CTMU_LONG_PRESS_RESET_TIME_VALUE_H = ((CTMU_RESET_TIME_CONFIG  >> 8) & 0xFF);
#endif

    log_info("M2P_CTMU_LONG_TIMEL = 0x%x", M2P_CTMU_LONG_TIMEL);
    log_info("M2P_CTMU_LONG_TIMEH = 0x%x", M2P_CTMU_LONG_TIMEH);
    log_info("M2P_CTMU_HOLD_TIMEL = 0x%x", M2P_CTMU_HOLD_TIMEL);
    log_info("M2P_CTMU_HOLD_TIMEH = 0x%x", M2P_CTMU_HOLD_TIMEH);

    log_info("M2P_CTMU_SOFTOFF_LONG_TIMEL = 0x%x", M2P_CTMU_SOFTOFF_LONG_TIMEL);
    log_info("M2P_CTMU_SOFTOFF_LONG_TIMEH = 0x%x", M2P_CTMU_SOFTOFF_LONG_TIMEH);

    log_info("M2P_CTMU_LONG_PRESS_RESET_TIME_VALUE_L = 0x%x", M2P_CTMU_LONG_PRESS_RESET_TIME_VALUE_L);
    log_info("M2P_CTMU_LONG_PRESS_RESET_TIME_VALUE_H = 0x%x", M2P_CTMU_LONG_PRESS_RESET_TIME_VALUE_H);

    M2P_CTMU_CH_ENABLE = ch_enable;
    M2P_CTMU_CH_WAKEUP_EN = ch_wakeup_en;
    M2P_CTMU_CH_DEBUG = ch_debug;

    for (u8 ch = 0; ch < LP_CTMU_CHANNEL_SIZE; ch ++) {
        if (__this->config->ch[ch].enable) {
            u8 ch_sensity = __this->config->ch[ch].sensitivity;
            M2P_MESSAGE_ACCESS(M2P_MASSAGE_CTMU_CH0_CFG0L + ch * 8) = ((ch_sensitivity_table[ch_sensity + ch * 10].cfg0) & 0xFF);
            M2P_MESSAGE_ACCESS(M2P_MASSAGE_CTMU_CH0_CFG0H + ch * 8) = (((ch_sensitivity_table[ch_sensity + ch * 10].cfg0) >> 8) & 0xFF);
            M2P_MESSAGE_ACCESS(M2P_MASSAGE_CTMU_CH0_CFG1L + ch * 8) = ((ch_sensitivity_table[ch_sensity + ch * 10].cfg1) & 0xFF);
            M2P_MESSAGE_ACCESS(M2P_MASSAGE_CTMU_CH0_CFG1H + ch * 8) = (((ch_sensitivity_table[ch_sensity + ch * 10].cfg1) >> 8) & 0xFF);
            M2P_MESSAGE_ACCESS(M2P_MASSAGE_CTMU_CH0_CFG2L + ch * 8) = ((ch_sensitivity_table[ch_sensity + ch * 10].cfg2) & 0xFF);
            M2P_MESSAGE_ACCESS(M2P_MASSAGE_CTMU_CH0_CFG2H + ch * 8) = (((ch_sensitivity_table[ch_sensity + ch * 10].cfg2) >> 8) & 0xFF);

            log_info("M2P_CTMU_CH%d_CFG0L = 0x%x", ch, M2P_MESSAGE_ACCESS(M2P_MASSAGE_CTMU_CH0_CFG0L + ch * 8));
            log_info("M2P_CTMU_CH%d_CFG0H = 0x%x", ch, M2P_MESSAGE_ACCESS(M2P_MASSAGE_CTMU_CH0_CFG0H + ch * 8));
            log_info("M2P_CTMU_CH%d_CFG1L = 0x%x", ch, M2P_MESSAGE_ACCESS(M2P_MASSAGE_CTMU_CH0_CFG1L + ch * 8));
            log_info("M2P_CTMU_CH%d_CFG1H = 0x%x", ch, M2P_MESSAGE_ACCESS(M2P_MASSAGE_CTMU_CH0_CFG1H + ch * 8));
            log_info("M2P_CTMU_CH%d_CFG2L = 0x%x", ch, M2P_MESSAGE_ACCESS(M2P_MASSAGE_CTMU_CH0_CFG2L + ch * 8));
            log_info("M2P_CTMU_CH%d_CFG2H = 0x%x", ch, M2P_MESSAGE_ACCESS(M2P_MASSAGE_CTMU_CH0_CFG2H + ch * 8));

            ctmu_port_init(__this->config->ch[ch].port);
#if TCFG_LP_EARTCH_KEY_ENABLE
            if (__this->config->eartch_en && (__this->config->eartch_ch == ch)) {
                M2P_CTMU_CH_CFG |= BIT(1);
                M2P_CTMU_EARTCH_CH = (__this->config->eartch_ref_ch << 4) | (__this->config->eartch_ch & 0xf);
#if (TCFG_EARTCH_EVENT_HANDLE_ENABLE && TCFG_LP_EARTCH_KEY_ENABLE)
                extern int eartch_event_deal_init(void);
                eartch_event_deal_init();
#endif
                u16 trim_value;
                int ret = syscfg_read(LP_KEY_EARTCH_TRIM_VALUE, &trim_value, sizeof(trim_value));
                __this->eartch_trim_flag = 0;
                __this->eartch_inear_ok = 0;
                __this->eartch_trim_value = 0;
                if (ret > 0) {
                    __this->eartch_inear_ok = 1;
                    __this->eartch_trim_value = trim_value;
                    log_info("eartch_trim_value = %d", __this->eartch_trim_value);
                    M2P_CTMU_EARTCH_TRIM_VALUE_L = (__this->eartch_trim_value & 0xFF);
                    M2P_CTMU_EARTCH_TRIM_VALUE_H = ((__this->eartch_trim_value >> 8) & 0xFF);
                } else {
                    //没有trim的情况下用不了
                    M2P_CTMU_EARTCH_TRIM_VALUE_L = (10000 & 0xFF);
                    M2P_CTMU_EARTCH_TRIM_VALUE_H = ((10000 >> 8) & 0xFF);
                }
                //软件触摸灵敏度调试
                M2P_CTMU_INEAR_VALUE_L = __this->config->eartch_soft_inear_val & 0xFF;
                M2P_CTMU_INEAR_VALUE_H = __this->config->eartch_soft_inear_val >> 8;
                M2P_CTMU_OUTEAR_VALUE_L = __this->config->eartch_soft_outear_val & 0xFF;
                M2P_CTMU_OUTEAR_VALUE_H = __this->config->eartch_soft_outear_val >> 8;
            }
#endif
        }
    }

    //CTMU 初始化命令
    if (p11_wakeup_query() == 0 || get_charge_online_flag() || (!(ch_wakeup_en))) {
        LPCTMU_ANA0_CONFIG(LPCTMU_IV_VALUE);
        P2M_CTMU_WKUP_MSG &= (~(P2M_MESSAGE_INIT_MODE_FLAG));
    } else {
        P2M_CTMU_WKUP_MSG |= (P2M_MESSAGE_INIT_MODE_FLAG);
        log_info("p11 wakeup, lp touch key continue work");
    }
    __this->softoff_mode = LP_TOUCH_SOFTOFF_MODE_ADVANCE;

    log_info("M2P_CTMU_CH_ENABLE = 0x%x", M2P_CTMU_CH_ENABLE);
    log_info("M2P_CTMU_CH_WAKEUP_EN = 0x%x", M2P_CTMU_CH_WAKEUP_EN);
    log_info("M2P_CTMU_CH_CFG = 0x%x", M2P_CTMU_CH_CFG);
    log_info("M2P_CTMU_EARTCH_CH = 0x%x", M2P_CTMU_EARTCH_CH);

    lp_touch_key_send_cmd(CTMU_M2P_INIT);

#if TCFG_LP_TOUCH_KEY_BT_TOOL_ENABLE
    lp_touch_key_online_debug_init();
#endif /* #if TCFG_LP_TOUCH_KEY_BT_TOOL_ENABLE */

    __this->init = 1;

    /* extern void wdt_clear(); */
    /* while(1) { */
    /*     wdt_clear(); */
    /* } */
}


u8 last_state = CTMU_P2M_EARTCH_OUT_EVENT;
void p33_ctmu_key_event_irq_handler()
{
    u8 ret = 0;
    u8 ctmu_event = P2M_CTMU_KEY_EVENT;
    u8 ch_num = P2M_CTMU_KEY_CNT;
    u16 ch_res = 0;
    u16 chx_res[LP_CTMU_CHANNEL_SIZE];

#if TCFG_LP_EARTCH_KEY_ENABLE
    if (testbox_get_key_action_test_flag(NULL)) {
        if (__this->config->eartch_en && (__this->config->eartch_ch == ch_num)) {
            ret = lp_touch_key_testbox_remote_test(ch_num, ctmu_event);
            if (ret == true) {
                return;
            }
        }
    }
#endif
    /* printf("ctmu_event = 0x%x\n", ctmu_event); */
    switch (ctmu_event) {
    case CTMU_P2M_CH0_RES_EVENT:
    case CTMU_P2M_CH1_RES_EVENT:
    case CTMU_P2M_CH2_RES_EVENT:
    case CTMU_P2M_CH3_RES_EVENT:
    case CTMU_P2M_CH4_RES_EVENT:
        chx_res[0] = (P2M_CTMU_CH0_H_RES << 8) | P2M_CTMU_CH0_L_RES;
        chx_res[1] = (P2M_CTMU_CH1_H_RES << 8) | P2M_CTMU_CH1_L_RES;
        chx_res[2] = (P2M_CTMU_CH2_H_RES << 8) | P2M_CTMU_CH2_L_RES;
        chx_res[3] = (P2M_CTMU_CH3_H_RES << 8) | P2M_CTMU_CH3_L_RES;
        chx_res[4] = (P2M_CTMU_CH4_H_RES << 8) | P2M_CTMU_CH4_L_RES;
        ch_res = chx_res[ch_num];
        /* printf("ch%d_res: %d\n", ch_num, ch_res); */

#if TWS_BT_SEND_KEY_CH_RES_DATA_ENABLE
        if (tws_api_get_tws_state() & TWS_STA_SIBLING_CONNECTED) {
            lpctmu_tws_send_res_data(BT_KEY_CH_RES_MSG,
                                     chx_res[0],
                                     chx_res[1],
                                     chx_res[2],
                                     chx_res[3],
                                     chx_res[4]);
        }
#endif

#if TCFG_LP_TOUCH_KEY_BT_TOOL_ENABLE
        lp_touch_key_online_debug_send(ch_num, ch_res);
#endif

#if TCFG_LP_EARTCH_KEY_ENABLE
        if (__this->config->eartch_en && (__this->config->eartch_ch == ch_num)) {

            u16 eartch_ch_res = chx_res[ch_num];
            u16 eartch_ref_ch_res = chx_res[__this->config->eartch_ref_ch];
            u16 eartch_iir = (P2M_CTMU_EARTCH_H_IIR_VALUE << 8) | P2M_CTMU_EARTCH_L_IIR_VALUE;
            u16 eartch_trim = (P2M_CTMU_EARTCH_H_TRIM_VALUE << 8) | P2M_CTMU_EARTCH_L_TRIM_VALUE;
            u16 eartch_diff = (P2M_CTMU_EARTCH_H_DIFF_VALUE << 8) | P2M_CTMU_EARTCH_L_DIFF_VALUE;

#if TWS_BT_SEND_EARTCH_RES_DATA_ENABLE
            if (tws_api_get_tws_state() & TWS_STA_SIBLING_CONNECTED) {
                lpctmu_tws_send_res_data(BT_EARTCH_RES_MSG,
                                         eartch_ch_res,
                                         eartch_ref_ch_res,
                                         eartch_iir,
                                         eartch_trim,
                                         eartch_diff);
            }
#endif

#if !TCFG_LP_TOUCH_KEY_BT_TOOL_ENABLE
            if (__this->eartch_trim_flag) {
                if (__this->eartch_trim_value == 0) {
                    __this->eartch_trim_value = eartch_diff;
                } else {
                    __this->eartch_trim_value = ((eartch_diff + __this->eartch_trim_value) >> 1);
                }
                if (__this->eartch_trim_flag++ > 20) {
                    __this->eartch_trim_flag = 0;
                    M2P_CTMU_CH_DEBUG &= ~BIT(ch_num);
                    int ret = syscfg_write(LP_KEY_EARTCH_TRIM_VALUE, &(__this->eartch_trim_value), sizeof(__this->eartch_trim_value));
                    log_info("write ret = %d", ret);
                    if (ret > 0) {
                        M2P_CTMU_EARTCH_TRIM_VALUE_L = (__this->eartch_trim_value & 0xFF);
                        M2P_CTMU_EARTCH_TRIM_VALUE_H = ((__this->eartch_trim_value >> 8) & 0xFF);
                        __this->eartch_inear_ok = 1;
#if TCFG_EARTCH_EVENT_HANDLE_ENABLE
                        eartch_state_update(EARTCH_STATE_TRIM_OK);
#endif
                        log_info("trim: %d\n", __this->eartch_inear_ok);
                        is_lpkey_active = 0;
                    } else {
                        __this->eartch_inear_ok = 0;
#if TCFG_EARTCH_EVENT_HANDLE_ENABLE
                        eartch_state_update(EARTCH_STATE_TRIM_ERR);
#endif
                        log_info("trim: %d\n", __this->eartch_inear_ok);
                        is_lpkey_active = 0;
                    }
                }
                log_debug("eartch ch trim value: %d, res = %d", __this->eartch_trim_value, eartch_diff);
            }
#endif

            if (P2M_CTMU_EARTCH_EVENT != last_state) {
                last_state = P2M_CTMU_EARTCH_EVENT;
                if (last_state == CTMU_P2M_EARTCH_IN_EVENT) {
                    printf("soft inear\n");
#if TWS_BT_SEND_EVENT_ENABLE
                    lpctmu_tws_send_event_data(EAR_IN, BT_EVENT_SW_MSG);
#endif
                    if (__this->eartch_inear_ok) {
                        ctmu_eartch_event_handle(EAR_IN);
                    }

                } else if (last_state == CTMU_P2M_EARTCH_OUT_EVENT) {
                    printf("soft outear");
#if TWS_BT_SEND_EVENT_ENABLE
                    lpctmu_tws_send_event_data(EAR_OUT, BT_EVENT_SW_MSG);
#endif
                    if (__this->eartch_inear_ok) {
                        ctmu_eartch_event_handle(EAR_OUT);
                    }

                }
            }

        }
#endif
        break;

    case CTMU_P2M_CH0_SHORT_KEY_EVENT:
    case CTMU_P2M_CH1_SHORT_KEY_EVENT:
    case CTMU_P2M_CH2_SHORT_KEY_EVENT:
    case CTMU_P2M_CH3_SHORT_KEY_EVENT:
    case CTMU_P2M_CH4_SHORT_KEY_EVENT:
        log_debug("CH%d: SHORT click", ch_num);
        ctmu_short_click_handle(ch_num);
        break;
    case CTMU_P2M_CH0_LONG_KEY_EVENT:
    case CTMU_P2M_CH1_LONG_KEY_EVENT:
    case CTMU_P2M_CH2_LONG_KEY_EVENT:
    case CTMU_P2M_CH3_LONG_KEY_EVENT:
    case CTMU_P2M_CH4_LONG_KEY_EVENT:
        log_debug("CH%d: LONG click", ch_num);
        ctmu_long_click_handle(ch_num);
        break;
    case CTMU_P2M_CH0_HOLD_KEY_EVENT:
    case CTMU_P2M_CH1_HOLD_KEY_EVENT:
    case CTMU_P2M_CH2_HOLD_KEY_EVENT:
    case CTMU_P2M_CH3_HOLD_KEY_EVENT:
    case CTMU_P2M_CH4_HOLD_KEY_EVENT:
        log_debug("CH%d: HOLD click", ch_num);
        ctmu_hold_click_handle(ch_num);
        break;
    case CTMU_P2M_CH0_FALLING_EVENT:
    case CTMU_P2M_CH1_FALLING_EVENT:
    case CTMU_P2M_CH2_FALLING_EVENT:
    case CTMU_P2M_CH3_FALLING_EVENT:
    case CTMU_P2M_CH4_FALLING_EVENT:
        log_debug("CH%d: FALLING", ch_num);
        break;
    case CTMU_P2M_CH0_RAISING_EVENT:
    case CTMU_P2M_CH1_RAISING_EVENT:
    case CTMU_P2M_CH2_RAISING_EVENT:
    case CTMU_P2M_CH3_RAISING_EVENT:
    case CTMU_P2M_CH4_RAISING_EVENT:
        log_debug("CH%d: RAISING", ch_num);
        ctmu_raise_click_handle(ch_num);
        break;
    case CTMU_P2M_EARTCH_IN_EVENT:
        log_debug("CH%d: IN", ch_num);
        break;
    case CTMU_P2M_EARTCH_OUT_EVENT:
        log_debug("CH%d: OUT", ch_num);
        break;
    default:
        break;
    }
}


u8 lp_touch_key_power_on_status()
{
    u8 ret = 0;
    log_debug("P2M_CTMU_CTMU_WKUP_MSG = 0x%x", P2M_CTMU_WKUP_MSG);
    if (P2M_CTMU_WKUP_MSG & P2M_MESSAGE_POWER_ON_FLAG) {  //长按开机查询
        if (P2M_CTMU_WKUP_MSG & (P2M_MESSAGE_KEY_ACTIVE_FLAG)) { //按键释放查询, 0:释放, 1: 触摸保持
            ret = 1; //key active
        }
    }
    return ret;
}

void lp_touch_key_disable(void)
{
    log_debug("%s", __func__);
    P2M_CTMU_WKUP_MSG &= (~(P2M_MESSAGE_SYNC_FLAG));
    lp_touch_key_send_cmd(CTMU_M2P_DISABLE);
    while (!(P2M_CTMU_WKUP_MSG & P2M_MESSAGE_SYNC_FLAG));
    __this->softoff_mode = LP_TOUCH_SOFTOFF_MODE_LEGACY;
}

void lp_touch_key_enable(void)
{
    log_debug("%s", __func__);
    lp_touch_key_send_cmd(CTMU_M2P_ENABLE);
    __this->softoff_mode = LP_TOUCH_SOFTOFF_MODE_ADVANCE;
}

void lp_touch_key_charge_mode_enter()
{
#if (!LP_TOUCH_KEY_CHARGE_MODE_SW_DISABLE)
    log_debug("%s", __func__);
    P2M_CTMU_WKUP_MSG &= (~(P2M_MESSAGE_SYNC_FLAG));
    lp_touch_key_send_cmd(CTMU_M2P_CHARGE_ENTER_MODE);
    while (!(P2M_CTMU_WKUP_MSG & P2M_MESSAGE_SYNC_FLAG));
    __this->softoff_mode = LP_TOUCH_SOFTOFF_MODE_LEGACY;
#endif
}

void lp_touch_key_charge_mode_exit()
{
#if (!LP_TOUCH_KEY_CHARGE_MODE_SW_DISABLE)
    log_debug("%s", __func__);
    lp_touch_key_send_cmd(CTMU_M2P_CHARGE_EXIT_MODE);
    __this->softoff_mode = LP_TOUCH_SOFTOFF_MODE_ADVANCE;
#endif
}

//=============================================//
//NOTE: 该函数为进关机时被库里面回调
//在板级配置struct low_power_param power_param中变量lpctmu_en配置为TCFG_LP_TOUCH_KEY_ENABLE时:
//该函数在决定softoff进触摸模式还是普通模式:
//	return 1: 进触摸模式关机(LP_TOUCH_SOFTOFF_MODE_ADVANCE);
//	return 0: 进普通模式关机(触摸关闭)(LP_TOUCH_SOFTOFF_MODE_LEGACY);
//使用场景:
// 	1)在充电舱外关机, 需要触摸开机, 进带触摸关机模式;
// 	2)在充电舱内关机，可以关闭触摸模块, 进普通关机模式, 关机功耗进一步降低.
//=============================================//
u8 lp_touch_key_softoff_mode_query(void)
{
    return __this->softoff_mode;
}

void set_lpkey_active(u8 set)
{
    is_lpkey_active = set;
}

static u8 lpkey_idle_query(void)
{
    return !is_lpkey_active;
}

REGISTER_LP_TARGET(key_lp_target) = {
    .name = "lpkey",
    .is_idle = lpkey_idle_query,
};


#endif

