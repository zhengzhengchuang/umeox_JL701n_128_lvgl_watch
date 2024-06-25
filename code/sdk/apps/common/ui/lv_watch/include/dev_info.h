#ifndef __DEV_INFO_H__
#define __DEV_INFO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "ui_menu.h"

//设备类型
enum
{
    Xring = 0x01,
    Qwatch = 0x02,
    Bracelet = 0x03,
};
typedef uint8_t Dev_Type_t;


//设备型号
enum
{
    Model_MC01 = 0x01,
    Model_QW02 = 0x02,
};
typedef uint8_t Dev_Model_t;

#ifdef __cplusplus
}
#endif

#endif
