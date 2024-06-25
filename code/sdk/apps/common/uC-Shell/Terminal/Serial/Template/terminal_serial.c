/*
*********************************************************************************************************
*                                              uC/Shell
*                                            Shell utility
*
*                    Copyright 2007-2020 Silicon Laboratories Inc. www.silabs.com
*
*                                 SPDX-License-Identifier: APACHE-2.0
*
*               This software is subject to an open source license and is distributed by
*                Silicon Laboratories Inc. pursuant to the terms of the Apache License,
*                    Version 2.0 available at www.apache.org/licenses/LICENSE-2.0.
*
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                              TERMINAL
*
*                                    TEMPLATE COMMUNICATIONS PORT
*
* Filename : terminal_serial.c
* Version  : V1.04.00
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include  <terminal.h>
#include  "device/uart.h"
#include  "asm/gpio.h"
#include  "app_config.h"


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                        TerminalSerial_Init()
*
* Description : Initialize serial communications.
*
* Argument(s) : none.
*
* Return(s)   : DEF_OK,   if interface was opened.
*               DEF_FAIL, otherwise.
*
* Caller(s)   : Terminal_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

#define TERMINAL_UART_NUM 1
static u32 dma_rxbuf[128 / 4];

CPU_BOOLEAN  TerminalSerial_Init(void)
{
    struct uart_platform_data uart_config = {
        .baudrate = 2000000,
#if TCFG_UC_SHELL_EN
        .tx_pin = UC_SHELL_IO_TX_PIN,
        .rx_pin = UC_SHELL_IO_RX_PIN,
#else
        .tx_pin = IO_PORTA_12,
        .rx_pin = IO_PORTA_13,
#endif
    };

    int ret = uart_init_2(TERMINAL_UART_NUM, &uart_config);
    if (ret == 0) {
        uart_dma_rx_init(TERMINAL_UART_NUM, dma_rxbuf, 128);
    }

    return 1;
}


/*
*********************************************************************************************************
*                                        TerminalSerial_Exit()
*
* Description : Uninitialize serial communications.
*
* Argument(s) : none.
*
* Return(s)   : none.
*
* Caller(s)   : Terminal_Init().
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  TerminalSerial_Exit(void)
{

}


/*
*********************************************************************************************************
*                                         TerminalSerial_Wr()
*
* Description : Serial output.
*
* Argument(s) : pbuf        Pointer to the buffer to transmit.
*
*               buf_len     Number of bytes in the buffer.
*
* Return(s)   : none.
*
* Caller(s)   : Terminal_Out().
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT16S  TerminalSerial_Wr(void        *pbuf,
                              CPU_SIZE_T   buf_len)
{
    for (int i = 0; i < buf_len; i++) {
        uart_putbyte(TERMINAL_UART_NUM, ((char *)pbuf)[i]);
    }
    return 0;
}


/*
*********************************************************************************************************
*                                       TerminalSerial_RdByte()
*
* Description : Serial byte input.
*
* Argument(s) : none.
*
* Return(s)   : Byte read from port.
*
* Caller(s)   : various.
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_INT08U  TerminalSerial_RdByte(void)
{
    return (u8)uart_getbyte(TERMINAL_UART_NUM);
}


/*
*********************************************************************************************************
*                                       TerminalSerial_WrByte()
*
* Description : Serial byte output.
*
* Argument(s) : c           Byte to write.
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  TerminalSerial_WrByte(CPU_INT08U  c)
{
    uart_putbyte(TERMINAL_UART_NUM, c);
}
