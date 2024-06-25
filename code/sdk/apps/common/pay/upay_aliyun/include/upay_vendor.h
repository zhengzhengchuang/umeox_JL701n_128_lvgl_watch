/****************************************************************************
 * IoTPay modules 1.0 2017/10/19
 * This software is provided "AS IS," without a warranty of any kind. ALL
 * EXPRESS OR IMPLIED CONDITIONS, REPRESENTATIONS AND WARRANTIES, INCLUDING
 * ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * OR NON-INFRINGEMENT, ARE HEREBY EXCLUDED. ALIPAY, INC.
 * AND ITS LICENSORS SHALL NOT BE LIABLE FOR ANY DAMAGES SUFFERED BY LICENSEE
 * AS A RESULT OF USING, MODIFYING OR DISTRIBUTING THIS SOFTWARE OR ITS
 * DERIVATIVES. IN NO EVENT WILL ALIPAY OR ITS LICENSORS BE LIABLE FOR ANY LOST
 * REVENUE, PROFIT OR DATA, OR FOR DIRECT, INDIRECT, SPECIAL, CONSEQUENTIAL,
 * INCIDENTAL OR PUNITIVE DAMAGES, HOWEVER CAUSED AND REGARDLESS OF THE THEORY
 * OF LIABILITY, ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE,
 * EVEN IF ALIPAY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 ****************************************************************************
 * You acknowledge that this software is not designed, licensed or intended
 * for use in the design, construction, operation or maintenance of any
 * nuclear facility.
*****************************************************************************/
#ifndef UPAY_VENDOR_H
#define UPAY_VENDOR_H

#include "upay_common.h"


//业务数据API，需要一块单独大小为12K的区域存储，要求恢复出厂设置必须被清除、用户OTA不能被清除

/**
 * 业务数据读取，大小为12K，需要厂商实现
 *
 * @param[in]   fd    do not use
 * @param[in]   offset    the offset of the reserved partition
 * @param[out]  read_buff      the data need to write
 * @param[in]   read_len  the length of the data need to write
 *
 * @return 0 : On success, EIO : If an error occurred with any step
 */
EXTERNC int aos_hal_kv_nv_read(int fd, uint32_t offset, uint8_t *read_buff, uint32_t read_len);

/**
 * 业务数据写入，大小为12K，需要厂商实现
 *
 * @param[in]   fd    do not use
 * @param[in]   offset    the offset of the reserved partition
 * @param[out]  write_buff      the data need to write
 * @param[in]   write_len  the length of the data need to write
 *
 * @return 0 : On success, EIO : If an error occurred with any step
 */
EXTERNC int aos_hal_kv_nv_write(int fd, uint32_t offset, uint8_t *write_buff, uint32_t write_len);

/**
 * 业务数据写入，大小为12K，需要厂商实现
 *
 * @param[in]   fd    do not use
 * @param[in]   offset    the offset of the reserved partition
 * @param[in]   size  the length of the data need to erase
 *
 * @return 0 : On success, EIO : If an error occurred with any step
 */
EXTERNC int aos_hal_kv_nv_erase(int fd, uint32_t off_set, uint32_t size);

//安全数据API，需要一块单独大小为2.5K的区域存储，要求恢复出厂设置不能被清除、OTA不能被清除、常规烧录升级不能被清除
/**
 * 安全数据写入，大小为2.5K，需要厂商实现
 *
 * @param[in]   offset    the offset of the reserved partition
 * @param[out]  data      the data need to write
 * @param[in]   write_len  the length of the data need to write
 *
 * @return 0 : On success, EIO : If an error occurred with any step
 */
EXTERNC int32_t hal_flash_id2_write(uint32_t offset, void *data, uint32_t write_len);

/**
 * 安全数据读取，大小为2.5K，需要厂商实现
 *
 * @param[in]   offset    the offset of the reserved partition
 * @param[out]  data      the data need to read
 * @param[in]   read_len  the length of the data need to read
 *
 * @return 0 : On success, EIO : If an error occurred with any step
 */
EXTERNC int32_t hal_flash_id2_read(uint32_t offset, void *data, uint32_t read_len);
/**
 * 安全数据擦除，大小为2.5K，需要厂商实现
 *
 * @param[in]   erase_offset    the offset of the reserved partition
 * @param[in]   earse_len  the length of the data need to erase
 *
 * @return 0 : On success, EIO : If an error occurred with any step
 */

EXTERNC int32_t hal_flash_id2_erase(uint32_t erase_offset, uint32_t earse_len);


