#include "gSensor_manage.h"
#include "app_config.h"
#include "circular_buf.h"
#include "gpio.h"
#include "sensor_gp_timer.h"
#include "gp_timer_api.h"
#include "sensor_api.h"
#include "msg.h"
#include "iic_soft.h"
#include "bank_switch.h"
#include "gSensor_algo.h"
/* #include "device/device.h" */
/* #include "debug.h" */
/* #include "key_event_deal.h" */
/* #include "btstack/avctp_user.h" */
/* #include "app_main.h" */
/* #pragma bss_seg(".gsport_dev_bss") */
/* #pragma const_seg(".gsport_dev_const") */
/* #pragma code_seg(".gsport_dev_code") */
/*  */


/* #if TCFG_GSENSOR_ENABLE */

#ifndef log_info
#define log_info(...) // printf
#endif
#ifndef log_e
#define log_e(...) printf(__VA_ARGS__)
#endif

#define BANK_NAME  BANK_SENSOR_TAG


static const struct gsensor_platform_data *platform_data;
G_SENSOR_INTERFACE *gSensor_hdl = NULL;
G_SENSOR_INFO  __gSensor_info = {.iic_delay = 10};
#define gSensor_info (&__gSensor_info)
#define BUF_POINT ((20+30)*3) //gsensorlen
/* #define BUF_POINT ((200+30)*3+7*1024+256) //gsensorlen */
static cbuffer_t *data_w_cbuf;
static short data_buf[BUF_POINT];
static u8 read_write_status = 0;
static u16 callback_len = sizeof(data_buf);

#if TCFG_GSENOR_USER_IIC_TYPE
#define iic_init(iic)                       hw_iic_init()
#define iic_close(iic)                      hw_iic_close()
#define iic_start(iic)                      hw_iic_start()
#define iic_stop(iic)                       hw_iic_stop()
#define iic_tx_byte(iic, byte)              hw_iic_master_tx_byte(byte)
#define iic_rx_byte(iic, ack)               hw_iic_master_rx_byte(ack)
#define iic_read_buf(iic, buf, len)         hw_iic_master_read_buf(buf, len)
#define iic_write_buf(iic, buf, len)        hw_iic_master_write_buf(buf, len)
/* #define iic_suspend(iic)                    hw_iic_suspend() */
/* #define iic_resume(iic)                     hw_iic_resume() */
#else
#define iic_init(iic)                       soft_iic_init(iic)
#define iic_close(iic)                     soft_iic_uninit(iic)
#define iic_start(iic)                      soft_iic_start(iic)
#define iic_stop(iic)                       soft_iic_stop(iic)
#define iic_tx_byte(iic, byte)              soft_iic_tx_byte(iic, byte)
#define iic_rx_byte(iic, ack)               soft_iic_rx_byte(iic, ack)
#define iic_read_buf(iic, buf, len)         soft_iic_read_buf(iic, buf, len)
#define iic_write_buf(iic, buf, len)        soft_iic_write_buf(iic, buf, len)
/* #define iic_suspend(iic)                    soft_iic_suspend(iic) */
/* #define iic_resume(iic)                     soft_iic_resume(iic) */
#endif




//8 bit 地址
int gravity_sensor_write(u8 w_chip_id,
                         u8 *data, u8 length)
{
    u8  ret;
    iic_start(gSensor_info->iic_hdl);
    if (0 == iic_tx_byte(gSensor_info->iic_hdl, w_chip_id)) {
        ret = -1;
        log_e("\n gsen iic wr err 0\n");
        goto __gcend;
    }
    delay(gSensor_info->iic_delay);
    ret = iic_write_buf(gSensor_info->iic_hdl, data, length);
    if (ret != length) {
        ret = -1;
        log_e("\n gsen iic wr err 1 %x %x\n", ret, length);
        goto __gcend;
    }
__gcend:
    iic_stop(gSensor_info->iic_hdl);
    return ret;
}









u8 gravity_sensor_command(u8 w_chip_id, u8 register_address, u8 function_command)
{
    /* putchar('['); */
    u8 ret = 1;
    iic_start(gSensor_info->iic_hdl);
    if (0 == iic_tx_byte(gSensor_info->iic_hdl, w_chip_id)) {
        ret = 0;
        log_e("\n gsen iic wr err 0\n");
        goto __gcend;
    }

    delay(gSensor_info->iic_delay);

    if (0 == iic_tx_byte(gSensor_info->iic_hdl, register_address)) {
        ret = 0;
        log_e("\n gsen iic wr err 1\n");
        goto __gcend;
    }

    delay(gSensor_info->iic_delay);

    if (0 == iic_tx_byte(gSensor_info->iic_hdl, function_command)) {
        ret = 0;
        log_e("\n gsen iic wr err 2\n");
        goto __gcend;
    }

__gcend:
    iic_stop(gSensor_info->iic_hdl);
    /* putchar(']'); */
    return ret;
}

