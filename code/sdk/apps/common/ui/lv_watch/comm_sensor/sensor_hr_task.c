#include "../lv_watch.h"

extern void CtrlGh30xModuleStop(void);
extern void CtrlGh30xModuleStart(uint8_t work_type);

#define FifoSize (360)

static u32 Wlen = 0;
static u32 Rlen = 0;
static u32 WFifoIdx = 0;
static u32 RFifoIdx = 0;
static u8 FifoData[FifoSize];

static void HrGsFifoParaInit(void)
{
    Wlen = 0;
    Rlen = 0;
    WFifoIdx = 0;
    RFifoIdx = 0;
    memset(FifoData, 0, FifoSize);

    return;
}

static void PpgProcessHandle(void)
{
    hal_gh30x_int_handler_bottom_half();

    return;
}

static void PpgEnableModuleHandle(int *msg)
{
    HrGsFifoParaInit();

    CtrlGh30xModuleStart((u8)msg[1]);
    SetPpgWorkType((u8)msg[1]);
    SetPpgWearStatus(true);
    SetPpgEnableFlag(true);
    SetHrDataFilter(Hr_Data_Filter);

    return;
}

static void PpgDisableModuleHandle(void)
{
    CtrlGh30xModuleStop();
    SetPpgWorkType(PpgWorkNone);
    SetPpgEnableFlag(false);

    return;
}

static void PpgTask(void *p)
{
    int ret;
    int msg[8] = {0};

    HrGsFifoParaInit();

    DisablePpgModule();

    while(1)
    {
        ret = os_taskq_pend(NULL, msg, ARRAY_SIZE(msg)); 

        if(ret == OS_TASKQ)
            PpgTaskMsgHandle(msg, ARRAY_SIZE(msg));
    }
    
    return;
}

void PpgTaskCreate(void)
{
    int err = task_create(PpgTask, NULL, Ppg_Task_Name);
    if(err) printf("ppg task create err!!!!!!!:%d \n", err);

    return;
}

void PpgTaskMsgHandle(int *msg, u8 len)
{
    if(!msg || len == 0)
        return;

    int cmd = msg[0];

    switch(cmd)
    {
        case PpgMsgProcess:
            PpgProcessHandle();
            break;

        case PpgMsgEnable:
            PpgEnableModuleHandle(msg);  
            break;

        case PpgMsgDisable:
            PpgDisableModuleHandle();
            break;

        default:
            break;
    }

    return;
}

int PostPpgTaskMsg(int *msg, u8 len)
{
    int err = 0;
    int count = 0;

    if(!msg || len == 0)
        return -1;

__retry:
    err = os_taskq_post_type(Ppg_Task_Name, \
        msg[0], len - 1, &msg[1]);

    if(cpu_in_irq() || cpu_irq_disabled())
        return err;

    if(err) 
    {
        if(!strcmp(os_current_task(), Ppg_Task_Name)) 
            return err;

        if(count > 20)
            return -1;
        
        count++;
        os_time_dly(1);
        goto __retry;
    }

    return err;
}

void DisablePpgModule(void)
{
    int PpgMsg[2];
	PpgMsg[0] = PpgMsgDisable;
	PostPpgTaskMsg(PpgMsg, 1); 

    return;
}

void EnablePpgModule(u8 type, u8 mode)
{
    int PpgMsg[2];
	PpgMsg[0] = PpgMsgEnable;
    PpgMsg[1] = (int)type;
	PostPpgTaskMsg(PpgMsg, 2);

    SetPpgMode(mode);

    return;
}

static u8 PpgMode = \
    PpgModeManual;
u8 GetPpgMode(void)
{
    return PpgMode;
}
void SetPpgMode(u8 mode)
{
    PpgMode = mode;
    return;
}

static u8 PpgWork = \
    PpgWorkNone;
u8 GetPpgWorkType(void)
{
    return PpgWork;
}
void SetPpgWorkType(u8 work)
{
    PpgWork = work;
    return;
}

