#include "app_config.h"
#include "includes.h"
#include "typedef.h"
#include "clock_cfg.h"
#include "app_task.h"
#include "key_event_deal.h"
#include "ui/lcd_spi/lcd_drive.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_demo_conf.h"
#include "ui.h"
#include "ui_helpers.h"
#include "lvgl_main.h"
#include "../../../../common/ui/lv_watch/lv_watch.h"
#include "../../../../common/ui/lv_watch/include/ui_act_id.h"
#include "../../../../common/ui/lv_watch/poc_modem/poc_modem_ui.h"

#if LVGL_TEST_ENABLE

#if LV_USE_LOG && LV_LOG_PRINTF
// static void lv_rt_log(const char *buf)
// {
//     printf(buf);
// }
#endif

#if 0
enum {
    UI_MSG_OTHER,
    UI_MSG_KEY,
    UI_MSG_TOUCH,
    UI_MSG_SHOW,
    UI_MSG_HIDE,
};
#endif

void rounder_cb(lv_disp_drv_t *disp_drv, lv_area_t *area) {
    area->x1 = (area->x1 / 2) * 2; // Round down x1 to the nearest multiple of 2
    area->x2 = (area->x2 / 2) * 2 + 1; // Round down x2 to the nearest multiple of 2
}

void render_start_cb(struct _lv_disp_drv_t * disp_drv){
    // extern void lcd_wait_te();
    //  lcd_wait_te();
}

static u16 bl_timer = 0;
static void bl_timer_cb(void *priv)
{
    if(bl_timer)
        sys_timeout_del(bl_timer);
    bl_timer = 0;

    struct lcd_interface *lcd = lcd_get_hdl();

    u8 bl_val = \
        GetVmParaCacheByLabel(vm_label_backlight);

    if(lcd->backlight_ctrl)
        lcd->backlight_ctrl(bl_val);

    return;
}

