#ifndef __P11_APP_H__
#define __P11_APP_H__

//-----special operation----------
//P11_CLOCK->PWR_CON
#define RESET_FLAG()                (P11_CLOCK->PWR_CON & BIT(7))

#define RESET_FLAG_CLR()            P11_CLOCK->PWR_CON |= BIT(6); \
                                               NOP()

#define INIT_FLAG_SET()             P11_CLOCK->PWR_CON |= BIT(5); \
                                               NOP()

//-----special operation----------
#define SET_RECOVER_FLAG() 			P11_CLOCK->PWR_CON |= BIT(5); \
											   NOP()

#define INIT_FLAG_GET()             (P11_CLOCK->PWR_CON &= ~BIT(5))

#define INIT_FLAG_CLR()             P11_CLOCK->PWR_CON &= ~BIT(5); \
                                               NOP()

#define SOFT_RESET_SET()            P11_CLOCK->PWR_CON |= BIT(4); \
                                               NOP()

#define SSMODE_NO_DLY()             P11_CLOCK->PWR_CON |= BIT(3); \
                                               NOP()

#define SSMODE_DLY()                P11_CLOCK->PWR_CON &= ~BIT(3); \
                                               NOP()

//========================== P11_SYSTEM->P2M_CLK_CON0
#define P2M_RC_250K                 BIT(0)
#define P2M_RC_16M                  BIT(1)
#define P2M_BT_OSC                  BIT(2)
#define P2M_LRC                     BIT(3)
#define P2M_CLK_SET(x)              P11_SYSTEM->P2M_CLK_CON0 = ((P11_SYSTEM->P2M_CLK_CON0 & ~0x1f) | (x) & 0x1f)

#define P2M_CLK_GET(x)              (P11_SYSTEM->P2M_CLK_CON0 & 0x1f)

#define LED_CLK_SEL(x)              P11_SYSTEM->P2M_CLK_CON0 = ((P11_SYSTEM->P2M_CLK_CON0 & ~0xe0) | (x) << 5)
enum {
    LED_CLK_SEL_NONE = 0,
    LED_CLK_SEL_RC_250K,
    LED_CLK_SEL_RC_16M,
    LED_CLK_SEL_BT_OSC,
    LED_CLK_SEL_LRC,
};

#define GET_LED_CLK_SEL(x)           (P11_SYSTEM->P2M_CLK_CON0 & 0xe0)

//========================== P11_SYSTEM->P11_SYS_CON0
#define MSYS_STATE_SET(x)            P11_SYSTEM->P11_SYS_CON0 = ((P11_SYSTEM->P11_SYS_CON0 & ~0x0f) | (x) & 0xf)
enum {
    MSYS_STATE_NORMAL = 0,
    MSYS_STATE_PWR_DOWN,
    MSYS_STATE_PWR_OFF,
    MSYS_STATE_SOFT_PWR_OFF,
    MSYS_STATE_LIGHT_PWR_DOWN,
};

#define MSYS_STATE_GET()             (P11_SYSTEM->P11_SYS_CON0 & 0xf)
#define P11_KEY_CLR()               P11_SYSTEM->P11_SYS_CON0 |= BIT(4)
#define P33_WKUP_MSYS_ENABLE()      P11_SYSTEM->P11_SYS_CON0 |= BIT(5)
#define P33_WKUP_MSYS_DISABLE()     P11_SYSTEM->P11_SYS_CON0 &= ~BIT(5)
#define GET_P33_WKUP_MSYS()			(P11_SYSTEM->P11_SYS_CON0 & BIT(5) ? 1:0)
#define SET_P33_WKUP_MSYS(en)		SFR(P11_SYSTEM->P11_SYS_CON0, 5, 1, en)
#define P33_WKUP_IE_SET()           P11_SYSTEM->P11_SYS_CON0 |= BIT(6)
#define P33_WKUP_IE_CLR()           P11_SYSTEM->P11_SYS_CON0 &= ~BIT(6)
#define P33_WKUP_PND()              (P11_SYSTEM->P11_SYS_CON0 & BIT(7))

