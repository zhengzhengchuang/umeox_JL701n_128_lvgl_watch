#include "ui/ui_api.h"
#include "ui/ui.h"
#include "ui/ui_style.h"
#include "ui/qr_code.h"
#include "key_event_deal.h"
#include "le_smartbox_adv.h"
#include "app_config.h"
#include "custom_cfg.h"
#include "system/includes.h"

#if TCFG_PAY_ALIOS_ENABLE
#if (TCFG_PAY_ALIOS_WAY_SEL == TCFG_PAY_ALIOS_WAY_ALIYUN)
#include "upay.h"
#define BINDING_STU_FAIL				UPAY_STATUS_BINDING_FAIL
#define BINDING_STU_GETTING_PROFILE		UPAY_STATUS_GETTING_PROFILE
#define BINDING_STU_FINISH				UPAY_STATUS_FINISH_BINDING
#define BINDING_STU_FINISH_OK			UPAY_STATUS_FINISH_BINDING_OK
#define BINDING_STU_OK					UPAY_STATUS_BINDING_OK
#define BINDING_STU_E					upay_binding_status_e
#define GET_BINDING_STU()				upay_get_binding_status()
#define GET_PAYCODE(x,y)				upay_get_paycode(x,y)
#define GET_NICK_NAME(x,y)				upay_get_nick_name(x,y)
#define GET_LOGON_ID(x,y)				upay_get_logon_ID(x,y)
#define GET_BINDING_STRING(x,y)			upay_get_binding_string(x,y)
#define GET_VERSION()					upay_get_version()
#define ENV_INIT()						upay_env_init()
#define ENV_DEINIT()					upay_env_deinit()
#define UNBINDING()						upay_unbinding()
#define AOS_NV_FLUSH()					aos_hal_kv_nv_flush()
#define ALIOS_CACHE_INIT()
#define ALIOS_CACHE_UNINIT()
//#define ALIOS_RET_ALLOC()				upay_rc_t ret
#define POWER_ON()
#define POWER_OFF()
#else
#include "alipay.h"
#include "alipay_common.h"
#include "gpio.h"
#define BINDING_STU_FAIL				STATUS_BINDING_FAIL
#define BINDING_STU_GETTING_PROFILE		STATUS_GETTING_PROFILE
#define BINDING_STU_FINISH				STATUS_FINISH_BINDING
#define BINDING_STU_FINISH_OK			STATUS_FINISH_BINDING_OK
#define BINDING_STU_OK					STATUS_BINDING_OK
#define BINDING_STU_E					binding_status_e
#define GET_BINDING_STU()				alipay_get_binding_status()
#define GET_PAYCODE(x,y)				alipay_get_paycode(x,y)
#define GET_NICK_NAME(x,y)				alipay_get_nick_name(x,y)
#define GET_LOGON_ID(x,y)				alipay_get_logon_ID(x,y)
#define GET_BINDING_STRING(x,y)			alipay_get_binding_string(x,y)
#define GET_VERSION()					printf("Hard alipay V1.1.4")
#define ENV_INIT()						alipay_env_init()
#define ENV_DEINIT()					alipay_env_deinit()
#define UNBINDING()						alipay_unbinding()
#define AOS_NV_FLUSH()
extern void alipay_cache_buf_init(void);
extern void alipay_cache_buf_uninit(void);
#define ALIOS_CACHE_INIT()				alipay_cache_buf_init()
#define ALIOS_CACHE_UNINIT()			alipay_cache_buf_uninit()
//#define ALIOS_RET_ALLOC()				retval_t ret
#define POWER_ON()                      alipay_power_on()
#define POWER_OFF()                     alipay_power_off()
u8  alipay_check_open_status = 0;
#endif
#endif

#define LOG_TAG_CONST       UI
#define LOG_TAG     		"[UI-ACTION]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"

/* #undef log_info */
/* #define log_info	printf */

extern void *br28_malloc(int size);
extern void *br28_zalloc(int size);
extern void br28_free(void *buf);

extern void mem_stats(void);
extern void mem_vlt_stats(void);
#define MEM_STATUS()		mem_vlt_stats()//mem_stats()

extern int aos_hal_kv_nv_flush(void);

/* #define QR_CODE_MESSAGE "{\"bleAddr\":\"0E:50:09:BC:8B:83\",\"connectWay\":1,\"edrAddr\":\"0E:50:09:1B:D0:E5\",\"name\":\"JL_WATCH_1\",\"pid\":49,\"vid\":2}" */
//ALIPAY_WATCH#2#[MAC-ADDR]#Device_Name#Ver
/* #define ALIPAY_QR_CODE_MESSAGE "{ALIPAY_WATCH#2#76:BE:8C:6F:D3:74#AC701N_WATCH_wilson#4.0.0}" */

#if TCFG_PAY_ALIOS_ENABLE
#define ALIPAY_CHECK_TIME_MS				200
#define ALIPAY_CHECK_TIME_OUT_CNT			(30*1000/ALIPAY_CHECK_TIME_MS)	// 绑定超时
#define ALIPAY_WARN_TIME_OUT_CNT			(5*1000/ALIPAY_CHECK_TIME_MS)	// 提示时间
#define ALIPAY_REFRESH_TIME_OUT_CNT			(60*1000/ALIPAY_CHECK_TIME_MS)	// 超时刷新支付码
static u8  alipay_check_status = 0;
static u16 alipay_check_cnt = 0;
#endif /* #if TCFG_PAY_ALIOS_ENABLE */

static u16 alipay_time_id = 0;
static u8  alipay_show_barcode = 0;

static u8 *row_img = 0;
static u8 *copy_row_img = 0;
/* static u8 alipay_QRcode_mess[70]; */

#define QR_CODE_MESS_SIZE   (180 * sizeof(u8))
static u8 *qr_code_mess = NULL;

#define alipay_QRcode_mess		qr_code_mess
#define ALIPAY_QRCODE_BUF_LEN	QR_CODE_MESS_SIZE

int ui_qrcode_init()
{
    if (!qr_code_mess) {
        qr_code_mess = zalloc(QR_CODE_MESS_SIZE);
        if (!qr_code_mess) {
            ASSERT(0, "qr_code_mess malloc err!");
            return -ENOMEM;
        }
    }
    return 0;
}

int ui_qrcode_uninit()
{
    if (qr_code_mess) {
        free(qr_code_mess);
        qr_code_mess = NULL;
    }
    return 0;
}

