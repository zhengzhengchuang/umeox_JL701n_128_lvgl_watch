#include "../lv_watch.h"

static u8 GsFifoData[Gs_Fifo_Size];

static void GsEnableHandle(void)
{
    qmi8658_enableSensors(QMI8658_ACC_ENABLE);

    return;
}

static void GsDisableHandle(void)
{
    qmi8658_enableSensors(QMI8658_DISABLE_ALL);
    
    return;
}

static void GsIntProcess(void)
{
	memset(GsFifoData, 0, Gs_Fifo_Size);
	u8 fifo = qmi8658_read_fifo(GsFifoData);
	if(fifo > Qmi8658_Fifo_WM)
		fifo = Qmi8658_Fifo_WM;
    
    u32 fifo_len = fifo*Gs_Fifo_Num*6;

    /*运动心率与Gs数据的配套使用*/
    HrGsDataFifoWrite(GsFifoData, fifo_len);

    /*地磁与Gs数据的配套使用*/
    GmGsDataFifoWrite(GsFifoData, fifo_len);

    /* gomore与Gs数据配套使用 */
    GoGsDataFifoWrite(GsFifoData, fifo_len);

    return;
}

static void GsTask(void *p)
{
    int ret;
    int msg[8] = {0};

    EnableGsModule();

    while(1)
    {
        ret = os_taskq_pend(NULL, msg, ARRAY_SIZE(msg)); 

        if(ret == OS_TASKQ)
            GsTaskMsgHandle(msg, ARRAY_SIZE(msg));
    }
    
    return;
}

void GsTaskCreate(void)
{
    int err = task_create(GsTask, NULL, Gs_Task_Name);
    if(err) printf("Gs task create err!!!!!!!:%d \n", err);

    return;
}

void GsTaskMsgHandle(int *msg, u8 len)
{
    if(!msg || len == 0)
        return;

    int msg_cmd = msg[0];

    switch(msg_cmd)
    {
        case GsMsgProcess:
            GsIntProcess();
            break;

        case GsMsgEnable:
            GsEnableHandle();
            break;

        case GsMsgDisable:
            GsDisableHandle();
            break;

        default:
            break;
    }

    return;
}

int PostGsTaskMsg(int *msg, u8 len)
{
    int err = 0;
    int count = 0;

    if(!msg || len == 0)
        return -1;

__retry:
    err = os_taskq_post_type(Gs_Task_Name, msg[0], \
        len - 1, &msg[1]);

    if(cpu_in_irq() || cpu_irq_disabled())
        return err;

    if(err) 
    {
        if(!strcmp(os_current_task(), Gs_Task_Name)) 
            return err;

        if(count > 20)
            return -1;
        
        count++;
        os_time_dly(1);
        goto __retry;
    }

    return err;
}

void EnableGsModule(void)
{
    int GsMsg[1];
	GsMsg[0] = GsMsgEnable;
	PostGsTaskMsg(GsMsg, 1); 

    return;
} 

void DisableGsModule(void)
{
    int GsMsg[1];
	GsMsg[0] = GsMsgDisable;
	PostGsTaskMsg(GsMsg, 1); 

    return;
} 
