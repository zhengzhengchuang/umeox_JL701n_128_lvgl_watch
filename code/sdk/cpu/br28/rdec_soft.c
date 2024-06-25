#include "asm/rdec_soft.h"
#include "spinlock.h"
#include "gpio.h"
#include "asm/adc_api.h"
#include "app_config.h"


#if 1

#define RDEC_SOFT_TEST_ENABLE 0 //测试使能
#define RDEC_DEBUG_ENABLE  0 //打印使能

#if RDEC_DEBUG_ENABLE
#define rdec_soft_debug(fmt, ...) 	printf("[RDEC] "fmt, ##__VA_ARGS__)
#else
#define rdec_soft_debug(...)
#endif

static struct rdec_soft_config *rdec_cfg_info[TIMER_MAX_NUM] = {NULL};
DEFINE_SPINLOCK(gptimer_lock);
const static u32 timer_src_table[16] = { //timer 时钟源选择
    0,
    0, //lsb_clk
    0, //rtc_osc
    0, //rc_16M
    0, //lrc_clk
    12, //std_12M
    0, //std_24M
    0, //std_48M
    0, //pat_clk
    0,
    0,
    0,
    0,
    0,
    0,
    0, //tmrx_cin(crossbar)
};
const static u32 timer_div_table[16] = { //tiemr 分频系数选择
    1,
    4,
    16,
    64,
    2,
    8,
    32,
    128,
    256,
    1024,
    4096,
    16384,
    512,
    2048,
    8192,
    32768,
};
const static u8 adc_ch_io_table[16] = {  //gpio->adc_ch 表
    IO_PORTA_00,
    IO_PORTA_05,
    IO_PORTA_06,
    IO_PORTA_08,
    IO_PORTC_04,
    IO_PORTC_05,
    IO_PORTB_01,
    IO_PORTB_02,
    IO_PORTB_05,
    IO_PORTB_09,
    IO_PORT_DP,
    IO_PORT_DM,
    IO_PORTG_00,
    IO_PORTG_01,
    IO_PORTG_05,
    IO_PORTG_07,
};
static u32 adc_io2ch(u32 gpio)   //根据传入的GPIO，返回对应的ADC_CH
{
    for (u8 i = 0; i < ARRAY_SIZE(adc_ch_io_table); i++) {
        if (adc_ch_io_table[i] == gpio) {
            return i;
        }
    }
    rdec_soft_debug("add_adc_ch io error!!! change other io_port!!!\n");
    return 0xffffffff; //未找到支持ADC的IO
}



static void rdec_soft_cfg_dump(struct rdec_soft_config *cfg)
{
    rdec_soft_debug("func:%s, line:%d\n", __func__, __LINE__);
    rdec_soft_debug("cfg->timer.timerx: TIMER%d\n", cfg->timer.timerx);
    rdec_soft_debug("cfg->timer.clk_src: clk_src_table[%d]\n", cfg->timer.clk_src);
    rdec_soft_debug("cfg->timer.clk_div: clk_div_table[%d]\n", cfg->timer.clk_div);
    rdec_soft_debug("cfg->timer.filter: %dus\n", cfg->timer.filter);
    rdec_soft_debug("cfg->timer.priority: %d\n", cfg->timer.priority);
    rdec_soft_debug("cfg->rdec.rdec_a: 0x%x\n", cfg->rdec.rdec_a);
    rdec_soft_debug("cfg->rdec.rdec_b: 0x%x\n", cfg->rdec.rdec_b);
    rdec_soft_debug("cfg->rdec.mode: %d\n", cfg->rdec.mode);
}

