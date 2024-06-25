#include "includes.h"
#include "timer.h"
#include "gp_timer_api.h"
#include "lp_timer_api.h"
#include "app_config.h"
#include "usr_timer.h"

volatile u32 cur_time_tick = 0;
#if TCFG_VCHR02_DEV_ENABLE
volatile u32 vchr02_cur_time_tick = 0;//set 20k
#endif
/*  */
/* #pragma bss_seg(".timer_bss") */
/* #pragma const_seg(".timer_const") */
/* #pragma code_seg(".timer_code") */
/*  */

u32 get_cur_sys_tick(void)
{
    return cur_time_tick;
}


static void timer_isr_fun(void *priv)
{
    struct loop_detect_handler *detect;
    list_for_each_loop_detect(detect) {
        if ((cur_time_tick %  detect->time) == 0) {
            if (detect->fun) {
                detect->fun(detect->priv);
            }
        }
    }
    cur_time_tick += 40;
#if TCFG_VCHR02_DEV_ENABLE
    vchr02_cur_time_tick++;
#endif
}

void timer_tick_init()
{
#if 0
    lp_timer_init(P11_LPTMR3, 40 * 1000);
    /* gp_timer_init(P11_GPTMR1, 10950 * 40); //10ms */
    /* gp_timer_register_irq(P11_GPTMR1, timer0_isr_fun); */
    lp_timer_register_irq(P11_LPTMR3, timer_isr_fun);
#else

    usr_timer_add(NULL, timer_isr_fun, 40, 0);

#endif
}

//=========================== P11_LPTMRx->CON0 =============================//
#define USR_LP_TIMER_ENABLE(p, v) 						SFR(p->CON0, 0, 1, v)
#define USR_LP_TIMER_CONTINUE(p, v) 					SFR(p->CON0, 1, 1, v)
#define USR_LP_TIMER_P11_RESUME_WK_IE(p, v) 			SFR(p->CON0, 2, 1, v)
#define USR_LP_TIMER_P11_TIMEOUT_WK_IE(p, v) 			SFR(p->CON0, 3, 1, v)
#define USR_LP_TIMER_P11_RESUME_PENDING_CLR(p) 			(p->CON0 |= BIT(4))
#define USR_LP_TIMER_P11_RESUME_PENDING_IS(p) 			(p->CON0 & BIT(5))
#define USR_LP_TIMER_P11_TIMEOUT_PENDING_CLR(p) 		(p->CON0 |= BIT(6))
#define USR_LP_TIMER_P11_TIMEOUT_PENDING_IS(p) 			(p->CON0 & BIT(7))

//======USR_===================== P11_LPTMRx->CON1 =============================//
#define USR_LP_TIMER_SOFT_KICK_START_ENABLE(p, v) 		SFR(p->CON1, 0, 1, v)
#define USR_LP_TIMER_HW_KICK_START_ENABLE(p, v) 		SFR(p->CON1, 1, 1, v)
#define USR_LP_TIMER_MSYS_RESUME_WK_IE(p, v) 			SFR(p->CON1, 2, 1, v)
#define USR_LP_TIMER_MSYS_TIMEOUT_WK_IE(p, v) 			SFR(p->CON1, 3, 1, v)
#define USR_LP_TIMER_MSYS_RESUME_PENDING_CLR(p) 		(p->CON1 |= BIT(4))
#define USR_LP_TIMER_MSYS_RESUME_PENDING_IS(p) 			(p->CON1 & BIT(5))
#define USR_LP_TIMER_MSYS_TIMEOUT_PENDING_CLR(p) 		(p->CON1 |= BIT(6))
#define USR_LP_TIMER_MSYS_TIMEOUT_PENDING_IS(p) 		(p->CON1 & BIT(7))

//=========================== P11_LPTMRx->CON2 =============================//
enum USR_LP_TIMER_CLK_TABLE {
    USR_LP_TIMER_CLK_RC250K,
    USR_LP_TIMER_CLK_RC16M,
    USR_LP_TIMER_CLK_BT_OSC,
    USR_LP_TIMER_CLK_LRC_OSC,
    USR_LP_TIMER_CLK_RTC_OSC,
    USR_LP_TIMER_CLK_PAT_CLK,
};
#define USR_LP_TIMER_CLOCK_SEL(p, v) 					SFR(p->CON2, 0, 4, v)

enum USR_LP_TIMER_DIV_TABLE {
    USR_LP_TIMER_DIV_1,
    USR_LP_TIMER_DIV_4,
    USR_LP_TIMER_DIV_16,
    USR_LP_TIMER_DIV_64,
};
#define USR_LP_TIMER_CLOCK_DIV_SEL(p, v) 				SFR(p->CON2, 4, 4, v)
#define USR_LP_TIMER_KICK_START_RUN(p) 					(p->CON2 |= BIT(8))
#define USR_LP_TIMER_RUNNING_IS(p) 						(p->CON2 & BIT(9))


