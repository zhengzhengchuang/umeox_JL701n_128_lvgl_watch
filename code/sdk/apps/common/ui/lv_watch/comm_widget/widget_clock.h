#ifndef __WIDGET_CLOCK_H__
#define __WIDGET_CLOCK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../lv_watch.h"

/*********************************************************************************
                        1、秒针按秒转动  2、秒针无级转动                                     
*********************************************************************************/
#define Clock_Pointer_Rot (2)

/*********************************************************************************
                        无效角度                                     
*********************************************************************************/
#define Clock_Pointer_Inv_Angle (-1)

/*********************************************************************************
                        指针偏移                                     
*********************************************************************************/
#define Clock_Pointer_Ver_Offset (10)

enum
{
    clk_pointer_type_hour = 0x00,
    clk_pointer_type_minute,
    clk_pointer_type_second,
};
typedef uint8_t clock_pointer_type_t;

typedef struct
{
    int16_t clk_p_width;
    int16_t clk_p_height;
    int16_t clk_p_center;

    uint32_t clk_p_file_dat;

    clock_pointer_type_t clk_p_type;
}common_clock_pointer_para_t;

void common_clock_pointer_refresh(void);
void common_clock_pointer_para_init(void);
void common_clock_pointer_angle_update(void);
void common_clock_pointer_create(lv_obj_t *obj, \
    const common_clock_pointer_para_t *para, uint8_t num);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
