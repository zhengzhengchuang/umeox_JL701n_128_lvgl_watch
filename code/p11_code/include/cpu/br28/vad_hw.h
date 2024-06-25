#ifndef __VAD_HW_H__
#define __VAD_HW_H__

#include "vad_algo.h"

void vad_init(struct avad_config *a_cfg, struct dvad_config *d_cfg);

void vad_hw_updata_read_ptr(u32 frames);

void vad_close(void);

#endif /* #ifndef __VAD_HW_H__ */
