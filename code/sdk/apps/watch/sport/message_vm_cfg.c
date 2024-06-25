#include "typedef.h"
#include "includes.h"
#include "app_config.h"
#include "message_vm_cfg.h"

#if 1
#if TCFG_NOR_VM
#include "ui_vm/ui_vm.h"
#endif

#if 0
//#if TCFG_NOR_VM
struct flash_vm_info {
    u8 type;
    u8  max;
    int offset;
    int len;
};

#define F_TYPE_PHONEBOOK_LEN 	4096
#define F_TYPE_SPORTRECORD_LEN 	40*1024
#define F_TYPE_HEART_LEN		4096
#define F_TYPE_BLOOD_OXYGEN_LEN	4096
#define F_TYPE_SLEEP_LEN		4096
#define F_TYPE_MESSAGE_LEN		4096*2
#define F_TYPE_WEATHER_LEN		4096
#define F_TYPE_CALL_LOG_LEN		4096
#define F_TYPE_STEP_LEN			4096

#define F_TYPE_PHONEBOOK_OFFSET 	0
#define F_TYPE_SPORTRECORD_OFFSET	F_TYPE_PHONEBOOK_OFFSET+F_TYPE_PHONEBOOK_LEN
#define F_TYPE_HEART_OFFSET 		F_TYPE_SPORTRECORD_OFFSET+F_TYPE_SPORTRECORD_LEN
#define F_TYPE_BLOOD_OXYGEN_OFFSET 	F_TYPE_HEART_OFFSET+ F_TYPE_HEART_LEN
#define F_TYPE_SLEEP_OFFSET 		F_TYPE_BLOOD_OXYGEN_OFFSET+F_TYPE_BLOOD_OXYGEN_LEN
#define F_TYPE_MESSAGE_OFFSET 		F_TYPE_SLEEP_OFFSET+F_TYPE_SLEEP_LEN
#define F_TYPE_WEATHER_OFFSET 		F_TYPE_MESSAGE_OFFSET+F_TYPE_MESSAGE_LEN
#define F_TYPE_CALL_LOG_OFFSET 		F_TYPE_WEATHER_OFFSET+F_TYPE_WEATHER_LEN
#define F_TYPE_STEP_OFFSET 			F_TYPE_CALL_LOG_OFFSET+F_TYPE_CALL_LOG_LEN


static const  struct flash_vm_info info[F_TYPE_COUNT + 1] = {
    {F_TYPE_PHONEBOOK,   1,   F_TYPE_PHONEBOOK_OFFSET,		F_TYPE_PHONEBOOK_LEN },
    {F_TYPE_SPORTRECORD, 2,   F_TYPE_SPORTRECORD_OFFSET,	F_TYPE_SPORTRECORD_LEN},
    {F_TYPE_HEART,       7,   F_TYPE_HEART_OFFSET,			F_TYPE_HEART_LEN},
    {F_TYPE_BLOOD_OXYGEN, 7,  F_TYPE_BLOOD_OXYGEN_OFFSET,	F_TYPE_BLOOD_OXYGEN_LEN},
    {F_TYPE_SLEEP,       7,   F_TYPE_SLEEP_OFFSET, 			F_TYPE_SLEEP_LEN},
    {F_TYPE_MESSAGE,     10,  F_TYPE_MESSAGE_OFFSET, 		F_TYPE_MESSAGE_LEN},
    {F_TYPE_WEATHER,     4,   F_TYPE_WEATHER_OFFSET, 		F_TYPE_WEATHER_LEN},
    {F_TYPE_CALL_LOG,	4,	  F_TYPE_CALL_LOG_OFFSET, 		F_TYPE_CALL_LOG_LEN},
    {F_TYPE_STEP,		4,	  F_TYPE_STEP_OFFSET, 			F_TYPE_STEP_LEN},
    {0, 0, 0},
};

static void *file[F_TYPE_COUNT];

#endif

void *get_flash_vm_hd(u8 type)
{
#if TCFG_NOR_VM
    // if (type > F_TYPE_MAX) {
    //     return NULL;
    // }
    // return file[type - F_TYPE_BASE];
    return NULL;
#else
    return NULL;
#endif
}

u8 get_flash_vm_number_max(u8 type)
{
#if TCFG_NOR_VM
    // const struct flash_vm_info *p;
    // if (type > F_TYPE_MAX) {
    //     return 0;
    // }
    // for (p = info; p->type != 0 ; p++) {
    //     if (p->type == type) {
    //         return p->max;
    //     }
    // }
#endif
    return 0;
}

