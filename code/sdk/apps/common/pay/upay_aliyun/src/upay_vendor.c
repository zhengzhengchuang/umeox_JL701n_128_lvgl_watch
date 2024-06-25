//该文件是需要厂商实现的底层接口
#include "upay.h"
#include "upay_vendor.h"
#include "custom_cfg.h"
#include "le_smartbox_adv.h"
#include "btstack/le/ble_api.h"
#include "timestamp.h"
#include "app_config.h"
#include "ui/watch_syscfg_manage.h"

#if TCFG_PAY_ALIOS_ENABLE && (TCFG_PAY_ALIOS_WAY_SEL==TCFG_PAY_ALIOS_WAY_ALIYUN)

#define vendor_printf(...) printf(__VA_ARGS__)

/* #define MY_DEBUG(...) */
#define MY_DEBUG_BUF(x,y)
#define MY_DEBUG	printf
/* #define MY_DEBUG_BUF	put_buf */

#define CST_OFFSET_TIME			(8*60*60)	// 北京时间时差

extern int le_controller_get_mac(void *addr);
extern void bt_addr2string(u8 *addr, u8 *buf);

#define AOS_HAL_KV_NV_LEN			(12*1024)
#define AOS_HAL_ID2_LEN				(2 * 1024 + 512)
#define AOS_HAL_MAC_LEN				(512)
#define AOS_HAL_SECRET_LEN	      	(3 * 1024)


#define UPAY_DATA_BUF_TEST		0

#if UPAY_DATA_BUF_TEST
static u8 test_kv_nv_buf[AOS_HAL_KV_NV_LEN] = {0};
static u8 test_id2_buf[AOS_HAL_ID2_LEN] = {0};
#else

#define AOS_HAL_KV_NV_OFFSET		(0)
#define AOS_HAL_ID2_OFFSET			(AOS_HAL_KV_NV_OFFSET + AOS_HAL_KV_NV_LEN)
#define AOS_MAC_OFFSET				(AOS_HAL_ID2_OFFSET + AOS_HAL_ID2_LEN)
#define AOS_SECR_OFFSET				((AOS_MAC_OFFSET + AOS_HAL_MAC_LEN + 4095)&(~4095))//尽量4k对齐

#define ALIYUN_MAC_FLAG				0x12345A5A

struct aliyun_mac {
    u32 flag;
    u16 crc;
    u8 mac_addr[6];
    u8 ble_addr[6];
};

extern int aliyun_storage_read(int offset, u8 *data, int len);
extern int aliyun_storage_write(int offset, u8 *data, int len);
extern int aliyun_storage_earse(int offset, int len);

#endif

extern int aos_hal_kv_nv_flush(void);

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
int aos_hal_kv_nv_read(int fd, uint32_t offset, uint8_t *read_buff, uint32_t read_len)
{
    int ret = 0;
#if UPAY_DATA_BUF_TEST
    memcpy(read_buff, &test_kv_nv_buf[offset], read_len);
#else
    int rlen = aliyun_storage_read(AOS_HAL_KV_NV_OFFSET + offset, read_buff, read_len);
    if (rlen != read_len) {
        ret = EIO;
    }
#endif
    MY_DEBUG("kv_nv_read:%d, offset:0x%x, len:%d \n", ret, offset, read_len);
    MY_DEBUG_BUF(read_buff, read_len);
    // int ret = 0;
    // ret = 	_fdb_read(read_buff, read_len);
    // rt_kprintf("aos_hal_kv_nv_read len = %d ret = %d\n", read_len,ret);
    // return ret;
    //TODO:
    return ret;

}
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
int aos_hal_kv_nv_write(int fd, uint32_t offset, uint8_t *write_buff, uint32_t write_len)
{
    int ret = 0;
#if UPAY_DATA_BUF_TEST
    memcpy(&test_kv_nv_buf[offset], write_buff, write_len);
#else
    int wlen = aliyun_storage_write(AOS_HAL_KV_NV_OFFSET + offset, write_buff, write_len);
    if (wlen != write_len) {
        ret = EIO;
    }
#endif
    MY_DEBUG("kv_nv_write:%d, offset:0x%x, len:%d \n", ret, offset, write_len);
    MY_DEBUG_BUF(write_buff, write_len);
    // int ret = 0;
    // ret = _fdb_write(write_buff,write_len);
    // rt_kprintf("aos_hal_kv_nv_write len = %d ret = %d\n", write_len,ret);
    // return ret;
    //TODO:
    return ret;
}


