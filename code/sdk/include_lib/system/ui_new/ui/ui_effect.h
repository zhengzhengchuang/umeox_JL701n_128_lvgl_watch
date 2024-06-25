#ifndef __UI_EFFECT_H__
#define __UI_EFFECT_H__


#include "generic/typedef.h"


//注意!!!当有两个页面的时候，也就是滑屏的时候
//1.一个页面需要做特效，另外一个不需要特效，那么
//只需要配置做特效的那个页面的(effect_mode + copy)
//2.两个页面都需要做特效，那么两个页面配置的
//copy方式必须一致，但effect_mode可以不同
//3.如果两个页面都不需要做特效，effect_mode = 0,
//那么会默认以EFFECT_COPY_SPLIC方式拼接成一张大图




#define EFFECT_COPY_SIGLE			2//单独存放页面,滑屏的时候有两个页面单独存放
#define EFFECT_COPY_SPLIC			1//拼接成一个页面,不滑屏的时候只有一个页面

//effect_mode:高8位是effect_mode，低8位是进行特效前需要
//将合成的页面拼接成一张大图还是单独存放
//新加的特效宏要按顺序加1 !!!
#define EFFECT_MODE_NONE		(u16)(0)
#define EFFECT_MODE_SCA			(u16)((1 << 8) | EFFECT_COPY_SIGLE)//屏幕的宽和高都小于或者等于240时才可使用
#define EFFECT_MODE_USER0		(u16)((2 << 8) | EFFECT_COPY_SPLIC)
#define EFFECT_MODE_USER1		(u16)((3 << 8) | EFFECT_COPY_SIGLE)
#define EFFECT_MODE_FLIP_SCA_ALPHA			(u16)((4 << 8) | EFFECT_COPY_SIGLE)
#define EFFECT_MODE_3D			(u16)((5 << 8) | EFFECT_COPY_SIGLE)
#define EFFECT_MODE_SCA_ALPHA			(u16)((6 << 8) | EFFECT_COPY_SIGLE)
#define EFFECT_MODE_REDRAW_PART			(u16)((7 << 8) | EFFECT_COPY_SIGLE)
#define EFFECT_MODE_AUTO_SCA_WITH_BG	(u16)((8 << 8) | EFFECT_COPY_SIGLE)//页面间子菜单缩放切换
#define EFFECT_MODE_GET_FRAME			(u16)((9 << 8) | EFFECT_COPY_SIGLE)
#define EFFECT_MODE_AUTO_SCA_WITH_FRAME	(u16)((10 << 8) | EFFECT_COPY_SIGLE)//同一个页面的布局层子菜单缩放切换
#define EFFECT_MODE_AUTO_PASS			(u16)((11 << 8) | EFFECT_COPY_SIGLE)//子菜单滑动过渡切换效果
#define EFFECT_MODE_DRAWER			(u16)((12 << 8) | EFFECT_COPY_SIGLE)//抽屉效果
#define EFFECT_MODE_AUTO_BIG_SCA			(u16)((13 << 8) | EFFECT_COPY_SIGLE)//页面间缩放从2.0到1.0特效



#define EFFECT_MODE_MAX				(u16)13

//注意！！！这里高8要随着最大值加1而动态改变
#define EFFECT_MODE_LIMIT		(u16)(((EFFECT_MODE_MAX + 1) << 8) | EFFECT_COPY_SPLIC)



struct effect_ctrl {
    // u8 copy_to_psram;//EFFECT_COPY_SIGLE / EFFECT_COPY_SPLIC
    u8 *splicing_buf; //拼接buf
    u8 *psbuf[2]; //单独buf

    u8 *sca_obuf; //硬件缩放的输出buf,内部缩放特效使用
    u8 *cache_buf[2]; //内部特效使用

    int not_refresh;//特效处理完之后，是否推屏，置一表示不推屏

    //由特效处理函数赋值,有特效时必须在特效处理函数里面赋值
    //1.滑屏的时候，如果ps_obuf = splicing_buf,表示特效处理完
    //之后还是放到原来的空间(对于那种不需要随着滑动而改变的特
    //效，只做一次就可以不断来回滑动)，否则就是新的一块空间(该
    //空间只有一个页面大小)
    //2.不滑屏的时候，该值由特效本身决定
    u8 *ps_obuf;
    int mix;
};

struct effect_part {
    int group;
    u8 *bg_addr;
};

//用户自定义回调原型
//ctrl:特效控制结构体 详细看结构体说明
//cur_head:当前需要做特效的页面结构体(struct imb_task_head)
//other_head:另外一个页面结构体，另外页面不一定需要做特效
typedef void *(*effect_cb)(struct effect_ctrl *ctrl, void *cur_head,  void *other_head);


//EFFECT_MODE_SCA
//1.取值范围0.3~1
//2.可以不传该参数，内部使用1
//3.参数要静态或者全局变量
struct effect_ratio {
    float ratio_w;
    float ratio_h;
};


