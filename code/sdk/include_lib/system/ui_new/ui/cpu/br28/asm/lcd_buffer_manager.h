#ifndef __LCD_BUFFER_MANAGER_H__
#define __LCD_BUFFER_MANAGER_H__

#include "generic/list.h"

struct lcd_buffer_head {
    struct list_head head;
    u8 *pre_buf;
    u8 buffer_num;
};

// 缓存buf的状态
typedef enum lcd_buf_status {
    LCD_BUFFER_INIT,		// 初始状态，防止还没写数据就推屏
    LCD_BUFFER_IDLE,		// 空闲
    LCD_BUFFER_LOCK,		// 锁定
    LCD_BUFFER_PENDING,		// 等待
    LCD_BUFFER_INUSED,		// 使用
} BUF_STATUS;


// lcd buffer
struct lcd_buffer {
    struct list_head entry;
    BUF_STATUS status;
    u8 *baddr;
};

u8 *lcd_buffer_init(u8 index, u8 *baddr, u32 size, u8 buffer_num);
void lcd_buffer_release(u8 index);
u8 *lcd_buffer_get(u8 index, u8 *pre_baddr);
u8   lcd_buffer_pending(u8 index, u8 *buffer);
void lcd_buffer_idle(u8 index);
u8 *lcd_buffer_next(u8 index);
u8 *lcd_buffer_check(u8 index, u8 *lcd_buf);

#endif
