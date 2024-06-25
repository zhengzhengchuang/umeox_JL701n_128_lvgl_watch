#ifdef SUPPORT_MS_EXTENSIONS
#pragma bss_seg(".vad_api.bss")
#pragma data_seg(".vad_api.data")
#pragma const_seg(".vad_api.const")
#pragma code_seg(".vad_api.text")
#pragma str_literal_override(".vad_api.text")
#endif /* #ifdef SUPPORT_MS_EXTENSIONS */


#include "includes.h"
#include "gp_timer_api.h"
#include "app_config.h"
#include "gpio.h"
#include "vad_hw.h"

#if CONFIG_VAD_ENABLE

#define P11_VAD_POST_TO_MSYS()  (P11_SYSTEM->P2M_INT_SET |= BIT(P2M_VAD_INDEX))
#define P11_VAD_P2M_ENABLE()  	(P11_SYSTEM->P2M_INT_IE |= BIT(P2M_VAD_INDEX))
#define P11_VAD_P2M_DISABLE()  	(P11_SYSTEM->P2M_INT_IE &= ~BIT(P2M_VAD_INDEX))

enum P11_VAD_CMD_TABLE {
    P11_VAD_CMD_START = 0xAA,
    P11_VAD_CMD_FRAME,
    P11_VAD_CMD_STOP,
};

enum M2P_VAD_CMD_TABLE {
    M2P_VAD_CMD_INIT = 0x55,
    M2P_VAD_CMD_FRAME,
    M2P_VAD_CMD_CLOSE,
    M2P_VAD_CMD_TEST,
};

enum VAD_STATUS_TABLE {
    VAD_STATUS_CLOSE = 0,
    VAD_STATUS_IDLE,
    VAD_STATUS_ACTIVE,
};

static struct vad_handle {
    u8 status;
};

static struct vad_handle vad_hdl = {0};
#define __this 			(&vad_hdl)

static void p11_vad_post_to_main_system(enum P11_VAD_CMD_TABLE cmd)
{
    p2m_message[P2M_MESSAGE_VAD_CMD] = cmd;

    P11_VAD_POST_TO_MSYS();
}

void p11_vad_wakeup(void)
{
    __power_recover();

    __this->status = VAD_STATUS_ACTIVE;
}

void p11_dvad_start(void)
{
    printf("%s\n", __func__);

    __power_recover();

    __this->status = VAD_STATUS_ACTIVE;

    p11_vad_post_to_main_system(P11_VAD_CMD_START);
}


void p11_vad_update_frame(u8 *buf, u32 len)
{
    static u8 cnt = 0;

    p11_vad_post_to_main_system(P11_VAD_CMD_FRAME);
}

void p11_dvad_pause(void)
{
    p11_vad_post_to_main_system(P11_VAD_CMD_STOP);
}

void p11_dvad_stop(void)
{
    printf("%s\n", __func__);
    __this->status = VAD_STATUS_IDLE;
    p11_vad_post_to_main_system(P11_VAD_CMD_STOP);
}

/* static struct avad_config avad_cfg; */
/* static struct dvad_config dvad_cfg; */

__NO_INIT__ static int __avad_cfg[CONFIG_P2M_AVAD_CONFIG_SIZE / sizeof(int)] SEC_USED(.m2p_avad_config);
__NO_INIT__ static int __dvad_cfg[CONFIG_P2M_DVAD_CONFIG_SIZE / sizeof(int)] SEC_USED(.m2p_dvad_config);

