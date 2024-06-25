#ifndef __PSRAM_API_H__
#define __PSRAM_API_H__

#include "asm/clock.h"
#include "asm/clock_hw.h"

enum PSRAM_PORT_SEL_TABLE {
    PSRAM_PORT_SEL_PORTA = 0,
};

enum PSRAM_MODE_SEL_TABLE {
    PSRAM_MODE_1_WIRE = 0,
    PSRAM_MODE_4_WIRE_CMD1_ADR4_DAT4,
    PSRAM_MODE_4_WIRE_CMD4_ADR4_DAT4,
};


struct psram_platform_data {
    u8 power_port;
    enum PSRAM_PORT_SEL_TABLE port;
    enum PSRAM_MODE_SEL_TABLE mode;
    u32 init_clk;
};

#define PSRAM_PLATFORM_DATA_BEGIN(data) \
		static const struct psram_platform_data data = {

#define PSRAM_PLATFORM_DATA_END()  \
};


/* ---------------------------------------------------------------------------- */
/**
 * @brief psram硬件初始化函数
 *
 * @param config: psram配置参数
 */
/* ---------------------------------------------------------------------------- */
void psram_hw_init(const struct psram_platform_data *config);


/* ---------------------------------------------------------------------------- */
/**
 * @brief psram进入低功耗模式
 */
/* ---------------------------------------------------------------------------- */
void psram_hw_enter_sleep(void);

/* ---------------------------------------------------------------------------- */
/**
 * @brief psram退出standby模式
 */
/* ---------------------------------------------------------------------------- */
void psram_exit_sleep(void);

/* ---------------------------------------------------------------------------- */
/**
 * @brief: psram NO cache 地址转换为 cache地址
 *
 * @param addr:不经cache访问psram地址(0x3000000 ~ 0x3FFFFFF)
 *
 * @return: 经cache访问psram的地址(0x2000000 ~ 0x2FFFFFF)
 */
/* ---------------------------------------------------------------------------- */
u32 psram_nocache2cache_addr(u32 addr);

/* ---------------------------------------------------------------------------- */
/**
 * @brief: psram cache 地址转换为 NO cache地址
 *
 * @param addr:经cache访问psram地址(0x2000000 ~ 0x2FFFFFF)
 *
 * @return: 不经cache访问psram的地址(0x3000000 ~ 0x3FFFFFF)
 */
/* ---------------------------------------------------------------------------- */
u32 psram_cache2nocache_addr(u32 addr);

/* ---------------------------------------------------------------------------- */
/**
 * @brief: psram 内存申请接口
 *
 * @param size: 申请空间大小, 单位byte;
 *
 * @return: 成功: 申请内存地址, 出错: NULL
 */
/* ---------------------------------------------------------------------------- */
extern void *malloc_psram(size_t size);


/* ---------------------------------------------------------------------------- */
/**
 * @brief: psram 内存释放接口
 *
 * @param pv: 释放的内存地址
 */
/* ---------------------------------------------------------------------------- */
extern void free_psram(void *pv);

/* ---------------------------------------------------------------------------- */
/**
 * @brief: psram内存统计接口, 调用可获取信息:
 * 1. Free Block: 空闲块数量;
 * 2. MaxBlockSize: 最大空闲块大小;
 * 3. MinBlockSize: 最小空闲块大小;
 * 4. TotalFree: 空闲块总大小, EverMinFree: 系统运行过程中曾经最小值
 *
 */
/* ---------------------------------------------------------------------------- */
extern void psram_heap_stats(void);

/* ---------------------------------------------------------------------------- */
/**
 * @brief: psram内存统计接口, 调用可获取信息:
 * 1. Free Block: 空闲块数量;
 * 2. MaxBlockSize: 最大空闲块大小;
 * 3. MinBlockSize: 最小空闲块大小;
 * 4. TotalFree: 空闲块总大小, EverMinFree: 系统运行过程中曾经最小值;
 * 5. EveryBlock: 每一块空闲块, 起始地址, 结束地址, 空闲块大小;
 *
 */
/* ---------------------------------------------------------------------------- */
extern void psram_heap_dump(void);



/* ---------------------------------------------------------------------------- */
/**
 * @brief 获取psram在使用的buf数量
 *
 * @return psram在使用的buf数量
 */
/* ---------------------------------------------------------------------------- */
extern u32 psram_get_used_block(void);

/* ---------------------------------------------------------------------------- */
/**
 * @brief 刷新cache数据缓存到psram, 并清除cache的缓存数据
 *
 * @param begin: 经cache访问psram地址(0x2000000 ~ 0x2FFFFFF), 要求4字节对齐
 * @param len: 刷新长度
 */
/* ---------------------------------------------------------------------------- */
extern void psram_flush_invaild_cache(u32 *begin, u32 len);


/* ---------------------------------------------------------------------------- */
/**
 * @brief 刷新cache所有数据缓存到psram, 并清除cache的缓存数据
 */
/* ---------------------------------------------------------------------------- */
extern void psram_flush_invaild_all_cache(void);

/* ---------------------------------------------------------------------------- */
/**
 * @brief 刷新cache数据缓存到psram, 不清除cache的缓存数据
 *
 * @param begin: 经cache访问psram地址(0x2000000 ~ 0x2FFFFFF), 要求4字节对齐
 * @param len: 刷新长度
 */
/* ---------------------------------------------------------------------------- */
extern void psram_flush_cache(u32 *begin, u32 len);


/* ---------------------------------------------------------------------------- */
/**
 * @brief 刷新cache所有数据缓存到psram, 不清除cache的缓存数据
 */
/* ---------------------------------------------------------------------------- */
extern void psram_flush_all_cache(void);


/* ---------------------------------------------------------------------------- */
/**
 * @brief 注册powerdown流程处理接口
 *
 * @param handle
 */
/* ---------------------------------------------------------------------------- */
extern void psram_heap_register_powerdown_handle(void (*handle)(void));

/* ---------------------------------------------------------------------------- */
/**
 * @brief 注册psram刷新cache接口
 *
 * @param handle
 */
/* ---------------------------------------------------------------------------- */
extern void psram_heap_register_flush_dcu_handle(void (*handle)(void *Pv, size_t xLength));

/* ---------------------------------------------------------------------------- */
/**
 * @brief psram heap reset
 */
/* ---------------------------------------------------------------------------- */
extern void psram_heap_reset(void);

#endif /* #ifndef __PSRAM_API_H__ */
