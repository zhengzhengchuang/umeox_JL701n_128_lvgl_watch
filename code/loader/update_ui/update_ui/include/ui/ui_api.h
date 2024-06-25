#ifndef _UI_API_H_
#define _UI_API_H_


enum ui_devices_type {
	LED_7,
	LCD_SEG3X9,
	TFT_LCD,//彩屏
	DOT_LCD,//点阵屏
};

//板级配置数据结构
struct ui_devices_cfg {
	enum ui_devices_type type;
	void *private_data;
};


#endif