#if 0
void flash_message_cfg_init()
{
#if TCFG_NOR_VM
    const struct flash_vm_info *p;
    for (p = info; p->type != 0 ; p++) {
        printf("init type =%d\n", p->type);
        ASSERT(!(p->offset & 4095));
        ASSERT(!(p->len & 4095));
        flash_common_init(&file[p->type - F_TYPE_BASE], "ui_vm", p->type, p->max, p->offset, p->offset + p->len);
    }

#endif
}


void flash_message_cfg_reset()
{
#if TCFG_NOR_VM
    const struct flash_vm_info *p;
    for (p = info; p->type != 0 ; p++) {
        printf("init type =%d\n", p->type);
        ASSERT(!(p->offset & 4095));
        ASSERT(!(p->len & 4095));
        flash_common_reset(&file[p->type - F_TYPE_BASE], "ui_vm", p->type, p->max, p->offset, p->offset + p->len);
    }
#endif
}



int flash_message_write(u8 *data, u16 len)
{
#if TCFG_NOR_VM
    while (flash_common_get_total(get_flash_vm_hd(F_TYPE_MESSAGE)) == get_flash_vm_number_max(F_TYPE_MESSAGE)) {
        flash_common_delete_by_index(get_flash_vm_hd(F_TYPE_MESSAGE), 0);
    }
    int id = 0;
    ASSERT(len >= 7);
    memcpy(&id, &(data[3]), 4);//l :2 :v:1:value
    if (id == 0) {
        printf("ancs tag == 0\n");
        id = -1;
    }
    return flash_common_write_file(get_flash_vm_hd(F_TYPE_MESSAGE), id, len, data);
#endif
    return 0;
}

int flash_message_read_by_index(u8 index, u8 *data, u16 len)
{
#if TCFG_NOR_VM
    u16 size = flash_common_get_file_size_by_index(get_flash_vm_hd(F_TYPE_MESSAGE), index);
    if (len > size) {
        len = size;
    }
    return flash_common_read_by_index(get_flash_vm_hd(F_TYPE_MESSAGE), index, 0, len, data);
#endif
    return 0;
}

int flash_message_delete_by_mask(u32 mask)
{
#if TCFG_NOR_VM
    if (mask == 0) {
        printf("ancs mask == 0\n");
        mask  = -1;
    }

    return flash_common_delete_by_id(get_flash_vm_hd(F_TYPE_MESSAGE), mask);
#endif
    return 0;
}


int flash_message_delete_by_index(u32 index)
{
#if TCFG_NOR_VM
    return flash_common_delete_by_index(get_flash_vm_hd(F_TYPE_MESSAGE), index);
#endif
    return 0;
}

int flash_message_count()
{
#if TCFG_NOR_VM
    return flash_common_get_total(get_flash_vm_hd(F_TYPE_MESSAGE));
#endif
    return 0;
}


int flash_weather_read(u8 *data, u16 len)
{
#if TCFG_NOR_VM
    return flash_common_read_by_index(get_flash_vm_hd(F_TYPE_WEATHER), 0, 0, len, data);
#endif
    return 0;
}

int flash_weather_write(u8 *data, u16 len)
{
#if TCFG_NOR_VM
    while (flash_common_get_total(get_flash_vm_hd(F_TYPE_WEATHER))) {
        flash_common_delete_by_index(get_flash_vm_hd(F_TYPE_WEATHER), 0);
    }
    return flash_common_write_file(get_flash_vm_hd(F_TYPE_WEATHER), 0, len, data);
#endif
    return 0;
}

static struct phonebook call_log_mess = {
    .name = "unknow",
};


typedef struct phonebook_file {
    void *file;//电话本文件句柄
    struct phonebook *phonebook;
    u16    count;
    u16    show_temp;
} phonebook_file;


static struct phonebook_file *p_handler = NULL;
static struct phonebook_file *c_handler = NULL;

#define __this_p_hd 	(p_handler)
#define __this_c_hd 	(c_handler)
#define sizeof_this_hd     (sizeof(struct phonebook_file))


extern void book_list_save(u8 type, const u8 *name, const u8 *number, const u8 *date);
static u32 book_list_get_count_in_exflash(u8 type);
static void book_list_save_in_exflash(u8 type, const u8 *name, const u8 *number, const u8 *date);
extern void delete_call_file();

