
#include "stdlib.h"
#include "app_config.h"
#include "ui/ui_api.h"
#include "ui/ui.h"
#include "ui/ui_style.h"
#include "app_task.h"
#include "system/timer.h"
#include "btstack/avctp_user.h"
#include "app_main.h"
#include "init.h"
#include "key_event_deal.h"
#include <math.h>
//#include "math_fast_funcion.h"

#if TCFG_UI_ENABLE && (!TCFG_LUA_ENABLE)
#if TCFG_UI_ENABLE_CALCULATOR

#ifdef CONFIG_UI_STYLE_JL_ENABLE
#define STYLE_NAME  JL
#if 0
#define log_debug printf
#else
#define log_debug
#endif
#define SHOW_NUM_MAX 		9 //最大显示位数
#define IN_NUM_MAX 			7 //最大输入位数，包含符号位

#define abs(x) ((x>0.0)?(x):(-x))
#define dir(x) ((x>0.000001)?(1):(-1))
enum {
    NONE,
    ADD,
    SUB,
    MUL,
    DIV,
};
struct ui_calculator {
    double num_1;
    double num_2;
    u8 cal_mode;//运算模式
    u8 point_1;//BIT7记录是否有效
    u8 point_2;//BIT7记录是否有效
    char negative;
    u8 str[SHOW_NUM_MAX];
};

static struct ui_calculator __calculator = {
    .negative = 1,
};
#define __this (&__calculator)
//重置参数
static void calculator_reset(void)
{
    __this->num_1 = 0.0;
    __this->num_2 = 0.0;
    __this->point_1 = 0;
    __this->point_2 = 0;
    __this->negative = 1;
    __this->cal_mode = NONE;
    memset(__this->str, 0, SHOW_NUM_MAX);
}
//字符转浮点
static double __atof(char *str)
{
    double s = 0.0;
    double d = 10.0;
    int jishu = 0;
    bool falg = false;
    while (*str == ' ') {
        str++;
    }
    if (*str == '-') { //记录数字正负
        falg = true;
        str++;
    }
    if (!(*str >= '0' && *str <= '9')) { //如果一开始非数字则退出，返回0.0
        return s;
    }
    while (*str >= '0' && *str <= '9' && *str != '.') { //计算小数点前整数部分
        s = s * 10.0 + *str - '0';
        str++;
    }
    if (*str == '.') { //以后为小数部分
        str++;
    }
    while (*str >= '0' && *str <= '9') { //计算小数部分
        s = s + (*str - '0') / d;
        d *= 10.0;
        str++;
    }
    s = s * (falg ? -1.0 : 1.0);
    return s;
}
//运算函数
static double __calculator_add(double A, double B)
{
    log_debug("%s A=%f b=%f res=%f\n", __func__, A, B, (A + B));
    return (A + B);
}
static double __calculator_sub(double A, double B)
{
    log_debug("%s A=%f b=%f res=%f\n", __func__, A, B, (A - B));
    return (A - B);
}
static double __calculator_mul(double A, double B)
{
    log_debug("%s A=%f b=%f res=%f\n", __func__, A, B, (A * B));
    return (A * B);
}
static double __calculator_div(double A, double B)
{
    if (fabs(B) < 1e-6) { //除0判断
        return 0;
    }
    log_debug("%s A=%f b=%f res=%f\n", __func__, A, B, (A / B));
    return (A / B);
}
//显示负号函数
static void negative_show()
{
    log_debug("%s %d", __func__, __this->negative);
    if (__this->negative == 1) {
        ui_pic_set_hide_by_id(CAL_NEG_PIC, 1);
    } else {
        ui_pic_set_hide_by_id(CAL_NEG_PIC, 0);
    }
    ui_redraw(CAL_NEG_PIC);
}
//显示函数（数字）
static void number_show_update(void)
{
    static u8 numberstr[SHOW_NUM_MAX] = {0};
    memset(numberstr, 0, SHOW_NUM_MAX);
    memcpy(numberstr, __this->str, strlen(__this->str));
#if 1
    for (int i = SHOW_NUM_MAX - 1; i > 0; i--) {
        if ((numberstr[i] != 0) && (numberstr[i] != '0')) {
            if (numberstr[i] == '.') {
                numberstr[i] = '\0';
            } else {
                numberstr[++i] = '\0';
            }
            break;
        }
    }

#else
    u8 *p = strchr(numberstr, '.');
    log_debug("%s %s %d", numberstr, __this->str, __this->point_1 & 0x3f);
    if (p != NULL) {
        if (__this->point_2 & BIT(7)) {
        } else {
            if (__this->point_1 & BIT(7)) {
                p += (__this->point_1 & 0x3f);
                p++;
                *p = '\0';
            } else {
                for (int i = SHOW_NUM_MAX - 1; i > 0; i--) {
                    if ((numberstr[i] != 0) && (numberstr[i] != '0')) {
                        if (numberstr[i] == '.') {
                            numberstr[i] = '\0';
                        } else {
                            numberstr[++i] = '\0';
                        }
                        break;
                    }
                }
            }
        }
    }
#endif
    struct unumber unum;
    unum.type = TYPE_STRING;
    if (strlen(__this->str) == 0) {
        unum.num_str = "0";
    } else {
        unum.num_str = numberstr;
    }
    log_debug("%s %s %d", numberstr, __this->str, __this->point_1 & 0x3f);
    ui_number_update_by_id(NUM_RESULT, &unum);
    negative_show();
}
//输入函数
static int string_input(u8 num)
{
    if (strlen(__this->str) < IN_NUM_MAX - 1) { //留一个位给正负号
        sprintf(__this->str, "%s%d", __this->str, num);
    } else {
        printf("input str full");
    }
    if (__this->point_2 & BIT(7)) {
        __this->point_2++;
    }
    log_debug("%s\n", __this->str);
    number_show_update();
    return 0;
}
//清除字符
static void string_clear(void)
{
    memset(__this->str, 0, SHOW_NUM_MAX);
}
//显示函数（输出）point_2
static void string_out(void)
{
    if (fabs(__this->num_1) < 1e-6) { //除0判断
        log_debug("__this->num_1<1e-6");
        string_clear();
    } else {
        sprintf(__this->str, "%f", abs(__this->num_1));
        __this->negative = dir(__this->num_1);
    }
    number_show_update();
}

