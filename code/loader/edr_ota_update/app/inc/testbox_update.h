#ifndef __TESTBOX_UPDATE_H__
#define __TESTBOX_UPDATE_H__

enum {
    UPDATA_STOP = 0,
    UPDATA_OPEN,
    UPDATA_SEEK,
    UPDATA_READ,
    UPDATA_READ_OFFSIZE,
    UPDATA_STOP_KEYERR,
    UPDATA_ACCEPT_OK,
    UPDATA_WAIT_REBOOT,
};

/* updata status */
typedef enum {
#ifdef EDR_UPDATA_SUPPORT_CONNECT
    UPDATA_START = 0x00,
#else
    UPDATA_START = 0x01,
#endif
    UPDATA_REV_DATA,
} UPDATA_BIT_FLAG;

enum {
    BT_SEEK_SET = 0x01,
    BT_SEEK_CUR = 0x02,

    BT_SEEK_TYPE_UPDATE_LEN = 0x10,
};

typedef struct _testbox_update_info {
    u8  seek_type;
    u8  bt_reinit;
    volatile u8    state;
    volatile u32 file_offset;       //文件偏移
    volatile u32 bt_time_timeout;
    u8  *read_buf;
    u32  data_len;					//实际读取到的长度
    u32  need_rx_len;
} testbox_update_info;

#define UPDATE_CMD_WAIT_TIMEOUT (400UL) //unit:10ms 等待测试盒回复超时时间

#endif
