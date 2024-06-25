#ifndef __APPLE_NETWORK_H__
#define __APPLE_NETWORK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../../lv_watch.h"

/****************图标源总数****************/
#define Apple_Network_Icon_Num      (37)

/****************网格宽、高度****************/
#define Apple_Network_Container_W   (368)
#define Apple_Network_Container_H   (448)

/****************整型平方****************/
#define Int_Num_Squ(x) ((int32_t)(x*x))

/****************网格缩放后参数内容****************/
typedef struct 
{
    uint16_t zoom_scale;
    lv_point_t zoom_pivot;
}zoom_ctx_t;

/****************网格蜂窝参数内容****************/
typedef struct 
{
    lv_point_t curved_dela;
    lv_point_t curved_offset;
    lv_obj_t *apple_network_container;
    zoom_ctx_t zoom_ctx[Apple_Network_Icon_Num];
    bool apple_network_hidden[Apple_Network_Icon_Num];
    lv_obj_t *apple_network_icon[Apple_Network_Icon_Num];
    lv_point_t apple_network_axial[Apple_Network_Icon_Num];
    lv_point_t apple_network_pivot[Apple_Network_Icon_Num];
}apple_network_ctx_t;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
