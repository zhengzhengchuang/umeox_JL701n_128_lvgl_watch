#include "../lv_watch.h"

void HistoryHrDataSend(int cmd, int year, int month, int day)
{
    u8 para_pkt = 1;
    u8 data_pkt = 24;
    u8 total_pkt = para_pkt + data_pkt;

    struct sys_time time;
    memset(&time, 0, sizeof(struct sys_time));
    time.year = year;
    time.month = month;
    time.day = day;
    u32 timestamp_0 = UtcTimeToSec(&time);
    
    u8 nfy_buf[Cmd_Pkt_Len];

    u8 idx;
    u32 timestamp_1;
    u8 num = VmHrItemNum();
    for(u8 i = 0; i < num; i++)
    {
        bool ret = VmHrCtxByIdx(i);
        if(ret == false) continue;

        memset(&time, 0, sizeof(struct sys_time));
        time.year = r_hr.time.year;
        time.month = r_hr.time.month;
        time.day = r_hr.time.day;
        timestamp_1 = UtcTimeToSec(&time);
        if(timestamp_1 >= timestamp_0)
        {
            para_pkt = 1;
            data_pkt = 24;
            total_pkt = para_pkt + data_pkt;

            for(u8 j = 0; j < total_pkt; j++)
            {
                if(j == 0)
                {
                    //参数包
                    memset(nfy_buf, 0x00, Cmd_Pkt_Len);

                    idx = 0;
                    nfy_buf[idx++] = cmd;
                    nfy_buf[idx++] = 0x00;
                    nfy_buf[idx++] = data_pkt;
                    nfy_buf[idx++] = r_hr.time.year - 2000;
                    nfy_buf[idx++] = r_hr.time.month;
                    nfy_buf[idx++] = r_hr.time.day;

                    u8 crc_idx = Cmd_Pkt_Len - 1;
                    nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

                    printf_buf(nfy_buf, Cmd_Pkt_Len);
                    
                    umeox_common_le_notify_data(nfy_buf, Cmd_Pkt_Len);
                }else
                {
                    //数据包
                    memset(nfy_buf, 0x00, Cmd_Pkt_Len);

                    idx = 0;
                    nfy_buf[idx++] = cmd;
                    nfy_buf[idx++] = j;
                    nfy_buf[idx++] = Hr_Inv_Dur;
                    nfy_buf[idx++] = r_hr.data[(j-1)*4 + 0];
                    nfy_buf[idx++] = r_hr.data[(j-1)*4 + 1];
                    nfy_buf[idx++] = r_hr.data[(j-1)*4 + 2];
                    nfy_buf[idx++] = r_hr.data[(j-1)*4 + 3];

                    u8 crc_idx = Cmd_Pkt_Len - 1;
                    nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

                    printf_buf(nfy_buf, Cmd_Pkt_Len);
                    
                    umeox_common_le_notify_data(nfy_buf, Cmd_Pkt_Len);
                }
            }
        }
    }

    para_pkt = 1;
    if(w_hr.CurIdx%4 == 0)
        data_pkt = w_hr.CurIdx/4;
    else
        data_pkt = w_hr.CurIdx/4 + 1;
    total_pkt = para_pkt + data_pkt;
    for(u8 j = 0; j < total_pkt; j++)
    {
        if(j == 0)
        {
            //参数包
            memset(nfy_buf, 0x00, Cmd_Pkt_Len);

            idx = 0;
            nfy_buf[idx++] = cmd;
            nfy_buf[idx++] = 0x00;
            nfy_buf[idx++] = data_pkt;
            nfy_buf[idx++] = w_hr.time.year - 2000;
            nfy_buf[idx++] = w_hr.time.month;
            nfy_buf[idx++] = w_hr.time.day;

            u8 crc_idx = Cmd_Pkt_Len - 1;
            nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

            printf_buf(nfy_buf, Cmd_Pkt_Len);
            
            umeox_common_le_notify_data(nfy_buf, Cmd_Pkt_Len);
        }else
        {
            //数据包
            memset(nfy_buf, 0, Cmd_Pkt_Len);

            idx = 0;
            nfy_buf[idx++] = cmd;
            nfy_buf[idx++] = j;
            nfy_buf[idx++] = Hr_Inv_Dur;
            nfy_buf[idx++] = w_hr.data[(j-1)*4 + 0];
            nfy_buf[idx++] = w_hr.data[(j-1)*4 + 1];
            nfy_buf[idx++] = w_hr.data[(j-1)*4 + 2];
            nfy_buf[idx++] = w_hr.data[(j-1)*4 + 3];

            u8 crc_idx = Cmd_Pkt_Len - 1;
            nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

            printf_buf(nfy_buf, Cmd_Pkt_Len);
            
            umeox_common_le_notify_data(nfy_buf, Cmd_Pkt_Len);
        }
    }

    return;
}


