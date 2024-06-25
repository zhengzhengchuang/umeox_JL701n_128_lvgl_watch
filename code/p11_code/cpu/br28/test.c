#include "includes.h"

//====================================//
//            FOR TEST CODE           //
//====================================//

// test code:
#define SOFT_INT_INDEX  0

___interrupt
static void soft_int_test_isr(void)
{
    static u32 isr_cnt = 0;

    bit_clr_swi(SOFT_INT_INDEX);
    printf("%s: %d\n", __func__, isr_cnt++);
}

void soft_int_test(void)
{
    static u32 set_cnt = 0;
    INTALL_HWI(IRQ_SOFT0_IDX, soft_int_test_isr, IRQ_SOFT0_IP);

    while (1) {
        delay(1000000);
        printf("\n%s: %d\n", __func__, set_cnt++);
        bit_set_swi(SOFT_INT_INDEX);
    }
}