static void timer_set_work_mode(int tid, u8 work_mode)
{
    if (rdec_cfg_info[tid] == NULL) {
        rdec_soft_debug("func:%s, line:%d\n", __func__, __LINE__);
        return;
    }
    JL_TIMER_TypeDef *JL_TIMERx = tid2timerx(tid);
    spin_lock(&gptimer_lock);
    JL_TIMERx->CON &= ~TIMER_MODE_DISABLE;
    JL_TIMERx->CON |= TIMER_PCLR;
    JL_TIMERx->CNT = 0;
    JL_TIMERx->PRD = 0;
    if (work_mode == CAPTURE_EDGE_RISE) {
        JL_TIMERx->CON |= TIMER_MODE_RISE;
    } else if (work_mode == CAPTURE_EDGE_FALL) {
        JL_TIMERx->CON |= TIMER_MODE_FALL;
    } else if (work_mode == TIMER_TIMEOUT) {
        u8 clk_src = rdec_cfg_info[tid]->timer.clk_src & 0xf;
        u8 clk_div = rdec_cfg_info[tid]->timer.clk_div & 0xf;
        u32 us = rdec_cfg_info[tid]->timer.filter;
        /* rdec_soft_debug("us:%d, src:%d, div:%d\n", us, timer_src_table[clk_src], timer_div_table[clk_div]); */
        JL_TIMERx->PRD = us * timer_src_table[clk_src] / timer_div_table[clk_div];
        JL_TIMERx->CON |= TIMER_MODE_TIME;
    }
    spin_unlock(&gptimer_lock);

}
static u8 timer_get_work_mode(int tid)
{
    if (rdec_cfg_info[tid] == NULL) {
        rdec_soft_debug("func:%s, line:%d\n", __func__, __LINE__);
        return -1;
    }
    JL_TIMER_TypeDef *JL_TIMERx = tid2timerx(tid);
    u8 mode = JL_TIMERx->CON & 0b11;
    return mode;
}
static u8 last_edge;
static void timer_hw_cb(int tid)
{
    u32 rdec_a = rdec_cfg_info[tid]->rdec.rdec_a;
    u32 rdec_b = rdec_cfg_info[tid]->rdec.rdec_b;
    int rdec_cnt = rdec_cfg_info[tid]->rdec.cnt;
    int rdec_dir = rdec_cfg_info[tid]->rdec.dir;
    u8 timer_mode = timer_get_work_mode(tid);
    u8 rdec_mode = rdec_cfg_info[tid]->rdec.mode;

    switch (rdec_mode) {
    case RDEC_PHASE_1:
        if (timer_mode == TIMER_TIMEOUT) {
            if (gpio_read(rdec_a) == last_edge) { //无效
                if (last_edge) {
                    timer_set_work_mode(tid, CAPTURE_EDGE_FALL);
                } else {
                    timer_set_work_mode(tid, CAPTURE_EDGE_RISE);
                }
            } else {
                if (gpio_read(rdec_b) ^ last_edge) {
                    rdec_dir = -1;
                } else {
                    rdec_dir = 1;
                }
                if (last_edge) {
                    while (gpio_read(rdec_b));
                    timer_set_work_mode(tid, CAPTURE_EDGE_RISE);
                } else {
                    while (!gpio_read(rdec_b));
                    timer_set_work_mode(tid, CAPTURE_EDGE_FALL);
                }
                rdec_cnt += rdec_dir;
                rdec_cfg_info[tid]->rdec.dir = rdec_dir;
                rdec_cfg_info[tid]->rdec.cnt = rdec_cnt;
            }
        } else if (timer_mode == CAPTURE_EDGE_RISE) {
            last_edge = 0;
            timer_set_work_mode(tid, TIMER_TIMEOUT);
        } else if (timer_mode == CAPTURE_EDGE_FALL) {
            last_edge = 1;
            timer_set_work_mode(tid, TIMER_TIMEOUT);
        }
        break;
    case RDEC_PHASE_2:
        if (timer_mode == TIMER_TIMEOUT) {
            if (gpio_read(rdec_a)) { //无效
                timer_set_work_mode(tid, CAPTURE_EDGE_FALL);
            } else {
                if (gpio_read(rdec_b)) {
                    rdec_dir = -1;
                } else {
                    rdec_dir = 1;
                }
                rdec_cfg_info[tid]->rdec.dir = rdec_dir;
                timer_set_work_mode(tid, CAPTURE_EDGE_RISE);
            }
        } else if (timer_mode == CAPTURE_EDGE_RISE) {
            rdec_cnt += rdec_dir;
            rdec_cfg_info[tid]->rdec.cnt = rdec_cnt;
            timer_set_work_mode(tid, CAPTURE_EDGE_FALL);
        } else if (timer_mode == CAPTURE_EDGE_FALL) {
            timer_set_work_mode(tid, TIMER_TIMEOUT);
        }
        break;
    //RDEC_ADC方式暂不支持 PHASE_1
    /* case RDEC_PHASE_1_ADC: */
    /*     if (timer_mode == TIMER_TIMEOUT) { */
    /*         if (gpio_read(rdec_a) == last_edge) { //无效 */
    /*             if (last_edge) { */
    /*                 timer_set_work_mode(tid, CAPTURE_EDGE_FALL); */
    /*             } else { */
    /*                 timer_set_work_mode(tid, CAPTURE_EDGE_RISE); */
    /*             } */
    /*         } else { */
    /*             u32 ad_value = adc_get_value_preemption(AD_CH_PA5); */
    /*             printf("ad_value = %d\n", ad_value);  */
    /*             if (last_edge) { */
    /*                 if ((ad_value > RDEC_ADC_MIN_VALUE) && (ad_value < RDEC_ADC_MAX_VALUE)) { */
    /*                     rdec_dir = -1; */
    /*                 } else { */
    /*                     rdec_dir = 1; */
    /*                 } */
    /*                 while (gpio_read(rdec_a)); */
    /*                 timer_set_work_mode(tid, CAPTURE_EDGE_RISE); */
    /*             } else { */
    /*                 if ((ad_value > RDEC_ADC_MIN_VALUE) && (ad_value < RDEC_ADC_MAX_VALUE)) { */
    /*                     rdec_dir = 1; */
    /*                 } else { */
    /*                     rdec_dir = -1; */
    /*                 } */
    /*                 while (!gpio_read(rdec_a)); */
    /*                 timer_set_work_mode(tid, CAPTURE_EDGE_FALL); */
    /*             } */
    /*             rdec_cnt += rdec_dir; */
    /*             rdec_cfg_info[tid]->rdec.dir = rdec_dir; */
    /*             rdec_cfg_info[tid]->rdec.cnt = rdec_cnt; */
    /*         } */
    /*     } else if (timer_mode == CAPTURE_EDGE_RISE) { */
    /*         last_edge = 0; */
    /*         timer_set_work_mode(tid, TIMER_TIMEOUT); */
    /*     } else if (timer_mode == CAPTURE_EDGE_FALL) { */
    /*         last_edge = 1; */
    /*         timer_set_work_mode(tid, TIMER_TIMEOUT); */
    /*     } */
    /*     break; */
    case RDEC_PHASE_2_ADC:
        if (timer_mode == TIMER_TIMEOUT) {
            if (gpio_read(rdec_a)) { //无效
                timer_set_work_mode(tid, CAPTURE_EDGE_FALL);
            } else {
                u32 ad_value = adc_get_value_preemption(adc_io2ch(rdec_a));
                /* printf("ad_value = %d, MIN = %d, MAX = %d\n",  */
                /* ad_value, RDEC_ADC_MIN_VALUE, RDEC_ADC_MAX_VALUE); */
                if ((ad_value > RDEC_ADC_MIN_VALUE) && (ad_value < RDEC_ADC_MAX_VALUE)) {
                    rdec_dir = -1;
                } else if (ad_value < 10) {
                    rdec_dir = 1;
                } else {
                    rdec_dir = 0;
                }
                rdec_cfg_info[tid]->rdec.dir = rdec_dir;
                timer_set_work_mode(tid, CAPTURE_EDGE_RISE);
            }
        } else if (timer_mode == CAPTURE_EDGE_RISE) {
            rdec_cnt += rdec_dir;
            rdec_cfg_info[tid]->rdec.cnt = rdec_cnt;
            timer_set_work_mode(tid, CAPTURE_EDGE_FALL);
        } else if (timer_mode == CAPTURE_EDGE_FALL) {
            timer_set_work_mode(tid, TIMER_TIMEOUT);
        }
        break;
    default:
        break;
    }
}
___interrupt
static void timer_hw0_cb()
{
    spin_lock(&gptimer_lock);
    JL_TIMER0->CON |= TIMER_PCLR; //清 pnd
    spin_unlock(&gptimer_lock);
    timer_hw_cb(0);
}
___interrupt
static void timer_hw1_cb()
{
    spin_lock(&gptimer_lock);
    JL_TIMER1->CON |= TIMER_PCLR; //清 pnd
    spin_unlock(&gptimer_lock);
    timer_hw_cb(1);
}
___interrupt
static void timer_hw2_cb()
{
    spin_lock(&gptimer_lock);
    JL_TIMER2->CON |= TIMER_PCLR; //清 pnd
    spin_unlock(&gptimer_lock);
    timer_hw_cb(2);
}
___interrupt
static void timer_hw3_cb()
{
    spin_lock(&gptimer_lock);
    JL_TIMER3->CON |= TIMER_PCLR; //清 pnd
    spin_unlock(&gptimer_lock);
    timer_hw_cb(3);
}
___interrupt
static void timer_hw4_cb()
{
    spin_lock(&gptimer_lock);
    JL_TIMER4->CON |= TIMER_PCLR; //清 pnd
    spin_unlock(&gptimer_lock);
    timer_hw_cb(4);
}
___interrupt
static void timer_hw5_cb()
{
    spin_lock(&gptimer_lock);
    JL_TIMER5->CON |= TIMER_PCLR; //清 pnd
    spin_unlock(&gptimer_lock);
    timer_hw_cb(5);
}
static void (*timer_cb_table[])(void) = {
    timer_hw0_cb,
    timer_hw1_cb,
    timer_hw2_cb,
    timer_hw3_cb,
    timer_hw4_cb,
    timer_hw5_cb,
};