//如因为资源问题，去掉了库中的12K缓存机制，则此API需要厂商实现
int aos_hal_kv_nv_erase(int fd, uint32_t off_set, uint32_t size)
{
    printf("%s %d\n", __func__, __LINE__);

    int ret = 0;
    int wlen = aliyun_storage_earse(AOS_HAL_KV_NV_OFFSET + off_set, size);
    if (wlen != size) {
        ret = EIO;
    }

    MY_DEBUG("id2_erase:%d, offset:0x%x, len:%d \n", ret, off_set, size);

    return ret;
}

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
int32_t hal_flash_id2_write(uint32_t offset, void *data, uint32_t write_len)
{
    int ret = 0;
#if UPAY_DATA_BUF_TEST
    memcpy(&test_id2_buf[offset], data, write_len);
#else
    int wlen = aliyun_storage_write(AOS_HAL_ID2_OFFSET + offset, data, write_len);
    if (wlen != write_len) {
        ret = EIO;
    }
#endif
    MY_DEBUG("id2_write:%d, offset:0x%x, len:%d \n", ret, offset, write_len);
    MY_DEBUG_BUF(data, write_len);
    //TODO:
    return ret;
}

/**
 * 安全数据读取，大小为2.5K，需要厂商实现
 *
 * @param[in]   offset    the offset of the reserved partition
 * @param[out]  data      the data need to read
 * @param[in]   read_len  the length of the data need to read
 *
 * @return 0 : On success, EIO : If an error occurred with any step
 */
int32_t hal_flash_id2_read(uint32_t offset, void *data, uint32_t read_len)
{
    int ret = 0;
#if UPAY_DATA_BUF_TEST
    memcpy(data, &test_id2_buf[offset], read_len);
#else
    int rlen = aliyun_storage_read(AOS_HAL_ID2_OFFSET + offset, data, read_len);
    if (rlen != read_len) {
        ret = EIO;
    }
#endif
    MY_DEBUG("id2_read:%d, offset:0x%x, len:%d \n", ret, offset, read_len);
    MY_DEBUG_BUF(data, read_len);
    //TODO:
    return ret;
}



/**
 * 安全数据擦除，大小为2.5K，需要厂商实现
 *
 * @param[in]   erase_offset    the offset of the reserved partition
 * @param[in]   earse_len  the length of the data need to erase
 *
 * @return 0 : On success, EIO : If an error occurred with any step
 */

int32_t hal_flash_id2_erase(uint32_t erase_offset, uint32_t earse_len)
{
    //因为我们平台demo写已经实现了刷功能,可以不实现这个
    return 0;
}




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
int hal_secure_partition_erase(int fd, uint32_t offset, uint32_t size)
{
    //TODO:
    return 0;
}
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
int hal_secure_partition_read(int fd, uint32_t offset, void *buffer, uint32_t read_len)
{
    int ret = 0;
    int rlen = aliyun_storage_read(AOS_SECR_OFFSET + offset, buffer, read_len);
    if (rlen != read_len) {
        ret = EIO;
    }
    MY_DEBUG("%s :base %x %d, offset:0x%x, len:%d \n", __func__, AOS_SECR_OFFSET, ret, offset, read_len);
    MY_DEBUG_BUF(buffer, read_len);
    return ret;
}
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
int hal_secure_partition_write(int fd, uint32_t offset, void *buffer, uint32_t write_len)
{


    int wlen = aliyun_storage_write(AOS_SECR_OFFSET + offset, buffer, write_len);
    int ret = 0;
    if (wlen != write_len) {
        ret = EIO;
    }
    MY_DEBUG("%s :base %x %d, offset:0x%x, len:%d \n", __func__, AOS_SECR_OFFSET, ret, offset, write_len);
    MY_DEBUG_BUF(buffer, read_len);

    return ret;
}

/////////////////////////////////////////////
////获取当前系统时间戳
/////////////////////////////////////////////
uint32_t upay_get_timestamp(void)
{
    // 需要厂商实现
    struct sys_time time;
    void *fd = dev_open("rtc", NULL);
    if (!fd) {
        MY_DEBUG("upay_get_timestamp err!!!\n");
        return UPAY_RV_IO_ERROR;
    }
    dev_ioctl(fd, IOCTL_GET_SYS_TIME, (u32)&time);
    dev_close(fd);
    uint32_t utime = timestamp_mytime_2_utc_sec(&time) - CST_OFFSET_TIME;
    MY_DEBUG("upay_get_timestamp: %d,%d,%d,%d,%d,%d : %d,0x%x \n", time.year, time.month, time.day, time.hour, time.min, time.sec, utime, utime);
    return utime;
}


//这里是  杰理 buff满的缓存机制，可以有的可以忽略

struct upay_ble_list {
    struct list_head entry;
    u16 len;
    u8 data[0];
};