static u8 HrFilter = \
    Hr_Data_Filter;
u8 GetHrDataFilter(void)
{
    return HrFilter;
}
void SetHrDataFilter(u8 f)
{
    HrFilter = f;
    return;
}

static bool PpgEn = false; 
bool GetPpgEnableFlag(void)
{
    return PpgEn;
}
void SetPpgEnableFlag(bool en)
{
    PpgEn = en;
    return;
}

static bool PpgWear = true;
bool GetPpgWearStatus(void)
{
    return PpgWear;
}
void SetPpgWearStatus(bool wear)
{
    PpgWear = wear;
    return;
}

void HrGsDataFifoWrite(u8 *w_buf, u32 w_len)
{
    if(w_buf == NULL || w_len == 0)
        return;

    bool PpgEn = GetPpgEnableFlag();
    if(PpgEn == false)
        return;

    if(WFifoIdx + w_len <= FifoSize)
    {
        memcpy(&FifoData[WFifoIdx], w_buf, w_len);

        WFifoIdx += w_len;
        WFifoIdx %= FifoSize;
    }else
    {
        u32 remain_len = FifoSize - WFifoIdx;

        if(remain_len != 0)
            memcpy(&FifoData[WFifoIdx], w_buf, remain_len);

        WFifoIdx = 0;

        memcpy(&FifoData[WFifoIdx], &w_buf[remain_len], (w_len - remain_len));

        WFifoIdx += (w_len - remain_len);
    }

    Wlen += w_len;

    return;
}

