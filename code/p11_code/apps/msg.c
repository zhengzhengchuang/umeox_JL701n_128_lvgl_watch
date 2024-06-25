#include "includes.h"
#include "ram_share.h"
#include "msg.h"
#include "sensor_api.h"
#include "app_config.h"
#include "circular_buf.h"
#include "sensor_api.h"
/*  */
/* #pragma bss_seg(".msg_bss") */
/* #pragma const_seg(".msg_const") */
/* #pragma code_seg(".msg_code") */
/*  */


#define P11_USER_POST_TO_MSYS(x)   (P11_SYSTEM->P2M_INT_SET |= BIT((x)))
#define P11_USER_P2M_ENABLE(x)  	(P11_SYSTEM->P2M_INT_IE |= BIT((x)))
#define P11_USER_P2M_DISABLE(x)  	(P11_SYSTEM->P2M_INT_IE &= ~BIT((x)))


#define MSYS_USER_POST_TO_P11(x)   (P11_SYSTEM->M2P_INT_SET |= BIT((x)))

#define USER_P2M_BASE_ADDR (P2M_USER_PEND)
#define USER_M2P_BASE_ADDR (M2P_USER_PEND)

#define USER_MSG_PEND_BASE  (0)
#define USER_MSG_TYPE_BASE  (1)
#define USER_MSG_LEN_BASE   (2)
#define USER_MSG_ADDR_BASE  (4)

static int send_ram[(256) / 4];

static cbuffer_t msg_cbuf;
static u32 msg_pool[64];




void task_clear_all_message(void)
{
    cbuf_clear(&msg_cbuf);
}

void task_message_init()
{
    cbuf_init(&msg_cbuf, msg_pool, sizeof(msg_pool));
    cbuf_clear(&msg_cbuf);
}

int task_post_msg(char *name, int argc, ...)
{
    u16 msg_value = 0x0fff;
    int i = 0;
    int  param_len = 0;
    int param = 0;
    ENTER_CRITICAL();
    va_list argptr;
    va_start(argptr, argc);
    for (i = 0; i < argc; ++i) {
        if (i == 0) {
            msg_value = 0x0fff & va_arg(argptr, int);
            if (msg_value == MSG_CLEAN_ALL_MSG) {
                cbuf_clear(&msg_cbuf);
                va_end(argptr);
                EXIT_CRITICAL();
                return 0;
            }
            param_len = argc - 1;
            msg_value = (param_len << 12) | msg_value;
            cbuf_write(&msg_cbuf, (void *)&msg_value, 2);

        } else {
            param = va_arg(argptr, int);
            cbuf_write(&msg_cbuf, (void *)&param, 4);
        }
    }
    va_end(argptr);
    EXIT_CRITICAL();
    return 0;
}

int task_get_msg(u16 timeout, int len, int *msg)
{
    u16 msg_value = 0x0fff;
    u8  param_len = 0;
    int i = 0;
    int param;
    u32 event, event_to_msg;
    //debug
    for (i = 0; i < len; i++) {
        msg[i] = 0x0fff;
    }


    ENTER_CRITICAL();
    if (2 != cbuf_read(&msg_cbuf, (void *)&msg_value, 2)) {
        /* memset(msg, NO_MSG, len); */
        EXIT_CRITICAL();
        return MSG_NO_ERROR;
    }

    msg[0] = msg_value & 0x0fff;
    param_len = msg_value >> 12;
    for (i = 0 ; i < param_len; i++) {
        cbuf_read(&msg_cbuf, (void *)&param, 4);
        if (i + 1 < len) {
            msg[i + 1] = param;
        }
    }
    if (i >= len) {
        puts("msg_buf_not_enc\n");
        EXIT_CRITICAL();
        return MSG_BUF_NOT_ENOUGH;
    }
    EXIT_CRITICAL();
    return 0;
}

__INLINE__
void mdebug_io_1(u8 i)
{
    P11_PORT->PB_SEL |= BIT(i); 	//P11 Control
    P11_PORT->PB_DIR &= ~BIT(i); 	//dir output
    P11_PORT->PB_OUT |= BIT(i); 	//output 1
}