int _gravity_sensor_get_ndata(u8 r_chip_id, u8 register_address, u8 *buf, int data_len)
{
//	printf("%s",__func__);
    /* putchar('{'); */
    u8 read_len = 0;

    iic_start(gSensor_info->iic_hdl);
    if (0 == iic_tx_byte(gSensor_info->iic_hdl, r_chip_id - 1)) {
        log_e("\n gsen iic rd err 0\n");
        read_len = 0;
        goto __gdend;
    }


    delay(gSensor_info->iic_delay);
    if (0 == iic_tx_byte(gSensor_info->iic_hdl, register_address)) {
        log_e("\n gsen iic rd err 1\n");
        read_len = 0;
        goto __gdend;
    }

    iic_start(gSensor_info->iic_hdl);
    if (0 == iic_tx_byte(gSensor_info->iic_hdl, r_chip_id)) {
        log_e("\n gsen iic rd err 2\n");
        read_len = 0;
        goto __gdend;
    }

    delay(gSensor_info->iic_delay);

    for (; data_len > 1; data_len--) {
        *buf++ = iic_rx_byte(gSensor_info->iic_hdl, 1);
        read_len ++;
    }

    *buf = iic_rx_byte(gSensor_info->iic_hdl, 0);
    read_len ++;

__gdend:

    iic_stop(gSensor_info->iic_hdl);
    delay(gSensor_info->iic_delay);
    return read_len;
}


void gSensor_int_io_detect(void *priv)
{
    u8 int_io_status = 0;
    u8 det_result = 0;
    int_io_status = gpio_read(platform_data->gSensor_int_io);
    log_info("status %d\n", int_io_status);
    gSensor_hdl->gravity_sensor_ctl(GSENSOR_INT_DET, &int_io_status);
    if (gSensor_hdl->gravity_sensor_check == NULL) {
        return;
    }
    det_result = gSensor_hdl->gravity_sensor_check();
    if (det_result == 1) {
        log_info("GSENSOR_EVENT_CLICK\n");
        /* gSensor_event_to_user(KEY_EVENT_CLICK); */
    } else if (det_result == 2) {
        log_info("GSENSOR_EVENT_DOUBLE_CLICK\n");
        /* gSensor_event_to_user(KEY_EVENT_DOUBLE_CLICK); */
    } else if (det_result == 3) {
        log_info("GSENSOR_EVENT_THREE_CLICK\n");
        /* gSensor_event_to_user(KEY_EVENT_TRIPLE_CLICK); */
    }
}


int gSensor_read_data(u8 *buf, u8 buflen)
{
    if (buflen < 32) {
        return 0;
    }
    axis_info_t accel_data[32];
    if (!gpio_read(platform_data->gSensor_int_io)) {
        gSensor_hdl->gravity_sensor_ctl(READ_GSENSOR_DATA, accel_data);
    }
    memcpy(buf, accel_data, sizeof(accel_data));
    return sizeof(accel_data) / sizeof(axis_info_t);
}
//输出三轴数组和数据长度
//
int get_gSensor_data(short *buf)
{
    axis_info_t accel_data[32];
    /* if (!gpio_read(platform_data->gSensor_int_io))  */
    {
        gSensor_hdl->gravity_sensor_ctl(READ_GSENSOR_DATA, accel_data);
        for (int i = 0; i < 29; i++) {
            buf[i * 3] = accel_data[i].x;
            buf[i * 3 + 1] = accel_data[i].y;
            buf[i * 3 + 2] = accel_data[i].z;
            //	printf("cnt:%1d   x:%5d   y:%5d   z:%5d\n", i, accel_data[i].x, accel_data[i].y, accel_data[i].z);
        }
        return sizeof(accel_data) / sizeof(axis_info_t);
    }
    return 0;
}


int read_gsensor_buf_check_len()
{
    return data_w_cbuf->data_len;
}


int gsensor_set_soft_irq_size(u16 size)
{

    if (size <= sizeof(data_buf)) {
        callback_len = size;
        return true;
    }
    return false;
}

