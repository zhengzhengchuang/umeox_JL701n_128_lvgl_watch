#ifndef __GH3011_ALGO_HOOK_H__
#define __GH3011_ALGO_HOOK_H__

#include "gh30x_example_common.h"
#if (__HBD_DOUBLE_CORE_ENABLE__ == 0)
#include "../../Algo_Code/call/inc/goodix_mem.h"
#include "../../Lib_Code/inc/goodix_type.h"
#include "../../Lib_Code/inc/goodix_hba.h"
#include "../../Lib_Code/inc/goodix_spo2.h"
#include "../../Lib_Code/inc/goodix_sys_hrv.h"
#else //(__HBD_DOUBLE_CORE_ENABLE__ == 0)
#define PPG_CHANNEL_NUM		4
#if (__HBD_HB_ALGORITHM_ENABLE__)
//hba algo typedef
typedef enum {
	HBA_SCENES_DEFAULT = 0,				// Ĭ�ϣ����㷨�ڲ�ʶ����

	HBA_SCENES_DAILY_LIFE = 1,			// �ճ�����
	HBA_SCENES_RUNNING_INSIDE = 2,		// �����ܲ���
	HBA_SCENES_WALKING_INSIDE = 3,		// ���ڲ���
	HBA_SCENES_STAIRS = 4,				// ����¥��

	HBA_SCENES_RUNNING_OUTSIDE = 5,		// �����ܲ�
	HBA_SCENES_WALKING_OUTSIDE = 6,		// ���ⲽ��

	HBA_SCENES_STILL_REST = 7,			// ��Ϣ
	HBA_SCENES_REST = 8,				// ��Ϣ
	HBA_SCENES_STILLRADON = 9,			// ����

	HBA_SCENES_BIKING_INSIDE = 10,		//�������г�
	HBA_SCENES_BIKING_OUTSIDE = 11,		//�������г�
	HBA_SCENES_BIKING_MOUNTAIN= 12,		//�������г�ԽҰ
	HBA_SCENES_RUNNING_HIGH_HR = 13,	//�������ܲ�

	HBA_SCENES_RUNNING_TREADMILL_CCOMBINE= 14,		// �ܲ��������

	HBA_SCENES_HIGH_INTENSITY_COMBINE = 15,		// ��ǿ���˶����
	HBA_SCENES_TRADITIONAL_STRENGTH_COMBINE = 16,		// ��ͳ����ѵ�����
	HBA_SCENES_STEP_TEST = 17,		    // ̨�ײ���

	HBA_SCENES_BALL_SPORTS = 18,		// �����˶�
	HBA_SCENES_AEROBICS = 19,		    // ������

    HBA_SCENES_SLEEP = 20,              // ˯�߳���
    HBA_SCENES_JUMP = 21,               //����������LS ROMA��
    HBA_SCENES_CORDLESS_JUMP = 22,	    // ���������������
	HBA_SCENES_SWIMMING = 23,           // ��Ӿ����
    HBA_SCENES_SIZE = 24,               // ������Ŀ
}hba_scenes_e;

typedef enum {
    HBA_TEST_DYNAMIC = 0,		// Ĭ�ϣ���̬����
    HBA_TEST_DOT = 1,			// ���
    HBA_TEST_SENSELESS = 2,		// �޸�ģʽ
}hba_test_mode;

typedef struct {
    hba_test_mode mode;                             // ����ģʽ
	hba_scenes_e scence;							// ����
	GU32 fs;									// ������
	GS32 valid_channel_num;						// ��Чͨ����
	// �����ⲿ�����㷨�����ԵĲ���
    GU32 back_track_len;                       // ���ݵ�ʱ��,Ĭ��30s,���ʱ������Ϊ120s�����ʱ������Ϊ30s
	GS32 hba_latest_output_time;					// ������ֵʱ��
	GS32 hba_earliest_output_time;				// �����ֵʱ��
	GU32 hba_lowerest_confidence;				// ��ͳ�ֵ���Ŷ�
	GU32 hba_out_step_second;					// ��ֵ���
	GU32 hba_convergence_rate;					// ׷�ٹ����е���������
    GU32 senseless_mode_step;                   // �޸м��ʱ��������Ϊ0��ʾδ֪
    GU32 senseless_mode_duration;               // �޸г���ʱ��������Ϊ0��ʾδ֪
}goodix_hba_config;

