/*********************************************************************************************
    *   Filename        : gpio.h

    *   Description     : 本文件存放p11 gpio的接口函数和宏定义

    *   Author          : MoZhiYe

    *   Email           : mozhiye@zh-jieli.com

    *   Last modifiled  : 2021-05-17 09:00

    *   Copyright:(c)JIELI  2021-2029  @ , All Rights Reserved.
*********************************************************************************************/

#ifndef __GPIO_H__
#define __GPIO_H__

#define IO_GROUP_NUM 				16

//PB0 ~ PB13
#define IO_PORTB_00 				(IO_GROUP_NUM * 1 + 0)
#define IO_PORTB_01 				(IO_GROUP_NUM * 1 + 1)
#define IO_PORTB_02 				(IO_GROUP_NUM * 1 + 2)
#define IO_PORTB_03 				(IO_GROUP_NUM * 1 + 3)
#define IO_PORTB_04 				(IO_GROUP_NUM * 1 + 4)
#define IO_PORTB_05 				(IO_GROUP_NUM * 1 + 5)
#define IO_PORTB_06 				(IO_GROUP_NUM * 1 + 6)
#define IO_PORTB_07 				(IO_GROUP_NUM * 1 + 7)
#define IO_PORTB_08 				(IO_GROUP_NUM * 1 + 8)
#define IO_PORTB_09 				(IO_GROUP_NUM * 1 + 9)
#define IO_PORTB_10 				(IO_GROUP_NUM * 1 + 10)
#define IO_PORTB_11 				(IO_GROUP_NUM * 1 + 11)
// #define IO_PORTB_12 				(IO_GROUP_NUM * 1 + 12)
// #define IO_PORTB_13 				(IO_GROUP_NUM * 1 + 13)
// #define IO_PORTB_14 				(IO_GROUP_NUM * 1 + 14)
// #define IO_PORTB_15 				(IO_GROUP_NUM * 1 + 15)

#define IO_MAX_NUM 					IO_PORTB_11

#ifndef EINVAL
#define EINVAL      22  /* Invalid argument */
#endif /* #ifndef EINVAL */

#ifndef EFAULT
#define	EFAULT		14	/* Bad address */
#endif /* #ifndef EFAULT */

//===================================================//
// BR28 DEBUG IO
//===================================================//
#define DEBUG_GPIO IO_PORTB_03

#define DEBUG_IO_1() //{P11_PORT->PB_OUT |=  BIT(DEBUG_GPIO-IO_PORTB_00), P11_PORT->PB_SEL |= BIT(DEBUG_GPIO-IO_PORTB_00), P11_PORT->PB_DIR &= ~BIT(DEBUG_GPIO-IO_PORTB_00);}
#define DEBUG_IO_0() //{P11_PORT->PB_OUT &= ~BIT(DEBUG_GPIO-IO_PORTB_00), P11_PORT->PB_SEL |= BIT(DEBUG_GPIO-IO_PORTB_00), P11_PORT->PB_DIR &= ~BIT(DEBUG_GPIO-IO_PORTB_00);}

#define DEBUG_IO_TEST_1() // {P11_PORT->PB_OUT |=  BIT(DEBUG_GPIO-IO_PORTB_00), P11_PORT->PB_SEL |= BIT(DEBUG_GPIO-IO_PORTB_00),\
                            P11_PORT->PB_DIR &= ~BIT(DEBUG_GPIO-IO_PORTB_00);}

#define DEBUG_IO_TEST_0() // {P11_PORT->PB_OUT &= ~BIT(DEBUG_GPIO-IO_PORTB_00), P11_PORT->PB_SEL |= BIT(DEBUG_GPIO-IO_PORTB_00),\
                            P11_PORT->PB_DIR &= ~BIT(DEBUG_GPIO-IO_PORTB_00);}





//===================================================//
// BR28 P11 Crossbar API
//===================================================//

enum OCH_SIGNAL_TABLE {
    OCH_SIGNAL_RC250K = 1,
    OCH_SIGNAL_RC16M,
    OCH_SIGNAL_BT_OSC,
    OCH_SIGNAL_LRC_OSC,
    OCH_SIGNAL_RTC_OSC,
    OCH_SIGNAL_SYS_CLK,
    OCH_SIGNAL_UART1_TX,
    OCH_SIGNAL_SLEEP_R,
    OCH_SIGNAL_LCTMU_TEST,
};

enum PORT_ICHX_SEL {
    ICHX_GP_ICH0,
    ICHX_GP_ICH1,
    ICHX_GP_ICH2,
    ICHX_P33_PORT_IN,
    ICHX_GPTMR0_PWM,
    ICHX_GPTMR1_PWM,
};

enum PFI_TABLE {
    PFI_GP_ICH0 = ((u32)(&(P11_IMAP->P11_FI_GP_ICH0))),
    PFI_GP_ICH1 = ((u32)(&(P11_IMAP->P11_FI_GP_ICH1))),
    PFI_GP_ICH2 = ((u32)(&(P11_IMAP->P11_FI_GP_ICH2))),
    PFI_UART0_RX = ((u32)(&(P11_IMAP->P11_FI_UART0_RX))),
    PFI_SPI_DI = ((u32)(&(P11_IMAP->P11_FI_SPI_DI))),
    PFI_IIC_SCL = ((u32)(&(P11_IMAP->P11_FI_IIC_SCL))),
    PFI_IIC_SDA = ((u32)(&(P11_IMAP->P11_FI_IIC_SDA))),
    PFI_DMIC_DAT = ((u32)(&(P11_IMAP->P11_FI_DMIC_DAT))),
};

int gpio_set_direction(u32 gpio, u32 dir);

int gpio_set_output_value(u32 gpio, u32 value);

int gpio_set_pull_up(u32 gpio, u32 value);

int gpio_set_pull_down(u32 gpio, int value);

int gpio_set_die(u32 gpio, int value);

int gpio_set_dieh(u32 gpio, int value);

int gpio_read(u32 gpio);

int gpio_set_fun_output_port(u32 gpio, u32 fun_index, u8 dir_ctl, u8 data_ctl);

int gpio_set_fun_input_port(u32 gpio, enum PFI_TABLE pfun);


#endif /* #ifndef __GPIO_H__ */
