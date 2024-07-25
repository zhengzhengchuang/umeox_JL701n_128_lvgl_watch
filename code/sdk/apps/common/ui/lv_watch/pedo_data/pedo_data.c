#include "acc_data.h"
#include "../lv_watch.h"

#define VM_MASK (0x55ab)

PedoData_t PedoData;
static vm_pedo_ctx_t w_pedo_cache;

#define __this_module (&PedoData)
#define __this_module_size (sizeof(PedoData_t))

static const PedoData_t init = {
    .steps = 0,
    .calorie = 0,
    .distance = 0,
};

#define ACC_MAX 40
static float accX[ACC_MAX];
static float accY[ACC_MAX];
static float accZ[ACC_MAX];

static void PedoDataStore(float steps, float calorie, float distance)
{
    if(steps == 0.0f && calorie == 0.0f && distance == 0.0f)
        return;

    __this_module->steps += (u32)steps;
    __this_module->calorie += (u32)calorie;
    __this_module->distance += (u32)distance;

    if(__this_module->steps > Steps_Max)
        __this_module->steps = Steps_Max;

    if(__this_module->calorie > Calorie_Max)
        __this_module->calorie = Calorie_Max;

    if(__this_module->distance > Distance_Max)
        __this_module->distance = Distance_Max;

    PedoDataVmWrite();

    return;
}

static void SetPedoDataVmCtxCache(float steps, float calorie, float distance)
{
    if(steps == 0.0f && calorie == 0.0f && distance == 0.0f)
        return;
    u8 idx = w_pedo.CurIdx;
    if(idx >= Pedo_Day_Num)
        return;

    w_pedo.steps[idx] += (u32)steps;
    w_pedo.calorie[idx] += (u32)calorie;
    w_pedo.distance[idx] += (u32)distance;
    return;
}

void PedoDataClear(void)
{
    PedoDataVmReset();
    return;
}

void PedoDataAlgoProcess(struct sys_time *ptime)
{
    bool BondFlag = GetDevBondFlag();
    if(BondFlag == false)
        return;

    u16 acc_len = GoGsDataFifoRead(accX, accY, accZ, ACC_MAX);

    mInput.timestamp = (int)UtcTimeToSec(ptime);

    mInput.accX = accX;
    mInput.accY = accY;
    mInput.accZ = accZ;
    mInput.accLength = acc_len;
    int16_t flag = updateIndex(&mInput);
    if(flag < 0) return;
    float steps = mInput.stepCountOut;
    float calorie = mInput.kcalOut;
    float distance = 0.0f;
    
    SedSetSteps(steps);
    PedoDataStore(steps, calorie, distance);
    SetPedoDataVmCtxCache(steps, calorie, distance);

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
        memcpy(&(w_pedo.time), ptime, sizeof(struct sys_time));
    }

    u16 timestamp = ptime->hour*60 + ptime->min;
    w_pedo.CurIdx = timestamp/Pedo_Inv_Dur;
    
    return;
}

//false 没过期  true 过期
static bool PedoVmDataIsPast(struct sys_time *ptime)
{
    //今天
    struct sys_time *time1 = &(w_pedo.time);
    struct sys_time *time2 = ptime;

    if(time1->year == time2->year && time1->month == time2->month && \
        time1->day == time2->day)
    {
        return false;
    }
        
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

void PowerOnSetPedoVmCache(void)
{
    printf("%s:%d\n", __func__, sizeof(vm_pedo_ctx_t));

    /*初始化今天的vm缓存参数*/
    memset(&w_pedo, 0, sizeof(vm_pedo_ctx_t));
    w_pedo.check_code = Nor_Vm_Check_Code;
    GetUtcTime(&(w_pedo.time));
    w_pedo.CurIdx = (w_pedo.time.hour*60 + w_pedo.time.min)/Pedo_Inv_Dur;

    u8 num = VmPedoItemNum();
    printf("%s:num = %d\n", __func__, num);

    /*读取vm的最新一条数据*/
    bool data_ret = GetPedoData();
    if(!data_ret) return;
    printf("%s:data_ret = %d\n", __func__, data_ret);

    /*判断vm的最新数据是否已经过期*/
    bool IsPast = PedoVmDataIsPast(&(r_pedo.time));
    printf("%s:IsPast = %d\n", __func__, IsPast);
    if(IsPast == true)
    {
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

    // for(u8 i = 0; i < Pedo_Day_Num; i++)
    //     printf("steps[%d] = %d\n", i, w_pedo.steps[i]);

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

    GoMoreAlgoInit();

    return;
}

void PedoDataVmWrite(void)
{
    for(u8 i = 0; i < 3; i++)
    {
        int ret = syscfg_write(CFG_PEDO_DATA_INFO, \
            __this_module, __this_module_size);
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