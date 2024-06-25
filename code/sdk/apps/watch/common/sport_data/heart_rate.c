// #include "event.h"
// #include "system/includes.h"
// #include "app_config.h"
// #include "sport_data/heart_rate.h"
// #include "sport_data/watch_common.h"
// #include "sport_data/watch_time_common.h"
// #include "math.h"
// #include "dev_manager.h"
// #include "stdlib.h"
// #include "fs/fs.h"
// #include "sys_time.h"
// #include "time.h"
// #include "ui_vm/ui_vm.h"
// #include "asm/crc16.h"
// #include "message_vm_cfg.h"
// #include "debug.h"

// #define HR_TIME_INTERVAL 5//心率时间5-10
// #define heartrate_file_enable 1//文件写读开关
// #define HR_PATH  "storage/sd1/C/sport/heart_rate/HR"

// //********************************************
// //              心率数据参数
// //********************************************
// struct heart_rate {
//     int systimerid;
//     u8 work_status;
//     u8 real_rate_value;
//     u8 Tcount;
//     u8 real_rate_buf[60 / HR_TIME_INTERVAL + 1];//小时数据，用于文件写读
// } heart_rate_data = {
//     .work_status = 0,
// };
// #if heartrate_file_enable
// struct hr_file_data {
//     u16 file_id;
//     u16 w_file_offset;
//     u16 w_data_head_offset;
//     u16 data_len;
//     u8 file_status;
//     struct sys_time rfiletime;
//     FILE *fp;
//     u8 *databuf;
// } hrfiledata;
// enum {
//     HR_FILE_NULL,
//     HR_FILE_NORMAL,
//     HR_FILE_BREAK
// };
// //***************************************************
// //   将心率数据写入文件
// //***************************************************
// static int heart_rate_file_write(void)
// {
//     printf("%s %d", __func__, __LINE__);
//     struct sys_time ntime;
//     watch_file_get_sys_time(&ntime);
// #if TCFG_NOR_VM
//     if (hrfiledata.file_status == HR_FILE_NULL) { //当日数据未记录
//         u8 fbuf[11];//写文件头
//         fbuf[0] = 0x03;
//         fbuf[1] = (ntime.year >> 8) & 0xff;
//         fbuf[2] = (ntime.year & 0xff);
//         fbuf[3] = ntime.month;
//         fbuf[4] = ntime.day;
//         fbuf[5] = 0xff; //CRC
//         fbuf[6] = 0xff; //CRC
//         fbuf[7] = 0x00; //版本号
//         fbuf[8] = HR_TIME_INTERVAL; //存储时间间隔
//         fbuf[9] = 0xff; //保留位1
//         fbuf[10] = 0xff; //保留位2
//         /*printf("filetype=%d,year=%d,month=%d,day=%d,interval=%d",fbuf[0],ntime.year,ntime.month,ntime.day,HR_TIME_INTERVAL);*/
//         if (flash_common_get_total(get_flash_vm_hd(F_TYPE_HEART)) == get_flash_vm_number_max(F_TYPE_HEART)) {
//             printf("total==%d", flash_common_get_total(get_flash_vm_hd(F_TYPE_HEART)));
//             flash_common_delete_by_index(get_flash_vm_hd(F_TYPE_HEART), 0);
//         }//清理旧文件
//         hrfiledata.file_id = flash_common_open_id(get_flash_vm_hd(F_TYPE_HEART), 0, 512); // 创建新文件
//         if (hrfiledata.file_id == 0) {
//             log_e("hr_file_id_error\n");
//             return 0;
//         }
//         printf("file_head id=%d", hrfiledata.file_id);
//         flash_common_write_packet(get_flash_vm_hd(F_TYPE_HEART), hrfiledata.file_id, 11, fbuf); //写入文件头
//         hrfiledata.w_file_offset = 11;
//         hrfiledata.w_data_head_offset = hrfiledata.w_file_offset;
//         u8 data_head[4];//数据段首信息
//         data_head[0] = ntime.hour;
//         data_head[1] = ntime.min;
//         data_head[2] = 0xff; //len_h
//         data_head[3] = 0xff; //len_l
//         printf("hour=%d,min=%d", data_head[0], data_head[1]);
//         flash_common_write_packet(get_flash_vm_hd(F_TYPE_HEART), hrfiledata.file_id, 4, data_head);
//         hrfiledata.w_file_offset += 4;
//         printf("file_data0_head");
//     } else if (hrfiledata.file_status == HR_FILE_BREAK) { //数据被打断
//         u8 *tmp_buf = zalloc(hrfiledata.w_file_offset + 1);
//         //读取旧文件数据
//         flash_common_read_by_id(get_flash_vm_hd(F_TYPE_HEART), hrfiledata.file_id, 0, hrfiledata.w_file_offset, tmp_buf);
//         //删除旧文件
//         flash_common_delete_by_id(get_flash_vm_hd(F_TYPE_HEART), hrfiledata.file_id);
//         //创建新文件
//         flash_common_open_id(get_flash_vm_hd(F_TYPE_HEART), hrfiledata.file_id, 512); //创建文件
//         flash_common_write_packet(get_flash_vm_hd(F_TYPE_HEART), hrfiledata.file_id, hrfiledata.w_file_offset, tmp_buf);
//         free(tmp_buf);
//         printf("%s file_packet_ok", __func__);
//         u8 data_head[4];
//         data_head[0] = ntime.hour;
//         data_head[1] = ntime.min;
//         data_head[2] = 0xff; //len_h
//         data_head[3] = 0xff; //len_l
//         flash_common_write_packet(get_flash_vm_hd(F_TYPE_HEART), hrfiledata.file_id, 4, data_head);
//         hrfiledata.w_data_head_offset = hrfiledata.w_file_offset;
//         hrfiledata.w_file_offset += 4;

