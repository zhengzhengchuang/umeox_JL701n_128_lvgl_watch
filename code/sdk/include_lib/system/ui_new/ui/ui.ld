
SECTIONS
{
    .data : ALIGN(4)
    {
        system_ui_font_data_start = .;
        *(.font_Arabic.data)
        *(.font_ascii.data)
        *(.font_big5.data)
        *(.font_gbk.data)
        *(.font_Hebrew.data)
        *(.font_ksc.data)
        *(.font_other_language.data)
        *(.font_sdfs.data)
        *(.font_sjis.data)
        *(.font_textout.data)
        *(.font_thai.data)
        *(.font_utf8_2_unicode.data)
        system_ui_font_data_end = .;
        system_ui_font_data_total_size = system_ui_font_data_end - system_ui_font_data_start;

        system_ui_framework_data_start = .;
        *(.layer.data)
        *(.layout.data)
        *(.lyrics.data)
        *(.ui_battery.data)
        *(.ui_button.data)
        *(.ui_core.data)
        *(.ui_core_api.data)
        *(.ui_core_dot.data)
        *(.ui_file_attrs.data)
        *(.ui_file_browser.data)
        *(.ui_grid.data)
        *(.ui_number.data)
        *(.ui_p.data)
        *(.ui_pic.data)
        *(.ui_progress.data)
        *(.ui_progress_multi.data)
        *(.ui_rotatable_pic.data)
        *(.ui_rotate.data)
        *(.ui_slider.data)
        *(.ui_slider_vert.data)
        *(.ui_text.data)
        *(.ui_time.data)
        *(.ui_watch.data)
        *(.window.data)
        system_ui_framework_data_end = .;
        system_ui_framework_data_total_size = system_ui_framework_data_end - system_ui_framework_data_start;
        
        system_ui_resource_data_start = .;
        *(.ascii.data)
        *(.memvar.data)
        *(.quicklz.data)
        *(.resfile.data)
        *(.rle.data)
        system_ui_resource_data_end = .;
        system_ui_resource_data_total_size = system_ui_resource_data_end - system_ui_resource_data_start;

        system_ui_timer_data_start = .;
        *(.timer.data)
        system_ui_timer_data_end = .;
        system_ui_timer_data_total_size = system_ui_timer_data_end - system_ui_timer_data_start;

        system_ui_draw_data_start = .;
        *(.buffer_manager.data)
        *(.image_process.data)
        *(.ui_bar_chart.data)
        *(.ui_circle.data)
        *(.ui_figure.data)
        *(.ui_wave.data)
        system_ui_draw_data_end = .;
        system_ui_draw_data_total_size = system_ui_draw_data_end - system_ui_draw_data_start;

        system_ui_simple_data_start = .;
        *(.ap_res.data)
        system_ui_simple_data_end = .;
        system_ui_simple_data_total_size = system_ui_simple_data_end - system_ui_simple_data_start;

        system_ui_vm_data_start = .;
        *(.ui_vm_common.data)
        system_ui_vm_data_end = .;
        system_ui_vm_data_total_size = system_ui_vm_data_end - system_ui_vm_data_start;

        system_lcd_device_data_start = .;
   	    lcd_device_begin = .;
        KEEP(*(.lcd_device_info))
        lcd_device_end = .;
        system_lcd_device_data_end = .;
        system_lcd_device_data_total_size = system_lcd_device_data_end - system_lcd_device_data_start;

        . = ALIGN(4);
    } > ram0

    .bss (NOLOAD) :ALIGN(4)
    {
        system_ui_font_data_bss_start = .;
        *(.font_Arabic.data.bss)
        *(.font_ascii.data.bss)
        *(.font_big5.data.bss)
        *(.font_gbk.data.bss)
        *(.font_Hebrew.data.bss)
        *(.font_ksc.data.bss)
        *(.font_other_language.data.bss)
        *(.font_sdfs.data.bss)
        *(.font_sjis.data.bss)
        *(.font_textout.data.bss)
        *(.font_thai.data.bss)
        *(.font_utf8_2_unicode.data.bss)
        system_ui_font_data_bss_end = .;
        system_ui_font_data_bss_total_size = system_ui_font_data_bss_end - system_ui_font_data_bss_start;
        
        system_ui_framework_data_bss_start = .;
        *(.layer.data.bss)
        *(.layout.data.bss)
        *(.lyrics.data.bss)
        *(.ui_battery.data.bss)
        *(.ui_button.data.bss)
        *(.ui_core.data.bss)
        *(.ui_core_api.data.bss)
        *(.ui_core_dot.data.bss)
        *(.ui_file_attrs.data.bss)
        *(.ui_file_browser.data.bss)
        *(.ui_grid.data.bss)
        *(.ui_number.data.bss)
        *(.ui_p.data.bss)
        *(.ui_pic.data.bss)
        *(.ui_progress.data.bss)
        *(.ui_progress_multi.data.bss)
        *(.ui_rotatable_pic.data.bss)
        *(.ui_rotate.data.bss)
        *(.ui_slider.data.bss)
        *(.ui_slider_vert.data.bss)
        *(.ui_text.data.bss)
        *(.ui_time.data.bss)
        *(.ui_watch.data.bss)
        *(.window.data.bss)
        system_ui_framework_data_bss_end = .;
        system_ui_framework_data_bss_total_size = system_ui_framework_data_bss_end - system_ui_framework_data_bss_start;

        system_ui_resource_data_bss_start = .;
        *(.ascii.data.bss)
        *(.mem_var.data.bss)
        *(.quicklz.data.bss)
        *(.resfile.data.bss)
        *(.rle.data.bss)
        system_ui_resource_data_bss_end = .;
        system_ui_resource_data_bss_total_size = system_ui_resource_data_bss_end - system_ui_resource_data_bss_start;

        system_ui_timer_data_bss_start = .;
        *(.timer.data.bss)
        system_ui_timer_data_bss_end = .;
        system_ui_timer_data_bss_total_size = system_ui_timer_data_bss_end - system_ui_timer_data_bss_start;

        system_ui_draw_data_bss_start = .;
        *(.buffer_manager.data.bss)
        *(.image_process.data.bss)
        *(.ui_bar_chart.data.bss)
        *(.ui_circle.data.bssa)
        *(.ui_figure.data.bss)
        *(.ui_wave.data.bss)
        system_ui_draw_data_bss_end = .;
        system_ui_draw_data_bss_total_size = system_ui_draw_data_bss_end - system_ui_draw_data_bss_start;

        system_ui_simple_data_bss_start = .;
        *(.ap_res.data.bss)
        system_ui_simple_data_bss_end = .;
        system_ui_simple_data_bss_total_size = system_ui_simple_data_bss_end - system_ui_simple_data_bss_start;

        system_ui_vm_data_bss_start = .;
        *(.ui_vm_common.data.bss)
        system_ui_vm_data_bss_end = .;
        system_ui_vm_data_bss_total_size = system_ui_vm_data_bss_end - system_ui_vm_data_bss_start;
        . = ALIGN(4);
    } > ram0

    .text : ALIGN(4)
    {
        system_ui_font_text_start = .;
        *(.ui_ram)
        *(.font_Arabic.text)
        *(.font_Arabic.text.const)
        *(.font_ascii.text)
        *(.font_ascii.text.const)
        *(.font_big5.text)
        *(.font_big5.text.const)
        *(.font_gbk.text)
        *(.font_gbk.text.const)
        *(.font_Hebrew.text)
        *(.font_Hebrew.text.const)
        *(.font_ksc.text)
        *(.font_ksc.text.const)
        *(.font_other_language.text)
        *(.font_other_language.text.const)
        *(.font_sdfs.text)
        *(.font_sdfs.text.const)
        *(.font_sjis.text)
        *(.font_sjis.text.const)
        *(.font_textout.text)
        *(.font_textout.text.const)
        *(.font_thai.text)
        *(.font_thai.text.const)
        *(.font_utf8_2_unicode.text)
        *(.font_utf8_2_unicode.text.const)
        system_ui_font_text_end = .;
        system_ui_font_text_total_size = system_ui_font_text_end - system_ui_font_text_start;

        system_ui_framework_text_start = .;
        *(.layer.text)
        *(.layout.text)
        *(.lyrics.text)
        *(.ui_battery.text)
        *(.ui_button.text)
        *(.ui_core.text)
        *(.ui_core_api.text)
        *(.ui_core_dot.text)
        *(.ui_file_attrs.text)
        *(.ui_file_browser.text)
        *(.ui_grid.text)
        *(.ui_number.text)
        *(.ui_p.text)
        *(.ui_pic.text)
        *(.ui_progress.text)
        *(.ui_progress_multi.text)
        *(.ui_rotatable_pic.text)
        *(.ui_rotate.text)
        *(.ui_slider.text)
        *(.ui_slider_vert.text)
        *(.ui_text.text)
        *(.ui_time.text)
        *(.ui_watch.text)
        *(.window.text)
        *(.layer.text.const)
        *(.layout.text.const)
        *(.lyrics.text.const)
        *(.ui_battery.text.const)
        *(.ui_button.text.const)
        *(.ui_core.text.const)
        *(.ui_core_api.text.const)
        *(.ui_core_dot.text.const)
        *(.ui_file_attrs.text.const)
        *(.ui_file_browser.text.const)
        *(.ui_grid.text.const)
        *(.ui_number.text.const)
        *(.ui_p.text.const)
        *(.ui_pic.text.const)
        *(.ui_progress.text.const)
        *(.ui_progress_multi.text.const)
        *(.ui_rotatable_pic.text.const)
        *(.ui_rotate.text.const)
        *(.ui_slider.text.const)
        *(.ui_slider_vert.text.const)
        *(.ui_text.text.const)
        *(.ui_time.text.const)
        *(.ui_watch.text.const)
        *(.window.text.const)
        system_ui_framework_text_end = .;
        system_ui_framework_text_total_size = system_ui_framework_text_end - system_ui_framework_text_start;

        system_ui_resource_text_start = .;
        *(.ascii.text)
        *(.mem_var.text)
        *(.quicklz.text)
        *(.resfile.text)
        *(.rle.text)
        *(.ascii.text.const)
        *(.memvar.text.const)
        *(.quicklz.text.const)
        *(.resfile.text.const)
        *(.rle.text.const)
        system_ui_resource_text_end = .;
        system_ui_resource_text_total_size = system_ui_resource_text_end - system_ui_resource_text_start;

        system_ui_timer_text_start = .;
        *(.timer.text)
        *(.timer.text.const)
        system_ui_timer_text_end = .;
        system_ui_timer_text_total_size = system_ui_timer_text_end - system_ui_timer_text_start;

        system_ui_draw_text_start = .;
        *(.buffer_manager.text)
        *(.image_process.text)
        *(.ui_bar_chart.text)
        *(.ui_circle.text)
        *(.ui_figure.text)
        *(.ui_wave.text)
        *(.buffer_manager.text.const)
        *(.image_process.text.const)
        *(.ui_bar_chart.text.const)
        *(.ui_circle.text.const)
        *(.ui_figure.text.const)
        *(.ui_wave.text.const)
        system_ui_draw_text_end = .;
        system_ui_draw_text_total_size = system_ui_draw_text_end - system_ui_draw_text_start;

        system_ui_simple_text_start = .;
        *(.ap_res.text)
        *(.ap_res.text.const)
        system_ui_simple_text_end = .;
        system_ui_simple_text_total_size = system_ui_simple_text_end - system_ui_simple_text_start;

        system_ui_vm_text_start = .;
        *(.ui_vm_common.text)
        *(.ui_vm_common.text.const)
        system_ui_vm_text_end = .;
        system_ui_vm_text_total_size = system_ui_vm_text_end - system_ui_vm_text_start;

        . = ALIGN(4);
   	    lcd_interface_begin = .;
        KEEP(*(.lcd_if_info))
        lcd_interface_end = .;


		ui_style_begin = .;
		KEEP(*(.ui_style))
		ui_style_end = .;


        elm_event_handler_begin_JL = .;
        KEEP(*(.elm_event_handler_JL))
		elm_event_handler_end_JL = .;

        elm_event_handler_begin_UPGRADE = .;
        KEEP(*(.elm_event_handler_UPGRADE))
        elm_event_handler_end_UPGRADE = .;



		elm_event_handler_begin_DIAL = .;
        KEEP(*(.elm_event_handler_DIAL))
		elm_event_handler_end_DIAL = .;


		control_event_handler_begin = .;
        KEEP(*(.control_event_handler))
		control_event_handler_end = .;

        control_ops_begin = .;
        KEEP(*(.control_ops))
        control_ops_end = .;

        battery_notify_begin = .;
        *(.battery_notify)
        battery_notify_end = .;

        lcd_sleep_ctrl_headler_begin = .;
        KEEP(*(.lcd_sleep_headler))
        lcd_sleep_ctrl_headler_end = .;

        . = ALIGN(4);
    } > code0
}