struct _QR_CODE {
    int code128_mode;
    u8 qr_version;
    u8 qr_max_version;
    u8 qr_ecc_level;
    int qr_code_max_input_len;
    int qr_buf_size;
    int img_w;
    int out_size;
    int line_size;
};

struct _BAR_CODE {
    int code128_mode;
    int img_w;
    int img_h;
    int out_size;
    int line_size;
};

struct _QR_CODE bt_con_qr_code = {
    .code128_mode = 60,
    .qr_version = 3,
    .qr_max_version = 8,
    .qr_ecc_level = 2,
    .qr_code_max_input_len = 384,
    .qr_buf_size = 4096,
    .img_w = 300,
};

struct _QR_CODE user_guide_qr_code = {
    .code128_mode = 60,
    .qr_version = 3,
    .qr_max_version = 8,
    .qr_ecc_level = 2,
    .qr_code_max_input_len = 384,
    .qr_buf_size = 4096,
    .img_w = 227,
};

struct _QR_CODE alipay_qr_code = {
    .code128_mode = 60,
    .qr_version = 3,
    .qr_max_version = 8,
    .qr_ecc_level = 2,
    .qr_code_max_input_len = 384,
    .qr_buf_size = 4096,
    .img_w = 260,
};

#define BAR_CODE_REMAIN_LEN		8
struct _BAR_CODE alipay_bar_code = {
    .code128_mode = 0, //60, // 纯数字才能填0
    .img_w = 156 * 2 + BAR_CODE_REMAIN_LEN * 2, // 19个数字输出156
    .img_h = 120,
};

static jl_code_param_t jl_code_param;

void *jl_qr_code_malloc(int size)
{
    return br28_malloc(size);
}

void jl_qr_code_free(void *p)
{
    br28_free(p);
}
// u32 gpio = IO_PORTG_01;//指定IO给加密芯片供电
#if TCFG_PAY_ALIOS_ENABLE &&(TCFG_PAY_ALIOS_WAY_SEL == TCFG_PAY_ALIOS_WAY_T_HEAD)
void alipay_power_on(void)
{
    log_info("%s,%d", __func__, __LINE__);

    //sdpg_config(1); // 对应加密芯片电源管脚
    alipay_check_open_status = 1;
    printf("alipay_check_open_status =%d", alipay_check_open_status);
    /*
     gpio_set_pull_down(gpio, 0);
     gpio_set_pull_up(gpio, 0);
     gpio_set_hd(gpio, 0);//看需求是否需要开启强推,会导致芯片功耗大
     gpio_set_hd0(gpio, 0);
     gpio_set_direction(gpio, 0);
     gpio_set_output_value(gpio, 1); //1高0低*/

    extern void HS_IIC_Init(void);
    HS_IIC_Init();
}
void alipay_power_off(void)
{
    log_info("%s,%d", __func__, __LINE__);
    alipay_check_open_status = 0;
    printf("alipay_check_open_status =%d", alipay_check_open_status);
    extern void HS_IIC_Uninit(void);
    HS_IIC_Uninit();
    //sdpg_config(0);
    // gpio_set_output_value(gpio, 0); //1高0低
}

static void lcd_alipay_sleep_enter(void)
{
    if (alipay_check_open_status) {
        alipay_power_off();
    }
}

REGISTER_LCD_SLEEP_HEADLER(lcd_alipay_sleep) = {
    .name = "vad",
    .enter = lcd_alipay_sleep_enter,
};

#endif

void bt_addr2string(u8 *addr, u8 *buf)
{
    u8 len = 0;
    for (s8 i = 5; i >= 0; i--) {
        if ((addr[i] / 16) >= 10) {
            buf[len] = 'A' + addr[i] / 16 - 10;
        } else {
            buf[len] = '0' + addr[i] / 16;
        }
        if ((addr[i] % 16) >= 10) {
            buf[len + 1] = 'A' + addr[i] % 16 - 10;
        } else {
            buf[len + 1] = '0' + addr[i] % 16;
        }
        len += 2;
        buf[len] = ':';
        len += 1;
    }
    buf[len - 1] = '\0';
    log_info("%s", buf);
}

static void qr_code_message_init(void)
{
    u8 ble_addr_ptr[18] = {0};
    u8 edr_addr_ptr[18] = {0};
#if (RCSP_CHANNEL_SEL==RCSP_USE_SPP)
    u8 conway = 1;
#else /* #if (RCSP_CHANNEL_SEL==RCSP_USE_SPP) */
    u8 conway = 0;
#endif /* #if (RCSP_CHANNEL_SEL==RCSP_USE_SPP) */
    u16 vid = get_vid_pid_ver_from_cfg_file(GET_VID_FROM_EX_CFG);
    u16 pid = get_vid_pid_ver_from_cfg_file(GET_PID_FROM_EX_CFG);
    u8 ble_addr[6];
    u8 *edr_addr = bt_get_mac_addr();
    u8 *name = bt_get_local_name();
    extern int le_controller_get_mac(void *addr);
    le_controller_get_mac(ble_addr);
    bt_addr2string(ble_addr, ble_addr_ptr);
    bt_addr2string(edr_addr, edr_addr_ptr);
    snprintf(qr_code_mess, QR_CODE_MESS_SIZE,
             "{\"bleAddr\":\"%s\",\"connectWay\":\"%d\",\"edrAddr\":\"%s\",\"name\":\"%s\",\"pid\":\"%d\",\"vid\":\"%d\"}",
             ble_addr_ptr, conway, edr_addr_ptr, name, pid, vid);
    /* snprintf(alipay_QRcode_mess, sizeof(alipay_QRcode_mess), */
    /* "ALIPAY_WATCH#2#%s#%s#4.0.0", */
    /* ble_addr_ptr, name); */
    /* log_info("%s", qr_code_mess); */
}