//功能函数
//负号
static void calculation_negative(void)
{
    log_debug("%s", __func__);
    __this->negative = -__this->negative;
    negative_show();
}
//小数点
static void calculation_point(void)
{
    if (__this->point_2 & BIT(7)) {
        return;
    }
    __this->point_2 |= BIT(7);
    log_debug("%s %d\n", __func__, __this->point_2);
    sprintf(__this->str, "%s.", __this->str);
    number_show_update();
}
//清除所有
static int calculation_del(void)
{
    calculator_reset();
    number_show_update();
    negative_show();
    return 0;
}
//回退单字符
static int calculation_back(void)
{
    if (strlen(__this->str) > 0) {
        __this->str[strlen(__this->str) - 1] = '\0';
        number_show_update();
    } else {
        return 0;
    }
    log_debug("%s %s\n", __func__, __this->str);
    return strlen(__this->str);
}
//等于
#define __POINT_MAX(A,B) ((A>B)?A:B)
#define POINT_MAX(A,B) ((__POINT_MAX(A,B)>6)?6:__POINT_MAX(A,B))

static double calculation_equal(u8 mode)
{
    double cal_res = 0.0;
    log_debug("%s %d\n", __func__, mode);
    switch (mode) {
    case ADD:
        cal_res = __calculator_add(__this->num_1, __this->num_2);
        break;
    case SUB:
        cal_res = __calculator_sub(__this->num_1, __this->num_2);
        break;
    case MUL:
        cal_res = __calculator_mul(__this->num_1, __this->num_2);
        break;
    case DIV:
        cal_res = __calculator_div(__this->num_1, __this->num_2);
        break;
    case NONE:
        cal_res = __this->num_2;
        break;
    default:
        break;
    }
    __this->cal_mode = NONE;
    __this->point_1 = POINT_MAX((__this->point_1 & 0X3F), (__this->point_2 & 0X3F));
    if (__this->point_1 != 0) {
        __this->point_1 |= BIT(7);
    }
    __this->point_2 = 0;
    log_debug("%s %f %d %d", __func__, cal_res, __this->point_1, __this->point_2);
    return cal_res;
}

