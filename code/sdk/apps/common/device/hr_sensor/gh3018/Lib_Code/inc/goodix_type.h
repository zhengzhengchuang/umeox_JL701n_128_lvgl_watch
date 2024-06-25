#ifndef __GOODIX_TYPE
#define __GOODIX_TYPE

// ����ȽϵĹ�����ֵ
#ifndef EPS
#define EPS            0.000001f
#endif

#include "typedef.h"

// ��� lib ��
#if _WIN32 || _WIN64
#define  DRVDLL_API    extern  _declspec(dllexport)
#else
#define  DRVDLL_API
#endif

// �����ض���
#if (_WIN32 || _WIN64)
typedef signed char			int8_t;
typedef short				int16_t;
typedef int					int32_t;
typedef long long			int64_t;
typedef unsigned char		uint8_t;
typedef unsigned short		uint16_t;
typedef unsigned int		uint32_t;
typedef unsigned long long	uint64_t;
#else
#include <stdint.h>
#endif
#ifndef _ARM_MATH_H
typedef float				float32_t;
typedef double				float64_t;
#endif

#ifndef GOODIX_ALGO_H
typedef unsigned char       GU8;    /**< 8bit unsigned integer type */
typedef signed char         GS8;    /**< 8bit signed integer type */
typedef unsigned short      GU16;   /**< 16bit unsigned integer type */
typedef signed short        GS16;   /**< 16bit signed integer type */
typedef int                 GS32;   /**< 32bit signed integer type */
typedef unsigned int        GU32;   /**< 32bit unsigned integer type */
typedef float               GF32;   /**< float type */
typedef double              GD64;   /**< double type */
typedef char                GCHAR;  /**< char type */
typedef unsigned char       GBOOL;
typedef long long           GS64;   /**< 64bit signed integer type */
#endif

//#include <stddef.h>
// #include "string.h"

typedef enum {
	False = 0,
	True = 1,
}goodix_flag_e;

// ��ǰ��������״̬���㷨�ڲ���⡾Ŀǰ��� R �㡿
typedef enum {
	// ���ֵ�Ƿ���Ч
	stage_noneffect_out = 0,

	// Ĭ��ֵ
	stage_default,
	

	// ���ٻָ����
	//stage_former_begin,
	//stage_former_top,
	//stage_former_downing,

	// ����������
	stage_pqrst_p,
	stage_pqrst_q,
	stage_pqrst_r,
	stage_pqrst_j,
	stage_pqrst_j60,
	stage_pqrst_s,
	stage_pqrst_t_onset,
	stage_pqrst_t,
	stage_pqrst_t_offset,
	stage_pqrst_u,
}goodix_stage;

// ECG�źŲ��������־
typedef enum {
	FLAG_NORMAL,				// ECG�źż��Ϊ����
	FLAG_ATRIAL_FIBRILLATION,	// ECG�źż��Ϊ����
	FLAG_OTHER,					// ECG�źż��Ϊ����
	FLAG_NOISE,					// ECG�źż��Ϊ����
}goodix_classification_flag;

#endif // !__GOODIX_TYPE