void jl_qr_code_process(u8 *qr_code_str, int qr_code_len, struct _QR_CODE *qr_code)
{
    int ret;
    u8 num = 0;
    log_info("before init:");
    log_info("qr:%s \n", qr_code_str);
    MEM_STATUS();
    //qr_code需要消耗一定的内存：qr_buf_size+4*(21+(qr_max_version-1)*4)*(21+(qr_max_version-1)*4)
    jl_code_init(qr_code->code128_mode, qr_code->qr_version, qr_code->qr_max_version, qr_code->qr_ecc_level,  qr_code->qr_code_max_input_len, qr_code->qr_buf_size, qr_code->img_w);
    ret = jl_code_process(JL_CODE_MODE_QR_CODE, qr_code_str, qr_code_len, &qr_code->out_size, &qr_code->line_size);  //返回值为 1，说明编码成功。
    log_info("out_size:%d line_size:%d", qr_code->out_size, qr_code->line_size);
    if (ret == 1) {                                   //out_size为基础数据大小，也就是原始数据大小
        jl_code_param.l_size = 1;
        if (jl_code_param.l_size > qr_code->line_size) {
            jl_code_param.l_size = qr_code->line_size;
        }
        jl_code_set_info(&jl_code_param);  //设置一个输出数据占用几个像素点，这个地方同比放大了数据
        row_img = br28_zalloc(qr_code->out_size * jl_code_param.l_size);
        if (row_img == 0) {
            jl_code_deinit();
            log_info("after deinit:");
            MEM_STATUS();
            return;
        }
        copy_row_img = br28_zalloc(qr_code->out_size * jl_code_param.l_size * qr_code->out_size * jl_code_param.l_size);
        if (copy_row_img == 0) {
            jl_code_deinit();
            log_info("after deinit:");
            MEM_STATUS();
            return;
        }
        log_info("remain heap size:");
        MEM_STATUS();
        for (unsigned char j = 0; j < qr_code->out_size; j++) {
            //读取输出数据，这里实际上得到的是 out_size* jl_code_param.l_size个数据，是将原数据同比放大jl_code_param.l_size倍
            jl_code_get_data(qr_code->out_size, j, row_img);
            /* put_buf(row_img, qr_code->out_size * jl_code_param.l_size); */
            for (int k = 0; k < jl_code_param.l_size; k++) {
                memcpy(copy_row_img + (j * jl_code_param.l_size + k) * qr_code->out_size * jl_code_param.l_size, row_img, qr_code->out_size * jl_code_param.l_size);
            }
        }
    }
    jl_code_deinit();
    log_info("after deinit:");
    MEM_STATUS();
}

void jl_bar_code_process(u8 *bar_code_str, int bar_code_len, struct _BAR_CODE *bar_code)
{
    int ret;
    u8 num = 0;
    log_info("before init:");
    log_info("bar:%s \n", bar_code_str);
    MEM_STATUS();
    jl_code_init(bar_code->code128_mode, 1, 0, 0, 0, 4096, bar_code->img_w);
    /* jl_code_init(bar_code->code128_mode, 1, 0, 0, 0, 4096, 244); */
    ret = jl_code_process(JL_CODE_MODE_CODE_128, bar_code_str, bar_code_len, &bar_code->out_size, &bar_code->line_size);  //返回值为 1，说明编码成功。
    log_info("out_size:%d line_size:%d", bar_code->out_size, bar_code->line_size);
    if (ret == 1) {                                   //out_size为基础数据大小，也就是原始数据大小
        jl_code_param.l_size = 1;
        if (jl_code_param.l_size > bar_code->line_size) {
            jl_code_param.l_size = bar_code->line_size;
        }
        jl_code_set_info(&jl_code_param);  //设置一个输出数据占用几个像素点，这个地方同比放大了数据
        row_img = br28_zalloc(bar_code->out_size * jl_code_param.l_size);
        if (row_img == 0) {
            jl_code_deinit();
            log_info("after deinit:");
            MEM_STATUS();
            return;
        }
        copy_row_img = br28_zalloc(bar_code->out_size * jl_code_param.l_size);
        if (copy_row_img == 0) {
            jl_code_deinit();
            log_info("after deinit:");
            MEM_STATUS();
            return;
        }
        log_info("remain heap size:");
        MEM_STATUS();

        jl_code_get_data(bar_code->out_size, 0, row_img);
        /* put_buf(row_img, bar_code->out_size * jl_code_param.l_size); */
        for (int k = 0; k < jl_code_param.l_size; k++) {
            memcpy(copy_row_img + k * bar_code->out_size * jl_code_param.l_size, row_img, bar_code->out_size * jl_code_param.l_size);
        }
    }
    jl_code_deinit();
    log_info("after deinit:");
    MEM_STATUS();
}


void fill_rect(void *_dc, struct rect *rectangle, u16 color);
static void qr_fill_rect(void *_dc, int x, int y, int width, int height, int color)
{
    struct rect rectangle;
    rectangle.left = x;
    rectangle.top = y;
    rectangle.width = width;
    rectangle.height = height;

    fill_rect(_dc, &rectangle, color);
}

/* #if (!TCFG_LUA_ENABLE) */
#ifdef CONFIG_UI_STYLE_JL_ENABLE
#if TCFG_UI_ENABLE_QR_CODE

#define STYLE_NAME  JL

static void ui_draw_qrcode(int id, u8 *dst_buf, struct rect *dst_r, struct rect *src_r, u8 bytes_per_pixel, void *priv)
{
    int w, h;
    struct rect r;
    struct rect rect = {0};
    int dst_stride = (dst_r->width * bytes_per_pixel + 3) / 4 * 4;
    int src_stride = (src_r->width * bytes_per_pixel + 3) / 4 * 4;
    u8 step_size;
    u8 remain_size;
    struct _QR_CODE *qr_code = (struct _QR_CODE *)priv;

    struct draw_context dc = {0};
    dc.buf = dst_buf;
    memcpy(&dc.disp, dst_r, sizeof(struct rect));
    memcpy(&dc.draw, dst_r, sizeof(struct rect));
    memcpy(&rect, src_r, sizeof(struct rect));

    //添加白底画图区域
    qr_fill_rect(&dc, rect.left, rect.top, rect.width, rect.height, 0xffff);
    /*
     * 以下绘图函数只能在该控件的范围内显示，超出控件区域不显示
     * */
    if ((qr_code->out_size == 0) || (jl_code_param.l_size == 0)) {
        step_size = 0;
        remain_size = 0;
    } else {
        step_size = rect.width / (qr_code->out_size * jl_code_param.l_size);
        remain_size = (rect.width % (qr_code->out_size * jl_code_param.l_size)) / 2;
    }
    /* log_info("step_size:%d %d %d",step_size,remain_size,qr_code->out_size * jl_code_param.l_size); */
    for (int i = 0; i < qr_code->out_size * jl_code_param.l_size; i++) {
        for (int j = 0; j < qr_code->out_size * jl_code_param.l_size; j++) {
            /* if (img[i][j] == 0xff) { */
            if (copy_row_img[i * qr_code->out_size * jl_code_param.l_size + j] == 0xff) {
                qr_fill_rect(&dc, rect.left + 0 + remain_size + step_size * j, rect.top + 0 + remain_size + step_size * i, step_size, step_size, 0xffff);
            }
            /* if (img[i][j] == 0x00) { */
            if (copy_row_img[i * qr_code->out_size * jl_code_param.l_size + j] == 0x00) {
                qr_fill_rect(&dc, rect.left + 0 + remain_size + step_size * j, rect.top + 0 + remain_size + step_size * i, step_size, step_size, 0x0000);
            }
        }
    }
}

