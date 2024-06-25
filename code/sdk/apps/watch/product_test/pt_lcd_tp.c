#include "product_test.h"
#include "pt_lcd_tp.h"

#if PT_LCD_TP_ENABLE

int pt_lcd_tp_init(void)
{
    return 0;
}

int pt_lcd_tp_start(void)
{
    return 0;
}

int pt_lcd_tp_stop(void)
{
    return 0;
}

int pt_lcd_tp_ioctrl(u32 order, int len, void *param)
{
    u32 result = 0;
    switch (PT_ORDER_C_GET(order)) {
    case PT_N_C_START:
        result = pt_lcd_tp_start();
        break;
    case PT_N_C_STOP:
        result = pt_lcd_tp_stop();
        break;
    case PT_N_C_GET_RESULT:
        result = PT_E_PARAM;
        break;
    default:
        result = PT_E_PARAM;
        break;
    }
    product_test_push_data(order, 4, &result);
    return result;
}


REGISTER_PT_MODULE(lcd_tp) = {
    .module = PT_M_LCD_TP,
    .attr	= PT_ATTR_USER_JUDGE | PT_ATTR_INTERACTION,
    .init	= pt_lcd_tp_init,
    .ioctrl	= pt_lcd_tp_ioctrl,
};

#endif /* #if PT_LCD_TP_ENABLE*/

