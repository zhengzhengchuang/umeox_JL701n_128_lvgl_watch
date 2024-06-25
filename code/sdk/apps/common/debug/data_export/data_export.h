

#ifndef __DATA_EXPORT_H_
#define __DATA_EXPORT_H_

#include "generic/includes.h"

// 回调接口
typedef struct {
    void *priv;
    int (*write)(void *priv, void *buf, int len);
} de_ch_ops;

int data_export_init(void); // 会创建任务"data_export"

void *data_export_ch_create(int buf_len, de_ch_ops *ops);

void data_export_ch_release(void *hdl);

int data_export_ch_write(void *hdl, u8 ch, void *buf, int len);


int data_export_ch_write_alloc_push(void *hdl, u8 *data);
u8 *data_export_ch_write_alloc(void *hdl, u8 ch, int len);

#endif // #ifndef __DATA_EXPORT_H_

