#ifndef __MSG_P11_H__
#define __MSG_P11_H__


extern void user_p11_p2m_post_init();
extern void user_p11_post_to_main_system(u8 cmd, u16 len, u8 *data, u8 wait);
extern int user_main_post_to_p11_system(u8 cmd, u8 wait);

enum P11_CMD_TABLE {
    P11_SYS_RAM_INIT =    0,
    P11_SYS_KICK     =    1,
    P11_SYS_TO_SELF  =    2,
    P11_SYS_GSENSOR_IRQ      = 3,
    P11_SYS_HR_SENSOR_IQR    = 4,
    P11_SYS_SPO2_SENSOR_IQR  = 5,
    P11_SYS_GSENSOR_START    = 6,
    P11_SYS_GSENSOR_STOP     = 7,
    P11_SYS_HRSENSOR_START   = 8,
    P11_SYS_HRSENSOR_STOP    = 9,
    P11_SYS_SPO2_START   = 10,
    P11_SYS_SPO2_STOP    = 11,
    P11_SYS_GSENSOR_SEARCH = 12,
    P11_SYS_HRSENSOR_SEARCH = 13,




    P11_SYS_IIC_INIT_SYNC     = 14,
    P11_SYS_IIC_R_SYNC     = 15,
    P11_SYS_IIC_W_SYNC     = 16,

    P11_SYS_ALGO_EVENT   = 50,

    P11_SYS_WAKE     = 0xff,
};



enum {
    MSG_CLEAN_ALL_MSG = 0x0ffe,
    NO_MSG = 0x0fff,
};

enum {
    MSG_NO_ERROR = 0,
    MSG_EVENT_EXIST = -1,
    MSG_NOT_EVENT = -2,
    MSG_EVENT_PARAM_ERROR = -3,
    MSG_BUF_NOT_ENOUGH = -4,
};

//最大31
enum P11_SENSOR_TABLE {
    E_G_SENSOR_ONLINE = 0,
    E_HR_SENSOR_ONLINE = 1,
    E_SPO2_SENSOR_ONLINE = 2,
    E_BAROMETER_SENSOR_ONLINE = 3,
    E_SENSOR_WEAR_CHECK = 4,



    E_SENSOR_MAX = 32,
};

#define N_GSENSOR      (0)
#define N_HR_SENSOR    (1)
#define N_SPO2_SENSOR  (2)

#define N_SENSOR_NUM   ((N_SPO2_SENSOR)+1)

struct SENSOR_SHARE_PORT {
    u32 event;//按bit为单位,支持32位的状态同步
    int sensor_hd[N_SENSOR_NUM][36 / 4]; //36是cbuf句柄的大小
};



extern void task_clear_all_message(void);
extern void task_message_init();
extern int task_post_msg(char *name, int argc, ...);
extern int task_get_msg(u16 timeout, int len, int *msg);



#endif