//========================== P11_SYSTEM->P11_SYS_CON1
#define P11_ISOLATE_ENABLE()        P11_SYSTEM->P11_SYS_CON1 |= BIT(0)
#define P11_ISOLATE_DISABLE()       P11_SYSTEM->P11_SYS_CON1 &= ~BIT(0)
#define P11_RX_DISABLE()            P11_SYSTEM->P11_SYS_CON1 |= BIT(1)
#define P11_RX_ENABLE()             P11_SYSTEM->P11_SYS_CON1 &= ~BIT(1)
#define P11_TX_DISABLE()            P11_SYSTEM->P11_SYS_CON1 |= BIT(2)
#define P11_TX_ENABLE()             P11_SYSTEM->P11_SYS_CON1 &= ~BIT(2)
#define P2M_RESET_ENABLE()          P11_SYSTEM->P11_SYS_CON1 &= ~BIT(3)
#define P2M_RESET_DISABLE()         P11_SYSTEM->P11_SYS_CON1 |= BIT(3)
#define M2P_RESET_MASK_ENABLE()     P11_SYSTEM->P11_SYS_CON1 |= BIT(4)
#define M2P_RESET_MASK_DISABLE()    P11_SYSTEM->P11_SYS_CON1 &= ~BIT(4)
#define MSYS_DPOR_EN_ENABLE()       P11_SYSTEM->P11_SYS_CON1 |= BIT(5)
#define MSYS_DPOR_EN_DISABLE()      P11_SYSTEM->P11_SYS_CON1 &= ~BIT(5)
#define MSYS_DPOR_ISO_ENABLE()      P11_SYSTEM->P11_SYS_CON1 |= BIT(6)
#define MSYS_DPOR_ISO_DISABLE()     P11_SYSTEM->P11_SYS_CON1 &= ~BIT(6)
#define MSYS_IO_LATCH_ENABLE()      P11_SYSTEM->P11_SYS_CON1 |= BIT(7)
#define MSYS_IO_LATCH_DISABLE()     P11_SYSTEM->P11_SYS_CON1 &= ~BIT(7)

#define MSYS_VDD_POR()              (P11_SYSTEM->P11_SYS_CON1 & BIT(8))
#define MSYS_RESET()                (P11_SYSTEM->P11_SYS_CON1 & BIT(9))


//========================== P11_WDT->CON
enum {
    WDT_1Ms = 0,
    WDT_2Ms,
    WDT_4Ms,
    WDT_8Ms,

    WDT_16Ms,
    WDT_32Ms,
    WDT_64Ms,
    WDT_128Ms,

    WDT_256Ms,
    WDT_512Ms,
    WDT_1s,
    WDT_2s,
    WDT_4s,
    WDT_8s,
    WDT_16s,
    WDT_32s,
};

#define WDT_PND()           (P11_WDT->CON & BIT(7))
#define WDT_CLR_PND()       P11_WDT->CON |= BIT(6)
#define WDT_MODE_RESET()    P11_WDT->CON &= ~BIT(5)
#define WDT_MODE_INT()      P11_WDT->CON |= BIT(5)
#define WDT_ENABLE()        P11_WDT->CON |= BIT(4)
#define WDT_DISABLE()       P11_WDT->CON &= ~BIT(4)
#define WDT_TIMEOUT(x)      P11_WDT->CON &= ~0xf; P11_WDT->CON |= x & 0xf

#define WDT_EXCEPTION_EN()  P11_WDT->CON |= BIT(8)
#define WDT_EXCEPTION_DIS() P11_WDT->CON &= ~BIT(8)

/*
 *------------------- P11_CLOCK->CLK_CON
 */


enum UART_CLK_TABLE {
    UART_CLK_DIABLE = 0,
    UART_CLK_STD_12M,
    UART_CLK_RC16M,
    UART_CLK_SYS_CLK,
};
#define P11_UART_CLK_SEL(x) 		SFR(P11_CLOCK->CLK_CON2, 2, 2, x)

