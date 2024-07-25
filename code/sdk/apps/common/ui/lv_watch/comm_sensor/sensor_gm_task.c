#include "../lv_watch.h"

#define ONE_G (9.807f)
#define Cali_Vm_Mask (0x55bb)

GmCaliInfoPara_t Cali_Info;
static const GmCaliInfoPara_t InitInfo = {
    .cali_succ = false,
    .calipara = {0.0f},
};

/****地磁与加速度计参数配套*****/
#define FifoSize (GmGs_Fifo_WM*6*3)
static u32 WFifoIdx;
static u32 RFifoIdx;
static u8 FifoData[FifoSize];

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

static int8_t QmcAccuracy;

static void GmGsFifoParaInit(void)
{
    WFifoIdx = 0;
    RFifoIdx = 0;
    memset(FifoData, 0, FifoSize);

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

static u16 gm_timer;
static void TimerCb(void *priv)
{
    int GmMsg[2];
	GmMsg[0] = GmMsgProcess;
	PostGmTaskMsg(GmMsg, 1);

    return;
}

static void GmTimerCreate(void)
{
    if(gm_timer == 0)
        gm_timer = sys_hi_timer_add(NULL, TimerCb, 40);

    return;
}

static void GmTimerDestory(void)
{
    if(gm_timer)
        sys_hi_timer_del(gm_timer);

    gm_timer = 0;

    return;
}

static void EnableGmModuleHandle(void)
{
    qmc6309_enable();
    GmAccRawDataInit();
    SetGmEnableFlag(true);     
    GmTimerCreate();

    return;
}

static void DisableGmModuleHandle(void)
{
    qmc6309_disable();
    SetGmEnableFlag(false);
    GmTimerDestory();

    return;
}

static void GmProcess(void)
{
    static u8 CaliCnt = 0;

    if(GmAccRawIdx == 0)
        GmGsDataFifoRead(GmAccRawX, GmAccRawY, GmAccRawZ, GmGs_Fifo_WM);

#if 0
    printf("x = %d, y = %d, z = %d\n", \
        GmAccRawX[GmAccRawIdx], GmAccRawY[GmAccRawIdx], GmAccRawZ[GmAccRawIdx]);
#endif

    AccCorrect[0] = (float)(GmAccRawX[GmAccRawIdx]*ONE_G)/GetGsACCSsvt();
	AccCorrect[1] = (float)(GmAccRawY[GmAccRawIdx]*ONE_G)/GetGsACCSsvt();
	AccCorrect[2] = (float)(GmAccRawZ[GmAccRawIdx]*ONE_G)/GetGsACCSsvt();
    
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

    fusion_docali(GmRawData, AccCorrect, GmOutput, GmOutputOffset, EulerValue, &QmcAccuracy);
    s16 yaw = (s16)(EulerValue[0] + 0.5f);
	//printf("yaw = %d, QmcAccuracy = %d\n", yaw, QmcAccuracy); //EulerValue[0]:是地磁计算出来的yaw航向角;

    if(QmcAccuracy == 3)
    {
        //8字校准成功 
        CaliCnt = 0;
        SetGmYawAngle(yaw);
 
        bool *cali_succ = &(Cali_Info.cali_succ);
        if(*cali_succ == false)
        {
            *cali_succ = true;

            Cali_Info.calipara[0] = GmOutputOffset[0];
            Cali_Info.calipara[1] = GmOutputOffset[1];
            Cali_Info.calipara[2] = GmOutputOffset[2];
            Cali_Info.calipara[3] = GmOutputOffset[3];

            GmCaliInfoUpdate();

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
            GmCaliInfoUpdate();
            
            printf("Recalibrate parameters\n");
        }
    }else
    {
        CaliCnt = 0;

        bool *cali_succ = \
            &(Cali_Info.cali_succ);
        *cali_succ = false;
        GmCaliInfoUpdate();
    }

    return;
}

static void GmTask(void *p)
{
    int ret;
    int msg[8] = {0};

    GmGsFifoParaInit();
    GmAccRawDataInit();

    GmCaliInfoRead();

    qst_ical_init(Cali_Info.calipara);
	fusion_enable();

    /*开机先失能模块*/
    DisableGmModule();

    while(1)
    {
        ret = os_taskq_pend(NULL, msg, ARRAY_SIZE(msg)); 

        if(ret == OS_TASKQ)
            GmTaskMsgHandle(msg, ARRAY_SIZE(msg));
    }
    
    return;
}

void GmTaskCreate(void)
{
    int err = task_create(GmTask, NULL, Gm_Task_Name);
    if(err) printf("Gm task create err!!!!!!!:%d \n", err);

    return;
}

void GmTaskMsgHandle(int *msg, u8 len)
{
    if(!msg || len == 0)
        return;

    int cmd = msg[0];

    switch(cmd)
    {
        case GmMsgProcess:
            GmProcess();
            break;
        
        case GmMsgEnable:
            EnableGmModuleHandle();
            break;

        case GmMsgDisable:
            DisableGmModuleHandle(); 
            break;

        default:
            printf("*************Gm msg not found\n");
            break;
    }

    return;
}

int PostGmTaskMsg(int *msg, u8 len)
{
    int err = 0;
    int count = 0;

    if(!msg || len == 0)
        return -1;

__retry:
    err = os_taskq_post_type(Gm_Task_Name, msg[0], \
        len - 1, &msg[1]);

    if(cpu_in_irq() || cpu_irq_disabled())
        return err;

    if(err) 
    {
        if(!strcmp(os_current_task(), Gm_Task_Name)) 
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
static s16 YawAngle;
s16 GetGmYawAngle(void)
{
    return YawAngle;
}

void SetGmYawAngle(s16 yaw)
{
    YawAngle = yaw;
    return;
}

/***********地磁使能标志**************/
static bool GmEn;
bool GetGmEnableFlag(void)
{
    return GmEn;
}

void SetGmEnableFlag(bool f)
{
    GmEn = f;
    return;
}

/***********地磁校准成功**************/
bool GetGmCaliSucc(void)
{
    return Cali_Info.cali_succ;
}

void SetGmCaliSucc(bool f)
{
    Cali_Info.cali_succ = f;

    return;
}

/***********地磁模块启动/停止**************/
void EnableGmModule(void)
{
    bool GmEn = GetGmEnableFlag();
    if(GmEn == true)
        return;

    int GmMsg[2];
	GmMsg[0] = GmMsgEnable;
	PostGmTaskMsg(GmMsg, 1); 

    return;
}

void DisableGmModule(void)
{
    bool GmEn = GetGmEnableFlag();
    if(GmEn == false)
        return;

    int GmMsg[2];
	GmMsg[0] = GmMsgDisable;
	PostGmTaskMsg(GmMsg, 1); 

    return;
}

void GmGsDataFifoWrite(u8 *w_buf, u32 w_len)
{
    if(!w_buf || w_len == 0)
        return;

    u8 *pWFifoStr = &FifoData[WFifoIdx];

    memcpy(pWFifoStr, w_buf, w_len);

    RFifoIdx = WFifoIdx;

    WFifoIdx += w_len;
    WFifoIdx %= FifoSize;

    return;
}

u16 GmGsDataFifoRead(s16 *xdata, s16 *ydata, s16 *zdata, u16 max_len)
{
    u16 r_len = 0;

    u8 *pRFifoStr = &FifoData[RFifoIdx];

    u8 FifoData[6];
    s16 AccRawData[3];

    u32 i;
    for(i = 0; i < max_len; i++)
    {
        memcpy(FifoData, pRFifoStr, 6);

        AccRawData[0] = (s16)(((u16)FifoData[1]<<8)|(FifoData[0]));
        AccRawData[1] = (s16)(((u16)FifoData[3]<<8)|(FifoData[2]));
        AccRawData[2] = (s16)(((u16)FifoData[5]<<8)|(FifoData[4]));

        xdata[i] = AccRawData[0];
        ydata[i] = AccRawData[1];
        zdata[i] = AccRawData[2];

        pRFifoStr += 6;
    }

    return r_len;
}

void GmCaliInfoRead(void)
{
    int vm_op_len = sizeof(GmCaliInfoPara_t);
    int ret = syscfg_read(CFG_GM_CALIPARA_INFO, \
        &Cali_Info, vm_op_len);
    if(ret != vm_op_len || Cali_Info.vm_mask != Cali_Vm_Mask)
        GmCaliInfoReset();

    return;
}

void GmCaliInfoWrite(void)
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

void GmCaliInfoReset(void)
{
    int vm_op_len = sizeof(GmCaliInfoPara_t);

    memcpy(&Cali_Info, &InitInfo, vm_op_len);
        
    Cali_Info.vm_mask = Cali_Vm_Mask;

    GmCaliInfoUpdate();

    return;
}

void GmCaliInfoUpdate(void)
{
    GmCaliInfoWrite();

    return;
}
