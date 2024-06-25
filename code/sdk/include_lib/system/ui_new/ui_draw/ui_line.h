/**
 * @file ui_line.h
 *
 */

#ifndef UI_LINE_H
#define UI_LINE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "ui_draw/ui_obj.h"
#include "ui_draw/ui_basic.h"
#include "ui_draw/ui_mask.h"
#include "ui_draw/ui_rect.h"
#include "ui_draw/ui_figure.h"

#if UI_USE_LINE != 0


/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Data of line*/
typedef struct {
    /*Inherited from 'base_obj' so no inherited ext.*/ /*Ext. of ancestor*/
    const ui_point_t *point_array;                     /*Pointer to an array with the points of the line*/
    uint16_t point_num;                                /*Number of points in 'point_array' */
    uint8_t auto_size : 1;                             /*1: set obj. width to x max and obj. height to y max */
    uint8_t y_inv : 1;                                 /*1: y == 0 will be on the bottom*/
} ui_line_ext_t;

/*Styles*/
enum {
    UI_LINE_PART_MAIN,
};
typedef uint8_t ui_line_style_t;



typedef struct {

    struct rect coords_rect;//相对背景的位置和范围,不会随着移动而变化

    ui_style_list_t style;

    const ui_point_t *point_array; //在画线框内的点坐标数组，每个点的坐标是相对coords_rect坐标来说的,该数组必须是全局或者静态
    uint16_t point_num;//点的个数, 至少要有两个点

} newline_info_t;



/**********************
 * GLOBAL PROTOTYPES
 **********************/
void draw_newline(const newline_info_t *info, u8 *buf, u32 buflen, struct rect *disp, struct rect *ctrl, int draw_id);

void ui_line_set_style(ui_obj_t *line, ui_style_list_t *style_copy);
/**
 * Create a line objects
 * @param par pointer to an object, it will be the parent of the new line
 * @return pointer to the created line
 */
ui_obj_t *ui_line_create(ui_obj_t *__line, ui_line_ext_t *line_ext, ui_area_t *coords);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set an array of points. The line object will connect these points.
 * @param line pointer to a line object
 * @param point_a an array of points. Only the address is saved,
 * so the array can NOT be a local variable which will be destroyed
 * @param point_num number of points in 'point_a'
 */
void ui_line_set_points(ui_obj_t *line, const ui_point_t point_a[], uint16_t point_num);

/**
 * Enable (or disable) the auto-size option. The size of the object will fit to its points.
 * (set width to x max and height to y max)
 * @param line pointer to a line object
 * @param en true: auto size is enabled, false: auto size is disabled
 */
void ui_line_set_auto_size(ui_obj_t *line, bool en);

/**
 * Enable (or disable) the y coordinate inversion.
 * If enabled then y will be subtracted from the height of the object,
 * therefore the y=0 coordinate will be on the bottom.
 * @param line pointer to a line object
 * @param en true: enable the y inversion, false:disable the y inversion
 */
void ui_line_set_y_invert(ui_obj_t *line, bool en);

#define ui_line_set_y_inv                                                                                              \
    ui_line_set_y_invert /*The name was inconsistent. In v.6.0 only `lv_line_set_y_invert`will                         \
work */

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the auto size attribute
 * @param line pointer to a line object
 * @return true: auto size is enabled, false: disabled
 */
bool ui_line_get_auto_size(const ui_obj_t *line);

/**
 * Get the y inversion attribute
 * @param line pointer to a line object
 * @return true: y inversion is enabled, false: disabled
 */
bool ui_line_get_y_invert(const ui_obj_t *line);

/**********************
 *      MACROS
 **********************/

#endif /*UI_USE_LINE*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*UI_LINE_H*/

