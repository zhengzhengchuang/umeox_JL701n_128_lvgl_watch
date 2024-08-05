#include "../lv_watch.h"

extern void CtrlGh30xModuleStop(void);
extern void CtrlGh30xModuleStart(uint8_t work_type);

/****gomore与加速度计参数配套*****/
#define FifoSize (HrGs_Fifo_WM*6*5)
static u32 Wlen;
static u32 Rlen;
static u32 Widx;
static u32 Ridx;
static u8 WFifoData[FifoSize];
static u8 RFifoData[FifoSize];

static void HrGsFifoParaInit(void)
{
    Wlen = 0;
    Rlen = 0;
    Widx = 0;
    Ridx = 0;
    memset(WFifoData, 0, FifoSize);
    memset(RFifoData, 0, FifoSize);

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

    SetPpgWorkType((u8)msg[1]);
    SetPpgWearStatus(true);
    SetPpgEnableFlag(true);
    CtrlGh30xModuleStart((u8)msg[1]);
    
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

static u8 PpgMode = PpgModeManual;
u8 GetPpgMode(void)
{
    return PpgMode;
}
void SetPpgMode(u8 mode)
{
    PpgMode = mode;
    return;
}

static u8 PpgWork = PpgWorkNone;
u8 GetPpgWorkType(void)
{
    return PpgWork;
}
void SetPpgWorkType(u8 work)
{
    PpgWork = work;
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
    bool BondFlag = GetDevBondFlag();
    if(BondFlag == false) 
        return;

    bool PpgEn = GetPpgEnableFlag();
    if(PpgEn == false)
        return;

    u8 *pW = WFifoData;
    if(Widx + w_len <= FifoSize)
    {
        memcpy(&pW[Widx], w_buf, w_len);

        Widx += w_len;
        Widx %= FifoSize;
    }

    Wlen += w_len;

    return;
}

static const u8 bogs_rlen = 30;
static const u8 hrgs_rlen = 30;
void HrGsDataFifoRead(ST_GS_DATA_TYPE *gs_data, u16 *gs_len)
{
    bool BondFlag = GetDevBondFlag();
    if(BondFlag == false) 
        return;

    bool PpgEn = GetPpgEnableFlag();
    if(PpgEn == false)
        return;

    u32 r_len;

    u8 work = GetPpgWorkType();
    if(work == PpgWorkHr)
        r_len = hrgs_rlen*6;
    else if(work == PpgWorkBo)
        r_len = bogs_rlen*6;
    else
        return;

    if(Rlen + r_len > Wlen)
    {
        if(Rlen >= Wlen) return;

        r_len = Wlen - Rlen;
        r_len -= (r_len % 6);

        if(r_len == 0) return;
    }
    Rlen += r_len;

    u32 r_idx = Ridx;
    u8 *pR = RFifoData;
    u8 *pW = WFifoData;
    if(Ridx + r_len <= FifoSize)
    {
        memcpy(&pR[Ridx], &pW[Ridx], r_len);
        Ridx += r_len;
        Ridx %= FifoSize;
    }else
    {
        u32 remain_len = FifoSize - Ridx;
        memcpy(&pR[Ridx], &pW[Ridx], remain_len);
        Ridx = 0;
        memcpy(&pR[Ridx], &pW[Ridx], r_len - remain_len);
        Ridx += (r_len - remain_len);
    }
  
    u8 FifoData[6];
    s16 AccData[3];

    if(r_idx + r_len <= FifoSize)
    {
        for(u32 i = 0; i < r_len/6; i++)
        {
            memcpy(FifoData, &pR[r_idx], 6);

            AccData[0] = (s16)(((u16)FifoData[1]<<8)|(FifoData[0]));
            AccData[1] = (s16)(((u16)FifoData[3]<<8)|(FifoData[2]));
            AccData[2] = (s16)(((u16)FifoData[5]<<8)|(FifoData[4]));

            gs_data[i].sXAxisVal = AccData[0];
            gs_data[i].sYAxisVal = AccData[1];
            gs_data[i].sZAxisVal = AccData[2];

            r_idx += 6;
        }
    }else
    {
        u32 remain_len = FifoSize - r_idx; 

        if(remain_len % 6 == 0)
        {
            u32 i, j;
            for(i = 0; i < remain_len/6; i++)
            {
                memcpy(FifoData, &pR[r_idx], 6);

                AccData[0] = (s16)(((u16)FifoData[1]<<8)|(FifoData[0]));
                AccData[1] = (s16)(((u16)FifoData[3]<<8)|(FifoData[2]));
                AccData[2] = (s16)(((u16)FifoData[5]<<8)|(FifoData[4]));

                gs_data[i].sXAxisVal = AccData[0];
                gs_data[i].sYAxisVal = AccData[1];
                gs_data[i].sZAxisVal = AccData[2];

                r_idx += 6;
            }

            r_idx = 0;
            u32 len = r_len - remain_len;
            
            for(j = 0; j < len/6; j++)
            {
                memcpy(FifoData, &pR[r_idx], 6);

                AccData[0] = (s16)(((u16)FifoData[1]<<8)|(FifoData[0]));
                AccData[1] = (s16)(((u16)FifoData[3]<<8)|(FifoData[2]));
                AccData[2] = (s16)(((u16)FifoData[5]<<8)|(FifoData[4]));

                gs_data[j+i].sXAxisVal = AccData[0];
                gs_data[j+i].sYAxisVal = AccData[1];
                gs_data[j+i].sZAxisVal = AccData[2];

                r_idx += 6;
            }
        }
    }

    *gs_len = r_len/6;

    return;
}