static void lvgl_task(void *p)
{
    // sd_jpg_test();
    int msg[64];
    int ret;

#if LV_USE_LOG && LV_LOG_PRINTF
    //lv_log_register_print_cb(lv_rt_log);
#endif

    lv_init();

    vm_store_para_init();
    ui_act_id_t act_id = ui_act_id_watchface;
    int DevBondFlag = \
        GetVmParaCacheByLabel(vm_label_dev_bond);
    if(DevBondFlag == false)
        act_id = ui_act_id_dev_bond;
    ui_info_cache_init(act_id);
    
    lv_port_disp_init(p);

    //  等待模式任务启动再启动LVGL
    while(!app_get_curr_task())
    {
        printf("checks\n");
        os_time_dly(10);
    }

    lcd_sleep_ctrl(0);

    lv_disp_get_default()->driver->render_start_cb = \
        render_start_cb;

    // Set the rounder_cb function    
    lv_disp_get_default()->driver->rounder_cb = \
        rounder_cb;

    lv_port_indev_init();
 
    //lv_port_fs_init();

    ui_menu_jump(act_id);

    if(!bl_timer)
        sys_timeout_add(NULL, bl_timer_cb, 50);
#if 0

#if LV_USE_DEMO_WIDGETS
    lv_demo_widgets();
#endif /* #if LV_USE_DEMO_WIDGETS */

#if LV_USE_DEMO_STRESS
    lv_demo_stress();
#endif /* #if LV_USE_DEMO_STRESS */

#if LV_USE_DEMO_MUSIC
    // lv_demo_music();
#endif /* #if LV_USE_DEMO_MUSIC */

    //  官方表盘DEMO
    //ui_init();

    //  滑屏图片测试DEMO
    //lv_example_tileview_1();

    //自定义
    // lv_img_dsc_t img_dst;
    // open_fd("usr_nor");
    // #include "ui/ui/jl_images/FILE_index.h"
    // #define RES_BASE_ADDR   0x600000   //自定义区起始地址
    // // lv_open_res(get_res_fd(), RES_BASE_ADDR, 0, FILE_index[FILE_bird_1], &img_dst); //54hz
    // // lv_open_res(get_res_fd(), RES_BASE_ADDR, 0, FILE_index[FILE_ARGB8565_bird_1_dat], &img_dst); //58hzFILE_1199_jpg
    // lv_open_res(get_res_fd(), RES_BASE_ADDR, 0, FILE_index[FILE_1234_jpg], &img_dst); //52hz
    // lv_obj_t *img_obj = lv_img_create(lv_scr_act());
    // lv_img_set_src(img_obj, &img_dst);
    // lv_obj_set_size(img_obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    // lv_obj_set_align(img_obj, LV_ALIGN_CENTER);

    //  文件系统读bin图片
    // lv_img_dsc_t img_dst;
    // extern u32 get_file_addr(char*);
    // // u32 addr = get_file_addr("storage/virfat_flash/C/lvimg/bird_1.bin");//55hz
    // printf("addr : %x", addr);
    // memcpy(&(img_dst.header), addr, sizeof(lv_img_header_t));
    // img_dst.data = addr + sizeof(lv_img_header_t);
    // img_dst.data_size = img_dst.header.h * img_dst.header.w * 2,
    // printf("zero %d, cf %d, h %d, re %d, w %d, addr %x, size %d",
    //     img_dst.header.always_zero,
    //     img_dst.header.cf,
    //     img_dst.header.h,
    //     img_dst.header.reserved,
    //     img_dst.header.w,
    //     img_dst.data,
    //     img_dst.data_size
    // );
    // lv_obj_t *img_obj = lv_img_create(lv_scr_act());
    // lv_img_set_src(img_obj, &img_dst);
    // lv_obj_set_size(img_obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    // lv_obj_set_align(img_obj, LV_ALIGN_CENTER);

    //  FS文件系统读取,
    // lv_obj_t *img_obj = lv_img_create(lv_scr_act());
    // #include "ui/ui/jl_images/usr_pic_index.h"
    // // lv_img_set_src(img_obj, "A:"A_SMARTWEAR_UI_BIRD_1);//58hz
    // // lv_img_set_src(img_obj, "B:storage/virfat_flash/C/lvimg/bird_1.bin");//55hz
    // // lv_img_set_src(img_obj, "P:storage/virfat_flash/C/lvimg/bird_1.bin");//不缓存头信息：14hz //缓存头信息：15hz
    // lv_img_set_src(img_obj, "B:storage/virfat_flash/C/lvimg/1234.jpg");//53hz
    // // lv_img_set_src(img_obj, "P:storage/virfat_flash/C/lvimg/1234.jpg");//解不了

    //  背景透明度测试, 打开my_draw_blend优化效果： lv-17FPS -> imb-37FPS
    // static lv_style_t style;
    // lv_style_init(&style);

    // /*Set a background color and a radius*/
    // lv_style_set_radius(&style, 10);
    // lv_style_set_bg_opa(&style, LV_OPA_60);
    // lv_style_set_bg_color(&style, lv_palette_lighten(LV_PALETTE_GREY, 1));

    // /*Add outline*/
    // lv_style_set_outline_width(&style, 2);
    // lv_style_set_outline_color(&style, lv_palette_main(LV_PALETTE_BLUE));
    // lv_style_set_outline_pad(&style, 8);

    // /*Create an object with the new style*/
    // lv_obj_t * obj = lv_obj_create(lv_scr_act());
    // lv_obj_add_style(obj, &style, 0);
    // lv_obj_center(obj);
    // lv_obj_set_size(obj, 454, 454);
#endif

    // 用户动态控制是否等TE信号，
    // 开PSRAM下，等TE完全消除切线，不等TE偶尔有轻微切线。
    // 不开PSRAM下，分屏刷要看绘图复杂度,等TE可能消除切线，不等TE切线比较明显
    // 等TE的帧率 = TE频率/2，帧率是固定的，如屏幕的TE是44Hz，那么实际刷新是22Hz
    // 不等TE的帧率 = 极限刷屏的帧率
    SetUsrWaitTe(1);

    //  用户动态控制是否全屏刷
    // extern volatile u8 usr_full_screen;
    // usr_full_screen = 1;

    // static int last_time = 0;
    // int time_till_next = 0; //  距离下一次执行时间，此时间与待机低功耗相关

    int next = 0;
    while(1) 
    {   
        /* LVGL任务 */
        if(!lcd_sleep_status())
            next = lv_task_handler() / 10;
        else
            next = 500; 
    
        /* 挂起并接收消息 */
        if(next)
        {
            if(next>10) 
                printf("lv tick %d ms", next*10);
                
            ret = os_taskq_pend_timeout(NULL, msg, ARRAY_SIZE(msg), next);
        }else 
        {
            ret = os_taskq_accept(ARRAY_SIZE(msg), msg);      
            if(!lcd_sleep_status()) wdt_clear();              
        }

        if(1 && !lcd_sleep_status())
            lv_obj_invalidate(lv_scr_act());

        /* 处理消息 */
        if(ret == OS_TASKQ)
            ui_msg_handle(msg, ARRAY_SIZE(msg));
#if 0
        printf("time_till_next = %d\n", time_till_next);
        if(time_till_next != 0)
            printf("**********%s\n", __func__);
        #if 1
        /* 挂起并接收消息 */
        if(time_till_next == -1)
        {
            //休眠后一直pend
            printf("lv os_taskq_pend\n");
            ret = os_taskq_pend(NULL, msg, ARRAY_SIZE(msg));  
        }else if(time_till_next >= 10)
        { 
            //下一次执行时间，单位10ms
            ret = os_taskq_pend_timeout(NULL, msg, ARRAY_SIZE(msg), time_till_next/10);  
        }else
        {   
            //小于10ms，不挂起，提高绘图效率
            ret  = os_taskq_accept(ARRAY_SIZE(msg), msg); 
            printf("ret = %d\n", ret);
            wdt_clear();

            printf("msec = %d\n", jiffies_msec()-last_time);
            if(jiffies_msec()-last_time>=2000) 
            {
                last_time = jiffies_msec();
                printf("--delay 10--"); 
                os_time_dly(1);
            }
        }

        /* 处理消息 */
        if(ret == OS_TASKQ)
            ui_msg_handle(msg, ARRAY_SIZE(msg));

        /* 运行LVGL和休眠处理 */
        time_till_next = lcd_sleep_status()?-1:lv_task_handler(); //LVGL服务函数，返回下一次执行时间

        lv_obj_invalidate(lv_scr_act());//触发全屏重绘
        #endif
#endif
    }
}

