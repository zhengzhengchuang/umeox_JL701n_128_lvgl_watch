#include "acc_data.h"
#include "../lv_watch.h"

#define VM_MASK (0x55ae)

PedoData_t PedoData;
static vm_pedo_ctx_t w_pedo_cache;

#define __this_module (&PedoData)
#define __this_module_size (sizeof(PedoData_t))

static const PedoData_t init = {
    .steps = 0,
    .calorie = 0,
    .distance = 0,
};

static void PedoDataRemoteSend(void)
{
    /* Ble未连接，不上报 */
    u8 BleBtConn = GetDevBleBtConnectStatus();
    if(BleBtConn == 0 || BleBtConn == 2)
        return;

    u8 nfy_buf[Cmd_Pkt_Len];
    memset(nfy_buf, 0x00, Cmd_Pkt_Len);

    u32 steps = GetPedoDataSteps();
    u32 kcal = GetPedoDataKcal();
    u32 dis_m = GetPedoDataDisM();

    le_cmd_t cmd = Cmd_Get_Pedo_Data;

    u8 idx = 0;
    nfy_buf[idx++] = cmd;
    nfy_buf[idx++] = (steps>>16)&(0xff);
    nfy_buf[idx++] = (steps>>8)&(0xff);
    nfy_buf[idx++] = (steps>>0)&(0xff);
    nfy_buf[idx++] = (kcal>>8)&(0xff);
    nfy_buf[idx++] = (kcal>>0)&(0xff);
    nfy_buf[idx++] = (dis_m>>16)&(0xff);
    nfy_buf[idx++] = (dis_m>>8)&(0xff);
    nfy_buf[idx++] = (dis_m>>0)&(0xff);

    u8 crc_idx = Cmd_Pkt_Len - 1;
    nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

    printf_buf(nfy_buf, Cmd_Pkt_Len);
    
    umeox_common_le_notify_data(nfy_buf, Cmd_Pkt_Len);

    return;
}

float KmToMile(float km)
{
    float mile = km*0.621371f;
    return mile;
}

float DistanceCalc(float steps)
{
    float steplen = 0.0f;
    float distance = 0.0f;

    if(steps == 0.0f) 
        return distance;

    u8 gender = User_Info.gender;
    u8 height = User_Info.height;
    if(gender == Gender_Female)
        steplen = height*(0.35f)/100.0f;
    else if(gender == Gender_Male)
        steplen = height*(0.4f)/100.0f;
    else
        steplen = height*(0.4f)/100.0f;

    distance = steplen*steps;//

    return distance;
}

void PedoDataHandle(float steps, float calorie, float distance)
{
    float *psteps = &__this_module->steps;
    float *pcalorie = &__this_module->calorie;
    float *pdistance = &__this_module->distance;

    *psteps += steps;
    *pcalorie += calorie;
    *pdistance += distance;

    if(*psteps > Steps_Max) *psteps = Steps_Max;
    if(*pcalorie > Calorie_Max) *pcalorie = Calorie_Max;
    if(*pdistance > Distance_Max) *pdistance = Distance_Max;
    PedoDataVmWrite();

    /* 主动上报app */
    //PedoDataRemoteSend();

    return;
}

void SetPedoDataVmCtxCache(float steps, float calorie, float distance)
{
    u8 idx = w_pedo.CurIdx;
    if(idx >= Pedo_Day_Num)
        return;

    w_pedo.steps[idx] += steps;
    w_pedo.calorie[idx] += calorie;
    w_pedo.distance[idx] += distance;

    return;
}

void PedoDataClear(void)
{
    PedoDataVmReset();
    return;
}

void VmPedoCtxFlashWrite(void)
{
    if(w_pedo_cache.check_code != 0)
        VmPedoCtxFlashSave(&w_pedo_cache);

    memset(&w_pedo_cache, 0, sizeof(vm_pedo_ctx_t));

    return;
}