//     }
//     //写入一小时数据
//     printf("hrfile  seek=%d len=%d ", hrfiledata.w_file_offset, (60 / HR_TIME_INTERVAL));
//     flash_common_write_packet(get_flash_vm_hd(F_TYPE_HEART), hrfiledata.file_id,	(60 / HR_TIME_INTERVAL), heart_rate_data.real_rate_buf);
//     hrfiledata.w_file_offset += (60 / HR_TIME_INTERVAL);
// #else
//     if (hrfiledata.file_status == HR_FILE_NULL) { //当日数据未记录
//         u8 fbuf[11];//写文件头
//         fbuf[0] = 0x03;
//         fbuf[1] = (ntime.year >> 8) & 0xff;
//         fbuf[2] = (ntime.year & 0xff);
//         fbuf[3] = ntime.month;
//         fbuf[4] = ntime.day;
//         fbuf[5] = 0xff; //CRC
//         fbuf[6] = 0xff; //CRC
//         fbuf[7] = 0x00; //版本号
//         fbuf[8] = HR_TIME_INTERVAL; //存储时间间隔
//         fbuf[9] = 0xff; //保留位1
//         fbuf[10] = 0xff; //保留位2
//         /*printf("filetype=%d,year=%d,month=%d,day=%d,interval=%d",fbuf[0],ntime.year,ntime.month,ntime.day,HR_TIME_INTERVAL);*/
//         char path[45];
//         sprintf(path, "%s%02d%02d%02d", HR_PATH, (ntime.year) % 100, ntime.month, ntime.day);
//         printf("%s", path);
//         hrfiledata.fp = fopen(path, "w+");
//         int file_len = flen(hrfiledata.fp);
//         if (hrfiledata.fp == NULL) {
//             printf("open_file_error!!!");
//             free(fp);
//             return 0;
//         }
//         fwrite(hrfiledata.fp, fbuf, 11);
//         hrfiledata.w_file_offset = 11;
//         hrfiledata.w_data_head_offset = hrfiledata.w_file_offset;
//         u8 data_head[4];//数据段首信息
//         data_head[0] = ntime.hour;
//         data_head[1] = ntime.min;
//         data_head[2] = 0xff; //len_h
//         data_head[3] = 0xff; //len_l
//         fwrite(hrfiledata.fp, data_head, 4);
//         hrfiledata.w_file_offset += 4;
//         printf("file_data0_head");
//     } else if (hrfiledata.file_status == HR_FILE_BREAK) { //数据被打断

//         u8 data_head[4];
//         data_head[0] = ntime.hour;
//         data_head[1] = ntime.min;
//         data_head[2] = 0xff; //len_h
//         data_head[3] = 0xff; //len_l
//         fseek(hrfiledata.fp, file_len, 0);
//         fwrite(hrfiledata.fp, data_head, 4);
//         hrfiledata.w_file_offset += 4;
//     }
//     //写入一小时数据
//     fwrite(hrfiledata.fp, heart_rate_data.real_rate_buf, 60 / HR_TIME_INTERVAL);
//     hrfiledata.w_file_offset += 60 / HR_TIME_INTERVAL;
// #endif
//     //文件标记为正常
//     hrfiledata.file_status = HR_FILE_NORMAL;
//     return 1;
// }

