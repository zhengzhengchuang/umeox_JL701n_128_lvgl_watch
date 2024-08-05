#include "../lv_watch.h"

#define Task_Len (2)

void le_task_post(int type, int *para, int len)
{
    int task_post[Task_Len] = {0};

    task_post[0] = type;

    if(para != NULL)
    {
        if(len > Task_Len - 1)
            len = Task_Len - 1;

        memcpy(&task_post[1], para, len*sizeof(int));
    }

    post_le_task_msg(task_post, Task_Len);

    return;
}

static void le_task_handle(void *p)
{
    int rev_ret;
    int rev_msg[64] = {0};

    while(1)
    {
        rev_ret = \
            os_taskq_pend(NULL, rev_msg, ARRAY_SIZE(rev_msg)); 

        if(rev_ret == OS_TASKQ)
            le_task_msg_handle(rev_msg, ARRAY_SIZE(rev_msg));
    }
    
    return;
}

void le_task_msg_handle(int *msg, u8 len)
{
    if(!msg || len == 0)
        return;

    int ui_msg[2] = {0};

    int cmd = msg[0];

    switch(cmd)
    {
        case Le_Set_UtcTime:
            common_clock_pointer_angle_update();
            break;

        case Le_Set_Shutdown:
            DevOpShutdownHandle();
            break;

        case Le_Set_Restart:
            DevOpRestartHandle();
            break;

        case Le_Set_Reset:
            DevOpResetHandle();
            break;

        case Le_Req_Exit_Camera:
            DevReqOpCameraHandle(DevReqExitCamera);
            break;

        case Le_Dnd_Update:
            DndManualUpdate();
            break;

        case Le_Find_Dev:
            int enable = msg[1];
            if(enable)
                FindDevEnableHandle();
            else
                FindDevDisableHandle();
            break;

        case Le_User_Info_Update:
            UserInfoParaUpdate();
            break;

        case Le_GalgoKey_Update:
            //GalgoInfoParaUpdate();      
            break;

        case Le_First_Dev_Bond:
            ui_msg[0] = ui_msg_first_bond_handle;
            post_ui_msg(ui_msg, 1);
            break;

        case Le_Ori_Dev_Bond:
            ui_msg[0] = ui_msg_ori_bond_handle;
            post_ui_msg(ui_msg, 1);
            break;

        case Le_New_Dev_Bond:
            ui_msg[0] = ui_msg_new_bond_handle;
            post_ui_msg(ui_msg, 1);
            break;

        case Le_Dev_UnBond:
            ui_msg[0] = ui_msg_unbond_handle;
            post_ui_msg(ui_msg, 1);
            break;

        case Le_Notify_Push:
            ui_msg[0] = ui_msg_nor_message_write;
            post_ui_msg(ui_msg, 1);
            break;

        case Le_ANCSSw_Update:
            MsgNotifyInfoParaUpdate();
            break;

        case Le_LL_Info_Update:
            LLInfoParaUpdate();
            KaabaQiblaParaUpdate();
            PTimeInfoParaUpdate();
            break;

        case Le_Weather_Update:
            ui_msg[0] = ui_msg_nor_weather_write;
            post_ui_msg(ui_msg, 1);
            break;

        case Le_Sed_Info_Update:
            SedInfoParaUpdate();
            break;

        case Le_Remote_Enter_Camera:
            RemoteReqEnterCameraHandle();
            break;

        case Le_Remote_Exit_Camera:
            RemoteReqExitCameraHandle();
            break;

        case Le_PTime_Cfg_Para_Update:
            PTimeCfgParaUpdate();
            PTimeInfoParaUpdate();
            break;

        case Le_Call_Contacts_Clear:
            VmContactsCtxClear();
            break;

        case Le_Call_Contacts_Update:
            ui_msg[0] = ui_msg_nor_contacts_write;
            post_ui_msg(ui_msg, 1);
            break;

        case Le_History_Hr_Data:
            HistoryHrDataSend(msg[1], msg[2], msg[3], msg[4]);
            break;

        case Le_History_Bo_Data:
            HistoryBoDataSend(msg[1], msg[2], msg[3], msg[4]);
            break;

        case Le_History_Pedo_Data:
            HistoryPedoDataSend(msg[1], msg[2], msg[3], msg[4]);
            break;

        case Le_History_Sleep_Data:
            HistorySleepDataSend(msg[1], msg[2], msg[3], msg[4]);
            break;

        default:
            printf("le task not found\n");
            break;
    }

    return;
}

void le_task_create(void)
{
    int err = task_create(le_task_handle, NULL, Le_Task_Name);
    if(err) printf("le task create err!!!!!!!:%d \n", err);

    return;
}

int post_le_task_msg(int *msg, u8 len)
{
    int err = 0;
    int count = 0;

    if(!msg || len == 0)
        return -1;

__retry:
    err = os_taskq_post_type(Le_Task_Name, \
        msg[0], len - 1, &msg[1]);

    if(cpu_in_irq() || cpu_irq_disabled())
        return err;

    if(err) 
    {
        if(!strcmp(os_current_task(), Le_Task_Name)) 
            return err;

        if(count > 20)
            return -1;
        
        count++;
        os_time_dly(1);
        goto __retry;
    }

    return err;
}