int rdec_soft_init(struct rdec_soft_config *cfg)
{
    rdec_soft_debug("func:%s, line:%d\n", __func__, __LINE__);

    int tid = cfg->timer.timerx;
    ASSERT(rdec_cfg_info[tid] == NULL, "%s()\n", __func__);
    rdec_cfg_info[tid] = (struct rdec_soft_config *)malloc(sizeof(struct rdec_soft_config));
    ASSERT(rdec_cfg_info[tid] != NULL, "%s()\n", __func__);
    memset(rdec_cfg_info[tid], 0, sizeof(struct rdec_soft_config));
    memcpy(rdec_cfg_info[tid], cfg, sizeof(struct rdec_soft_config));
    rdec_soft_cfg_dump(rdec_cfg_info[tid]);

    JL_TIMER_TypeDef *JL_TIMERx = tid2timerx(tid);//TIMER_BASE_ADDR + tid * TIMER_OFFSET;
    ASSERT(((JL_TIMERx->CON & 0b11) == 0), "timer%d busy!!!\n", tid);

    u32 rdec_a = rdec_cfg_info[tid]->rdec.rdec_a;
    if (rdec_a != NO_CONFIG_PORT) {
        gpio_set_direction(rdec_a, 1);
        gpio_set_die(rdec_a, 1);
        if (rdec_cfg_info[tid]->rdec.mode == RDEC_PHASE_2_ADC) {
            gpio_set_pull_up(rdec_a, 0);
        } else {
            gpio_set_pull_up(rdec_a, 1);
        }
        gpio_set_pull_down(rdec_a, 0);
        gpio_ich_sel_iutput_signal(rdec_a, INPUT_CH_SIGNAL_TIMER0_CAPTURE + 2 * tid, INPUT_CH_TYPE_GP_ICH);
    }
    u32 rdec_b = rdec_cfg_info[tid]->rdec.rdec_b;
    if (rdec_b != NO_CONFIG_PORT) {
        gpio_set_direction(rdec_b, 1);
        gpio_set_die(rdec_b, 1);
        gpio_set_pull_up(rdec_b, 1);
        gpio_set_pull_down(rdec_b, 0);
    }

    spin_lock(&gptimer_lock);
    JL_TIMERx->CON = TIMER_PCLR; //清con0寄存器
    JL_TIMERx->CNT = 0; //清计数器
    JL_TIMERx->PRD = 0; //清PRD
    u8 clk_src = rdec_cfg_info[tid]->timer.clk_src & 0xf;
    u8 clk_div = rdec_cfg_info[tid]->timer.clk_div & 0xf;
    JL_TIMERx->CON |= (clk_src << TIMER_SSEL); //设置时钟源
    JL_TIMERx->CON |= (clk_div << TIMER_PSET); //设置分频系数
    spin_unlock(&gptimer_lock);
    request_irq(IRQ_TIME0_IDX_BASE + tid, rdec_cfg_info[tid]->timer.priority, timer_cb_table[tid], 0);

    return (int)tid;
}

