#ifndef __CONVERT_DATA_H
#define __CONVERT_DATA_H

#include "system/includes.h"
#include "media/audio_stream.h"

struct convert_data {
    struct audio_stream_entry entry;	// 音频流入口
    u16 dat_len;
    u16 dat_total;
    u16 buf_len;
    u16 *buf;
    u8 type;
};

struct convert_data *convet_data_open(int type, u32 buf_len);
void convet_data_close(struct convert_data *hdl);

void user_sat16(s32 *in, s16 *out, u32 npoint);

//type
#define CONVERT_32_TO_16 0
#define CONVERT_16_TO_32 1

#endif/*__CONVERT_DATA_H*/