//=========================== P11_LPTMRx->PRD =============================//
#define USR_LP_TIMER_PRD_SET(p, v) 						(p->PRD = v)

//=========================== P11_LPTMRx->CNT =============================//
#define USR_LP_TIMER_CNT_SET(p, v) 						//(p->CNT = v)

//=========================== P11_LPTMRx->RSC =============================//
#define USR_LP_TIMER_PRD_RSC(p, v)

___interrupt
static void usr_lp_timer0_int(void)
{
    printf("%s: CON0 = 0x%x\n", __func__, P11_LPTMR0->CON0);

    if (USR_LP_TIMER_P11_TIMEOUT_PENDING_IS(P11_LPTMR0)) {
        USR_LP_TIMER_P11_TIMEOUT_PENDING_CLR(P11_LPTMR0);
        printf("p11 timer0 timerout\n");
    }
}

/*========================================================================
 * @brief:
 *
 * @param:
 * @return:
 ========================================================================*/
 static void (*lp_timer3_irq_handler_hook)(void) = NULL;
 extern volatile u8 user_test_flag;

___interrupt
static void usr_lp_timer1_int(void)
{
    printf("%s: CON0 = 0x%x\n", __func__, P11_LPTMR1->CON0);

//    gpio_set_direction(IO_PORTB_06, 0);
//    P11_PORT->PB_OUT ^= BIT(6);
extern  u32 get_lp_time_cnt;
    get_lp_time_cnt += 600;
    p2m_message[P2M_MESSAGE_TIME_CNT0] = (get_lp_time_cnt & 0xff);//p2m_message[39] + (32*1000);
    p2m_message[P2M_MESSAGE_TIME_CNT1] = ((get_lp_time_cnt >>8)&0xff);//p2m_message[39] + (32*1000);
    p2m_message[P2M_MESSAGE_TIME_CNT2] = ((get_lp_time_cnt >>16)&0xff);//p2m_message[39] + (32*1000);
    p2m_message[P2M_MESSAGE_TIME_CNT3] = ((get_lp_time_cnt >>24)&0xff);//p2m_message[39] + (32*1000);

    if (USR_LP_TIMER_P11_TIMEOUT_PENDING_IS(P11_LPTMR1)) {
        USR_LP_TIMER_P11_TIMEOUT_PENDING_CLR(P11_LPTMR1);
    //    printf("p11 timer3 timerout\n");
        if(!user_test_flag){
            DEBUG_IO_TEST_1();
            user_test_flag = 1;
        }else{
            DEBUG_IO_TEST_0();
            user_test_flag = 0;
        }
        if (lp_timer3_irq_handler_hook) {
            lp_timer3_irq_handler_hook();
        }
        printf("p11 timer1 timerout\n");
    }
}

/*========================================================================
 * @brief:
 *
 * @param:
 * @return:
 ========================================================================*/

___interrupt
static void usr_lp_timer2_int(void)
{
    printf("%s: CON0 = 0x%x\n", __func__, P11_LPTMR2->CON0);

    if (USR_LP_TIMER_P11_TIMEOUT_PENDING_IS(P11_LPTMR2)) {
        USR_LP_TIMER_P11_TIMEOUT_PENDING_CLR(P11_LPTMR2);
        printf("p11 timer2 timerout\n");
    }
}

/*========================================================================
 * @brief:
 *
 * @param:
 * @return:
 ========================================================================*/
 extern volatile u8 p2m_message[];
 u32 get_lp_time_cnt = 0;
 volatile u8 user_test_flag = 0;
#include "gpio.h"


___interrupt
static void usr_lp_timer3_int(void)
{
   // printf("%s: CON0 = 0x%x\n", __func__, P11_LPTMR3->CON0);


    get_lp_time_cnt += 600;
    p2m_message[P2M_MESSAGE_TIME_CNT0] = (get_lp_time_cnt & 0xff);//p2m_message[39] + (32*1000);
    p2m_message[P2M_MESSAGE_TIME_CNT1] = ((get_lp_time_cnt >>8)&0xff);//p2m_message[39] + (32*1000);
    p2m_message[P2M_MESSAGE_TIME_CNT2] = ((get_lp_time_cnt >>16)&0xff);//p2m_message[39] + (32*1000);
    p2m_message[P2M_MESSAGE_TIME_CNT3] = ((get_lp_time_cnt >>24)&0xff);//p2m_message[39] + (32*1000);
   // printf("msg_value = %d\n",get_lp_time_cnt );
    if (USR_LP_TIMER_P11_TIMEOUT_PENDING_IS(P11_LPTMR3)) {
        USR_LP_TIMER_P11_TIMEOUT_PENDING_CLR(P11_LPTMR3);
    //    printf("p11 timer3 timerout\n");
        if(!user_test_flag){
            DEBUG_IO_TEST_1();
            user_test_flag = 1;
        }else{
            DEBUG_IO_TEST_0();
            user_test_flag = 0;
        }
        if (lp_timer3_irq_handler_hook) {
            lp_timer3_irq_handler_hook();
        }
    }
}