//=============================================================//
//                     VAD lowpower时钟选项                    //
//=============================================================//
enum VAD_LP_CLOCK_SOURCE {
    VAD_LP_CLK_BTOSC,
    VAD_LP_CLK_RC16M,
    VAD_LP_CLK_LRC,
};
//=============================================================//
//                     M2P中断消息列表                         //
//=============================================================//

/*
 *  Must Sync to MSYS code
 */

enum {
    M2P_LP_INDEX    = 0,
    M2P_PF_INDEX,
    M2P_LLP_INDEX,
    M2P_P33_INDEX,
    M2P_SF_INDEX,
    M2P_CTMU_INDEX,
    M2P_CCMD_INDEX,       //common cmd
    M2P_VAD_INDEX,
    M2P_USER_INDEX,
    M2P_WDT_INDEX,
};

//=============================================================//
//                     P2M中断消息列表                         //
//=============================================================//
enum {
    P2M_LP_INDEX    = 0,
    P2M_PF_INDEX,
    P2M_LLP_INDEX,
    P2M_WK_INDEX,
    P2M_WDT_INDEX,
    P2M_LP_INDEX2,
    P2M_CTMU_INDEX,
    P2M_CTMU_POWUP,
    P2M_REPLY_CCMD_INDEX,  //reply common cmd
    P2M_VAD_INDEX,
    P2M_USER_INDEX,
    P2M_BANK_INDEX,
    P2M_LPTMR_INDEX,
};

//=============================================================//
//                     P2M通信消息列表                         //
//=============================================================//
enum {
    P2M_MESSAGE_WKUP_SRC = 0,
    P2M_MESSAGE_WKUP_PND0,
    P2M_MESSAGE_WKUP_PND1,
    P2M_REPLY_COMMON_CMD,
    P2M_MESSAGE_VAD_CMD,
    P2M_MESSAGE_VAD_CBUF_WPTR,
    P2M_MESSAGE_BANK_ADR_L,
    P2M_MESSAGE_BANK_ADR_H,
    P2M_MESSAGE_BANK_INDEX,
    P2M_MESSAGE_BANK_ACK,
    P2M_P11_HEAP_BEGIN_ADDR_L = 10,
    P2M_P11_HEAP_BEGIN_ADDR_H,
    P2M_P11_HEAP_SIZE_L,
    P2M_P11_HEAP_SIZE_H,


    P2M_MESSAGE_CTMU_KEY_EVENT = 16,
    P2M_MESSAGE_CTMU_KEY_CNT,
    P2M_MESSAGE_CTMU_WKUP_MSG,
    P2M_MESSAGE_CTMU_EARTCH_EVENT,

    P2M_MESSAGE_CTMU_CH0_L_RES,
    P2M_MESSAGE_CTMU_CH0_H_RES,
    P2M_MESSAGE_CTMU_CH1_L_RES,
    P2M_MESSAGE_CTMU_CH1_H_RES,
    P2M_MESSAGE_CTMU_CH2_L_RES,
    P2M_MESSAGE_CTMU_CH2_H_RES,
    P2M_MESSAGE_CTMU_CH3_L_RES,
    P2M_MESSAGE_CTMU_CH3_H_RES,
    P2M_MESSAGE_CTMU_CH4_L_RES,
    P2M_MESSAGE_CTMU_CH4_H_RES,

    P2M_MESSAGE_CTMU_EARTCH_L_IIR_VALUE,
    P2M_MESSAGE_CTMU_EARTCH_H_IIR_VALUE,
    P2M_MESSAGE_CTMU_EARTCH_L_TRIM_VALUE,
    P2M_MESSAGE_CTMU_EARTCH_H_TRIM_VALUE,
    P2M_MESSAGE_CTMU_EARTCH_L_DIFF_VALUE,
    P2M_MESSAGE_CTMU_EARTCH_H_DIFF_VALUE,

    P2M_MESSAGE_AWKUP_P_PND,
    P2M_MESSAGE_AWKUP_N_PND,
    P2M_MESSAGE_WKUP_RTC,

