
#ifndef UI_LINEMETER_H
#define UI_LINEMETER_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "ui_draw/ui_obj.h"

#if UI_USE_LINEMETER != 0

/*********************
 *      DEFINES
 *********************/
#define UI_LINEMETER_PRECISE			1

/**********************
 *      TYPEDEFS
 **********************/
/*Data of line meter*/
typedef struct {
    /*No inherited ext.*/ /*Ext. of ancestor*/
    /*New data for this type */
    uint16_t scale_angle; /*Angle of the scale in deg. (0..360)*/
    uint16_t angle_ofs;
    uint16_t line_cnt;     /*Count of lines */
    int16_t lvl_ofs;
    int32_t cur_value;
    int32_t min_value;
    int32_t max_value;
    uint8_t mirrored : 1;
} ui_linemeter_ext_t;

/*Styles*/
enum {
    UI_LINEMETER_PART_MAIN,
    _UI_LINEMETER_PART_VIRTUAL_LAST,
    _UI_LINEMETER_PART_REAL_LAST,
};
typedef uint8_t ui_linemeter_part_t;




typedef struct {

    struct rect coords_rect;//相对背景的位置和范围,不会随着移动而变化

    ui_style_list_t style;

    int32_t min_value;//一般0~100,默认是0，可以根据需要调整
    int32_t max_value;//一般0~100,默认是100，可以根据需要调整,例如一分钟60秒，那么可以设置为60就可以达到匀速变化
    int32_t cur_value;//min_value ~ max_value
    uint16_t angle_ofs;//起始角度，默认是0
    uint16_t line_cnt; //根数不能太多，不然刷新不过来!!! 最大80, 默认18
    uint16_t scale_angle; /*(0~360)*///总的角度，默认240

} linemeter_info_t;



/**********************
 * GLOBAL PROTOTYPES
 **********************/

//对外接口
void draw_linemeter(const linemeter_info_t *info, u8 *buf, u32 buflen, struct rect *disp, struct rect *ctrl, int draw_id);




/****************************************以下是内部使用接口*********************************************/

/**
 * Create a line meter objects
 * @param par pointer to an object, it will be the parent of the new line meter
 * @param copy pointer to a line meter object, if not NULL then the new object will be copied from
 * it
 * @return pointer to the created line meter
 */
ui_obj_t *ui_linemeter_create(ui_obj_t *lmeter, ui_linemeter_ext_t *lext, ui_area_t *coords);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new value on the line meter
 * @param lmeter pointer to a line meter object
 * @param value new value
 */
void ui_linemeter_set_value(ui_obj_t *lmeter, int32_t value);

/**
 * Set minimum and the maximum values of a line meter
 * @param lmeter pointer to he line meter object
 * @param min minimum value
 * @param max maximum value
 */
void ui_linemeter_set_range(ui_obj_t *lmeter, int32_t min, int32_t max);

/**
 * Set the scale settings of a line meter
 * @param lmeter pointer to a line meter object
 * @param angle angle of the scale (0..360)
 * @param line_cnt number of lines
 */
void ui_linemeter_set_scale(ui_obj_t *lmeter, uint16_t angle, uint16_t line_cnt);

/**
 * Set the set an offset for the line meter's angles to rotate it.
 * @param lmeter pointer to a line meter object
 * @param angle angle offset (0..360), rotates clockwise
 */
void ui_linemeter_set_angle_offset(ui_obj_t *lmeter, uint16_t angle);

/**
 * Set the orientation of the meter growth, clockwise or counterclockwise (mirrored)
 * @param lmeter pointer to a line meter object
 * @param mirror mirror setting
 */
void ui_linemeter_set_mirror(ui_obj_t *lmeter, bool mirror);
void ui_linemeter_set_style(ui_obj_t *lmeter, ui_style_list_t *style_copy);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the value of a line meter
 * @param lmeter pointer to a line meter object
 * @return the value of the line meter
 */
int32_t ui_linemeter_get_value(const ui_obj_t *lmeter);

/**
 * Get the minimum value of a line meter
 * @param lmeter pointer to a line meter object
 * @return the minimum value of the line meter
 */
int32_t ui_linemeter_get_min_value(const ui_obj_t *lmeter);

/**
 * Get the maximum value of a line meter
 * @param lmeter pointer to a line meter object
 * @return the maximum value of the line meter
 */
int32_t ui_linemeter_get_max_value(const ui_obj_t *lmeter);

/**
 * Get the scale number of a line meter
 * @param lmeter pointer to a line meter object
 * @return number of the scale units
 */
uint16_t ui_linemeter_get_line_count(const ui_obj_t *lmeter);

/**
 * Get the scale angle of a line meter
 * @param lmeter pointer to a line meter object
 * @return angle of the scale
 */
uint16_t ui_linemeter_get_scale_angle(const ui_obj_t *lmeter);

/**
 * Get the offset for the line meter.
 * @param lmeter pointer to a line meter object
 * @return angle offset (0..360)
 */
uint16_t ui_linemeter_get_angle_offset(ui_obj_t *lmeter);

void ui_linemeter_draw_scale(ui_obj_t *lmeter, const ui_area_t *clip_area, uint8_t part);

/**
 * get the mirror setting for the line meter
 * @param lmeter pointer to a line meter object
 * @return mirror (true or false)
 */
bool ui_linemeter_get_mirror(ui_obj_t *lmeter);


/****************************************内部使用接口 end*********************************************/



/**********************
 *      MACROS
 **********************/

#endif /*UI_USE_LINEMETER*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*UI_LINEMETER_H*/

