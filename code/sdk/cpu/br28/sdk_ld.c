// *INDENT-OFF*
#include "app_config.h"

/* =================  BR28 SDK memory ========================
 _______________ ___ 0x1A0000(176K)
|   isr base    |
|_______________|___ _IRQ_MEM_ADDR(size = 0x100)
|rom export ram |
|_______________|
|    update     |
|_______________|___ RAM_LIMIT_H
|     HEAP      |
|_______________|___ data_code_pc_limit_H
| audio overlay |
|_______________|
|   data_code   |
|_______________|___ data_code_pc_limit_L
|     bss       |
|_______________|
|     data      |
|_______________|
|   irq_stack   |
|_______________|
|   boot info   |
|_______________|
|     TLB       |
|_______________|0 RAM_LIMIT_L

 =========================================================== */

#include "maskrom_stubs.ld"

EXTERN(
_start
#include "sdk_used_list.c"
);

UPDATA_SIZE     = 0x80;
UPDATA_BEG      = _MASK_MEM_BEGIN - UPDATA_SIZE;
UPDATA_BREDR_BASE_BEG = 0x1A0000;

EX_SEC_SIZE     = 0x100;  // 可掉电段的大小(单位:byte)   需要根据实际使用需求配置!

RAM_LIMIT_L     = 0x100000;
RAM_LIMIT_H     = UPDATA_BEG;
PHY_RAM_SIZE    = RAM_LIMIT_H - RAM_LIMIT_L;

//from mask export
ISR_BASE       = _IRQ_MEM_ADDR;
ROM_RAM_SIZE   = _MASK_MEM_SIZE;
ROM_RAM_BEG    = _MASK_MEM_BEGIN;

#ifdef TCFG_LOWPOWER_RAM_SIZE
RAM0_BEG 		= RAM_LIMIT_L + (128K * TCFG_LOWPOWER_RAM_SIZE);
#else
RAM0_BEG 		= RAM_LIMIT_L;
#endif
RAM0_END 		= RAM_LIMIT_H;
RAM0_SIZE 		= RAM0_END - RAM0_BEG;

RAM1_BEG 		= RAM_LIMIT_L;
RAM1_END 		= RAM0_BEG;
RAM1_SIZE 		= RAM1_END - RAM1_BEG;

#ifdef CONFIG_NEW_CFG_TOOL_ENABLE
CODE_BEG        = 0x6000100;
#else
CODE_BEG        = 0x6000120;
#endif

#if ((defined TCFG_PSRAM_DEV_ENABLE) && TCFG_PSRAM_DEV_ENABLE)
PSRAM_BEGIN = 0x2000000;
PSRAM_SIZE = TCFG_PSRAM_SIZE;
PSRAM_END = PSRAM_BEGIN + PSRAM_SIZE;
#endif /* #if ((defined TCFG_PSRAM_DEV_ENABLE) && TCFG_PSRAM_DEV_ENABLE) */

#ifndef CONFIG_EFFECT_CORE_V2_ENABLE
EQ_PRIV_SECTION_NUM = 10;
EQ_SECTION_NUM = EQ_SECTION_MAX;
#endif
//=============== About BT RAM ===================
//CONFIG_BT_RX_BUFF_SIZE = (1024 * 18);

MEMORY
{
	code0(rx)    	  : ORIGIN = CODE_BEG,  LENGTH = CONFIG_FLASH_SIZE
	ram0(rwx)         : ORIGIN = RAM0_BEG,  LENGTH = RAM0_SIZE

    //ram1 - 用于volatile-heap
	//ram1(rwx)         : ORIGIN = RAM1_BEG,  LENGTH = RAM1_SIZE

#if ((defined TCFG_PSRAM_DEV_ENABLE) && TCFG_PSRAM_DEV_ENABLE)
	psram(rwx)        : ORIGIN = PSRAM_BEGIN,  LENGTH = PSRAM_SIZE
#endif /* #if ((defined TCFG_PSRAM_DEV_ENABLE) && TCFG_PSRAM_DEV_ENABLE) */
}


ENTRY(_start)

