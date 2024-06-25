#ifndef __IMG_H__
#define __IMG_H__

#include "generic/typedef.h"

/*----------------------------------------------------------------------------*/
/**@brief    img输入配置
   @param    data_source : DATA_SRC_FLASH / DATA_SRC_SRAM / DATA_SRC_PSRAM
   @param    data_type   : DATA_NOZIP / DATA_ZIP
   @param    src_w : 图像宽度
   @param    src_h : 图像高度
   @param    format : 图像格式
   @param    tab : 地址映射表(对应DATA_SRC_FLASH选项)
   @param    offset : 4k地址空间里的偏移(DATA_SRC_FLASH), sram/psram地址(DATA_SRC_SRAM/DATA_SRC_PSRAM)
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
int img_set_input(enum DATA_SRC data_source, enum DATA_TYPE data_type, int src_w, int src_h, enum LAYER_FORMAT format, u32 *tab, u32 offset);

/*----------------------------------------------------------------------------*/
/**@brief    img输出配置
   @param    out_addr : 解压后数据存放地址(解压后的图像每行像素字节数4对齐,注意预留足够的buffer空间)
   @param    xoffset : 截取x坐标(相对于图片)
   @param    yoffset : 截取y坐标(相对于图片)
   @param    des_w : 截取宽度
   @param    des_h : 截取高度
   @return
   @note     可以解压图片指定区域，解压后的图像每行像素字节数4对齐
*/
/*----------------------------------------------------------------------------*/
int img_set_output(u8 *out_addr, int xoffset, int yoffset, int des_w, int des_h);

/*----------------------------------------------------------------------------*/
/**@brief    启动img解压(非阻塞)
   @param    void
   @return   void
   @note
*/
/*----------------------------------------------------------------------------*/
void img_kick_async();

/*----------------------------------------------------------------------------*/
/**@brief    等待img解压完成
   @param    void
   @return   void
   @note    跟img_kick_async接口配合使用
*/
/*----------------------------------------------------------------------------*/
void img_kick_async_wait_complete();

/*----------------------------------------------------------------------------*/
/**@brief   启动img解压(阻塞)
   @param   void
   @return  void
   @note
*/
/*----------------------------------------------------------------------------*/
void img_kick_sync();

/*----------------------------------------------------------------------------*/
/**@brief   img运行状态
   @param   void
   @return  0:停止运行 1:正在运行
   @note
*/
/*----------------------------------------------------------------------------*/
u8 img_run();

#endif

