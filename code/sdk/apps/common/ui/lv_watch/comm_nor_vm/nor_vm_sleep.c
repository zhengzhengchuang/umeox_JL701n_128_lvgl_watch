#include "nor_vm_main.h"
#include "../lv_watch.h"

vm_sleep_ctx_t w_sleep;
vm_sleep_ctx_t r_sleep;
static const nor_vm_type_t nor_vm_type = \
    nor_vm_type_sleep;

/*********************************************************************************
                              清除                                         
*********************************************************************************/
void VmSleepCtxClear(void)
{ 
    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);

    if(!nor_vm_file)
        return;

    u8 num = VmSleepItemNum();

    while(num)
    {
        flash_common_delete_by_index(nor_vm_file, 0);
        num--;
    }
    
    return;
}

/*********************************************************************************
                              存储数量                                         
*********************************************************************************/
u8 VmSleepItemNum(void)
{
    u8 num = 0;

    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);

    if(!nor_vm_file)
        return num;

    num = flash_common_get_total(nor_vm_file);

    if(num > Slp_Max_Days)
        num = Slp_Max_Days;

    return num;
}

/*********************************************************************************
                              获取内容                                        
*********************************************************************************/
bool VmSleepCtxByIdx(u8 idx)
{
    u8 num = VmSleepItemNum();
    if(num == 0) return false;

    if(idx >= num) return false;
    
    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);
    int ctx_len = sizeof(vm_sleep_ctx_t);
    
    if(!nor_vm_file) return false;

    memset(&r_sleep, 0, ctx_len);
    flash_common_read_by_index(nor_vm_file, idx, 0, \
        ctx_len, (u8 *)&r_sleep);

    if(r_sleep.check_code != Nor_Vm_Check_Code)
        return false;

    return true;
}

/*********************************************************************************
                              内容存储                                   
*********************************************************************************/
void VmSleepCtxFlashSave(void *p)
{
    bool BondFlag = GetDevBondFlag();
    if(BondFlag == false)
        return;

    if(!p) return;

    void *nor_vm_file = \
        nor_flash_vm_file(nor_vm_type);
    int ctx_len = sizeof(vm_sleep_ctx_t);

    if(!nor_vm_file) return;

    u8 num = VmSleepItemNum();

    printf("num = %d\n", num);
    
    if(num >= Slp_Max_Sec)
        flash_common_delete_by_index(nor_vm_file, 0);

    flash_common_write_file(nor_vm_file, 0, ctx_len, (u8 *)p);

    return;
}