// static int heart_rate_file_write_break(void)//心率文件被打断
// {
//     printf("%s%d", __func__, __LINE__);
// #if TCFG_NOR_VM
//     printf("file_id=%d", hrfiledata.file_id);
//     //存入当前数据
//     if (heart_rate_data.Tcount >= 1) {
//         flash_common_write_packet(get_flash_vm_hd(F_TYPE_HEART), hrfiledata.file_id,	heart_rate_data.Tcount, heart_rate_data.real_rate_buf);
//     }
//     printf("%s%d", __func__, __LINE__);
//     flash_common_write_push(get_flash_vm_hd(F_TYPE_HEART), hrfiledata.file_id);
//     printf("%s%d", __func__, __LINE__);
//     //写入数据段长度
//     hrfiledata.w_file_offset += heart_rate_data.Tcount;
//     heart_rate_data.Tcount = 0;
//     u16 head_data_len = hrfiledata.w_file_offset - hrfiledata.w_data_head_offset - 4;
//     u8 len_buf[2];
//     len_buf[0] = (head_data_len >> 8) & 0xff;
//     len_buf[1] = head_data_len & 0xff;
//     flash_common_update_by_id(get_flash_vm_hd(F_TYPE_HEART), hrfiledata.file_id, hrfiledata.w_data_head_offset + 2, 2, len_buf);
// #else
//     if (heart_rate_data.Tcount >= 1) {
//         fwrite(hrfiledata.fp, heart_rate_data.real_rate_buf, heart_rate_data.Tcount);
//         hrfiledata.w_file_offset += heart_rate_data.Tcount;
//         heart_rate_data.Tcount = 0;
//     }
//     u16 head_data_len = hrfiledata.w_file_offset - hrfiledata.w_data_head_offset - 4;
//     fseek(hrfiledata.fp, hrfiledata.w_data_head_offset + 2, 0);
//     fwrite(hrfiledata.fp, &head_data_len, 2);
// #endif
//     //标记文件状态为打断
//     hrfiledata.file_status = HR_FILE_BREAK;
//     return 0;
// }

// static int heart_rate_file_write_stop(void) //跨天更换文件
// {
//     printf("%s", __func__);
// #if TCFG_NOR_VM
//     if (heart_rate_data.Tcount >= 1) { //写入最后数据
//         flash_common_write_packet(get_flash_vm_hd(F_TYPE_HEART), hrfiledata.file_id, heart_rate_data.Tcount, heart_rate_data.real_rate_buf);
//         hrfiledata.w_file_offset += heart_rate_data.Tcount;
//         printf("%s,tcount=%d,offset=%d", __func__, heart_rate_data.Tcount, hrfiledata.w_file_offset);
//         heart_rate_data.Tcount = 0;
//     }
//     flash_common_write_push(get_flash_vm_hd(F_TYPE_HEART), hrfiledata.file_id);
//     //写入最后一段数的len
//     u16 head_data_len = hrfiledata.w_file_offset - hrfiledata.w_data_head_offset - 4;
//     u8 len_buf[2];
//     len_buf[0] = (head_data_len >> 8) & 0xff;
//     len_buf[1] = head_data_len & 0xff;
//     flash_common_update_by_id(get_flash_vm_hd(F_TYPE_HEART), hrfiledata.file_id, hrfiledata.w_data_head_offset + 2, 2, len_buf);
//     //写入CRC
//     u16 temp_data_len = hrfiledata.w_file_offset - 11;
//     u8 *temp_data = zalloc(temp_data_len + 1);
//     flash_common_read_by_id(get_flash_vm_hd(F_TYPE_HEART), hrfiledata.file_id, 11, temp_data_len, temp_data);
//     u16 crc_value = CRC16(temp_data, temp_data_len);
//     u8 crc_buf[2];
//     crc_buf[0] = crc_value >> 8 & 0xff;
//     crc_buf[1] = crc_value & 0xff;
//     flash_common_update_by_id(get_flash_vm_hd(F_TYPE_HEART), hrfiledata.file_id, 5, 2, crc_buf);

