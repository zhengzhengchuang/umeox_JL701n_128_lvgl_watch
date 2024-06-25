#ifndef __VAD_ALGO_H__
#define __VAD_ALGO_H__

struct avad_config {
    int avad_quantile_p;
    int avad_th;
    int avad_gain_db;
    int avad_compare_v;
};

struct dvad_config {
    int d_low_con_th;
    int d_high_con_th;
    int d2a_th_db;
    int d2a_frame_con;
    int dvad_gain_id;
    int d_frame_con;
    int d_stride1;
    int d_stride2;
};


#define hist_num                 98
#define dvad_frames_updata       128
#define CONT_DD_ADFRMSIZE	     25
#define CONT_DD_ADFRMSIZE1	     30
#define co_1	16
#define co_2	16

typedef int					int32;
typedef short				int16;
typedef signed char			int8;
typedef unsigned int		uint32;
typedef unsigned short		uint16;
typedef unsigned char		uint8;
typedef long long			s64;
typedef unsigned long long  u64;

/**************dingdian%%%%%%%%%%%%%%%*/

typedef struct htk_fe_d {
    unsigned short pow_hist[hist_num];
    unsigned char sum_cnt;
    char speech_leave;
    char sil_leave;

    uint8 d_voice_state;
    char speech_fifo[CONT_DD_ADFRMSIZE1];
    char speech_cnt;
    char vad_cnt;
    int frame_cnt;
    int energy_10frame;
    uint8 speech_indexw1;
    int sp_2_sl;
    int sp_2_s2;
} htk_fe_dvad;


void dvad_Init(short init_noise);
void avad_Init(struct avad_config *cfg);
//int simple_avad(int cnts_l, int cnts_h, int frame_len, struct avad_config *cfg);
int dvad_Run(short *inbuf, int inlen, struct dvad_config *cfg);

#endif /* #ifndef __VAD_ALGO_H__ */