static void ui_draw_barcode(int id, u8 *dst_buf, struct rect *dst_r, struct rect *src_r, u8 bytes_per_pixel, void *priv)
{
    int w, h;
    struct rect r;
    struct rect rect = {0};
    int dst_stride = (dst_r->width * bytes_per_pixel + 3) / 4 * 4;
    int src_stride = (src_r->width * bytes_per_pixel + 3) / 4 * 4;
    u8 step_size;
    u8 w_remain_size;
    u8 h_remain_size;
    u8 height_size;
    struct _BAR_CODE *bar_code = (struct _BAR_CODE *)priv;

    struct draw_context dc = {0};
    dc.buf = dst_buf;
    memcpy(&dc.disp, dst_r, sizeof(struct rect));
    memcpy(&dc.draw, dst_r, sizeof(struct rect));
    memcpy(&rect, src_r, sizeof(struct rect));

    //添加白底画图区域
    qr_fill_rect(&dc, rect.left, rect.top, rect.width, rect.height, 0xffff);
    /*
     * 以下绘图函数只能在该控件的范围内显示，超出控件区域不显示
     * */
    if ((bar_code->out_size == 0) || (jl_code_param.l_size == 0)) {
        step_size = 0;
        w_remain_size = 0;
        h_remain_size = 0;
    } else {
        step_size = rect.width / (bar_code->out_size * jl_code_param.l_size);
        w_remain_size = (rect.width % (bar_code->out_size * jl_code_param.l_size)) / 2;
        h_remain_size = BAR_CODE_REMAIN_LEN;
    }
    height_size = rect.height - h_remain_size * 2;
    log_info("rect:%d,%d,%d,%d \n", rect.left, rect.top, rect.width, rect.height);
    log_info("step_size:%d %d %d %d", step_size, w_remain_size, height_size, bar_code->out_size * jl_code_param.l_size);
    for (int j = 0; j < bar_code->out_size * jl_code_param.l_size; j++) {
        if (copy_row_img[j] == 0xff) {
            qr_fill_rect(&dc, rect.left + 0 + w_remain_size + step_size * j, rect.top + h_remain_size, step_size, height_size, 0xffff);
        }
        if (copy_row_img[j] == 0x00) {
            qr_fill_rect(&dc, rect.left + 0 + w_remain_size + step_size * j, rect.top + h_remain_size, step_size, height_size, 0x0000);
        }
    }
}

static int layout_qr_code_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct element *elm = (struct element *)_ctrl;
    struct draw_context *dc = (struct draw_context *)arg;
    struct rect rect = {0};
    u8 step_size;
    u8 remain_size;

    switch (event) {
    case ON_CHANGE_INIT:
        qr_code_message_init();
        jl_qr_code_process(qr_code_mess, strlen(qr_code_mess), &bt_con_qr_code);
        break;
    case ON_CHANGE_SHOW:
        /*根据需要调用以下三个接口:ui_remove_backcolor,ui_remove_backimage,ui_remove_border*/
        /* ui_remove_backcolor(elm);//移除控件背景颜色 */
        /* ui_remove_backimage(elm);//移除控件背景图像 */
        /* ui_remove_border(elm);//移除控件边界 */
        break;
    case ON_CHANGE_SHOW_POST:
        ui_core_get_element_abs_rect(elm, &rect); //跟随控件移动,注释掉这句则不跟随控件移动
        int draw_width = (rect.width > rect.height) ? rect.height : rect.width;
        if (draw_width <= bt_con_qr_code.img_w) {
            ASSERT(0);
        }
        ui_draw(dc,
                NULL,
                rect.left + (draw_width - bt_con_qr_code.img_w) / 2,
                rect.top + (draw_width - bt_con_qr_code.img_w) / 2,
                bt_con_qr_code.img_w,
                bt_con_qr_code.img_w,
                ui_draw_qrcode,
                (void *)&bt_con_qr_code,
                sizeof(struct _QR_CODE),
                0);
        break;
    case ON_CHANGE_RELEASE:
        if (row_img) {
            br28_free(row_img);
            row_img = 0;
        }
        if (copy_row_img) {
            br28_free(copy_row_img);
            copy_row_img = 0;
        }
        log_info("qr_code release:");
        MEM_STATUS();
        break;
    }
    return false;
}
/* REGISTER_UI_EVENT_HANDLER(LAYOUT_QR_CODE) */
REGISTER_UI_EVENT_HANDLER(QR_CODE_LAYOUT)
.onchange = layout_qr_code_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};



static int layout_user_guide_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct element *elm = (struct element *)_ctrl;
    struct draw_context *dc = (struct draw_context *)arg;
    struct rect rect = {0};

    switch (event) {
    case ON_CHANGE_INIT:
        qr_code_message_init();
        jl_qr_code_process(qr_code_mess, strlen(qr_code_mess), &user_guide_qr_code);
        break;
    case ON_CHANGE_SHOW:
        /*根据需要调用以下三个接口:ui_remove_backcolor,ui_remove_backimage,ui_remove_border*/
        /* ui_remove_backcolor(elm);//移除控件背景颜色 */
        /* ui_remove_backimage(elm);//移除控件背景图像 */
        /* ui_remove_border(elm);//移除控件边界 */
        break;
    case ON_CHANGE_SHOW_POST:
        ui_core_get_element_abs_rect(elm, &rect); //跟随控件移动,注释掉这句则不跟随控件移动
        int draw_width = (rect.width > rect.height) ? rect.height : rect.width;
        if (draw_width <= user_guide_qr_code.img_w) {
            ASSERT(0);
        }
        ui_draw(dc,
                NULL,
                rect.left + (draw_width - user_guide_qr_code.img_w) / 2,
                rect.top + (draw_width - user_guide_qr_code.img_w) / 2,
                user_guide_qr_code.img_w,
                user_guide_qr_code.img_w,
                ui_draw_qrcode,
                (void *)&user_guide_qr_code,
                sizeof(struct _QR_CODE),
                0);
        break;

    case ON_CHANGE_RELEASE:
        if (row_img) {
            br28_free(row_img);
            row_img = 0;
        }
        if (copy_row_img) {
            br28_free(copy_row_img);
            copy_row_img = 0;
        }
        log_info("qr_code release:");
        MEM_STATUS();
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(LAYOUT_USER_GUIDE)//使用指南
.onchange = layout_user_guide_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};