int read_gsensor_nbuf(u8 *buf, short datalen)
{
    /* printf("%s %d\n",__func__,__LINE__); */
    if (data_w_cbuf == NULL) {

        //printf("%s %d\n", __func__, __LINE__);
        return 0;
    }
    if (gSensor_info->init_flag  == 1) {
        //		if (read_write_status == 0) {
        //      	read_write_status = 1;
        //      	return 0;
        //  	}
        if (data_w_cbuf->data_len >=  datalen) {
            cbuf_read(data_w_cbuf, buf, datalen);
            return datalen;
        } else {
            /* printf("%s %d\n", __func__, __LINE__); */
            return 0;
        }
    } else {
        printf("%s NOT ONLINE ", __func__);
        return 0;
    }
}


extern int user_main_post_to_p11_system(u8 cmd, u8 wait);
extern void hr_sensor_input_accel_data(void *data, u8 point);

void gsensor_data_irq_handle(void)
{
    static u8 gsensor_err = 0;

    axis_info_t accel_data[32];
    if (data_w_cbuf == NULL) {
        return ;
    }
    if (gSensor_info->init_flag  == 1) {
        /* if (!gpio_read(platform_data->gSensor_int_io))  */
        {
            int point  = gSensor_hdl->gravity_sensor_ctl(READ_GSENSOR_DATA, accel_data);
            u32 wlen = -1;
            if (point) {
                gsensor_err = 0;
                gsensor_algo_run(accel_data, point);
//                hr_sensor_input_accel_data(accel_data, point);
                wlen = cbuf_write(data_w_cbuf, accel_data, point * 2 * 3);
            } else {
                //多次读gsensor数据为空，gsensor可能异常，对gsensor 进行复位
                gsensor_err++;
                if (gsensor_err > 2) {
                    gSensor_hdl->gravity_sensor_ctl(GSENSOR_RESET_INT, NULL);
                    gsensor_err = 0;
                }
            }
            if (wlen == 0) {
                cbuf_clear(data_w_cbuf);
                wlen = cbuf_write(data_w_cbuf, accel_data, point * 2 * 3);
                user_p11_post_to_main_system(P11_SYS_GSENSOR_IRQ, 0, NULL, 0);
                /* printf("data_w_cbuf_full %x %x %x\n", data_w_cbuf, data_w_cbuf->data_len, &(data_w_cbuf->data_len)); */
                dev_log("g_full p=%d l=%d t=%d\n", point, data_w_cbuf->data_len, data_w_cbuf->total_len);
            } else {
                if (cbuf_get_data_len(data_w_cbuf) >=  callback_len) {
                    dev_log("<<%d p>>>", callback_len);
                    user_p11_post_to_main_system(P11_SYS_GSENSOR_IRQ, 0, NULL, 0);
                }
            }
        }
    } else {
        //printf("%s ",__func__);
        return ;
    }
}
void write_gsensor_data_handle(void)
{
    //printf("%s", __func__);
    sensorhub_irq_add(IRQ_GSENSOR_INDEX);
    // putchar('R');
    user_main_post_to_p11_system(P11_SYS_KICK, 0);
    return ;
}



/* REGISTER_CODE_SEG(BANK_CODE_SENSOR_NORMAL_BANK_1) */
int gsensor_io_ctl(u8 cmd, void *arg)
{
    //printf("%s cmd=%d\n", __func__, cmd);
    return gSensor_hdl->gravity_sensor_ctl(cmd, arg);
}
//u8 gravity_sen_dev_cur;		/*当前挂载的Gravity sensor*/
extern G_SENSOR_INTERFACE  gsensor_dev_begin[];
extern G_SENSOR_INTERFACE gsensor_dev_end[];

/* REGISTER_CODE_SEG(BANK_CODE_SENSOR_INIT_BANK_0) */
int gravity_sensor_init(void *_data, cbuffer_t *cbuffer)
{

    gSensor_info->init_flag  = 0;

    int retval = 0;
    platform_data = (const struct gsensor_platform_data *)_data;
    gSensor_info->iic_hdl = platform_data->iic;
    retval = iic_init(gSensor_info->iic_hdl);

    log_e("\n  gravity_sensor_init\n");

    if (retval < 0) {
        log_e("\n  open iic for gsensor err\n");
        return retval;
    } else {
        log_info("\n iic open succ\n");
    }

    retval = -EINVAL;
    list_for_each_gsensor(gSensor_hdl) {
        if (!memcmp(gSensor_hdl->logo, platform_data->gSensor_name, strlen(platform_data->gSensor_name))) {
            retval = 0;
            break;
        }
    }
    /* printf("gsensor_dev_begin:0x%x,gsensor_dev_end :0x%x\n", gsensor_dev_begin, gsensor_dev_end); */
    /* printf("gsensor_dev_begin:0x%x,gsensor_dev_end :0x%x,sizeof(struct):%d\n", gsensor_dev_begin, gsensor_dev_end, sizeof(G_SENSOR_INTERFACE)); */
    /* printf("gSensor_hdl:0x%x,2logo:%s, name:%s\n", gSensor_hdl, gSensor_hdl->logo, platform_data->gSensor_name); */

    if (retval < 0) {
        log_e(">>>gSensor_hdl logo err\n");
        return retval;
    }

    if (gSensor_hdl->gravity_sensor_init()) {
        log_e(">>>>gSensor_Int ERROR\n");
    } else {
        log_info(">>>>gSensor_Int SUCC\n");
        gSensor_info->init_flag  = 1;
        if (cbuffer) {
            data_w_cbuf = cbuffer;
        } else {
            ASSERT(0);
        }
        cbuf_init(data_w_cbuf, data_buf, BUF_POINT * sizeof(u16));
        printf("gsensor init ok!\n");
        if (platform_data->gSensor_int_io != -1) {
            /* timer0_capture_init(platform_data->gSensor_int_io);//默认下降沿触发 */


            /* gp_timer_capture(P11_GPTMR0, platform_data->gSensor_int_io, GP_TIMER_CAPTURE_EDGE_DOWN, 0); */

            /* gp_timer_register_irq(P11_GPTMR0, write_gsensor_data_handle); */


        }
    }
    return 0;
}

