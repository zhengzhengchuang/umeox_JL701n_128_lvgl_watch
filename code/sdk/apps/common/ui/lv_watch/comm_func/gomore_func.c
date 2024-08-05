#include "../lv_watch.h"

extern void swapX(const uint8_t *src, uint8_t *dst, int len);

static IndexIO mInput;

static void *sdkMem = NULL;
static char *prevData = NULL;
static bool algo_init_flag = false;

#define ACC_MAX 40
static float accX[ACC_MAX];
static float accY[ACC_MAX];
static float accZ[ACC_MAX];

#define FifoSize (GoGs_Fifo_WM*6*5)
static u32 WFifoIdx;
static u32 RFifoIdx;
static u8 WFifoData[FifoSize];
static u8 RFifoData[FifoSize];

static int8_t slp_stages[2880];

static char *GoMoreDevIdParse(void)
{
    static char id_buf[13];
    memset(id_buf, 0, 13);

    static u8 mac_addr[6];
    memset(mac_addr, 0, 6);
    swapX(GetDevBleMac(), mac_addr, 6);

    u8 buf_idx = 0;
    u8 mac_idx = 0;
    for(u8 i = 0; i < 17; i++)
    {
        if(((i + 1) % 3) == 0)
		{
            mac_idx++;
			continue;
		}else if(((i + 1) % 3) == 1)
		{
			id_buf[buf_idx] = ((mac_addr[mac_idx]>>4)&0x0f) > 9 ? \
			    ((mac_addr[mac_idx]>>4)&0x0f) + 0x37:((mac_addr[mac_idx] >> 4)&0x0f) + 0x30;
		}else if(((i + 1) % 3) == 2)
		{
			id_buf[buf_idx] = (mac_addr[mac_idx]&0x0f) > 9 ? \
			    ((mac_addr[mac_idx])&0x0f) + 0x37:(mac_addr[mac_idx]&0x0f) + 0x30;
		}

        buf_idx++;
    }

    return id_buf;
}

int8_t gomore_pkey_get(char *pKey, uint8_t Size)
{
    if(pKeyExample == NULL)
        return -2;

    Size = strlen(pKeyExample);

    memcpy(pKey, pKeyExample, Size);

    return Size;
}

int8_t gomore_device_id_get(char *Device_ID, uint8_t Size)
{
    if(deviceIdExample == NULL)
        return -2;

    Size = strlen(deviceIdExample);

    memcpy(Device_ID, deviceIdExample, Size);

    return Size;
}

void GoMoreAlgoProcess(struct sys_time *ptime)
{
    if(algo_init_flag == true) return;

    u16 acc_len = GoGsDataFifoRead(accX, accY, accZ, ACC_MAX);
    // for(u16 i = 0; i < acc_len; i++)
    //     printf("%d:%f  %f  %f\n", i, accX[0], accY[0], accZ[1]);

    mInput.timestamp = (int)UtcTimeToSec(ptime);
    int time_zone = GetVmParaCacheByLabel(vm_label_time_zone);
    mInput.timeZoneOffset = (int16_t)((time_zone/10.0f)*60);
    
    /* acc数据 */
    mInput.accX = accX;
    mInput.accY = accY;
    mInput.accZ = accZ;
    mInput.accLength = acc_len;

    /* ppg数据 */
    // mInput.ppg1 = GetSleepPpgRawData();
    // mInput.ppgLength = GetSleepPpgLen();
    // mInput.ppgNumChannels = 1;

    int16_t flag = updateIndex(&mInput);
    if(flag < 0) return;

    /* pedo data */
    SedSetSteps(mInput.stepCountOut);
    PedoDataHandle(mInput.stepCountOut, mInput.kcalOut, DistanceCalc(mInput.stepCountOut));
    SetPedoDataVmCtxCache(mInput.stepCountOut, mInput.kcalOut, DistanceCalc(mInput.stepCountOut));

    /* sleep相关 */
    SleepStatusOutHandle(mInput.sleepPeriodStatusOut);
    SleepPpgSensorOnOff(mInput.sleepStagePpgOnOffOut);

    return;
}
 
static void GoMoreAlgoUninit(void)
{
    if(sdkMem)
    {
        stopHealthSession(sdkMem);
        free(sdkMem);
        sdkMem = NULL;
    } 

    if(prevData)
    {
        free(prevData);
        prevData = NULL;
    }

    printf("_____GoMoreAlgoUninit\n");

    return;
}

