
# make 编译并下载
# make VERBOSE=1 显示编译详细过程
# make clean 清除编译临时文件
#
# 注意： Linux 下编译方式：
#     1. 从 http://pkgman.jieliapp.com/doc/all 处找到下载链接
#     2. 下载后，解压到 /opt/jieli 目录下，保证
#       /opt/jieli/common/bin/clang 存在（注意目录层次）
#     3. 确认 ulimit -n 的结果足够大（建议大于8096），否则链接可能会因为打开文件太多而失败
#       可以通过 ulimit -n 8096 来设置一个较大的值
#

# 工具路径设置
ifeq ($(OS), Windows_NT)
# Windows 下工具链位置
TOOL_DIR := C:/JL/pi32/bin
CC    := clang.exe
CXX   := clang.exe
LD    := pi32v2-lto-wrapper.exe
AR    := llvm-ar.exe
MKDIR := mkdir_win -p
RM    := rm -rf

SYS_LIB_DIR := C:/JL/pi32/pi32v2-lib/r3-large
SYS_INC_DIR := C:/JL/pi32/pi32v2-include
EXT_CFLAGS  := # Windows 下不需要 -D__SHELL__
export PATH:=$(TOOL_DIR);$(PATH)

## 后处理脚本
FIXBAT          := tools\utils\fixbat.exe # 用于处理 utf8->gbk 编码问题
POST_SCRIPT     := cpu\br28\tools\download.bat
RUN_POST_SCRIPT := $(POST_SCRIPT)
else
# Linux 下工具链位置
TOOL_DIR := /opt/jieli/pi32v2/bin
CC    := clang
CXX   := clang
LD    := lto-wrapper
AR    := lto-ar
MKDIR := mkdir -p
RM    := rm -rf
export OBJDUMP := $(TOOL_DIR)/objdump
export OBJCOPY := $(TOOL_DIR)/objcopy
export OBJSIZEDUMP := $(TOOL_DIR)/objsizedump

SYS_LIB_DIR := $(TOOL_DIR)/../lib/r3-large
SYS_INC_DIR := $(TOOL_DIR)/../include
EXT_CFLAGS  := -D__SHELL__ # Linux 下需要这个保证正确处理 download.c
export PATH:=$(TOOL_DIR):$(PATH)

## 后处理脚本
FIXBAT          := touch # Linux下不需要处理 bat 编码问题
POST_SCRIPT     := cpu/br28/tools/download.sh
RUN_POST_SCRIPT := bash $(POST_SCRIPT)
endif

CC  := $(TOOL_DIR)/$(CC)
CXX := $(TOOL_DIR)/$(CXX)
LD  := $(TOOL_DIR)/$(LD)
AR  := $(TOOL_DIR)/$(AR)
# 输出文件设置
OUT_ELF   := cpu/br28/tools/sdk.elf
OBJ_FILE  := $(OUT_ELF).objs.txt
# 编译路径设置
BUILD_DIR := objs

# 编译参数设置
CFLAGS := \
	-target pi32v2 \
	-mcpu=r3 \
	-integrated-as \
	-flto \
	-Wuninitialized \
	-Wno-invalid-noreturn \
	-fno-common \
	-mllvm -inline-threshold=5 \
	-Oz \
	-g \
	-flto \
	-fallow-pointer-null \
	-fprefer-gnu-section \
	-Wno-shift-negative-value \
	-Wundef \
	-Wframe-larger-than=256 \
	-Wincompatible-pointer-types \
	-Wreturn-type \
	-Wimplicit-function-declaration \
	-mllvm -pi32v2-large-program=true \
	-fms-extensions \
	-fdiscrete-bitfield-abi \
	-w \

# C++额外的编译参数 
# -Wl,--no-relax 
# -mcmodel=large 
# -fno-common
# -mlong-calls
CXXFLAGS :=


# 宏定义
DEFINES := \
	-DSUPPORT_MS_EXTENSIONS \
	-DCONFIG_RELEASE_ENABLE \
	-DCONFIG_CPU_BR28 \
	-DCONFIG_PRINT_FLOAT_ENABLE \
	-DCONFIG_NEW_BREDR_ENABLE \
	-DCONFIG_NEW_MODEM_ENABLE \
	-DCONFIG_NEW_TWS_FORWARD_ENABLE \
	-DCONFIG_UCOS_ENABLE \
	-DCONFIG_EQ_SUPPORT_ASYNC \
	-DEQ_CORE_V1 \
	-DCONFIG_FAT_MAX_DEEPTH=3 \
	-DCONFIG_AAC_CODEC_FFT_USE_MUTEX \
	-DCONFIG_DNS_ENABLE \
	-DCONFIG_DMS_MALLOC \
	-DCONFIG_MMU_ENABLE \
	-DCONFIG_SBC_CODEC_HW \
	-DCONFIG_MSBC_CODEC_HW \
	-DCONFIG_AEC_M=256 \
	-DCONFIG_AUDIO_ONCHIP \
	-DCONFIG_MEDIA_DEVELOP_ENABLE \
	-DCONFIG_MIXER_CYCLIC \
	-DCONFIG_SOUND_PLATFORM_ENABLE=1 \
	-DCONFIG_SUPPORT_EX_TWS_ADJUST \
	-D__GCC_PI32V2__ \
	-DCONFIG_NEW_ECC_ENABLE \
	-DCONFIG_MAC_ADDR_GET_USED_SFC \
	-DSUPPORT_BLUETOOTH_PROFILE_RELEASE \
	-DCONFIG_SOUNDBOX \
	-DAPP_USE_SOFT_SPI_ACCESS_VM \
	-DEVENT_HANDLER_NUM_CONFIG=2 \
	-DEVENT_TOUCH_ENABLE_CONFIG=0 \
	-DEVENT_POOL_SIZE_CONFIG=256 \
	-DCONFIG_EVENT_KEY_MAP_ENABLE=0 \
	-DTIMER_POOL_NUM_CONFIG=15 \
	-DAPP_ASYNC_POOL_NUM_CONFIG=0 \
	-DVFS_ENABLE=1 \
	-DUSE_SDFILE_NEW=1 \
	-DSDFILE_STORAGE=1 \
	-DVFS_FILE_POOL_NUM_CONFIG=1 \
	-DFS_VERSION=0x020001 \
	-DFATFS_VERSION=0x020101 \
	-DSDFILE_VERSION=0x020000 \
	-DVM_MAX_PAGE_ALIGN_SIZE_CONFIG=64*1024 \
	-DVM_MAX_SECTOR_ALIGN_SIZE_CONFIG=64*1024 \
	-DVM_ITEM_MAX_NUM=256 \
	-DCONFIG_TWS_ENABLE \
	-DCONFIG_WATCH_CASE_ENABLE=1 \
	-DCONFIG_OS_AFFINITY_ENABLE \
	-DCONFIG_LITE_AEC_ENABLE=0 \
	-DAUDIO_REC_POOL_NUM=1 \
	-DAUDIO_DEC_POOL_NUM=3 \
	-DAUDIO_LINEIN_ENABLE \
	-DCONFIG_LMP_CONN_SUSPEND_ENABLE \
	-DCONFIG_BTCTRLER_TASK_DEL_ENABLE \
	-DCONFIG_LINK_DISTURB_SCAN_ENABLE=0 \
	-DCONFIG_UPDATA_ENABLE \
	-DCONFIG_OTA_UPDATA_ENABLE \
	-DCONFIG_EFFECT_CORE_V2_ENABLE \
	-DCONFIG_ITEM_FORMAT_VM \
	-DLV_LVGL_H_INCLUDE_SIMPLE \
	-DLV_EX_CONF_INCLUDE_SIMPLE \
	-DLV_DEMO_CONF_INCLUDE_SIMPLE \


DEFINES += $(EXT_CFLAGS) # 额外的一些定义

# 头文件搜索路径
INCLUDES := \
	-Iinclude_lib \
	-Iinclude_lib/driver \
	-Iinclude_lib/driver/device \
	-Iinclude_lib/driver/cpu/br28 \
	-Iinclude_lib/system \
	-Iinclude_lib/system/generic \
	-Iinclude_lib/system/device \
	-Iinclude_lib/system/fs \
	-Iinclude_lib/system/ui_new \
	-Iinclude_lib/btctrler \
	-Iinclude_lib/btctrler/port/br28 \
	-Iinclude_lib/update \
	-Iinclude_lib/agreement \
	-Iinclude_lib/btstack/third_party/common \
	-Iinclude_lib/btstack/third_party/rcsp \
	-Iinclude_lib/media/media_develop \
	-Iinclude_lib/media/media_develop/media \
	-Iinclude_lib/media/media_develop/media/cpu/br28 \
	-Iinclude_lib/media/media_develop/media/cpu/br28/asm \
	-Iapps/watch/include \
	-Iapps/watch/include/task_manager \
	-Iapps/watch/include/task_manager/bt \
	-Iapps/watch/include/user_api \
	-Iapps/watch/include/sport_data \
	-Iapps/common \
	-Iapps/common/device \
	-Iapps/common/audio \
	-Iapps/common/audio/decode \
	-Iapps/common/audio/encode \
	-Iapps/common/audio/stream \
	-Iapps/common/power_manage \
	-Iapps/common/device/key \
	-Iapps/common/third_party_profile/common \
	-Iapps/common/third_party_profile/jieli \
	-Iapps/common/third_party_profile/jieli/trans_data_demo \
	-Iapps/common/third_party_profile/jieli/JL_rcsp \
	-Iapps/common/third_party_profile/jieli/JL_rcsp/bt_trans_data \
	-Iapps/common/third_party_profile/dma_deal \
	-Iapps/common/dev_manager \
	-Iapps/common/file_operate \
	-Iapps/common/music \
	-Iapps/common/cJSON \
	-Iapps/common/include \
	-Iapps/common/config/include \
	-Iapps/common/device/imu_sensor \
	-Iapps/common/device/bmp280 \
	-Iapps/common/device/nfc \
	-Iapps/watch/board/br28 \
	-Iapps/watch/ui \
	-Iapps/watch/product_test \
	-Icpu/br28 \
	-Icpu/br28/audio_common \
	-Icpu/br28/audio_dec \
	-Icpu/br28/audio_enc \
	-Icpu/br28/audio_effect \
	-Icpu/br28/audio_mic \
	-Icpu/br28/localtws \
	-Iapps/common/debug \
	-Iapps/common/debug/data_export \
	-Iinclude_lib/media \
	-Iinclude_lib/system/math/cpu/br28 \
	-Iapps/common/device/usb \
	-Iapps/common/device/usb/device \
	-Iapps/common/device/usb/host \
	-Iapps/watch/smartbox \
	-Iapps/watch/smartbox/bt_manage/bt_trans_data \
	-Iapps/watch/smartbox/bt_manage \
	-Iapps/watch/smartbox/smartbox_setting \
	-Iapps/watch/smartbox/smartbox_setting/smartbox_misc_setting \
	-Iapps/watch/smartbox/smartbox_setting_opt \
	-Iapps/watch/smartbox/smartbox_update \
	-Iapps/watch/smartbox/external_flash \
	-Iapps/watch/smartbox/file_transfer \
	-Iapps/watch/smartbox/sensors_data_opt \
	-Iapps/watch/smartbox/sensors_data_opt/sports_data_opt \
	-Iapps/watch/smartbox/sensors_data_opt/sensors_info_opt \
	-Iapps/watch/smartbox/sensors_data_opt/sport_data_info_opt \
	-Iinclude_lib/system/ui_new/ui/cpu/br28 \
	-Icpu/br28/ui_driver \
	-Icpu/br28/audio_way \
	-Icpu/br28/ui_driver/lvgl \
	-Iapps/common/ui/lvgl \
	-Iapps/common/ui/lvgl/src \
	-Iapps/common/ui/lvgl_app/lv_demos \
	-Iapps/common/ui/ui \
	-Iapps/common/ui/ui/jl_images \
	-Iinclude_lib/media/aispeech/asr/include \
	-Iinclude_lib/media/aispeech/enc/include \
	-Iapps/common/pay/upay_aliyun/include \
	-Iapps/common/pay/upay_t_head \
	-Iapps/common/pay/upay_t_head/include \
	-Iapps/common/pay/upay_t_head/soc/port \
	-Iapps/common/pay/upay_t_head/soc/se/JL/hsc32i1 \
	-Iapps/common/pay/upay_t_head/soc/softse \
	-Iapps/common/timestamp \
	-Iapps/common/device/gSensor/silan \
	-Iapps/common/device/gSensor/qmi8658 \
	-Iapps/common/uC-Shell/Cfg/Template \
	-Iapps/common/uC-Shell/Cmd/General \
	-Iapps/common/uC-Shell/Source \
	-Iapps/common/uC-Shell/Terminal/Cfg/Template \
	-Iapps/common/uC-Shell/Terminal/Source \
	-I$(SYS_INC_DIR) \


