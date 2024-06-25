#ifndef HWI_H
#define HWI_H

#include "common.h"

//=================================================
#define IRQ_EMUEXCPT_IDX   		0		//0
#define IRQ_EXCEPTION_IDX  		1		//0
#define IRQ_SYSCALL_IDX    		2		//0
#define IRQ_TICK_TMR_IDX   		3		//0
#define IRQ_TIME0_IDX      		4   	//0
#define IRQ_TIME1_IDX      		5   	//0
#define IRQ_TIME2_IDX      		6   	//0
#define IRQ_TIME3_IDX      		7   	//0
#define IRQ_TIME4_IDX      		8   	//0
#define IRQ_TIME5_IDX      		9   	//0

#define IRQ_UART0_IDX      		12		//0
#define IRQ_UART1_IDX      		13		//0
#define IRQ_UART2_IDX      		14		//0

#define IRQ_SPI0_IDX       		16		//0
#define IRQ_SPI1_IDX       		17		//0
#define IRQ_SPI2_IDX       		18		//0

#define IRQ_SD0_IDX        		20		//0

#define IRQ_IIC_IDX        		24		//0

#define IRQ_USB_SOF_IDX    		28   //1
#define IRQ_USB_CTRL_IDX   		29  	//1

#define IRQ_P2M_IDX        		32  	//0
#define IRQ_LP_TIMER0_IDX  		33
#define IRQ_LP_TIMER1_IDX  		34

#define IRQ_PORT_IDX		   	38		//0
#define IRQ_GPADC_IDX		   	39		//0
#define IRQ_CTM_IDX			   	40		//1
#define IRQ_PWM_LED_IDX		   	41		//1
#define IRQ_OSA_IDX			   	42
#define IRQ_LRCT_IDX		   	43       //1
#define IRQ_GPC_IDX			   	44		//1
#define IRQ_FMTX_IDX		   	45		//1
#define IRQ_TDM_IDX			   	46		//1
#define IRQ_RDEC0_IDX		   	47		//1
#define IRQ_SBC_IDX			   	48		//1
#define IRQ_AES_IDX        		49

#define IRQ_ANC_IDX             67
#define IRQ_AUDIO_IDX      		68		//1
#define IRQ_ALINK0_IDX     		69  	//1
#define IRQ_PDM_LINK_IDX   		70		//1
#define IRQ_VAD_IDX        		71		//1

#define IRQ_BT_CLKN_IDX    		76		//2
#define IRQ_BT_DBG_IDX     		77		//1
#define IRQ_BLE_RX_IDX     		78		//2
#define IRQ_BLE_EVENT_IDX  		79		//1

#define IRQ_BT_TIMEBASE_IDX  	80		//1
#define IRQ_WL_LOFC_IDX    		81		//2
#define IRQ_BREDR_IDX      		82		//2

#define IRQ_SYNC_IDX      		88		//2
#define IRQ_SRC_SYNC_IDX        89		//1
#define IRQ_SRC_IDX       		90		//1
#define IRQ_EQ_IDX         		91		//1
#define IRQ_DMA_COPY_IDX        92		//1
#define IRQ_DCP_IDX	       		93		//1

#define IRQ_IMD_IDX	       		104		//1
#define IRQ_IMB_IDX	       		105		//1
#define IRQ_JPG_IDX	       		106		//1

#define IRQ_SOFT0_IDX      		124
#define IRQ_SOFT1_IDX      		125
#define IRQ_SOFT2_IDX      		126
#define IRQ_SOFT3_IDX      		127

extern u32 _IRQ_MEM_ADDR[];

#define IRQ_MEM_ADDR    (_IRQ_MEM_ADDR)

#define MAX_IRQ_ENTRY_NUM   64

void interrupt_init();

void local_irq_enable();
void local_irq_disable();

//---------------------------------------------//
// interrupt cli/sti
//---------------------------------------------//

static inline int int_cli(void)
{
    int msg;
    asm volatile("cli %0" : "=r"(msg) :);
    return msg;
}

static inline void int_sti(int msg)
{
    asm volatile("sti %0" :: "r"(msg));
}

// #define INSTALL_HWI(a,b,c)\
//         HWI_Install(a,b,c)

void HWI_Install(unsigned int index, unsigned int isr, unsigned int priority);

void bit_clr_ie(unsigned char index);
void bit_set_ie(unsigned char index);

void irq_init();
void request_irq(u8 index, u8 priority, void (*handler)(void), u8 cpu_id);
void unrequest_irq(u8 index);
void reg_set_ip(unsigned char index, unsigned char priority, u8 cpu_id);

void ENABLE_INT(void) ;
#define enable_int ENABLE_INT
void DISABLE_INT(void) ;
#define disable_int DISABLE_INT
#ifdef IRQ_TIME_COUNT_EN
void irq_handler_enter(int irq);

void irq_handler_exit(int irq);

void irq_handler_times_dump();


#else

#define irq_handler_enter(irq)      do { }while(0)
#define irq_handler_exit(irq)       do { }while(0)
#define irq_handler_times_dump()    do { }while(0)

#endif

#define __asm_csync() \
    do { \
		asm volatile("csync;"); \
    } while (0)

#endif

