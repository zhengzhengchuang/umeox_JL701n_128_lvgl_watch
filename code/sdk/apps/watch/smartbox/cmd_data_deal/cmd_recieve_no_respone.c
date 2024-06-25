#include "smartbox/config.h"
#include "smartbox/event.h"
#include "smartbox_extra_flash_cmd.h"
#include "sensor_log_notify.h"

///>>>>>>>>>>>>>收到APP发来不需要响应的命令处理

/* #define RCSP_DEBUG_EN */
#ifdef RCSP_DEBUG_EN
#define rcsp_putchar(x)                putchar(x)
#define rcsp_printf                    printf
#define rcsp_printf_buf(x,len)         put_buf(x,len)
#else
#define rcsp_putchar(...)
#define rcsp_printf(...)
#define rcsp_printf_buf(...)
#endif

#if (SMART_BOX_EN)

/* #define RCSP_DEBUG_EN */
#ifdef RCSP_DEBUG_EN
#define rcsp_putchar(x)                putchar(x)
#define rcsp_printf                    printf
#define rcsp_printf_buf(x,len)         put_buf(x,len)
#else
#define rcsp_putchar(...)
#define rcsp_printf(...)
#define rcsp_printf_buf(...)
#endif

void cmd_recieve_no_respone(void *priv, u8 OpCode, u8 *data, u16 len)
{
    rcsp_printf("cmd_recieve_no_respone, %x\n", OpCode);
    switch (OpCode) {
    default:
#if JL_SMART_BOX_EXTRA_FLASH_OPT
        if (0 == JL_smartbox_extra_flash_cmd_no_resp(priv, OpCode, data, len)) {
            break;
        }
#endif
        if (0 == JL_smartbox_sensors_log_notify(priv, OpCode, data, len)) {
            break;
        }
    }
}
#endif//SMART_BOX_EN