# 需要编译的 .c 文件
c_SRC_FILES := \
	apps/common/audio/audio_digital_vol.c \
	apps/common/audio/decode/decode.c \
	apps/common/audio/encode/encode_write_file.c \
	apps/common/audio/sine_make.c \
	apps/common/audio/uartPcmSender.c \
	apps/common/bt_common/bt_test_api.c \
	apps/common/cJSON/cJSON.c \
	apps/common/config/app_config.c \
	apps/common/config/bt_profile_config.c \
	apps/common/config/ci_transport_uart.c \
	apps/common/config/new_cfg_tool.c \
	apps/common/debug/data_export/data_export.c \
	apps/common/debug/data_export/data_export_to_file.c \
	apps/common/debug/debug.c \
	apps/common/debug/debug_lite.c \
	apps/common/dev_manager/dev_manager.c \
	apps/common/dev_manager/dev_reg.c \
	apps/common/dev_manager/dev_update.c \
	apps/common/device/sensor_iic/sensor_iic.c \
	apps/common/device/gSensor/qmi8658/qmi8658.c \
	apps/common/device/m_sensor/qmc6309/qmc6309.c \
	apps/common/device/motor/motor.c \
	apps/common/device/hr_sensor/gh3018/Algo_Code/call/src/gh30x_demo_algo_call_adt.c \
	apps/common/device/hr_sensor/gh3018/Algo_Code/call/src/gh30x_demo_algo_call_hr.c \
	apps/common/device/hr_sensor/gh3018/Algo_Code/call/src/gh30x_demo_algo_call_hrv.c \
	apps/common/device/hr_sensor/gh3018/Algo_Code/call/src/gh30x_demo_algo_call_nadt.c \
	apps/common/device/hr_sensor/gh3018/Algo_Code/call/src/gh30x_demo_algo_call_spo2.c \
	apps/common/device/hr_sensor/gh3018/Algo_Code/common/src/gh30x_demo_algo_call.c \
	apps/common/device/hr_sensor/gh3018/Algo_Code/common/src/gh30x_demo_algo_config.c \
	apps/common/device/hr_sensor/gh3018/Algo_Code/common/src/gh30x_demo_algo_hook.c \
	apps/common/device/hr_sensor/gh3018/Algo_Code/common/src/gh30x_demo_algo_memory.c \
	apps/common/device/hr_sensor/gh3018/Drv_Code/src/gh30x_example_comm_pkg.c \
	apps/common/device/hr_sensor/gh3018/Drv_Code/src/gh30x_example_ctrl.c \
	apps/common/device/hr_sensor/gh3018/Drv_Code/src/gh30x_example_hook.c \
	apps/common/device/hr_sensor/gh3018/Drv_Code/src/gh30x_example_port.c \
	apps/common/device/hr_sensor/gh3018/Drv_Code/src/gh30x_example_process.c \
	apps/common/device/hr_sensor/gh3018/Drv_Code/src/gh30x_example_reg_array.c \
	apps/common/device/hr_sensor/gh3018/Lib_Code/src/heart_net_org_dll.c \
	apps/common/device/hr_sensor/gh3018/Lib_Code/src/HRNet_knConfNetWeights.c \
	apps/common/device/hr_sensor/gh3018/Lib_Code/src/HRNet_knSceneNetWeights.c \
	apps/common/device/hr_sensor/gh3018/Lib_Code/src/HRNet_knSceneSwitchWeights.c \
	apps/common/device/hr_sensor/gh3018/Lib_Code/src/HRNet_knWeights.c \
	apps/common/device/hr_sensor/gh3018/Lib_Code/src/HRNet_knWeightsSmall.c \
	apps/common/device/hr_sensor/gh3018/Lib_Code/src/pv_v2.0.1_spo2wrnet_for_GH3011_masimo_2155fb.c \
	apps/common/device/key/adkey.c \
	apps/common/device/key/iokey.c \
	apps/common/device/key/irkey.c \
	apps/common/device/key/key_driver.c \
	apps/common/device/key/rdec_key.c \
	apps/common/device/key/touch_key.c \
	apps/common/device/nandflash/nandflash.c \
	apps/common/device/nfc/nfc_fm11nt081d.c \
	apps/common/device/norflash/inside_norflash.c \
	apps/common/device/norflash/norflash.c \
	apps/common/device/norflash/norflash_sfc.c \
	apps/common/device/touch_panel/chsc6x/chsc6x_comp.c \
	apps/common/device/touch_panel/chsc6x/chsc6x_main.c \
	apps/common/device/touch_panel/chsc6x/chsc6x_platform.c \
	apps/common/device/usb/device/cdc.c \
	apps/common/device/usb/device/custom_hid.c \
	apps/common/device/usb/device/descriptor.c \
	apps/common/device/usb/device/hid.c \
	apps/common/device/usb/device/msd.c \
	apps/common/device/usb/device/msd_upgrade.c \
	apps/common/device/usb/device/rcsp_hid_inter.c \
	apps/common/device/usb/device/task_pc.c \
	apps/common/device/usb/device/uac1.c \
	apps/common/device/usb/device/uac_stream.c \
	apps/common/device/usb/device/usb_device.c \
	apps/common/device/usb/device/user_setup.c \
	apps/common/device/usb/host/adb.c \
	apps/common/device/usb/host/aoa.c \
	apps/common/device/usb/host/audio.c \
	apps/common/device/usb/host/audio_demo.c \
	apps/common/device/usb/host/hid.c \
	apps/common/device/usb/host/usb_bulk_transfer.c \
	apps/common/device/usb/host/usb_ctrl_transfer.c \
	apps/common/device/usb/host/usb_host.c \
	apps/common/device/usb/host/usb_storage.c \
	apps/common/device/usb/usb_config.c \
	apps/common/device/usb/usb_host_config.c \
	apps/common/fat_nor/nandflash_ftl.c \
	apps/common/fat_nor/nor_fs.c \
	apps/common/fat_nor/phone_rec_fs.c \
	apps/common/fat_nor/virfat_flash.c \
	apps/common/file_operate/file_api.c \
	apps/common/file_operate/file_bs_deal.c \
	apps/common/file_operate/file_manager.c \
	apps/common/music/breakpoint.c \
	apps/common/music/general_player.c \
	apps/common/music/music_decrypt.c \
	apps/common/music/music_id3.c \
	apps/common/music/music_player.c \
	apps/common/pay/upay_aliyun/src/aos/aos_memory.c \
	apps/common/pay/upay_aliyun/src/aos/aos_mutex.c \
	apps/common/pay/upay_aliyun/src/aos/aos_queue.c \
	apps/common/pay/upay_aliyun/src/aos/aos_sem.c \
	apps/common/pay/upay_aliyun/src/aos/aos_system.c \
	apps/common/pay/upay_aliyun/src/aos/aos_task.c \
	apps/common/pay/upay_aliyun/src/aos/aos_time.c \
	apps/common/pay/upay_aliyun/src/aos/aos_timer.c \
	apps/common/pay/upay_aliyun/src/aos_api_test.c \
	apps/common/pay/upay_aliyun/src/bizbus_hal.c \
	apps/common/pay/upay_aliyun/src/upay_demo.c \
	apps/common/pay/upay_aliyun/src/upay_storage.c \
	apps/common/pay/upay_aliyun/src/upay_vendor.c \
	apps/common/pay/upay_t_head/soc/se/JL/hsc32i1/hs_private.c \
	apps/common/pay/upay_t_head/soc/se/JL/hsc32i1/hs_test.c \
	apps/common/pay/upay_t_head/soc/se/JL/vendor_cache.c \
	apps/common/pay/upay_t_head/soc/se/JL/vendor_crypto.c \
	apps/common/pay/upay_t_head/soc/se/JL/vendor_product.c \
	apps/common/pay/upay_t_head/soc/se/JL/vendor_rot.c \
	apps/common/pay/upay_t_head/soc/se/JL/vendor_ssp.c \
	apps/common/pay/upay_t_head/soc/softse/aes.c \
	apps/common/pay/upay_t_head/soc/softse/kdf.c \
	apps/common/pay/upay_t_head/soc/softse/sha256.c \
	apps/common/pay/upay_t_head/soc/softse/utils.c \
	apps/common/pay/upay_t_head/soc/test/test_api.c \
	apps/common/pay/upay_t_head/soc/test/test_crypto_hal.c \
	apps/common/pay/upay_t_head/soc/test/test_ssp_hal.c \
	apps/common/pay/upay_t_head/soc/test/yunit.c \
	apps/common/pay/upay_t_head/soc/test/yunit_test_main.c \
	apps/common/temp_trim/dtemp_pll_trim.c \
	apps/common/third_party_profile/common/3th_profile_api.c \
	apps/common/third_party_profile/common/custom_cfg.c \
	apps/common/third_party_profile/jieli/le_client_demo.c \
	apps/common/third_party_profile/jieli/online_db/online_db_deal.c \
	apps/common/third_party_profile/jieli/online_db/spp_online_db.c \
	apps/common/third_party_profile/jieli/trans_data_demo/le_trans_data.c \
	apps/common/third_party_profile/jieli/trans_data_demo/spp_trans_data.c \
	apps/common/timestamp/timestamp.c \
	apps/common/uC-Shell/Cmd/General/sh_shell.c \
	apps/common/uC-Shell/Source/shell.c \
	apps/common/uC-Shell/Terminal/Mode/VT100/terminal_mode.c \
	apps/common/uC-Shell/Terminal/OS/uCOS-II/terminal_os.c \
	apps/common/uC-Shell/Terminal/Serial/Template/terminal_serial.c \
	apps/common/uC-Shell/Terminal/Source/terminal.c \
	apps/common/uC-Shell/shell_main.c \
	apps/common/ui/lcd/lcd_ui_api.c \
	apps/common/ui/led7/led7_ui_api.c \
    apps/common/ui/lvgl/src/core/lv_disp.c  \
    apps/common/ui/lvgl/src/core/lv_event.c  \
    apps/common/ui/lvgl/src/core/lv_group.c  \
    apps/common/ui/lvgl/src/core/lv_indev.c  \
    apps/common/ui/lvgl/src/core/lv_indev_scroll.c  \
    apps/common/ui/lvgl/src/core/lv_obj.c  \
    apps/common/ui/lvgl/src/core/lv_obj_class.c  \
    apps/common/ui/lvgl/src/core/lv_obj_draw.c  \
    apps/common/ui/lvgl/src/core/lv_obj_pos.c  \
    apps/common/ui/lvgl/src/core/lv_obj_scroll.c  \
    apps/common/ui/lvgl/src/core/lv_obj_style.c  \
    apps/common/ui/lvgl/src/core/lv_obj_style_gen.c  \
    apps/common/ui/lvgl/src/core/lv_obj_tree.c  \
    apps/common/ui/lvgl/src/core/lv_refr.c  \
    apps/common/ui/lvgl/src/core/lv_theme.c  \
    apps/common/ui/lvgl/src/draw/lv_draw.c  \
    apps/common/ui/lvgl/src/draw/lv_draw_arc.c  \
    apps/common/ui/lvgl/src/draw/lv_draw_img.c  \
    apps/common/ui/lvgl/src/draw/lv_draw_label.c  \
    apps/common/ui/lvgl/src/draw/lv_draw_layer.c  \
    apps/common/ui/lvgl/src/draw/lv_draw_line.c  \
    apps/common/ui/lvgl/src/draw/lv_draw_mask.c  \
    apps/common/ui/lvgl/src/draw/lv_draw_rect.c  \
    apps/common/ui/lvgl/src/draw/lv_draw_transform.c  \
    apps/common/ui/lvgl/src/draw/lv_draw_triangle.c  \
    apps/common/ui/lvgl/src/draw/lv_img_buf.c  \
    apps/common/ui/lvgl/src/draw/lv_img_cache.c  \
    apps/common/ui/lvgl/src/draw/lv_img_decoder.c  \
    apps/common/ui/lvgl/src/draw/arm2d/lv_gpu_arm2d.c  \
    apps/common/ui/lvgl/src/draw/nxp/pxp/lv_draw_pxp.c  \
    apps/common/ui/lvgl/src/draw/nxp/pxp/lv_draw_pxp_blend.c  \
    apps/common/ui/lvgl/src/draw/nxp/pxp/lv_gpu_nxp_pxp.c  \
    apps/common/ui/lvgl/src/draw/nxp/pxp/lv_gpu_nxp_pxp_osa.c  \
    apps/common/ui/lvgl/src/draw/nxp/vglite/lv_draw_vglite.c  \
    apps/common/ui/lvgl/src/draw/nxp/vglite/lv_draw_vglite_arc.c  \
    apps/common/ui/lvgl/src/draw/nxp/vglite/lv_draw_vglite_blend.c  \
    apps/common/ui/lvgl/src/draw/nxp/vglite/lv_draw_vglite_line.c  \
    apps/common/ui/lvgl/src/draw/nxp/vglite/lv_draw_vglite_rect.c  \
    apps/common/ui/lvgl/src/draw/nxp/vglite/lv_vglite_buf.c  \
    apps/common/ui/lvgl/src/draw/nxp/vglite/lv_vglite_utils.c  \
    apps/common/ui/lvgl/src/draw/sdl/lv_draw_sdl.c  \
    apps/common/ui/lvgl/src/draw/sdl/lv_draw_sdl_arc.c  \
    apps/common/ui/lvgl/src/draw/sdl/lv_draw_sdl_bg.c  \
    apps/common/ui/lvgl/src/draw/sdl/lv_draw_sdl_composite.c  \
    apps/common/ui/lvgl/src/draw/sdl/lv_draw_sdl_img.c  \
    apps/common/ui/lvgl/src/draw/sdl/lv_draw_sdl_label.c  \
    apps/common/ui/lvgl/src/draw/sdl/lv_draw_sdl_layer.c  \
    apps/common/ui/lvgl/src/draw/sdl/lv_draw_sdl_line.c  \
    apps/common/ui/lvgl/src/draw/sdl/lv_draw_sdl_mask.c  \
    apps/common/ui/lvgl/src/draw/sdl/lv_draw_sdl_polygon.c  \
    apps/common/ui/lvgl/src/draw/sdl/lv_draw_sdl_rect.c  \
    apps/common/ui/lvgl/src/draw/sdl/lv_draw_sdl_stack_blur.c  \
    apps/common/ui/lvgl/src/draw/sdl/lv_draw_sdl_texture_cache.c  \
    apps/common/ui/lvgl/src/draw/sdl/lv_draw_sdl_utils.c  \
    apps/common/ui/lvgl/src/draw/stm32_dma2d/lv_gpu_stm32_dma2d.c  \
    apps/common/ui/lvgl/src/draw/sw/lv_draw_sw.c  \
    apps/common/ui/lvgl/src/draw/sw/lv_draw_sw_arc.c  \
    apps/common/ui/lvgl/src/draw/sw/lv_draw_sw_blend.c  \
    apps/common/ui/lvgl/src/draw/sw/lv_draw_sw_dither.c  \
    apps/common/ui/lvgl/src/draw/sw/lv_draw_sw_gradient.c  \
    apps/common/ui/lvgl/src/draw/sw/lv_draw_sw_img.c  \
    apps/common/ui/lvgl/src/draw/sw/lv_draw_sw_layer.c  \
    apps/common/ui/lvgl/src/draw/sw/lv_draw_sw_letter.c  \
    apps/common/ui/lvgl/src/draw/sw/lv_draw_sw_line.c  \
    apps/common/ui/lvgl/src/draw/sw/lv_draw_sw_polygon.c  \
    apps/common/ui/lvgl/src/draw/sw/lv_draw_sw_rect.c  \
    apps/common/ui/lvgl/src/draw/sw/lv_draw_sw_transform.c  \
    apps/common/ui/lvgl/src/draw/swm341_dma2d/lv_gpu_swm341_dma2d.c  \
    apps/common/ui/lvgl/src/extra/lv_extra.c  \
    apps/common/ui/lvgl/src/extra/layouts/flex/lv_flex.c  \
    apps/common/ui/lvgl/src/extra/layouts/grid/lv_grid.c  \
    apps/common/ui/lvgl/src/extra/libs/bmp/lv_bmp.c  \
    apps/common/ui/lvgl/src/extra/libs/ffmpeg/lv_ffmpeg.c  \
    apps/common/ui/lvgl/src/extra/libs/freetype/lv_freetype.c  \
    apps/common/ui/lvgl/src/extra/libs/fsdrv/lv_fs_fatfs.c  \
    apps/common/ui/lvgl/src/extra/libs/fsdrv/lv_fs_posix.c  \
    apps/common/ui/lvgl/src/extra/libs/fsdrv/lv_fs_stdio.c  \
    apps/common/ui/lvgl/src/extra/libs/fsdrv/lv_fs_win32.c  \
    apps/common/ui/lvgl/src/extra/libs/gif/gifdec.c  \
    apps/common/ui/lvgl/src/extra/libs/gif/lv_gif.c  \
    apps/common/ui/lvgl/src/extra/libs/png/lodepng.c  \
    apps/common/ui/lvgl/src/extra/libs/png/lv_png.c  \
    apps/common/ui/lvgl/src/extra/libs/qrcode/lv_qrcode.c  \
    apps/common/ui/lvgl/src/extra/libs/qrcode/qrcodegen.c  \
    apps/common/ui/lvgl/src/extra/libs/rlottie/lv_rlottie.c  \
    apps/common/ui/lvgl/src/extra/libs/sjpg/lv_sjpg.c  \
    apps/common/ui/lvgl/src/extra/libs/sjpg/tjpgd.c  \
    apps/common/ui/lvgl/src/extra/others/fragment/lv_fragment.c  \
    apps/common/ui/lvgl/src/extra/others/fragment/lv_fragment_manager.c  \
    apps/common/ui/lvgl/src/extra/others/gridnav/lv_gridnav.c  \
    apps/common/ui/lvgl/src/extra/others/ime/lv_ime_pinyin.c  \
    apps/common/ui/lvgl/src/extra/others/imgfont/lv_imgfont.c  \
    apps/common/ui/lvgl/src/extra/others/monkey/lv_monkey.c  \
    apps/common/ui/lvgl/src/extra/others/msg/lv_msg.c  \
    apps/common/ui/lvgl/src/extra/others/snapshot/lv_snapshot.c  \
    apps/common/ui/lvgl/src/extra/themes/basic/lv_theme_basic.c  \
    apps/common/ui/lvgl/src/extra/themes/default/lv_theme_default.c  \
    apps/common/ui/lvgl/src/extra/themes/mono/lv_theme_mono.c  \
    apps/common/ui/lvgl/src/extra/widgets/animimg/lv_animimg.c  \
    apps/common/ui/lvgl/src/extra/widgets/calendar/lv_calendar.c  \
    apps/common/ui/lvgl/src/extra/widgets/calendar/lv_calendar_header_arrow.c  \
    apps/common/ui/lvgl/src/extra/widgets/calendar/lv_calendar_header_dropdown.c  \
    apps/common/ui/lvgl/src/extra/widgets/chart/lv_chart.c  \
    apps/common/ui/lvgl/src/extra/widgets/colorwheel/lv_colorwheel.c  \
    apps/common/ui/lvgl/src/extra/widgets/imgbtn/lv_imgbtn.c  \
    apps/common/ui/lvgl/src/extra/widgets/keyboard/lv_keyboard.c  \
    apps/common/ui/lvgl/src/extra/widgets/led/lv_led.c  \
    apps/common/ui/lvgl/src/extra/widgets/list/lv_list.c  \
    apps/common/ui/lvgl/src/extra/widgets/menu/lv_menu.c  \
    apps/common/ui/lvgl/src/extra/widgets/meter/lv_meter.c  \
    apps/common/ui/lvgl/src/extra/widgets/msgbox/lv_msgbox.c  \
    apps/common/ui/lvgl/src/extra/widgets/span/lv_span.c  \
    apps/common/ui/lvgl/src/extra/widgets/spinbox/lv_spinbox.c  \
    apps/common/ui/lvgl/src/extra/widgets/spinner/lv_spinner.c  \
    apps/common/ui/lvgl/src/extra/widgets/tabview/lv_tabview.c  \
    apps/common/ui/lvgl/src/extra/widgets/tileview/lv_tileview.c  \
    apps/common/ui/lvgl/src/extra/widgets/win/lv_win.c  \
    apps/common/ui/lvgl/src/font/lv_font.c  \
    apps/common/ui/lvgl/src/font/lv_font_dejavu_16_persian_hebrew.c  \
    apps/common/ui/lvgl/src/font/lv_font_fmt_txt.c  \
    apps/common/ui/lvgl/src/font/lv_font_loader.c  \
	apps/common/ui/lvgl/src/font/font_common_32.c  \
	apps/common/ui/lvgl/src/font/font_common_num_12.c  \
	apps/common/ui/lvgl/src/font/font_common_num_18.c  \
	apps/common/ui/lvgl/src/font/font_common_num_24.c  \
	apps/common/ui/lvgl/src/font/font_common_num_32.c  \
	apps/common/ui/lvgl/src/font/font_common_num_36.c  \
	apps/common/ui/lvgl/src/font/font_common_num_42.c  \
	apps/common/ui/lvgl/src/font/font_common_num_52.c  \
	apps/common/ui/lvgl/src/font/font_common_num_64.c  \
	apps/common/ui/lvgl/src/font/font_common_num_78.c  \
	apps/common/ui/lvgl/src/font/font_common_24_no_ch.c  \
    apps/common/ui/lvgl/src/font/lv_font_montserrat_12.c  \
    apps/common/ui/lvgl/src/hal/lv_hal_disp.c  \
    apps/common/ui/lvgl/src/hal/lv_hal_indev.c  \
    apps/common/ui/lvgl/src/hal/lv_hal_tick.c  \
    apps/common/ui/lvgl/src/misc/lv_anim.c  \
    apps/common/ui/lvgl/src/misc/lv_anim_timeline.c  \
    apps/common/ui/lvgl/src/misc/lv_area.c  \
    apps/common/ui/lvgl/src/misc/lv_async.c  \
    apps/common/ui/lvgl/src/misc/lv_bidi.c  \
    apps/common/ui/lvgl/src/misc/lv_color.c  \
    apps/common/ui/lvgl/src/misc/lv_fs.c  \
    apps/common/ui/lvgl/src/misc/lv_gc.c  \
    apps/common/ui/lvgl/src/misc/lv_ll.c  \
    apps/common/ui/lvgl/src/misc/lv_log.c  \
    apps/common/ui/lvgl/src/misc/lv_lru.c  \
    apps/common/ui/lvgl/src/misc/lv_math.c  \
    apps/common/ui/lvgl/src/misc/lv_mem.c  \
    apps/common/ui/lvgl/src/misc/lv_printf.c  \
    apps/common/ui/lvgl/src/misc/lv_style.c  \
    apps/common/ui/lvgl/src/misc/lv_style_gen.c  \
    apps/common/ui/lvgl/src/misc/lv_templ.c  \
    apps/common/ui/lvgl/src/misc/lv_timer.c  \
    apps/common/ui/lvgl/src/misc/lv_tlsf.c  \
    apps/common/ui/lvgl/src/misc/lv_txt.c  \
    apps/common/ui/lvgl/src/misc/lv_txt_ap.c  \
    apps/common/ui/lvgl/src/misc/lv_utils.c  \
    apps/common/ui/lvgl/src/widgets/lv_arc.c  \
    apps/common/ui/lvgl/src/widgets/lv_bar.c  \
    apps/common/ui/lvgl/src/widgets/lv_btn.c  \
    apps/common/ui/lvgl/src/widgets/lv_btnmatrix.c  \
    apps/common/ui/lvgl/src/widgets/lv_canvas.c  \
    apps/common/ui/lvgl/src/widgets/lv_checkbox.c  \
    apps/common/ui/lvgl/src/widgets/lv_dropdown.c  \
    apps/common/ui/lvgl/src/widgets/lv_img.c  \
    apps/common/ui/lvgl/src/widgets/lv_label.c  \
    apps/common/ui/lvgl/src/widgets/lv_line.c  \
    apps/common/ui/lvgl/src/widgets/lv_objx_templ.c  \
    apps/common/ui/lvgl/src/widgets/lv_roller.c  \
    apps/common/ui/lvgl/src/widgets/lv_slider.c  \
    apps/common/ui/lvgl/src/widgets/lv_switch.c  \
    apps/common/ui/lvgl/src/widgets/lv_table.c  \
    apps/common/ui/lvgl/src/widgets/lv_textarea.c  \
	apps/common/ui/ui/jl_images/file_index.c  \
	apps/common/ui/ui/jl_images/ex_file_index.c  \
	apps/common/update/ex_flash_file_download.c \
	apps/common/update/norflash_ufw_update.c \
	apps/common/update/norflash_update.c \
	apps/common/update/testbox_update.c \
	apps/common/update/uart_update.c \
	apps/common/update/uart_update_master.c \
	apps/common/update/update.c \
	apps/common/update/user_file_download/ext_flash_file_download.c \
	apps/common/update/user_file_download/reserve_file_download.c \
	apps/common/update/user_file_download/user_file_download.c \
	apps/watch/aec/br28/audio_aec.c \
	apps/watch/aec/br28/audio_aec_demo.c \
	apps/watch/app_main.c \
	apps/watch/board/br28/board_7012a7_demo/board_7012a7_demo.c \
	apps/watch/board/br28/board_7012a7_demo/key_table/adkey_table.c \
	apps/watch/board/br28/board_7012a7_demo/key_table/iokey_table.c \
	apps/watch/board/br28/board_7012a7_demo/key_table/irkey_table.c \
	apps/watch/board/br28/board_7012a7_demo/key_table/rdec_key_table.c \
	apps/watch/board/br28/board_7012a7_demo/key_table/touch_key_table.c \
	apps/watch/board/br28/board_7012a_demo/board_7012a_demo.c \
	apps/watch/board/br28/board_7012a_demo/key_table/adkey_table.c \
	apps/watch/board/br28/board_7012a_demo/key_table/iokey_table.c \
	apps/watch/board/br28/board_7012a_demo/key_table/irkey_table.c \
	apps/watch/board/br28/board_7012a_demo/key_table/rdec_key_table.c \
	apps/watch/board/br28/board_7012a_demo/key_table/touch_key_table.c \
	apps/watch/board/br28/board_701n_demo/board_701n_demo.c \
	apps/watch/board/br28/board_701n_demo/key_table/adkey_table.c \
	apps/watch/board/br28/board_701n_demo/key_table/iokey_table.c \
	apps/watch/board/br28/board_701n_demo/key_table/irkey_table.c \
	apps/watch/board/br28/board_701n_demo/key_table/rdec_key_table.c \
	apps/watch/board/br28/board_701n_demo/key_table/touch_key_table.c \
	apps/watch/board/br28/board_701n_lvgl_demo/board_701n_lvgl_demo.c \
	apps/watch/board/br28/board_701n_lvgl_demo/key_table/adkey_table.c \
	apps/watch/board/br28/board_701n_lvgl_demo/key_table/iokey_table.c \
	apps/watch/board/br28/board_701n_lvgl_demo/key_table/irkey_table.c \
	apps/watch/board/br28/board_701n_lvgl_demo/key_table/rdec_key_table.c \
	apps/watch/board/br28/board_701n_lvgl_demo/key_table/touch_key_table.c \
	apps/watch/board/br28/board_ad2802e_demo/board_ad2802e_demo.c \
	apps/watch/board/br28/board_ad2802e_demo/key_table/adkey_table.c \
	apps/watch/board/br28/board_ad2802e_demo/key_table/iokey_table.c \
	apps/watch/board/br28/board_ad2802e_demo/key_table/irkey_table.c \
	apps/watch/board/br28/board_ad2802e_demo/key_table/rdec_key_table.c \
	apps/watch/board/br28/board_ad2802e_demo/key_table/touch_key_table.c \
	apps/watch/common/dev_status.c \
	apps/watch/common/init.c \
	apps/watch/common/smart_voice_user.c \
	apps/watch/common/sport_data/altitude_presure.c \
	apps/watch/common/sport_data/blood_oxygen.c \
	apps/watch/common/sport_data/detection_response.c \
	apps/watch/common/sport_data/heart_rate.c \
	apps/watch/common/sport_data/motion_module.c \
	apps/watch/common/sport_data/sleep.c \
	apps/watch/common/sport_data/sport.c \
	apps/watch/common/sport_data/watch_common.c \
	apps/watch/common/sport_data/watch_time_common.c \
	apps/watch/common/step_calcul.c \
	apps/watch/common/task_table.c \
	apps/watch/common/tone_table.c \
	apps/watch/common/user_cfg.c \
	apps/watch/font/fontinit.c \
	apps/watch/kws_voice_event_deal.c \
	apps/watch/log_config/app_config.c \
	apps/watch/log_config/lib_btctrler_config.c \
	apps/watch/log_config/lib_btstack_config.c \
	apps/watch/log_config/lib_driver_config.c \
	apps/watch/log_config/lib_media_config.c \
	apps/watch/log_config/lib_system_config.c \
	apps/watch/log_config/lib_update_config.c \
	apps/watch/power_manage/app_charge.c \
	apps/watch/power_manage/app_chargestore.c \
	apps/watch/power_manage/app_power_manage.c \
	apps/watch/product_test/product_test.c \
	apps/watch/product_test/pt_gpio.c \
	apps/watch/product_test/pt_gsensor.c \
	apps/watch/product_test/pt_hr.c \
	apps/watch/product_test/pt_lcd_tp.c \
	apps/watch/product_test/pt_motor.c \
	apps/watch/product_test/pt_sd.c \
	apps/watch/product_test/pt_speaker_mic.c \
	apps/watch/smartbox/browser/browser.c \
	apps/watch/smartbox/bt_manage/bt_trans_data/le_smartbox_adv.c \
	apps/watch/smartbox/bt_manage/bt_trans_data/le_smartbox_module.c \
	apps/watch/smartbox/bt_manage/bt_trans_data/le_smartbox_multi_client.c \
	apps/watch/smartbox/bt_manage/bt_trans_data/le_smartbox_multi_common.c \
	apps/watch/smartbox/bt_manage/bt_trans_data/le_smartbox_multi_trans.c \
	apps/watch/smartbox/bt_manage/smartbox_bt_manage.c \
	apps/watch/smartbox/cmd_data_deal/cmd_recieve.c \
	apps/watch/smartbox/cmd_data_deal/cmd_recieve_no_respone.c \
	apps/watch/smartbox/cmd_data_deal/cmd_respone.c \
	apps/watch/smartbox/cmd_data_deal/cmd_user.c \
	apps/watch/smartbox/cmd_data_deal/command.c \
	apps/watch/smartbox/cmd_data_deal/data_recieve.c \
	apps/watch/smartbox/cmd_data_deal/data_recieve_no_respone.c \
	apps/watch/smartbox/cmd_data_deal/data_respone.c \
	apps/watch/smartbox/config.c \
	apps/watch/smartbox/event.c \
	apps/watch/smartbox/external_flash/smartbox_extra_flash_cmd.c \
	apps/watch/smartbox/external_flash/smartbox_extra_flash_opt.c \
	apps/watch/smartbox/feature.c \
	apps/watch/smartbox/file_transfer/dev_format.c \
	apps/watch/smartbox/file_transfer/file_bluk_trans_prepare.c \
	apps/watch/smartbox/file_transfer/file_delete.c \
	apps/watch/smartbox/file_transfer/file_env_prepare.c \
	apps/watch/smartbox/file_transfer/file_simple_transfer.c \
	apps/watch/smartbox/file_transfer/file_trans_back.c \
	apps/watch/smartbox/file_transfer/file_transfer.c \
	apps/watch/smartbox/func_cmd/bt_func.c \
	apps/watch/smartbox/func_cmd/fm_func.c \
	apps/watch/smartbox/func_cmd/linein_func.c \
	apps/watch/smartbox/func_cmd/music_func.c \
	apps/watch/smartbox/func_cmd/rtc_func.c \
	apps/watch/smartbox/function.c \
	apps/watch/smartbox/mass_data/mass_data.c \
	apps/watch/smartbox/sensors_data_opt/nfc_data_opt.c \
	apps/watch/smartbox/sensors_data_opt/sensor_log_notify.c \
	apps/watch/smartbox/sensors_data_opt/sensors_data_opt.c \
	apps/watch/smartbox/sensors_data_opt/sport_data_func.c \
	apps/watch/smartbox/sensors_data_opt/sport_data_info_opt/sport_info_bt_disconn.c \
	apps/watch/smartbox/sensors_data_opt/sport_data_info_opt/sport_info_continuous_heart_rate.c \
	apps/watch/smartbox/sensors_data_opt/sport_data_info_opt/sport_info_exercise_heart_rate.c \
	apps/watch/smartbox/sensors_data_opt/sport_data_info_opt/sport_info_fall_detection.c \
	apps/watch/smartbox/sensors_data_opt/sport_data_info_opt/sport_info_personal_info.c \
	apps/watch/smartbox/sensors_data_opt/sport_data_info_opt/sport_info_pressure_detection.c \
	apps/watch/smartbox/sensors_data_opt/sport_data_info_opt/sport_info_raise_wrist.c \
	apps/watch/smartbox/sensors_data_opt/sport_data_info_opt/sport_info_sedentary.c \
	apps/watch/smartbox/sensors_data_opt/sport_data_info_opt/sport_info_sensor_opt.c \
	apps/watch/smartbox/sensors_data_opt/sport_data_info_opt/sport_info_sleep_detection.c \
	apps/watch/smartbox/sensors_data_opt/sport_info_opt.c \
	apps/watch/smartbox/sensors_data_opt/sport_info_sync.c \
	apps/watch/smartbox/sensors_data_opt/sport_info_vm.c \
	apps/watch/smartbox/sensors_data_opt/sports_data_opt/sport_data_air_pressure.c \
	apps/watch/smartbox/sensors_data_opt/sports_data_opt/sport_data_altitude.c \
	apps/watch/smartbox/sensors_data_opt/sports_data_opt/sport_data_blood_oxygen.c \
	apps/watch/smartbox/sensors_data_opt/sports_data_opt/sport_data_exercise_recovery_time.c \
	apps/watch/smartbox/sensors_data_opt/sports_data_opt/sport_data_exercise_steps.c \
	apps/watch/smartbox/sensors_data_opt/sports_data_opt/sport_data_heart_rate.c \
	apps/watch/smartbox/sensors_data_opt/sports_data_opt/sport_data_max_oxygen_uptake.c \
	apps/watch/smartbox/sensors_data_opt/sports_data_opt/sport_data_pressure_detection.c \
	apps/watch/smartbox/sensors_data_opt/sports_data_opt/sport_data_sports_information.c \
	apps/watch/smartbox/sensors_data_opt/sports_data_opt/sport_data_training_load.c \
	apps/watch/smartbox/smartbox.c \
	apps/watch/smartbox/smartbox_device_config.c \
	apps/watch/smartbox/smartbox_rcsp_manage.c \
	apps/watch/smartbox/smartbox_setting/adv_bt_name_setting.c \
	apps/watch/smartbox/smartbox_setting/adv_key_setting.c \
	apps/watch/smartbox/smartbox_setting/adv_led_setting.c \
	apps/watch/smartbox/smartbox_setting/adv_mic_setting.c \
	apps/watch/smartbox/smartbox_setting/adv_time_stamp_setting.c \
	apps/watch/smartbox/smartbox_setting/adv_work_setting.c \
	apps/watch/smartbox/smartbox_setting/smartbox_color_led_setting.c \
	apps/watch/smartbox/smartbox_setting/smartbox_eq_setting.c \
	apps/watch/smartbox/smartbox_setting/smartbox_high_low_vol_setting.c \
	apps/watch/smartbox/smartbox_setting/smartbox_karaoke_eq_setting.c \
	apps/watch/smartbox/smartbox_setting/smartbox_karaoke_setting.c \
	apps/watch/smartbox/smartbox_setting/smartbox_misc_setting/smartbox_misc_drc_setting.c \
	apps/watch/smartbox/smartbox_setting/smartbox_misc_setting/smartbox_misc_reverbration_setting.c \
	apps/watch/smartbox/smartbox_setting/smartbox_misc_setting/smartbox_misc_setting.c \
	apps/watch/smartbox/smartbox_setting/smartbox_music_info_setting.c \
	apps/watch/smartbox/smartbox_setting/smartbox_vol_setting.c \
	apps/watch/smartbox/smartbox_setting_opt/smartbox_adv_bluetooth.c \
	apps/watch/smartbox/smartbox_setting_opt/smartbox_setting_opt.c \
	apps/watch/smartbox/smartbox_setting_opt/smartbox_setting_sync.c \
	apps/watch/smartbox/smartbox_task.c \
	apps/watch/smartbox/smartbox_update/rcsp_ch_loader_download.c \
	apps/watch/smartbox/smartbox_update/smartbox_update.c \
	apps/watch/smartbox/smartbox_update/smartbox_update_tws.c \
	apps/watch/smartbox/switch_device.c \
	apps/watch/sport/manage_source.c \
	apps/watch/sport/message_vm_cfg.c \
	apps/watch/sport/sport_api.c \
	apps/watch/task_manager/app_common.c \
	apps/watch/task_manager/app_task_switch.c \
	apps/watch/task_manager/bt/bt.c \
	apps/watch/task_manager/bt/bt_ble.c \
	apps/watch/task_manager/bt/bt_emitter.c \
	apps/watch/task_manager/bt/bt_event_fun.c \
	apps/watch/task_manager/bt/bt_key_fun.c \
	apps/watch/task_manager/bt/bt_product_test.c \
	apps/watch/task_manager/bt/bt_switch_fun.c \
	apps/watch/task_manager/bt/bt_tws.c \
	apps/watch/task_manager/bt/vol_sync.c \
	apps/watch/task_manager/idle/idle.c \
	apps/watch/task_manager/music/music.c \
	apps/watch/task_manager/pc/pc.c \
	apps/watch/task_manager/power_off/power_off.c \
	apps/watch/task_manager/power_on/power_on.c \
	apps/watch/task_manager/record/record.c \
	apps/watch/task_manager/rtc/alarm_api.c \
	apps/watch/task_manager/rtc/alarm_user.c \
	apps/watch/task_manager/rtc/rtc.c \
	apps/watch/task_manager/rtc/virtual_rtc.c \
	apps/watch/task_manager/task_key.c \
	apps/watch/third_party_profile/ancs_client_demo/ancs_client_demo.c \
	apps/watch/third_party_profile/trans_data_demo/trans_data_demo.c \
	apps/watch/ui/lcd/lyrics_api.c \
	apps/watch/ui/lcd/ui_screen_saver.c \
	apps/watch/ui/lcd/ui_sys_param_api.c \
	apps/watch/ui/lcd/watch_syscfg_manage.c \
	apps/watch/ui/led/pwm_led_api.c \
	apps/watch/ui/led/pwm_led_para_table.c \
	apps/watch/ui/lua_ui/lua_bsp.c \
	apps/watch/ui/lua_ui/lua_bt.c \
	apps/watch/ui/lua_ui/lua_device.c \
	apps/watch/ui/lua_ui/lua_gui.c \
	apps/watch/ui/lua_ui/lua_module.c \
	apps/watch/ui/lua_ui/lua_music.c \
	apps/watch/ui/lua_ui/lua_obj.c \
	apps/watch/ui/lua_ui/lua_obj_battery.c \
	apps/watch/ui/lua_ui/lua_obj_compass.c \
	apps/watch/ui/lua_ui/lua_obj_image.c \
	apps/watch/ui/lua_ui/lua_obj_layout.c \
	apps/watch/ui/lua_ui/lua_obj_list.c \
	apps/watch/ui/lua_ui/lua_obj_multiprogress.c \
	apps/watch/ui/lua_ui/lua_obj_number.c \
	apps/watch/ui/lua_ui/lua_obj_progress.c \
	apps/watch/ui/lua_ui/lua_obj_slider.c \
	apps/watch/ui/lua_ui/lua_obj_text.c \
	apps/watch/ui/lua_ui/lua_obj_time.c \
	apps/watch/ui/lua_ui/lua_obj_watch.c \
	apps/watch/ui/lua_ui/lua_sys.c \
	apps/watch/ui/lua_ui/lua_utils.c \
	apps/watch/ui/lua_ui/lua_weather.c \
	apps/watch/user_api/dev_multiplex_api.c \
	apps/watch/user_api/product_info_api.c \
	apps/watch/user_api/vm_api.c \
	apps/watch/version.c \
	cpu/br28/adc_api.c \
	cpu/br28/audio_codec_clock.c \
	cpu/br28/audio_common/app_audio.c \
	cpu/br28/audio_common/sound_device_driver.c \
	cpu/br28/audio_common/sound_pcm_demo.c \
	cpu/br28/audio_dec/audio_dec.c \
	cpu/br28/audio_dec/audio_dec_bt.c \
	cpu/br28/audio_dec/audio_dec_file.c \
	cpu/br28/audio_dec/audio_dec_fm.c \
	cpu/br28/audio_dec/audio_dec_linein.c \
	cpu/br28/audio_dec/audio_dec_midi_ctrl.c \
	cpu/br28/audio_dec/audio_dec_midi_file.c \
	cpu/br28/audio_dec/audio_dec_pc.c \
	cpu/br28/audio_dec/audio_dec_record.c \
	cpu/br28/audio_dec/audio_dec_spdif.c \
	cpu/br28/audio_dec/audio_dec_tone.c \
	cpu/br28/audio_dec/audio_spectrum.c \
	cpu/br28/audio_dec/audio_sync.c \
	cpu/br28/audio_dec/audio_usb_mic.c \
	cpu/br28/audio_dec/lfwordana_enc_api.c \
	cpu/br28/audio_dec/tone_player.c \
	cpu/br28/audio_effect/audio_autotune_demo.c \
	cpu/br28/audio_effect/audio_ch_swap_demo.c \
	cpu/br28/audio_effect/audio_dynamic_eq_demo.c \
	cpu/br28/audio_effect/audio_eff_default_parm.c \
	cpu/br28/audio_effect/audio_eq_drc_demo.c \
	cpu/br28/audio_effect/audio_gain_process_demo.c \
	cpu/br28/audio_effect/audio_noise_gate_demo.c \
	cpu/br28/audio_effect/audio_sound_track_2_p_x.c \
	cpu/br28/audio_effect/audio_surround_demo.c \
	cpu/br28/audio_effect/audio_vbass_demo.c \
	cpu/br28/audio_effect/audio_voice_changer_demo.c \
	cpu/br28/audio_effect/effects_adj.c \
	cpu/br28/audio_effect/eq_config.c \
	cpu/br28/audio_enc/audio_adc_demo.c \
	cpu/br28/audio_enc/audio_enc.c \
	cpu/br28/audio_enc/audio_enc_file.c \
	cpu/br28/audio_enc/audio_enc_recoder.c \
	cpu/br28/audio_enc/audio_mic_codec.c \
	cpu/br28/audio_enc/audio_sbc_codec.c \
	cpu/br28/audio_iis.c \
	cpu/br28/audio_link.c \
	cpu/br28/audio_mic_capless.c \
	cpu/br28/audio_way/audio_app_stream.c \
	cpu/br28/audio_way/audio_bt_emitter/audio_bt_emitter.c \
	cpu/br28/audio_way/audio_bt_emitter/audio_bt_emitter_controller.c \
	cpu/br28/audio_way/audio_bt_emitter/audio_bt_emitter_dma.c \
	cpu/br28/audio_way/audio_bt_emitter/audio_bt_emitter_hw.c \
	cpu/br28/audio_way/audio_way.c \
	cpu/br28/audio_way/audio_way_bt_emitter.c \
	cpu/br28/audio_way/audio_way_dac.c \
	cpu/br28/charge.c \
	cpu/br28/chargestore.c \
	cpu/br28/clock_manager.c \
	cpu/br28/hw_fft.c \
	cpu/br28/iic_hw.c \
	cpu/br28/iic_soft.c \
	cpu/br28/irflt.c \
	cpu/br28/localtws/localtws.c \
	cpu/br28/localtws/localtws_dec.c \
	cpu/br28/lp_touch_key.c \
	cpu/br28/lp_touch_key_tool.c \
	cpu/br28/lua_port_api.c \
	cpu/br28/mcpwm.c \
	cpu/br28/overlay_code.c \
	cpu/br28/plcnt.c \
	cpu/br28/power/power_app.c \
	cpu/br28/power/power_check.c \
	cpu/br28/power/power_port.c \
	cpu/br28/power/power_trim.c \
	cpu/br28/psram.c \
	cpu/br28/pwm_led.c \
	cpu/br28/rdec.c \
	cpu/br28/rdec_soft.c \
	cpu/br28/setup.c \
	cpu/br28/smart_voice/aispeech_asr.c \
	cpu/br28/smart_voice/jl_kws_platform.c \
	cpu/br28/smart_voice/kws_event.c \
	cpu/br28/smart_voice/nn_vad.c \
	cpu/br28/smart_voice/smart_voice_config.c \
	cpu/br28/smart_voice/smart_voice_core.c \
	cpu/br28/smart_voice/user_asr.c \
	cpu/br28/smart_voice/vad_clock_trim.c \
	cpu/br28/smart_voice/vad_mic.c \
	cpu/br28/smart_voice/voice_mic_data.c \
	cpu/br28/spi.c \
	cpu/br28/uart_dev.c \
	cpu/br28/ui_driver/LED_1888/LED1888.c \
	cpu/br28/ui_driver/imb_demo.c \
	cpu/br28/ui_driver/interface/ui_platform.c \
	cpu/br28/ui_driver/jpeg_demo.c \
	cpu/br28/ui_driver/lcd_drive/lcd_drive.c \
	cpu/br28/ui_driver/lcd_drive/lcd_mcu/lcd_mcu_JD5858_320x385.c \
	cpu/br28/ui_driver/lcd_drive/lcd_mcu/lcd_mcu_JD5858_360x360.c \
	cpu/br28/ui_driver/lcd_drive/lcd_rgb/lcd_rgb_st7789v_240x240.c \
	cpu/br28/ui_driver/lcd_drive/lcd_spi/lcd_qspi_ICNA3310B_466x466.c \
	cpu/br28/ui_driver/lcd_drive/lcd_spi/lcd_qspi_ST77903_400x400.c \
	cpu/br28/ui_driver/lcd_drive/lcd_spi/lcd_qspi_ST77903_400x400_v1.c \
	cpu/br28/ui_driver/lcd_drive/lcd_spi/lcd_qspi_ST77903_400x400_v2.c \
	cpu/br28/ui_driver/lcd_drive/lcd_spi/lcd_qspi_ST77903_400x400_v3.c \
	cpu/br28/ui_driver/lcd_drive/lcd_spi/lcd_qspi_ST77903_400x400_v4.c \
	cpu/br28/ui_driver/lcd_drive/lcd_spi/lcd_spi_L_WEA2012_356x400.c \
	cpu/br28/ui_driver/lcd_drive/lcd_spi/lcd_spi_gc9307_RQ_BOE_240x283.c \
	cpu/br28/ui_driver/lcd_drive/lcd_spi/lcd_spi_rm69330_454x454.c \
	cpu/br28/ui_driver/lcd_drive/lcd_spi/lcd_spi_sh8601a_454x454.c \
	cpu/br28/ui_driver/lcd_drive/lcd_spi/lcd_qspi_ST77916_320x386.c \
	cpu/br28/ui_driver/lcd_drive/lcd_spi/lcd_spi_st7789_BOE1.54_update_240x240.c \
	cpu/br28/ui_driver/lcd_drive/lcd_spi/lcd_spi_st7789v_240x240.c \
	cpu/br28/ui_driver/lcd_drive/lcd_spi/lcd_qspi_ft2308_368x448.c \
	cpu/br28/ui_driver/lcd_seg/lcd_seg3x9_driver.c \
	cpu/br28/ui_driver/led7/led7_driver.c \
	cpu/br28/ui_driver/lvgl/lv_port_disp.c \
	cpu/br28/ui_driver/lvgl/lv_port_fs.c \
	cpu/br28/ui_driver/lvgl/lv_port_indev.c \
	cpu/br28/ui_driver/lvgl/lvgl_main.c \
	cpu/br28/ui_driver/ui_common.c \
	cpu/br28/umidigi_chargestore.c \
	cpu/br28/user_p11_cmd.c \
	apps/common/ui/lv_watch/lv_watch.c \
	apps/common/ui/lv_watch/poc_modem/poc_modem_ui.c \
	apps/common/ui/lv_watch/poc_modem/poc_modem_vm.c \
	apps/common/ui/lv_watch/poc_modem/poc_modem_cache.c \
	apps/common/ui/lv_watch/comm_key/common_key.c \
	apps/common/ui/lv_watch/comm_key/common_rdec.c \
	apps/common/ui/lv_watch/comm_lang/lang_conf.c \
	apps/common/ui/lv_watch/comm_lang/lang_table.c \
	apps/common/ui/lv_watch/comm_lang/al_name_table.c \
	apps/common/ui/lv_watch/comm_lang/quran_table.c \
	apps/common/ui/lv_watch/comm_parse/le_task.c \
	apps/common/ui/lv_watch/comm_parse/common_rev.c \
	apps/common/ui/lv_watch/comm_parse/common_send.c \
	apps/common/ui/lv_watch/comm_parse/common_parse.c \
	apps/common/ui/lv_watch/comm_parse/common_le_set.c \
	apps/common/ui/lv_watch/comm_parse/common_le_get.c \
	apps/common/ui/lv_watch/comm_parse/le_history_data.c \
	apps/common/ui/lv_watch/comm_offscr/common_offscreen.c \
	apps/common/ui/lv_watch/comm_refr/common_refresh.c \
	apps/common/ui/lv_watch/comm_widget/widget_time.c \
	apps/common/ui/lv_watch/comm_widget/widget_bo.c \
	apps/common/ui/lv_watch/comm_widget/widget_hr.c \
	apps/common/ui/lv_watch/comm_widget/widget_step.c \
	apps/common/ui/lv_watch/comm_widget/widget_pace.c \
	apps/common/ui/lv_watch/comm_widget/widget_calorie.c \
	apps/common/ui/lv_watch/comm_widget/widget_distance.c \
	apps/common/ui/lv_watch/comm_widget/widget_weather.c \
	apps/common/ui/lv_watch/comm_widget/widget_num_str.c \
	apps/common/ui/lv_watch/comm_widget/common_data.c \
	apps/common/ui/lv_watch/comm_widget/widget_date.c \
	apps/common/ui/lv_watch/comm_widget/widget_week.c \
	apps/common/ui/lv_watch/comm_widget/widget_clock.c \
	apps/common/ui/lv_watch/comm_widget/common_widget.c \
	apps/common/ui/lv_watch/comm_widget/widget_refresh.c \
	apps/common/ui/lv_watch/comm_func/call_func.c \
	apps/common/ui/lv_watch/comm_func/azkar_func.c \
	apps/common/ui/lv_watch/comm_func/message_func.c \
	apps/common/ui/lv_watch/comm_func/prayer_time_func.c \
	apps/common/ui/lv_watch/comm_func/countdown_func.c \
	apps/common/ui/lv_watch/comm_func/stopwatch_func.c \
	apps/common/ui/lv_watch/comm_func/ll_info_func.c \
	apps/common/ui/lv_watch/comm_func/kaaba_qibla_func.c \
	apps/common/ui/lv_watch/comm_func/tasbih_reminder_func.c \
	apps/common/ui/lv_watch/comm_func/allah_name_func.c \
	apps/common/ui/lv_watch/comm_func/hijri_calendar_func.c \
	apps/common/ui/lv_watch/comm_func/sleep_func.c \
	apps/common/ui/lv_watch/comm_func/weather_func.c \
	apps/common/ui/lv_watch/comm_func/hr_func.c \
	apps/common/ui/lv_watch/comm_func/bo_func.c \
	apps/common/ui/lv_watch/comm_func/alarm_func.c \
	apps/common/ui/lv_watch/comm_func/dnd_func.c \
	apps/common/ui/lv_watch/comm_func/find_func.c \
	apps/common/ui/lv_watch/comm_func/user_info_func.c \
	apps/common/ui/lv_watch/comm_func/gomore_func.c \
	apps/common/ui/lv_watch/comm_func/dev_bond_func.c \
	apps/common/ui/lv_watch/comm_func/sedentary_func.c \
	apps/common/ui/lv_watch/comm_func/camera_func.c \
	apps/common/ui/lv_watch/comm_func/remote_music_func.c \
	apps/common/ui/lv_watch/comm_func/dev_op_func.c \
	apps/common/ui/lv_watch/comm_func/data_mana_func.c \
	apps/common/ui/lv_watch/comm_func/quran_play_func.c \
	apps/common/ui/lv_watch/comm_func/tws_func.c \
	apps/common/ui/lv_watch/pedo_data/pedo_data.c \
	apps/common/ui/lv_watch/comm_task/comm_task.c \
	apps/common/ui/lv_watch/comm_task/utc_time_task.c \
	apps/common/ui/lv_watch/comm_sensor/sensor_hr_task.c \
	apps/common/ui/lv_watch/comm_sensor/sensor_gs_task.c \
	apps/common/ui/lv_watch/comm_sensor/sensor_gm_task.c \
	apps/common/ui/lv_watch/ui_translate/ui_translate.c \
	apps/common/ui/lv_watch/ui_cover/ui_cover.c \
	apps/common/ui/lv_watch/ui_tileview/ui_tileview.c \
	apps/common/ui/lv_watch/ui_menu/menu/style_00.c \
	apps/common/ui/lv_watch/ui_menu/menu/style_01.c \
	apps/common/ui/lv_watch/ui_menu/menu/style_02.c \
	apps/common/ui/lv_watch/ui_menu/weather/weather_data.c \
	apps/common/ui/lv_watch/ui_menu/weather/weather_report.c \
	apps/common/ui/lv_watch/ui_menu/device_op/device_op.c \
	apps/common/ui/lv_watch/ui_menu/sedentary/sedentary.c \
	apps/common/ui/lv_watch/ui_menu/off_wrist/off_wrist.c \
	apps/common/ui/lv_watch/ui_menu/low_battery/low_battery.c \
	apps/common/ui/lv_watch/ui_menu/scrollbar/scrollbar.c \
	apps/common/ui/lv_watch/ui_menu/kaaba/gm_cali.c \
	apps/common/ui/lv_watch/ui_menu/kaaba/kaaba_position.c \
	apps/common/ui/lv_watch/ui_menu/kaaba/kaaba_qibla.c \
	apps/common/ui/lv_watch/ui_menu/set/set_main.c \
	apps/common/ui/lv_watch/ui_menu/sound_ctrl/sound_ctrl.c \
	apps/common/ui/lv_watch/ui_menu/menu_view/menu_view.c \
	apps/common/ui/lv_watch/ui_menu/watchface/watchface_00.c \
	apps/common/ui/lv_watch/ui_menu/watchface/watchface_01.c \
	apps/common/ui/lv_watch/ui_menu/watchface/watchface_02.c \
	apps/common/ui/lv_watch/ui_menu/watchface/watchface_03.c \
	apps/common/ui/lv_watch/ui_menu/watchface/watchface_04.c \
	apps/common/ui/lv_watch/ui_menu/watchface/watchface_05.c \
	apps/common/ui/lv_watch/ui_menu/watchface/watchface_06.c \
	apps/common/ui/lv_watch/ui_menu/watchface/watchface_07.c \
	apps/common/ui/lv_watch/ui_menu/watchface/watchface_08.c \
	apps/common/ui/lv_watch/ui_menu/watchface/watchface_09.c \
	apps/common/ui/lv_watch/ui_menu/watchface/watchface_10.c \
	apps/common/ui/lv_watch/ui_menu/watchface/watchface_11.c \
	apps/common/ui/lv_watch/ui_menu/watchface/watchface_main.c \
	apps/common/ui/lv_watch/ui_menu/tool_box/tool_box.c \
	apps/common/ui/lv_watch/ui_menu/tool_box/not_conn.c \
	apps/common/ui/lv_watch/ui_menu/tool_box/find_phone.c \
	apps/common/ui/lv_watch/ui_menu/lang_sel/lang_sel.c \
	apps/common/ui/lv_watch/ui_menu/disp_ctrl/backlight.c \
	apps/common/ui/lv_watch/ui_menu/disp_ctrl/screen_sleep.c \
	apps/common/ui/lv_watch/ui_menu/unit_switch/unit_switch.c \
	apps/common/ui/lv_watch/ui_menu/about/about.c \
	apps/common/ui/lv_watch/ui_menu/reset/reset.c \
	apps/common/ui/lv_watch/ui_menu/find_dev/find_dev.c \
	apps/common/ui/lv_watch/ui_menu/split_screen/split_screen.c \
	apps/common/ui/lv_watch/ui_menu/call/call_main.c \
	apps/common/ui/lv_watch/ui_menu/call/call_disc.c \
	apps/common/ui/lv_watch/ui_menu/call/contacts.c \
	apps/common/ui/lv_watch/ui_menu/call/call_log.c \
	apps/common/ui/lv_watch/ui_menu/call/call_dial.c \
	apps/common/ui/lv_watch/ui_menu/call/call_out.c \
	apps/common/ui/lv_watch/ui_menu/call/call_in.c \
	apps/common/ui/lv_watch/ui_menu/call/call_online.c \
	apps/common/ui/lv_watch/ui_menu/call/call_volume.c \
	apps/common/ui/lv_watch/ui_menu/call/call_end.c \
	apps/common/ui/lv_watch/ui_menu/message/msg_list.c \
	apps/common/ui/lv_watch/ui_menu/message/msg_detail.c \
	apps/common/ui/lv_watch/ui_menu/camera/camera.c \
	apps/common/ui/lv_watch/ui_menu/countdown/countdown_time.c \
	apps/common/ui/lv_watch/ui_menu/countdown/countdown_state.c \
	apps/common/ui/lv_watch/ui_menu/countdown/countdown_over.c \
	apps/common/ui/lv_watch/ui_menu/stopwatch/stopwatch_main.c \
	apps/common/ui/lv_watch/ui_menu/stopwatch/stopwatch_state.c \
	apps/common/ui/lv_watch/ui_menu/stopwatch/stopwatch_over.c \
	apps/common/ui/lv_watch/ui_menu/hr/hr_sample.c \
	apps/common/ui/lv_watch/ui_menu/bo/bo_sample.c \
	apps/common/ui/lv_watch/ui_menu/tasbih/tasbih_main.c \
	apps/common/ui/lv_watch/ui_menu/tasbih/tasbih_start_time.c \
	apps/common/ui/lv_watch/ui_menu/tasbih/tasbih_end_time.c \
	apps/common/ui/lv_watch/ui_menu/tasbih/tasbih_inv_time.c \
	apps/common/ui/lv_watch/ui_menu/tasbih/tasbih_days.c \
	apps/common/ui/lv_watch/ui_menu/tasbih/tasbih_remind.c \
	apps/common/ui/lv_watch/ui_menu/hijri_calendar/Gcalendar_main.c \
	apps/common/ui/lv_watch/ui_menu/hijri_calendar/Hcalendar_main.c \
	apps/common/ui/lv_watch/ui_menu/hijri_calendar/Hcalendar_modify.c \
	apps/common/ui/lv_watch/ui_menu/hijri_calendar/Hcalendar_remind.c \
	apps/common/ui/lv_watch/ui_menu/hijri_calendar/Hcalendar_festivals.c \
	apps/common/ui/lv_watch/ui_menu/sleep/sleep_main.c \
	apps/common/ui/lv_watch/ui_menu/sleep/sleep_detail.c \
	apps/common/ui/lv_watch/ui_menu/alarm/alarm_main.c \
	apps/common/ui/lv_watch/ui_menu/alarm/alarm_time.c \
	apps/common/ui/lv_watch/ui_menu/alarm/alarm_repeat.c \
	apps/common/ui/lv_watch/ui_menu/alarm/alarm_modify.c \
	apps/common/ui/lv_watch/ui_menu/alarm/alarm_remind.c \
	apps/common/ui/lv_watch/ui_menu/more/more_menu.c \
	apps/common/ui/lv_watch/ui_menu/remote_music/remote_music.c \
	apps/common/ui/lv_watch/ui_menu/prayer_time/prayer_time_main.c \
	apps/common/ui/lv_watch/ui_menu/prayer_time/prayer_time_popup.c \
	apps/common/ui/lv_watch/ui_menu/prayer_time/prayer_time_preview.c \
	apps/common/ui/lv_watch/ui_menu/prayer_time/prayer_time_remind.c \
	apps/common/ui/lv_watch/ui_menu/prayer_time/prayer_time_advance.c \
	apps/common/ui/lv_watch/ui_menu/dev_bond/dev_bond.c \
	apps/common/ui/lv_watch/ui_menu/dev_bond/new_bond.c \
	apps/common/ui/lv_watch/ui_menu/allah_name/al_name_ex.c \
	apps/common/ui/lv_watch/ui_menu/allah_name/al_name_list_0.c \
	apps/common/ui/lv_watch/ui_menu/allah_name/al_name_list_1.c \
	apps/common/ui/lv_watch/ui_menu/quran_play/fav_list.c \
	apps/common/ui/lv_watch/ui_menu/quran_play/audio_list.c \
	apps/common/ui/lv_watch/ui_menu/quran_play/quran_list.c \
	apps/common/ui/lv_watch/ui_menu/quran_play/quran_main.c \
	apps/common/ui/lv_watch/ui_menu/quran_play/quran_volume.c \
	apps/common/ui/lv_watch/ui_menu/factory/factory.c \
	apps/common/ui/lv_watch/ui_menu/azkar/azkar_list.c \
	apps/common/ui/lv_watch/ui_menu/azkar/azkar_sec_ar.c \
	apps/common/ui/lv_watch/ui_menu/azkar/azkar_sec_comm.c \
	apps/common/ui/lv_watch/ui_menu/tws/tws_main.c \
	apps/common/ui/lv_watch/ui_menu/tws/tws_disc.c \
	apps/common/ui/lv_watch/ui_menu/tws/tws_list.c \
	apps/common/ui/lv_watch/ui_menu/tws/tws_unpair.c \
	apps/common/ui/lv_watch/ui_menu/tws/scan_timeout.c \
	apps/common/ui/lv_watch/comm_nor_vm/nor_vm_main.c \
	apps/common/ui/lv_watch/comm_nor_vm/nor_vm_contacts.c \
	apps/common/ui/lv_watch/comm_nor_vm/nor_vm_weather.c \
	apps/common/ui/lv_watch/comm_nor_vm/nor_vm_call_log.c \
	apps/common/ui/lv_watch/comm_nor_vm/nor_vm_message.c \
	apps/common/ui/lv_watch/comm_nor_vm/nor_vm_sleep.c \
	apps/common/ui/lv_watch/comm_nor_vm/nor_vm_hr.c \
	apps/common/ui/lv_watch/comm_nor_vm/nor_vm_bo.c \
	apps/common/ui/lv_watch/comm_nor_vm/nor_vm_pedo.c \