#if 1//NOR_DEBUG_DATA
static vm_sleep_ctx_t sleep_test[7];
static const u16 slp_dur[20] = {
    1440, 1800, 1800, 2400, 3000, 1200, 1200, 1800, 1800, 900, \
    2100, 1800, 1200, 1200, 1380, 600, 1200, 600, 480, 900,
};
static const u8 slp_state[20] = {
    2, 3, 1, 2, 1, 2, 3, 2, 0, 2, \
    3, 1, 0, 2, 3, 1, 2, 0, 2, 0,
};
void sleep_data_test(void)
{
    u8 idx = 0;
    /* 第一天 */
    struct sys_time s_time;
    s_time.year = 2024; s_time.month = 8; s_time.day = 12; 
    s_time.hour = 23; s_time.min = 30; s_time.sec = 0;
    u32 total_start_ts = UtcTimeToSec(&s_time);

    struct sys_time e_time;
    e_time.year = 2024; e_time.month = 8; e_time.day = 13; 
    e_time.hour = 7; e_time.min = 30; e_time.sec = 0;
    u32 total_end_ts = UtcTimeToSec(&e_time);
    sleep_test[idx].check_code = Nor_Vm_Check_Code;
    sleep_test[idx].SecNum = 20;
    sleep_test[idx].timestamp = 1723492800; //2024年8月12日20时的数据
    sleep_test[idx].total_start_ts = total_start_ts;
    sleep_test[idx].total_end_ts = total_end_ts;
    //printf("0:total_start_ts = %d, total_end_ts = %d\n", total_start_ts, total_end_ts);

    u32 ts = total_start_ts;
    u32 es = ts;
    for(u8 i = 0; i < 20; i++){
        sleep_test[idx].ctx[i].stage = slp_state[i];
        sleep_test[idx].ctx[i].start_ts = ts;
        sleep_test[idx].ctx[i].end_ts = es + slp_dur[i];
        //printf("%s:sec = %d, ts = %d, es = %d\n, ", __func__, i, sleep_test[0].ctx[i].start_ts, sleep_test[0].ctx[i].end_ts);
        ts += slp_dur[i];
        es = ts;
    }

    /* 第二天 */
    idx++;
    s_time.year = 2024; s_time.month = 8; s_time.day = 13; 
    s_time.hour = 23; s_time.min = 30; s_time.sec = 0;
    total_start_ts = UtcTimeToSec(&s_time);

    e_time.year = 2024; e_time.month = 8; e_time.day = 14; 
    e_time.hour = 7; e_time.min = 30; e_time.sec = 0;
    total_end_ts = UtcTimeToSec(&e_time);
    sleep_test[idx].check_code = Nor_Vm_Check_Code;
    sleep_test[idx].SecNum = 20;
    sleep_test[idx].timestamp = 1723579200; //2024年8月13日20时的数据
    sleep_test[idx].total_start_ts = total_start_ts;
    sleep_test[idx].total_end_ts = total_end_ts;
    //printf("0:total_start_ts = %d, total_end_ts = %d\n", total_start_ts, total_end_ts);

    ts = total_start_ts;
    es = ts;
    for(u8 i = 0; i < 20; i++){
        sleep_test[idx].ctx[i].stage = slp_state[i];
        sleep_test[idx].ctx[i].start_ts = ts;
        sleep_test[idx].ctx[i].end_ts = es + slp_dur[i];
        //printf("%s:sec = %d, ts = %d, es = %d\n, ", __func__, i, sleep_test[0].ctx[i].start_ts, sleep_test[0].ctx[i].end_ts);
        ts += slp_dur[i];
        es = ts;
    }

    /* 第三天 */
    idx++;
    s_time.year = 2024; s_time.month = 8; s_time.day = 14; 
    s_time.hour = 23; s_time.min = 30; s_time.sec = 0;
    total_start_ts = UtcTimeToSec(&s_time);

    e_time.year = 2024; e_time.month = 8; e_time.day = 15; 
    e_time.hour = 7; e_time.min = 30; e_time.sec = 0;
    total_end_ts = UtcTimeToSec(&e_time);
    sleep_test[idx].check_code = Nor_Vm_Check_Code;
    sleep_test[idx].SecNum = 20;
    sleep_test[idx].timestamp = 1723665600; //2024年8月14日20时的数据
    sleep_test[idx].total_start_ts = total_start_ts;
    sleep_test[idx].total_end_ts = total_end_ts;
    //printf("0:total_start_ts = %d, total_end_ts = %d\n", total_start_ts, total_end_ts);

    ts = total_start_ts;
    es = ts;
    for(u8 i = 0; i < 20; i++){
        sleep_test[idx].ctx[i].stage = slp_state[i];
        sleep_test[idx].ctx[i].start_ts = ts;
        sleep_test[idx].ctx[i].end_ts = es + slp_dur[i];
        //printf("%s:sec = %d, ts = %d, es = %d\n, ", __func__, i, sleep_test[0].ctx[i].start_ts, sleep_test[0].ctx[i].end_ts);
        ts += slp_dur[i];
        es = ts;
    }

    /* 第四天 */
    idx++;
    s_time.year = 2024; s_time.month = 8; s_time.day = 15; 
    s_time.hour = 23; s_time.min = 30; s_time.sec = 0;
    total_start_ts = UtcTimeToSec(&s_time);

    e_time.year = 2024; e_time.month = 8; e_time.day = 16; 
    e_time.hour = 7; e_time.min = 30; e_time.sec = 0;
    total_end_ts = UtcTimeToSec(&e_time);
    sleep_test[idx].check_code = Nor_Vm_Check_Code;
    sleep_test[idx].SecNum = 20;
    sleep_test[idx].timestamp = 1723752000; //2024年8月15日20时的数据
    sleep_test[idx].total_start_ts = total_start_ts;
    sleep_test[idx].total_end_ts = total_end_ts;
    //printf("0:total_start_ts = %d, total_end_ts = %d\n", total_start_ts, total_end_ts);

    ts = total_start_ts;
    es = ts;
    for(u8 i = 0; i < 20; i++){
        sleep_test[idx].ctx[i].stage = slp_state[i];
        sleep_test[idx].ctx[i].start_ts = ts;
        sleep_test[idx].ctx[i].end_ts = es + slp_dur[i];
        //printf("%s:sec = %d, ts = %d, es = %d\n, ", __func__, i, sleep_test[0].ctx[i].start_ts, sleep_test[0].ctx[i].end_ts);
        ts += slp_dur[i];
        es = ts;
    }

    /* 第五天 */
    idx++;
    s_time.year = 2024; s_time.month = 8; s_time.day = 16; 
    s_time.hour = 23; s_time.min = 30; s_time.sec = 0;
    total_start_ts = UtcTimeToSec(&s_time);

    e_time.year = 2024; e_time.month = 8; e_time.day = 17; 
    e_time.hour = 7; e_time.min = 30; e_time.sec = 0;
    total_end_ts = UtcTimeToSec(&e_time);
    sleep_test[idx].check_code = Nor_Vm_Check_Code;
    sleep_test[idx].SecNum = 20;
    sleep_test[idx].timestamp = 1723838400; //2024年8月16日20时的数据
    sleep_test[idx].total_start_ts = total_start_ts;
    sleep_test[idx].total_end_ts = total_end_ts;
    //printf("0:total_start_ts = %d, total_end_ts = %d\n", total_start_ts, total_end_ts);

    ts = total_start_ts;
    es = ts;
    for(u8 i = 0; i < 20; i++){
        sleep_test[idx].ctx[i].stage = slp_state[i];
        sleep_test[idx].ctx[i].start_ts = ts;
        sleep_test[idx].ctx[i].end_ts = es + slp_dur[i];
        //printf("%s:sec = %d, ts = %d, es = %d\n, ", __func__, i, sleep_test[0].ctx[i].start_ts, sleep_test[0].ctx[i].end_ts);
        ts += slp_dur[i];
        es = ts;
    }

    /* 第六天 */
    idx++;
    s_time.year = 2024; s_time.month = 8; s_time.day = 17; 
    s_time.hour = 23; s_time.min = 30; s_time.sec = 0;
    total_start_ts = UtcTimeToSec(&s_time);

    e_time.year = 2024; e_time.month = 8; e_time.day = 18; 
    e_time.hour = 7; e_time.min = 30; e_time.sec = 0;
    total_end_ts = UtcTimeToSec(&e_time);
    sleep_test[idx].check_code = Nor_Vm_Check_Code;
    sleep_test[idx].SecNum = 20;
    sleep_test[idx].timestamp = 1723924800; //2024年8月17日20时的数据
    sleep_test[idx].total_start_ts = total_start_ts;
    sleep_test[idx].total_end_ts = total_end_ts;
    //printf("0:total_start_ts = %d, total_end_ts = %d\n", total_start_ts, total_end_ts);

    ts = total_start_ts;
    es = ts;
    for(u8 i = 0; i < 20; i++){
        sleep_test[idx].ctx[i].stage = slp_state[i];
        sleep_test[idx].ctx[i].start_ts = ts;
        sleep_test[idx].ctx[i].end_ts = es + slp_dur[i];
        //printf("%s:sec = %d, ts = %d, es = %d\n, ", __func__, i, sleep_test[0].ctx[i].start_ts, sleep_test[0].ctx[i].end_ts);
        ts += slp_dur[i];
        es = ts;
    }

    /* 第七天 */
    idx++;
    s_time.year = 2024; s_time.month = 8; s_time.day = 18; 
    s_time.hour = 23; s_time.min = 30; s_time.sec = 0;
    total_start_ts = UtcTimeToSec(&s_time);

    e_time.year = 2024; e_time.month = 8; e_time.day = 19; 
    e_time.hour = 7; e_time.min = 30; e_time.sec = 0;
    total_end_ts = UtcTimeToSec(&e_time);
    sleep_test[idx].check_code = Nor_Vm_Check_Code;
    sleep_test[idx].SecNum = 20;
    sleep_test[idx].timestamp = 1724011200; //2024年8月18日20时的数据
    sleep_test[idx].total_start_ts = total_start_ts;
    sleep_test[idx].total_end_ts = total_end_ts;
    //printf("0:total_start_ts = %d, total_end_ts = %d\n", total_start_ts, total_end_ts);

    ts = total_start_ts;
    es = ts;
    for(u8 i = 0; i < 20; i++){
        sleep_test[idx].ctx[i].stage = slp_state[i];
        sleep_test[idx].ctx[i].start_ts = ts;
        sleep_test[idx].ctx[i].end_ts = es + slp_dur[i];
        //printf("%s:sec = %d, ts = %d, es = %d\n, ", __func__, i, sleep_test[0].ctx[i].start_ts, sleep_test[0].ctx[i].end_ts);
        ts += slp_dur[i];
        es = ts;
    }

    for(u8 i = 0; i < 7; i++)
    {
        VmSleepCtxFlashSave(&sleep_test[i]);
    }

    return;
}
#endif