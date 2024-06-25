#include "msg.h"
#include "circular_buf.h"
#include "common.h"
#include "printf.h"
#include "os/os_cpu.h"
#include "asm/wdt.h"

static cbuffer_t msg_cbuf;
static u32 msg_pool[MAX_POOL];

void sys_nop_delay(void)
{
    //__builtin_pi32v2_nop();
    __asm__ volatile("nop");
}

int task_get_msg(u16 timeout, int len, int *msg)
{
    int msg_value = 0;
    u8  param_len = 0;
    int i = 0;
    int param;
    u32 event, event_to_msg;
    //debug
    for (i = 0; i < len; i++) {
        msg[i] = 0xffff;
    }

    //get_msg
    wdt_clr();
    CPU_SR_ALLOC();
    OS_ENTER_CRITICAL();
#if USE_EVENT_EN
    event = get_event();
    if (event != NO_EVENT) {
        clear_one_event(event);
        event_to_msg = evnet2msg[event];
        msg[0] = event_to_msg;
        //printf("event_mag %d\n ", event_to_msg);
        OS_EXIT_CRITICAL();
        return MSG_NO_ERROR;
    }
#endif
    if (2 != cbuf_read(&msg_cbuf, (void *)&msg_value, 2)) {
        /* memset(msg, NO_MSG, len); */
        OS_EXIT_CRITICAL();

        /*get no msg,cpu enter idle*/
        sys_nop_delay();

        return MSG_NO_ERROR;
    }
    //msg[0] = msg_value;
    //param_len = msg_value >> 12;
    param_len = msg_value;
    for (i = 0 ; i < param_len; i++) {
        cbuf_read(&msg_cbuf, (void *)&param, 4);
        if (i < len) {
            msg[i] = param;
        }
    }
    if (i >= len) {
        puts("msg_buf_not_enc\n");
        OS_EXIT_CRITICAL();
        return MSG_BUF_NOT_ENOUGH;
    }
    OS_EXIT_CRITICAL();
    return MSG_NO_ERROR;
}

extern void stack_run_loop_resume();
int task_post_msg_base(const char *name, int argc, int cmd, int *argv)
{
    u16 msg_value = 0x0fff;
    int i = 0;
    int param_len = 0;
    int param = 0;

    if (0xff == cmd) {
        printf("cmd == 0xff\n");
        stack_run_loop_resume();
        return MSG_NO_ERROR;
    }

    CPU_SR_ALLOC();
    OS_ENTER_CRITICAL();
    //va_list argptr;
    //va_start(argptr, argc);
    /* printf("msg:cnt:%x\n", argc); */
    for (i = 0; i < argc + 1; ++i) {
        if (i == 0) {
            param_len = argc;
            msg_value = param_len & 0xffff;
            /* printf("msg[0]:%x\n",msg_value); */
            if (cbuf_write(&msg_cbuf, (void *)&msg_value, 2) != 2) {
                ASSERT(0, "stack message full! %d\n", __LINE__);
            }

        } else {
            //param = va_arg(argptr, int);
            if (i == 1) {
                param = cmd;
            } else {
                param = argv[i - 2];
            }
            /* printf("msg[%d]:%x\n", i, param); */
            if (cbuf_write(&msg_cbuf, (void *)&param, 4) != 4) {
                ASSERT(0, "stack message full! %d\n", __LINE__);
            }
        }
    }
    //va_end(argptr);
    OS_EXIT_CRITICAL();
    stack_run_loop_resume();
    return MSG_NO_ERROR;
}
int task_post_msg(char *name, int argc, ...)
{
    int argv[8];
    va_list argptr;

    ASSERT(argc <= 8);

    va_start(argptr, argc);

    u8 i;
    for (i = 0; i < argc; i++) {
        argv[i] = va_arg(argptr, int);
    }

    va_end(argptr);

    //puts("msg_push:\n");
    //put_buf((u8 *)argv,argc*4);
    return task_post_msg_base(name, argc, argv[0], &argv[1]);
}
void task_clear_all_message(void)
{
    cbuf_clear(&msg_cbuf);
}

void task_message_init(void)
{
    cbuf_init(&msg_cbuf, msg_pool, sizeof(msg_pool));
    cbuf_clear(&msg_cbuf);
}