static void get_sys_time(struct sys_time *time)
{
    void *fd = dev_open("rtc", NULL);
    if (!fd) {
        memset(time, 0, sizeof(*time));
        return;
    }
    dev_ioctl(fd, IOCTL_GET_SYS_TIME, (u32)time);
    dev_close(fd);
}

void set_call_log_type(u8 type)
{
    call_log_mess.type = type;
}

void set_call_log_name(u8 *name)
{
    if (name) {
        snprintf(call_log_mess.name, sizeof(call_log_mess.name), "%s", name);
    }
    y_printf("p_name:%s", call_log_mess.name);
}

void set_call_log_number(u8 *number)
{
    if (number) {
        snprintf(call_log_mess.number, sizeof(call_log_mess.number), "%s", number);
    }
    y_printf("p_number:%s", call_log_mess.number);
}

void set_call_log_date(u8 *date)
{
    //格式:xxxx/xx/xxTxx:xx:xx
    struct sys_time time;
    get_sys_time(&time);
    snprintf(call_log_mess.date, sizeof(call_log_mess.date), "%04d/%02d/%02dT%02d:%02d:%02d", time.year, time.month, time.day, time.hour, time.min, time.sec);
}

void set_call_log_message(u8 type, const u8 *name, const u8 *number, const u8 *date)
{
    u16 len = 0;
    static u8 busy = 0;
    if (busy) {
        ASSERT(0);
    }
    busy = 1;
    call_log_mess.type = type;
    if (name) {
        snprintf(call_log_mess.name, sizeof(call_log_mess.name), "%s", name);
    }
    if (date) {
        snprintf(call_log_mess.date, sizeof(call_log_mess.date), "%s", date);
    } else {
        snprintf(call_log_mess.date, sizeof(call_log_mess.date), "%s", "0000/00/00T00:00:00");
    }
    if (number) {
        snprintf(call_log_mess.number, sizeof(call_log_mess.number), "%s", number);
    }
    busy = 0;
}

void update_call_log_message()
{
    if (call_log_mess.type != 0) {
        if (strlen(call_log_mess.number)) {
            book_list_save(call_log_mess.type, call_log_mess.name, call_log_mess.number, call_log_mess.date);
        }
        memset(&call_log_mess, 0, CALL_LOGS_LEN);
        strcpy(call_log_mess.name, "unknow");
        call_log_mess.type  = 0;
    }
}