void rdec_soft_deinit(int id)
{
    rdec_soft_debug("func:%s, line:%d\n", __func__, __LINE__);
    int tid = id;
    if (rdec_cfg_info[tid] == NULL) {
        rdec_soft_debug("func:%s, line:%d\n", __func__, __LINE__);
        return;
    }
    JL_TIMER_TypeDef *JL_TIMERx = tid2timerx(tid);
    spin_lock(&gptimer_lock);
    JL_TIMERx->CON = TIMER_PCLR; //清con0寄存器
    JL_TIMERx->CNT = 0; //清计数器
    JL_TIMERx->PRD = 0; //清PRD
    spin_unlock(&gptimer_lock);

    unrequest_irq(IRQ_TIME0_IDX_BASE + tid);
    gpio_ich_disable_iutput_signal(rdec_cfg_info[tid]->rdec.rdec_a,
                                   INPUT_CH_SIGNAL_TIMER0_CAPTURE + 2 * tid,
                                   INPUT_CH_TYPE_GP_ICH);
    memset(rdec_cfg_info[tid], 0, sizeof(struct rdec_soft_config));
    free(rdec_cfg_info[tid]);
    rdec_cfg_info[tid] = NULL;
}

void rdec_soft_start(int id)
{
    rdec_soft_debug("func:%s, line:%d\n", __func__, __LINE__);
    int tid = id;
    if (rdec_cfg_info[tid] == NULL) {
        rdec_soft_debug("func:%s, line:%d\n", __func__, __LINE__);
        return;
    }
    u8 mode = rdec_cfg_info[tid]->rdec.mode;
    u32 rdec_a = rdec_cfg_info[tid]->rdec.rdec_a;
    u32 rdec_b = rdec_cfg_info[tid]->rdec.rdec_b;
    if ((mode == RDEC_PHASE_2) || (mode == RDEC_PHASE_2_ADC)) {
        timer_set_work_mode(tid, TIMER_MODE_FALL);
    } else if ((mode == RDEC_PHASE_1) || (mode == RDEC_PHASE_1_ADC)) {
        if ((gpio_read(rdec_a) == 1) && (gpio_read(rdec_a) == 1)) {
            timer_set_work_mode(tid, TIMER_MODE_FALL);
        } else {
            timer_set_work_mode(tid, TIMER_MODE_RISE);
        }
    } else {
        rdec_soft_debug("rdec_cfg_mode error!\n");
    }
}
void rdec_soft_pause(int id)
{
    rdec_soft_debug("func:%s, line:%d\n", __func__, __LINE__);
    int tid = id;
    if (rdec_cfg_info[tid] == NULL) {
        rdec_soft_debug("func:%s, line:%d\n", __func__, __LINE__);
        return;
    }
    JL_TIMER_TypeDef *JL_TIMERx = tid2timerx(tid);
    spin_lock(&gptimer_lock);
    JL_TIMERx->CON = TIMER_PCLR; //清con0寄存器
    JL_TIMERx->CNT = 0; //清计数器
    JL_TIMERx->PRD = 0; //清PRD
    spin_unlock(&gptimer_lock);
}

