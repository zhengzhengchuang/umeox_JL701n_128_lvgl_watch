#ifndef __PEDO_DATA_H__
#define __PEDO_DATA_H__

#ifdef __cplusplus
extern "C" {
#endif

#define Steps_Max (99999)
#define Calorie_Max (9999)
#define Distance_Max (99999)

typedef struct
{
    u16 vm_mask;

    u32 steps;    //步数
    u32 calorie; //卡路里:千卡
    u32 distance;//距离:米
}PedoData_t;
extern PedoData_t PedoData;

void PedoDataClear(void);
void VmPedoCtxFlashWrite(void);
void PedoDataMinProcess(struct sys_time *ptime);
void PedoDataAlgoProcess(struct sys_time *ptime);

void PowerOnSetPedoVmCache(void);
void PowerOffSetPedoVmFlashSave(void);

void PedoDataVmRead(void);
void PedoDataVmWrite(void);
void PedoDataVmReset(void);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