static void p11_vad_init(void)
{
    printf("%s\n", __func__);
    P11_VAD_P2M_ENABLE();

    //TODO: init VAD HARDWARE
    /************************avad config set********************/
    struct avad_config *avad_cfg = (struct avad_config *)__avad_cfg;
    struct dvad_config *dvad_cfg = (struct dvad_config *)__dvad_cfg;
    /*
    avad_cfg->avad_quantile_p = 3; //0.8
    avad_cfg->avad_gain_db = 10;
    avad_cfg->avad_compare_v = 3;

    dvad_cfg->dvad_gain_id = 10;
    dvad_cfg->d2a_th_db = 20;
    dvad_cfg->d_frame_con = 100;
    dvad_cfg->d2a_frame_con = 100;
    dvad_cfg->d_stride1 = 3;//<<7
    dvad_cfg->d_stride2 = 5;//<<7
    dvad_cfg->d_low_con_th = 6;
    dvad_cfg->d_high_con_th = 3;
    */

    /* printf("avad_cfg @ 0x%x, dvad_cfg @ 0x%x\n", avad_cfg, dvad_cfg);
     printf("avad_cfg->avad_th = %d\n", avad_cfg->avad_th);
     printf("avad_cfg->quantile_p  = %d\n", avad_cfg->quantile_p);
     printf("avad_cfg->step = %d\n", avad_cfg->step);
     printf("avad_cfg->avad_stride = %d\n", avad_cfg->avad_stride);
     printf("avad_cfg->tmp_thl = %d\n", avad_cfg->tmp_thl);
     printf("avad_cfg->sig_l = %d\n", avad_cfg->sig_l);
     printf("avad_cfg->sig_h = %d\n", avad_cfg->sig_h);
     printf("avad_cfg->avad_threshold = %d\n", avad_cfg->avad_threshold);
     printf("avad_cfg->num_th = %d\n", avad_cfg->num_th);
     printf("avad_cfg->high_num = %d\n", avad_cfg->high_num);
     printf("avad_cfg->s_high_con_th = %d\n", avad_cfg->s_high_con_th);

     printf("dvad_cfg->d_low_con_th = %d\n", dvad_cfg->d_low_con_th);
     printf("dvad_cfg->d_high_con_th = %d\n", dvad_cfg->d_high_con_th);
     printf("dvad_cfg->d_stride1 = %d\n", dvad_cfg->d_stride1);
     printf("dvad_cfg->d_stride2 = %d\n", dvad_cfg->d_stride2);
     printf("dvad_cfg->st_1 = %d\n", dvad_cfg->st_1);
     printf("dvad_cfg->st_2 = %d\n", dvad_cfg->st_2);
     printf("dvad_cfg->si_num_th = %d\n", dvad_cfg->si_num_th);
     printf("dvad_cfg->sf_con_th = %d\n", dvad_cfg->sf_con_th);*/

    vad_init(avad_cfg, dvad_cfg);

    __this->status = VAD_STATUS_IDLE;
}

static void p11_vad_close(void)
{
    __this->status = VAD_STATUS_CLOSE;

    vad_close();
    return;
}

void p11_vad_test();
void p11_vad_m2p_handle(void)
{
    u8 cmd = m2p_message[M2P_MESSAGE_VAD_CMD];

    //printf("%s, cmd = 0x%x\n", __func__, cmd);
    switch (cmd) {
    case M2P_VAD_CMD_INIT:
        p11_vad_init();
        break;
    case M2P_VAD_CMD_FRAME:
        vad_hw_updata_read_ptr(m2p_message[M2P_MESSAGE_VAD_CBUF_RPTR]);
        break;
    case M2P_VAD_CMD_CLOSE:
        p11_vad_close();
        break;
    case M2P_VAD_CMD_TEST:
        p11_vad_test();
        break;
    default:
        break;
    }

    return;
}

//===========================================================================//
//               			  P11_VAD POWER MANAGE                           //
//===========================================================================//
static enum LOW_POWER_LEVEL vad_level_query(void)
{
    enum LOW_POWER_LEVEL level = LOW_POWER_MODE_DEEP_SLEEP;

    switch (__this->status) {
    case VAD_STATUS_CLOSE:
        level = LOW_POWER_MODE_DEEP_SLEEP;
        break;
    case VAD_STATUS_IDLE:
        level = LOW_POWER_MODE_SLEEP;
        break;
    case VAD_STATUS_ACTIVE:
        level = LOW_POWER_MODE_IDLE;
        break;
    default:
        level = LOW_POWER_MODE_DEEP_SLEEP;
        break;
    }

    return level;
}

static void p11_vad_low_power_enter(enum LOW_POWER_LEVEL level)
{
    return;
}

static void p11_vad_low_power_exit(enum LOW_POWER_LEVEL level)
{
    return;
}

REGISTER_LOWPOWER_TARGET(vad_lp_target, 0) = {
    .name = "vad",
    .level = vad_level_query,
    .enter = p11_vad_low_power_enter,
    .exit = p11_vad_low_power_exit,
};


#endif /* #if CONFIG_VAD_ENABLE */