    //继续从这个添加
    P2M_MESSAGE_TIME_CNT0,
    P2M_MESSAGE_TIME_CNT1,
    P2M_MESSAGE_TIME_CNT2,
    P2M_MESSAGE_TIME_CNT3,


    //
    P2M_USER_PEND =      56,//传感器使用或者开放客户使用
    P2M_USER_MSG_TYPE  = 57,
    P2M_USER_MSG_LEN0 =  58,
    P2M_USER_MSG_LEN1  = 59,
    P2M_USER_ADDR0  =    60,
    P2M_USER_ADDR1  =    61,
    P2M_USER_ADDR2 =     62,
    P2M_USER_ADDR3 =     63,
};

//=============================================================//
//                     M2P通信消息列表                         //
//=============================================================//
enum {
    /*===========power=============*/
    M2P_LRC_PRD = 0,
    M2P_MESSAGE_WVDD,
    M2P_LRC_TMR_50us,
    M2P_LRC_TMR_200us,
    M2P_LRC_TMR_600us,
    M2P_VDDIO_KEEP,
    M2P_LRC_KEEP,
    M2P_COMMON_CMD,
    M2P_MESSAGE_VAD_CMD,
    M2P_MESSAGE_VAD_CBUF_RPTR,
    M2P_VDDIO_KEEP_TYPE = 10,
    M2P_RCH_FEQ_H,
    M2P_MEM_CONTROL,
    M2P_BTOSC_KEEP,
    M2P_CTMU_KEEP,
    M2P_RTC_KEEP,
    M2P_WDT_SYNC,
    M2P_PVDD_LEVEL_SLEEP_TRIM,

    M2P_LRC_FEQ2 = 18,
    M2P_LRC_FEQ1,
    M2P_LRC_FEQ0,

    M2P_LIGHT_PDOWN_DVDD_VOL,

    M2P_MESSAGE_CTMU_CMD = 24,
    M2P_MESSAGE_CTMU_MSG,
    M2P_CTMU_PRD0,
    M2P_CTMU_PRD1,
    M2P_CTMU_CH_ENABLE,
    M2P_CTMU_CH_DEBUG,
    M2P_CTMU_CH_CFG,
    M2P_CTMU_CH_WAKEUP_EN,
    M2P_CTMU_EARTCH_CH,
    M2P_CTMU_TIME_BASE,

    M2P_MESSAGE_CTMU_LONG_TIME_L,
    M2P_MESSAGE_CTMU_LONG_TIME_H,
    M2P_MESSAGE_CTMU_HOLD_TIME_L,
    M2P_MESSAGE_CTMU_HOLD_TIME_H,
    M2P_MESSAGE_CTMU_SOFTOFF_LONG_TIME_L,
    M2P_MESSAGE_CTMU_SOFTOFF_LONG_TIME_H,
    M2P_CTMU_LONG_PRESS_RESET_TIME_VALUE_L, //长按复位
    M2P_CTMU_LONG_PRESS_RESET_TIME_VALUE_H,

    M2P_CTMU_INEAR_VALUE_L,
    M2P_CTMU_INEAR_VALUE_H,
    M2P_CTMU_OUTEAR_VALUE_L,
    M2P_CTMU_OUTEAR_VALUE_H,
    M2P_MESSAGE_CTMU_EARTCH_TRIM_VALUE_L,
    M2P_MESSAGE_CTMU_EARTCH_TRIM_VALUE_H,

    M2P_MESSAGE_CTMU_CH0_CFG0L = 48,
    M2P_MESSAGE_CTMU_CH0_CFG0H,
    M2P_MESSAGE_CTMU_CH0_CFG1L,
    M2P_MESSAGE_CTMU_CH0_CFG1H,
    M2P_MESSAGE_CTMU_CH0_CFG2L,
    M2P_MESSAGE_CTMU_CH0_CFG2H,

