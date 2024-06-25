#ifndef __PRODUCT_TEST_H
#define __PRODUCT_TEST_H

#include "system/includes.h"
#include "app_config.h"

// 模块使能
#ifndef PRODUCT_TEST_ENABLE
#define PRODUCT_TEST_ENABLE			0
#endif

#ifndef PT_GPIO_ENABLE
#define PT_GPIO_ENABLE				0
#endif
#ifndef PT_GSENSOR_ENABLE
#define PT_GSENSOR_ENABLE			0
#endif
#ifndef PT_MOTOR_ENABLE
#define PT_MOTOR_ENABLE				0
#endif
#ifndef PT_SD_ENABLE
#define PT_SD_ENABLE				0
#endif
#ifndef PT_HR_ENABLE
#define PT_HR_ENABLE				0
#endif
#ifndef PT_MR_ENABLE
#define PT_MR_ENABLE				0
#endif
#ifndef PT_PRESSURE_ENABLE
#define PT_PRESSURE_ENABLE			0
#endif
#ifndef PT_GPS_ENABLE
#define PT_GPS_ENABLE				0
#endif
#ifndef PT_SPEAKER_MIC_ENABLE
#define PT_SPEAKER_MIC_ENABLE		0
#endif
#ifndef PT_LCD_TP_ENABLE
#define PT_LCD_TP_ENABLE			0
#endif

#ifndef PT_GPIO_CHECK_LCD_TP
#define PT_GPIO_CHECK_LCD_TP		0
#endif


// 模块（module/MOD）
#define PT_ORDER_M_SET(x)			(u32)((x)<<0)
#define PT_ORDER_M_GET(x)			(((u32)(x)>>0) & 0x000000FF)

// 命令（command/CMD）
#define PT_ORDER_C_SET(x)			(u32)((x)<<8)
#define PT_ORDER_C_GET(x)			(((u32)(x)>>8) & 0x000000FF)


// 模块（module/MOD）
#define PT_M_SYSTEM					0x00 // 系统命令
#define PT_M_GPIO					0x01 // GPIO
#define PT_M_POWER					0x02 // 功耗
#define PT_M_CHARGE					0x03 // 充电电流
#define PT_M_FLASH					0x04 // 内置flash
#define PT_M_PSRAM					0x05 // 内置psram
#define PT_M_EDR					0x10 // 蓝牙EDR
#define PT_M_BLE					0x11 // 蓝牙BLE
#define PT_M_EX_FLASH				0x40 // 外置flash
#define PT_M_EX_PSRAM				0x41 // 外置PSRAM
#define PT_M_SD						0x42 // SD卡
#define PT_M_SPI_NAND				0x43 // spi nand
#define PT_M_SPEAKER				0x44 // 喇叭
#define PT_M_MIC					0x45 // 麦克风
#define PT_M_LCD					0x46 // 显示屏
#define PT_M_TP						0x47 // 触摸屏
#define PT_M_MOTOR					0x48 // 马达
#define PT_M_GSENSOR				0x49 // GSENSOR
#define PT_M_HR						0x4A // 心率血氧
#define PT_M_MR						0x4B // 地磁传感器
#define PT_M_PRESSURE				0x4C // 气压传感器
#define PT_M_GPS					0x4D // GPS

#define PT_M_SPEAKER_MIC			0x60 // 喇叭+麦克风
#define PT_M_LCD_TP					0x61 // 显示屏+触摸屏

#define PT_M_USER					0x80 // 用户自定义扩展。0x80~0xBF

// 属性（attribute/ATTR）
#define PT_ATTR_SELF				BIT(0) // 自检（自动判别）。如果有其他属性值，即为有条件自检。例如喇叭+麦克风需要接外设才能完成自检
#define PT_ATTR_USER_JUDGE			BIT(1) // 需要人工判别。例如马达主动等需要人工判别
#define PT_ATTR_INTERACTION			BIT(2) // 需要人工交互。例如LCD+TP需要人工触摸
#define PT_ATTR_EXTRA_DEV			BIT(3) // 需要外设。例如PCBA上没有接麦克风，需要外接麦克风

#define PT_ATTR_ENV_OUT				BIT(24) // 对外界有影响。例如马达会对外主动，喇叭会对外发声。有这种属性的模块应该轮询检测
#define PT_ATTR_ENV_IN				BIT(25) // 受外界影响。例如麦克风需要采集环境音


