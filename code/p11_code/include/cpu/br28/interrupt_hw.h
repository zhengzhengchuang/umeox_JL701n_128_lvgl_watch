#ifndef __INTERRUPT_HW__
#define __INTERRUPT_HW__


/*
wkup:优先级3
   |
  \|/
ctmu:优先级2
   |
  \|/
pmu:优先级1
   |
  \|/
other:优先级0
*/

#define IRQ_EXCEPTION_IP   	    0
#define IRQ_M2P_IP   			1
#define IRQ_P33_IP   			3
#define IRQ_SOFT0_IP       		0
#define IRQ_WDT_IP   			0
#define IRQ_SPI_IP   			0
#define IRQ_UART0_IP   			0
#define IRQ_UART1_IP   			0
#define IRQ_IIC_IP   			0
#define IRQ_LP_TMR0_IP   		1
#define IRQ_LP_TMR1_IP   		0
#define IRQ_LP_TMR2_IP   		0
#define IRQ_LP_TMR3_IP   		0
#define IRQ_GP_TMR0_IP   		0
#define IRQ_GP_TMR1_IP   		0
#define IRQ_LP_CTM_IP   	    2
#define IRQ_LP_VAD_IP   		0




#endif
