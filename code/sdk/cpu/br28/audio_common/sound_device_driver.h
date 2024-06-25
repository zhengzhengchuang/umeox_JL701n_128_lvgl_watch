#ifndef SOUND_DEVICE_DRIVER_H_
#define SOUND_DEVICE_DRIVER_H_

struct broadcast_linein_params {
    u16 sample_rate;    //采样率
    u8 gain;			//增益
    u8 channel_num;     //通道数
    void *output_priv;                      //数据接口私有参数
    int (*output_handler)(void *output_priv, s16 *data, int len);//输出接口

};
int broadcast_linein_open(struct broadcast_linein_params *params);
void broadcast_linein_close(void);

#endif