static void usr_lp_timer_config_dump(P11_LPTMR_TypeDef *lp_timer)
{
    printf("lp_timer->CON0 = 0x%x\n", lp_timer->CON0);
    printf("lp_timer->CON1 = 0x%x\n", lp_timer->CON1);
    printf("lp_timer->CON2 = 0x%x\n", lp_timer->CON2);
    printf("lp_timer->PRD  = 0x%x\n", lp_timer->PRD);
    printf("lp_timer->RSC  = 0x%x\n", lp_timer->RSC);
    printf("lp_timer->CNT  = 0x%x\n", lp_timer->CNT);
}

const void *usr_usr_lp_timer_int_table[] = {
    usr_lp_timer0_int,
    usr_lp_timer1_int,
    usr_lp_timer2_int,
    usr_lp_timer3_int,
};

const void *usr_lp_timer_sfr_table[] = {
    P11_LPTMR0,
    P11_LPTMR1,
    P11_LPTMR2,
    P11_LPTMR3
};

#define USR_LP_TIMER_CLK_FREQ 			(200000UL)


static const u32 timer_div[] = {
    /*0000*/    1,
    /*0001*/    4,
    /*0010*/    16,
    /*0011*/    64,
};


void usr_lp_timer_init(P11_LPTMR_TypeDef *lp_timer, u32 us)
{
    u8 offset = 0;

    if (lp_timer == NULL) {
        return;
    }
    for (offset = 0; offset < ARRAY_SIZE(usr_lp_timer_sfr_table); offset++) {
        if (usr_lp_timer_sfr_table[offset] == lp_timer) {
            break;
        }
    }

    if (offset >= ARRAY_SIZE(usr_lp_timer_sfr_table)) {
        return;
    }

    u8 offset1 = (u32)((u32)lp_timer - (u32)P11_LPTMR0) / ((u32)P11_LPTMR1_BASE - (u32)P11_LPTMR0_BASE);

    printf("offset = 0x%x\n", offset);
    P11_CLOCK->WKUP_EN |= (1 << (offset + 9));
    INTALL_HWI(IRQ_LP_TMR0_IDX + offset, usr_usr_lp_timer_int_table[offset], 0);

    USR_LP_TIMER_CLOCK_SEL(lp_timer, USR_LP_TIMER_CLK_LRC_OSC); //200K

//    u32 MIN_TIME_CNT = 0x100;
//
//    u32 MAX_TIME_CNT = 0xffff / 4 * 3;//49,151.25
//    u32 prd_cnt =  0xffff;//(us * (USR_LP_TIMER_CLK_FREQ / 1000)) / (1000000UL / 100);/// =
    u32 prd_cnt =  600;//(us * (USR_LP_TIMER_CLK_FREQ / 1000)) / (1000000UL / 100);/// =

    USR_LP_TIMER_CLOCK_DIV_SEL(lp_timer, 0);///CON2 |????24BIT

    /* u32 prd = (us * (LP_TIMER_CLK_FREQ / 1000)) / (1000000UL / 100); */

    USR_LP_TIMER_PRD_SET(lp_timer, prd_cnt);///PRD
    USR_LP_TIMER_CNT_SET(lp_timer, 0);///PRD

    USR_LP_TIMER_P11_TIMEOUT_WK_IE(lp_timer, 1);///CON0 |????23BIT
//USR_LP_TIMER_P11_TIMEOUT_PENDING_CLR(lp_timer);
    USR_LP_TIMER_SOFT_KICK_START_ENABLE(lp_timer, 1);///CON1 |????20bit
    USR_LP_TIMER_HW_KICK_START_ENABLE(lp_timer, 0);///CON1|????21bit

    USR_LP_TIMER_CONTINUE(lp_timer, 1);///CON0 |????21bit

    USR_LP_TIMER_ENABLE(lp_timer, 1);///CON0|????20bit

    usr_lp_timer_config_dump(lp_timer);

    USR_LP_TIMER_KICK_START_RUN(lp_timer);///CON2 |????28

    return;
}

static void timer3_isr_fun(void)
{
//    struct loop_detect_handler *detect;
//
//    list_for_each_loop_detect(detect) {
//        if ((cur_time_tick %  detect->time) == 0) {
//            if (detect->fun) {
//                detect->fun(detect->priv);
//            }
//        }
//    }
//    printf("%d",P11_LPTMR3->CNT);
    putchar('2');
//    p2m_message[39] += (32*1000);
}



void user_lptime_init(void)
{
//        extern void usr_lp_timer_init(P11_LPTMR_TypeDef *lp_timer, u32 us);
    usr_lp_timer_init(P11_LPTMR1,40 * 1000);
   // lp_timer_register_irq(P11_LPTMR1, timer3_isr_fun);
}