int lvgl_test_init(void *param)
{
    int err = 0;
    clock_add_set(DEC_UI_CLK);
    err = task_create(lvgl_task, param, UI_TASK_NAME);
    if (err) {
        r_printf("ui task create err:%d \n", err);
    }
    return err;
}

void ui_msg_handle(int *msg, u8 len)
{
    if(!msg || len == 0)
        return;

    int msg_cmd = msg[0];

    switch(msg_cmd)
    {
        case ui_msg_menu_jump:
            ui_act_id_t act_id = msg[1];
            ui_menu_jump_handle(act_id);  
            break;

        case ui_msg_menu_refresh:
            common_refresh_msg_handle();
            break;

        case ui_msg_menu_offscreen:
            common_offscreen_msg_handle();
            break;

        case ui_msg_clp_refresh:
            common_clock_pointer_refresh();
            break;
        
        case ui_msg_widget_refresh:
            widget_refresh_handle();
            break;

        case ui_msg_key_handle:
            common_key_msg_handle(msg[1], msg[2]);
            break;

        case ui_msg_rdec_handle:
            common_rdec_msg_handle(msg[1]);
            break;

        case ui_msg_nor_vm_hr:
            VmHrCtxFlashWrite();
            break;

        case ui_msg_nor_vm_bo:
            VmBoCtxFlashWrite();
            break;

        case ui_msg_nor_vm_clear:
            ResetAllNorVmData();
            break;

        case ui_msg_nor_vm_sleep:
            break;

        case ui_msg_nor_vm_weather:
            WeatherInfoParaUpdate();
            break;

        case ui_msg_nor_vm_message:
            MsgNotifyProcess();
            break;

        case ui_msg_nor_vm_call_log:
            VmCallLogCtxFlashWrite();
            break;

        case ui_msg_nor_vm_contacts:
            UpdateContactsVmFlash();
            break;

        case ui_msg_unbond_handle:
            DevUnBondHandle();
            break;

        case ui_msg_ori_bond_handle:
            OriDevBondHandle();
            break;

        case ui_msg_new_bond_handle:
            NewDevBondHandle();
            break;

        case ui_msg_first_bond_handle:
            FirstDevBondHandle();
            break;
        
        default:
            printf("*************ui msg not found\n");
            break;
    }

    return;
}

int post_ui_msg(int *msg, u8 len)
{
    int err = 0;
    int count = 0;

__retry:
    err = os_taskq_post_type(UI_TASK_NAME, msg[0], len - 1, &msg[1]);

    if (cpu_in_irq() || cpu_irq_disabled()) {
        return err;
    }

    if (err) {
        if (!strcmp(os_current_task(), UI_TASK_NAME)) {
            return err;
        }
        if (count > 20) {
            return -1;
        }
        count++;
        os_time_dly(1);
        goto __retry;
    }
    return err;
}

void ui_key_msg_post(int key_value, int key_event)
{
    int key_msg[3] = {0xff};

    key_msg[0] = ui_msg_key_handle;
    key_msg[1] = key_value;
    key_msg[2] = key_event;
    post_ui_msg(key_msg, 3);

    return;
}

static u8 lv_idle_query(void)
{
    if(lcd_sleep_status())
        return 1;

    return 0;
}

REGISTER_LP_TARGET(lv_lp_target) = {
    .name = "lv_lp",
    .is_idle = lv_idle_query,
};

#if 0
int ui_key_msg_post(int key)
{
    u8 count = 0;
    int msg[8];

    if (key >= 0x80) {
        printf("key need <= 0x80");
        return -1;
    }

    msg[0] = UI_MSG_KEY;
    msg[1] = key;
    /* touch_msg_counter++; */
    return post_ui_msg(msg, 2);
}

void lvgl_enter_sleep(){
    int msg[8];
    msg[0] = UI_MSG_HIDE;
    post_ui_msg(msg, 1);
}

void lvgl_exit_sleep(){
    int msg[8];
    msg[0] = UI_MSG_SHOW;
    post_ui_msg(msg, 1);    
}
#endif

#endif /* #if LVGL_TEST_ENABLE */

