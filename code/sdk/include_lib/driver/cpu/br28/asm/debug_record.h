#ifndef __DEBUG_RECORD_H__
#define __DEBUG_RECORD_H__

//==================================================================================//
// 异常信息以:
// [exception_head]
// [item0_head][item0_payload]
// [item1_head][item1_payload]
// ......
// [itemN_head][itemN_payload]
// 格式记录.
//==================================================================================//
#define DEBUG_MAGIC 			(('D' << 24) | ('L' << 16) | ('O' << 8) | ('G' << 0))

enum DEBUG_ITEM_LIST {
    //系统保留:
    DEBUG_ITEM_TYPE_P11 = 0,
    DEBUG_ITEM_TYPE_DRIVER,
    DEBUG_ITEM_TYPE_SYSTEM,
    DEBUG_ITEM_TYPE_TASK,
    DEBUG_ITEM_TYPE_INSTRUCTION_RET,
    DEBUG_ITEM_TYPE_STACK,

    //用户自定义:
    DEBUG_ITEM_TYPE_USRER = 0x80,
};

struct debug_record_head {
    u32 magic;				//固定为DEBUG_MAGIC
    u16 crc; 			    //所有子项数据的crc16
    u16 len; 				//所有子项item的总长度, 不包含本结构体本身
    u32 run_time; 		    //系统运行时间
    struct sys_time time;   //rtc时间
    u32 data[0];
};

struct debug_item_head {
    u16 len; 					//子项的长度
    u8 type; 					//子项的类型, enum DEBUG_ITEM_LIST
    u32 data[0];
};


struct debug_record_handle {
    u8 type; 						//子项声明类型, enum DEBUG_ITEM_LIST
    char *name; 					//子项名称
    u16(*debug_record_info_get)(void *buf); 	//获取子项数据
    void (*debug_record_info_put)(void *buf); 	//输出子项数据
};
#define REGISTER_DEBUG_RECORD_HANDLE(record) \
	const struct debug_record_handle record SEC_USED(.debug_record_handle_ops)

extern const int config_debug_exception_record;

extern const int config_debug_exception_record_dump_info;

#define DEBUG_RECORD_ENABLE_CHECK_BEGIN(); \
	if (config_debug_exception_record) {

#define DEBUG_RECORD_ENABLE_CHECK_END(); \
	}

#define DEBUG_RECORD_ENABLE_CHECK_END_RET(); \
	} else { \
		return 0; \
	}

#define DEBUG_RECORD_DUMP_INFO_CHECK_BEGIN(); \
	if (config_debug_exception_record_dump_info) {

#define DEBUG_RECORD_DUMP_INFO_CHECK_END(); \
	}

#define DEBUG_RECORD_MODULE_ENABLE_CHECK_BEGIN(a); \
	if (a) {

#define DEBUG_RECORD_MODULE_ENABLE_CHECK_END(a); \
	}

#define DEBUG_RECORD_MODULE_ENABLE_CHECK_END_RET(a); \
	} else { \
		return 0; \
	}



/* ---------------------------------------------------------------------------- */
/**
 * @brief 系统异常时调用该接口记录异常信息
 */
/* ---------------------------------------------------------------------------- */
void debug_exception_record(void);

/* ---------------------------------------------------------------------------- */
/**
 * @brief 擦除Flash DEBUG区域
 */
/* ---------------------------------------------------------------------------- */
void debug_record_flash_zone_erase(void);


/* ---------------------------------------------------------------------------- */
/**
 * @brief 用户获取异常记录信息
 *
 * @param info: struct debug_record_info类型, 函数执行结果如下:
 *				1) 系统存在异常信息, record_len为异常信息长度, 单位为byte;
 *				2) 系统不存在异常信息, record_len为0;
 */
/* ---------------------------------------------------------------------------- */
struct debug_record_info  {
    u32 record_len; //异常记录数据长度
    u8 *record_buf; 		//返回异常数据记录地址, 该buf只读;
};
void user_debug_record_info_get(struct debug_record_info *info);


/* ---------------------------------------------------------------------------- */
/**
 * @brief 清除log信息
 */
/* ---------------------------------------------------------------------------- */
void user_debug_record_info_clear(void);

#endif /* #ifndef __DEBUG_RECORD_H__ */
