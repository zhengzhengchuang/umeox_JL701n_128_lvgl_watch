#ifndef __UI_F_VM_CFG_H__
#define __UI_F_VM_CFG_H__

extern void *get_flash_vm_hd(u8 type);
extern u8 get_flash_vm_number_max(u8 type);

#if 0
#define PHONEBOOK_SAVE_INSIDE_FLASH     0

#if (TCFG_NOR_FS && WATCH_FILE_TO_FLASH)
#if TCFG_NOR_FAT
#define CALL_LOGS_PATH      "storage/fat_nor/C/calllog.txt"
#define PHONE_BOOK_PATH      "storage/fat_nor/C/call.txt"
#else // TCFG_NOR_FAT
#define CALL_LOGS_PATH      "storage/virfat_flash/C/calllog.txt"
#define PHONE_BOOK_PATH      "storage/virfat_flash/C/call.txt"
#endif // TCFG_NOR_FAT
#else // TCFG_NOR_FS
#define CALL_LOGS_PATH      "storage/sd1/C/DOWNLOAD/calllog.txt"
#define PHONE_BOOK_PATH      "storage/sd1/C/DOWNLOAD/call.txt"
#endif // TCFG_NOR_FS

#define BOOK_MASK (0x1234)
//结构体新成员在最后加
typedef struct phonebook {
    char name[20];
    char number[20];
    char date[20];
    u8 type;
    u32 mask;
} phonebook;

#define PHONE_BOOK_LEN            (40)
#define CALL_LOGS_LEN             (sizeof(struct phonebook))
#define PBAP_MAX_NUM              (10)
#define MAX_CALL_LOG_NUM          (30)

extern int flash_message_init(const char *name, int max);

extern void flash_message_cfg_reset();

extern void flash_message_release();

extern int flash_message_write(u8 *data, u16 len);

extern int flash_message_read_by_index(u8 index, u8 *data, u16 len);

extern int flash_message_delete_by_mask(u32 mask);

extern int flash_message_delete_by_index(u32 index);

extern int flash_message_count();

extern int flash_weather_init(const char *name);

extern int flash_weather_read(u8 *data, u16 len);

extern int flash_weather_write(u8 *data, u16 len);

extern int open_file_handler(u8 type, int show_temp);
extern void close_file_handler(u8 type);
extern void delete_call_file();

extern int phonebook_get_name_by_number(u8 *number, u8 *name);
extern phonebook *book_list_read_by_index(u8 type, u8 index);
extern u32 book_list_get_count(u8 type);

extern void set_call_log_type(u8 type);
extern void set_call_log_name(u8 *name);
extern void set_call_log_number(u8 *number);
extern void set_call_log_date(u8 *date);
extern void set_call_log_message(u8 type, const u8 *name, \
    const u8 *number, const u8 *date);
#endif
#endif