SECTIONS
{
    . = ORIGIN(code0);
    .text ALIGN(4):
    {
        PROVIDE(text_rodata_begin = .);

        *(.startup.text)

		*(.text)

		. = ALIGN(4);
	    update_target_begin = .;
	    PROVIDE(update_target_begin = .);
	    KEEP(*(.update_target))
	    update_target_end = .;
	    PROVIDE(update_target_end = .);
		. = ALIGN(4);

		*(.LOG_TAG_CONST*)
        *(.rodata*)

		. = ALIGN(4); // must at tail, make rom_code size align 4
        PROVIDE(text_rodata_end = .);

        clock_critical_handler_begin = .;
        KEEP(*(.clock_critical_txt))
        clock_critical_handler_end = .;

        chargestore_handler_begin = .;
        KEEP(*(.chargestore_callback_txt))
        chargestore_handler_end = .;

		. = ALIGN(4);
	    pt_module_begin = .;
	    PROVIDE(pt_module_begin = .);
	    KEEP(*(.pt_module))
	    pt_module_end = .;
	    PROVIDE(pt_module_end = .);

		/********maskrom arithmetic ****/
        *(.opcore_table_maskrom)
        *(.bfilt_table_maskroom)
        *(.opcore_maskrom)
        *(.bfilt_code)
        *(.bfilt_const)
		/********maskrom arithmetic end****/

		. = ALIGN(4);
		__VERSION_BEGIN = .;
        KEEP(*(.sys.version))
        __VERSION_END = .;

        *(.noop_version)
		. = ALIGN(4);

		. = ALIGN(4);
		tool_interface_begin = .;
		KEEP(*(.tool_interface))
		tool_interface_end = .;
        . = ALIGN(4);
        cmd_interface_begin = .;
        KEEP(*(.eff_cmd))
        cmd_interface_end = .;
		. = ALIGN(4);

#if !(TCFG_LCD_QSPI_ST77903_ENABLE || TCFG_LCD_QSPI_ST77903_V1_ENABLE || TCFG_LCD_QSPI_ST77903_V2_ENABLE || TCFG_LCD_QSPI_ST77903_V3_ENABLE || TCFG_LCD_QSPI_ST77903_V4_ENABLE)
        *(.st77903_ram_code)
#endif

		. = ALIGN(32);
		m_code_addr = . ;
		*(.m.code*)
		*(.movable.code*)
			m_code_size = ABSOLUTE(. - m_code_addr) ;
		. = ALIGN(32);
	  } > code0

    . = ORIGIN(ram0);
	//TLB 起始需要16K 对齐；
    .mmu_tlb ALIGN(0x10000):
    {
        *(.mmu_tlb_segment);
    } > ram0

	.boot_info ALIGN(32):
	{
		*(.boot_info)
        . = ALIGN(4);
		*(.debug_record)
        . = ALIGN(4);
	} > ram0

	.irq_stack ALIGN(32):
    {
        _cpu0_sstack_begin = .;
        *(.cpu0_stack)
        _cpu0_sstack_end = .;
		. = ALIGN(4);

        _cpu1_sstack_begin = .;
        *(.cpu1_stack)
        _cpu1_sstack_end = .;
		. = ALIGN(4);

    } > ram0

	.data ALIGN(32):
	{
		//cpu start
        . = ALIGN(4);
        *(.data_magic)

		. = ALIGN(4);
        *(.data*)

		. = ALIGN(4);
	} > ram0

	.bss ALIGN(32):
    {
        . = ALIGN(4);
        *(.bss)
        . = ALIGN(4);
        *(.volatile_ram)

		*(.btstack_pool)

        *(.mem_heap)
		*(.memp_memory_x)

        . = ALIGN(4);
		*(.non_volatile_ram)

        . = ALIGN(32);
    } > ram0

	.data_code ALIGN(32):
	{
		data_code_pc_limit_begin = .;
		*(.flushinv_icache)
        *(.cache)
        *(.os_critical_code)
        *(.os_ram_code)
        *(.volatile_ram_code)
        *(.debug_ram_code)
        *(.hwi_ram_code)
        *(.pwr_trim_ram_code)
        *(.clock_hw_ram_code)
        *(.sfc_spi_ram_code)
        *(.sfc_spi_code)
        *(.norflash_sfc_code)
#if (TCFG_LCD_QSPI_ST77903_ENABLE || TCFG_LCD_QSPI_ST77903_V1_ENABLE || TCFG_LCD_QSPI_ST77903_V2_ENABLE || TCFG_LCD_QSPI_ST77903_V3_ENABLE || TCFG_LCD_QSPI_ST77903_V4_ENABLE)
        *(.st77903_ram_code)
        *(.gpio.text)
        *(.gpio.text.const)
#endif
        *(.chargebox_code)

        *(.ui_ram)
		*(.lvgl_ram)
        *(.math_fast_funtion_code)

		. = ALIGN(4);
        _SPI_CODE_START = . ;
        *(.spi_code)
		. = ALIGN(4);
        _SPI_CODE_END = . ;
		. = ALIGN(4);

	} > ram0

	.moveable_slot ALIGN(4):
	{
	    *(movable.slot.*)

	} >ram0

	__report_overlay_begin = .;
    #include "app_overlay.ld"

	data_code_pc_limit_end = .;
	__report_overlay_end = .;

	_HEAP_BEGIN = . ;
	_HEAP_END = (RAM0_END - EX_SEC_SIZE);

    . = (RAM0_END - EX_SEC_SIZE);
    .reserved_ram ALIGN(4):
    {
	    *(.reserved_segment)
    } > ram0
    _RESERVED_END = .;

#if ((defined TCFG_PSRAM_DEV_ENABLE) && TCFG_PSRAM_DEV_ENABLE)
	. = ORIGIN(psram);
	.ps_ram_data_code ALIGN(32):
	{
	    *(.psram_data)
	    *(.psram_code)
		. = ALIGN(4);
	} > psram

	.ps_ram_bss ALIGN(32):
	{
	    *(.psram_bss)
		. = ALIGN(4);
	} > psram

	.ps_ram_noinit ALIGN(32):
	{
	    *(.psram_noinit)
		. = ALIGN(4);
	} > psram

	_PSRAM_HEAP_BEGIN = .;
	_PSRAM_HEAP_END = PSRAM_END;

#endif /* #if ((defined TCFG_PSRAM_DEV_ENABLE) && TCFG_PSRAM_DEV_ENABLE) */

}