int open_file_handler(u8 type, int show_temp)
{
#if TCFG_NOR_VM
    if (type == 0) {
        if (!__this_p_hd) {
            __this_p_hd = zalloc(sizeof_this_hd);
            __this_p_hd->show_temp = show_temp + !show_temp;
            __this_p_hd->phonebook = malloc(sizeof(struct phonebook) * __this_p_hd->show_temp);
            int len =  flash_common_get_file_size_by_index(get_flash_vm_hd(F_TYPE_PHONEBOOK), 0);
            printf("len = %d\n", len);
            ASSERT(!(len % PHONE_BOOK_LEN));
            __this_p_hd->count = len / PHONE_BOOK_LEN;
            printf("phone book count = %d\n", __this_p_hd->count);
        }
    } else {
        if (!__this_c_hd) {
            __this_c_hd = zalloc(sizeof_this_hd);
            __this_c_hd->show_temp = show_temp + !show_temp;
            __this_c_hd->phonebook = malloc(sizeof(struct phonebook) * __this_c_hd->show_temp);
            __this_c_hd->count = flash_common_get_total(get_flash_vm_hd(F_TYPE_CALL_LOG));
            printf("phone book log  count = %d\n", __this_c_hd->count);
        }
    }

    return 0;
#else
    struct vfs_attr attr = {0};

    if (type == 0) {
        if (!__this_p_hd) {
            __this_p_hd = zalloc(sizeof_this_hd);
            __this_p_hd->show_temp = show_temp + !show_temp;
            __this_p_hd->phonebook = malloc(sizeof(struct phonebook) * __this_p_hd->show_temp);
        }

        if (__this_p_hd->file) {
            fclose(__this_p_hd->file);
            __this_p_hd->file = 0;
        }

        __this_p_hd->file = fopen(PHONE_BOOK_PATH, "r");
        if (!__this_p_hd->file) {
            printf("opne fail \n");
            if (__this_p_hd) {
                free(__this_p_hd->phonebook);
                free(__this_p_hd);
                __this_p_hd = NULL;
            }
            return -1;
        }

        fget_attrs(__this_p_hd->file, &attr);
        printf("file size:%d", attr.fsize);
        printf("line:%d", __LINE__);
        int fsize = attr.fsize;
        if (fsize < PHONE_BOOK_LEN) {
            if (__this_p_hd) {
                free(__this_p_hd->phonebook);
                free(__this_p_hd);
                __this_p_hd = NULL;
            }
            return -1;
        }
        while (1) {
            int len;
            fseek(__this_p_hd->file, __this_p_hd->count * PHONE_BOOK_LEN, SEEK_SET);
            len = fread(__this_p_hd->file, __this_p_hd->phonebook, PHONE_BOOK_LEN);
            __this_p_hd->count ++;
            fsize  -= len;
            if (fsize <= 0) {
                break;
            }
        }
    } else {

        if (!__this_c_hd) {
            __this_c_hd = zalloc(sizeof_this_hd);
            __this_c_hd->show_temp = show_temp + !show_temp;
            __this_c_hd->phonebook = malloc(sizeof(struct phonebook) * __this_c_hd->show_temp);
        }

        if (__this_c_hd->file) {
            fclose(__this_c_hd->file);
            __this_c_hd->file = 0;
        }

        __this_c_hd->file = fopen(CALL_LOGS_PATH, "r");
        if (!__this_c_hd->file) {
            printf("opne fail \n");
            if (__this_c_hd) {
                free(__this_c_hd->phonebook);
                free(__this_c_hd);
                __this_c_hd = NULL;
            }
            return -1;
        }

        fget_attrs(__this_c_hd->file, &attr);
        printf("file size:%d", attr.fsize);
        printf("line:%d", __LINE__);
        int fsize = attr.fsize;
        while (1) {
            int len;
            fseek(__this_c_hd->file, __this_c_hd->count * CALL_LOGS_LEN, SEEK_SET);
            __this_c_hd->phonebook->mask = 0;
            len = fread(__this_c_hd->file, __this_c_hd->phonebook, CALL_LOGS_LEN);
            if (BOOK_MASK == __this_c_hd->phonebook->mask) {
                __this_c_hd->count ++;
            } else {
                printf("phonebook file need delete\n");
                break;
            }
            fsize  -= len;
            printf("name = %s number=%s data=%s %d\n", __this_c_hd->phonebook->name, __this_c_hd->phonebook->number, __this_c_hd->phonebook->date, __this_c_hd->count);
            if (fsize <= 0) {
                break;
            }
        }
        if (!__this_c_hd->count) {
            delete_call_file();
        }

    }
    return 0;
#endif
}

void close_file_handler(u8 type)
{
#if TCFG_NOR_VM

    if (type == 0) {
        if (__this_p_hd) {
            free(__this_p_hd->phonebook);
            free(__this_p_hd);
            __this_p_hd = NULL;
        }
    } else {
        if (__this_c_hd) {
            free(__this_c_hd->phonebook);
            free(__this_c_hd);
            __this_c_hd = NULL;
        }
    }

#else
    if (type == 0) {
        if (__this_p_hd && __this_p_hd->file) {
            fclose(__this_p_hd->file);
            free(__this_p_hd->phonebook);
            free(__this_p_hd);
            __this_p_hd = NULL;
        }
    } else {
        if (__this_c_hd && __this_c_hd->file) {
            fclose(__this_c_hd->file);
            free(__this_c_hd->phonebook);
            free(__this_c_hd);
            __this_c_hd = NULL;
        }
    }
#endif
}

void delete_call_file()
{
#if TCFG_NOR_VM
    while (flash_common_get_total(get_flash_vm_hd(F_TYPE_CALL_LOG))) {
        flash_common_delete_by_index(get_flash_vm_hd(F_TYPE_CALL_LOG), 0);
    }
#else
    if (__this_c_hd && __this_c_hd->file) {
        fdelete(__this_c_hd->file);
        free(__this_c_hd->phonebook);
        free(__this_c_hd);
        __this_c_hd = NULL;
    }
#endif
}