void HistoryBoDataSend(int cmd, int year, int month, int day)
{
    u8 para_pkt = 1;
    u8 data_pkt = 24;
    u8 total_pkt = para_pkt + data_pkt;

    struct sys_time time;
    memset(&time, 0, sizeof(struct sys_time));
    time.year = year;
    time.month = month;
    time.day = day;
    u32 timestamp_0 = UtcTimeToSec(&time);
    
    u8 nfy_buf[Cmd_Pkt_Len];

    u8 idx;
    u32 timestamp_1;
    u8 num = VmBoItemNum();
    for(u8 i = 0; i < num; i++)
    {
        bool ret = VmBoCtxByIdx(i);
        if(ret == false) continue;

        memset(&time, 0, sizeof(struct sys_time));
        time.year = r_bo.time.year;
        time.month = r_bo.time.month;
        time.day = r_bo.time.day;
        timestamp_1 = UtcTimeToSec(&time);
        if(timestamp_1 >= timestamp_0)
        {
            para_pkt = 1;
            data_pkt = 24;
            total_pkt = para_pkt + data_pkt;

            for(u8 j = 0; j < total_pkt; j++)
            {
                if(j == 0)
                {
                    //参数包
                    memset(nfy_buf, 0x00, Cmd_Pkt_Len);

                    idx = 0;
                    nfy_buf[idx++] = cmd;
                    nfy_buf[idx++] = 0x00;
                    nfy_buf[idx++] = data_pkt;
                    nfy_buf[idx++] = r_bo.time.year - 2000;
                    nfy_buf[idx++] = r_bo.time.month;
                    nfy_buf[idx++] = r_bo.time.day;

                    u8 crc_idx = Cmd_Pkt_Len - 1;
                    nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

                    printf_buf(nfy_buf, Cmd_Pkt_Len);
                    
                    umeox_common_le_notify_data(nfy_buf, Cmd_Pkt_Len);
                }else
                {
                    //数据包
                    memset(nfy_buf, 0x00, Cmd_Pkt_Len);

                    idx = 0;
                    nfy_buf[idx++] = cmd;
                    nfy_buf[idx++] = j;
                    nfy_buf[idx++] = Bo_Inv_Dur;
                    nfy_buf[idx++] = r_bo.data[(j-1)*4 + 0];
                    nfy_buf[idx++] = r_bo.data[(j-1)*4 + 1];
                    nfy_buf[idx++] = r_bo.data[(j-1)*4 + 2];
                    nfy_buf[idx++] = r_bo.data[(j-1)*4 + 3];

                    u8 crc_idx = Cmd_Pkt_Len - 1;
                    nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

                    printf_buf(nfy_buf, Cmd_Pkt_Len);
                    
                    umeox_common_le_notify_data(nfy_buf, Cmd_Pkt_Len);
                }
            }
        }
    }

    para_pkt = 1;
    if(w_bo.CurIdx%4 == 0)
        data_pkt = w_bo.CurIdx/4;
    else
        data_pkt = w_bo.CurIdx/4 + 1;
    total_pkt = para_pkt + data_pkt;
    for(u8 j = 0; j < total_pkt; j++)
    {
        if(j == 0)
        {
            //参数包
            memset(nfy_buf, 0x00, Cmd_Pkt_Len);

            idx = 0;
            nfy_buf[idx++] = cmd;
            nfy_buf[idx++] = 0x00;
            nfy_buf[idx++] = data_pkt;
            nfy_buf[idx++] = w_bo.time.year - 2000;
            nfy_buf[idx++] = w_bo.time.month;
            nfy_buf[idx++] = w_bo.time.day;

            u8 crc_idx = Cmd_Pkt_Len - 1;
            nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

            printf_buf(nfy_buf, Cmd_Pkt_Len);
            
            umeox_common_le_notify_data(nfy_buf, Cmd_Pkt_Len);
        }else
        {
            //数据包
            memset(nfy_buf, 0, Cmd_Pkt_Len);

            idx = 0;
            nfy_buf[idx++] = cmd;
            nfy_buf[idx++] = j;
            nfy_buf[idx++] = Bo_Inv_Dur;
            nfy_buf[idx++] = w_bo.data[(j-1)*4 + 0];
            nfy_buf[idx++] = w_bo.data[(j-1)*4 + 1];
            nfy_buf[idx++] = w_bo.data[(j-1)*4 + 2];
            nfy_buf[idx++] = w_bo.data[(j-1)*4 + 3];

            u8 crc_idx = Cmd_Pkt_Len - 1;
            nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

            printf_buf(nfy_buf, Cmd_Pkt_Len);
            
            umeox_common_le_notify_data(nfy_buf, Cmd_Pkt_Len);
        }
    }

    return;
}