static struct list_head upay_le_hd = LIST_HEAD_INIT(upay_le_hd);
//接口不可以跨线程,链表没有做保护
void upay_list_add(u8 *data, u16 len)
{
    struct upay_ble_list *b_list = zalloc(sizeof(struct upay_ble_list) + len);
    ASSERT(b_list, "bt list malloc err");
    memcpy(b_list->data, data, len);
    b_list->len = len;
    list_add_tail(&b_list->entry, &upay_le_hd);
}



static u16 send_resume_t = 0;


//接口不可以跨线程,链表没有做保护
int upay_list_resume()
{
    struct upay_ble_list *b_list;

    if (send_resume_t) {
        send_resume_t = 0;
    }
__agin:

    if (list_empty(&upay_le_hd)) {
        return 0;
    }

    int ret = 0;
    b_list = list_first_entry(&upay_le_hd, struct upay_ble_list, entry);
    ret  = upay_ble_send_data(b_list->data, b_list->len);
    if (!ret) {
        list_del(&b_list->entry);
        free(b_list);
        goto  __agin;
    }

    if (ret == BLE_BUFFER_FULL) {
        printf("err in %s %d\n", __FUNCTION__, __LINE__);
        send_resume_t = sys_timeout_add(NULL, upay_list_resume, 500);
        return -1;
    }

    printf("err in %s %d\n", __FUNCTION__, __LINE__);
    struct upay_ble_list *p, *n;
    list_for_each_entry_safe(p, n, &upay_le_hd, entry) {
        list_del(&p->entry);
        free(p);
    }
    return -2;

}




//接口不可以跨线程,链表没有做保护
int upay_list_clean()
{
    if (list_empty(&upay_le_hd)) {
        return 0;
    }
    struct upay_ble_list *p, *n;
    list_for_each_entry_safe(p, n, &upay_le_hd, entry) {
        list_del(&p->entry);
        free(p);
    }

    return 0;
}

//以上是  杰理 buff满的缓存机制，可以有的可以忽略



/*BLE写接口:
 * @param [in] data 待发送数据的指针
 * @param [in] len  待发送数据的大小
 * @return void
 */
void upay_ble_write(uint8_t *data, uint16_t len)
{
    //里面有一个防止掉包的重发，用户可以自己实现
    if (send_resume_t) {
        sys_timeout_del(send_resume_t);
        send_resume_t = 0;
    }
    int err = upay_list_resume();//激活剩余数据
    if (!err) {
        err = upay_ble_send_data(data, len);
        if (!err) {
            return;
        }
    }
    upay_list_add(data, len); //发送失败加入链表
    send_resume_t = sys_timeout_add(NULL, upay_list_resume, 500); //激活

}





/////////////////////////////////////////////
////获取设备ID(以冒号分割的16进制mac地址)
////要求内容以‘\0’结尾且长度不包含'\0'
/////////////////////////////////////////////
upay_retval_t upay_get_deviceid(uint8_t *buf_did, uint32_t *len_did)
{
    // 需要厂商实现
    u8 mac[6];
    le_controller_get_mac(mac);
    bt_addr2string(mac, buf_did);
    *len_did = strlen(buf_did);
    MY_DEBUG("get mac:%d, %s \n", *len_did, buf_did);
    return UPAY_RV_OK;
}

/////////////////////////////////////////////
////获取设备SN(厂商印刷在卡片上的设备序列号)
////长度不超过32个字符，只能包含大小写字母、数字、下划线
////要求内容以‘\0’结尾且长度不包含'\0'
////仅卡片类产品且有SN在小程序显示需求的厂商实现
////其他厂商请输出buf_sn=""(空字符串)，len_sn=0
/////////////////////////////////////////////
upay_retval_t upay_get_sn(uint8_t *buf_sn, uint32_t *len_sn)
{
    // 需要厂商实现
    extern u32 get_product_serial_num(u8 * buf, u16 len);
    u32 rlen = get_product_serial_num(buf_sn, 31);
    buf_sn[rlen] = '\0';
    *len_sn = rlen;
    MY_DEBUG("get sn:%d, %s \n", *len_sn, buf_sn);
    return UPAY_RV_OK;
}

/////////////////////////////////////////////////////////////////////////////////////
// 由于有些设备上不支持输出动态string, 所以没有定义成变长参数，
// 只会输出一些关键的trace log, 便于发现问题时快速定位
// 注: format参数中可能含有静态string, 实现时不可忽略
/////////////////////////////////////////////////////////////////////////////////////
void upay_log(UPAY_log_level level, const char *format, int32_t value)
{
    // 需要厂商实现
    printf(format, value);
}
void upay_log_ext(const char *format, ...)
{
    return ; // 有打印%s,但传的地址非法导致异常

    // 需要厂商实现
    va_list args;
    va_start(args, format);
    print(NULL, 0, format, args);

    /* puts("\n\n>>>>>>>>> upaytest >>>>>>>>\n\n"); */
    /* puts(format); */
    /* putchar('\n'); */
    /* putchar('\n'); */
}

