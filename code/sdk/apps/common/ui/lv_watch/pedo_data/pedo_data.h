#ifndef __PEDO_DATA_H__
#define __PEDO_DATA_H__

#ifdef __cplusplus
extern "C" {
#endif

#define Steps_Max (99999.0f)
#define Calorie_Max (9999.0f)
#define Distance_Max (99999.0f)

typedef struct
{
    u16 vm_mask;

    float steps;    //步数
    float calorie;  //卡路里:千卡
    float distance; //距离:米
}PedoData_t;
extern PedoData_t PedoData;

void PedoDataClear(void);
void VmPedoCtxFlashWrite(void);
void PedoDataMinProcess(struct sys_time *ptime);

float KmToMile(float km);
float DistanceCalc(float steps);
void PedoDataHandle(float steps, float calorie, float distance);
void SetPedoDataVmCtxCache(float steps, float calorie, float distance);

/* 开机、时间更新时调用 */
void WPedoParaInit(void);

void PowerOnSetPedoVmCache(void);
void PowerOffSetPedoVmFlashSave(void);

void PedoDataVmRead(void);
void PedoDataVmWrite(void);
void PedoDataVmReset(void);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
