#ifndef __USER_P11_EVENT_IRQ_H__
#define __USER_P11_EVENT_IRQ_H__


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



struct p11_event_handler {
    int event_type;
    int (*handler)(int event_type, u16 len, u8 *data);
};


#define P11_IRQ_EVENT_HANDLER(type, fn) \
 	static const struct p11_event_handler __event_handler_##fn sec(.p11_irq_handler) __attribute__((used)) = { \
		.event_type = type, \
		.handler = fn, \
	}

extern struct p11_event_handler p11_event_handler_begin[];
extern struct p11_event_handler p11_event_handler_end[];

#define list_for_each_p11_event_handler(p) \
	for (p = p11_event_handler_begin; p < p11_event_handler_end; p++)

//数据最大数据长度在p11 系统配置，用户可以灵活配置
//当前配置512 字节
extern void user_main_post_to_p11_system(u8 cmd, u16 len, u8 *data, u8 wait);


extern u32 master_p11cbuf_get_data_len(int index);
extern u32 master_p11cbuf_read(int index, void *buf, u32 len);
#endif
