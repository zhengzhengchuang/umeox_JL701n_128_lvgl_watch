#include "product_test.h"
#include "custom_cfg.h"
#include "ui/ui_api.h"
#include "tone_player.h"
#include "system/timer.h"

#if PRODUCT_TEST_ENABLE

#define PT_TEST			0

#define PT_TASK_NAME 			"pt"

enum {
    PT_TASK_MSG_RUN,
    PT_UART_RX_DAT,
};


// 消息链表
struct pt_mod_msg {
    struct list_head entry;
    struct pt_pkt *pkt;
};

// 模块状态
struct pt_mod_stat {
    u32 init_ok : 1;	// 初始化ok。// true or false
    struct pt_module *opt;
};

static struct pt_mod_stat *pt_mod = NULL;
static LIST_HEAD(pt_msg_head);

static u16 pt_pkt_data_len_max = 512; // 还未实现拼包
struct playload_packet {
    u16 magic;
    u16 crc;
    u16 data_len;
    u16 seq;
    u16 op;
    u8 data[0];
};
#define PT_CRC_EXTRA_LEN              sizeof(struct playload_packet) - sizeof(playload_packet->magic) - sizeof(playload_packet->crc)
struct playload_packet *playload_packet;
static const uart_bus_t *uart_bus;

#if PT_TEST

#define PT_TEST_STEP_SYS_START			0
#define PT_TEST_STEP_IDLE				1
#define PT_TEST_STEP_START				2
#define PT_TEST_STEP_CHECK				3
#define PT_TEST_STEP_STOP				4
#define PT_TEST_STEP_STOP_ERR			5
#define PT_TEST_STEP_SYS_STOP			6

#define PT_TEST_PKT_LEN(x)				(sizeof(struct playload_packet) + sizeof(struct pt_pkt) + (x))
#define PT_TEST_PKT_CRC_LEN(x)			(sizeof(struct pt_pkt) + (x))

struct pt_test_mod {
    u16 mod;
    u16 to_100ms;
};

static const struct pt_test_mod pt_test_mod_tab[] = {
#if PT_GPIO_ENABLE
    {PT_M_GPIO, 50},
#endif /* #if PT_GPIO_ENABLE */
#if PT_SD_ENABLE
    {PT_M_SD, 50},
#endif /* #if PT_SD_ENABLE */
#if PT_HR_ENABLE
    {PT_M_HR, 50},
#endif /* #if PT_HR_ENABLE */
#if PT_GSENSOR_ENABLE
    {PT_M_GSENSOR, 50},
#endif /* #if PT_GSENSOR_ENABLE */
#if PT_SPEAKER_MIC_ENABLE
    {PT_M_SPEAKER_MIC, 500},
#endif /* #if PT_SPEAKER_MIC_ENABLE */
#if PT_MOTOR_ENABLE
    {PT_M_MOTOR, 200},
#endif /* #if PT_MOTOR_ENABLE */
};

static u8 pt_test_idx = 0;
static u8 pt_test_step = 0;
static u16 pt_test_to = 0;

void uart_isr_hook(void *arg, u32 status);

static void pt_test_deal(void *priv)
{
    uart_isr_hook(NULL, UT_RX);
}


static int pt_test_pkt_enc(u32 order, u8 *inbuf, u32 inbuf_len, u8 *dat, u8 dat_len)
{
    static u16 seq = 0;
    u32 cmd_size = 0;
    cmd_size = PT_TEST_PKT_LEN(dat_len);
    printf("enc ord:0x%x, in:0x%x, l:%d, dat:0x%x, l:%d \n", order, inbuf, inbuf_len, dat, dat_len);
    printf("cmd_size :%d \n", cmd_size);
    if (cmd_size > inbuf_len) {
        printf("cmdsize err :%d,%d \n", cmd_size, inbuf_len);
        return 0;
    }
    memset(inbuf, 0, cmd_size);

    struct playload_packet pld_pkt = {0};
    pld_pkt.magic = 0xaa55;
    pld_pkt.seq = seq++;
    pld_pkt.data_len = PT_TEST_PKT_CRC_LEN(dat_len);

    struct pt_pkt pkt = {0};
    pkt.order = order;
    pkt.data_len = dat_len;
    memcpy(&inbuf[sizeof(struct playload_packet)], &pkt, sizeof(struct pt_pkt));

    if (dat_len) {
        memcpy(&inbuf[PT_TEST_PKT_LEN(0)], dat, dat_len);
    }
    memcpy(inbuf, &pld_pkt, sizeof(struct playload_packet));
    pld_pkt.crc = CRC16(&inbuf[list_offsetof(struct playload_packet, data_len)], pld_pkt.data_len + PT_CRC_EXTRA_LEN);
    memcpy(inbuf, &pld_pkt, sizeof(struct playload_packet));

    /* printf("data_len offset:0x%x \n", &inbuf[list_offsetof(struct playload_packet, data_len)]); */
    /* printf("data_len :%d,%d \n", pld_pkt.data_len, pld_pkt.data_len + PT_CRC_EXTRA_LEN); */
    /* printf("crc:0x%x \n", pld_pkt.crc); */
    /* put_buf(inbuf, cmd_size); */

    return cmd_size;
}