void upay_get_productModel(uint8_t *buffer, uint32_t *len)
{
    *len = strlen(TCFG_PAY_ALIOS_PRODUCT_MODEL);
    strcpy(buffer, TCFG_PAY_ALIOS_PRODUCT_MODEL);
}

/* int upay_get_imei(uint8_t *buf_imei, uint32_t *len_imei) */
/* { */
/*     return 0; */
/* } */

/*获取公司名称，公司名称为字符串格式并且字母要求全部大写，长度要求不超过15个字节(不包含结束符)，
 *如果公司全称不超过15个字节，则使用全称比如 "HUAWEI" "GARMIN"；如果超过15个字节，请优先使用中文拼音
 *则第一个字的拼音为完整，后续为拼音首字母的缩写，比如 xiaotiancai应写为 "XIAOTC"；若一定要使用英文
 *的公司名称，请与alipay同学协商。
 * @param [in] buffer  存放公司名称的缓存
 * @param [in out] len 输入为允许读取的最大长度；输出时为实际读取的长度，长度不包含结束符
 * @return void
 */
void upay_get_companyName(uint8_t *buffer, uint32_t *len)
{
    //TODO:需要厂商实现
    *len = strlen(TCFG_PAY_ALIOS_COMPANY_NAME);
    strcpy(buffer, TCFG_PAY_ALIOS_COMPANY_NAME);
}

/*获取编译时间，返回UTC时间戳，单位为s，用于辅助版本校验*/
uint32_t upay_get_compile_timestamp()
{
    return upay_get_timestamp();
}

int upay_mac_deal(u8 *edr_addr, u8 *ble_addr)
{
#if UPAY_DATA_BUF_TEST
#else
    struct aliyun_mac mac = {0};
    u8 mac_buf_tmp[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    aliyun_storage_read(AOS_MAC_OFFSET, &mac, sizeof(struct aliyun_mac));
    /* put_buf(&mac, sizeof(struct aliyun_mac)); */
    /* printf("flag:0x%x, crc:0x%x, 0x%x \n", mac.flag, mac.crc, CRC16(mac.mac_addr, 6*2)); */
    if ((mac.flag == ALIYUN_MAC_FLAG) && (mac.crc == CRC16(mac.mac_addr, 6 * 2))) {
        if (edr_addr && (memcmp(mac.mac_addr, mac_buf_tmp, 6))) {
            memcpy(edr_addr, mac.mac_addr, 6);
        }
        if (ble_addr && (memcmp(mac.ble_addr, mac_buf_tmp, 6))) {
            memcpy(ble_addr, mac.ble_addr, 6);
        }
        return true;
    } else {
        if (edr_addr) {
            memcpy(mac.mac_addr, edr_addr, 6);
        } else {
            memcpy(mac.mac_addr, mac_buf_tmp, 6);
        }
        if (ble_addr) {
            memcpy(mac.ble_addr, ble_addr, 6);
        } else {
            memcpy(mac.ble_addr, mac_buf_tmp, 6);
        }
        mac.flag = ALIYUN_MAC_FLAG;
        mac.crc = CRC16(mac.mac_addr, 6 * 2);
        aliyun_storage_write(AOS_MAC_OFFSET, &mac, sizeof(struct aliyun_mac));
    }
#endif /* #if UPAY_DATA_BUF_TEST */
    return false;

}




static int __write_upay_data(void *priv)
{
#if UPAY_DATA_BUF_TEST
#else
    if ((int)priv == (int)SYSCFG_WRITE_ERASE_STATUS) { // 恢复出厂
        aliyun_storage_earse(AOS_HAL_KV_NV_OFFSET, AOS_HAL_KV_NV_LEN);
        /* aliyun_storage_earse(AOS_SECR_OFFSET, AOS_HAL_SECRET_LEN); */
    } else { // 关机、重启
        aos_hal_kv_nv_flush();
    }
#endif
    return 0;
}

REGISTER_WATCH_SYSCFG(upay_ops) = {
    .name = "upay",
    .read = NULL,
    .write = __write_upay_data,
};

void clear_upay_data(void)
{
    g_printf("HJY %s[line:%d]\n", __func__, __LINE__);
    aliyun_storage_earse(AOS_HAL_KV_NV_OFFSET, AOS_HAL_KV_NV_LEN);
    aliyun_storage_earse(AOS_SECR_OFFSET, AOS_HAL_SECRET_LEN);
}


#endif /* #if TCFG_PAY_ALIOS_ENABLE && (TCFG_PAY_ALIOS_WAY_SEL==TCFG_PAY_ALIOS_WAY_ALIYUN) */

