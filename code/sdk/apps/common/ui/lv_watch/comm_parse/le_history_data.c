#include "../lv_watch.h"

void HistoryHrDataSend(int cmd, int year, int month, int day)
{
    u8 para_pkt = 1;
    u8 data_pkt = 24;
    u8 total_pkt = para_pkt + data_pkt;

    struct sys_time time;
    u8 nfy_buf[Cmd_Pkt_Len];

    time.year = year; time.month = month; time.day = day;
    time.hour = 0; time.min = 0; time.sec = 0;
    u32 timestamp_0 = UtcTimeToSec(&time);
    
    u8 idx;
    u32 timestamp_1;
    u8 num = VmHrItemNum();
    for(u8 i = 0; i < num; i++)
    {
        bool ret = VmHrCtxByIdx(i);
        if(ret == false) continue;

        timestamp_1 = r_hr.timestamp;
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

                    SecToUtcTime(r_hr.timestamp, &time);

                    idx = 0;
                    nfy_buf[idx++] = cmd;
                    nfy_buf[idx++] = 0x00;
                    nfy_buf[idx++] = data_pkt;
                    nfy_buf[idx++] = time.year - 2000;
                    nfy_buf[idx++] = time.month;
                    nfy_buf[idx++] = time.day;

                    u8 crc_idx = Cmd_Pkt_Len - 1;
                    nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

                    //printf_buf(nfy_buf, Cmd_Pkt_Len);
                    
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

                    //printf_buf(nfy_buf, Cmd_Pkt_Len);
                    
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

            SecToUtcTime(w_hr.timestamp, &time);

            idx = 0;
            nfy_buf[idx++] = cmd;
            nfy_buf[idx++] = 0x00;
            nfy_buf[idx++] = data_pkt;
            nfy_buf[idx++] = time.year - 2000;
            nfy_buf[idx++] = time.month;
            nfy_buf[idx++] = time.day;

            u8 crc_idx = Cmd_Pkt_Len - 1;
            nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

            //printf_buf(nfy_buf, Cmd_Pkt_Len);
            
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

            //printf_buf(nfy_buf, Cmd_Pkt_Len);
            
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
    u8 nfy_buf[Cmd_Pkt_Len];

    time.year = year; time.month = month; time.day = day;
    time.hour = 0; time.min = 0; time.sec = 0;
    u32 timestamp_0 = UtcTimeToSec(&time);
    
    u8 idx;
    u32 timestamp_1;
    u8 num = VmBoItemNum();
    for(u8 i = 0; i < num; i++)
    {
        bool ret = VmBoCtxByIdx(i);
        if(ret == false) continue;

        timestamp_1 = r_bo.timestamp;
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

                    SecToUtcTime(r_bo.timestamp, &time);

                    idx = 0;
                    nfy_buf[idx++] = cmd;
                    nfy_buf[idx++] = 0x00;
                    nfy_buf[idx++] = data_pkt;
                    nfy_buf[idx++] = time.year - 2000;
                    nfy_buf[idx++] = time.month;
                    nfy_buf[idx++] = time.day;

                    u8 crc_idx = Cmd_Pkt_Len - 1;
                    nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

                    //printf_buf(nfy_buf, Cmd_Pkt_Len);
                    
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

                    //printf_buf(nfy_buf, Cmd_Pkt_Len);
                    
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

            SecToUtcTime(w_bo.timestamp, &time);

            idx = 0;
            nfy_buf[idx++] = cmd;
            nfy_buf[idx++] = 0x00;
            nfy_buf[idx++] = data_pkt;
            nfy_buf[idx++] = time.year - 2000;
            nfy_buf[idx++] = time.month;
            nfy_buf[idx++] = time.day;

            u8 crc_idx = Cmd_Pkt_Len - 1;
            nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

            //printf_buf(nfy_buf, Cmd_Pkt_Len);
            
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

            //printf_buf(nfy_buf, Cmd_Pkt_Len);
            
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
    u8 nfy_buf[Cmd_Pkt_Len];

    time.year = year; time.month = month; time.day = day;
    time.hour = 0; time.min = 0; time.sec = 0;
    u32 timestamp_0 = UtcTimeToSec(&time);
    
    u8 i;
    u8 idx;
    u32 timestamp_1;
    u8 num = VmPedoItemNum();
    for(i = 0; i < num; i++)
    {
        bool ret = VmPedoCtxByIdx(i);
        if(ret == false) continue;

        timestamp_1 = r_pedo.timestamp;
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

                    SecToUtcTime(r_pedo.timestamp, &time);

                    idx = 0;
                    nfy_buf[idx++] = cmd;
                    nfy_buf[idx++] = 0x00;
                    nfy_buf[idx++] = data_pkt;
                    nfy_buf[idx++] = time.year - 2000;
                    nfy_buf[idx++] = time.month;
                    nfy_buf[idx++] = time.day;

                    u8 crc_idx = Cmd_Pkt_Len - 1;
                    nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

                    //printf_buf(nfy_buf, Cmd_Pkt_Len);
                    
                    umeox_common_le_notify_data(nfy_buf, Cmd_Pkt_Len);
                }else
                {
                    //数据包
                    memset(nfy_buf, 0, Cmd_Pkt_Len);

                    u32 steps = (u32)r_pedo.steps[j-1];
                    u32 calorie = (u32)r_pedo.calorie[j-1];
                    u32 distance = (u32)r_pedo.distance[j-1];

                    idx = 0;
                    nfy_buf[idx++] = cmd;
                    nfy_buf[idx++] = j;
                    nfy_buf[idx++] = Pedo_Inv_Dur;
                    nfy_buf[idx++] = (steps>>16)&(0xff);
                    nfy_buf[idx++] = (steps>>8)&(0xff);
                    nfy_buf[idx++] = (steps>>0)&(0xff);

                    nfy_buf[idx++] = (calorie>>8)&(0xff);
                    nfy_buf[idx++] = (calorie>>0)&(0xff);

                    nfy_buf[idx++] = (distance>>16)&(0xff);
                    nfy_buf[idx++] = (distance>>8)&(0xff);
                    nfy_buf[idx++] = (distance>>0)&(0xff);

                    u8 crc_idx = Cmd_Pkt_Len - 1;
                    nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

                    //printf_buf(nfy_buf, Cmd_Pkt_Len);
                    
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

            SecToUtcTime(w_pedo.timestamp, &time);

            idx = 0;
            nfy_buf[idx++] = cmd;
            nfy_buf[idx++] = 0x00;
            nfy_buf[idx++] = data_pkt;
            nfy_buf[idx++] = time.year - 2000;
            nfy_buf[idx++] = time.month;
            nfy_buf[idx++] = time.day;

            u8 crc_idx = Cmd_Pkt_Len - 1;
            nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

            //printf_buf(nfy_buf, Cmd_Pkt_Len);
            
            umeox_common_le_notify_data(nfy_buf, Cmd_Pkt_Len);
        }else
        {
            //数据包
            memset(nfy_buf, 0, Cmd_Pkt_Len);

            u32 steps = (u32)w_pedo.steps[j-1];
            u32 calorie = (u32)w_pedo.calorie[j-1];
            u32 distance = (u32)w_pedo.distance[j-1];

            idx = 0;
            nfy_buf[idx++] = cmd;
            nfy_buf[idx++] = j;
            nfy_buf[idx++] = Pedo_Inv_Dur;
            nfy_buf[idx++] = (steps>>16)&(0xff);
            nfy_buf[idx++] = (steps>>8)&(0xff);
            nfy_buf[idx++] = (steps>>0)&(0xff);

            nfy_buf[idx++] = (calorie>>8)&(0xff);
            nfy_buf[idx++] = (calorie>>0)&(0xff);

            nfy_buf[idx++] = (distance>>16)&(0xff);
            nfy_buf[idx++] = (distance>>8)&(0xff);
            nfy_buf[idx++] = (distance>>0)&(0xff);

            u8 crc_idx = Cmd_Pkt_Len - 1;
            nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

            //printf_buf(nfy_buf, Cmd_Pkt_Len);
            
            umeox_common_le_notify_data(nfy_buf, Cmd_Pkt_Len);
        }
    }

    return;
}

static u8 slp_sync_cnt;
static u32 slp_timestamp[Slp_Max_Days];
void HistorySleepDataSend(int cmd, int year, int month, int day)
{
    /* 跟上面发送数据不同，睡眠数据按天获取 */
    u8 para_pkt;
    u8 data_pkt;
    u8 total_pkt;

    struct sys_time time;
    u8 nfy_buf[Cmd_Pkt_Len];

    /* 睡眠以20点作为起始点 */
    time.year = year; time.month = month; time.day = day;
    time.hour = SlpStartH, time.min = 0, time.sec = 0;
    u32 timestamp_0 = UtcTimeToSec(&time);

    u8 idx;
    u8 num = VmPedoItemNum();
    if(num == 0)
    {
        //没有历史数据
        goto __No_Data;
    }else
    {
        u8 i;
        bool ret;

        //开始检索日期
        if(slp_sync_cnt == 0)
        {
            /* 先把日期先缓存下来 */
            for(i = 0; i < num; i++)
            {
                ret = VmPedoCtxByIdx(i);
                if(ret == false) continue;

                slp_timestamp[i] = r_sleep.timestamp;
            }
        }
    
        for(i = 0; i < num; i++)
        {
            if(timestamp_0 == slp_timestamp[i])
                break;
        }

        slp_sync_cnt++;
        if(slp_sync_cnt >= Slp_Max_Days)
        {
            /* 同步到最大天数，认为结束 */
            slp_sync_cnt = 0;
            memset(slp_timestamp, 0, sizeof(slp_timestamp));
        }

        if(i >= num) goto __No_Data;

        /* 时间比对正确，再次读取具体内容 */
        ret = VmPedoCtxByIdx(i);
        if(ret == false) goto __No_Data;

        para_pkt = 1;
        data_pkt = r_sleep.SecNum;
        total_pkt = para_pkt + data_pkt;

        struct sys_time time1;
        struct sys_time time2;
        for(u8 j = 0; j < total_pkt; j++)
        {
            if(j == 0)
            {
                //参数包
                memset(nfy_buf, 0, Cmd_Pkt_Len);

                SecToUtcTime(r_sleep.total_start_ts, &time1);
                SecToUtcTime(r_sleep.total_end_ts, &time2);

                idx = 0;
                nfy_buf[idx++] = cmd;
                nfy_buf[idx++] = 0x00;
                nfy_buf[idx++] = data_pkt;
                nfy_buf[idx++] = time1.year - 2000;
                nfy_buf[idx++] = time1.month;
                nfy_buf[idx++] = time1.day;
                nfy_buf[idx++] = time1.hour;
                nfy_buf[idx++] = time1.min;
                nfy_buf[idx++] = time2.year - 2000;
                nfy_buf[idx++] = time2.month;
                nfy_buf[idx++] = time2.day;
                nfy_buf[idx++] = time2.hour;
                nfy_buf[idx++] = time2.min;

                u8 crc_idx = Cmd_Pkt_Len - 1;
                nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

                //printf_buf(nfy_buf, Cmd_Pkt_Len);
                
                umeox_common_le_notify_data(nfy_buf, Cmd_Pkt_Len);
            }else
            {
                //数据包
                memset(nfy_buf, 0, Cmd_Pkt_Len);

                SecToUtcTime(r_sleep.ctx[j-1].start_ts, &time1);
                SecToUtcTime(r_sleep.ctx[j-1].end_ts, &time2);

                idx = 0;
                nfy_buf[idx++] = cmd;
                nfy_buf[idx++] = j;
                nfy_buf[idx++] = r_sleep.ctx[j-1].stage;
                nfy_buf[idx++] = time1.year - 2000;
                nfy_buf[idx++] = time1.month;
                nfy_buf[idx++] = time1.day;
                nfy_buf[idx++] = time1.hour;
                nfy_buf[idx++] = time1.min;
                nfy_buf[idx++] = time2.year - 2000;
                nfy_buf[idx++] = time2.month;
                nfy_buf[idx++] = time2.day;
                nfy_buf[idx++] = time2.hour;
                nfy_buf[idx++] = time2.min;

                u8 crc_idx = Cmd_Pkt_Len - 1;
                nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);

                //printf_buf(nfy_buf, Cmd_Pkt_Len);
                
                umeox_common_le_notify_data(nfy_buf, Cmd_Pkt_Len);
            }
        }
    }

    goto __end;

__No_Data:
    memset(nfy_buf, 0, Cmd_Pkt_Len);
    idx = 0;
    nfy_buf[idx++] = cmd;
    nfy_buf[idx++] = 0xff;
    u8 crc_idx = Cmd_Pkt_Len - 1;
    nfy_buf[crc_idx] = calc_crc(nfy_buf, crc_idx);  
    umeox_common_le_notify_data(nfy_buf, Cmd_Pkt_Len);

__end:
    return;
}