#include "app.ld"
#include "update/update.ld"
#include "btstack/btstack_lib.ld"
#include "system/port/br28/system_lib.ld"
#include "btctrler/port/br28/btctler_lib.ld"
#include "driver/cpu/br28/driver_lib.ld"
#include "media/cpu/br28/audio_lib.ld"
#include "ui_new/ui/ui.ld"
//================== Section Info Export ====================//
text_begin  = ADDR(.text);
text_size   = SIZEOF(.text);
text_end    = text_begin + text_size;
ASSERT((text_size % 4) == 0,"!!! text_size Not Align 4 Bytes !!!");

bss_begin = ADDR(.bss);
bss_size  = SIZEOF(.bss);
bss_end    = bss_begin + bss_size;
ASSERT((bss_size % 4) == 0,"!!! bss_size Not Align 4 Bytes !!!");

data_addr = ADDR(.data);
data_begin = text_begin + text_size;
data_size =  SIZEOF(.data);
ASSERT((data_size % 4) == 0,"!!! data_size Not Align 4 Bytes !!!");

moveable_slot_addr = ADDR(.moveable_slot);
moveable_slot_begin = data_begin + data_size;
moveable_slot_size =  SIZEOF(.moveable_slot);

data_code_addr = ADDR(.data_code);
data_code_begin = moveable_slot_begin + moveable_slot_size;
data_code_size =  SIZEOF(.data_code);
ASSERT((data_code_size % 4) == 0,"!!! data_code_size Not Align 4 Bytes !!!");


//================ OVERLAY Code Info Export ==================//
aec_addr = ADDR(.overlay_aec);
aec_begin = data_code_begin + data_code_size;
aec_size =  SIZEOF(.overlay_aec);

aac_addr = ADDR(.overlay_aac);
aac_begin = aec_begin + aec_size;
aac_size =  SIZEOF(.overlay_aac);

#if ((defined TCFG_PSRAM_DEV_ENABLE) && TCFG_PSRAM_DEV_ENABLE)
ps_ram_data_code_addr = ADDR(.ps_ram_data_code);
ps_ram_data_code_begin = aac_begin + aac_size;
ps_ram_data_code_size =  SIZEOF(.ps_ram_data_code);

ps_ram_bss_addr = ADDR(.ps_ram_bss);
ps_ram_bss_size =  SIZEOF(.ps_ram_bss);

