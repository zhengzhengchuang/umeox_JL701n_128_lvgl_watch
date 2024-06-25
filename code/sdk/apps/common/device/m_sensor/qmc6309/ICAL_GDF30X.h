#ifndef ICAL_H_
#define ICAL_H_
//#include <stdio.h>
// Internal Type Definitions
#include <stdint.h>

#if 0
typedef unsigned char	UCHAR;
typedef signed char			INT8;
typedef INT8			int8_t;
typedef unsigned char	UINT8;
typedef UINT8			uint8_t;
typedef short			INT16;
typedef INT16			int16_t;
typedef unsigned short	UINT16;
typedef UINT16			uint16_t;
typedef int			INT32;
typedef INT32			int32_t;
typedef unsigned int	UINT32;
typedef UINT32			uint32_t;
#endif
#if 0
typedef int8_t			INT8;
typedef uint8_t			UINT8;
typedef int16_t			INT16;
typedef uint16_t		UINT16;
typedef int32_t			INT32;
typedef uint32_t		UINT32;
#endif

//typedef float			REAL;
//typedef long long		int64_t;
//typedef int64_t			INT64;
//typedef unsigned long long		uint64_t;
//typedef uint64_t		UINT64;

typedef struct
{
	float	Offset[3];		// Magnetic Hard Iron x, y, z offsets
	float rr;
} TRANSFORM_T;
//#define QST_ICAL_CORE_DEBUG

// #ifdef QST_ICAL_CORE_DEBUG
// #define QST_PRINT(...) do{printf(__VA_ARGS__);}while(0)
// #else
// #define QST_PRINT(...) do{}while(0)
// #endif


/*初始化时调用 ，如下两个函数*/
/*
初始化的时候 ，需要定义 float calipara[4] = {0, 0, 0, 0};数组 ， 然后，在开机的时候，将这个数组值读出来，给函数qst_ical_init() 赋参数；
代码示例：
float calipara[4] = {0, 0, 0, 0};
qst_ical_init(calipara);
fusion_enable();
*/
void qst_ical_init(float *calipara);
void fusion_enable();


/*float input_mag[3]   输入地磁 mag xyz数据，单位是  uT； 
**float input_acc[3]   输入加速度 acc xyz数据, 单位是  9.8m/s2;
**float out_calimag[3] 输出校准后的 mag 数据； 
**float output_offset[4]  输出校准的 offset数据，这个数据需要在校准成功后， 保存到 falsh  或者 NVRAM 区， 供下次开机时读出来使用；
**float output_ori[3]   输出的 ORI 角度值， output_ori[0] 是 yaw 航向角，即指南针方向角度。
**int8_t *accuracy     输出的 校准精度值。 3表示 比较好的校准成功。

代码示例：
float euler_value[3] = {0, 0, 0};
float acc_correct[3] = {0, 0, 0};
float mag_raw[3] = {0, 0, 0}; 
float mag_output[3] = {0, 0, 0}; 
float mag_output_offset[4] = {0, 0, 0,0}; 
int8_t accuracy;
fusion_docali(mag_raw,acc_correct,mag_output, mag_output_offset,euler_value,&accuracy);
*/
void fusion_docali(float input_mag[3],float input_acc[3],float out_calimag[3], float output_offset[4],float output_ori[3],int8_t *accuracy);
//void fusion_docali(float input_mag[3],float input_acc[3], float output_offset[4],float output_ori[3],int8_t *accuracy);
//void fusion_docali(float input_mag[3],float input_acc[3],float out_calimag[3], float output_offset[4]);

void fuison_Get_Ori(float *ori);
int fuison_GetMagAccuracy();

#endif /* ICAL_H_ */