void PedoDataMinProcess(struct sys_time *ptime)
{
    bool BondFlag = GetDevBondFlag();
    if(BondFlag == false)
        return;

    if(ptime->hour == 0 && ptime->min == 0)
    {
        PedoDataClear();

        //保存上一天的历史数据
        SetPedoDayVmData(true);
        memcpy(&w_pedo_cache, &w_pedo, sizeof(vm_pedo_ctx_t));
        int ui_msg_post[1];
        ui_msg_post[0] = ui_msg_nor_pedo_write;
        post_ui_msg(ui_msg_post, 1);
    
        //清除缓存数据，开始新一天的数据记录
        memset(&w_pedo, 0, sizeof(vm_pedo_ctx_t));
        w_pedo.check_code = Nor_Vm_Check_Code;
        w_pedo.timestamp = UtcTimeToSec(ptime);
    }

    u32 timestamp = ptime->hour*60 + ptime->min;
    w_pedo.CurIdx = timestamp/Pedo_Inv_Dur;
    
    return;
}

//false 没过期  true 过期
static bool PedoVmDataIsPast(u32 timestamp)
{
    if(w_pedo.timestamp < timestamp) 
        goto __end;

    if(w_pedo.timestamp - timestamp < SEC_PER_DAY) 
        return false;

__end:      
    return true;
}

static bool GetPedoData(void)
{
    bool ret = false;

    uint8_t num = VmPedoItemNum();
    if(num == 0) goto __end;

    u8 idx = num - 1;
    bool r_ret = VmPedoCtxByIdx(idx);
    if(r_ret == false)
        goto __end;

    ret = true;

__end:
    return ret;
}

void WPedoParaInit(void)
{
    memset(&w_pedo, 0, sizeof(vm_pedo_ctx_t));
    w_pedo.check_code = Nor_Vm_Check_Code;
    struct sys_time time;
    GetUtcTime(&time);
    w_pedo.CurIdx = (time.hour*60 + time.min)/Pedo_Inv_Dur;
    time.hour = 0; time.min = 0; time.sec = 0;
    w_pedo.timestamp = UtcTimeToSec(&time);

    return;
}

void PowerOnSetPedoVmCache(void)
{
    printf("%s:%d\n", __func__, sizeof(vm_pedo_ctx_t));

    WPedoParaInit();
    
    /*读取vm的最新一条数据*/
    bool data_ret = GetPedoData();
    if(data_ret == false) return;
    printf("%s:data_ret = %d\n", __func__, data_ret);

    u8 num = VmPedoItemNum();

    /*判断vm的最新数据是否已经过期*/
    bool IsPast = PedoVmDataIsPast(r_pedo.timestamp);
    printf("%s:IsPast = %d\n", __func__, IsPast);
    if(IsPast == true)
    {
        //过期数据，需要清除
        PedoDataVmReset();

        //如果说已经过期数据，且存储数超过上限，删除最旧一条
        if(num > Pedo_Max_Days)
        {
            u8 del_idx = 0;
            VmPedoCtxDelByIdx(del_idx);
        }

        return;
    }

    //删除副本数据，继续新的vm缓存
    u8 del_idx = num - 1;
    VmPedoCtxDelByIdx(del_idx);

    //拷贝保存的数据
    memcpy(w_pedo.steps, r_pedo.steps, sizeof(r_pedo.steps));
    memcpy(w_pedo.calorie, r_pedo.calorie, sizeof(r_pedo.calorie));
    memcpy(w_pedo.distance, r_pedo.distance, sizeof(r_pedo.distance));

    return;
}

void PowerOffSetPedoVmFlashSave(void)
{
    SetPedoDayVmData(false);
    memcpy(&w_pedo_cache, &w_pedo, sizeof(vm_pedo_ctx_t));
    int ui_msg_post[1];
    ui_msg_post[0] = ui_msg_nor_pedo_write;
    post_ui_msg(ui_msg_post, 1);

    return;
}

void PedoDataVmRead(void)
{
    int ret = syscfg_read(CFG_PEDO_DATA_INFO, \
        __this_module, __this_module_size);
    if(ret != __this_module_size || __this_module->vm_mask != VM_MASK)
        PedoDataVmReset();

    int CommMsg[1];
    CommMsg[0] = comm_msg_gomore_init;
    PostCommTaskMsg(CommMsg, 1);

    return;
}

void PedoDataVmWrite(void)
{
    for(u8 i = 0; i < 3; i++)
    {
        int ret = syscfg_write(CFG_PEDO_DATA_INFO, __this_module, __this_module_size);
        if(ret == __this_module_size)
            break;
    }

    return;
}

void PedoDataVmReset(void)
{
    memcpy(__this_module, &init, __this_module_size);

    __this_module->vm_mask = VM_MASK;

    PedoDataVmWrite();

    return;
}