static int layout_alipay_barcode_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct element *elm = (struct element *)_ctrl;
    struct draw_context *dc = (struct draw_context *)arg;
    struct rect rect = {0};

    switch (event) {
    case ON_CHANGE_INIT:
        printf("%s,%d \n", __func__, __LINE__);
        jl_bar_code_process(alipay_QRcode_mess, strlen(alipay_QRcode_mess), &alipay_bar_code);
        break;
    case ON_CHANGE_SHOW:
        /*根据需要调用以下三个接口:ui_remove_backcolor,ui_remove_backimage,ui_remove_border*/
        /* ui_remove_backcolor(elm);//移除控件背景颜色 */
        /* ui_remove_backimage(elm);//移除控件背景图像 */
        /* ui_remove_border(elm);//移除控件边界 */
        break;
    case ON_CHANGE_SHOW_POST:
        ui_core_get_element_abs_rect(elm, &rect); //跟随控件移动,注释掉这句则不跟随控件移动
        if (rect.width <= alipay_bar_code.img_w) {
            ASSERT(0);
        }
        if (rect.height <= alipay_bar_code.img_h) {
            ASSERT(0);
        }
        ui_draw(dc,
                NULL,
                rect.left + (rect.width - alipay_bar_code.img_w) / 2,
                rect.top + (rect.height - alipay_bar_code.img_h) / 2,
                alipay_bar_code.img_w,
                alipay_bar_code.img_h,
                ui_draw_barcode,
                (void *)&alipay_bar_code,
                sizeof(struct _BAR_CODE),
                0);
        break;

    case ON_CHANGE_RELEASE:
        if (row_img) {
            br28_free(row_img);
            row_img = 0;
        }
        if (copy_row_img) {
            br28_free(copy_row_img);
            copy_row_img = 0;
        }
        log_info("qr_code release:");
        MEM_STATUS();
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(UPAY_SHOW_BARCODE_LAYOUT)
.onchange = layout_alipay_barcode_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

#if 0
static int alipay_barcode_number_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct ui_text *text = (struct ui_text *)_ctrl;

    switch (event) {
    case ON_CHANGE_INIT:
        printf("%s,%d \n", __func__, __LINE__);
        ui_text_set_str(text, "ascii", (const char *)alipay_QRcode_mess, strlen(alipay_QRcode_mess), 0);
        break;
    default:
        break;
    }
    return false;
}
#else
static int alipay_barcode_number_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct ui_number *number = (struct ui_number *)_ctrl;
    struct unumber param = {0};

    switch (event) {
    case ON_CHANGE_INIT:
        printf("%s,%d \n", __func__, __LINE__);
        printf("str:%s \n", alipay_QRcode_mess);
        param.type = TYPE_STRING;
        param.num_str = alipay_QRcode_mess;
        ui_number_update(number, &param);
        break;
    default:
        break;
    }
    return false;
}
#endif
REGISTER_UI_EVENT_HANDLER(UPAY_SHOW_BARCODE_NUMBER)
.onchange = alipay_barcode_number_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};


static int layout_alipay_qrcode_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct element *elm = (struct element *)_ctrl;
    struct draw_context *dc = (struct draw_context *)arg;
    struct rect rect = {0};

    switch (event) {
    case ON_CHANGE_INIT:
        printf("%s,%d \n", __func__, __LINE__);
        jl_qr_code_process(alipay_QRcode_mess, strlen(alipay_QRcode_mess), &alipay_qr_code);
        break;
    case ON_CHANGE_SHOW:
        /*根据需要调用以下三个接口:ui_remove_backcolor,ui_remove_backimage,ui_remove_border*/
        /* ui_remove_backcolor(elm);//移除控件背景颜色 */
        /* ui_remove_backimage(elm);//移除控件背景图像 */
        /* ui_remove_border(elm);//移除控件边界 */
        break;
    case ON_CHANGE_SHOW_POST:
        ui_core_get_element_abs_rect(elm, &rect); //跟随控件移动,注释掉这句则不跟随控件移动
        int draw_width = (rect.width > rect.height) ? rect.height : rect.width;
        if (draw_width <= alipay_qr_code.img_w) {
            ASSERT(0);
        }
        ui_draw(dc,
                NULL,
                rect.left + (draw_width - alipay_qr_code.img_w) / 2,
                rect.top + (draw_width - alipay_qr_code.img_w) / 2,
                alipay_qr_code.img_w,
                alipay_qr_code.img_w,
                ui_draw_qrcode,
                (void *)&alipay_qr_code,
                sizeof(struct _QR_CODE),
                0);
        break;

    case ON_CHANGE_RELEASE:
        if (row_img) {
            br28_free(row_img);
            row_img = 0;
        }
        if (copy_row_img) {
            br28_free(copy_row_img);
            copy_row_img = 0;
        }
        log_info("qr_code release:");
        MEM_STATUS();
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(UPAY_SHOW_QRCODE_LAYOUT)
.onchange = layout_alipay_qrcode_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

static void upay_show_switch(void)
{
    alipay_show_barcode = !alipay_show_barcode;
    /* printf("touch up alipay_show_barcode :%d\n", alipay_show_barcode); */
    if (alipay_show_barcode == 0) {
        ui_hide(UPAY_SHOW_BARCODE_LAYOUT);
        ui_show(UPAY_SHOW_QRCODE_LAYOUT);
    } else {
        ui_hide(UPAY_SHOW_QRCODE_LAYOUT);
        ui_show(UPAY_SHOW_BARCODE_LAYOUT);
    }
    ui_text_show_index_by_id(UPAY_SHOW_SWITCH, alipay_show_barcode);
    ui_pic_show_image_by_id(UPAY_SHOW_SEL_0, alipay_show_barcode);
    ui_pic_show_image_by_id(UPAY_SHOW_SEL_1, alipay_show_barcode);
}

static int changecode_ontouch(void *ctr, struct element_touch_event *e)
{
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        return true;
        break;
    case ELM_EVENT_TOUCH_UP:
        upay_show_switch();
        break;
    default:
        return false;
        break;
    }
    return false;//不接管消息
}