static const u8 BoGs_Data_Max = 30;
static const u8 HrGs_Data_Max = 30;
void HrGsDataFifoRead(ST_GS_DATA_TYPE *Gs_data, u16 *r_idx)
{
    if(Gs_data == NULL || r_idx == NULL)
        return;

    bool PpgEn = GetPpgEnableFlag();
    if(PpgEn == false) return;

    u8 FifoData[6];
    s16 AccRawData[3];

    u32 r_max_len;

    u8 work_type = GetPpgWorkType();
    if(work_type == PpgWorkHr)
        r_max_len = HrGs_Data_Max*6;
    else if(work_type == PpgWorkBo)
        r_max_len = BoGs_Data_Max*6;
    else
        return;

    if(Rlen + r_max_len > Wlen)
    {
        if(Rlen >= Wlen)
            return;

        r_max_len = Wlen - Rlen;
        r_max_len -= (r_max_len % 6);

        if(r_max_len == 0)
            return;
    }

    if(RFifoIdx + r_max_len <= FifoSize)
    {
        u32 i;
        for(i = 0; i < r_max_len/6; i++)
        {
            memcpy(FifoData, &FifoData[RFifoIdx], 6);

            AccRawData[0] = (s16)(((u16)FifoData[1] << 8) | \
                (FifoData[0]));
            AccRawData[1] = (s16)(((u16)FifoData[3] << 8) | \
                (FifoData[2]));
            AccRawData[2] = (s16)(((u16)FifoData[5] << 8) | \
                (FifoData[4]));

            Gs_data[i].sXAxisVal = AccRawData[0];
            Gs_data[i].sYAxisVal = AccRawData[1];
            Gs_data[i].sZAxisVal = AccRawData[2];

            RFifoIdx += 6;
        }
    }else
    {
        u32 remain_len = \
            FifoSize - RFifoIdx; 

        if(remain_len % 6 == 0)
        {
            u32 i;
            for(i = 0; i < remain_len/6; i++)
            {
                memcpy(FifoData, \
                    &FifoData[RFifoIdx], 6);

                AccRawData[0] = (s16)(((u16)FifoData[1] << 8) | \
                    (FifoData[0]));
                AccRawData[1] = (s16)(((u16)FifoData[3] << 8) | \
                    (FifoData[2]));
                AccRawData[2] = (s16)(((u16)FifoData[5] << 8) | \
                    (FifoData[4]));

                Gs_data[i].sXAxisVal = AccRawData[0];
                Gs_data[i].sYAxisVal = AccRawData[1];
                Gs_data[i].sZAxisVal = AccRawData[2];

                RFifoIdx += 6;
            }

            u32 j;
            RFifoIdx %= \
                FifoSize;
            u32 data_remain_len = \
                r_max_len - remain_len;
            
            for(j = 0; j < data_remain_len/6; j++)
            {
                memcpy(FifoData, \
                    &FifoData[RFifoIdx], 6);

                AccRawData[0] = (s16)(((u16)FifoData[1] << 8) | \
                    (FifoData[0]));
                AccRawData[1] = (s16)(((u16)FifoData[3] << 8) | \
                    (FifoData[2]));
                AccRawData[2] = (s16)(((u16)FifoData[5] << 8) | \
                    (FifoData[4]));

                Gs_data[j + i].sXAxisVal = AccRawData[0];
                Gs_data[j + i].sYAxisVal = AccRawData[1];
                Gs_data[j + i].sZAxisVal = AccRawData[2];

                RFifoIdx += 6;
            }
        }else
        {
            remain_len -= (remain_len % 6);
            
            u32 i;
            for(i = 0; i < remain_len/6; i++)
            {
                memcpy(FifoData, \
                    &FifoData[RFifoIdx], 6);

                AccRawData[0] = (s16)(((u16)FifoData[1] << 8) | \
                    (FifoData[0]));
                AccRawData[1] = (s16)(((u16)FifoData[3] << 8) | \
                    (FifoData[2]));
                AccRawData[2] = (s16)(((u16)FifoData[5] << 8) | \
                    (FifoData[4]));

                Gs_data[i].sXAxisVal = AccRawData[0];
                Gs_data[i].sYAxisVal = AccRawData[1];
                Gs_data[i].sZAxisVal = AccRawData[2];

                RFifoIdx += 6;
            }

            u8 cache_idx = 0;
            u8 fifo_remain_len_tmp = \
                FifoSize - RFifoIdx;
            memcpy(&FifoData[cache_idx], &FifoData[RFifoIdx], \
                fifo_remain_len_tmp);

            RFifoIdx += fifo_remain_len_tmp; 
            RFifoIdx %= FifoSize;

            cache_idx += fifo_remain_len_tmp;
            memcpy(&FifoData[cache_idx], &FifoData[RFifoIdx], \
                (6 - fifo_remain_len_tmp));
            AccRawData[0] = (s16)(((u16)FifoData[1] << 8) | \
                (FifoData[0]));
            AccRawData[1] = (s16)(((u16)FifoData[3] << 8) | \
                (FifoData[2]));
            AccRawData[2] = (s16)(((u16)FifoData[5] << 8) | \
                (FifoData[4]));

            Gs_data[i++].sXAxisVal = AccRawData[0];
            Gs_data[i++].sYAxisVal = AccRawData[1];
            Gs_data[i++].sZAxisVal = AccRawData[2];

            RFifoIdx += (6 - fifo_remain_len_tmp);

            u32 j;
            u32 data_remain_len = \
                (r_max_len - remain_len - 6);
            for(j = 0; j < data_remain_len/6; j++)
            {
                memcpy(FifoData, \
                    &FifoData[RFifoIdx], 6);

                AccRawData[0] = (s16)(((u16)FifoData[1] << 8) | \
                    (FifoData[0]));
                AccRawData[1] = (s16)(((u16)FifoData[3] << 8) | \
                    (FifoData[2]));
                AccRawData[2] = (s16)(((u16)FifoData[5] << 8) | \
                    (FifoData[4]));

                Gs_data[j + i].sXAxisVal = AccRawData[0];
                Gs_data[j + i].sYAxisVal = AccRawData[1];
                Gs_data[j + i].sZAxisVal = AccRawData[2];

                RFifoIdx += 6;
            }
        }
    }

    Rlen += r_max_len;

    *r_idx =  r_max_len/6;

    return;
}
