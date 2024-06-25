#include "gSensor/gSensor_manage.h"
#include "system/includes.h"
#include <stdlib.h>
#include "math.h"

#define FILTER_CNT			4

typedef struct filter_avg {
    axis_info_t info[FILTER_CNT];
    unsigned char count;
} filter_avg_t;

//extern G_SENSOR_INTERFACE *gSensor_hdl;
//读取xyz数据存入均值滤波器，存满进行计算，滤波后样本存入sample,如何读取存满就不多说了。
static void filter_calculate(filter_avg_t *filter, axis_info_t *sample)
{
    unsigned int i;
    short x_sum = 0, y_sum = 0, z_sum = 0;
    for (i = 0; i < FILTER_CNT; i++) {
        x_sum += filter->info[i].x;
        y_sum += filter->info[i].y;
        z_sum += filter->info[i].z;
    }
    sample->x = x_sum / FILTER_CNT;
    sample->y = y_sum / FILTER_CNT;
    sample->z = z_sum / FILTER_CNT;
}
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define SAMPLE_SIZE   50

typedef struct {
    axis_info_t newmax;
    axis_info_t newmin;
    axis_info_t oldmax;
    axis_info_t oldmin;
} peak_value_t;

static void peak_value_init(peak_value_t *peak)
{
    peak->newmax.x = -1 ;
    peak->newmax.y = -1 ;
    peak->newmax.z = -1 ;

    peak->newmin.x = 32767 ;
    peak->newmin.y = 32767 ;
    peak->newmin.z = 32767 ;
}


//在动态阈值结构体初始化时，一定要将max的值都赋值为最小值，min赋值为最大值，这样才有利于动态更新。
static void peak_update(peak_value_t *peak, axis_info_t *cur_sample)
{
    static unsigned int sample_size = 0;
    sample_size ++;
    if (sample_size > SAMPLE_SIZE) {
        /*采样达到50个，更新一次*/
        sample_size = 1;
        peak->oldmax = peak->newmax;
        peak->oldmin = peak->newmin;
        //初始化
        peak_value_init(peak);
        //printf("oldmax = %d %d %d \r\n" ,peak->oldmax.x , peak->oldmax.y ,peak->oldmax.z) ;
        //printf("oldmin = %d %d %d \r\n" ,peak->oldmin.x , peak->oldmin.y ,peak->oldmin.z) ;

    }
    peak->newmax.x = MAX(peak->newmax.x, cur_sample->x);
    peak->newmax.y = MAX(peak->newmax.y, cur_sample->y);
    peak->newmax.z = MAX(peak->newmax.z, cur_sample->z);

    peak->newmin.x = MIN(peak->newmin.x, cur_sample->x);
    peak->newmin.y = MIN(peak->newmin.y, cur_sample->y);
    peak->newmin.z = MIN(peak->newmin.z, cur_sample->z);
    //printf("newmax = %d %d %d \r\n" ,peak->newmax.x , peak->newmax.y ,peak->newmax.z) ;
    //printf("newmin = %d %d %d \r\n" ,peak->newmin.x , peak->newmin.y ,peak->newmin.z) ;

}

#define ABS(a) (0 - (a)) > 0 ? (-(a)) : (a)
#define DYNAMIC_PRECISION     			30     	 /*动态精度*/

/*一个线性移位寄存器，用于过滤高频噪声*/
typedef struct slid_reg {
    axis_info_t new_sample;
    axis_info_t old_sample;
} slid_reg_t;

static char slid_update(slid_reg_t *slid, axis_info_t *cur_sample)
{
    char res = 0;
    if (ABS((cur_sample->x - slid->new_sample.x)) > DYNAMIC_PRECISION) {
        slid->old_sample.x = slid->new_sample.x;
        slid->new_sample.x = cur_sample->x;
        res = 1;
    } else {
        slid->old_sample.x = slid->new_sample.x;
    }
    if (ABS((cur_sample->y - slid->new_sample.y)) > DYNAMIC_PRECISION) {
        slid->old_sample.y = slid->new_sample.y;
        slid->new_sample.y = cur_sample->y;
        res = 1;
    } else {
        slid->old_sample.y = slid->new_sample.y;
    }

    if (ABS((cur_sample->z - slid->new_sample.z)) > DYNAMIC_PRECISION) {
        slid->old_sample.z = slid->new_sample.z;
        slid->new_sample.z = cur_sample->z;
        res = 1;
    } else {
        slid->old_sample.z = slid->new_sample.z;
    }
    return res;
}
#define MOST_ACTIVE_NULL      			0      	 /*未找到最活跃轴*/
#define MOST_ACTIVE_X					1		 /*最活跃轴X*/
#define MOST_ACTIVE_Y					2        /*最活跃轴Y*/
#define MOST_ACTIVE_Z					3        /*最活跃轴Z*/

