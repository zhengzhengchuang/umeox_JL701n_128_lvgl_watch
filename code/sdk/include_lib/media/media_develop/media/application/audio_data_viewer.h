/*******************************************************************************************
  File : audio_dig_vol.h
  By   : LinHaibin
  brief: 数据查看节点
  Email: linhaibin@zh-jieli.com
  date : Sat, 19 Mar 2022 09:51:41 +0800
********************************************************************************************/
#ifndef _AUDIO_DATA_VIEWER_H_
#define _AUDIO_DATA_VIEWER_H_

/*******************************************************
* Function name	: audio_data_viewer_open
* Description	: 数据查看节点打开
* Parameter		:
*   @data_callback  回调函数
* Return        : 返回句柄, NULL：打开失败
*******************************************************/
void *audio_data_viewer_open(void (*data_callback)(u8, s16 *, u32));
/*******************************************************
* Function name	: audio_data_viewer_entry_get
* Description	: 获取数据流entry句柄
* Parameter		:
*   @_hdl           数据查看节点句柄
* Return        : 返回句柄, NULL：失败
*******************************************************/
void *audio_data_viewer_entry_get(void *_hdl);
/*******************************************************
* Function name	: audio_data_viewer_close
* Description	: 数据查看节点关闭
* Parameter		:
*   @_hdl       	句柄
* Return        : 0:正常  other:出错
********************************************************/
int audio_data_viewer_close(void *_hdl);

#endif
