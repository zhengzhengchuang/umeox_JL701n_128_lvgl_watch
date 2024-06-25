#ifndef __IIC_HW_H__
#define __IIC_HW_H__

//======================================================//
//                    IIC HW DEFINE                     //
//======================================================//
//------------------- P11_IIC->CON0
#define IIC_CON0_CLEAR() 					(P11_IIC->CON0 = 0)
#define IIC_BYTE_PENDING_IS() 				(P11_IIC->CON0 & BIT(15))
#define IIC_BYTE_PENDING_CLEAR() 			(P11_IIC->CON0 |= BIT(14))
#define IIC_END_PENDING_IS() 				(P11_IIC->CON0 & BIT(13))
#define IIC_END_PENDING_CLEAR() 			(P11_IIC->CON0 |= BIT(12))
#define IIC_END_PENDING_INT_ENABLE(v) 		SFR(P11_IIC->CON0, 10, 1, v)
#define IIC_IO_FILTER_INPUT_ENABLE(v) 		SFR(P11_IIC->CON0, 9, 1, v)
#define IIC_INT_ENABLE(v) 					SFR(P11_IIC->CON0, 8, 1, v)
#define IIC_TRANSMIT_ACK_IS() 				(P11_IIC->CON0 & BIT(7))
#define IIC_RECEIVE_ACK_SET(v) 				SFR(P11_IIC->CON0, 6, 1, v)
#define IIC_MASTER_RESTART_SET(v) 			SFR(P11_IIC->CON0, 5, 1, v)
#define IIC_MASTER_END_SET(v) 				SFR(P11_IIC->CON0, 4, 1, v)
#define IIC_DIR_TRANSMIT() 					(P11_IIC->CON0 &= ~BIT(3))
#define IIC_DIR_RECEIVE() 					(P11_IIC->CON0 |= BIT(3))
#define IIC_CONFIG_DONE() 					(P11_IIC->CON0 |= BIT(2))
#define IIC_MASTER_MODE() 					(P11_IIC->CON0 &= ~BIT(1))
#define IIC_SLAVE_MODE() 					(P11_IIC->CON0 |= BIT(1))
#define IIC_MODULE_ENABLE(v) 				SFR(P11_IIC->CON0, 0, 1, v)

//------------------- P11_IIC->CON1
#define IIC_CON1_CLEAR() 					(P11_IIC->CON1 = 0)
#define IIC_START_PENDING_IS() 				(P11_IIC->CON1 & BIT(15))
#define IIC_START_PENDING_CLEAR() 			(P11_IIC->CON1 |= BIT(14))
#define IIC_SLAVE_IGNORE_BUS_MODE(v) 		SFR(P11_IIC->CON1, 13, 1, v)

//------------------- P11_IIC->BAUD
#define IIC_MASTER_BAUD_CONFIG(v) 			(P11_IIC->BAUD = v)
#define IIC_SLAVE_ADDR_CONFIG(v) 			SFR(P11_IIC->BAUD, 1, 7, v)

//------------------- P11_IIC->BUF
#define IIC_READ_BUF() 						(P11_IIC->BUF)
#define IIC_WRITE_BUF(v) 					(P11_IIC->BUF = v)

#endif /* #ifndef __IIC_HW_H__ */