typedef struct
{
	GU32 frameid;								// ֡���
	GS32 ppg_rawdata[3 * PPG_CHANNEL_NUM];		// PPGԭʼ���ݣ������̹�1234-����1234-���1234
	GS32 cur_adj_flg[3 * PPG_CHANNEL_NUM];		// ���������־λ
	GS32 gain_adj_flg[3 * PPG_CHANNEL_NUM];		// ��������־λ
	GS32 enable_flg[3 * PPG_CHANNEL_NUM];		// ͨ��ʹ�ܱ�־λ

	GS32 acc_x;									// ���ٶȼ�x��
	GS32 acc_y;									// ���ٶȼ�y��
	GS32 acc_z;									// ���ٶȼ�z��

    GU32 sleep_flg;                             //˯��flg
}goodix_hba_input_rawdata;

typedef struct
{
	GU32 hba_out_flag;				// ��ֵ��ǣ�Ϊ 1 ��Ч
	GS32 hba_out;					// ����ֵ
	GF32 hba_confi;				// ���Ŷ�
	GF32 hba_snr;					// �����   : �˲�ȥ�����ź�Ƶ��������������������
    GU32 valid_level;			    // ���ŵȼ� : 0 -> 1 -> 2 ��Խ��Խ�ɿ�
    GU32 valid_score;				// ���ŷ��� : 0->100 Խ��Խ�ɿ�
    GU32 hba_acc_info;              // �˶�״̬ : 0-��Ϣ��С�˶���1-����-���˶���2-�ܲ�-���˶���
    hba_scenes_e hba_acc_scence;            // �˶����� : �ο� hba_scenes_e ö������
	// ÿһ·������ֵ�����ڵ���
	GU32 hba_out_flag_channel[PPG_CHANNEL_NUM];				// ��ֵ��ǣ�Ϊ 1 ��Ч
	GU32 hba_out_channel[PPG_CHANNEL_NUM];					// ����ֵ
	GF32 hba_confi_channel[PPG_CHANNEL_NUM];				// ���Ŷ�
	GF32 hba_snr_channel[PPG_CHANNEL_NUM];					// �����
	GF32 *p_hba_td_filter[PPG_CHANNEL_NUM];				// ʱ���˲����ָ������
	GF32 *p_hba_fd_filter[PPG_CHANNEL_NUM + 1];			// Ƶ��(ά��)�˲����ָ�����飬4·��1·����
}goodix_hba_result;

/**
* @brief HBA �㷨��������״ֵ̬
*/
typedef enum
{
	GX_ALGO_HBA_SUCCESS = 0x00000000,			/**< �ɹ�     */
	GX_ALGO_HBA_RWONG_INPUT = 0x00000001,		/**< �������ݸ�ʽ���Ϸ�     */
	GX_ALGO_HBA_NO_MEMORY = 0x00008000,			/**< �ڴ�ռ䲻��     */
}goodix_hba_ret;
#endif

#if(__HBD_SPO2_ALGORITHM_ENABLE__)
#define CHIP_PPG_CHL_NUM		                4  //оƬ֧�ֵ�ͨ����

typedef struct {
	//raw ������Ϣ
	GU32 valid_chl_num;			// ��Чͨ����
	GU32 raw_fs;			    // ԭʼ������
	
	GS32 cali_coef_a;							//У׼����2����
	GS32 cali_coef_b;							//У׼����1����
	GS32 cali_coef_c;							//У׼����������
	//hbʹ�ܱ�־
	GU32 hb_en_flg;					//hbʹ�ܱ�־
	//���״̬
	GU32 wear_mode;					//���״̬ 0:�ֻ�  1����ָ  2������
	// acc move thr
	GU32 acc_thr_max;
	GU32 acc_thr_min;
	GU32 acc_thr_scale;
	GU32 acc_thr_num;
	
	//CTR����
	GU32 ctr_en_flg;
	GU32 ctr_red_thr;
}goodix_spo2_config;