static void pt_test_dat_write(const u8 *outbuf, u32 len)
{
    int result;
    u8 *dat = &outbuf[PT_TEST_PKT_LEN(0)];
    struct pt_pkt pkt = {0};
    memcpy(&pkt, &outbuf[sizeof(struct playload_packet)], sizeof(struct pt_pkt));
    u16 mod = PT_ORDER_M_GET(pkt.order);
    u16 cmd = PT_ORDER_C_GET(pkt.order);
    printf("push mod:0x%x, cmd :0x%x \n", mod, cmd);
    if (mod == PT_M_SYSTEM) {
        if (cmd == PT_S_C_GET_VER) {
            if (ARRAY_SIZE(pt_test_mod_tab)) {
                pt_test_step = PT_TEST_STEP_IDLE;
                pt_test_idx = 0;
            } else {
                pt_test_step = PT_TEST_STEP_SYS_STOP;
            }
            sys_s_hi_timerout_add(NULL, pt_test_deal, 100);
        } else if (cmd == PT_S_C_END) {
        } else {
        }
    } else {
        memcpy(&result, dat, 4);
        switch (pt_test_step) {
        case PT_TEST_STEP_START:
            if (result) {
                printf("PT_TEST_STEP_START err:0x%x \n", result);
                break;
            }
            pt_test_to = pt_test_mod_tab[pt_test_idx].to_100ms;
            pt_test_step = PT_TEST_STEP_CHECK;
            sys_s_hi_timerout_add(NULL, pt_test_deal, 100);
            break;
        case PT_TEST_STEP_CHECK:
            if (result == PT_E_OK) {
                pt_test_step = PT_TEST_STEP_STOP;
                sys_s_hi_timerout_add(NULL, pt_test_deal, 100);
            } else if (result == PT_E_MOD_RUN) {
                if (pt_test_to) {
                    pt_test_to--;
                    sys_s_hi_timerout_add(NULL, pt_test_deal, 100);
                } else {
                    printf("pt_test timeout \n");
                    pt_test_step = PT_TEST_STEP_STOP_ERR;
                }
            } else {
                printf("PT_TEST_STEP_CHECK err:0x%x \n", result);
                break;
            }
            break;
        case PT_TEST_STEP_STOP:
            if (result) {
                printf("PT_TEST_STEP_STOP err:0x%x \n", result);
                break;
            }
            if (pt_test_idx >= ARRAY_SIZE(pt_test_mod_tab)) {
                pt_test_step = PT_TEST_STEP_SYS_STOP;
                sys_s_hi_timerout_add(NULL, pt_test_deal, 100);
                break;
            }
            pt_test_idx ++;
            pt_test_step = PT_TEST_STEP_IDLE;
            sys_s_hi_timerout_add(NULL, pt_test_deal, 100);
            break;
        default:
            break;
        }
    }
}
static u32 pt_test_dat_read(u8 *inbuf, u32 len, u32 timeout)
{
    u32 cmd_size = 0;
    u32 result = 0;
    switch (pt_test_step) {
    case PT_TEST_STEP_SYS_START:
        cmd_size = pt_test_pkt_enc(PT_ORDER_M_SET(PT_M_SYSTEM) | PT_ORDER_C_SET(PT_S_C_GET_VER),
                                   inbuf, len, NULL, 0);
        break;
    case PT_TEST_STEP_SYS_STOP:
        result |= BIT(0);
        /* result |= BIT(1); */
        cmd_size = pt_test_pkt_enc(PT_ORDER_M_SET(PT_M_SYSTEM) | PT_ORDER_C_SET(PT_S_C_END),
                                   inbuf, len, &result, 4);
        break;

    case PT_TEST_STEP_IDLE:
        cmd_size = pt_test_pkt_enc(PT_ORDER_M_SET(pt_test_mod_tab[pt_test_idx].mod) | PT_ORDER_C_SET(PT_N_C_START),
                                   inbuf, len, NULL, 0);
        pt_test_step = PT_TEST_STEP_START;
        break;
    case PT_TEST_STEP_CHECK:
        cmd_size = pt_test_pkt_enc(PT_ORDER_M_SET(pt_test_mod_tab[pt_test_idx].mod) | PT_ORDER_C_SET(PT_N_C_GET_RESULT),
                                   inbuf, len, NULL, 0);
        break;
    case PT_TEST_STEP_STOP:
        cmd_size = pt_test_pkt_enc(PT_ORDER_M_SET(pt_test_mod_tab[pt_test_idx].mod) | PT_ORDER_C_SET(PT_N_C_STOP),
                                   inbuf, len, NULL, 0);
        break;

    default:
        break;
    }
    return cmd_size;
}