REGISTER_UI_EVENT_HANDLER(UPAY_SHOW_SWITCH)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = changecode_ontouch,
};


static int upay_show_layout_ontouch(void *ctr, struct element_touch_event *e)
{
    switch (e->event) {
    case ELM_EVENT_TOUCH_L_MOVE:
        if (!alipay_show_barcode) {
            upay_show_switch();
            return true;
        }
        break;
    case ELM_EVENT_TOUCH_R_MOVE:
        if (alipay_show_barcode) {
            upay_show_switch();
            return true;
        }
        break;
    default:
        return false;
        break;
    }
    return false;//不接管消息
}

REGISTER_UI_EVENT_HANDLER(UPAY_SHOW_LAYOUT)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = upay_show_layout_ontouch,
};


#if TCFG_PAY_ALIOS_ENABLE
static void upay_check_binding_time(void *priv)
{
    if (alipay_time_id == 0) {
        return;
    }

    alipay_check_cnt ++;
    if (alipay_check_status == 2) { //bind ok
        if (alipay_check_cnt > ALIPAY_WARN_TIME_OUT_CNT) {
            UI_HIDE_CURR_WINDOW();
            UI_SHOW_WINDOW(PAGE_92);
            UI_WINDOW_BACK_DEL(PAGE_91);
        }
        return ;
    } else if (alipay_check_status == 3) { //bind fail
        if (alipay_check_cnt > ALIPAY_WARN_TIME_OUT_CNT) {
            /* UI_HIDE_CURR_WINDOW(); */
            /* UI_SHOW_WINDOW(PAGE_94); */
            /* UI_WINDOW_BACK_DEL(PAGE_92); */
            UI_WINDOW_BACK_SHOW(2);
        }
        return ;
    }
    int status = GET_BINDING_STU();
    log_info("quiry once, status is %02X now\n", status);
    if (BINDING_STU_OK == status) {
        log_info("Bind OK!\n");
        alipay_check_status = 2;
        alipay_check_cnt = 0;
        ui_hide(UPAY_BIND_QRCODE_LAYOUT);
        ui_hide(UPAY_BINDING_LAYOUT);
        ui_show(UPAY_BIND_SUCCEED_LAYOUT);
        AOS_NV_FLUSH();

    } else if ((BINDING_STU_FAIL == status) || (alipay_check_cnt > ALIPAY_CHECK_TIME_OUT_CNT)) {
        log_info("Bind FAIL!\n");
        alipay_check_status = 3;
        alipay_check_cnt = 0;
        ui_hide(UPAY_BIND_QRCODE_LAYOUT);
        ui_hide(UPAY_BINDING_LAYOUT);
        ui_show(UPAY_BIND_FAIL_LAYOUT);
    } else {
        if ((status >= BINDING_STU_GETTING_PROFILE) && (status <= BINDING_STU_FINISH_OK)) {
            log_info("Bind ing \n");
            if (alipay_check_status == 0) {
                alipay_check_status = 1;
                alipay_check_cnt = 0;
                ui_hide(UPAY_BIND_QRCODE_LAYOUT);
                ui_show(UPAY_BINDING_LAYOUT);
            } else {
                ui_pic_show_image_by_id(UPAY_BINDING_PIC, alipay_check_cnt % 10);
            }
        }
    }
}
static void upay_check_pay_time(void *priv)
{
    if (alipay_time_id == 0) {
        return;
    }

    alipay_check_cnt ++;
    if (alipay_check_cnt > ALIPAY_REFRESH_TIME_OUT_CNT) {
        alipay_check_cnt = 0;
        log_info("refresh\n");
        u32 msg_len = ALIPAY_QRCODE_BUF_LEN;
        GET_PAYCODE(alipay_QRcode_mess, &msg_len);    //获取支付码
        ui_hide(UPAY_SHOW_BARCODE_LAYOUT);
        ui_hide(UPAY_SHOW_QRCODE_LAYOUT);
        if (alipay_show_barcode == 0) {
            ui_show(UPAY_SHOW_QRCODE_LAYOUT);
        } else {
            ui_show(UPAY_SHOW_BARCODE_LAYOUT);
        }
    }

}
#endif /* #if TCFG_PAY_ALIOS_ENABLE */