// 标准命令（normal_command/NORM_CMD）
#define PT_N_C_GETINFO				0x01 // 获取信息（工具->固件）
#define PT_N_C_SETINFO				0x02 // 配置信息（工具->固件）
#define PT_N_C_START				0x03 // 启动测试（工具->固件）
#define PT_N_C_STOP					0x04 // 停止测试（工具->固件）
#define PT_N_C_PULL_DATA			0x05 // 下行数据（工具->固件）

#define PT_N_C_GET_RESULT			0x10 // 获取测试结果（工具->固件）
#define PT_N_C_SET_RESULT			0x11 // 设置测试结果（工具->固件）

#define PT_N_C_PRIV					0x20 // 模块私有命令。0x20~0x7F

#define PT_N_C_USER					0x80 // 用户自定义扩展。0x80~0xBF

// 系统命令（system_command/SYS_CMD）
#define PT_S_C_GET_VER				0x21 // 获取版本消息（工具->固件）
#define PT_S_C_SET_INFO				0x22 // 设置参数（工具->固件）
#define PT_S_C_GET_MODULE			0x23 // 获取模块列表（工具->固件）
#define PT_S_C_HOLD					0x70 // 保持连接状态（固件->工具）
#define PT_S_C_END					0x7F // 结束（工具->固件）


// 错误值（error/ERR）
#define PT_E_OK						0x00 // 成功
#define PT_E_SYS_UNREADY			0x01 // 系统还没准备好
#define PT_E_PARAM					0x02 // 参数错误
#define PT_E_TIMEOUT				0x03 // 超时
#define PT_E_NO_BUFF				0x04 // 申请不到buff
#define PT_E_OVER_ADDR				0x05 // 地址超范围
#define PT_E_OVER_LEN				0x06 // 大小超范围
#define PT_E_DATA_FORMAT			0x07 // 数据格式错误
#define PT_E_NO_DEV					0x08 // 没有设备
#define PT_E_NO_FILE				0x09 // 没有文件
#define PT_E_DEV_ID					0x0A // 设备ID错误
#define PT_E_SYS_QUEUE_FULL			0x0B // 消息池已满
#define PT_E_SYS_ERROR				0x0C // 系统出错
#define PT_E_MOD_NULL				0x41 // 模块不存在
#define PT_E_MOD_ERROR				0x42 // 模块存在但有异常
#define PT_E_MOD_UNREADY			0x43 // 模块还未准备好
#define PT_E_MOD_CANT_STOP			0x44 // 模块无法结束
#define PT_E_MOD_STOP_NO_END		0x45 // 模块没有检测完被强制结束
#define PT_E_MOD_TEST_ERROR			0x46 // 模块测试失败
#define PT_E_MOD_RUN				0x47 // 模块正在运行

// 包标识（pkt_log）
#define PT_PKT_ONCE					0x00 // 单次包
#define PT_PKT_CONTINUE				0x01 // 连续包
#define PT_PKT_CONTINUE_END			0x02 // 连续包结束

//--------------传输包结构体------------
struct pt_pkt {
    u32 order;
    u16 res : 14;
    u16 pkt_log : 2;
    u16 data_len;
    u8  dat[0];
};

//--------------模块结构体------------
struct pt_module {
    u8  module;	// 模块
    u16 attr;	// 属性
    int (*init)(void); // 初始化接口。获取ID等。如果初始化不过，后面对模块的所有操作都返回错误
    int (*ioctrl)(u32 order, int len, void *param); // 命令处理接口。每一条“工具->固件”的命令都需要回复
};

#define REGISTER_PT_MODULE(target) \
        const struct pt_module target sec(.pt_module)

extern const struct pt_module pt_module_begin[];
extern const struct pt_module pt_module_end[];

#define list_for_each_pt_module(p) \
    for (p = pt_module_begin; p < pt_module_end; p++)

#define GET_PT_MODULE_NUM()		(((int)pt_module_end - (int)pt_module_begin) / sizeof(struct pt_module))


// 推送数据
int product_test_push_data(u32 order, int len, u8 *dat);

// open
int product_test_open(void);

// 检测是否开启产测
int product_test_check_run(void);

#endif // #define __PRODUCT_TEST_H