static void pt_test_start(void)
{
    // y_printf("\n\n\n pt_test_start \n\n\n");
    sys_s_hi_timerout_add(NULL, pt_test_deal, 2000);
}
#endif /* #if PT_TEST */


int product_test_pkt_tx(void *dat, int len)
{
    memcpy(playload_packet->data, dat, len);
    playload_packet->data_len = len;
    playload_packet->crc = CRC16(&playload_packet->data_len, playload_packet->data_len + PT_CRC_EXTRA_LEN);
#if PT_TEST
    pt_test_dat_write(playload_packet, sizeof(struct playload_packet) + len);
#else /* #if PT_TEST */
    uart_bus->write(playload_packet, sizeof(struct playload_packet) + len);
#endif /* #if PT_TEST */
    printf("pt_tx_data:\n");
    put_buf(playload_packet, sizeof(struct playload_packet) + len);
    return len;
}

void product_test_pkt_rx(void *dat, int len)
{
    struct pt_mod_msg *msg = zalloc(sizeof(struct pt_mod_msg) + len);
    ASSERT(msg);
    msg->pkt = (struct pt_pkt *)((int)msg + sizeof(struct pt_mod_msg));
    memcpy(msg->pkt, dat, len);
    local_irq_disable();
    list_add_tail(&msg->entry, &pt_msg_head);
    local_irq_enable();

    os_taskq_post_msg(PT_TASK_NAME, 1, PT_TASK_MSG_RUN);
}

int product_test_push_data(u32 order, int len, u8 *dat)
{
    ASSERT(len < pt_pkt_data_len_max); // 还未实现拼包

    int pkt_len = sizeof(struct pt_pkt) + len;
    struct pt_pkt *pkt = zalloc(pkt_len);
    ASSERT(pkt);

    pkt->order = order;
    pkt->data_len = len;
    memcpy(pkt->dat, dat, len);

    int tlen = product_test_pkt_tx(pkt, pkt_len);
    if (tlen != pkt_len) {
        y_printf("pt push err:%d,%d \n", tlen, pkt_len);
        return PT_E_NO_BUFF;
    }
    return 0;
}


struct pt_mod_stat *product_test_find_module(u8 module)
{
    if (!pt_mod) {
        return NULL;
    }
    int i = 0;
    struct pt_module *mod;
    list_for_each_pt_module(mod) {
        if (mod->module == module) {
            return &pt_mod[i];
        }
        i++;
    }
    return NULL;
}

static void product_test_module_init(void)
{
    int i = 0;
    struct pt_module *mod;
    list_for_each_pt_module(mod) {
        pt_mod[i].opt = mod;
        pt_mod[i].init_ok = true;
        if (mod->init) {
            int ret = mod->init();
            if (ret) { // 模块初始化出错
                pt_mod[i].init_ok = false;
            }
        }
        i++;
    }
}

extern void sys_enter_soft_poweroff(void *priv);
static void pt_test_end(void *priv)
{
    //DP、DM设成高阻，关闭串口
    uart_dev_close(&u_arg, uart_bus);
    gpio_set_direction(IO_PORT_DP, 1);
    gpio_set_die(IO_PORT_DP, 0);
    gpio_set_pull_up(IO_PORT_DP, 0);
    gpio_set_pull_down(IO_PORT_DP, 0);

    gpio_set_direction(IO_PORT_DM, 1);
    gpio_set_die(IO_PORT_DM, 0);
    gpio_set_pull_up(IO_PORT_DM, 0);
    gpio_set_pull_down(IO_PORT_DM, 0);
    adc_pmu_detect_en(1);
    volatage_trim_init();
    sys_enter_soft_poweroff(NULL);
}