typedef struct
{
	GU32 frameid;								// ֡���
	GU8 bit_num;                                //����λ��
	GS32 ppg_rawdata[3 * CHIP_PPG_CHL_NUM];		// PPGԭʼ���ݣ������̹�1234-����1234-���1234
	GS8 cur_adj_flg[3 * CHIP_PPG_CHL_NUM];		// ���������־λ�� �����̹�1234-����1234-���1234
	GS8 gain_adj_flg[3 * CHIP_PPG_CHL_NUM];		// ��������־λ�� �����̹�1234-����1234-���1234
	GS8 enable_flg[3 * CHIP_PPG_CHL_NUM];		    // ͨ��ʹ�ܱ�־λ�� �����̹�1234-����1234-���1234

	GS32 ch_agc_drv0[3 * CHIP_PPG_CHL_NUM];		//  ��������0-�����̹�1234-����1234-���1234
	GS32 ch_agc_drv1[3 * CHIP_PPG_CHL_NUM];		//  ��������1-�����̹�1234-����1234-���1234
	GS8 ch_agc_gain[3 * CHIP_PPG_CHL_NUM];	    //  gainֵ1-�����̹�1234-����1234-���1234
	//ch_agc_drv0   ch_agc_drv1  ch_agc_gain
	//GS8 valid_ch_num;                            //ʵ��ʹ��ͨ����

	GS32 acc_x;				//���ٶȼ�x��
	GS32 acc_y;				//���ٶȼ�y��
	GS32 acc_z;				//���ٶȼ�z��
	GS32 wear_on_flag;		//�������־
}goodix_spo2_input_rawdata;

typedef struct
{
	//GF32 spo2[CHIP_PPG_CHL_NUM];				//Ѫ��ֵ
	GS32 spo2[CHIP_PPG_CHL_NUM];				//Ѫ��ֵ*10000
	GS32 valid_level[CHIP_PPG_CHL_NUM];		//���ŵȼ�
	GS32 confi_coeff[CHIP_PPG_CHL_NUM];		//���Ŷ�
	GS32 WeightR[CHIP_PPG_CHL_NUM];		//���Ŷ�

	//HB
	GS32 hb_mean[CHIP_PPG_CHL_NUM];
	GS32 hb_confi_lvl[CHIP_PPG_CHL_NUM];

	GS32 invalidFlg[CHIP_PPG_CHL_NUM];
	GS32 piIR[CHIP_PPG_CHL_NUM];
	GS32 piRed[CHIP_PPG_CHL_NUM];
	GS32 snrIR[CHIP_PPG_CHL_NUM];
	GS32 snrRed[CHIP_PPG_CHL_NUM];
	GS32 r_val[CHIP_PPG_CHL_NUM];				//rֵ��*10000��
	GS32 calc_flg[CHIP_PPG_CHL_NUM];

	//**********************��ͨ���������ֵ***************************//
	GS32 final_spo2;
	GS32 final_valid_level;
	GS32 final_confi_coeff;
	GS32 final_WeightR;

	GS32 final_hb_mean;
	GS32 final_hb_confi_lvl;

	GS32 final_invalidFlg;
	GS32 final_piIR;
	GS32 final_piRed;
	GS32 final_snrIR;
	GS32 final_snrRed;
	GS32 final_r_val;
	GS32 final_calc_flg;
}goodix_spo2_result;


/**
* @brief SPO2 �㷨��������״ֵ̬
*/
typedef enum
{
	GX_ALGO_SPO2_SUCCESS = 0x00000000,			/**< �ɹ�     */
	GX_ALGO_SPO2_RWONG_INPUT = 0x00000001,		/**< �������ݸ�ʽ���Ϸ�     */
	GX_ALGO_SPO2_NO_MEMORY = 0x00000002,        /**< �ڴ�ռ䲻��     */
	GX_ALGO_SPO2_FRAME_UNCOMPLETE = 0x00000003, /**< ���ڽ�����ƽ��������*/
	GX_ALGO_SPO2_WIN_UNCOMPLETE = 0x00000004,   /**< δ����������֡λ�ã�����s��     */
	GX_ALGO_SPO2_UNEXPECTED = 0x00000005,      /*��������߼��쳣 */
}goodix_spo2_ret;

#endif