# 需要编译的 .S 文件
S_SRC_FILES := \
	apps/watch/sdk_version.z.S \


# 需要编译的 .s 文件
s_SRC_FILES :=


# 需要编译的 .cpp 文件
cpp_SRC_FILES :=


# 需要编译的 .cc 文件
cc_SRC_FILES :=


# 需要编译的 .cxx 文件
cxx_SRC_FILES :=


# 链接参数
LFLAGS := \
	--plugin-opt=-pi32v2-always-use-itblock=false \
	--plugin-opt=-enable-ipra=true \
	--plugin-opt=-pi32v2-merge-max-offset=4096 \
	--plugin-opt=-pi32v2-enable-simd=true \
	--plugin-opt=mcpu=r3 \
	--plugin-opt=-global-merge-on-const \
	--plugin-opt=-inline-threshold=5 \
	--plugin-opt=-inline-max-allocated-size=32 \
	--plugin-opt=-inline-normal-into-special-section=true \
	--plugin-opt=-dont-used-symbol-list=malloc,free,sprintf,printf,puts,putchar \
	--plugin-opt=save-temps \
	--plugin-opt=-pi32v2-enable-rep-memop \
	--plugin-opt=-warn-stack-size=256 \
	--sort-common \
	--plugin-opt=-used-symbol-file=cpu/br28/sdk_used_list.used \
	--plugin-opt=-pi32v2-large-program=true \
	--gc-sections \
	--start-group \
	cpu/br28/liba/cpu.a \
	cpu/br28/liba/system.a \
	cpu/br28/liba/btstack.a \
	cpu/br28/liba/rcsp_stack.a \
	cpu/br28/liba/tme_stack.a \
	cpu/br28/liba/btctrler.a \
	cpu/br28/liba/aec.a \
	cpu/br28/liba/media.a \
	cpu/br28/liba/libepmotion.a \
	cpu/br28/liba/libAptFilt_pi32v2_OnChip.a \
	cpu/br28/liba/libEchoSuppress_pi32v2_OnChip.a \
	cpu/br28/liba/libNoiseSuppress_pi32v2_OnChip.a \
	cpu/br28/liba/libSplittingFilter_pi32v2_OnChip.a \
	cpu/br28/liba/libDelayEstimate_pi32v2_OnChip.a \
	cpu/br28/liba/libSMS_TDE_pi32v2_OnChip.a \
	cpu/br28/liba/libAdaptiveEchoSuppress_pi32v2_OnChip.a \
	cpu/br28/liba/libOpcore_maskrom_pi32v2_OnChip.a \
	cpu/br28/liba/lib_resample_cal.a \
	cpu/br28/liba/lib_resample_fast_cal.a \
	cpu/br28/liba/sbc_eng_lib.a \
	cpu/br28/liba/mp3_dec_lib.a \
	cpu/br28/liba/mp3_decstream_lib.a \
	cpu/br28/liba/mp3_enc_lib.a \
	cpu/br28/liba/wav_dec_lib.a \
	cpu/br28/liba/wma_dec_lib.a \
	cpu/br28/liba/flac_dec_lib.a \
	cpu/br28/liba/ape_dec_lib.a \
	cpu/br28/liba/dts_dec_lib.a \
	cpu/br28/liba/amr_dec_lib.a \
	cpu/br28/liba/opus_dec_lib.a \
	cpu/br28/liba/lib_midi_dec.a \
	cpu/br28/liba/lib_mp2_code.a \
	cpu/br28/liba/lib_adpcm_code.a \
	cpu/br28/liba/lib_adpcm_ima_code.a \
	cpu/br28/liba/speex_enc_lib.a \
	cpu/br28/liba/amr_enc_lib.a \
	cpu/br28/liba/lc3_codec_lib.a \
	cpu/br28/liba/agreement.a \
	cpu/br28/liba/media_app.a \
	cpu/br28/liba/lc3_dec_lib.a \
	cpu/br28/liba/opus_enc_lib.a \
	cpu/br28/liba/SpectrumShow.a \
	cpu/br28/liba/qr_code.a \
	cpu/br28/liba/libjlsp.a \
	cpu/br28/liba/drc.a \
	cpu/br28/liba/lfaudio_plc_lib.a \
	cpu/br28/liba/lib_esco_repair.a \
	cpu/br28/liba/pedometer.a \
	cpu/br28/liba/sleep.a \
	cpu/br28/liba/wrist_bright.a \
	cpu/br28/liba/Preprocess.a \
	cpu/br28/liba/qmc6309/fangkuang.a \
	cpu/br28/liba/gh3018/Algo_Lib/common_pi32v2_clang-4.0.1.a \
	cpu/br28/liba/gh3018/Algo_Lib/hba_pi32v2_clang-4.0.1.a \
	cpu/br28/liba/gh3018/Algo_Lib/hrv_pi32v2_clang-4.0.1.a \
	cpu/br28/liba/gh3018/Algo_Lib/nadt_pi32v2_clang-4.0.1.a \
	cpu/br28/liba/gh3018/Algo_Lib/spo2_pi32v2_clang-4.0.1.a \
	cpu/br28/liba/gh3018/Drv_Lib/Default_CLANG_PI32.a \
	cpu/br28/liba/gomore/libgomore.a \
	cpu/br28/liba/fall_detection.a \
	cpu/br28/liba/motion_detection.a \
	cpu/br28/liba/sedentary.a \
	cpu/br28/liba/p33_algo_motion.a \
	cpu/br28/liba/libjlsp_kws.a \
	cpu/br28/liba/lib_pitch_speed.a \
	cpu/br28/liba/libFFT_pi32v2_OnChip.a \
	cpu/br28/liba/wtg_dec_lib.a \
	cpu/br28/liba/bfilterfun_lib.a \
	cpu/br28/liba/crypto_toolbox_Osize.a \
	cpu/br28/liba/lib_dns.a \
	cpu/br28/liba/compressor.a \
	cpu/br28/liba/crossover_coff.a \
	cpu/br28/liba/limiter.a \
	cpu/br28/liba/update.a \
	cpu/br28/liba/ui_new.a \
	cpu/br28/liba/ui_draw_new.a \
	cpu/br28/liba/font_new.a \
	cpu/br28/liba/res_new.a \
	cpu/br28/liba/ui_flash_vm.a \
	apps/common/pay/upay_aliyun/vendor/upay.a \
	apps/common/pay/upay_aliyun/vendor/libicrypto.a \
	apps/common/pay/upay_aliyun/vendor/libid2.a \
	apps/common/pay/upay_aliyun/vendor/libisst.a \
	apps/common/pay/upay_aliyun/vendor/libkm.a \
	apps/common/pay/upay_aliyun/vendor/libls_alipay.a \
	apps/common/pay/upay_aliyun/vendor/libprov.a \
	apps/common/pay/upay_aliyun/vendor/libalipay_secure_cert.a \
	apps/common/pay/upay_aliyun/vendor/pay.a \
	apps/common/pay/upay_t_head/lib/JL/libalipay_crypto.a \
	apps/common/pay/upay_t_head/lib/JL/libalipay_HAL.a \
	apps/common/pay/upay_t_head/lib/JL/libalipay_IoTpay.a \
	apps/common/pay/upay_t_head/lib/JL/libalipay_secure_cert.a \
	apps/common/pay/upay_t_head/lib/JL/libcJSON.a \
	apps/common/pay/upay_t_head/lib/JL/libalipay_mbedtls.a \
	--end-group \
	-Tcpu/br28/sdk.ld \
	-M=cpu/br28/tools/sdk.map \
	--plugin-opt=mcpu=r3 \
	--plugin-opt=-mattr=+fprev1 \


