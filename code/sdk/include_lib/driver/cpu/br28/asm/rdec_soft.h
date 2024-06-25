#ifndef __RDEC_SOFT_H__
#define __RDEC_SOFT_H__

#include "typedef.h"
#include "rdec.h"

#define TIMER_MAX_NUM       6
#define TIMER_BASE_ADDR     (JL_TIMER0)
#define TIMER_OFFSET        (JL_TIMER1 - JL_TIMER0)
#define tid2timerx(tid)     (TIMER_BASE_ADDR + tid * TIMER_OFFSET)
#define IRQ_TIME0_IDX_BASE  (IRQ_TIME0_IDX)

#define RDEC_ADC_DEVIATION_BASE     30 //电压值小于 (vddio * 30%) 时,认为是低电平
#define RDEC_ADC_DEVIATION          5 //允许偏差 ±5%
#define RDEC_EXT_R1                 10000 //编码器外挂电阻阻值
#define RDEC_EXT_R2                 4700 //编码器外挂电阻阻值
#define RDEC_ADC_MAX_VALUE         ((1023 * (RDEC_EXT_R2) * (100+RDEC_ADC_DEVIATION)) / ((RDEC_EXT_R1+RDEC_EXT_R2) * 100 ))
#define RDEC_ADC_MIN_VALUE         ((1023 * (RDEC_EXT_R2) * (100-RDEC_ADC_DEVIATION)) / ((RDEC_EXT_R1+RDEC_EXT_R2) * 100 ))

//工作模式选择
#define TIMER_MODE_DISABLE      (0b11 << 0) //关闭TIMER功能
#define TIMER_MODE_TIME         (0b01 << 0) //定时、计数模式
#define TIMER_MODE_RISE         (0b10 << 0) //上升沿捕获
#define TIMER_MODE_FALL         (0b11 << 0) //下降沿捕获

#define TIMER_PCLR          (0b1 << 14) //清PND
//时钟源和分频系数偏移
#define TIMER_PSET          4 //分频系数
#define TIMER_SSEL          10 //时钟源

enum timer_work_mode {
    TIMER_CLOSE, //timer关闭
    TIMER_TIMEOUT, //定时模式
    CAPTURE_EDGE_RISE, //上升沿
    CAPTURE_EDGE_FALL, //下降沿
};
typedef enum {
    // TIMER0 = 0, //sdk被占用
    // TIMER1, //sdk被占用
    TIMER2 = 2,
    TIMER3,
    TIMER4,
    TIMER5,
} rdec_timer;

typedef enum {
    CLK_SRC_12M = 5,
    // CLK_SRC_24M,
    // CLK_SRC_48M,
} rdec_timer_clk_src;

typedef enum {
    CLK_DIV_1 = 0,
    CLK_DIV_4,
    CLK_DIV_16,
    CLK_DIV_64,
    CLK_DIV_2,
    CLK_DIV_8,
    CLK_DIV_32,
    CLK_DIV_128,
    CLK_DIV_256,
    CLK_DIV_1024,
    CLK_DIV_4096,
    CLK_DIV_16384,
    CLK_DIV_512,
    CLK_DIV_2048,
    CLK_DIV_8192,
    CLK_DIV_32768,
} rdec_timer_clk_div;

typedef enum {
    RDEC_PHASE_1, //半码
    RDEC_PHASE_2, //全码
    RDEC_PHASE_1_ADC, //半码 adc单线采集
    RDEC_PHASE_2_ADC, //全码 adc单线采集
} rdec_mode;

struct rdec_soft_timer_config {
    rdec_timer timerx; //需要使用的timerx, 0 表示timer0, 1 表示timer1 以此类推
    rdec_timer_clk_src clk_src; //timerx的时钟源
    rdec_timer_clk_div clk_div; //timerx的时钟分频系数
    u32 filter; //滤波参数, 单位: us
    u32 priority; //timerx捕获功能的优先级
};
struct rdec_soft_rdec_config {
    u32 rdec_a; //编码器A相
    u32 rdec_b; //编码器B相
    int cnt; //计数值
    int dir; //方向
    rdec_mode mode; //编码器模式
};

struct rdec_soft_config {
    struct rdec_soft_timer_config timer;
    struct rdec_soft_rdec_config rdec;
};

int rdec_soft_init(struct rdec_soft_config *cfg);
void rdec_soft_deinit(int id);
void rdec_soft_start(int id);
void rdec_soft_pause(int id);
void rdec_soft_rsume(int id);
int rdec_soft_get_value(int id);
int rdec_init(const struct rdec_platform_data *user_data);
#endif