#if(__HBD_HRV_ALGORITHM_ENABLE__)
#define HRV_CHNL_MAX_NUM 6
typedef struct
{
	GS32 ppg_green[HRV_CHNL_MAX_NUM];
	GS32 green_cur_adj_flag[HRV_CHNL_MAX_NUM];
	GS32 green_gain_adj_flag[HRV_CHNL_MAX_NUM];
	GS32 acc_x;
	GS32 acc_y;
	GS32 acc_z;
	GS32 frame_id;
	GS32 hr;
}ST_HRV_INPUT_INFO;


/* ���ش����� */
#define GX_HRV_ALGO_OK					((GU32)0x00000000) // �ɹ����أ����޳�ֵˢ��
#define GX_HRV_ALGO_FAILED				((GU32)0x10000001) // ʧ�ܷ���
#define GX_HRV_ALGO_NULL_PTR			((GU32)0x10000002) // �ⲿ�����ָ��Ϊ��
#define GX_HRV_ALGO_INVALID_PARAM		((GU32)0x10000003) // ������Χ��Ч
#define GX_HRV_ALGO_OUT_OF_MEM			((GU32)0x10000004) // �ڴ����ʧ��
#define GX_HRV_ALGO_DEINIT_ABORT		((GU32)0x10000005) // �ͷ�ʧ��
#define GX_HRV_ALGO_UPDATE              ((GU32)0x10000006) // �г�ֵˢ��
#define PPG_CHANNEL_NUM		            4
#define RRI_NUM                         4
#define ACC_THR_NUM                     4

typedef struct {
    GS32 need_ipl;                                   // �Ƿ���Ҫ��ֵ��1KHz
    GS32 fs;                                         // ������
    GS32 acc_thr[ACC_THR_NUM];                       // acc �����ֵ��Ĭ��ֵΪ��20/10/3/3
}goodix_hrv_config;
typedef struct {
    GU8  bit_num;
    GS32 ppg_rawdata[PPG_CHANNEL_NUM];                 // PPGԭʼ�źţ������ͨ��
    GS32 rawdata_cur_adj_flag[PPG_CHANNEL_NUM];        // ���������־λ
    GS32 rawdata_gain_adj_flag[PPG_CHANNEL_NUM];       // ��������־λ
    GS32 acc_x;                                      // ���ټ� x ��
    GS32 acc_y;                                      // ���ټ� y ��
    GS32 acc_z;                                      // ���ټ� z ��
    GS32 frame_id;                                   // ֡���
    GS32 hr;                                         // ��ǰ����ֵ
}goodix_hrv_input_rawdata;

typedef struct {
    GS32 rri[RRI_NUM];                               // RRI�������
    GS32 rri_confidence;                             // ������Ŷ�
    GS32 rri_valid_num;                              // RRI��Ч�������
}goodix_hrv_result;
#endif
#endif

#if(__HBD_NADT_ALGORITHM_ENABLE__)
#define NADT_CONFIG_SOFT_AUTOLED_TYPE            0
#define NADT_CONFIG_TURNLIGHT_TYPE               1
#define NADT_CONFIG_UNWEAR_TIME_TYPE             2
#define NADT_CONFIG_DETECT_TIMEOUT_TYPE          3
#define NADT_CONFIG_SAMPLE_RATE_TYPE             4
#define NADT_CONFIG_SLEEP_STATUS_TYPE            5
#define NADT_COFIG_UNWEAR_LEVEL_TYPE             6
#define NADT_CONFIG_LIVE_DETECT_EN_TYPE          7
#define NADT_CONFIG_HB_LOW_THR_TYPE              8
#define NADT_CONFIG_HB_HIGH_THR_TYPE             9
#define NADT_CONFIG_GREEN_SIG_LOW_THR_TYPE       10
#define NADT_CONFIG_GREEN_SIG_HIGH_THR_TYPE      11
#define NADT_CONFIG_BASE_RATIO_EN_TYPE           12
#define NADT_CONFIG_BASE_RATIO_THR_TYPE          13
#define NADT_CONFIG_LIVE_CONFIRM_EN_TYPE         14
#define NADT_CONFIG_IR_SIG_LOW_THR_TYPE          15
#define NADT_CONFIG_IR_SIG_HIGH_THR_TYPE         16
#define NADT_CONFIG_CTR_EN_TYPE                  17
#define NADT_CONFIG_CTR_THR_TYPE                 18
#define NADT_CONFIG_GAIN_EN_TYPE                 19
#define NADT_CONFIG_GAIN_STABLE_TIME_TYPE        20
#define NADT_CONFIG_GREEN_GAIN_THR_TYPE          21
#define NADT_CONFIG_IR_GAIN_THR_TYPE             22
#define NADT_CONFIG_PERIOD_EN_TYPE               23
#define NADT_CONFIG_PERIOD_LOW_THR_TYPE          24
#define NADT_CONFIG_PERIOD_HIGH_THR_TYPE         25
#define NADT_CONFIG_PERIOD_DIFF_THR_TYPE         26
#define NADT_CONFIG_TOTAL_CNT_TYPE               27
#define NADT_CONFIG_UNWEAR_CNT_TYPE              28
#define NADT_CONFIG_ADT_ONLY_SLEEP_TYPE          29
#define NADT_CONFIG_ADT_WEAR_OFF_THR_TYPE        30
#define NADT_CONFIG_ADT_WEAR_OFF_CNT_TYPE        31