static phonebook *book_list_read_by_index_in_exflash(u8 type, u32 index)//从1开始计数
{
    struct vfs_attr attr = {0};
    index = index - 1;//index 是 无符号 ,0 -1 会变成-1
    struct phonebook_file *handler = NULL;

    if (!type && !__this_p_hd) {
        return NULL;
    }

    if (type && !__this_c_hd) {
        return NULL;
    }

    if (!type) {
        handler = __this_p_hd;
    } else {
        handler = __this_c_hd;
    }

    if (index >= handler->count) {
        return NULL;
    }

    phonebook *phonebook = handler->phonebook + (index % handler->show_temp);

    if (!type) {
        fseek(handler->file, index * PHONE_BOOK_LEN, SEEK_SET);
        fread(handler->file, phonebook, PHONE_BOOK_LEN);
    }else{
        fseek(handler->file, index * CALL_LOGS_LEN, SEEK_SET);
        /* printf("pos = %d index = %d  set = %d \n",fpos(handler->file),index,index*CALL_LOGS_LEN); */
        fread(handler->file, phonebook, CALL_LOGS_LEN);
        if (BOOK_MASK != phonebook->mask) {
            return NULL;
        }
    }
    phonebook->number[19] = 0;
    int len = strlen(phonebook->number);

    if (len >= sizeof(phonebook->number)) {
        len = sizeof(phonebook->number) - 1;
    }
    memset(phonebook->number + len, ' ', 19 - len);

    /* printf(">>>>>> %s %d\n",__FUNCTION__,__LINE__); */
    /* printf("name = %s number=%s data=%s %d\n",phonebook->name,phonebook->number,phonebook->date,index); */
    return phonebook;
}

static void book_list_save_in_exflash(u8 type, const u8 *name, const u8 *number, const u8 *date)
{
    void *pbap_file_handler;
    struct vfs_attr attr = {0};
    u16 len = 0;
    if (type == 0) {
        return;
    }
    struct phonebook message;
    message.type = type;
    if (name) {
        snprintf(message.name, sizeof(message.name), "%s", name);
    }

    if (date) {
        snprintf(message.date, sizeof(message.date), "%s", date);
    } else {
        snprintf(message.date, sizeof(message.date), "%s", "0000/00/00T00:00:00");
    }

    if (number) {
        snprintf(message.number, sizeof(message.number), "%s", number);
    }
    
    pbap_file_handler = fopen(CALL_LOGS_PATH, "w+");
    if (!pbap_file_handler) {
        return;
    }
    message.mask = BOOK_MASK;
    fget_attrs(pbap_file_handler, &attr);
    fseek(pbap_file_handler, attr.fsize, SEEK_SET);
    len = fwrite(pbap_file_handler, &message, CALL_LOGS_LEN);
    if (len != CALL_LOGS_LEN) {
        g_printf("call logs write err");
    }
    fclose(pbap_file_handler);
}


static u32 book_list_get_count_in_exflash(u8 type)
{
    if (!type) {
        if (!__this_p_hd) {
            return 0;
        }
        return __this_p_hd->count;
    } else {
        if (!__this_c_hd) {
            return 0;
        }
        return __this_c_hd->count;
    }
}

int phonebook_get_name_by_number(u8 *number, u8 *name)
{
#if TCFG_NOR_VM
    struct phonebook temp_phonebook_buf;
    u8 temp_number[20] = {0};
    u8 vaild_flag = 0;
    int fsize =  flash_common_get_file_size_by_index(get_flash_vm_hd(F_TYPE_PHONEBOOK), 0);
    /* printf("size =%d\n",fsize); */
    ASSERT(!(fsize % PHONE_BOOK_LEN));
    for (int i = 0; i < fsize ; i += PHONE_BOOK_LEN) {
        int ret = flash_common_read_by_index(get_flash_vm_hd(F_TYPE_PHONEBOOK), 0, i, PHONE_BOOK_LEN, (u8 *)&temp_phonebook_buf);
        if (ret != PHONE_BOOK_LEN) {
            printf("%s %d,size= %d,len = %d,offset = %x\n", __FUNCTION__, __LINE__, fsize, ret, i);
            ASSERT(0);
        }


        int number_len = strlen(temp_phonebook_buf.number);

        memset(temp_number, 0, sizeof(temp_number));
        for (int j = 0, k = 0; j < number_len; j++) {
            if (temp_phonebook_buf.number[j] != ' ') { //去空格
                temp_number[k] =  temp_phonebook_buf.number[j];
                k++;
            }
        }

        /* printf("%s %s,%s i=%d,%d,%d",number,temp_phonebook_buf.number,temp_number,i,strlen(number),strlen(temp_phonebook_buf.number)); */

        if (!strcmp(number, temp_number)) {
            memcpy(name, temp_phonebook_buf.name, 20);
            vaild_flag = 1;
            break;
        }
    }
    if (vaild_flag) {
        return strlen(name);
    } else {
        return 0;
    }

#else

    struct phonebook temp_phonebook_buf;
    struct vfs_attr attr = {0};
    void *temp_hdl;
    u16 count = 0;
    u8 vaild_flag = 0;
    temp_hdl = fopen(PHONE_BOOK_PATH, "r");

    if (!temp_hdl) {
        printf("%s open phonebook file fail\n", __FUNCTION__);
        return 0;
    }

    fget_attrs(temp_hdl, &attr);
    printf("file size:%d %d", attr.fsize, __LINE__);
    if (attr.fsize < PHONE_BOOK_LEN) {
        printf("fsize less than PHONE_BOOK_LEN\n");
        if (temp_hdl) {
            fclose(temp_hdl);
        }
        return 0;
    }

    int fsize = attr.fsize;
    while (1) {
        int len;
        fseek(temp_hdl, count * PHONE_BOOK_LEN, SEEK_SET);
        len = fread(temp_hdl, &temp_phonebook_buf, PHONE_BOOK_LEN);
        if (!strcmp(number, temp_phonebook_buf.number)) {
            memcpy(name, temp_phonebook_buf.name, 20);
            vaild_flag = 1;
            break;
        }
        count ++;
        fsize  -= len;
        if (fsize <= 0) {
            break;
        }
    }

    if (temp_hdl) {
        fclose(temp_hdl);
    }

    printf("%s:%s\n", __FUNCTION__, name);
    if (vaild_flag) {
        return strlen(name);
    } else {
        return 0;
    }
#endif
}