// #else
//     if (heart_rate_data.Tcount >= 1) { //写入最后数据
//         fwrite(hrfiledata.fp, heart_rate_data.real_rate_buf, heart_rate_data.Tcount);
//         hrfiledata.w_file_offset += heart_rate_data.Tcount;
//         heart_rate_data.Tcount = 0;
//     }
//     u16 head_data_len = hrfiledata.w_file_offset - hrfiledata.w_data_head_offset - 4;
//     fseek(hrfiledata.fp, hrfiledata.w_data_head_offset + 2, 0);
//     fwrite(hrfiledata.fp, &head_data_len, 2);
//     //写入CRC
//     u16 temp_data_len = hrfiledata.w_file_offset - 11;
//     u8 *temp_data = zalloc(temp_data_len + 1);
//     fseek(hrfiledata.fp, 11);
//     fread(hrfiledata.fp, temp_data, temp_data_len);
//     u16 crc_value = CRC16(temp_data, temp_data_len);
//     u8 crc_buf[2];
//     crc_buf[0] = crc_value >> 8 & 0xff;
//     crc_buf[1] = crc_value & 0xff;
//     fseek(hrfiledata.fp, 5, 0);
//     fwrite(hrfiledata.fp, crc_buf, 2);
//     fclose(hrfiledata.fp);
//     hrfiledata.fp = NULL;
// #endif
//     //切换文件标记
//     free(temp_data);

//     temp_data = NULL;
//     hrfiledata.file_status = HR_FILE_NULL;
//     hrfiledata.w_data_head_offset = 0;
//     hrfiledata.w_file_offset = 0;
//     hrfiledata.file_id = 0;
//     return 0;
// }
// //***************************************************
// //   读取心率文件内容
// //   参数：日期
// //****************************************************
// static int heart_rate_file_time_get(struct sys_time *ntime)//找到最新文件的日期
// {

//     printf("%s__%d", __func__, __LINE__);

//     u16 id_table[7 * 2];
//     int table_len = 7 * 4;
//     flash_common_get_id_table(get_flash_vm_hd(F_TYPE_HEART), table_len, (u8 *)id_table);
//     printf("table_len=%d", table_len);
//     for (int i = 0; i < 7; i++) {
//         printf("table,id=%d,size=%d", id_table[i * 2], id_table[i * 2 + 1]);
//         if ((id_table[i * 2] == 0) & (i >= 1)) {
//             hrfiledata.file_id = id_table[(i - 1) * 2]; //找到最新的id
//             hrfiledata.w_file_offset = id_table[(i - 1) * 2 + 1]; //size
//             break;
//         }
//     }
//     if (hrfiledata.file_id == 0) {
//         printf("no_file");
//         return 0;
//     }
//     printf("fileid==%d,size=%d", hrfiledata.file_id, hrfiledata.w_file_offset);
//     u8 tbuf[4];
//     flash_common_read_by_id(get_flash_vm_hd(F_TYPE_HEART), hrfiledata.file_id, 1, 4, tbuf);
//     hrfiledata.rfiletime.year = (tbuf[1] | (tbuf[0] << 8));
//     hrfiledata.rfiletime.month = tbuf[2];
//     hrfiledata.rfiletime.day = tbuf[3];
//     struct sys_time tmp_time;
//     if (ntime == NULL) {
//         ntime = &tmp_time;
//         watch_file_get_sys_time(ntime);
//     }
//     printf("yyyy%d=%d,mm%d=%d,dd%d=%d", hrfiledata.rfiletime.year, ntime->year, hrfiledata.rfiletime.month, ntime->month, hrfiledata.rfiletime.day, ntime->day);
//     if ((hrfiledata.rfiletime.year == ntime->year) & (hrfiledata.rfiletime.month == ntime->month) & (hrfiledata.rfiletime.day == ntime->day)) {
//         return 1;
//     } else {
//         return 0;
//     }
// }
// static int heart_rate_file_read(struct sys_time *p)
// {
//     printf("%s,%d", __func__, __LINE__);
// #if TCFG_NOR_VM
//     int file_id = 0;
//     if (p != NULL) {
//         file_id = watch_file_time_to_id(F_TYPE_HEART, p);
//         if (file_id == 0) {
//             log_e("file_id_not find\n");
//             return 0;
//         }
//     } else {
//         file_id = 0xffff; //缺省默认当天
//     }
//     if (file_id == 0xffff) {
//         if (hrfiledata.file_id == 0) {
//             log_e("file_id==NULL;");
//             return 0;
//         } else {
//             file_id = hrfiledata.file_id;
//         }
//     }
//     printf("id=%d ", file_id);
//     hrfiledata.data_len = 0;
//     int pmin = 0;
//     int nmin = 0;
//     int offset = 0;
//     int read_len = 11;
//     u8 fbuf[11];
//     u8 data_head[4];
//     if (hrfiledata.databuf == NULL) {
//         hrfiledata.databuf = zalloc(24 * 60 / HR_TIME_INTERVAL + 1);
//     } else {
//         memset(hrfiledata.databuf, 0, 24 * 60 / HR_TIME_INTERVAL);
//     }
//     printf("runhrer");
//     int file_size = flash_common_get_file_size_by_id(get_flash_vm_hd(F_TYPE_HEART), file_id);
//     printf("filesize=%d", file_size);
//     flash_common_read_by_id(get_flash_vm_hd(F_TYPE_HEART), file_id, offset, read_len, fbuf);
//     offset += read_len;
//     printf("type0x03=%d,year=%d,month=%d,day=%d,crc=%d,version=%d,time_interval=%d", \
//            fbuf[0], (fbuf[1] << 8 | fbuf[2]), fbuf[3], fbuf[4], (fbuf[5] << 8 | fbuf[6]), fbuf[7], fbuf[8]);

