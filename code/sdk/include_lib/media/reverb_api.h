#ifndef reverb_api_h__
#define reverb_api_h__

#include "media/audio_stream.h"
typedef struct REVERBN_PARM_SET {
    int dry;             // 0-200%
    int wet;             //0-300%
    int delay;              //0-100ms
    int rot60;             //100-15000ms
    int Erwet;            // 5%- 400%
    int Erfactor;         // 50%-250%
    int Ewidth;           // -100% - 100%
    int Ertolate;          // 0- 300%
    int Erbasslpf;         //0-18k
    int ErbassB;           //-100% - 100%
    int predelay;         // 0- 20ms
    int allpassfeedback;             //0% - 100%
    int diffusion;         //0% - 100%
    int dampinglpf;       //0-18k
    int basslpf;          //0-1.1k
    int bassB;           // 0-80%
    int lowcut;
} REVERBN_PARM_SET;


/*open 跟 run 都是 成功 返回 RET_OK，错误返回 RET_ERR*/
/*魔音结构体*/
typedef struct __REVERBN_FUNC_API_ {
    unsigned int (*need_buf)(unsigned int *ptr, REVERBN_PARM_SET *reverb_parm, int sr);
    int (*open)(unsigned int *ptr, REVERBN_PARM_SET *reverb_parm, int sr);
    int (*init)(unsigned int *ptr, REVERBN_PARM_SET *reverb_parm);
    int (*run)(unsigned int *ptr, short *inbuf, short *outdata, int len);
} REVERBN_FUNC_API;

typedef struct _REVERBN_API_STRUCT_ {
    // REVERBN_PARM_SET*reverb_parm_obj;    //参数
    REVERBN_PARM_SET parm;    //参数
    REVERBN_PARM_SET target_parm;    //目标参数
    unsigned int *ptr;                  //运算buf指针
    REVERBN_FUNC_API *func_api;           //函数指针
    struct audio_stream_entry entry;	// 音频流入口
    int out_len;
    int process_len;
    s16 *out_buf;
    u8 run_en;
    u8 update;
    int sr;
} REVERBN_API_STRUCT;

extern REVERBN_FUNC_API *get_reverbn_func_api();



#if 1

typedef struct _EF_ECHO__PARM_ {
    unsigned int delay;                      //回声的延时时间 0-300ms
    unsigned int decayval;                   // 0-70%
    unsigned int filt_enable;                //滤波器使能标志
    unsigned int lpf_cutoff;                 //0-20k
    unsigned int wetgain;                     //0-200%
    unsigned int drygain;                     //0-100%
} ECHO_PARM_SET;



typedef struct  _EF_REVERB_FIX_PARM {
    unsigned int sr;
    unsigned int max_ms;
} EF_REVERB_FIX_PARM;



/*open 跟 run 都是 成功 返回 RET_OK，错误返回 RET_ERR*/
/*魔音结构体*/
typedef struct __ECHO_FUNC_API_ {
    unsigned int (*need_buf)(unsigned int *ptr, EF_REVERB_FIX_PARM *echo_fix_parm);
    int (*open)(unsigned int *ptr, ECHO_PARM_SET *echo_parm, EF_REVERB_FIX_PARM *echo_fix_parm);
    int (*init)(unsigned int *ptr, ECHO_PARM_SET *echo_parm);
    int (*run)(unsigned int *ptr, short *inbuf, short *outdata, int len);
    void (*reset_wetdry)(unsigned int *ptr, int wetgain, int drygain);  //新增
} ECHO_FUNC_API;

typedef struct _EHCO_API_STRUCT_ {
    ECHO_PARM_SET echo_parm_obj;  //参数
    ECHO_PARM_SET target_parm;  //参数
    EF_REVERB_FIX_PARM echo_fix_parm;
    unsigned int *ptr;                   //运算buf指针
    ECHO_FUNC_API *func_api;            //函数指针
    struct audio_stream_entry entry;	// 音频流入口
    int out_len;
    int process_len;
    u8 run_en;
    u8 update;
} ECHO_API_STRUCT;

extern ECHO_FUNC_API *get_echo_func_api();

#endif

#if 1

typedef struct _EF_REVERB0_PARM_ {
    unsigned int wet;  //0-300%         //回声的延时时间 0-300ms
    unsigned int dry;  //0-200%         // 0-70%
    short damping;     //0-100          //影响衰减的频率，值越大衰减越快
    short feedback;    //0-100          //feedback越大，衰减越慢
    short roomsize;    //0-100          //空间大小
    short highcut_freq;//0-18k
    short lowcut_freq; //0-18k
} EF_REVERB0_PARM;



typedef struct  _EF_REVERB0_FIX_PARM {
    unsigned int sr;
} EF_REVERB0_FIX_PARM;



/*open 跟 run 都是 成功 返回 RET_OK，错误返回 RET_ERR*/
/*魔音结构体*/
typedef struct __REVERB0_FUNC_API_ {
    unsigned int (*need_buf)(unsigned int *ptr, EF_REVERB0_FIX_PARM *echo_fix_parm);
    int (*open)(unsigned int *ptr, EF_REVERB0_PARM *echo_parm, EF_REVERB0_FIX_PARM *echo_fix_parm);
    int (*init)(unsigned int *ptr, EF_REVERB0_PARM *echo_parm);
    int (*run)(unsigned int *ptr, short *inbuf, short *outdata, int len);
} REVERB0_FUNC_API;



typedef struct _REVERB0_API_STRUCT_ {
    EF_REVERB0_PARM reverb_parm_obj;
    EF_REVERB0_FIX_PARM reverb_fixparm_obj;    //参数
    unsigned int *ptr;                   //运算buf指针
    REVERB0_FUNC_API *func_api;            //函数指针
    struct audio_stream_entry entry;	// 音频流入口
    int out_len;
    int process_len;
    s16 *out_buf;
    u8 run_en;
    u8 update;
} REVERB0_API_STRUCT;

extern REVERB0_FUNC_API *get_reverb0_func_api();
#endif





#if  1


typedef  struct  _Plate_reverb_parm_ {
    int wet;                      //0-300%
    int dry;                      //0-200%
    int pre_delay;                 //0-40ms
    int highcutoff;                //0-20k 高频截止
    int diffusion;                  //0-100%
    int decayfactor;                //0-100%
    int highfrequencydamping;       //0-100%
    int modulate;                  // 0或1
    int roomsize;                   //20%-100%
} Plate_reverb_parm;



typedef struct __PLATE_REVERB_FUNC_API_ {
    unsigned int (*need_buf)(Plate_reverb_parm *preverb_parm);
    int (*open)(unsigned int *ptr, Plate_reverb_parm *preverb_parm, EF_REVERB0_FIX_PARM *echo_fix_parm);
    int (*init)(unsigned int *ptr, Plate_reverb_parm *preverb_parm);
    int (*run)(unsigned int *ptr, short *inbuf, short *outdata, int len);
} PLATE_REVERB0_FUNC_API;



typedef struct _PLATE_REVERB_API_STRUCT_ {
    Plate_reverb_parm  reverb_parm_obj;
    EF_REVERB0_FIX_PARM reverb_fixparm_obj;    //参数
    unsigned int *ptr;                   //运算buf指针
    PLATE_REVERB0_FUNC_API *func_api;            //函数指针
    struct audio_stream_entry entry;	// 音频流入口
    int out_len;
    int process_len;
    s16 *out_buf;
    u8 run_en;
    u8 update;
} PLATE_REVERB_API_STRUCT;

extern PLATE_REVERB0_FUNC_API *get_plate_reverb_func_api();



#endif


#endif // reverb_api_h__
