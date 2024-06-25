#ifndef __PSRAM_HW_H__
#define __PSRAM_HW_H__

//IO Mapping Define:
#define PSRAM_PORT_SEL(...)


//PORTA IO Define:
#define PSRAM_PORTA_CS 				IO_PORTE_03
#define PSRAM_PORTA_CLK 			IO_PORTE_04
#define PSRAM_PORTA_D0 				IO_PORTE_00
#define PSRAM_PORTA_D1 				IO_PORTE_02
#define PSRAM_PORTA_D2 				IO_PORTE_01
#define PSRAM_PORTA_D3 				IO_PORTE_05

#define PSRAM_MAPPING_SIZE 			(0x1000000)
#define PSRAM_CACHE_ADDR 			(0x2000000) //Peripheral & CPU(rwx)
#define PSRAM_NO_CACHE_ADDR 		(0x3000000) //Peripheral Only

#define     PSRAM_IO_DEBUG_0(i,x)       //{JL_PORT##i->DIR &= ~BIT(x), JL_PORT##i->OUT &= ~BIT(x);}
#define     PSRAM_IO_DEBUG_1(i,x)       //{JL_PORT##i->DIR &= ~BIT(x), JL_PORT##i->OUT |= BIT(x);}


#endif /* #ifndef __PSRAM_HW_H__ */