//     while (1) {

//         read_len = 4;
//         flash_common_read_by_id(get_flash_vm_hd(F_TYPE_HEART), file_id, offset, read_len, data_head);
//         offset += read_len;
//         printf("hour=%d,min=%d", data_head[0], data_head[1]);
//         nmin = data_head[0] * 60 + data_head[1];
//         if ((nmin - pmin) > HR_TIME_INTERVAL) {
//             memset(&hrfiledata.databuf[hrfiledata.data_len], 1, (u16)((nmin - pmin) / HR_TIME_INTERVAL));
//             hrfiledata.data_len += (u16)((nmin - pmin) / HR_TIME_INTERVAL);
//         }
//         pmin = nmin;
//         read_len = data_head[2] << 8 | data_head[3];
//         flash_common_read_by_id(get_flash_vm_hd(F_TYPE_HEART), file_id, offset, read_len, &hrfiledata.databuf[hrfiledata.data_len]);
//         hrfiledata.data_len += read_len;
//         offset += read_len;
//         printf_buf(hrfiledata.databuf, hrfiledata.data_len);
//         printf("offset%d filesize%d", offset, file_size);
//         if (offset >= file_size) {
//             break;
//         }
//     }
//     return 0;
// #else
//     struct sys_time ptime;
//     int mode = 0;
//     if (p == NULL) {
//         mode = 1;
//         ptime = zalloc(sizeof(struct sys_time));
//         watch_file_get_sys_time(ptime);//传入空指针默认当天
//         if (ptime == NULL) {
//             printf("get_sys_time_error!!!");
//             return 0;
//         }
//     }
//     memcpy(&ptime, p, sizeof(struct sys_time));
//     char path[45];
//     sprintf(path, "%s%02d%02d%02d", HR_PATH, (ptime.year) % 100, ptime.month, ptime.day);
//     FILE *fp = fopen(path, "w+");
//     if (fp == NULL) {
//         printf("open_file_error!!!");
//         return 0;
//     }
//     int file_len = flen(fp);
//     u8 fbuf[11];
//     fread(fp, fbuf, 11);
//     //定位到第一个数据,开始读取数据
//     fseek(fp, 11, 0);
//     hrfiledata.databuf = zalloc(file_len - 11 + 1);
//     if (hrfiledata.databuf == NULL) {
//         log_e("hrfiledata.databuf zalloc error!\n");
//         return 0;
//     }
//     fread(fp, hrfiledata.databuf, file_len - 11);

