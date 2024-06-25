#include "lfwordana_enc_api.h"

//使用内置flash音源时(默认使用)
#define  MIDI_W2S_SOUCE_FILE_PATH  SDFILE_RES_ROOT_PATH"nihao.raw\0"
//使用内置flash、外挂flash 或者sd卡音源时
#define  MIDI_W2S_SOUCE_FILE_PATH2  SDFILE_RES_ROOT_PATH"nihao.raw\0"

#if defined(MIDI_SUPPORT_W2S) && MIDI_SUPPORT_W2S
#define REC_LEN   3000   //预处理输出长度
struct ana_process {
    struct lfwordana_encoder *ana;
    OS_MUTEX mutex;
    short rec_outdata[REC_LEN];

    struct audio_adc_output_hdl adc_output;
    struct adc_mic_ch mic_ch;
    cbuffer_t cbuf;
    short *mic_out_buf;
    u8 source;

    void *data_in;
    int data_in_len;
    u8 *out_path;
    void *w2s_fp;//句柄预留

    u16 timer;
    void *in_fp;
};
#define SOURCE_FROM_MIC  0
#define SOURCE_FROM_FILE 1
#define SOURCE_FROM_BUF  2

struct ana_process *ana_proc = NULL;

static void lfwordana_process_task(void *priv)
{
    struct ana_process *ana_p = priv;
    struct lfwordana_encoder *ana = ana_p->ana;
    int ret = 0;
    u32 len = 0;
    while (1) {
        if ((ana_p->source == SOURCE_FROM_MIC) || (ana_p->source == SOURCE_FROM_FILE)) {
            os_sem_pend(&ana_p->mutex, 0);
            void *data = cbuf_read_alloc(&ana_p->cbuf, &len);
            ret = lfwordana_encoder_run(ana, data, len);
            cbuf_read_updata(&ana_p->cbuf, len);
        } else if (ana_p->source == SOURCE_FROM_BUF) {
            if (ana_p->data_in) {
                ret = lfwordana_encoder_run(ana, ana_p->data_in, ana_p->data_in_len);
            }
        }
        if (!ret) {
            if (ana_p->w2s_fp) { //保存预处理后的数据
                ret = fwrite(ana_p->w2s_fp, ana_p->ana->w2s_parmIn.rec_data, ana_p->ana->w2s_parmIn.rec_len);
                if (ret != ana_p->ana->w2s_parmIn.rec_len) {
                    log_e("lfwordana write err\n");
                }
                fclose(ana_p->w2s_fp);
                ana_p->w2s_fp = NULL;
            }

            midi_w2s_parm_set(&ana->w2s_parmOut, sizeof(MIDI_W2S_STRUCT));//保存预处理输出的参数
            if (ana_p->source == SOURCE_FROM_MIC) {
                audio_mic_close(&ana_p->mic_ch, &ana_p->adc_output);
                if (ana_p->mic_out_buf) {
                    free(ana_p->mic_out_buf);
                }
            }
            lfwordana_encoder_close(ana_p->ana);
            free(ana_p);
            ana_proc = ana_p = NULL;
            os_task_del("audio_enc");//线程自杀
            break;
        }
    }
}

static void adc_output_to_enc(void *priv, s16 *data, int len)
{
    struct ana_process *ana_p = priv;
    int ret = cbuf_write(&ana_p->cbuf, data, len);
    if (!ret) {
        log_e("mic data lose \n");
    }
    os_mutex_post(&ana_p->mutex);
}
void input_file_process(void *p)
{
    struct ana_process *ana_p = (struct ana_process *)p;
    if (ana_p->in_fp) {
        int len;
        void *ptr = cbuf_write_alloc(&ana_p->cbuf, &len);
        if (!len) {
            log_e("mic data lose \n");
            return;
        }
        if (len > 512) {
            len = 512;
        }
        len = fread(ana_p->in_fp, ptr, len);
        cbuf_write_updata(&ana_p->cbuf, len);
        os_mutex_post(&ana_p->mutex);
    }
}
void lfwordana_process()
{
    if (!ana_proc) {
        ana_proc = zalloc(sizeof(struct ana_process));
    }
    struct ana_process *ana_p = &ana_proc;
    REC_W2S_STUCT parm = {0};
    parm.toneadjust_enable = 1;            //滤波使能
    parm.multi_enable = 0;                 //是否区分多字的情况，buf大的时候考虑做，buf小的时候不考虑
    parm.rec_data = ana_p->rec_outdata;     //输出数据地址【保证short对齐】
    parm.rec_len = REC_LEN;                //输出地址长度【按short】
    parm.energy_thresh = 3000;            //能量阈值
    ana_p->out_path = MIDI_W2S_FILE_PATH;//"storage/sd0/C/W2S/W2S.raw\0";
    struct lfwordana_encoder *ana = lfwordana_encoder_open(&parm);
    ana_p->ana = ana;
    ana_p->w2s_fp = fopen(ana_p->out_path, "wb");

    /* ana_p->source = SOURCE_FROM_FILE; */
    ana_p->source = SOURCE_FROM_BUF;
    u32 mic_out_buf_len = 2048;
    if (ana_p->source == SOURCE_FROM_MIC) { //算法库未支持多次输入
        u8 gain = 8;
        u32 sample_rate = 16000;
        ana_p->mic_out_buf = malloc(mic_out_buf_len);
        cbuf_init(&ana_p->cbuf, ana_p->mic_out_buf, mic_out_buf_len);
        os_sem_create(&ana_p->mutex, 0);
        ana_p->adc_output.handler = adc_output_to_enc;
        ana_p->adc_output.priv    = ana_p;
        if (audio_mic_open(&ana_p->mic_ch, sample_rate, gain) == 0) {
            audio_mic_add_output(&ana_p->adc_output);
            audio_mic_start(&ana_p->mic_ch);
        }
    } else if (ana_p->source == SOURCE_FROM_BUF) {
        void *fp = fopen(MIDI_W2S_SOUCE_FILE_PATH, "r");//在内置flash的，pcm音源文件
        if (!fp) {
            lfwordana_encoder_close(ana_p->ana);
            return;
        }
        struct vfs_attr attr = {0};
        fget_attrs(fp, &attr);
        ana_p->data_in = (void *)attr.sclust;
        ana_p->data_in_len = attr.fsize;
        fclose(fp);
        /* ana_p->data_in = your_data_addr;	//输入数据地址，内置flash可用文件簇地址 */
        /* ana_p->data_in_len = your_data_len;//输入出数据总长度 */
    } else if (ana_p->source == SOURCE_FROM_FILE) { //算法库未支持多次输入
        ana_p->mic_out_buf = malloc(mic_out_buf_len);
        cbuf_init(&ana_p->cbuf, ana_p->mic_out_buf, mic_out_buf_len);
        os_sem_create(&ana_p->mutex, 0);
        ana_p->in_fp = fopen(MIDI_W2S_SOUCE_FILE_PATH2, "r");//在内置flash、外挂flash、sd卡等路径的，pcm音源文件
        ana_p->timer = sys_timer_add(ana_p, input_file_process, 50);
    }

    task_create(lfwordana_process_task, ana_p, "audio_enc");
}

#endif/* MIDI_SUPPORT_W2S */