int gsensor_disable(void)
{
    printf("\n%s\n", __func__);
// #if CONFIG_SENSOR_ALG_ENABLE
//     soft_iic_uninit(0);//some sensor need wait iic idle little ms
// #endif
    if (data_w_cbuf == NULL) {
        return -1;
    } else {
        if (gSensor_info->init_flag  == 1) {
            int valid = 0;
            gSensor_hdl->gravity_sensor_ctl(GSENSOR_DISABLE, &valid);
            cbuf_clear(data_w_cbuf);
            gsensor_algo_uninit();
            if (valid == 0) {
                /* free(data_w_cbuf); */
                /* data_w_cbuf = NULL; */
                return 0;
            }
        }
    }
    return -1;
}

int gsensor_enable(void)
{
    printf("\n%s\n", __func__);
    //查找设备
    int valid = 0;
    gSensor_hdl->gravity_sensor_ctl(SEARCH_SENSOR, &valid);
    if (valid == 0) {
        return -1;
    }
    //工作空间
    /* data_buf = zalloc(BUF_SIZE); */
    /* if (data_buf == NULL) { */
    /*     printf("gsensor_cbuf_error!"); */
    /*     return -1; */
    /* } */
    /* data_w_cbuf = zalloc(sizeof(cbuffer_t)); */
    /* if (data_w_cbuf == NULL) { */
    /*     return -1; */
    /* } */
    /* cbuf_init(data_w_cbuf, data_buf, BUF_SIZE); */
    printf("cbuf_init");
    //设置参数
    valid = 0;
    gSensor_hdl->gravity_sensor_ctl(GSENSOR_RESET_INT, &valid);

    if (valid == -1) {
        return -1;
    }

    gsensor_algo_init();
    printf("gsensor_reset_succeed\n");
    return 0;
}


/************************ gsensor read data test********************/
#if 0
void gSensor_read_sensor_data_test()
{
    axis_info_t accel_data[32];
    u8 temp1[10];
    temp1[0] = 0x03;
    temp1[1] = 0xfc;
    temp1[9] = 0x03;
    temp1[8] = 0xfc;
    if (!gpio_read(platform_data->gSensor_int_io)) {
        gSensor_hdl->gravity_sensor_ctl(READ_GSENSOR_DATA, accel_data);

        for (u8 i = 0; i < 29; i++) {
            /* temp1[3] = accel_data[i].x >> 8; */
            /* temp1[2] = accel_data[i].x; */
            /* temp1[5] = accel_data[i].y >> 8; */
            /* temp1[4] = accel_data[i].y; */
            /* temp1[7] = accel_data[i].z >> 8; */
            /* temp1[6] = accel_data[i].z; */
            /* for (u8 j = 0; j < 10; j++) { */
            /* 	putbyte(temp1[j]); */
            /* } */
            printf("cnt:%2d,  x:%5d,  y:%5d,  z:%5d\n", i, accel_data[i].x, accel_data[i].y, accel_data[i].z);
        }
    }
}

void gsensor_read_cbuf_test()
{
    short temp[96];
    u16 i = 0;
    if (read_gsensor_nbuf(temp, 2 * 96)) {
        for (i = 0; i < 96; i += 3) {
            printf("x:%d,y:%d,z:%d\n", temp[i], temp[i + 1], temp[i + 2]);
        }
        printf("\n");
    } else {
        printf("read cbuf fail!\n");
    }
}


#endif
/* #endif */
