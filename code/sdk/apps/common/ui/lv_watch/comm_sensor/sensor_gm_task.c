#include "../lv_watch.h"

#define ONE_G (9.807f)
#define Cali_Vm_Mask (0x55bb)

GmCaliInfoPara_t Cali_Info;
static const GmCaliInfoPara_t InitInfo = {
    .cali_succ = false,
    .calipara = {0.0f},
};

/****地磁与加速度计参数配套*****/
#define GmGs_FifoSize (GmGs_Fifo_WM*6*3)
static u32 GmGs_WFifoIdx;
static u32 GmGs_RFifoIdx;
static u8 GmGs_FifoData[GmGs_FifoSize];

static u8 GmAccRawIdx;
static s16 GmAccRawX[GmGs_Fifo_WM];
static s16 GmAccRawY[GmGs_Fifo_WM];
static s16 GmAccRawZ[GmGs_Fifo_WM];

/****地磁算法传参*****/
float GmOutput[3] = {0.0f, 0.0f, 0.0f};
float GmRawData[3] = {0.0f, 0.0f, 0.0f};
float AccCorrect[3] = {0.0f, 0.0f, 0.0f};
float EulerValue[3] = {0.0f, 0.0f, 0.0f};
float GmOutputOffset[4] = {0.0f, 0.0f, 0.0f, 0.0f}; 

static u16 timer_id;
static s16 GmYawAngle;
static bool GmEnableFlag;
static int8_t QmcAccuracy;

static void GmGsFifoParaInit(void)
{
    GmGs_WFifoIdx = 0;
    GmGs_RFifoIdx = 0;
    memset(GmGs_FifoData, 0, \
        GmGs_FifoSize);

    return;
}

static void GmAccRawDataInit(void)
{
    GmAccRawIdx = 0;

    memset(GmAccRawX, 0, sizeof(GmAccRawX));
    memset(GmAccRawY, 0, sizeof(GmAccRawY));
    memset(GmAccRawZ, 0, sizeof(GmAccRawZ));

    return;
}


static void ProcessTimeoutCb(void *priv)
{
    int SensorGmMsg[1];
	SensorGmMsg[0] = SensorGmMsgProcess;
	PostSensorGmTaskMsg(SensorGmMsg, 1);

    return;
}

static void ProcessTimerCreate(void)
{
    if(timer_id)
        sys_hi_timer_del(timer_id);

    timer_id = 0;

    if(!timer_id)
        timer_id = \
            sys_hi_timer_add(NULL, ProcessTimeoutCb, 20);

    return;
}

void ProcessTimerDestory(void)
{
    if(timer_id)
        sys_hi_timer_del(timer_id);

    timer_id = 0;

    return;
}

static void SensorGmTask(void *p)
{
    int rev_ret;
    int rev_msg[8] = {0};

    GmGsFifoParaInit();
    GmAccRawDataInit();

    SensorGmCaliInfoRead();

    qst_ical_init(\
        Cali_Info.calipara);
	fusion_enable();

    /*开机先失能模块*/
    DisableSensorGmModule();

    while(1)
    {
        rev_ret = \
            os_taskq_pend(NULL, rev_msg, \
                ARRAY_SIZE(rev_msg)); 

        if(rev_ret == OS_TASKQ)
            SensorGmTaskMsgHandle(rev_msg, \
                ARRAY_SIZE(rev_msg));
    }
    
    return;
}

void SensorGmTaskCreate(void)
{
    int err = task_create(SensorGmTask, \
        NULL, Sensor_Gm_Task_Name);
    if(err) 
        r_printf("Gm task create err!!!!!!!:%d \n", err);

    return;
}

void SensorGmTaskMsgHandle(int *rev_msg, u8 len)
{
    if(!rev_msg || len == 0)
        return;

    int msg_cmd = rev_msg[0];

    switch(msg_cmd)
    {
        case SensorGmMsgProcess:
        {
            SensorGmProcess();
        }
            break;
        
        case SensorGmMsgEnableModule:
        {
            qmc6309_enable();

            GmAccRawDataInit();

            SetSensorGmEnableFlag(true);
            
            ProcessTimerCreate();

            printf("Gm enable!!!!!!!!!!!!\n");
        } 
            break;

        case SensorGmMsgDisableModule:
        {
            qmc6309_disable();

            SetSensorGmEnableFlag(false);

            ProcessTimerDestory();

            printf("Gm disable!!!!!!!!!!!!\n");
        }  
            break;

        default:
            printf("*************Gm msg not found\n");
            break;
    }

    return;
}