LIBPATHS := \
	-L$(SYS_LIB_DIR) \


LIBS := \
	$(SYS_LIB_DIR)/libm.a \
	$(SYS_LIB_DIR)/libc.a \
	$(SYS_LIB_DIR)/libm.a \
	$(SYS_LIB_DIR)/libcompiler-rt.a \



c_OBJS    := $(c_SRC_FILES:%.c=%.c.o)
S_OBJS    := $(S_SRC_FILES:%.S=%.S.o)
s_OBJS    := $(s_SRC_FILES:%.s=%.s.o)
cpp_OBJS  := $(cpp_SRC_FILES:%.cpp=%.cpp.o)
cxx_OBJS  := $(cxx_SRC_FILES:%.cxx=%.cxx.o)
cc_OBJS   := $(cc_SRC_FILES:%.cc=%.cc.o)

OBJS      := $(c_OBJS) $(S_OBJS) $(s_OBJS) $(cpp_OBJS) $(cxx_OBJS) $(cc_OBJS)
DEP_FILES := $(OBJS:%.o=%.d)


OBJS      := $(addprefix $(BUILD_DIR)/, $(OBJS))
DEP_FILES := $(addprefix $(BUILD_DIR)/, $(DEP_FILES))


VERBOSE ?= 0
ifeq ($(VERBOSE), 1)
QUITE :=
else
QUITE := @
endif