void rdec_soft_rsume(int id)
{
    rdec_soft_start(id);
}

int rdec_soft_get_value(int id)
{
    int tid = id;
    if (rdec_cfg_info[tid] == NULL) {
        rdec_soft_debug("func:%s, line:%d\n", __func__, __LINE__);
        return 0;
    }
    int ret = rdec_cfg_info[tid]->rdec.cnt;
    rdec_cfg_info[tid]->rdec.cnt = 0;
    return ret;
}

#if 1
#include "rdec_key.h"
static int user_id;
static struct rdec_soft_config user_cfg;
int rdec_init(const struct rdec_platform_data *user_data)
{
    ASSERT(user_data->num <= 1, "only use one rdec_soft");
    user_cfg.timer.timerx  = TIMER4; //使用timer4
    user_cfg.timer.clk_src  = CLK_SRC_12M; //时钟源选择12M
    user_cfg.timer.clk_div  = CLK_DIV_16; //时钟分频系数16
    user_cfg.timer.filter  = 1000; //滤波参数设置 1000us
    user_cfg.timer.priority  = 1; //优先级默认给1
    user_cfg.rdec.rdec_a  = user_data->rdec[0].sin_port0; //编码器A相
    user_cfg.rdec.rdec_b  = user_data->rdec[0].sin_port1; //编码器B相
    user_cfg.rdec.cnt  = 0; //初始化清0
    if (user_cfg.rdec.rdec_b == NO_CONFIG_PORT) {
        user_cfg.rdec.mode = RDEC_PHASE_2_ADC; //编码器模式
    } else {
        user_cfg.rdec.mode = RDEC_PHASE_2; //编码器模式
    }
    user_id = rdec_soft_init(&user_cfg);
    rdec_soft_start(user_id);
    return 0;
}
s8 get_rdec_rdat(int i)
{
    s8 ret;
    if (i == 0) {
        ret = (s8)rdec_soft_get_value(user_id);
        // if(ret != 0)
        //     printf("%s:ret = %d\n", __func__, ret);
    }
    return ret;
}
#endif