static u8 calculation_mode_set(u8 mode)
{
    log_debug("%s mode=%d", __func__, mode);
    __this->num_2 = __this->negative * __atof(__this->str);
    __this->num_1 = calculation_equal(__this->cal_mode);
    __this->cal_mode = mode;
    string_out();
    string_clear();
    __this->negative = 1;
    return __this->cal_mode;
}
//触摸按键响应
static int calculator_key(int key)
{
    switch (key) {
    //数字
    case CAL_NUM_0:
        log_debug("CAL_NUM_0");
        string_input(0);
        break;
    case CAL_NUM_1:
        log_debug("CAL_NUM_1");
        string_input(1);
        break;
    case CAL_NUM_2:
        log_debug("CAL_NUM_2");
        string_input(2);
        break;
    case CAL_NUM_3:
        log_debug("CAL_NUM_3");
        string_input(3);
        break;
    case CAL_NUM_4:
        log_debug("CAL_NUM_4");
        string_input(4);
        break;
    case CAL_NUM_5:
        log_debug("CAL_NUM_5");
        string_input(5);
        break;
    case CAL_NUM_6:
        log_debug("CAL_NUM_6");
        string_input(6);
        break;
    case CAL_NUM_7:
        log_debug("CAL_NUM_7");
        string_input(7);
        break;
    case CAL_NUM_8:
        log_debug("CAL_NUM_8");
        string_input(8);
        break;
    case CAL_NUM_9:
        log_debug("CAL_NUM_9");
        string_input(9);
        break;
    //四则运算
    case CAL_ADD:
        log_debug("CAL_ADD");
        calculation_mode_set(ADD);
        break;
    case CAL_SUB:
        log_debug("CAL_SUB");
        calculation_mode_set(SUB);
        break;
    case CAL_MUL:
        log_debug("CAL_MUL");
        calculation_mode_set(MUL);
        break;
    case CAL_DIV:
        log_debug("CAL_DIV");
        calculation_mode_set(DIV);
        break;
    case CAL_EQUAL:
        log_debug("CAL_EQUAL");
        if (__this->cal_mode != NONE) {
            calculation_mode_set(NONE);
        }
        break;
    //功能按键
    case CAL_POINT:
        log_debug("CAL_POINT");
        calculation_point();
        break;
    case CAL_NEG:
        log_debug("CAL_NEG");
        calculation_negative();
        break;
    case CAL_DEL:
        log_debug("CAL_DEL");
        calculation_del();
        break;
    case CAL_BACK:
        log_debug("CAL_BACK");
        calculation_back();
        break;
    }
    return 0;
}

/* static int calculation_show_onchange(void *ctr, enum element_change_event e, void *arg) */
/* { */
/* log_debug("%s %d\n",__func__,e); */
/* struct ui_text *text = (struct ui_text *)ctr; */
/* switch (e) { */
/* case ON_CHANGE_INIT: */
/* memset(__this->str,0,SHOW_NUM_MAX); */

/* extern void test(void); */
/* test(); */
/* break; */
/* case ON_CHANGE_SHOW: */
/* break; */
/* case ON_CHANGE_FIRST_SHOW: */
/* break; */
/* case ON_CHANGE_RELEASE: */
/* break; */
/* default: */
/* return false; */
/* } */
/* return false; */
/* } */
static int calculator_button_ontouch(void *ctr, struct element_touch_event *e)
{
    struct element *elm = (struct element *)ctr;
    switch (e->event) {
    case ELM_EVENT_TOUCH_DOWN:
        return true;
    case ELM_EVENT_TOUCH_HOLD:
        break;
    case ELM_EVENT_TOUCH_MOVE:
        return true;
        break;
    case ELM_EVENT_TOUCH_UP:
        calculator_key(elm->id);
        break;
    }
    return false;
}