phonebook *book_list_read_by_index(u8 type, u8 index)
{
#if TCFG_NOR_VM
    index = index - 1;//index 是 无符号 ,0 -1 会变成-1
    struct phonebook_file *handler = NULL;
    if (!type && !__this_p_hd) {
        return NULL;
    }

    if (type && !__this_c_hd) {
        return NULL;
    }

    if (!type) {
        handler = __this_p_hd;
    } else {
        handler = __this_c_hd;
    }

    if (index >= handler->count) {
        return NULL;
    }

    phonebook *phonebook = handler->phonebook + (index % handler->show_temp);


    if (!type) {
        memset(phonebook, 0, PHONE_BOOK_LEN);
        flash_common_read_by_index(get_flash_vm_hd(F_TYPE_PHONEBOOK), 0, index * PHONE_BOOK_LEN, PHONE_BOOK_LEN, (u8 *)phonebook);
    } else {
        memset(phonebook, 0, CALL_LOGS_LEN);
        flash_common_read_by_index(get_flash_vm_hd(F_TYPE_CALL_LOG), index, 0, CALL_LOGS_LEN, (u8 *)phonebook);
    }
    phonebook->number[19] = 0;
    int len = strlen(phonebook->number);

    if (len >= sizeof(phonebook->number)) {
        len = sizeof(phonebook->number) - 1;
    }
    memset(phonebook->number + len, ' ', 19 - len);
    return phonebook;

#else
    return book_list_read_by_index_in_exflash(type, index);
#endif
}

u32 book_list_get_count(u8 type)
{
#if TCFG_NOR_VM
    if (!type) {
        if (!__this_p_hd) {
            return 0;
        }
        return __this_p_hd->count;
    } else {
        if (!__this_c_hd) {
            return 0;
        }
        return __this_c_hd->count;
    }
#else
    return book_list_get_count_in_exflash(type);
#endif
}

void book_list_save(u8 type, const u8 *name, const u8 *number, const u8 *date)
{
#if TCFG_NOR_VM
    u16 len = 0;
    if (type == 0) {
        return;
    }
    struct phonebook message;
    message.type = type;
    if (name) {
        snprintf(message.name, sizeof(message.name), "%s", name);
    }

    if (date) {
        snprintf(message.date, sizeof(message.date), "%s", date);
    } else {
        snprintf(message.date, sizeof(message.date), "%s", "0000/00/00T00:00:00");
    }

    if (number) {
        snprintf(message.number, sizeof(message.number), "%s", number);
    }

    /* if (len != CALL_LOGS_LEN) { */
    /*     g_printf("call logs write err"); */
    /* } */

    while (flash_common_get_total(get_flash_vm_hd(F_TYPE_CALL_LOG)) == get_flash_vm_number_max(F_TYPE_CALL_LOG)) {
        flash_common_delete_by_index(get_flash_vm_hd(F_TYPE_CALL_LOG), 0);
    }
    flash_common_write_file(get_flash_vm_hd(F_TYPE_CALL_LOG), 0, CALL_LOGS_LEN, (u8 *)&message);

#else
    book_list_save_in_exflash(type, name, number, date);
#endif
}
#endif
#endif