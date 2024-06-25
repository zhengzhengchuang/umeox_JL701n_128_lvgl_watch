// *INDENT-OFF*
/*
/*
#-------------------------------------------------------+
#               br28 RAM mapping                        |
#-------------------------------------------0x1A0000 ---|
#                                             (0x200)   |       INT_ADDR
#-------------------------------------------0X19FE00----|---------------
#              Maskrom_export_RAM             (0x200)   |       0X19FE00
#-------------------------------------------0x19FC00 ---|
#                                                       |
#                                                       |
#              ota-loader              (163K:0x28C00)   |
#                                                       |
#                                                       |
#-------------------------------------------0x177000 ---|
#                                                       |
#                                                       |
#                                                       |
#              (EX_CODE)               (460K:0x73000)   |
#                                                       |
#                                                       |
#                                                       |
#-------------------------------------------0x104000 ---|
#                                                       |
#               uboot                   (16K:0x4000)    |
#                                                       |
#-------------------------------------------0x100000 ---|
*/

#include  "maskrom_stubs.ld"

//from mask export
ISR_BASE       = _IRQ_MEM_ADDR;

UPDATA_SIZE = 0x80;
UPDATE_BEG = _MASK_MEM_BEGIN - UPDATA_SIZE;

UPDATA_BREDR_BASE_BEG = 0x1A0000;

OTA_CODE_BEGIN  = 0x177000;         /*注意不要和uboot重叠*/
OTA_CODE_SIZE   = 162K;

EX_CODE_BEGIN 	= 0x104000;         /*用户自定义UI代码的地址，根据不同CPU来配置*/
EX_CODE_SIZE    = 460K;

MEMORY
{
	text_ram   : ORIGIN = OTA_CODE_BEGIN,  			LENGTH = OTA_CODE_SIZE
	bt_ram     : ORIGIN = UPDATA_BREDR_BASE_BEG,  	LENGTH = 0x10c0
	ex_code    : ORIGIN = EX_CODE_BEGIN,		    LENGTH = EX_CODE_SIZE
	reserved   : ORIGIN = 0x0,						LENGTH = 0x100
	version_ram :ORIGIN = 0x8000000,  				LENGTH = 128

}

ENTRY(_start);
EXTERN(lib_ota_loader_version);

SECTIONS
{
    . = ORIGIN(text_ram);
    .text :
    {
        *(.start*)
        *(.text*)
        *(.data*)
        *(.rodata*)
        *(.bredr_irq_code)
        *(.frame_irq_code)
        *(.classic_irq_code)
        *(.classic_irq_const)
        . = ALIGN(32);
    } > text_ram

    .bss ALIGN(32) (NOLOAD):
    {
        *(.stack*)
        *(.bss)
        *(.usb_ep0)
        *(.usb_config_var)
        *(.update_buf)
        *(.bredr_rxtx_bulk)
        . = ALIGN(32);
    } > text_ram

    .bss_noinit ALIGN(32) (NOLOAD):
    {
        *(.bss_noinit)
        . = ALIGN(32);
    } > text_ram

	. = ORIGIN(bt_ram);
	.bt_bss (NOLOAD) :SUBALIGN(4)
	{
		*(.bd_base)
		*(.comm_rf_para)
		*(.bd_base1)
	} > bt_ram

	. = ORIGIN(ex_code);
	.ex_code (NOLOAD) :SUBALIGN(4)
	{
		. = LENGTH(ex_code);
	} > ex_code

	. = ORIGIN(reserved);
	.ver_tag ALIGN(4):
	{
		KEEP(*(.version_tag1))
		KEEP(*(.version_tag2))
		KEEP(*(.version_tag3))
		. = ALIGN(4);
	} > reserved

	. = ORIGIN(version_ram);
    .version ALIGN(32):
	{
        *(.ota_loader.version)
        . = ALIGN(32);
	} > version_ram

    text_begin  = ADDR(.text);
    text_size   = SIZEOF(.text);
    text_end    = text_begin + text_size;

    bt_ram_begin = ADDR(.bt_bss);
    bt_ram_size  = SIZEOF(.bt_bss);

    bss_begin  = ADDR(.bss);
    bss_size   = SIZEOF(.bss);
}