static int layout_alipay_bind_qrcode_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct element *elm = (struct element *)_ctrl;
    struct draw_context *dc = (struct draw_context *)arg;
    struct rect rect = {0};

    switch (event) {
    case ON_CHANGE_INIT:
        jl_qr_code_process(alipay_QRcode_mess, strlen(alipay_QRcode_mess), &alipay_qr_code);
        break;
    case ON_CHANGE_SHOW:
        /*根据需要调用以下三个接口:ui_remove_backcolor,ui_remove_backimage,ui_remove_border*/
        /* ui_remove_backcolor(elm);//移除控件背景颜色 */
        /* ui_remove_backimage(elm);//移除控件背景图像 */
        /* ui_remove_border(elm);//移除控件边界 */
        break;
    case ON_CHANGE_SHOW_POST:
        ui_core_get_element_abs_rect(elm, &rect); //跟随控件移动,注释掉这句则不跟随控件移动
        int draw_width = (rect.width > rect.height) ? rect.height : rect.width;
        if (draw_width <= alipay_qr_code.img_w) {
            ASSERT(0);
        }
        ui_draw(dc,
                NULL,
                rect.left + (draw_width - alipay_qr_code.img_w) / 2,
                rect.top + (draw_width - alipay_qr_code.img_w) / 2,
                alipay_qr_code.img_w,
                alipay_qr_code.img_w,
                ui_draw_qrcode,
                (void *)&alipay_qr_code,
                sizeof(struct _QR_CODE),
                0);
        break;

    case ON_CHANGE_RELEASE:
        if (row_img) {
            br28_free(row_img);
            row_img = 0;
        }
        if (copy_row_img) {
            br28_free(copy_row_img);
            copy_row_img = 0;
        }
        log_info("qr_code release:");
        MEM_STATUS();
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(UPAY_BIND_QRCODE_LAYOUT)
.onchange = layout_alipay_bind_qrcode_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

static int layout_alipay_bind_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    switch (event) {
    case ON_CHANGE_INIT:
#if TCFG_PAY_ALIOS_ENABLE
        alipay_check_cnt = 0;
        alipay_check_status = 0;
        ui_auto_shut_down_disable();
        printf("Before init ,status is %02X\n", GET_BINDING_STU());
        printf("upay ver:%s \n", GET_VERSION());
        ALIOS_CACHE_INIT();
        u8 ret = ENV_INIT();    //环境初始化，此时状态应该为STATUS_START_BINDING
        printf("After init ,status is %02X, ret:%d \n", GET_BINDING_STU(), ret);
        upay_ble_mode_enable(1);
        u32 msg_len = ALIPAY_QRCODE_BUF_LEN;
        ret = GET_BINDING_STRING(alipay_QRcode_mess, &msg_len);    //获取绑定码
        printf("upay_get_binding_string, len:%d, ret:%d \n", msg_len, ret);
        printf("%s\n", alipay_QRcode_mess);
        alipay_time_id = sys_timer_add(NULL, upay_check_binding_time, ALIPAY_CHECK_TIME_MS);
#endif /* #if TCFG_PAY_ALIOS_ENABLE */
        break;
    case ON_CHANGE_RELEASE:
#if TCFG_PAY_ALIOS_ENABLE
        if (alipay_time_id) {
            sys_timer_del(alipay_time_id);
            alipay_time_id = 0;
        }
        ENV_DEINIT();    //注销Alipay绑定环境
        ALIOS_CACHE_UNINIT();
        upay_ble_mode_enable(0);
        ui_auto_shut_down_enable();
#endif /* #if TCFG_PAY_ALIOS_ENABLE */
        break;
    default:
        break;
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(PAGE_91)
.onchange = layout_alipay_bind_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

static int layout_sel_pay_ontouch(void *ctr, struct element_touch_event *e)
{
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        return true;
        break;
    case ELM_EVENT_TOUCH_UP:
        UI_HIDE_CURR_WINDOW();
        UI_SHOW_WINDOW(PAGE_93);
        return true;
        break;
    default:
        return false;
        break;
    }
    return false;//不接管消息
}

REGISTER_UI_EVENT_HANDLER(UPAY_FUNC_SEL_PAY_LAYOUT)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = layout_sel_pay_ontouch,
};

static int layout_sel_set_ontouch(void *ctr, struct element_touch_event *e)
{
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        return true;
        break;
    case ELM_EVENT_TOUCH_UP:
        ui_hide(UPAY_FUNC_SEL_LAYOUT);
        ui_show(UPAY_UNBIND_SET_LAYOUT);
        return true;
        break;
    default:
        return false;
        break;
    }
    return false;//不接管消息
}

REGISTER_UI_EVENT_HANDLER(UPAY_FUNC_SEL_SET_LAYOUT)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = layout_sel_set_ontouch,
};

static int layout_unbind_set_button_ontouch(void *ctr, struct element_touch_event *e)
{
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        return true;
        break;
    case ELM_EVENT_TOUCH_UP:
        ui_hide(UPAY_UNBIND_SET_LAYOUT);
        ui_show(UPAY_UNBIND_ACK_LAYOUT);
        return true;
        break;
    default:
        return false;
        break;
    }
    return false;//不接管消息
}

REGISTER_UI_EVENT_HANDLER(UPAY_UNBIND_SET_BUTTON)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = layout_unbind_set_button_ontouch,
};

static int layout_unbind_set_ontouch(void *ctr, struct element_touch_event *e)
{
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        return true;
        break;
    case ELM_EVENT_TOUCH_R_MOVE:
        ui_hide(UPAY_UNBIND_SET_LAYOUT);
        ui_show(UPAY_FUNC_SEL_LAYOUT);
        return true;
        break;
    default:
        return false;
        break;
    }
    return false;//不接管消息
}

REGISTER_UI_EVENT_HANDLER(UPAY_UNBIND_SET_LAYOUT)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = layout_unbind_set_ontouch,
};

#if TCFG_PAY_ALIOS_ENABLE
static int alipay_unbind_set_name_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct ui_text *text = (struct ui_text *)_ctrl;
    u32 len = ALIPAY_QRCODE_BUF_LEN;
    u8 *dat = alipay_QRcode_mess;
    switch (event) {
    case ON_CHANGE_INIT:
        printf("%s,%d \n", __func__, __LINE__);
        len --;
        GET_NICK_NAME(dat, &len);
        if (len) {
            dat[len] = 0;
            put_buf(dat, len);
            text->attrs.default_code = 0x25a1;
            ui_text_set_utf8_str(text, "text", (const char *)dat, len, FONT_DEFAULT);
        }
        break;
    default:
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(UPAY_UNBIND_SET_NAME)
.onchange = alipay_unbind_set_name_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

static int alipay_unbind_set_id_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    struct ui_text *text = (struct ui_text *)_ctrl;
    u32 len = ALIPAY_QRCODE_BUF_LEN;
    u8 *dat = alipay_QRcode_mess;
    switch (event) {
    case ON_CHANGE_INIT:
        printf("%s,%d \n", __func__, __LINE__);
        dat += len;
        len --;
        GET_LOGON_ID(dat, &len);
        if (len) {
            dat[len] = 0;
            put_buf(dat, len);
            ui_text_set_str(text, "ascii", (const char *)dat, len, 0);
        }
        break;
    default:
        break;
    }
    return false;
}
REGISTER_UI_EVENT_HANDLER(UPAY_UNBIND_SET_ID)
.onchange = alipay_unbind_set_id_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

#endif /* #if TCFG_PAY_ALIOS_ENABLE */