int PostSensorGmTaskMsg(int *post_msg, u8 len)
{
    int err = 0;
    int count = 0;

    if(!post_msg || len == 0)
        return -1;

__retry:
    err = os_taskq_post_type(\
        Sensor_Gm_Task_Name, post_msg[0], \
            len - 1, &post_msg[1]);

    if(cpu_in_irq() || cpu_irq_disabled())
        return err;

    if(err) 
    {
        if(!strcmp(os_current_task(), \
            Sensor_Gm_Task_Name)) 
            return err;

        if(count > 20)
            return -1;
        
        count++;
        os_time_dly(1);
        goto __retry;
    }

    return err;
}

/***********地磁航向角**************/
s16 GetSensorGmYaw(void)
{
    return GmYawAngle;
}

void SetSensorGmYaw(s16 yaw)
{
    GmYawAngle = yaw;

    return;
}

/***********地磁使能标志**************/
bool GetSensorGmEnableFlag(void)
{
    return GmEnableFlag;
}

void SetSensorGmEnableFlag(bool f)
{
    GmEnableFlag = f;

    return;
}

/***********地磁校准成功**************/
bool GetSensorGmCaliSucc(void)
{
    return Cali_Info.cali_succ;
}

void SetSensorGmCaliSucc(bool f)
{
    Cali_Info.cali_succ = f;

    return;
}

/***********地磁模块启动/停止**************/
void EnableSensorGmModule(void)
{
    bool GmEnableFlag = \
        GetSensorGmEnableFlag();
    if(GmEnableFlag == true)
        return;

    int SensorGmMsg[1];
	SensorGmMsg[0] = SensorGmMsgEnableModule;
	PostSensorGmTaskMsg(SensorGmMsg, 1); 

    return;
}

void DisableSensorGmModule(void)
{
    bool GmEnableFlag = \
        GetSensorGmEnableFlag();
    if(GmEnableFlag == false)
        return;

    int SensorGmMsg[1];
	SensorGmMsg[0] = SensorGmMsgDisableModule;
	PostSensorGmTaskMsg(SensorGmMsg, 1); 

    return;
}

void SensorGmProcess(void)
{
    static u8 CaliCnt = 0;

    if(GmAccRawIdx == 0)
        SensorGmGsDataFifoRead(GmAccRawX, \
            GmAccRawY, GmAccRawZ, GmGs_Fifo_WM);

#if 0
    printf("x = %d, y = %d, z = %d\n", \
        GmAccRawX[GmAccRawIdx], GmAccRawY[GmAccRawIdx], GmAccRawZ[GmAccRawIdx]);
#endif

    AccCorrect[0] = \
        (float)(GmAccRawX[GmAccRawIdx]*ONE_G)/GetGsACCSsvt();
	AccCorrect[1] = \
        (float)(GmAccRawY[GmAccRawIdx]*ONE_G)/GetGsACCSsvt();
	AccCorrect[2] = \
        (float)(GmAccRawZ[GmAccRawIdx]*ONE_G)/GetGsACCSsvt();
    
    GmAccRawIdx++;
    GmAccRawIdx %= GmGs_Fifo_WM;

    qmc6309_read_mag_xyz(GmRawData);//地磁数据单件为：ut
    if((GmRawData[0] == 0.0f) && (GmRawData[1] == 0.0f) && \
        (GmRawData[2] == 0.0f))  // 处理全0情况。
    {
        GmRawData[0]=1.0f;
        GmRawData[1]=1.0f;
        GmRawData[2]=1.0f;
    }

    fusion_docali(GmRawData, AccCorrect, \
        GmOutput, GmOutputOffset, EulerValue, &QmcAccuracy);
    s16 yaw = (s16)(EulerValue[0] + 0.5f);
	//printf("yaw = %d, QmcAccuracy = %d\n", yaw, QmcAccuracy); //EulerValue[0]:是地磁计算出来的yaw航向角;

    if(QmcAccuracy == 3)
    {
        //8字校准成功 
        CaliCnt = 0;
        SetSensorGmYaw(yaw);
 
        bool *cali_succ = \
            &(Cali_Info.cali_succ);
        if(*cali_succ == false)
        {
            *cali_succ = true;

            Cali_Info.calipara[0] = \
                GmOutputOffset[0];
            Cali_Info.calipara[1] = \
                GmOutputOffset[1];
            Cali_Info.calipara[2] = \
                GmOutputOffset[2];
            Cali_Info.calipara[3] = \
                GmOutputOffset[3];

            SensorGmCaliInfoUpdate();

            printf("Update Cali Para succ\n");
        }
    }else if(QmcAccuracy == 0)
    {
        CaliCnt++;

        if(CaliCnt >= 100) 
        {
            CaliCnt = 0;

            bool *cali_succ = \
                &(Cali_Info.cali_succ);
            *cali_succ = false;
            SensorGmCaliInfoUpdate();
            
            printf("Recalibrate parameters\n");
        }
    }else
    {
        CaliCnt = 0;

        bool *cali_succ = \
            &(Cali_Info.cali_succ);
        *cali_succ = false;
        SensorGmCaliInfoUpdate();
    }

    return;
}