#define NADT_CONFIG_ONE_OFFSET             1
#define NADT_CONFIG_TWO_OFFSET             2

typedef struct
{
    GS32 nSleepInState;
} ST_NADT_CONFIG;
#endif

#if (__HB_NEED_ADT_CONFIRM__)
typedef struct
{
	const GF32 *pfCoefB;
	const GF32 *pfCoefA;
	GF32 *pfXbuff;
	GF32 *pfYbuff;
	GS32 lLen;
	GF32 fThr;
} ST_IIR_PARAM;
#endif

/* from hba lib. */
extern GS32 goodix_mem_init(void* mem_addr, GS32 size);
extern void goodix_mem_deinit(void);

#if(__HBD_HB_ALGORITHM_ENABLE__)
extern goodix_hba_ret goodix_hba_version(GU8 version[100]);
extern goodix_hba_ret goodix_hba_init(goodix_hba_config const *cfg);
extern goodix_hba_ret goodix_hba_deinit(void);
extern goodix_hba_ret goodix_hba_update(goodix_hba_input_rawdata* raw, goodix_hba_result *res);
#endif
#if(__HBD_SPO2_ALGORITHM_ENABLE__)
extern goodix_spo2_ret goodix_spo2_version(GU8 version[100]);
extern goodix_spo2_ret goodix_spo2_init(goodix_spo2_config *cfg);
extern goodix_spo2_ret goodix_spo2_calc(goodix_spo2_input_rawdata* raw, goodix_spo2_result* res);
extern goodix_spo2_ret goodix_spo2_deinit(void);
#endif
#if(__HBD_NADT_ALGORITHM_ENABLE__)
extern void NADT_ProcInit(void);
extern void NADT_ProcDeInit(void);
extern GBOOL NADT_Proc(GS32 lPacketInfoArr[], GU8 lResult[]);
extern void NADT_Control(GS32 lOptTye, GS32 lConfigValue[]);
extern GS8* NADT_GetVersion(void);
#endif
#if(__HBD_HRV_ALGORITHM_ENABLE__)
extern GU8 *goodix_hrv_version(void);
extern GU32 goodix_hrv_init(goodix_hrv_config *config);
extern GU32 goodix_hrv_calc(goodix_hrv_input_rawdata *input_data, goodix_hrv_result *output_data);
extern GU32 goodix_hrv_deinit(void);
#endif

#if(__HBD_NADT_ALGORITHM_ENABLE__)
/****************************************************************
* Description: Get Nadt version
* Input:    None,
* Return:  Nadt version
******************************************************************/

#if (__HBD_ALGORITHM_EXTERNANL_CONFIG_ENABLE__)
/****************************************************************
* Description: Set NADT 
* Input:  nAdtOnlyEnable 0:ADT and NADT in sleep mode
                          1:ADT only in sleep mode   
******************************************************************/
void HBD_AdtOnlySleepTypeEnable(GS32 nAdtOnlyEnable);

/****************************************************************
* Description: Set NADT 
* Input:  ST_NADT_CONFIG stConfig  config struct for NADT
******************************************************************/
void NADT_ParamConfigure(ST_NADT_CONFIG stConfig);
#endif

#endif

#if(__HBD_HB_ALGORITHM_ENABLE__)
/****************************************************************
* Description: Get hba algorithm version
* Input:    None,
* Return: library hba algorithm version
******************************************************************/
GS8 * HBD_GetHbaVersion (void);