    M2P_MESSAGE_CTMU_CH1_CFG0L = 56,
    M2P_MESSAGE_CTMU_CH1_CFG0H,
    M2P_MESSAGE_CTMU_CH1_CFG1L,
    M2P_MESSAGE_CTMU_CH1_CFG1H,
    M2P_MESSAGE_CTMU_CH1_CFG2L,
    M2P_MESSAGE_CTMU_CH1_CFG2H,

    M2P_MESSAGE_CTMU_CH2_CFG0L = 64,
    M2P_MESSAGE_CTMU_CH2_CFG0H,
    M2P_MESSAGE_CTMU_CH2_CFG1L,
    M2P_MESSAGE_CTMU_CH2_CFG1H,
    M2P_MESSAGE_CTMU_CH2_CFG2L,
    M2P_MESSAGE_CTMU_CH2_CFG2H,

    M2P_MESSAGE_CTMU_CH3_CFG0L = 72,
    M2P_MESSAGE_CTMU_CH3_CFG0H,
    M2P_MESSAGE_CTMU_CH3_CFG1L,
    M2P_MESSAGE_CTMU_CH3_CFG1H,
    M2P_MESSAGE_CTMU_CH3_CFG2L,
    M2P_MESSAGE_CTMU_CH3_CFG2H,

    M2P_MESSAGE_CTMU_CH4_CFG0L = 80,
    M2P_MESSAGE_CTMU_CH4_CFG0H,
    M2P_MESSAGE_CTMU_CH4_CFG1L,
    M2P_MESSAGE_CTMU_CH4_CFG1H,
    M2P_MESSAGE_CTMU_CH4_CFG2L,
    M2P_MESSAGE_CTMU_CH4_CFG2H,
    M2P_RVD2PVDD_EN,
    M2P_PVDD_EXTERN_DCDC,

    M2P_USER_PEND =      88,
    M2P_USER_MSG_TYPE  = 89,
    M2P_USER_MSG_LEN0 =  90,
    M2P_USER_MSG_LEN1  = 91,
    M2P_USER_ADDR0  =    92,
    M2P_USER_ADDR1  =    93,
    M2P_USER_ADDR2 =     94,
    M2P_USER_ADDR3 =     95,
};

enum {
    CLOSE_P33_INTERRUPT = 1,
    OPEN_P33_INTERRUPT,
    LOWPOWER_PREPARE,
    CHARGE_ENABLE,
    CHARGE_DISABLE,
    UPDATE_LRC_FEQ,

    M2P_WDT_CLEAR = 8,

    P2M_RESERVED_CMD = 0xFF,
};

enum {
    VDDIO_KEEP_TYPE_TRIM = 0,//vddio keep使用trim值：默认
    VDDIO_KEEP_TYPE_PG,		//vddio keep使用 mvvdio：功耗代价100ua
};

#define IS_TMR0_SRC_CLK_BTOSC()     ((P11_LPTMR0->CON2 & 0xf) == 0x2)

#define P11_WKUP_SYS()              P11_SYSTEM->P2M_INT_SET = m2p_cmd; m2p_cmd = 0
#define P11_WKUP_SYS_POST()         P11_SYSTEM->P2M_INT_SET = BIT(P2M_LP_INDEX2)

#define RC_250k_EN(a)       \
    if (a) {    \
        P11_CLOCK->CLK_CON1 |= BIT(10); \
    } else { \
        P11_CLOCK->CLK_CON1 &= ~BIT(10); \
    }

//simple version
//
#define ENTER_CRITICAL() 			disable_int()

#define EXIT_CRITICAL()  			enable_int()

#define PDOWN   0
#define POFF    1
#define PDOWN_VAD           2

extern void __power_recover(void);
extern void __power_recover(void);
extern void p11_reset_main_system();
extern void lp_timer1_enable(void);
extern void lp_timer1_disable(void);

extern void idle();
extern void standby();
extern void sleep();
extern void deep_sleep();

#define CALL_DEEP_SLEEP() 		 deep_sleep()
#define CALL_SLEEP() 			 sleep()
#define CALL_STANDBY() 			 standby()
#define CALL_IDLE() 			 idle()

#endif /* #ifndef __P11_APP_H__ */
