#include "../lv_watch.h"

extern void CtrlGh30xModuleStop(void);
extern void CtrlGh30xModuleStart(uint8_t work_type);

#define HrGs_FifoSize (1080)

u32 HrGs_Wlen = 0;
u32 HrGs_Rlen = 0;
u32 HrGs_WFifoIdx = 0;
u32 HrGs_RFifoIdx = 0;
u8 HrGs_FifoData[HrGs_FifoSize];

static void HrGsFifoParaInit(void)
{
    HrGs_Wlen = 0;
    HrGs_Rlen = 0;
    HrGs_WFifoIdx = 0;
    HrGs_RFifoIdx = 0;
    memset(HrGs_FifoData, 0, \
        HrGs_FifoSize);

    return;
}

static void SensorHrTask(void *p)
{
    int rev_ret;
    int rev_msg[8] = {0};

    HrGsFifoParaInit();

    HrSensorStopSample();

    while(1)
    {
        rev_ret = \
            os_taskq_pend(NULL, rev_msg, \
                ARRAY_SIZE(rev_msg)); 

        if(rev_ret == OS_TASKQ)
            SensorHrTaskHandle(rev_msg, \
                ARRAY_SIZE(rev_msg));
    }
    
    return;
}

void SensorHrTaskCreate(void)
{
    int err = task_create(SensorHrTask, \
        NULL, Sensor_Hr_Task_Name);
    if(err) 
        r_printf("Hr task create err!!!!!!!:%d \n", err);

    return;
}

void SensorHrTaskHandle(int *rev_msg, u8 len)
{
    if(!rev_msg || len == 0)
        return;

    int msg_cmd = rev_msg[0];

    switch(msg_cmd)
    {
        case SensorHrProcess:
        {
            hal_gh30x_int_handler_bottom_half();
        }
            break;

        case SensorHrEnableModule:
        {
            HrGsFifoParaInit();

            CtrlGh30xModuleStart(\
                (u8)rev_msg[1]);

            SetHrSensorWorkType(\
                (u8)rev_msg[1]);

            SetHrSensorWearStatus(true);

            SetHrSensorEnableFlag(true);

            SetHrSensorDataFilterCnt(\
                Hr_Data_Filter_Cnt);
        }  
            break;

        case SensorHrDisableModule:
        {
            CtrlGh30xModuleStop();

            SetHrSensorWorkType(\
                SensorWorkNone);

            SetHrSensorEnableFlag(false);
        } 
            break;

        default:
            break;
    }

    return;
}

int PostSensorHrTaskMsg(int *post_msg, u8 len)
{
    int err = 0;
    int count = 0;

    if(!post_msg || len == 0)
        return -1;

__retry:
    err = os_taskq_post_type(Sensor_Hr_Task_Name, \
        post_msg[0], len - 1, &post_msg[1]);

    if(cpu_in_irq() || cpu_irq_disabled())
        return err;

    if(err) 
    {
        if(!strcmp(os_current_task(), \
            Sensor_Hr_Task_Name)) 
            return err;

        if(count > 20)
            return -1;
        
        count++;
        os_time_dly(1);
        goto __retry;
    }

    return err;
}

static u8 SensorMode = \
    SensorModeManual;
u8 GetHrSensorMode(void)
{
    return SensorMode;
}
void SetHrSensorMode(u8 mode)
{
    SensorMode = mode;

    return;
}

static u8 SensorWorkType = \
    SensorWorkNone;
u8 GetHrSensorWorkType(void)
{
    return SensorWorkType;
}
void SetHrSensorWorkType(u8 type)
{
    SensorWorkType = type;

    return;
}

static u8 DataFilterCnt = \
    Hr_Data_Filter_Cnt;
u8 GetHrSensorDataFilterCnt(void)
{
    return DataFilterCnt;
}
void SetHrSensorDataFilterCnt(u8 Cnt)
{
    DataFilterCnt = Cnt;

    return;
}

static bool EnableFlag = \
    false; 
bool GetHrSensorEnableFlag(void)
{
    return EnableFlag;
}
void SetHrSensorEnableFlag(bool en)
{
    EnableFlag = en;

    return;
}

static bool HrSensorWear = \
    true;
bool GetHrSensorWearStatus(void)
{
    return HrSensorWear;
}
void SetHrSensorWearStatus(bool status)
{
    HrSensorWear = status;

    return;
}

void HrSensorStopSample(void)
{
    int SensorHrPost[1];
	SensorHrPost[0] = \
		SensorHrDisableModule;
	PostSensorHrTaskMsg(SensorHrPost, 1); 

    return;
}