#define ACTIVE_PRECISION      			30//60       /*活跃轴最小变化值*/

/*判断当前最活跃轴*/
static char is_most_active(peak_value_t *peak)
{
    char res = MOST_ACTIVE_NULL;
    short x_change = ABS((peak->newmax.x - peak->newmin.x));
    short y_change = ABS((peak->newmax.y - peak->newmin.y));
    short z_change = ABS((peak->newmax.z - peak->newmin.z));

    if (x_change > y_change && x_change > z_change && x_change >= ACTIVE_PRECISION) {
        res = MOST_ACTIVE_X;
    } else if (y_change > x_change && y_change > z_change && y_change >= ACTIVE_PRECISION) {
        res = MOST_ACTIVE_Y;
    } else if (z_change > x_change && z_change > y_change && z_change >= ACTIVE_PRECISION) {
        res = MOST_ACTIVE_Z;
    }
    return res;
}

/*判断是否走步*/
static int  detect_step(peak_value_t *peak, slid_reg_t *slid, axis_info_t *cur_sample)
{
    static u32 step_cnt = 0 ;
    char res = is_most_active(peak);
    switch (res) {
    case MOST_ACTIVE_NULL: {
        //fix
        break;
    }
    case MOST_ACTIVE_X: {
        short threshold_x = (peak->oldmax.x + peak->oldmin.x) / 2;
        if (slid->old_sample.x > threshold_x && slid->new_sample.x < threshold_x) {
            step_cnt ++;
        }
        break;
    }
    case MOST_ACTIVE_Y: {
        short threshold_y = (peak->oldmax.y + peak->oldmin.y) / 2;
        if (slid->old_sample.y > threshold_y && slid->new_sample.y < threshold_y) {
            step_cnt ++;
        }
        break;
    }
    case MOST_ACTIVE_Z: {
        short threshold_z = (peak->oldmax.z + peak->oldmin.z) / 2;
        if (slid->old_sample.z > threshold_z && slid->new_sample.z < threshold_z) {
            step_cnt ++;
        }
        break;
    }
    default:
        break;
    }
    return step_cnt ;
}
static void get_sample(axis_info_t *sample)
{
#define PI  (float)3.1415926
#define SAMPLE_RATE  50
#if 0
    static int i = 0 ;
    float ampX = 800;
    float ampY = 40;
    float ampZ = 40;

    int frex = 1 ;
    int frey = 10 ;
    int frez = 15 ;

    int offsetX = 30 ;
    int offsetY = 0 ;
    int offsetZ = 0 ;

    i++ ;
    float a = (float)(i % (SAMPLE_RATE / frex)) / (SAMPLE_RATE / frex)  ;
    float b = (float)(i % (SAMPLE_RATE / frey)) / (SAMPLE_RATE / frey)  ;
    float c = (float)(i % (SAMPLE_RATE / frez)) / (SAMPLE_RATE / frez) ;
    sample->x = offsetX + ampX * sinf((float)PI  *  a) + rand() % (int)ampX / 10;
    sample->y = offsetY + ampY * sinf((float)PI  *  b) + rand() % (int)ampY / 10;
    sample->z = offsetZ + ampZ * sinf((float)PI  *  c) + rand() % (int)ampZ / 10;
    //printf("%d ,%d ,%d\r\n", sample->x ,sample->y ,sample->z  );
#else
    //gSensor_hdl->gravity_sensor_ctl(GET_ACCEL_DATA, sample);
    /* printf("%d ,%d ,%d\r\n", sample->x ,sample->y ,sample->z); */
#endif
}

static slid_reg_t slid ;
static filter_avg_t filter ;
static axis_info_t   sample, sample_tmp;
static peak_value_t  peak;

//采样频率50Hz
static int a = 0 ;
static int step = 0  ;

void step_cal_init()
{
    peak_value_init(&peak) ;
    memset(&slid, 0, sizeof(slid)) ;
    memset(&filter, 0, sizeof(filter)) ;
}

int step_cal()
{
    //  printf("Hello world!\n");


    /* int loop = 1000 ; */
    //while(loop--)
    /* for (int i  = 0 ; i < loop  ; i++) { */
    get_sample(&sample_tmp) ;
    filter.info[a] = sample_tmp ;
    a++ ;
    if (a >= FILTER_CNT) {
        a = 0 ;
    }
    //printf("%d %d %d \r\n" ,filter.info[a].x, filter.info[a].y ,filter.info[a].z) ;

    filter_calculate(&filter, &sample) ;
    /* printf("%d %d %d \r\n", sample.x, sample.y, sample.z) ; */

    peak_update(&peak, &sample) ;
    slid_update(&slid, &sample) ;

    int b = detect_step(&peak, &slid, &sample) ;
    if (b != step) {
        printf("step =  %d \r\n", step) ;
        step = b ;
    }
    /* } */
    return 0;
}