/*获取产品型号，只能包含大写字母、数字、下划线(_)和横杠(-)，长度要求不超过10个字节(不包含结束符)
 * @param [in] buffer  存放产品型号的缓存
 * @param [in out] len 输入为允许读取的最大长度；输出时为实际读取的长度，长度不包含结束符
 * @return void
 */


//安全数据API，需要一块单独大小为3K的区域存储，要求恢复出厂设置不能被清除、OTA不能被清除、常规烧录升级不能被清除、与上面的2.5k要求一致，可以放在一个区域

/*
* erase alipay secure partition
*
* parametr: in:  fd: file handle, can be ignored if no file system
*                offset: the offset of the factory partition
*                size: the length of the data need to erase
* return: 0: success
-1: fail
*/
EXTERNC int hal_secure_partition_erase(int fd, uint32_t offset, uint32_t size);

/*
* read alipay secure partition
*
* parametr: in:  fd: file handle, can be ignored if no file system
*                offset: the offset of the factory partition
*                read_len: the length of the data need to read
*           out: buffer: the data read from the factory part
* return: 0: success
-1: fail
*/
EXTERNC int hal_secure_partition_read(int fd, uint32_t offset, void *buffer, uint32_t read_len);

/*
* write alipay secure  partition
*
* parametr: in: fd: file handle, can be ignored if no file system
*               offset: the offset of the factory partition
*               buffer: the data need to write
*               write_len: the length of the data need to write
*
* return: 0: success
*         -1: fail
*/
EXTERNC int hal_secure_partition_write(int fd, uint32_t offset, void *buffer, uint32_t write_len);

EXTERNC void upay_get_productModel(uint8_t *buffer, uint32_t *len);

/////////////////////////////////////////////
////获取当前系统时间戳
/////////////////////////////////////////////
EXTERNC uint32_t upay_get_timestamp(void);

/*BLE写接口:
 * @param [in] data 待发送数据的指针
 * @param [in] len  待发送数据的大小
 * @return void
 */
EXTERNC void upay_ble_write(uint8_t *data, uint16_t len);

/////////////////////////////////////////////
////获取设备ID(以冒号分割的16进制mac地址)
////要求内容以‘\0’结尾且长度不包含'\0'
/////////////////////////////////////////////
EXTERNC upay_retval_t upay_get_deviceid(uint8_t *buf_did, uint32_t *len_did);

/////////////////////////////////////////////
////获取设备SN(厂商印刷在卡片上的设备序列号)
////长度不超过32个字符，只能包含大小写字母、数字、下划线
////要求内容以‘\0’结尾且长度不包含'\0'
////其他厂商请输出buf_sn=""(空字符串)，len_sn=0
/////////////////////////////////////////////
EXTERNC upay_retval_t upay_get_sn(uint8_t *buf_sn, uint32_t *len_sn);

/////////////////////////////////////////////
////日志信息输出接口
/////////////////////////////////////////////
typedef enum {
    UPAY_LEVEL_FATAL = 0x01,
    UPAY_LEVEL_ERRO,
    UPAY_LEVEL_WARN,
    UPAY_LEVEL_INFO,
    UPAY_LEVEL_DBG,
} UPAY_log_level;

/////////////////////////////////////////////////////////////////////////////////////
// 由于有些设备上不支持输出动态string, 所以没有定义成变长参数，
// 只会输出一些关键的trace log, 便于发现问题时快速定位
// 注: format参数中可能含有静态string, 实现时不可忽略
/////////////////////////////////////////////////////////////////////////////////////
EXTERNC void upay_log(UPAY_log_level, const char *format, int32_t value);
EXTERNC void upay_log_ext(const char *format, ...);


/*获取公司名称，公司名称为字符串格式并且字母要求全部大写，长度要求不超过15个字节(不包含结束符)，
 *如果公司全称不超过15个字节，则使用全称比如 "HUAWEI" "GARMIN"；如果超过15个字节，请优先使用中文拼音
 *则第一个字的拼音为完整，后续为拼音首字母的缩写，比如 xiaotiancai应写为 "XIAOTC"；若一定要使用英文
 *的公司名称，请与alipay同学协商。
 * @param [in] buffer  存放公司名称的缓存
 * @param [in out] len 输入为允许读取的最大长度；输出时为实际读取的长度，长度不包含结束符
 * @return void
 */
void upay_get_companyName(uint8_t *buffer, uint32_t *len);

/*获取编译时候的时间戳，单位为秒
 * @return 编译时候的时间戳，单位为秒
 */
/*获取编译时间，返回UTC时间戳，单位为s，用于辅助版本校验*/
EXTERNC uint32_t upay_get_compile_timestamp();

#endif