static int pt_system_ioctrl(u32 order, int len, void *param)
{
    u32 result = 0;
    switch (PT_ORDER_C_GET(order)) {
    case PT_S_C_GET_VER: {
        u8 buf[8];
        u16 vid = get_vid_pid_ver_from_cfg_file(GET_VID_FROM_EX_CFG);
        u16 pid = get_vid_pid_ver_from_cfg_file(GET_PID_FROM_EX_CFG);
        u16 ver = get_vid_pid_ver_from_cfg_file(GET_VER_FROM_EX_CFG);
        memcpy(&buf[0], &vid, 2);
        memcpy(&buf[2], &pid, 2);
        memcpy(&buf[4], &ver, 2);
        memcpy(&buf[6], &pt_pkt_data_len_max, 2);
        product_test_push_data(order, sizeof(buf), buf);
        return 0;
    }
    break;
    case PT_S_C_SET_INFO:
        memcpy(pt_pkt_data_len_max, param, 2);
        break;
    case PT_S_C_GET_MODULE: {
        u8 mod_num = GET_PT_MODULE_NUM();
        u8 *buf = zalloc(1 + mod_num + mod_num * 2);
        ASSERT(buf);

        buf[0] = mod_num;
        int i = 0;
        struct pt_module *mod;
        list_for_each_pt_module(mod) {
            buf[1 + i] = mod->module;
            memcpy(buf[1 + mod_num + i * 2], mod->attr, 2);
            i++;
        }

        product_test_push_data(order, 1 + mod_num + mod_num * 2, buf);
        free(buf);
        return 0;
    }
    break;
    case PT_S_C_HOLD:
        /* product_test_push_data(order, 0, NULL); */
        return 0;
        break;
    case PT_S_C_END: {
        u32 info;
        u8 pt_result = 0xff;
        memcpy(&info, param, 4);
        if (info & BIT(0)) {
            sys_timeout_add(NULL, pt_test_end, 10);
        }
        if (info & BIT(1)) {
            pt_result = 1;
        } else {
            pt_result = 0;
        }
        syscfg_write(PT_TEST_RESULT, (u8 *)&pt_result, sizeof(pt_result));

        /* return 0; */
        break;
    }
    break;

    default:
        result = PT_E_PARAM;
        break;
    }
    product_test_push_data(order, 4, &result);
    return result;
}
void uart_isr_hook(void *arg, u32 status)
{
    if (status != UT_TX) {
        os_taskq_post_msg(PT_TASK_NAME, 1, PT_UART_RX_DAT);
    }
}


