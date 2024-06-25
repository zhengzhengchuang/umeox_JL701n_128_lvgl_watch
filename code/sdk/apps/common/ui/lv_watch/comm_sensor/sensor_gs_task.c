#include "../lv_watch.h"

static u8 GsFifoData[Gs_Fifo_Size];

static void SensorGsTask(void *p)
{
    int rev_ret;
    int rev_msg[8] = {0};

    EnableSensorGsModule();

    while(1)
    {
        rev_ret = \
            os_taskq_pend(NULL, rev_msg, \
                ARRAY_SIZE(rev_msg)); 

        if(rev_ret == OS_TASKQ)
            SensorGsTaskMsgHandle(rev_msg, \
                ARRAY_SIZE(rev_msg));
    }
    
    return;
}

void SensorGsTaskCreate(void)
{
    int err = task_create(SensorGsTask, \
        NULL, Sensor_Gs_Task_Name);
    if(err) 
        r_printf("Gs task create err!!!!!!!:%d \n", err);

    return;
}

void SensorGsTaskMsgHandle(int *rev_msg, u8 len)
{
    if(!rev_msg || len == 0)
        return;

    int msg_cmd = rev_msg[0];

    switch(msg_cmd)
    {
        case SensorGsMsgProcess:
        {
            SensorGsIntProcess();
        }
            break;

        case SensorGsMsgEnableModule:
        {
            qmi8658_enableSensors(\
                QMI8658_ACC_ENABLE);
        }
            break;

        case SensorGsMsgDisableModule:
        {
            qmi8658_enableSensors(\
                QMI8658_DISABLE_ALL);
        }
            break;

        default:
            printf("*************ui msg not found\n");
            break;
    }

    return;
}

int PostSensorGsTaskMsg(int *post_msg, u8 len)
{
    int err = 0;
    int count = 0;

    if(!post_msg || len == 0)
        return -1;

__retry:
    err = os_taskq_post_type(\
        Sensor_Gs_Task_Name, post_msg[0], \
            len - 1, &post_msg[1]);

    if(cpu_in_irq() || cpu_irq_disabled())
        return err;

    if(err) 
    {
        if(!strcmp(os_current_task(), \
            Sensor_Gs_Task_Name)) 
            return err;

        if(count > 20)
            return -1;
        
        count++;
        os_time_dly(1);
        goto __retry;
    }

    return err;
}

void EnableSensorGsModule(void)
{
    int SensorGsMsg[1];
	SensorGsMsg[0] = SensorGsMsgEnableModule;
	PostSensorGsTaskMsg(SensorGsMsg, 1); 

    return;
} 

void DisableSensorGsModule(void)
{
    int SensorGsMsg[1];
	SensorGsMsg[0] = SensorGsMsgDisableModule;
	PostSensorGsTaskMsg(SensorGsMsg, 1); 

    return;
} 

void SensorGsIntProcess(void)
{
	memset(GsFifoData, 0, \
		Gs_Fifo_Size);
	u8 fifo_level = \
		 qmi8658_read_fifo(GsFifoData);
	if(fifo_level > Qmi8658_Fifo_WM)
		fifo_level = Qmi8658_Fifo_WM;
  
    /*运动心率与Gs数据的配套使用*/
    HrGsDataFifoWrite(GsFifoData, \
        fifo_level*Gs_Fifo_Num*6);

    /*地磁与Gs数据的配套使用*/
    SensorGmGsDataFifoWrite(GsFifoData, \
        fifo_level*Gs_Fifo_Num*6);

    return;
}
