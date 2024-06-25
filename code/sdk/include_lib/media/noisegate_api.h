
#ifndef NOISEGATE_API_H
#define NOISEGATE_API_H

typedef struct _NoiseGateParam {
    int attackTime;
    int releaseTime;
    int threshold;
    int low_th_gain;
    int sampleRate;
    int channel;
    int IndataInc;
    int OutdataInc;
} NoiseGateParam;

typedef struct _NoiseGate_update_Param {
    int attackTime;  //启动时间
    int releaseTime; //释放时间
    int threshold;	  //阈值mdb  例如-65.5db 例如传下来应是-65500
    int low_th_gain;	//低于阈值增益  放大30bit 例如(int)(0.1 * (1 << 30))
} noisegate_update_param;

int noiseGate_buf();
#ifdef CONFIG_EFFECT_CORE_V2_ENABLE
void noiseGate_init(void *workbuf, NoiseGateParam *param);
void noiseGate_update(void *work_buf, NoiseGateParam *param);
#else
void noiseGate_init(void *work_buf, int attackTime, int releaseTime, int threshold, int low_th_gain, int sampleRate, int channel);
#endif
int noiseGate_run(void *work_buf, short *in_buf, short *out_buf, int per_channel_npoint);

#endif // NOISEGATE_API_H