//EFFECT_MODE_SCA_ALPHA & EFFECT_MODE_FLIP_SCA_ALPHA
//1.当作为EFFECT_MODE_FLIP_SCA_ALPHA模式的参数时，表示起始
//的值 start_alpha,start_scale
//2.可以不传该参数，内部使用默认的值(sca = 0.7, alpha = 64)
//3.flip_all_en : 滑动时是否两页都做特效
//4.参数要静态或者全局变量
struct effect_sca_alpha {
    float sca;
    int alpha;
    int flip_all_en;
    int cnt; // >=2 <=5
    int wait_te;
};

//EFFECT_MODE_3D
//1.focal取值100~1000，值越小变化越明显
//2.可以不传该参数，内部使用默认的值(focal = 500)
//3.参数要静态或者全局变量
struct effect_3d {
    int focal;
};

//EFFECT_MODE_REDRAW_PART
//1.只合成第一部分的那一次存放的地址，内部使用
//2.表示第一部分是否只合成一次，置一就只合成一次，然后做第一部分的特效时使用合成的那次作为输入
//3.reflesh表示做完该特效后是否刷新显示
//4.part1_effect_mode:该特效第一部分的特效模式
//5.part1_effect_priv:该特效第一部分的特效参数
//6.part1:第一部分的分组,分组是ui_core_set_group()函数分的组,只针对全屏布局
//7.patr2:第二部分的分组,分组是ui_core_set_group()函数分的组,只针对全屏布局
//8.参数要静态或者全局变量
struct effect_redraw_part {
    u8 *part1_kick_buf;
    u8 part1_one_kick;
    u8 reflesh;

    u16 part1_effect_mode;
    void *part1_effect_priv;
    struct effect_part part1;
    struct effect_part part2;
};

//子菜单滑动过渡切换效果
struct effect_auto_pass {
    int direction;//0:左到右 1:右到左
    int pl;//0:页面间切换 1:布局间切换
};

//子菜单滑动过渡切换效果
//从2.0缩放到1.0，带alpha会慢很多
struct effect_auto_big_sca {
    int has_alpha;//是否带alpha
    int pl;//0:页面间切换 1:布局间切换
    int cnt;//0:使用默认，其他：cnt > 2,次数越多越慢
};


//抽屉效果参数
struct drawer_item {
    struct list_head entry;
    int index;
    int page_id;
    struct rect ccoor;
    u8 *org_img;
    float sca;
    void *sca_info;
};

struct drawer_ctl {
    int init;
    struct list_head head;
    struct drawer_item *first_ditem; //显示出来的item中的第一个
    struct drawer_item *cur_item;
    int status;

    u8 *buf[3];
    u8 buf_busy;
    u8 has_new_item;

    int max_cy_limit;
    int min_cy_limit;
    float ratio;
};







//页面设置特效的唯一入口,在页面的图层或者全局的布局初始化的时候(ON_CHANGE_INIT)可以调用,如果id为-1,则可以在滑屏之前调用:
//id:单个页面的id，如果为-1，则表示设置页面间滑动的特效
//effect_mode:某个页面需要的特效，宏定义在ui_effect.h里面
//user_effect:用户自定义特效处理的回调函数，可以为空，如果有值，库里面
//会优先使用这个函数去处理该页面的特效,一般配对EFFECT_MODE_USER 0~N 使用
//effect_priv:特效的私有值，可以为空，库里面会传递给特效处理函数
//或者自定义特效回调函数
int ui_window_effect(int id, u16 effect_mode, void *user_effect, void *effect_priv);


//做特效时，可以控制页面停止刷新
void ui_window_stop_redraw(int stop_redraw);


/**
 * @brief ui_page_set_auto_sw_effect 使能页面跳转使用特效，只有psram版本能使用
 *
 * @param en 1:使能子菜单页面自动跳转特效 0:不使能
 */
void ui_page_set_auto_sw_effect(int en);

/**
 * @brief ui_page_get_auto_sw_effect 获取页面跳转特效使能状态
 *
 * @return 页面跳转使能状态
 */
int ui_page_get_auto_sw_effect();

/**
 * @brief ui_page_auto_sw_effect 页面跳转使用特效
 * 注意：该函数只能在ui任务里面使用，因为里面调用了ui_core_redraw, ui_core_show等相关的ui立即刷新页面的函数
 *
 * @param curr_win:页面ID或者layout ID(必须是全屏的布局)，必须和next_win类型保持一致
 * @param next_win:页面ID或者layout ID(必须是全屏的布局)，必须和curr_win类型保持一致
 * @param effect_mode:页面跳转的特效模式
 * @param user_effect:用户自定义页面跳转的特效处理的回调函数，可以为空
 * @param effect_priv:特效的私有值，可以为空，传递给特效处理函数
 *
 * @return -1:页面切换失败 0:页面切换成功
 */
int ui_page_auto_sw_effect(int curr_win, int next_win, u16 effect_mode, void *user_effect, void *effect_priv);


#endif