__INLINE__
void mdebug_io_0(u8 i)
{
    P11_PORT->PB_SEL |= BIT(i); 	//P11 Control
    P11_PORT->PB_DIR &= ~BIT(i); 	//dir output
    P11_PORT->PB_OUT &= ~BIT(i); 	//output 0
}


/**
 * @brief 用户小核发送数据到主核
 *
 * @param [in] cmd 命令
 * @param [in] len 数据长度
 * * @param [in] data 数据地址
 * @param [in] wait 是否等待传输结束
 */
void user_p11_post_to_main_system(u8 cmd, u16 len, u8 *data, u8 wait)
{
    u8 *pend = (u8 *) & (p2m_message[USER_P2M_BASE_ADDR + USER_MSG_PEND_BASE]);
    int timer_out = 100000;
    ENTER_CRITICAL();
    //do {
    //    asm("csync");
    //    timer_out--;
    //} while (timer_out && *pend);

    if (!timer_out || *pend) {
        //*pend = 0;
        EXIT_CRITICAL();
        printf("timer_out %d\n", __LINE__);

        /* while (1); */
        return;
    }

    p2m_message[USER_P2M_BASE_ADDR + USER_MSG_PEND_BASE] = 1;
    EXIT_CRITICAL();
    p2m_message[USER_P2M_BASE_ADDR + USER_MSG_TYPE_BASE] = cmd;
    memcpy((u8 *)&p2m_message[USER_P2M_BASE_ADDR + USER_MSG_LEN_BASE], &len, sizeof(u16));
    memcpy((u8 *)&p2m_message[USER_P2M_BASE_ADDR + USER_MSG_ADDR_BASE], &data, sizeof(u8 *));
    /* printf("addr %x %x data%x pend %x len=%d\n",p2m_message,&p2m_message[USER_BASE_ADDR + USER_MSG_PEND_BASE],data,pend,len); */
    /* put_buf(data,len); */
    extern void __power_recover(void);
    __power_recover();
    P11_USER_POST_TO_MSYS(P2M_USER_INDEX);
    timer_out = 100000;
    do {
        asm("csync");
        timer_out --;
    } while (wait && timer_out && *pend);
    if (!timer_out) {
        printf("timer_out %d\n", __LINE__);
        while (1);
    }

}

/**
 * @brief 用户小核和主核数据交换区域初始化
 */
void user_p11_p2m_post_init()
{
    p2m_message[USER_P2M_BASE_ADDR + USER_MSG_PEND_BASE] = 0;
    m2p_message[USER_M2P_BASE_ADDR + USER_MSG_PEND_BASE] = 0;
    P11_USER_P2M_ENABLE(P2M_USER_INDEX);
    int msg[2];
    msg[0] = sizeof(send_ram);
    msg[1] = send_ram;
    user_p11_post_to_main_system(0, sizeof(msg), (u8 *)msg, 1); //配置主核共享空间
}

/**
 * @brief 用户小核模拟主核数据，可以激活小核实现一个类似信号量的功能
 */
int user_main_post_to_p11_system(u8 cmd, u8 wait)
{

    return task_post_msg(NULL, 1, cmd);
#if 0 //很难这里保证不了和大核的绝对安全,使用其他办法同步
    u8 *pnd = (u8 *) & (m2p_message[USER_M2P_BASE_ADDR + USER_MSG_PEND_BASE]);
    asm("csync");
    if (pnd[0]) {
        printf("post to self fail \n");
        return -1;
    }

    pnd[0] = 1;
    pnd[1] = cmd;
    pnd[2] = (u8)(0 & 0xff);
    pnd[3] = (u8)(0 >> 8) & 0xff;
    pnd[4] = (u8)(0) & 0xff;
    pnd[5] = (u8)(0 >> 8) & 0xff;
    pnd[6] = (u8)(0 >> 16) & 0xff;
    pnd[7] = (u8)(0 >> 24) & 0xff;
    MSYS_USER_POST_TO_P11(M2P_USER_INDEX);
    int timer_out = 10000;
    do {
        asm("csync");
        timer_out --;
    } while (wait && timer_out && pnd[0]);
    if (!timer_out) {
        printf("timer_out %d\n", __LINE__);
        while (1);
    }
    return 0;
#endif
}