//     fclose(fp);
//     return 1;
// #endif
// }
// #endif
// //***************************************************
// //   获取心率数据
// //***************************************************
// static void heart_rate_refresh_data_handle(void)
// {
//     printf("%s %d", __func__, __LINE__);
//     if (heart_rate_data.work_status == 0) {
//         return ;
//     }
// #if (!TCFG_HR_SENSOR_ENABLE)
//     heart_rate_data.real_rate_value = rand32() % 40 + 70;//模拟数据
// #else
//     struct watch_heart_rate __heartrate_hd;
//     watch_heart_rate_handle_get(&__heartrate_hd);
//     heart_rate_data.real_rate_value = __heartrate_hd.heart_rate_data_get(LAST_DATA, NULL);
// #endif

// #if heartrate_file_enable
//     heart_rate_data.real_rate_buf[heart_rate_data.Tcount] = heart_rate_data.real_rate_value;
//     heart_rate_data.Tcount++;
//     struct sys_time ntime;
//     watch_file_get_sys_time(&ntime);
//     if ((ntime.hour == 23) & (ntime.min > 55)) {
//         heart_rate_file_write_stop();
//         return;
//     }
//     if (heart_rate_data.Tcount == (60 / HR_TIME_INTERVAL)) {
//         heart_rate_file_write();
//         heart_rate_data.Tcount = 0;
//     }
// #endif
// }
// static void heart_rate_refresh_data(void *priv)
// {
// #if (!TCFG_HR_SENSOR_ENABLE)
//     heart_rate_refresh_data_handle();
// #else
//     read_heart_rate_task();//打开传感器的20-30秒后的数据较为稳定真实
//     sys_timeout_add(NULL, heart_rate_refresh_data_handle, 20 * 1000);
// #endif

// }

// //********************************************
// //           心率数据处理开关
// //********************************************
// int heart_rate_start(void)
// {
//     printf("%s", __func__);
//     if (heart_rate_data.work_status == 1) {
//         printf("Repeated call%s", __func__);
//         return 0;
//     }
//     heart_rate_data.systimerid = sys_timer_add(NULL, heart_rate_refresh_data, 60 * 1000 * HR_TIME_INTERVAL); //TIME_INTERVAL分钟进行一次读数
//     //处理文件状态初值
// #if heartrate_file_enable
//     if (heart_rate_file_time_get(NULL) == 0) {
//         hrfiledata.file_status = HR_FILE_NULL;
//     } else {
//         hrfiledata.file_status = HR_FILE_BREAK;
//     }

// #endif
//     heart_rate_data.work_status = 1;
//     printf("%s succ", __func__);
//     heart_rate_refresh_data(NULL);
//     return 1;
// }

// int heart_rate_stop(void)
// {
//     printf("%s", __func__);
//     if (heart_rate_data.work_status == 0) {
//         printf("Repeated call%s", __func__);
//         return 0;
//     }
//     if (heart_rate_data.systimerid != 0) {
//         sys_timer_del(heart_rate_data.systimerid);//注销定时器
//     }
// #if heartrate_file_enable
//     if (hrfiledata.file_status == HR_FILE_NORMAL) {
//         heart_rate_file_write_break();//打断当前文件
//     }
//     if (hrfiledata.databuf != NULL) {
//         free(hrfiledata.databuf);
//         hrfiledata.databuf = NULL;
//     }
// #endif

//     heart_rate_data.work_status = 0;
//     printf("%s succ", __func__);
//     return 0;
// }
// int heart_rate_day_buf_disable(void)
// {

//     return 1;
// }
// int heart_rate_day_buf_enable(void)
// {
//     return 1;
// }

// //********************************************
// //     		对外数据接口
// //********************************************
// int get_day_heart_rate_data(u8 *buf, int buf_len, struct sys_time *time)
// {
//     printf("%s", __func__);
// #if heartrate_file_enable
//     heart_rate_file_read(time);
//     if (buf_len > hrfiledata.data_len) {
//         buf_len = hrfiledata.data_len;
//     }
//     memcpy(buf, hrfiledata.databuf, buf_len);
//     return buf_len;
// #else
//     return 0;

// #endif
// }
// int get_day_heart_rate_data_len(struct sys_time *time)
// {
//     printf("%s", __func__);
// #if heartrate_file_enable
//     heart_rate_file_read(time);
//     return hrfiledata.data_len;
// #else
//     return 0;

// #endif
// }
// void clr_heart_rate(void) //清除缓存数据
// {
//     if (heart_rate_data.work_status == 1) {
//         heart_rate_data.real_rate_buf[0] = 0;
//         heart_rate_data.real_rate_value = 0;
//         heart_rate_data.Tcount = 0;
//     }
// }