void GoMoreAlgoInit(void)
{
    /* 设备无绑定，不初始化算法 */
    bool BondFlag = GetDevBondFlag();
    if(BondFlag == false)
        return;

    if(algo_init_flag == true)
        GoMoreAlgoUninit();

    /* 解析设备id */
    //deviceIdExample = GoMoreDevIdParse();
    printf("deviceIdExample = %s\n", deviceIdExample);

    struct sys_time utc_time;
    GetUtcTime(&utc_time);
    int rtc_current_time = (int)UtcTimeToSec(&utc_time);
    printf("rtc_current_time = %d\n", rtc_current_time);

    //Sdk Auth
    workoutAuthParameters auth;
    setWorkoutAuthParametersExample(rtc_current_time, &auth);
    int16_t ret = setAuthParameters(&auth);
    if(ret < 0) return;

    // Allocate Memory 
    u32 MemSize = getMemSizeHealthFrame();
    u32 DataSize = getPreviousDataSize();
    sdkMem = malloc(MemSize);
    prevData = (char*)malloc(DataSize);

    memset(prevData, 0, DataSize);

    /* 算法用户信息初始化 */
    float userInfo[7];
    userInfo[0] = User_Info.age*(1.0f);
    userInfo[1] = User_Info.gender*(1.0f);
    userInfo[2] = User_Info.height*(1.0f);
    userInfo[3] = User_Info.weight*(1.0f);
    userInfo[4] = (220 - User_Info.age)*(1.0f);
    userInfo[5] = 70.0f;
    userInfo[6] = 40.0f;
    int r = healthIndexInitUser(sdkMem, rtc_current_time, userInfo, prevData);
    if(r < 0) return;
    // for(u8 i = 0; i < 7; i++)
    //     printf("userInfo[%d] = %f\n", i, userInfo[i]);

    /* 上电先关闭自动检测，时间范围：20:00~8:00 */
    SleepConfig slp_cfg;
    getSleepConfig(0, &slp_cfg);
    slp_cfg.disableAutoDetection = true;
    int16_t cfg = setSleepConfig(&slp_cfg);
    if(cfg == 0) printf("setSleepConfig success\n");

    slp_out.stages = slp_stages;
    // printf("minNapLength = %d\n", slp_cfg.minNapLength);
    // printf("maxGapLength = %d\n", slp_cfg.maxGapLength);
    // printf("disablePeriodFinetuning = %d\n", slp_cfg.disablePeriodFinetuning);
    // printf("disableAutoDetection = %d\n", slp_cfg.disableAutoDetection);
    // printf("longSleepPeriod[0] = %d\n", slp_cfg.longSleepPeriod[0]);
    // printf("longSleepPeriod[1] = %d\n", slp_cfg.longSleepPeriod[1]);

    setIndexIODefaultValue(&mInput);
    //mInput.timestamp = rtc_current_time;
    // int time_zone = GetVmParaCacheByLabel(vm_label_time_zone);
    // mInput.timeZoneOffset = (int16_t)((time_zone/10.0f)*60);
  
    algo_init_flag = true;

    printf("+++++++algo_init_flag\n");

    return;
}

static bool GsReading = false;
void GoGsDataFifoWrite(u8 *w_buf, u32 w_len)
{
    bool BondFlag = GetDevBondFlag();
    if(BondFlag == false) 
        return;

    if(GsReading == true) return;

    /*一定保证fifo是中断水印的整数倍*/
    u8 *pWFifo = &WFifoData[WFifoIdx];
    if(WFifoIdx + w_len <= FifoSize)
    {
        memcpy(pWFifo, w_buf, w_len);
        WFifoIdx += w_len;
    }else
    {
        u8 last_idx = (FifoSize/(GoGs_Fifo_WM*6)) - 1;
        for(u8 i = 0; i < last_idx; i++)
            memcpy(&WFifoData[i], &WFifoData[i + 1], GoGs_Fifo_WM*6);
        memcpy(&WFifoData[last_idx], w_buf, w_len);
        WFifoIdx = FifoSize;
    }

    return;
}

u16 GoGsDataFifoRead(float *acc_x, float *acc_y, float *acc_z, u16 max_len)
{
    u16 r_len = 0;

    memset(acc_x, 0, max_len*sizeof(float));
    memset(acc_y, 0, max_len*sizeof(float));
    memset(acc_z, 0, max_len*sizeof(float));

    GsReading = true;
    RFifoIdx = WFifoIdx;
    u8 *pRFifo = RFifoData;
    u8 *pWFifo = WFifoData;
    memcpy(pRFifo, pWFifo, RFifoIdx);
    WFifoIdx = 0;
    GsReading = false;

    r_len = RFifoIdx/6;
    if(r_len > max_len)
       r_len = max_len;

    u8 FifoData[6];
    s16 AccRawData[3];

    u32 i;
    for(i = 0; i < r_len; i++)
    {
        memcpy(FifoData, pRFifo, 6);

        AccRawData[0] = (s16)(((u16)FifoData[1]<<8)|(FifoData[0]));
        AccRawData[1] = (s16)(((u16)FifoData[3]<<8)|(FifoData[2]));
        AccRawData[2] = (s16)(((u16)FifoData[5]<<8)|(FifoData[4]));

        acc_x[i] = (float)((AccRawData[0]*1000.0f)/GetGsACCSsvt());
        acc_y[i] = (float)((AccRawData[1]*1000.0f)/GetGsACCSsvt());
        acc_z[i] = (float)((AccRawData[2]*1000.0f)/GetGsACCSsvt());

        pRFifo += 6;
    }

    return r_len;
}
