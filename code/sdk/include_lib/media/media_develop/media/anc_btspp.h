#ifndef _ANC_BTSPP_H_
#define _ANC_BTSPP_H_

#include "generic/typedef.h"
#include "asm/anc.h"

//前期训练 建议先做喇叭测试，再做MIC测试。这样内部可以通过喇叭判断MIC好不好

#define ANC_POW_DISPLAY             1			//显示调试信息
//-------------------ANC SPP 协议 -------------------//

//父命令
#define	ANC_COMMAND_RESPO			0x00		//命令回复 
#define ANC_TRAIN_NOMA_MODE			0x01 		//普通训练模式
#define ANC_TRAIN_FAST_MODE			0x02 		//快速训练模式

//训练工具子命令1
#define ANC_CHANGE_COMMAND			0x00		//父命令模式切换
#define	ANC_MUTE_TARIN				0x01		//静音训练
#define	ANC_NOISE_TARIN   			0x02 		//噪声训练
#define ANC_TARIN_AGAIN    			0x03		//继续训练
#define ANC_MODE_OFF				0x04		//ANC关
#define ANC_MODE_ON					0x05		//ANC降噪模式
#define ANC_TRAIN_EXIT				0x06		//ANC训练结束

#define ANC_PASS_MODE_ON			0x07		//ANC通透模式
#define ANC_TRAIN_STEP_1			0x08		//ANC训练步进1
#define ANC_TRAIN_STEP_2			0x09		//ANC训练步进2
#define ANC_CHANGE_MODE				0x10		//模式设置指令

#define ANC_MIC_GAIN_SET			0x11		//设置主MIC的增益
#define ANC_TRIM_MIC_GAIN_SET		0x12		//设置误差MIC的增益
#define ANC_DAC_ANA_GAIN_SET		0x13		//设置DAC的模拟增益
#define ANC_DIG_GAIN_SET			0x14		//设置ANC的数字增益

#define ANC_TRAIN_STEP_SET          0x15		//设置ANC训练步进 
#define ANC_SZ_NOISE_THR_SET		0x16		//设置SZ MIC底噪能量阈值
#define ANC_FZ_NOISE_THR_SET		0x17		//设置FZ MIC底噪能量阈值
#define ANC_SZ_ADAP_THR_SET			0x18		//设置SZ 自适应收敛阈值
#define ANC_FZ_ADAP_THR_SET			0x19		//设置FZ 自适应收敛阈值
#define ANC_WZ_TRAIN_THR_SET		0x1a		//设置WZ 训练阈值

#define ANC_NONADAP_TIME_SET		0x1b		//设置非自适应收敛时间
#define ANC_SZ_ADAP_TIME_SET		0x1c		//设置SZ自适应收敛时间
#define ANC_FZ_ADAP_TIME_SET		0x1d		//设置FZ自适应收敛时间
#define ANC_WZ_TRAIN_TIME_SET		0x1e		//设置WZ训练时间

//
//----------------耳机反馈子命令1---------------------//
#define	ANC_EXEC_SUCC				0x01		//执行成功 
#define ANC_EXEC_FAIL				0x02		//执行失败
#define ANC_ADJUST_MIC0_FAIL		0x03		//MIC0校验失败
#define ANC_ADJUST_MIC1_FAIL		0x04		//MIC1校验失败
#define ANC_ADJUST_MIC_ALL_FAIL		0x05		//MIC0/MIC1校验失败

#define ANC_MIC0_FAIL   		BIT(0)
#define ANC_MIC1_FAIL   		BIT(1)
#define ANC_ALL_MIC_FAIL   		ANC_MIC0_FAIL|ANC_MIC1_FAIL
// #endif

//----------------------NEW---------------------------//
#define ANC_TRIM_MIC_WORK_FAIL		0x80|BIT(0)	//MIC1工作不正常
#define ANC_MIC_WORK_FAIL   		0x80|BIT(1)	//MIC0工作不正常
#define ANC_TRIM_MIC_SNR_FAIL   	0x80|BIT(2)	//MIC1信噪比差
#define ANC_MIC_SNR_FAIL   			0x80|BIT(3) //MIC0信噪比差
#define ANC_TRIM_MIC_NOISE_FAIL  	0x80|BIT(4) //MIC1底噪高
#define ANC_MIC_NOISE_FAIL   		0x80|BIT(5) //MIC0底噪高
//多种错误信息组合为|的形式,如MIC0/MIC1工作都不正常，则返回0x80|BIT(0)|BIT(1)




#define ANC_SPP_PACK_NUM		10		//数据包长度

#define ANC_SPP_MAGIC			0x55AA

enum{
	ANC_SZ_MUTE_STATUS = 0,					//SZ非自适应（DAC静音）状态
	ANC_SZ_NADAP_STATUS,					//SZ非自适应状态
	ANC_SZ_ADAP_STATUS,						//SZ自适应状态
	ANC_FZ_MUTE_STATUS,						
	ANC_FZ_NADAP_STATUS,
	ANC_FZ_ADAP_STATUS,
	ANC_WZ_ADAP_STATUS
};

typedef struct {
	u16 magic;
	u16 crc;
	u16 len;
	u8 dat[0];
} anc_spp_data_t;

typedef struct{
	u8  mode;
	u8	train_step; 
	u8  ret_step;
	u8 Reserved2;
	u32 sz_lower_thr;
	u32 fz_lower_thr;
	u32 sz_noise_thr;			
	u32 fz_noise_thr;	
	u32 sz_adaptive_thr;
	u32 fz_adaptive_thr;
	u32 wz_train_thr;
	u16 non_adaptive_time;
	u16 sz_adaptive_time;
	u16 fz_adaptive_time;
	u16 wz_train_time;
} anc_train_para_t;

typedef struct {
	u8 train_busy;
	anc_train_para_t *para;
#if ANC_POW_DISPLAY
	u8 pow_dat[10];
	u8 status;
#endif
	anc_spp_data_t rx_buf;
	anc_spp_data_t tx_buf;

} anc_spp_t;


typedef struct{
	u8 mic_errmsg;
	u8 status;
	u32 pow;
	u32 temp_pow;
} anc_ack_msg_t;

void anc_spp_init(anc_train_para_t *para);
void anc_spp_uninit(void);
int anc_spp_event_deal(u8 *dat);
void anc_train_api_set(u8 cmd, u32 data, anc_train_para_t *para);
int anc_spp_rx_packet(u8 *dat, u8 len);
int anc_spp_tx_packet(u8 mode, u8 command);

void anc_btspp_display_pow(u32 pow,u8 status);
void anc_btspp_status_set(u8 status);

#endif/*_ANC_BTSPP_H_*/