void HrSensorStartSample(SensorWorkType_t type, \
    SensorWorkMode_t mode)
{
    int SensorHrPost[2];
	SensorHrPost[0] = \
		SensorHrEnableModule;
    SensorHrPost[1] = \
		(int)type;
	PostSensorHrTaskMsg(SensorHrPost, 2); 

    SetHrSensorMode(mode);

    return;
}

void HrGsDataFifoWrite(u8 *w_buf, u32 w_len)
{
    if(!w_buf || w_len == 0)
        return;

    bool EnableFlag = \
        GetHrSensorEnableFlag();
    if(EnableFlag == false)
        return;

    if(HrGs_WFifoIdx + w_len <= HrGs_FifoSize)
    {
        memcpy(&HrGs_FifoData[HrGs_WFifoIdx], \
            w_buf, w_len);

        HrGs_WFifoIdx += w_len;
        HrGs_WFifoIdx %= HrGs_FifoSize;
    }else
    {
        u32 fifo_remain_len = \
            HrGs_FifoSize - HrGs_WFifoIdx;

        if(fifo_remain_len != 0)
            memcpy(&HrGs_FifoData[HrGs_WFifoIdx], \
                w_buf, fifo_remain_len);

        HrGs_WFifoIdx = 0;

        memcpy(&HrGs_FifoData[HrGs_WFifoIdx], \
            &w_buf[fifo_remain_len], (w_len - fifo_remain_len));

        HrGs_WFifoIdx += (w_len - fifo_remain_len);
    }

    HrGs_Wlen += w_len;

    return;
}