#if (__HBD_ALGORITHM_EXTERNANL_CONFIG_ENABLE__)
/****************************************************************
* Description: Hb algorithm scenario config 
* Input:    uchScenario: 0~24,see hba_scenes_e
                         others: fixed 0(default) and return HBD_RET_PARAMETER_ERROR
* Return: HBD_RET_OK=success, 
          HBD_RET_PARAMETER_ERROR=paramters error,
******************************************************************/
GS8 HBD_HbAlgoScenarioConfig (hba_scenes_e uchScenario);

/****************************************************************
* Description: Hb algorithm test mode config ,before HBD_HbDetectStart
* Input:    emHbaTestMode: test mode, see hba_test_mode
            usSenselessModeStep
            usSenselessModeDuration
******************************************************************/
void HBD_HbaTestModeConfig (hba_test_mode emHbaTestMode, GU16 usSenselessModeStep, GU16 usSenselessModeDuration);

/****************************************************************
* Description: get current Hb algorithm test mode
* Input:    emCurHbaTestMode: test mode, see EM_HBA_TEST_MODE
******************************************************************/
GU8 HBD_GetHbaTestMode (void);

/****************************************************************
* Description: Hb algorithm output time config, only nonFast mode valid 
* Input:    nHbaLatestOutputTime
            nHbaEarliestOutputTime
* Return: HBD_RET_OK=success, 
******************************************************************/
GS8 HBD_HbAlgoOutputTimeConfig (GS32 nHbaLatestOutputTime, GS32 nHbaEarliestOutputTime);
#endif
#endif

/****************************************************************
* Description: Hb algorithm sleep flag config,before hb calculate     
* Input:    uchSleepFlg: 0:not sleep; 1:sleep
******************************************************************/
void HBD_HbaSleepFlagConfig(GU8 uchSleepFlg);

/****************************************************************
* Description: Hb algorithm get sleep flag
* Return:    guchSleepFlag: 0:not sleep; 1:sleep
******************************************************************/
GU8 HBD_HbaGetSleepFlag(void);

#if(__HBD_HRV_ALGORITHM_ENABLE__)
/****************************************************************
* Description: Get hrv algorithm version
* Input:    None,
* Return: library hrv algorithm version
******************************************************************/
GS8 * HBD_GetHrvVersion(void);
#endif

#if(__HBD_SPO2_ALGORITHM_ENABLE__)
/****************************************************************
* Description: Get SPO2 algorithm version
* Input:    None,
* Return: library SPO2 algorithm version
******************************************************************/
GU8 * HBD_GetSpo2Version (void);
#endif

#if (__HB_NEED_ADT_CONFIRM__)

void IIRFilter2Init(ST_IIR_PARAM *pstIIRInfo, const GF32 *pfCoefA, const GF32 *pfCoefB, GF32 *pfXbuff, GF32 *pfYbuff, GS32 lLen, GF32 fThr);
GF32 IIRFilterEx(ST_IIR_PARAM *pstIIRInfo, GF32 fDataIn);

/****************************************************************
* Description: adt confrim init
* Input:  ulNewAdtPpgThrVal : adt ppg thr val
* Return: None
******************************************************************/
void ADTConfirmAlgoInit(GU32 ulNewAdtPpgThrVal);

/****************************************************************
* Description: adt confrim calc
* Input:  *plRawdataBuff : rawdata bufferr
* Return: wear flag, 0x00: ThrCntMax error 0x11:wear, 0x12:unwear
******************************************************************/
GU8 ADTConfirmAlgoCalc(GS32 *plRawdataBuff);

/****************************************************************
* Description: config adt confrim
* Input:  usAdtConfirmGsThrVal : gsensor confirm thr
          uchAdtConfirmGsCalcThrCntMax: gsensor confirm thr cnt max 
          uchAdtConfirmGsCalcThrCnt  : gsensor confirm thr cnt
* Return: None
******************************************************************/
void HBD_AdtConfirmConfig(GU16 usAdtConfirmGsThrVal, GU8 uchAdtConfirmGsCalcThrCntMax, GU8 uchAdtConfirmGsCalcThrCnt);
#endif
#endif
