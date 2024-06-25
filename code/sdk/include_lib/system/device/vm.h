#ifndef _VM_H_
#define _VM_H_

#include "ioctl.h"
#include "device/device.h"

#define IOCTL_SET_VM_INFO               _IOW('V', 1, 1)
#define IOCTL_GET_VM_INFO               _IOW('V', 2, 1)
// enum {
//     #<{(|
//      *  用户自定义配置项 (0-64)
//      |)}>#
//     #<{(| ... |)}>#
// }

// VM define and api
typedef u16 vm_hdl;

struct vm_table {
    u16  index;
    u16 value_byte;
    int value;      //cache value which value_byte <= 4
};

typedef enum _vm_err {
    VM_ERR_NONE = 0,
    VM_INDEX_ERR = -0x100, //-256
    VM_INDEX_EXIST,     //0xFF -255
    VM_DATA_LEN_ERR,    //0xFE -254
    VM_READ_NO_INDEX,   //0xFD -253
    VM_READ_DATA_ERR,   //0xFC -252
    VM_WRITE_OVERFLOW,  //0xFB -251
    VM_NOT_INIT,        //-250
    VM_INIT_ALREADY,    //-249
    VM_DEFRAG_ERR,      //-248
    VM_ERR_INIT,        //-247
    VM_ERR_PROTECT      //-246
} VM_ERR;


// vm api
/**@brief  VM区域擦除函数
  @param  void
  @note   VM_ERR VM_ERR vm错误码
 */
VM_ERR vm_eraser(void);
/**
 * @brief VM初始化函数，所有VM操作之前都必须先初始化
 *
 * @param  [in] dev_hdl NULL
 * @param  [in] vm_addr VM起始地址
 * @param  [in] vm_len VM总大小空间
 * @param  [in] vm_mode 设置最小操作单元，目前支持按sector和按page写入
 *
 * @return VM_ERR vm错误码
 */
VM_ERR vm_init(void *dev_hdl, u32 vm_addr, u32 vm_len, u8 vm_mode);
/**
 * @brief 返回vm区域已经使用了多少。
 *
 * @return 大小(单位：bit)
 */
/* --------------------------------------------------------------------------*/
u32 vm_get_usage(void);
/* --------------------------------------------------------------------------*/
/**
 * @brief 按照百分比的形式返回vm区域已经使用了多少。建议设置在main函数的board_early_init函数的前面
 *
 * @return 百分比(0-99)
 */
/* --------------------------------------------------------------------------*/
u8 vm_get_usage_percent(void);

/* --------------------------------------------------------------------------*/
/**
 * @brief 按百分比的形式设置vm模块触发区域整理的警告线高度。注：该函数应该在vm_init初始化之前调用，否则vm_init会默认设置为vm_warning_line_num默认参数（80%）
 *
 * @param  [in] vm_warning_line_num 警告线百分比(0-100)
 */
/* --------------------------------------------------------------------------*/
void vm_set_warning_line(u8 vm_warning_line_num);

/*----------------------------------------------------------------------------*/
/**@brief  VM_WARNING_LINE检测函数
  @param  void：默认检测单前使用区域
  @return FALSE：未到达警告线，TRUE：以到达警告
  @note   用于整理VM前检测
 */
/*----------------------------------------------------------------------------*/
bool vm_warning_line_check(void);

/*----------------------------------------------------------------------------*/
/**@brief  VM检测是否到达百分比警告线，到达则整理VM区域，不到达则清除一个flash最小操作单元
  @param  level：0：查询VM是否需要整理，并进行一次擦除vm未使用区域的操作 1：整理全VM区域
  @return NON
  @note   当调用vm_write写入的累计内容到达百分比警告线时会触发vm整理区域机制，会长时间操作flash，因此出现系统卡顿现象。虽然vm写操作会擦除一次最小flash操作单元，但不一定能保证在vm已经使用区域大小到达警告线的时候可以完全擦除vm的未使用区域，所以外部app应该在实现流程不忙碌的时候调用vm_check_all(0)参数查询VM是否需要整理，并进行一次擦除操作。
 */
//VM_ERR vm_db_create_table(const struct vm_table *table, int num);
void vm_check_all(u8 level);    //level : default 0
/**
 * @brief VM不检测是否到达警告线，立即快速清除已经使用的flash空间并整理VM区域
 *
 * @return VM_ERR vm错误码
 */
/* --------------------------------------------------------------------------*/
VM_ERR vm_defrag_used(void);

/* --------------------------------------------------------------------------*/
/**
 * @brief VM不检测是否到达警告线，立即清除全部使用的flash空间并整理VM区域。注：该函数会擦除整块属于vm使用的flash区域，耗时较长，建议使用vm_defrag_used
 *
 * @return VM_ERR vm错误码
 */
/* --------------------------------------------------------------------------*/
VM_ERR vm_defrag_all(void);

/*----------------------------------------------------------------------------*/
/**@brief  VM模块重置，擦除掉vm占用的flash的内容，耗时较长。
  @return VM_ERR vm错误码
  @note   用于整理VM前检测
 */
/*----------------------------------------------------------------------------*/
VM_ERR vm_reset(void);
u8   get_vm_statu(void);
// io api
//s32 vm_read(vm_hdl hdl, void *data_buf, u16 len);
//s32 vm_write(vm_hdl hdl, const void *data_buf, u16 len);

void spi_port_hd(u8 level);

bool sfc_erase_zone(u32 addr, u32 len);

void vm_api_write_mult(u16 start_id, u16 end_id, void *buf, u16 len, u32 delay);
int vm_api_read_mult(u16 start_id, u16 end_id, void *buf, u16 len);


#endif  //_VM_H_