static u8 *uart_cbuf = NULL;
static u8 product_test_mode = 0;
#define BAUD_RATE   1000000
void pt_uart_init()
{
    u32 uart_max_dma_size = pt_pkt_data_len_max + sizeof(playload_packet);
    if (uart_max_dma_size & (uart_max_dma_size - 1)) {
        u32 index = 0;
        for (u32 i = 32; i > 1; i--) {
            if (uart_max_dma_size & (1 << i)) {
                index = i;
                break;
            }
        }
        uart_max_dma_size = 1 << (index + 1);
    }
    printf("uart_max_dma_size = %d  index = %d\n", uart_max_dma_size, index);
    uart_cbuf = zalloc(uart_max_dma_size);
    struct uart_platform_data_t u_arg = {0};
    u_arg.tx_pin = PT_IO_TX_PIN;
    u_arg.rx_pin = PT_IO_RX_PIN;
    u_arg.rx_cbuf = uart_cbuf;
    u_arg.rx_cbuf_size = uart_max_dma_size;
    u_arg.frame_length = pt_pkt_data_len_max;
    u_arg.rx_timeout = 1;
    u_arg.isr_cbfun = uart_isr_hook;
    u_arg.baud = BAUD_RATE;
    u_arg.is_9bit = 0;

    uart_bus = uart_dev_open(&u_arg);
    if (uart_bus != NULL) {
        printf("uart_dev_open() success\n");
    }
}
void uart_rx_data_process()
{
    memset(playload_packet, 0, sizeof(struct playload_packet));
#if PT_TEST
    u32 rx_len = pt_test_dat_read(playload_packet, sizeof(struct playload_packet) + pt_pkt_data_len_max, 1);
#else /* #if PT_TEST */
    u32 rx_len = uart_bus->read(playload_packet, sizeof(struct playload_packet) + pt_pkt_data_len_max, 1);
#endif /* #if PT_TEST */
    printf("pt_rx_data:\n");
    put_buf(playload_packet, rx_len);
    if (playload_packet->magic != 0xaa55) {
        return;
    }
    if (playload_packet->op != 0x0) {
        return;
    }
    /* printf("pk:0x%x, loffset:0x%x \n", playload_packet, &playload_packet->data_len); */
    /* printf("data_len :%d,%d \n", playload_packet->data_len, playload_packet->data_len + PT_CRC_EXTRA_LEN); */
    /* printf("crc:0x%x, 0x%x \n", playload_packet->crc, CRC16(&playload_packet->data_len, playload_packet->data_len + PT_CRC_EXTRA_LEN)); */

    if (playload_packet->crc == CRC16(&playload_packet->data_len, playload_packet->data_len + PT_CRC_EXTRA_LEN)) {
        if (product_test_mode == 0) {
            tone_play_stop();
#if PT_GPIO_CHECK_LCD_TP
            ui_set_shut_down_time_ms(10);
            ui_auto_shut_down_modify();
#endif /* #if PT_GPIO_CHECK_LCD_TP */
        }
        product_test_mode = 1;
        product_test_pkt_rx(playload_packet->data, playload_packet->data_len);
    }
}
static void product_test_main(void *priv)
{
    int msg[32];
    int ret;

    if (GET_PT_MODULE_NUM()) {
        pt_mod = zalloc(GET_PT_MODULE_NUM() * sizeof(struct pt_mod_stat));
        ASSERT(pt_mod);
    }

    product_test_module_init();

    while (1) {
        ret = os_taskq_pend(NULL, msg, ARRAY_SIZE(msg));
        if (ret != OS_TASKQ) {
            continue;
        }
        if (msg[1] == PT_TASK_MSG_RUN) {
            struct pt_mod_msg *p, *n;
            list_for_each_entry_safe(p, n, &pt_msg_head, entry) {//用于安全正序遍历所有入口
                struct pt_pkt *pkt;
                u8 module;
                u32 result = 0;

                pkt = p->pkt;

                ASSERT(pkt->pkt_log == PT_PKT_ONCE); // 还未实现拼包

                module = PT_ORDER_M_GET(pkt->order);//获取命令
                if (module == PT_M_SYSTEM) {
                    pt_system_ioctrl(pkt->order, pkt->data_len, pkt->dat);
                } else {
                    do {
                        struct pt_mod_stat *stat = product_test_find_module(module);
                        if (stat == NULL) {
                            result = PT_E_MOD_NULL;
                        } else if (stat->init_ok == false) {
                            result = PT_E_MOD_ERROR;
                        } else {
                            if (stat->opt->ioctrl) {
                                stat->opt->ioctrl(pkt->order, pkt->data_len, pkt->dat);
                                break; // ioctrl回调中push
                            }
                            result = PT_E_PARAM;
                        }
                        product_test_push_data(pkt->order, 4, &result);
                    } while (0);
                }
                local_irq_disable();
                list_del(&p->entry);
                free(p);
                local_irq_enable();
            }
        } else if (msg[1] == PT_UART_RX_DAT) {
            uart_rx_data_process();
        }
    }
}

void pt_check_ot()
{
    if (!product_test_mode) {
        uart_dev_close(&u_arg, uart_bus);
        free(playload_packet);
        free(uart_cbuf);
        gpio_set_direction(IO_PORT_DP, 1);
        gpio_set_die(IO_PORT_DP, 0);
        gpio_set_pull_up(IO_PORT_DP, 0);
        gpio_set_pull_down(IO_PORT_DP, 0);

        gpio_set_direction(IO_PORT_DM, 1);
        gpio_set_die(IO_PORT_DM, 0);
        gpio_set_pull_up(IO_PORT_DM, 0);
        gpio_set_pull_down(IO_PORT_DM, 0);
        task_kill(PT_TASK_NAME);
    }

}
// open
int product_test_open(void)
{
    u32 por_reset = is_reset_source(P33_VDDIO_LVD_RST) | is_reset_source(P33_VDDIO_POR_RST);
    if (!por_reset) {
        return 0;
    }
    pt_uart_init();
    playload_packet = (struct playload_packet *)malloc(sizeof(struct playload_packet) + pt_pkt_data_len_max);
    sys_timeout_add(NULL, pt_check_ot, PT_CHECK_OT);
    int err = 0;
    err = task_create(product_test_main, NULL, PT_TASK_NAME);
#if PT_TEST
    pt_test_start();
#endif /* #if PT_TEST */
    return err;
}

int product_test_check_run(void)
{
    if (product_test_mode) {
        return true;
    }
    return false;
}

static u8 pt_idle_query(void)
{
    return !product_test_mode;
}

REGISTER_LP_TARGET(pt_lp_target) = {
    .name = "product_test",
    .is_idle = pt_idle_query,
};


#else  /* #if PRODUCT_TEST_ENABLE */

int product_test_push_data(u32 order, int len, u8 *dat)
{
    int ret = *((int *)dat);
    printf("product_test_push_data:0x%x \n", ret);
    return 0;
}
#endif /* #if PRODUCT_TEST_ENABLE */