REGISTER_UI_EVENT_HANDLER(CAL_NUM_0)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = calculator_button_ontouch,
};
REGISTER_UI_EVENT_HANDLER(CAL_NUM_1)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = calculator_button_ontouch,
};
REGISTER_UI_EVENT_HANDLER(CAL_NUM_2)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = calculator_button_ontouch,
};
REGISTER_UI_EVENT_HANDLER(CAL_NUM_3)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = calculator_button_ontouch,
};
REGISTER_UI_EVENT_HANDLER(CAL_NUM_4)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = calculator_button_ontouch,
};
REGISTER_UI_EVENT_HANDLER(CAL_NUM_5)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = calculator_button_ontouch,
};
REGISTER_UI_EVENT_HANDLER(CAL_NUM_6)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = calculator_button_ontouch,
};
REGISTER_UI_EVENT_HANDLER(CAL_NUM_7)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = calculator_button_ontouch,
};
REGISTER_UI_EVENT_HANDLER(CAL_NUM_8)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = calculator_button_ontouch,
};
REGISTER_UI_EVENT_HANDLER(CAL_NUM_9)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = calculator_button_ontouch,
};
REGISTER_UI_EVENT_HANDLER(CAL_POINT)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = calculator_button_ontouch,
};
REGISTER_UI_EVENT_HANDLER(CAL_ADD)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = calculator_button_ontouch,
};
REGISTER_UI_EVENT_HANDLER(CAL_SUB)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = calculator_button_ontouch,
};
REGISTER_UI_EVENT_HANDLER(CAL_MUL)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = calculator_button_ontouch,
};
REGISTER_UI_EVENT_HANDLER(CAL_DIV)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = calculator_button_ontouch,
};
REGISTER_UI_EVENT_HANDLER(CAL_EQUAL)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = calculator_button_ontouch,
};
REGISTER_UI_EVENT_HANDLER(CAL_DEL)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = calculator_button_ontouch,
};
REGISTER_UI_EVENT_HANDLER(CAL_NEG)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = calculator_button_ontouch,
};
REGISTER_UI_EVENT_HANDLER(CAL_BACK)
.onchange = NULL,
 .onkey = NULL,
  .ontouch = calculator_button_ontouch,
};
#endif
#endif
#endif


#if 0
static void test(void)
{
    printf("___________12+34_________________");
    calculator_key(CAL_NUM_1);
    calculator_key(CAL_NUM_2);
    string_out();
    delay_2ms(300);
    calculator_key(CAL_ADD);
    string_out();
    delay_2ms(300);
    calculator_key(CAL_NUM_3);
    calculator_key(CAL_NUM_4);
    string_out();
    delay_2ms(300);
    calculator_key(CAL_EQUAL);
    string_out();
    delay_2ms(300);
    printf("___________12-34_________________");
    calculator_key(CAL_NUM_1);
    calculator_key(CAL_NUM_2);
    string_out();
    delay_2ms(300);
    calculator_key(CAL_SUB);
    string_out();
    delay_2ms(300);
    calculator_key(CAL_NUM_3);
    calculator_key(CAL_NUM_4);
    string_out();
    delay_2ms(300);
    calculator_key(CAL_EQUAL);
    string_out();
    delay_2ms(300);
    printf("___________12*34_________________");
    calculator_key(CAL_NUM_1);
    calculator_key(CAL_NUM_2);
    calculator_key(CAL_MUL);
    calculator_key(CAL_NUM_3);
    calculator_key(CAL_NUM_4);
    calculator_key(CAL_EQUAL);
    printf("___________12/34_________________");
    calculator_key(CAL_NUM_1);
    calculator_key(CAL_NUM_2);
    calculator_key(CAL_DIV);
    calculator_key(CAL_NUM_3);
    calculator_key(CAL_NUM_4);
    calculator_key(CAL_EQUAL);
    printf("___________-12+34_________________");
    calculator_key(CAL_NEG);
    calculator_key(CAL_NUM_1);
    calculator_key(CAL_NUM_2);
    calculator_key(CAL_ADD);
    calculator_key(CAL_NUM_3);
    calculator_key(CAL_NUM_4);
    calculator_key(CAL_EQUAL);
    printf("___________1.2+3.4_________________");
    calculator_key(CAL_NUM_1);
    calculator_key(CAL_POINT);
    calculator_key(CAL_NUM_2);
    calculator_key(CAL_ADD);
    calculator_key(CAL_NUM_3);
    calculator_key(CAL_POINT);
    calculator_key(CAL_NUM_4);
    calculator_key(CAL_EQUAL);
    printf("___________12+34-56x78/9_________________");
    calculator_key(CAL_NUM_1);
    calculator_key(CAL_NUM_2);
    calculator_key(CAL_ADD);
    calculator_key(CAL_NUM_3);
    calculator_key(CAL_NUM_4);
    calculator_key(CAL_SUB);
    calculator_key(CAL_NUM_5);
    calculator_key(CAL_NUM_6);
    calculator_key(CAL_MUL);
    calculator_key(CAL_NUM_7);
    calculator_key(CAL_NUM_8);
    calculator_key(CAL_DIV);
    calculator_key(CAL_NUM_0);
    calculator_key(CAL_NUM_9);
    calculator_key(CAL_EQUAL);
    printf("___________end_________________");

}
#endif

