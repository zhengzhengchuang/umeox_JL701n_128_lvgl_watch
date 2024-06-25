#include "app_config.h"


SECTIONS
{
    .text : ALIGN(4)
    {
        ui_menu_load_begin = .;
        KEEP(*(.ui_menu_load))
        ui_menu_load_end = .;

        gsensor_dev_begin = .;
        KEEP(*(.gsensor_dev))
        gsensor_dev_end = .;

        . = ALIGN(4);
        hrsensor_dev_begin = .;
        KEEP(*(.hrsensor_dev))
        hrsensor_dev_end = .;

        imusensor_dev_begin = .;
        KEEP(*(.imusensor_dev))
        imusensor_dev_end = .;


        . = ALIGN(4);
        fm_dev_begin = .;
        KEEP(*(.fm_dev))
        fm_dev_end = .;

        fm_emitter_dev_begin = .;
        KEEP(*(.fm_emitter_dev))
        fm_emitter_dev_end = .;

        storage_device_begin = .;
        KEEP(*(.storage_device))
        storage_device_end = .;

        p11_event_handler_begin = .;
        KEEP(*(.p11_irq_handler))
        p11_event_handler_end = .;

        _watch_syscfg_begin = .;
        PROVIDE(watch_syscfg_begin = .);
        KEEP(*(.watch_syscfg))
        _watch_syscfg_end = .;
        PROVIDE(watch_syscfg_end = .);

		. = ALIGN(4);
        *(.usb.text)
        *(.usb.text.const)

		. = ALIGN(32);
    } > code0

	.data ALIGN(32):
	{
		. = ALIGN(4);
        *(.usb.data)
		. = ALIGN(4);
	} > ram0

	.bss ALIGN(32):
    {
        *(.usb.data.bss.host_mem)  //由于usb有个bug，会导致dma写的数据超出预设的buf，最长可能写超1k，为了避免死机，所以usb dma buffer后面放一些其他模块的buff来避免死机
        *(.usb.data.bss)
#if (!USB_MEM_USE_OVERLAY)
        *(.usb.data.bss.exchange)
#endif
        . = ALIGN(32);
    } > ram0

	.data_code ALIGN(32):
	{
	} > ram0

}
