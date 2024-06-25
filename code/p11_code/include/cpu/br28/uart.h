#ifndef __UART_H__
#define __UART_H__

//======================================================//
//                    UART HW DEFINE                    //
//======================================================//
//------------------- P11_UARTx->BAUD
#define UART_BAUD_SET(x,v) 		(x->BAUD = v)

//------------------- P11_UARTx->CLK_CON0
#define UART_CON0_CLEAR(x) 			(x->CON0 = 0)
#define UART_TX_PENDING_IS(x) 		(x->CON0 & BIT(15))
#define UART_RX_PENDING_IS(x) 		(x->CON0 & BIT(14))
#define UART_TX_PENDING_CLEAR(x) 	(x->CON0 |= BIT(13))
#define UART_RX_PENDING_CLEAR(x) 	(x->CON0 |= BIT(12))
#define UART_OT_PENDING_IS(x) 		(x->CON0 & BIT(11))
#define UART_OT_PENDING_CLEAR(x) 	(x->CON0 |= BIT(10))
#define UART_OT_INT_ENABLE(x) 		(x->CON0 |= BIT(5))
#define UART_OT_INT_DISABLE(x) 		(x->CON0 &= ~BIT(5))

enum UART_CLK_DIV_TABLE {
    UART_CLK_DIV_4,
    UART_CLK_DIV_3,
};
/*
波特率计算:
 	UART_BAUD_SET  = Fuart_clk / ((BAUD + 1) * UART_DIV_SEL)
 */
#define UART_DIV_SEL(x,v) 			SFR(x->CON0, 4, 1, v)

#define UART_RX_INT_ENABLE(x) 		(x->CON0 |= BIT(3))
#define UART_RX_INT_DISABLE(x) 		(x->CON0 &= ~BIT(3))
#define UART_TX_INT_ENABLE(x) 		(x->CON0 |= BIT(2))
#define UART_TX_INT_DISABLE(x) 		(x->CON0 &= ~BIT(2))
#define UART_RX_ENABLE(x) 			(x->CON0 |= BIT(1))
#define UART_RX_DISABLE(x) 			(x->CON0 &= ~BIT(1))
#define UART_TX_ENABLE(x) 			(x->CON0 |= BIT(0))
#define UART_TX_DISABLE(x) 			(x->CON0 &= ~BIT(0))

//------------------- P11_UARTx->CLK_CON2
#define UART_CON1_CLEAR(x) 			(x->CON2 = 0)
#define UART_9BIT_ENABLE(x) 		(x->CON2 |= BIT(0))
#define UART_9BIT_DISABLE(x) 		(x->CON2 &= ~BIT(0))
#define UART_9BIT_TX_DATA(x,v) 		SFR(x->CON2, 1, 1 v)
#define UART_9BIT_RX_DATA(x) 		(x->CON2 & BIT(2))

//------------------- P11_UARTx->BUF
#define UART_BUF_WRITE(x,v) 		(x->BUF = v)
#define UART_BUF_READ(x) 			(x->BUF)

//------------------- P11_UARTx->OTCNT
/*
OT超时计算:
	Tot = Tuart_clk * UART_OTCNT_SET
 */
#define UART_OTCNT_SET(x,v) 			(x->OTCNT = v)

void debug_uart_init(u8 tx_port);

#endif