void SensorGmGsDataFifoWrite(u8 *w_buf, u32 w_len)
{
    if(!w_buf || w_len == 0)
        return;

    u8 *pWFifoStr = \
        &GmGs_FifoData[GmGs_WFifoIdx];

    memcpy(pWFifoStr, w_buf, w_len);

    GmGs_RFifoIdx = GmGs_WFifoIdx;

    GmGs_WFifoIdx += GmGs_Fifo_WM*6;
    GmGs_WFifoIdx %= GmGs_FifoSize;

    return;
}

u16 SensorGmGsDataFifoRead(s16 *xdata, s16 *ydata, \
    s16 *zdata, u16 r_max_num)
{
    u16 r_num = 0;

    if(!xdata || !ydata || !zdata || \
        r_max_num == 0)
        return r_num;

    u8 *pRFifoStr = \
        &GmGs_FifoData[GmGs_RFifoIdx];

    u8 FifoData[6];
    s16 AccRawData[3];

    u32 i;
    for(i = 0; i < r_max_num; i++)
    {
        memcpy(FifoData, pRFifoStr, 6);

        AccRawData[0] = (s16)(((uint16_t)FifoData[1] << 8) | \
            (FifoData[0]));
        AccRawData[1] = (s16)(((uint16_t)FifoData[3] << 8) | \
            (FifoData[2]));
        AccRawData[2] = (s16)(((uint16_t)FifoData[5] << 8) | \
            (FifoData[4]));

        xdata[i] = AccRawData[0];
        ydata[i] = AccRawData[1];
        zdata[i] = AccRawData[2];

        pRFifoStr += 6;
    }

    return r_num;
}

void SensorGmCaliInfoRead(void)
{
    int vm_op_len = \
        sizeof(GmCaliInfoPara_t);
    int ret = syscfg_read(CFG_GM_CALIPARA_INFO, \
        &Cali_Info, vm_op_len);
    if(ret != vm_op_len || \
        Cali_Info.vm_mask != Cali_Vm_Mask)
    {
        SensorGmCaliInfoReset();
    }

    return;
}

void SensorGmCaliInfoWrite(void)
{
    int vm_op_len = \
        sizeof(GmCaliInfoPara_t);
    
    for(u8 i = 0; i < 3; i++)
    {
        int ret = syscfg_write(CFG_GM_CALIPARA_INFO, \
            &Cali_Info, vm_op_len);
        if(ret == vm_op_len)
            break;
    }

    return;
}

void SensorGmCaliInfoReset(void)
{
    int vm_op_len = \
        sizeof(GmCaliInfoPara_t);

    memcpy(&Cali_Info, &InitInfo, vm_op_len);
        
    Cali_Info.vm_mask = Cali_Vm_Mask;

    SensorGmCaliInfoUpdate();

    return;
}

void SensorGmCaliInfoUpdate(void)
{
    SensorGmCaliInfoWrite();

    return;
}