ps_ram_noinit_addr = ADDR(.ps_ram_noinit);
ps_ram_noinit_size =  SIZEOF(.ps_ram_noinit);

ps_ram_size =  PSRAM_SIZE;

PROVIDE(PSRAM_HEAP_BEGIN = _PSRAM_HEAP_BEGIN);
PROVIDE(PSRAM_HEAP_END = _PSRAM_HEAP_END);
_PSRAM_MALLOC_SIZE = _PSRAM_HEAP_END - _PSRAM_HEAP_BEGIN;
PROVIDE(PSRAM_MALLOC_SIZE = _PSRAM_HEAP_END - _PSRAM_HEAP_BEGIN);
#endif /* #if ((defined TCFG_PSRAM_DEV_ENABLE) && TCFG_PSRAM_DEV_ENABLE) */

/*
lc3_addr = ADDR(.overlay_lc3);
lc3_begin = aac_begin + aac_size;
lc3_size = SIZEOF(.overlay_lc3);
*/

/* moveable_addr = ADDR(.overlay_moveable) ; */
/* moveable_size = SIZEOF(.overlay_moveable) ; */
//===================== HEAP Info Export =====================//
ASSERT(_HEAP_BEGIN > bss_begin,"_HEAP_BEGIN < bss_begin");
ASSERT(_HEAP_BEGIN > data_addr,"_HEAP_BEGIN < data_addr");
ASSERT(_HEAP_BEGIN > data_code_addr,"_HEAP_BEGIN < data_code_addr");
/* ASSERT(_HEAP_BEGIN > moveable_slot_addr,"_HEAP_BEGIN < moveable_slot_addr"); */
/* ASSERT(_HEAP_BEGIN > __report_overlay_begin,"_HEAP_BEGIN < __report_overlay_begin"); */

PROVIDE(HEAP_BEGIN = _HEAP_BEGIN);
PROVIDE(HEAP_END = _HEAP_END);
_MALLOC_SIZE = _HEAP_END - _HEAP_BEGIN;
PROVIDE(MALLOC_SIZE = _HEAP_END - _HEAP_BEGIN);

ASSERT(MALLOC_SIZE  >= 0x8000, "heap space too small !")

ASSERT(_RESERVED_END <= UPDATA_BEG, "RESERVED_END <= UPDATA_BEG : reserved_sec overflow !")

//============================================================//
//=== report section info begin:
//============================================================//
report_text_beign = ADDR(.text);
report_text_size = SIZEOF(.text);
report_text_end = report_text_beign + report_text_size;

report_mmu_tlb_begin = ADDR(.mmu_tlb);
report_mmu_tlb_size = SIZEOF(.mmu_tlb);
report_mmu_tlb_end = report_mmu_tlb_begin + report_mmu_tlb_size;

report_boot_info_begin = ADDR(.boot_info);
report_boot_info_size = SIZEOF(.boot_info);
report_boot_info_end = report_boot_info_begin + report_boot_info_size;

report_irq_stack_begin = ADDR(.irq_stack);
report_irq_stack_size = SIZEOF(.irq_stack);
report_irq_stack_end = report_irq_stack_begin + report_irq_stack_size;

report_data_begin = ADDR(.data);
report_data_size = SIZEOF(.data);
report_data_end = report_data_begin + report_data_size;

report_bss_begin = ADDR(.bss);
report_bss_size = SIZEOF(.bss);
report_bss_end = report_bss_begin + report_bss_size;

report_data_code_begin = ADDR(.data_code);
report_data_code_size = SIZEOF(.data_code);
report_data_code_end = report_data_code_begin + report_data_code_size;

report_overlay_begin = __report_overlay_begin;
report_overlay_size = __report_overlay_end - __report_overlay_begin;
report_overlay_end = __report_overlay_end;

report_heap_beign = _HEAP_BEGIN;
report_heap_size = _HEAP_END - _HEAP_BEGIN;
report_heap_end = _HEAP_END;

BR28_PHY_RAM_SIZE = PHY_RAM_SIZE;
BR28_SDK_RAM_SIZE = report_mmu_tlb_size + \
					report_boot_info_size + \
					report_irq_stack_size + \
					report_data_size + \
					report_bss_size + \
					report_overlay_size + \
					report_data_code_size + \
					report_heap_size;
//============================================================//
//=== report section info end
//============================================================//

