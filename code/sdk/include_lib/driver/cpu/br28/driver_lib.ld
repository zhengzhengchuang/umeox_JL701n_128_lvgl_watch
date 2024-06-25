SECTIONS
{
    .data : ALIGN(4)
    {
        driver_data_start = .;


        CLOCK_DATA_START = .;
        *(.clock.data)
        CLOCK_DATA_SIZE = ABSOLUTE(. - CLOCK_DATA_START);

        SDMMC_DATA_START = .;
        *(.sdmmc.data)
        SDMMC_DATA_SIZE = ABSOLUTE(. - SDMMC_DATA_START);

        *(.vm_sfc.data)

        *(.timer.data)

        *(.gpio.data)

        *(.debug_data)
        
		*(.uart_data)

        *(.uart_data)
       
        *(.iic_hw.data)
        *(.iic_soft.data)
        
        *(.spi.data)  
        *(.mcpwm.data)  
        *(.efuse.data)  
		
		*(.power_driver.data)
		*(.power_reset.data)
		*(.power_wakeup.data)

        driver_data_end = .;
        . = ALIGN(32);
    } > ram0

    .bss (NOLOAD) :ALIGN(4)
    {
        driver_bss_start = .;

        CLOCK_BSS_START = .;
        *(.clock.data.bss)
        CLOCK_BSS_SIZE = ABSOLUTE(. - CLOCK_BSS_START);

        SDMMC_BSS_START = .;
        *(.sdmmc.data.bss)
        SDMMC_BSS_SIZE = ABSOLUTE(. - SDMMC_BSS_START);

        *(.vm_sfc.data.bss)

        *(.timer.data.bss)

        *(.gpio.data.bss)

        *(.debug_bss)
        *(.power_bss)

        *(.uart_bss)

        *(.sd_var)

        *(.iic_hw.data.bss)
        *(.iic_soft.data.bss)

        *(.spi.data.bss)
        *(.mcpwm.data.bss)
        *(.efuse.data.bss)
        
		*(.power_driver.data.bss)
		*(.power_reset.data.bss)
		*(.power_wakeup.data.bss)
		
		driver_bss_end = .;
    } > ram0

    .text : ALIGN(4)
    {
        driver_code_start = .;

        *(.fm_code)

        CLOCK_CODE_START = .;
        *(.clock.text)
        *(.clock.text.const)
        CLOCK_CODE_SIZE = ABSOLUTE(. - CLOCK_CODE_START);

        SDMMC_CODE_START = .;
        *(.sdmmc.text)
        *(.sdmmc.text.const)
        SDMMC_CODE_SIZE = ABSOLUTE(. - SDMMC_CODE_START);

        *(.vm_sfc.text)
        *(.vm_sfc.text.const)

        *(.timer.text)
        *(.timer.text.const)

#if !(TCFG_LCD_QSPI_ST77903_ENABLE || TCFG_LCD_QSPI_ST77903_V1_ENABLE || TCFG_LCD_QSPI_ST77903_V2_ENABLE || TCFG_LCD_QSPI_ST77903_V3_ENABLE || TCFG_LCD_QSPI_ST77903_V4_ENABLE)
        *(.gpio.text)
        *(.gpio.text.const)
#endif

        *(.debug_code)
        *(.debug_const)

        *(.uart_code)
        *(.uart_const)

        *(.iic_hw.text)
        *(.iic_hw.text.const)

        *(.iic_soft.text)
        *(.iic_soft.text.const)
        
        *(.spi.text)
        *(.spi.text.const)

        *(.mcpwm.text)
        *(.mcpwm.text.const)

        *(.efuse.text)
        *(.efuse.text.const)
		
		*(.power_driver.text)
		*(.power_driver.text.const)

		*(.power_reset.text)
		*(.power_reset.text.const)
		
		*(.power_wakeup.text)
		*(.power_wakeup.text.const)

		. = ALIGN(4);
	    lp_target_begin = .;
	    PROVIDE(lp_target_begin = .);
	    KEEP(*(.lp_target))
	    lp_target_end = .;
	    PROVIDE(lp_target_end = .);

		. = ALIGN(4);
	    lp_request_begin = .;
	    PROVIDE(lp_request_begin = .);
	    KEEP(*(.lp_request))
	    lp_request_end = .;
	    PROVIDE(lp_request_end = .);

		. = ALIGN(4);
        deepsleep_target_begin = .;
        PROVIDE(deepsleep_target_begin = .);
        KEEP(*(.deepsleep_target))
        deepsleep_target_end = .;
        PROVIDE(deepsleep_target_end = .);
	
        driver_code_end = .;
	} > code0

    .data_code : ALIGN(4)
    {
        driver_data_code_start = .;
        #include "driver_lib_data_text.ld"
        driver_data_code_end = .;
		. = ALIGN(4);
    } > ram0

    /*代码统计 Code & RAM */
    DRIVER_RAM_TOTAL   = (driver_data_end - driver_data_start) + (driver_bss_end - driver_bss_start);
    DRIVER_CODE_TOTAL  = (driver_code_end - driver_code_start);
    DRIVER_DATA_CODE_TOTAL  = (driver_data_code_end - driver_data_code_start);
}
