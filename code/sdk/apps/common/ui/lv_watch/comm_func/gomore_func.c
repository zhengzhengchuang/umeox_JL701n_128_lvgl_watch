#include "../lv_watch.h"

extern void swapX(const uint8_t *src, uint8_t *dst, int len);

IndexIO mInput;

/****gomore与加速度计参数配套*****/
#define FifoSize (GoGs_Fifo_WM*6*5)
static u32 WFifoIdx;
static u32 RFifoIdx;
static u8 WFifoData[FifoSize];
static u8 RFifoData[FifoSize];

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
 
void GoMoreAlgoInit(void)
{
    /* 设备无绑定，不初始化算法 */
    bool BondFlag = GetDevBondFlag();
    if(BondFlag == false)
        return;

    /* 解析设备id */
    deviceIdExample = GoMoreDevIdParse();
    printf("deviceIdExample = %s\n", deviceIdExample);

    struct sys_time utc_time;
    GetUtcTime(&utc_time);
    int rtc_current_time = (int)UtcTimeToSec(&utc_time);
    printf("rtc_current_time = %d\n", rtc_current_time);

    //Sdk Auth
    workoutAuthParameters auth;
    setWorkoutAuthParametersExample(rtc_current_time, &auth);
    int16_t ret = setAuthParameters(&auth);
    printf("%s:ret = %d\n", __func__, ret);
    if(ret < 0) return;

    // Allocate Memory 
    u32 MemSize = getMemSizeHealthFrame();
    u32 DataSize = getPreviousDataSize();
    printf("MemSize = %d, DataSize = %d\n", MemSize, DataSize);
    void *sdkMem = malloc(MemSize);
    char *prevData = (char*)malloc(DataSize);
    printf("sdkMem = %p, prevData = %p\n", sdkMem, prevData);

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
    printf("%s:r = %d\n", __func__, r);

    setIndexIODefaultValue(&mInput);
    mInput.timestamp = rtc_current_time;

    return;
}

void GoGsDataFifoWrite(u8 *w_buf, u32 w_len)
{
    bool BondFlag = GetDevBondFlag();
    if(BondFlag == false) 
        return;

    /*一定保证fifo是中断水印的整数倍*/
    u8 *pWFifo = &WFifoData[WFifoIdx];
    memcpy(pWFifo, w_buf, w_len);
    WFifoIdx += w_len;
    WFifoIdx %= FifoSize;

    u8 *pRFifo = &RFifoData[RFifoIdx];
    if(RFifoIdx < FifoSize)
    {
        memcpy(pRFifo, w_buf, w_len);
        RFifoIdx += w_len;
    }else
    {
        u8 last_idx = (FifoSize/(GoGs_Fifo_WM*6)) - 1;
        for(u8 i = 0; i < last_idx; i++)
            memcpy(&RFifoData[i], &RFifoData[i + 1], GoGs_Fifo_WM*6);
        memcpy(&RFifoData[last_idx], w_buf, w_len);
        RFifoIdx = FifoSize;
    }

    return;
}

u16 GoGsDataFifoRead(float *acc_x, float *acc_y, float *acc_z, u16 max_len)
{
    u16 r_len = 0;

    memset(acc_x, 0, max_len*sizeof(float));
    memset(acc_y, 0, max_len*sizeof(float));
    memset(acc_z, 0, max_len*sizeof(float));

    r_len = RFifoIdx/6;
    RFifoIdx = 0;

    if(r_len > max_len)
       r_len = max_len;

    u8 *pRFifo = RFifoData;

    u8 FifoData[6];
    s16 AccRawData[3];

    u32 i;
    for(i = 0; i < r_len; i++)
    {
        memcpy(FifoData, pRFifo, 6);

        AccRawData[0] = (s16)(((u16)FifoData[1]<<8)|(FifoData[0]));
        AccRawData[1] = (s16)(((u16)FifoData[3]<<8)|(FifoData[2]));
        AccRawData[2] = (s16)(((u16)FifoData[5]<<8)|(FifoData[4]));

        acc_x[i] = (float)((-1)*AccRawData[0]*1000.0f)/GetGsACCSsvt();
        acc_y[i] = (float)((-1)*AccRawData[1]*1000.0f)/GetGsACCSsvt();
        acc_z[i] = (float)(AccRawData[2]*1000.0f)/GetGsACCSsvt();

        pRFifo += 6;
    }

    return r_len;
}
