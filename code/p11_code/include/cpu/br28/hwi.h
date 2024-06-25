#ifndef __ASS_Q32S_HWI__
#define __ASS_Q32S_HWI__

//*********************************************************************************//
// Module name : hwi.h                                                             //
// Description : q32DSP interrupt head file                                        //
// By Designer : zequan_liu                                                        //
// Dat changed :                                                                   //
//*********************************************************************************//


#define IRQ_EXCEPTION_IDX   	0
#define IRQ_SOFT0_IDX       	1
#define IRQ_M2P_IDX   			2
#define IRQ_P33_IDX   			3
#define IRQ_WDT_IDX   			4
#define IRQ_SPI_IDX   			5
#define IRQ_UART0_IDX   		6
#define IRQ_UART1_IDX   		7
#define IRQ_IIC_IDX   			8
#define IRQ_LP_TMR0_IDX   		9
#define IRQ_LP_TMR1_IDX   		10
#define IRQ_LP_TMR2_IDX   		11
#define IRQ_LP_TMR3_IDX   		12
#define IRQ_GP_TMR0_IDX   		13
#define IRQ_GP_TMR1_IDX   		14
#define IRQ_LP_CTM_IDX   		15
#define IRQ_LP_VAD_IDX   		16

//---------------------------------------------//
// interrupt install
//---------------------------------------------//

void reg_set_ip(unsigned char index, unsigned char dat);
void bit_set_ie(unsigned char index);
void bit_clr_ie(unsigned char index);
void bit_set_swi(unsigned char index);
void bit_clr_swi(unsigned char index);

#define INTALL_HWI(a,b,c) HWI_Install(a, (unsigned int)b, c)
void HWI_Install(unsigned char index, unsigned int isr, unsigned char priority);

//---------------------------------------------//
// core_num
//---------------------------------------------//

static inline int core_num(void)
{
    u32 num;
    asm volatile("%0 = cnum" : "=r"(num) :);
    return num;
}

//---------------------------------------------//
// interrupt enable
//---------------------------------------------//

#define ENABLE_INT enable_int
#define enable_int()  local_irq_enable()

#define DISABLE_INT disable_int
#define disable_int() local_irq_disable()

#define ___interrupt 	__attribute__((interrupt("")))


void local_irq_disable();
void local_irq_enable();

void irq_disable_core();
void irq_enable_core();

//*********************************************************************************//
//                                                                                 //
//                               end of this module                                //
//                                                                                 //
//*********************************************************************************//
#endif