void  user_m2p11_irq()
{
    u8 cmd;
    u8 type;
    u16 len;
    u8 *data;
    u8 *pnd = (u8 *)(m2p_message +  CONFIG_M2P_MESSAGE_SIZE - 8);
    cmd = pnd[1];
    len = pnd[2] | (pnd[3] << 8);
    data = (u8 *)((pnd[4] | (pnd[5] << 8) | (pnd[6] << 16) | (pnd[7] << 24)));

#if CONFIG_SENSOR_ALG_ENABLE
    switch (cmd) {
    case P11_SYS_KICK:
        pnd[0]  = 0;
        /* user_main_run(); */
        task_post_msg(NULL, 1, cmd);
        break;
    case P11_SYS_IIC_R_SYNC:
        //让p11 备进入主线程状态 准备
        task_post_msg(NULL, 1, P11_SYS_IIC_R_SYNC);
        break;
    case P11_SYS_IIC_W_SYNC:
        //让p11 备进入主线程状态 准备
        task_post_msg(NULL, 1, P11_SYS_IIC_W_SYNC);
        break;
    case P11_SYS_GSENSOR_START:
        //printf("P11_SYS_GSENSOR_START\n");
        task_post_msg(NULL, 1, cmd);
        /* sensor_hub_gsensor_enable(1); */
        break;
    case P11_SYS_GSENSOR_STOP:
        //printf("P11_SYS_GSENSOR_STOP\n");
        task_post_msg(NULL, 1, cmd);
        /* sensor_hub_gsensor_enable(0); */
        break;
    case P11_SYS_HRSENSOR_START:
        //printf("P11_SYS_HRSENSOR_START\n");
        task_post_msg(NULL, 1, cmd);
        /* sensor_hub_hrsensor_enable(1); */
        break;
    case P11_SYS_HRSENSOR_STOP:
        //printf("P11_SYS_HRSENSOR_STOP\n");
        task_post_msg(NULL, 1, cmd);
        /* sensor_hub_hrsensor_enable(0); */
        break;
    case P11_SYS_SPO2_START:
        //printf("P11_SYS_SPO2SENSOR_START\n");
        task_post_msg(NULL, 1, cmd);
        /* sensor_hub_spo2sensor_enable(1); */
        break;
    case P11_SYS_SPO2_STOP:
        //printf("P11_SYS_SPO2SENSOR_STOP\n");
        task_post_msg(NULL, 1, cmd);
        /* sensor_hub_spo2sensor_enable(0); */
        break;
    case P11_SYS_GSENSOR_SEARCH:
        printf("P11_SYS_GSENSOR_SEARCH\n");
        task_post_msg(NULL, 1, cmd);
        if (len) {	//出于实时性考虑在此处理
            int valid = 0;
            sensor_hub_gsensor_search(&valid);
            printf("%s valid%d\n", __func__, valid);
            memcpy(data, &valid, 4);
        }
        break;
    case P11_SYS_HRSENSOR_SEARCH:
        printf("P11_SYS_HRSENSOR_SEARCH\n");
        task_post_msg(NULL, 1, cmd);
        if (len) { //出于实时性考虑在此处理
            int valid = 0;
            sensor_hub_hrsensor_search(&valid);
            printf("%s valid%d\n", __func__, valid);
            memcpy(data, &valid, 4);
        }
        break;
    }
#endif
    //printf(">>>%s %d %x %x %x\n", __func__, __LINE__, cmd, len, data);
    asm("csync");
    //if (len) {
    //put_buf(data, len);
    //}
    pnd[0]  = 0;
}



