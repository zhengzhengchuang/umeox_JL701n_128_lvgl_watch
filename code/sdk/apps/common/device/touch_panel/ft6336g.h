#ifndef _FT6336G_H_
#define _FT6336G_H_


#if TCFG_TP_FT6336G_ENABLE
#define FT6336G_ID_G_MODE  1//1:trigger, 0:polling

/**********register address************/
#define FTS_REG_REG_MODE_SWITCH		0x00   //register mode switch. 0:Normal operating Mode
#define FTS_REG_TIMEENTERMONITOR		0x87   // the time delay value when entering monitor status.
#define FTS_REG_POINT_RATE			0x88   //report rate. range:1~(表示10hz~)
#define FTS_REG_CHIP_ID_HIGH		0xA3   //chip ID
#define FTS_REG_CHIP_ID_MID			0x9f   //chip ID
#define FTS_REG_CHIP_ID_LOW			0xA0   //chip ID
#define FTS_REG_ID_G_MODE			0xA4   //interrupt work mode. (1:trigger; 0:polling)
#define FTS_REG_ID_G_PMODE			0xA5   //power consumption mode of the TPM when in running status(0:active, 1:monitor, 3:hibernate(deep sleep))
#define FTS_REG_FW_VER				0xA6   //FW  version
#define FTS_REG_ID_G_STATE			0xA7   // to configure the run mode of TPM

/**********gesture register address************/
//特殊手势模式总使能
#define ID_G_SPEC_GESTURE_ENABLE		0xd0//1:normal to gesture,  0:gesture to normal
//bit7bit6:no,bit5:数字字符识别,bit4:双击,bit3:下滑直线,bit2:上滑直线,bit1:右滑直线,bit0:左滑直线
#define ID_G_SPEC_GESTURE_TYPE_ENABLE	0xd1//1:enable, 0:disable
#define ID_G_SPEC_GESTURE_CHAR1_ENABLE	0xd2
#define ID_G_SPEC_GESTURE_CHAR2_ENABLE	0xd5
#define ID_G_SPEC_GESTURE_CHAR3_ENABLE	0xd6
#define ID_G_SPEC_GESTURE_CHAR4_ENABLE	0xd7
#define ID_G_SPEC_GESTURE_CHAR5_ENABLE	0xd8
#define ID_G_SPEC_GESTURE_ID_NUM		0xd3
#define ID_G_SPEC_DEBUG_USE				0xda
// enum {
// 	ELM_EVENT_TOUCH_DOWN,
// 	ELM_EVENT_TOUCH_MOVE,
// 	ELM_EVENT_TOUCH_R_MOVE,
// 	ELM_EVENT_TOUCH_L_MOVE,
// 	ELM_EVENT_TOUCH_D_MOVE,
// 	ELM_EVENT_TOUCH_U_MOVE,
// 	ELM_EVENT_TOUCH_HOLD,
// 	ELM_EVENT_TOUCH_UP,
// };
enum power_mode {
    POWER_ACTIVE_MODE = 0,
    POWER_MONITOR_MODE = 1,
    POWER_HIBERNATE_MODE = 3, //100ua.quit:need ft6336 reset
};


typedef struct {
    u8 init;
    hw_iic_dev iic_hdl;
} ft6336_param;

int ft6336g_init(ft6336_param *param);
void ft6336g_int_en();
int ft6336g_power_consumption_mode(enum power_mode mode);
void ft6336g_reset();
void ft6336g_gesture_switch(u8 gesture_en);//1:normal to gesture,  0:gesture to normal
// int ft6336g_id_g_mode(u8 mode)//1:trigger, 0:polling
#endif
#endif