void HistoryPedoDataSend(int cmd, int year, int month, int day)
{
    u8 para_pkt = 1;
    u8 data_pkt = 24;
    u8 total_pkt = para_pkt + data_pkt;

    struct sys_time time;
    memset(&time, 0, sizeof(struct sys_time));
    time.year = year;
    time.month = month;
    time.day = day;
    u32 timestamp_0 = UtcTimeToSec(&time);
    
    u8 nfy_buf[Cmd_Pkt_Len];

    u8 i;
    u8 idx;
    u32 timestamp_1;
    u8 num = VmPedoItemNum();
    for(i = 0; i < num; i++)
    {
        bool ret = VmPedoCtxByIdx(i);
        if(ret == false) continue;

        memset(&time, 0, sizeof(struct sys_time));
        time.year = r_pedo.time.year;
        time.month = r_pedo.time.month;
        time.day = r_pedo.time.day;
        timestamp_1 = UtcTimeToSec(&time);
        if(timestamp_1 >= timestamp_0)
        {
            para_pkt = 1;
            data_pkt = 24;
            total_pkt = para_pkt + data_pkt;

            for(u8 j = 0; j < total_pkt; j++)
            {
                if(j == 0)
                {
                    //参数包
                    memset(nfy_buf, 0, Cmd_Pkt_Len);

                    idx = 0;
                    nfy_buf[idx++] = cmd;
                    nfy_buf[idx++] = 0x00;
                    nfy_buf[idx++] = data_pkt;
                    nfy_buf[idx++] = r_pedo.time.year - 2000;
                    nfy_buf[idx++] = r_pedo.time.month;
                    nfy_buf[idx++] = r_pedo.time.day;

                    u8 crc_idx = Cmd_Pkt_Len - 1;
                    nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

                    printf_buf(nfy_buf, Cmd_Pkt_Len);
                    
                    umeox_common_le_notify_data(nfy_buf, Cmd_Pkt_Len);
                }else
                {
                    //数据包
                    memset(nfy_buf, 0, Cmd_Pkt_Len);

                    idx = 0;
                    nfy_buf[idx++] = cmd;
                    nfy_buf[idx++] = j;
                    nfy_buf[idx++] = Pedo_Inv_Dur;
                    nfy_buf[idx++] = (r_pedo.steps[j - 1]>>16)&(0xff);
                    nfy_buf[idx++] = (r_pedo.steps[j - 1]>>8)&(0xff);
                    nfy_buf[idx++] = (r_pedo.steps[j - 1]>>0)&(0xff);

                    nfy_buf[idx++] = (r_pedo.calorie[j - 1]>>8)&(0xff);
                    nfy_buf[idx++] = (r_pedo.calorie[j - 1]>>0)&(0xff);

                    nfy_buf[idx++] = (r_pedo.distance[j - 1]>>16)&(0xff);
                    nfy_buf[idx++] = (r_pedo.distance[j - 1]>>8)&(0xff);
                    nfy_buf[idx++] = (r_pedo.distance[j - 1]>>0)&(0xff);

                    u8 crc_idx = Cmd_Pkt_Len - 1;
                    nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

                    printf_buf(nfy_buf, Cmd_Pkt_Len);
                    
                    umeox_common_le_notify_data(nfy_buf, Cmd_Pkt_Len);
                }
            }
        }
    }

    para_pkt = 1;
    data_pkt = w_pedo.CurIdx;
    total_pkt = para_pkt + data_pkt;
    for(u8 j = 0; j < total_pkt; j++)
    {
        if(j == 0)
        {
            //参数包
            memset(nfy_buf, 0, Cmd_Pkt_Len);

            idx = 0;
            nfy_buf[idx++] = cmd;
            nfy_buf[idx++] = 0x00;
            nfy_buf[idx++] = data_pkt;
            nfy_buf[idx++] = w_pedo.time.year - 2000;
            nfy_buf[idx++] = w_pedo.time.month;
            nfy_buf[idx++] = w_pedo.time.day;

            u8 crc_idx = Cmd_Pkt_Len - 1;
            nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

            printf_buf(nfy_buf, Cmd_Pkt_Len);
            
            umeox_common_le_notify_data(nfy_buf, Cmd_Pkt_Len);
        }else
        {
            //数据包
            memset(nfy_buf, 0, Cmd_Pkt_Len);

            idx = 0;
            nfy_buf[idx++] = cmd;
            nfy_buf[idx++] = j;
            nfy_buf[idx++] = Pedo_Inv_Dur;
            nfy_buf[idx++] = (w_pedo.steps[j-1]>>16)&(0xff);
            nfy_buf[idx++] = (w_pedo.steps[j-1]>>8)&(0xff);
            nfy_buf[idx++] = (w_pedo.steps[j-1]>>0)&(0xff);

            nfy_buf[idx++] = (w_pedo.calorie[j-1]>>8)&(0xff);
            nfy_buf[idx++] = (w_pedo.calorie[j-1]>>0)&(0xff);

            nfy_buf[idx++] = (w_pedo.distance[j-1]>>16)&(0xff);
            nfy_buf[idx++] = (w_pedo.distance[j-1]>>8)&(0xff);
            nfy_buf[idx++] = (w_pedo.distance[j-1]>>0)&(0xff);

            u8 crc_idx = Cmd_Pkt_Len - 1;
            nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

            printf_buf(nfy_buf, Cmd_Pkt_Len);
            
            umeox_common_le_notify_data(nfy_buf, Cmd_Pkt_Len);
        }
    }

    return;
}
