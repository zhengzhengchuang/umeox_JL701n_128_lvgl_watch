#include "../lv_watch.h"

#define Task_Len (2)

void nor_vm_task_post(int type, int *para, int len)
{
    int task_post[Task_Len] = {0};

    task_post[0] = type;

    if(para != NULL)
    {
        if(len > Task_Len - 1)
            len = Task_Len - 1;

        memcpy(&task_post[1], para, len*sizeof(int));
    }

    post_nor_vm_task_msg(task_post, Task_Len);

    return;
}

static void nor_vm_task_handle(void *p)
{
    int rev_ret;
    int rev_msg[32] = {0};

    while(1)
    {
        rev_ret = \
            os_taskq_pend(NULL, rev_msg, ARRAY_SIZE(rev_msg)); 

        if(rev_ret == OS_TASKQ)
            nor_vm_task_msg_handle(rev_msg, ARRAY_SIZE(rev_msg));
    }
    
    return;
}

void nor_vm_task_msg_handle(int *rev_msg, u8 len)
{
    if(!rev_msg || len == 0)
        return;

    int msg_cmd = rev_msg[0];

    switch(msg_cmd)
    {
        case nor_vm_message:
            MsgNotifyProcess();
            break;

        case nor_vm_call_log:
            VmCallLogCtxFlashWrite();
            break;

        case nor_vm_weather:
            WeatherInfoParaUpdate();
            break;

        case nor_vm_contacts:
            UpdateContactsVmFlash();
            break;

        default:
            printf("nor vm task not found\n");
            break;
    }

    return;
}

void nor_vm_task_create(void)
{
    int err = task_create(nor_vm_task_handle, \
        NULL, Nor_Vm_Task_Name);
    if(err) 
        r_printf("nor vm task create err!!!!!!!:%d \n", err);

    return;
}

int post_nor_vm_task_msg(int *post_msg, u8 len)
{
    int err = 0;
    int count = 0;

    if(!post_msg || len == 0)
        return -1;

__retry:
    err = os_taskq_post_type(Nor_Vm_Task_Name, \
        post_msg[0], len - 1, &post_msg[1]);

    if(cpu_in_irq() || cpu_irq_disabled())
        return err;

    if(err) 
    {
        if(!strcmp(os_current_task(), Nor_Vm_Task_Name)) 
            return err;

        if(count > 20)
            return -1;
        
        count++;
        os_time_dly(1);
        goto __retry;
    }

    return err;
}
