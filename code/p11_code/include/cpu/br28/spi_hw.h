#ifndef __SPI_HW_H__
#define __SPI_HW_H__

//======================================================//
//                    SPI HW DEFINE                     //
//======================================================//
//------------------- P11_SPI->CON
#define SPI_CON_CLEAR() 				(P11_SPI->CON = 0)
#define SPI_PENDING_IS() 				(P11_SPI->CON & BIT(15))
#define SPI_PENDING_CLEAR() 			(P11_SPI->CON |= BIT(14))
#define SPI_INT_ENABLE(v) 				SFR(P11_SPI->CON, 13, 1, v)
#define SPI_DIR_TRAMSMIT() 				(P11_SPI->CON &= ~BIT(12))
#define SPI_DIR_RECEIVE() 				(P11_SPI->CON |= BIT(12))
#define SPI_CS_IDLE_LOGIC(v) 			SFR(P11_SPI->CON, 7, 1, v)
#define SPI_CLK_IDLE_LOGIC(v) 			SFR(P11_SPI->CON, 6, 1, v)
#define SPI_UPDETE_AT_RISE_EDGE() 		(P11_SPI->CON &= ~BIT(5))
#define SPI_UPDETE_AT_FALL_EDGE() 		(P11_SPI->CON |= BIT(5))
#define SPI_SIMPLE_AT_RISE_EDGE() 		(P11_SPI->CON &= ~BIT(4))
#define SPI_SIMPLE_AT_FALL_EDGE() 		(P11_SPI->CON |= BIT(4))
#define SPI_BI_DIR(v) 					SFR(P11_SPI->CON, 3, 1, v) //双向/单向
#define SPI_CS_ENABLE(v) 				SFR(P11_SPI->CON, 2, 1, v) //always set 1
#define SPI_MASTER_MODE() 				(P11_SPI->CON &= ~BIT(1))
#define SPI_SLAVE_MODE() 				(P11_SPI->CON |= BIT(1))
#define SPI_MODULE_ENABLE(v) 			SFR(P11_SPI->CON, 0, 1, v)

//------------------- P11_SPI->BAUD
#define SPI_BAUD_CONFIG(v) 				(P11_SPI->BAUD = v)

//------------------- P11_SPI->BUF
#define SPI_BUF_READ() 					(P11_SPI->BUF)
#define SPI_BUF_WRITE(v) 				(P11_SPI->BUF = v)

//------------------- P11_SPI->ADR
#define SPI_DMA_ADDR_CONFIG(v) 			(P11_SPI->ADR = (u32)v)
#define SPI_DMA_ADDR_GET(v) 			(P11_SPI->ADR)

//------------------- P11_SPI->CNT
#define SPI_START_DMA_CNT(v) 			(P11_SPI->CNT = v)

#endif /* #ifndef __SPI_HW_H__ */