static const BoGs_Data_Max = 15;
static const HrGs_Data_Max = 60;
void HrGsDataFifoRead(ST_GS_DATA_TYPE *Gs_data, u16 *r_idx)
{
    if(!Gs_data || !r_idx)
        return;

    bool EnableFlag = \
        GetHrSensorEnableFlag();
    if(EnableFlag == false)
        return;

    u8 FifoData[6];
    s16 AccRawData[3];

    u32 r_max_len;

    u8 work_type = \
        GetHrSensorWorkType();
    if(work_type == SensorWorkHr)
        r_max_len = HrGs_Data_Max*6;
    else if(work_type == SensorWorkBo)
        r_max_len = BoGs_Data_Max*6;
    else
        return;

    if(HrGs_Rlen + r_max_len > HrGs_Wlen)
    {
        if(HrGs_Rlen >= HrGs_Wlen)
            return;

        r_max_len = \
            HrGs_Wlen - HrGs_Rlen;
        r_max_len -= \
            (r_max_len % 6);

        if(r_max_len == 0)
            return;
    }

    if(HrGs_RFifoIdx + r_max_len <= HrGs_FifoSize)
    {
        u32 i;
        for(i = 0; i < r_max_len/6; i++)
        {
            memcpy(FifoData, \
                &HrGs_FifoData[HrGs_RFifoIdx], 6);

            AccRawData[0] = (s16)(((uint16_t)FifoData[1] << 8) | \
                (FifoData[0]));
            AccRawData[1] = (s16)(((uint16_t)FifoData[3] << 8) | \
                (FifoData[2]));
            AccRawData[2] = (s16)(((uint16_t)FifoData[5] << 8) | \
                (FifoData[4]));

            Gs_data[i].sXAxisVal = AccRawData[0];
            Gs_data[i].sYAxisVal = AccRawData[1];
            Gs_data[i].sZAxisVal = AccRawData[2];

            HrGs_RFifoIdx += 6;
        }
    }else
    {
        u32 fifo_remain_len = \
            HrGs_FifoSize - HrGs_RFifoIdx; 

        if(fifo_remain_len % 6 == 0)
        {
            u32 i;
            for(i = 0; i < fifo_remain_len/6; i++)
            {
                memcpy(FifoData, \
                    &HrGs_FifoData[HrGs_RFifoIdx], 6);

                AccRawData[0] = (s16)(((uint16_t)FifoData[1] << 8) | \
                    (FifoData[0]));
                AccRawData[1] = (s16)(((uint16_t)FifoData[3] << 8) | \
                    (FifoData[2]));
                AccRawData[2] = (s16)(((uint16_t)FifoData[5] << 8) | \
                    (FifoData[4]));

                // xdata[i] = AccRawData[0];
                // ydata[i] = AccRawData[1];
                // zdata[i] = AccRawData[2];
                Gs_data[i].sXAxisVal = AccRawData[0];
                Gs_data[i].sYAxisVal = AccRawData[1];
                Gs_data[i].sZAxisVal = AccRawData[2];

                HrGs_RFifoIdx += 6;
            }

            u32 j;
            HrGs_RFifoIdx %= \
                HrGs_FifoSize;
            u32 data_remain_len = \
                r_max_len - fifo_remain_len;
            
            for(j = 0; j < data_remain_len/6; j++)
            {
                memcpy(FifoData, \
                    &HrGs_FifoData[HrGs_RFifoIdx], 6);

                AccRawData[0] = (s16)(((uint16_t)FifoData[1] << 8) | \
                    (FifoData[0]));
                AccRawData[1] = (s16)(((uint16_t)FifoData[3] << 8) | \
                    (FifoData[2]));
                AccRawData[2] = (s16)(((uint16_t)FifoData[5] << 8) | \
                    (FifoData[4]));

                // xdata[j + i] = AccRawData[0];
                // ydata[j + i] = AccRawData[1];
                // zdata[j + i] = AccRawData[2];
                Gs_data[j + i].sXAxisVal = AccRawData[0];
                Gs_data[j + i].sYAxisVal = AccRawData[1];
                Gs_data[j + i].sZAxisVal = AccRawData[2];

                HrGs_RFifoIdx += 6;
            }
        }else
        {
            fifo_remain_len -= \
                (fifo_remain_len % 6);
            
            u32 i;
            for(i = 0; i < fifo_remain_len/6; i++)
            {
                memcpy(FifoData, \
                    &HrGs_FifoData[HrGs_RFifoIdx], 6);

                AccRawData[0] = (s16)(((uint16_t)FifoData[1] << 8) | \
                    (FifoData[0]));
                AccRawData[1] = (s16)(((uint16_t)FifoData[3] << 8) | \
                    (FifoData[2]));
                AccRawData[2] = (s16)(((uint16_t)FifoData[5] << 8) | \
                    (FifoData[4]));

                // xdata[i] = AccRawData[0];
                // ydata[i] = AccRawData[1];
                // zdata[i] = AccRawData[2];
                Gs_data[i].sXAxisVal = AccRawData[0];
                Gs_data[i].sYAxisVal = AccRawData[1];
                Gs_data[i].sZAxisVal = AccRawData[2];

                HrGs_RFifoIdx += 6;
            }

            u8 cache_idx = 0;
            u8 fifo_remain_len_tmp = \
                HrGs_FifoSize - HrGs_RFifoIdx;
            memcpy(&FifoData[cache_idx], &HrGs_FifoData[HrGs_RFifoIdx], \
                fifo_remain_len_tmp);

            HrGs_RFifoIdx += fifo_remain_len_tmp; 
            HrGs_RFifoIdx %= HrGs_FifoSize;

            cache_idx += fifo_remain_len_tmp;
            memcpy(&FifoData[cache_idx], &HrGs_FifoData[HrGs_RFifoIdx], \
                (6 - fifo_remain_len_tmp));
            AccRawData[0] = (s16)(((uint16_t)FifoData[1] << 8) | \
                (FifoData[0]));
            AccRawData[1] = (s16)(((uint16_t)FifoData[3] << 8) | \
                (FifoData[2]));
            AccRawData[2] = (s16)(((uint16_t)FifoData[5] << 8) | \
                (FifoData[4]));

            // xdata[i++] = AccRawData[0];
            // ydata[i++] = AccRawData[1];
            // zdata[i++] = AccRawData[2];
            Gs_data[i++].sXAxisVal = AccRawData[0];
            Gs_data[i++].sYAxisVal = AccRawData[1];
            Gs_data[i++].sZAxisVal = AccRawData[2];

            HrGs_RFifoIdx += (6 - fifo_remain_len_tmp);

            u32 j;
            u32 data_remain_len = \
                (r_max_len - fifo_remain_len - 6);
            for(j = 0; j < data_remain_len/6; j++)
            {
                memcpy(FifoData, \
                    &HrGs_FifoData[HrGs_RFifoIdx], 6);

                AccRawData[0] = (s16)(((uint16_t)FifoData[1] << 8) | \
                    (FifoData[0]));
                AccRawData[1] = (s16)(((uint16_t)FifoData[3] << 8) | \
                    (FifoData[2]));
                AccRawData[2] = (s16)(((uint16_t)FifoData[5] << 8) | \
                    (FifoData[4]));

                // xdata[j + i] = AccRawData[0];
                // ydata[j + i] = AccRawData[1];
                // zdata[j + i] = AccRawData[2];
                Gs_data[j + i].sXAxisVal = AccRawData[0];
                Gs_data[j + i].sYAxisVal = AccRawData[1];
                Gs_data[j + i].sZAxisVal = AccRawData[2];

                HrGs_RFifoIdx += 6;
            }
        }
    }

    HrGs_Rlen += r_max_len;

    *r_idx =  r_max_len/6;

    // printf("w = %d, r = %d\n", \
    //     HrGs_Wlen, HrGs_Rlen);

    return;
}