static int layout_unbind_ack_sure_button_ontouch(void *ctr, struct element_touch_event *e)
{
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        return true;
        break;
    case ELM_EVENT_TOUCH_UP:
#if TCFG_PAY_ALIOS_ENABLE

        UNBINDING();
        AOS_NV_FLUSH();
#endif /* #if TCFG_PAY_ALIOS_ENABLE */
        ui_hide(UPAY_UNBIND_ACK_LAYOUT);
        ui_show(UPAY_UNBIND_WARN_LAYOUT);
        return true;
        break;
    default:
        return false;
        break;
    }
    return false;//不接管消息
}

REGISTER_UI_EVENT_HANDLER(UPAY_UNBIND_ACK_SURE_BUTTON)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = layout_unbind_ack_sure_button_ontouch,
};

static int layout_unbind_ack_cancle_button_ontouch(void *ctr, struct element_touch_event *e)
{
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        return true;
        break;
    case ELM_EVENT_TOUCH_UP:
        ui_hide(UPAY_UNBIND_ACK_LAYOUT);
        ui_show(UPAY_UNBIND_SET_LAYOUT);
        return true;
        break;
    default:
        return false;
        break;
    }
    return false;//不接管消息
}

REGISTER_UI_EVENT_HANDLER(UPAY_UNBIND_ACK_CANCLE_BUTTON)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = layout_unbind_ack_cancle_button_ontouch,
};

static int layout_unbind_ack_ontouch(void *ctr, struct element_touch_event *e)
{
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        break;
    case ELM_EVENT_TOUCH_R_MOVE:
        ui_hide(UPAY_UNBIND_ACK_LAYOUT);
        ui_show(UPAY_UNBIND_SET_LAYOUT);
        return true;
        break;
    default:
        return false;
        break;
    }
    return false;//不接管消息
}

REGISTER_UI_EVENT_HANDLER(UPAY_UNBIND_ACK_LAYOUT)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = layout_unbind_ack_ontouch,
};

static int layout_unbind_warn_button_ontouch(void *ctr, struct element_touch_event *e)
{
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        return true;
        break;
    case ELM_EVENT_TOUCH_UP:
        UI_WINDOW_BACK_SHOW(2);
        return true;
        break;
    default:
        return false;
        break;
    }
    return false;//不接管消息
}

REGISTER_UI_EVENT_HANDLER(UPAY_UNBIND_WARN_BUTTON)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = layout_unbind_warn_button_ontouch,
};

static void upay_unbind_warn_time(void *priv)
{
    if (alipay_time_id == 0) {
        return;
    }
    UI_WINDOW_BACK_SHOW(2);
}

static int layout_unbind_warn_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    switch (event) {
    case ON_CHANGE_INIT:
        alipay_time_id = sys_timeout_add(NULL, upay_unbind_warn_time, 3000);
        break;
    case ON_CHANGE_RELEASE:
        if (alipay_time_id) {
            sys_timeout_del(alipay_time_id);
            alipay_time_id = 0;
        }
        break;
    default:
        break;
    }
    return false;
}

static int layout_unbind_warn_ontouch(void *ctr, struct element_touch_event *e)
{
    return true;
}

REGISTER_UI_EVENT_HANDLER(UPAY_UNBIND_WARN_LAYOUT)
.onchange = layout_unbind_warn_onchange,
 .onkey = NULL,
  .ontouch = layout_unbind_warn_ontouch,
};


static int layout_alipay_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    switch (event) {
    case ON_CHANGE_INIT:
        printf("%s,%d \n", __func__, __LINE__);
#if TCFG_PAY_ALIOS_ENABLE
        alipay_check_cnt = 0;
        alipay_check_status = 0;
        alipay_show_barcode = 0;
        ui_auto_shut_down_disable();
        upay_ble_mode_enable(1);
        u32 msg_len = ALIPAY_QRCODE_BUF_LEN;
        GET_PAYCODE(alipay_QRcode_mess, &msg_len);    //获取支付码
        /* strcpy(alipay_QRcode_mess, "1234567890123456789"); */
        alipay_time_id = sys_timer_add(NULL, upay_check_pay_time, ALIPAY_CHECK_TIME_MS);
#endif /* #if TCFG_PAY_ALIOS_ENABLE */
        break;
    case ON_CHANGE_RELEASE:
        printf("%s,%d \n", __func__, __LINE__);
#if TCFG_PAY_ALIOS_ENABLE
        if (alipay_time_id) {
            sys_timer_del(alipay_time_id);
            alipay_time_id = 0;
        }
        upay_ble_mode_enable(0);
        ui_auto_shut_down_enable();
#endif /* #if TCFG_PAY_ALIOS_ENABLE */
        break;
    default:
        break;
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(PAGE_93)
.onchange = layout_alipay_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

static void upay_startup_time(void *priv)
{
    if (alipay_time_id == 0) {
        return;
    }

    u32 page = PAGE_91;
#if TCFG_PAY_ALIOS_ENABLE
    BINDING_STU_E bind_status = GET_BINDING_STU();
    printf("binding status:0x%x \n", (int)bind_status);
    if (bind_status == BINDING_STU_OK) {
        /* page = PAGE_93; */
        page = PAGE_92;
    }
#endif /* #if TCFG_PAY_ALIOS_ENABLE */
    UI_HIDE_CURR_WINDOW();
    UI_SHOW_WINDOW(page);
    UI_WINDOW_BACK_DEL(PAGE_94);
}

static int alipay_startup_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    switch (event) {
    case ON_CHANGE_INIT:
        printf("%s,%d \n", __func__, __LINE__);
#if TCFG_PAY_ALIOS_ENABLE
        POWER_ON();
#endif
        alipay_time_id = sys_timeout_add(NULL, upay_startup_time, 2000);
        break;
    case ON_CHANGE_RELEASE:
        printf("%s,%d \n", __func__, __LINE__);
        if (alipay_time_id) {
            sys_timeout_del(alipay_time_id);
            alipay_time_id = 0;
        }
        break;
    default:
        break;
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(PAGE_94)
.onchange = alipay_startup_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

int alipay_enter_check(int *window)
{
#if 0 //TCFG_PAY_ALIOS_ENABLE
    upay_binding_status_e bind_status = upay_get_binding_status();
    printf("binding status:0x%x \n", (int)bind_status);
    if (bind_status == UPAY_STATUS_BINDING_OK) {
        /* *window = PAGE_93; */
        *window = PAGE_92;
    }
#endif /* #if TCFG_PAY_ALIOS_ENABLE */
    return true;
}

#endif
#endif
/* #endif #<{(| #if (!TCFG_LUA_ENABLE) |)}># */