#include "asm/power_interface.h"
static struct timer_reg {
    u32 con;
    u32 cnt;
    u32 prd;
};
static struct timer_reg timer_reg[TIMER_MAX_NUM];

AT_VOLATILE_RAM_CODE_POWER
static void rdec_soft_enter_deepsleep(void)
{
    for (u8 i = 0; i < TIMER_MAX_NUM; i++) {
        if (rdec_cfg_info[i] != NULL) {
            JL_TIMER_TypeDef *JL_TIMERx = tid2timerx(i);
            timer_reg[i].con = JL_TIMERx->CON;
            timer_reg[i].cnt = JL_TIMERx->CNT;
            timer_reg[i].prd = JL_TIMERx->PRD;
        }
    }
}

AT_VOLATILE_RAM_CODE_POWER
static void rdec_soft_exit_deepsleep(void)
{
    for (u8 i = 0; i < TIMER_MAX_NUM; i++) {
        if (rdec_cfg_info[i] != NULL) {
            JL_TIMER_TypeDef *JL_TIMERx = tid2timerx(i);
            JL_TIMERx->PRD = timer_reg[i].prd;
            JL_TIMERx->CNT = timer_reg[i].cnt;
            JL_TIMERx->CON = timer_reg[i].con;
        }
    }
}
DEEPSLEEP_TARGET_REGISTER(rdec_soft) = {
    .name   = "rdec_soft",
    .enter  = rdec_soft_enter_deepsleep,
    .exit   = rdec_soft_exit_deepsleep,
};



#if RDEC_SOFT_TEST_ENABLE
#include "timer.h"
#include "generic/gpio.h"
static int rdec_soft_id;
static struct rdec_soft_config rdec_soft_cfg;
static void timer_test_func()
{
    putchar('t');
    int value = rdec_soft_get_value(rdec_soft_id);
    if (value != 0) {
        rdec_soft_debug("( %d )", value);
    }
}
/* static void adc_test_func() */
/* { */
/*     u32 value = adc_get_value_preemption(adc_io2ch(IO_PORTA_05)); */
/*     if ((value > 188) && (value < 207)) { */
/*         #<{(| printf("KEY_1 -%d-\n", value); |)}># */
/*     } else if ((value > 88) && (value < 97)) { */
/*         #<{(| printf("KEY_2 -%d-\n", value); |)}># */
/*     } else { */
/*         #<{(| putchar('*'); |)}># */
/*     } */
/* } */
void rdec_soft_test_func()
{
    rdec_soft_debug("func:%s, line:%d\n", __func__, __LINE__);

    rdec_soft_cfg.timer.timerx  = TIMER4; //使用timer4
    rdec_soft_cfg.timer.clk_src  = CLK_SRC_12M; //时钟源选择12M
    rdec_soft_cfg.timer.clk_div  = CLK_DIV_16; //时钟分频系数16
    rdec_soft_cfg.timer.filter  = 1000; //滤波参数设置 1000us
    rdec_soft_cfg.timer.priority  = 1; //优先级默认给1
    rdec_soft_cfg.rdec.rdec_a  = IO_PORTA_12; //编码器A相
    rdec_soft_cfg.rdec.rdec_b  = IO_PORTA_13; //编码器B相
    rdec_soft_cfg.rdec.cnt  = 0; //初始化清0
    rdec_soft_cfg.rdec.mode = RDEC_PHASE_2; //编码器模式

    rdec_soft_id =  rdec_soft_init(&rdec_soft_cfg);
    /* rdec_soft_deinit(rdec_soft_id); */
    rdec_soft_start(rdec_soft_id);

    /* sys_timer_add(NULL, timer_test_func, 100); //测试用，定时读取rdec值 */
    usr_timer_add(NULL, timer_test_func, 100, 1); //测试用，定时读取rdec值
    JL_PORTA->DIR &= ~BIT(15);
    JL_PORTA->OUT &= ~BIT(15);

    /* usr_timer_add(NULL, adc_test_func, 2, 1); */
}
#endif
#endif // end ofTCFG_SOFT_RDEC_CAPTURE_ENABLE