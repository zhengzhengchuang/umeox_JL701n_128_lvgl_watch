#include "app_config.h"
#include "ui/ui_style.h"
#include "ui/ui.h"
#include "ui/ui_api.h"
#include "system/timer.h"
#include "res/resfile.h"
#include "ui/res_config.h"
#include "ui/ui_resource.h"

#if (!TCFG_LUA_ENABLE)
#ifdef CONFIG_UI_STYLE_JL_ENABLE
#if TCFG_UI_ENABLE_PRESSURE

#define STYLE_NAME  JL

/* REGISTER_UI_STYLE(STYLE_NAME) */

#define LOG_TAG_CONST       UI
#define LOG_TAG     		"[UI-ACTION]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"


#define ui_text_for_id(id) \
	({ \
		struct element *elm = ui_core_get_element_by_id(id); \
	 	elm ? container_of(elm, struct ui_text, elm): NULL; \
	 })
#define ui_number_for_id(id) \
    ({ \
        struct element *elm = ui_core_get_element_by_id(id); \
        elm ? (struct ui_number *)elm: NULL; \
     })

static int pressure_onchange(void *_ctrl, enum element_change_event event, void *arg)
{
    static struct unumber n1, n2, n3;    //数字显示源
    struct element *elm = (struct element *)_ctrl;
    struct draw_context *dc = (struct draw_context *)arg;
    struct rect rect = {0};
    static u8 time = 0;
    static u8 hour[24];
    static u8 num[24];
    switch (event) {
    case ON_CHANGE_INIT:
        time++;
        n1.type = TYPE_NUM;
        n1.numbs = 1;
        n1.number[0] = rand32() % 100; //未读传感器值，这里用随机数代替，取余获取较小数值
        n2.type = TYPE_NUM;  //数据类型，分数字和字符串
        n2.numbs = 1;
        n2.number[0] = rand32() % 100;
        n3.type = TYPE_NUM;
        n3.numbs = 1;
        n3.number[0] = n1.number[0] / 2 + n2.number[0] / 2;
        num[time - 1] = n3.number[0];  //利用数组记录历史值，供下面循环使用
        hour[time - 1] = time - 1;
        ui_number_update(ui_number_for_id(PRESSURE_HIGH_NUM), &n1);  //更新需要显示的数据源
        ui_number_update(ui_number_for_id(PRESSURE_LOW_NUM), &n2);
        ui_number_update(ui_number_for_id(PRESSURE_AVER_NUM), &n3);
        if (n3.number[0] <= 30) {
            ui_text_set_index(ui_text_for_id(PRESSURE_STATE), 0);    //选择图片控件的偏移,不同的“压力值”对应不同图片
        } else if (n3.number[0] > 30 && n3.number[0] <= 50) {
            ui_text_set_index(ui_text_for_id(PRESSURE_STATE), 1);
        } else if (n3.number[0] > 50 && n3.number[0] <= 80) {
            ui_text_set_index(ui_text_for_id(PRESSURE_STATE), 2);
        } else {
            ui_text_set_index(ui_text_for_id(PRESSURE_STATE), 3);
        }
        break;
    case ON_CHANGE_SHOW:
        log_info("DRAW_IMG_onchange_ON_CHANGE_SHOW_POST\n");
        /* ui_remove_backcolor(elm);//移除控件背景颜色 */
        /* ui_remove_backimage(elm);//移除控件背景图像 */
        ui_remove_border(elm);//移除控件边界
        break;
    case ON_CHANGE_SHOW_POST:
        ui_core_get_element_abs_rect(elm, &rect); //跟随控件移动,注释掉这句则不跟随控件移动
        /* 以下绘图函数只能在该控件的范围内显示，超出控件区域不显示    */
        //填充矩形
        ui_draw_rect(dc, 0, 30, 240, 157, 0x7bef);   //绘制柱形图外框
        for (short i = 0; i < 6 ; i++) {             //绘制柱形图区域虚线
            for (short j = 0; j < 43; j++) {
                if (j % 2) {
                    ui_fill_rect(dc, j * 5, 41 + i * 24, 5, 1, 0x0000);
                } else {
                    ui_fill_rect(dc, j * 5, 41 + i * 24, 5, 1, 0x7bef);
                }
            }
        }
        for (short cnt = 0; cnt < 24; cnt++) {   //绘制柱形图，并通过循环保留之前的数据
            if (num[cnt] == 0) { //判断当前位置之后有无数据，如无直接返回，避免多余计算
                break;
            }
            if (num[cnt] <= 30) {
                ui_fill_rect(dc, hour[cnt] * 10, 41 + 120 - (int)(num[cnt] * 1.2), 9, (int)(num[cnt] * 1.2), 0x07e0);
            } else if (num[cnt] > 30 && num[cnt] <= 50) {
                ui_fill_rect(dc, hour[cnt] * 10, 41 + 120 - (int)(num[cnt] * 1.2), 9, (int)(num[cnt] * 1.2), 0x03e0);  //其中数字部分区别于ui工具有所不同，颜色值参考RGB565
            } else if (num[cnt] > 50 && num[cnt] <= 80) {
                ui_fill_rect(dc, hour[cnt] * 10, 41 + 120 - (int)(num[cnt] * 1.2), 9, (int)(num[cnt] * 1.2), 0xf81f);
            } else {
                ui_fill_rect(dc, hour[cnt] * 10, 41 + 120 - (int)(num[cnt] * 1.2), 9, (int)(num[cnt] * 1.2), 0xf800);
            }
        }
        break;
    case ON_CHANGE_RELEASE:
        if (time > 20) {  //超出显示长度清零，重新开始显示,共显示21条柱形图，可调节ui和柱形图的宽度调节显示的柱形图数量，这里不再作修改
            memset(hour, 0, sizeof(hour));
            memset(num, 0, sizeof(num));
            time = 0;
        }
        break;
    default:
        break;
    }
    return false;
}

REGISTER_UI_EVENT_HANDLER(PRESSURE)   //底层布局id
.onchange = pressure_onchange,
 .onkey = NULL,
  .ontouch = NULL,
};

#endif /* #if TCFG_UI_ENABLE_PRESSURE */
#endif
#endif /* #if (!TCFG_LUA_ENABLE) */