# 一些旧的 make 不支持 file 函数，需要 make 的时候指定 LINK_AT=0 make
LINK_AT ?= 1

# 表示下面的不是一个文件的名字，无论是否存在 all, clean, pre_build 这样的文件
# 还是要执行命令
# see: https://www.gnu.org/software/make/manual/html_node/Phony-Targets.html
.PHONY: all clean pre_build

# 不要使用 make 预设置的规则
# see: https://www.gnu.org/software/make/manual/html_node/Suffix-Rules.html
.SUFFIXES:

all: pre_build $(OUT_ELF)
	$(info +POST-BUILD)
	$(QUITE) $(RUN_POST_SCRIPT) sdk

pre_build:
	$(info +PRE-BUILD)
	$(QUITE) $(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -D__LD__ -E -P cpu/br28/sdk_used_list.c -o cpu/br28/sdk_used_list.used
	$(QUITE) $(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -D__LD__ -E -P cpu/br28/sdk_ld.c -o cpu/br28/sdk.ld
	$(QUITE) $(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -D__LD__ -E -P cpu/br28/tools/download.c -o $(POST_SCRIPT)
	$(QUITE) $(FIXBAT) $(POST_SCRIPT)
	$(QUITE) $(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -D__LD__ -E -P cpu/br28/tools/isd_config_rule.c -o cpu/br28/tools/isd_config.ini

clean:
	$(QUITE) $(RM) $(OUT_ELF)
	$(QUITE) $(RM) $(BUILD_DIR)



ifeq ($(LINK_AT), 1)
$(OUT_ELF): $(OBJS)
	$(info +LINK $@)
	$(shell $(MKDIR) $(@D))
	$(file >$(OBJ_FILE), $(OBJS))
	$(QUITE) $(LD) -o $(OUT_ELF) @$(OBJ_FILE) $(LFLAGS) $(LIBPATHS) $(LIBS)
else
$(OUT_ELF): $(OBJS)
	$(info +LINK $@)
	$(shell $(MKDIR) $(@D))
	$(QUITE) $(LD) -o $(OUT_ELF) $(OBJS) $(LFLAGS) $(LIBPATHS) $(LIBS)
endif


$(BUILD_DIR)/%.c.o : %.c
	$(info +CC $<)
	$(QUITE) $(MKDIR) $(@D)
	$(QUITE) $(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -MM -MT $@ $< -o $(@:.o=.d)
	$(QUITE) $(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/%.S.o : %.S
	$(info +AS $<)
	$(QUITE) $(MKDIR) $(@D)
	$(QUITE) $(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -MM -MT $@ $< -o $(@:.o=.d)
	$(QUITE) $(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/%.s.o : %.s
	$(info +AS $<)
	$(QUITE) $(MKDIR) $(@D)
	$(QUITE) $(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -MM -MT $@ $< -o $(@:.o=.d)
	$(QUITE) $(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/%.cpp.o : %.cpp
	$(info +CXX $<)
	$(QUITE) $(MKDIR) $(@D)
	$(QUITE) $(CXX) $(CXXFLAGS) $(CFLAGS) $(DEFINES) $(INCLUDES) -MM -MT $@ $< -o $(@:.o=.d)
	$(QUITE) $(CXX) $(CXXFLAGS) $(CFLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/%.cxx.o : %.cxx
	$(info +CXX $<)
	$(QUITE) $(MKDIR) $(@D)
	$(QUITE) $(CXX) $(CXXFLAGS) $(CFLAGS) $(DEFINES) $(INCLUDES) -MM -MT $@ $< -o $(@:.o=.d)
	$(QUITE) $(CXX) $(CXXFLAGS) $(CFLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/%.cc.o : %.cc
	$(info +CXX $<)
	$(QUITE) $(MKDIR) $(@D)
	$(QUITE) $(CXX) $(CXXFLAGS) $(CFLAGS) $(DEFINES) $(INCLUDES) -MM -MT $@ $< -o $(@:.o=.d)
	$(QUITE) $(CXX) $(CXXFLAGS) $(CFLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@

-include $(DEP_FILES)
