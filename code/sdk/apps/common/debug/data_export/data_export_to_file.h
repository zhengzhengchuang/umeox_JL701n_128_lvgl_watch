
#ifndef __DATA_EXPORT_TO_FILE_H_
#define __DATA_EXPORT_TO_FILE_H_

#include "generic/includes.h"

void data_export_file_close(void *hdl);

void *data_export_file_open(void *path, int ch_num, int buf_len);

int data_export_file_set_ch_name(void *hdl, u8 ch, u8 ch_name);

int data_export_file_write(void *hdl, u8 ch, void *buf, int len);

u8 *data_export_file_write_alloc(void *hdl, u8 ch, int len);

int data_export_file_write_alloc_push(void *hdl, u8 ch, void *buf);

#endif // #__DATA_EXPORT_TO_FILE_H_

