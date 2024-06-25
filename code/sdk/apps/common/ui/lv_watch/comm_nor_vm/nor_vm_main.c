#include "nor_vm_main.h"

static void *nor_vm_file[\
    Nor_Vm_Type_Max] = {NULL};

static const nor_vm_para_t nor_vm_para[\
    Nor_Vm_Type_Max] = 
{
    {nor_vm_type_contacts, Contacts_Max_Num, \
        Nor_Vm_Contacts_Size, Nor_Vm_Contacts_Offset},
    {nor_vm_type_weather, Weather_Sync_Days, \
        Nor_Vm_Weather_Size, Nor_Vm_Weather_Offset},
    {nor_vm_type_call_log, Call_log_Max_Num, \
        Nor_Vm_Call_log_Size, Nor_Vm_Call_log_Offset},
    {nor_vm_type_message, Msg_Max_Num, \
        Nor_Vm_Message_Size, Nor_Vm_Message_Offset},
    {nor_vm_type_sleep, Sleep_Max_Days, \
        Nor_Vm_Sleep_Size, Nor_Vm_Sleep_Offset},
    {nor_vm_type_hr, Hr_Max_Days, \
        Nor_Vm_Hr_Size, Nor_Vm_Hr_Offset},
    {nor_vm_type_bo, Bo_Max_Days, \
        Nor_Vm_Bo_Size, Nor_Vm_Bo_Offset},
    {nor_vm_type_activity, Activity_Max_Days, \
        Nor_Vm_Activity_Size, Nor_Vm_Activity_Offset},
};

void nor_flash_vm_init(void)
{
    const nor_vm_para_t *p;
    for (p = nor_vm_para; p->type < \
        Nor_Vm_Type_Max; p++) 
    {
        ASSERT(!(p->vm_size & 4095));
        
        ASSERT(!(p->vm_offset & 4095));

        flash_common_init(&nor_vm_file[p->type], "ui_vm", \
            p->type, p->vm_num_max, p->vm_offset, p->vm_offset + p->vm_size);
    }

    SetSleepInfoPara();
    SetWeatherInfoPara();

    PowerOnVmDataRead();

    return;
}

void nor_flash_vm_clear(void)
{
    const nor_vm_para_t *p;
    for (p = nor_vm_para; p->type < \
        Nor_Vm_Type_Max; p++) 
    {
        ASSERT(!(p->vm_size & 4095));
        
        ASSERT(!(p->vm_offset & 4095));

        flash_common_reset(&nor_vm_file[p->type], "ui_vm", \
            p->type, p->vm_num_max, p->vm_offset, p->vm_offset + p->vm_size);
    }

#if 0
    VmHrCtxClear();
    VmBoCtxClear();
    VmSleepCtxClear();
    VmWeatherCtxClear();
    VmMessageCtxClear();
    VmCallLogCtxClear();
    VmContactsCtxClear();
    VmActivityCtxClear();
#endif

    return;
}

void *nor_flash_vm_file(nor_vm_type_t nor_vm_type)
{
    if(nor_vm_type >= Nor_Vm_Type_Max)
        return NULL;
    
    return nor_vm_file[nor_vm_type];
}

uint8_t nor_flash_vm_num_max(nor_vm_type_t nor_vm_type)
{
    if(nor_vm_type >= \
        Nor_Vm_Type_Max)
        return 0;

    const nor_vm_para_t *p;
    for (p = nor_vm_para; p->type < \
        Nor_Vm_Type_Max; p++) 
    {
        if(p->type == nor_vm_type)
            return p->vm_num_max;
    }

    return 0;
}

