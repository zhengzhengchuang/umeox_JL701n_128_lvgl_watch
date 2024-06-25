#ifndef __GOODIX_SPO2_INTERFACE_H
#define __GOODIX_SPO2_INTERFACE_H

#include <stdint.h>
#include "goodix_type.h"

/**
* @brief SPO2 算法函数返回状态值
*/
typedef enum
{
	GX_ALGO_SPO2_SUCCESS = 0x00000000,			/**< 成功     */
	GX_ALGO_SPO2_RWONG_INPUT = 0x00000001,		/**< 输入数据格式不合法     */
	GX_ALGO_SPO2_NO_MEMORY = 0x00000002,        /**< 内存空间不够     */
	GX_ALGO_SPO2_FRAME_UNCOMPLETE = 0x00000003, /**< 处于降采样平均过程中*/
	GX_ALGO_SPO2_WIN_UNCOMPLETE = 0x00000004,   /**< 未到完整滑窗帧位置（整数s）     */
	GX_ALGO_SPO2_UNEXPECTED = 0x00000005,      /*不合设计逻辑异常 */
}goodix_spo2_ret;

#define CHIP_PPG_CHL_NUM		                4  //芯片支持的通道数

typedef struct {
	//raw 配置信息
	uint32_t valid_chl_num;			// 有效通道数
	uint32_t raw_fs;			    // 原始采样率
	
	int32_t cali_coef_a;							//校准参数2次项
	int32_t cali_coef_b;							//校准参数1次项
	int32_t cali_coef_c;							//校准参数常数项
	//hb使能标志
	uint32_t hb_en_flg;					//hb使能标志
	//佩戴状态
	uint32_t wear_mode;					//佩戴状态 0:手环  1：手指  2：耳机
	// acc move thr
	uint32_t acc_thr_max;
	uint32_t acc_thr_min;
	uint32_t acc_thr_scale;
	uint32_t acc_thr_num;
	
	//CTR设置
	uint32_t ctr_en_flg;
	uint32_t ctr_red_thr;
}goodix_spo2_config;


typedef struct
{
	uint32_t frameid;								// 帧序号
	uint8_t bit_num;                                //数据位数
	int32_t ppg_rawdata[3 * CHIP_PPG_CHL_NUM];		// PPG原始数据，依次绿光1234-红外1234-红光1234
	int8_t cur_adj_flg[3 * CHIP_PPG_CHL_NUM];		// 电流调光标志位， 依次绿光1234-红外1234-红光1234
	int8_t gain_adj_flg[3 * CHIP_PPG_CHL_NUM];		// 增益调光标志位， 依次绿光1234-红外1234-红光1234
	int8_t enable_flg[3 * CHIP_PPG_CHL_NUM];		    // 通道使能标志位， 依次绿光1234-红外1234-红光1234

	int32_t ch_agc_drv0[3 * CHIP_PPG_CHL_NUM];		//  驱动电流0-依次绿光1234-红外1234-红光1234
	int32_t ch_agc_drv1[3 * CHIP_PPG_CHL_NUM];		//  驱动电流1-依次绿光1234-红外1234-红光1234
	int8_t ch_agc_gain[3 * CHIP_PPG_CHL_NUM];	    //  gain值1-依次绿光1234-红外1234-红光1234
	//ch_agc_drv0   ch_agc_drv1  ch_agc_gain
	//int8_t valid_ch_num;                            //实际使用通道数

	int32_t acc_x;				//加速度计x轴
	int32_t acc_y;				//加速度计y轴
	int32_t acc_z;				//加速度计z轴
	int32_t wear_on_flag;		//佩戴检测标志
}goodix_spo2_input_rawdata;

typedef struct
{
	//float32_t spo2[CHIP_PPG_CHL_NUM];				//血氧值
	int32_t spo2[CHIP_PPG_CHL_NUM];				//血氧值*10000
	int32_t valid_level[CHIP_PPG_CHL_NUM];		//置信等级
	int32_t confi_coeff[CHIP_PPG_CHL_NUM];		//置信度
	int32_t WeightR[CHIP_PPG_CHL_NUM];		//置信度

	//HB
	int32_t hb_mean[CHIP_PPG_CHL_NUM];
	int32_t hb_confi_lvl[CHIP_PPG_CHL_NUM];

	int32_t invalidFlg[CHIP_PPG_CHL_NUM];
	int32_t piIR[CHIP_PPG_CHL_NUM];
	int32_t piRed[CHIP_PPG_CHL_NUM];
	int32_t snrIR[CHIP_PPG_CHL_NUM];
	int32_t snrRed[CHIP_PPG_CHL_NUM];
	int32_t r_val[CHIP_PPG_CHL_NUM];				//r值（*10000）
	int32_t calc_flg[CHIP_PPG_CHL_NUM];

	//**********************四通道输出最终值***************************//
	int32_t final_spo2;
	int32_t final_valid_level;
	int32_t final_confi_coeff;
	int32_t final_WeightR;

	int32_t final_hb_mean;
	int32_t final_hb_confi_lvl;

	int32_t final_invalidFlg;
	int32_t final_piIR;
	int32_t final_piRed;
	int32_t final_snrIR;
	int32_t final_snrRed;
	int32_t final_r_val;
	int32_t final_calc_flg;
}goodix_spo2_result;

#ifdef __cplusplus
extern "C" {
#endif

	/**
	* @brief SPO2 处理算法版本号
	* @param[in]  version     外部储存版本号空间，分配20字节
	* @param[out] 无
	* @return     函数执行情况
	*/
	DRVDLL_API goodix_spo2_ret goodix_spo2_version(uint8_t version[100]);

	/**
	* @brief SPO2 处理算法初始化
	* @param[in]  fs         PPG和ACC的采样率
	* @param[in]  memaddr    用于算法使用的外部内存首地址
	* @param[in]  memsize	 用于算法使用的外部内存大小
	* @param[out] 无
	* @return     函数执行情况
	*/
	DRVDLL_API goodix_spo2_ret goodix_spo2_init(goodix_spo2_config *cfg);

	/**
	* @brief SPO2 血氧计算
	* @param[in]  raw		输入原始数据结构体
	* @param[out] res		输出结果结构体
	* @return     函数执行情况
	*/
	DRVDLL_API goodix_spo2_ret goodix_spo2_calc(goodix_spo2_input_rawdata* raw, goodix_spo2_result* res);

	/**
	* @brief SPO2 算法销毁，释放内部的内存空间，调用其他模块算法时使用
	* @param[out] 无
	* @return     函数执行情况
	*/
	DRVDLL_API goodix_spo2_ret goodix_spo2_deinit(void);

	DRVDLL_API goodix_spo2_ret goodix_spo2_calc_test_bin(float32_t* pin0, float32_t* pin1, goodix_spo2_result* res);
#ifdef __cplusplus
}
#endif

#endif
