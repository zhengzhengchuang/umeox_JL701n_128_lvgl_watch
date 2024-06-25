SECTIONS
{
    .data : ALIGN(4)
    {
		#include "audio_lib_data.ld"
        . = ALIGN(4);
        *(.jlsp_data)
    } > ram0

    .bss (NOLOAD) :ALIGN(4)
    {
		#include "audio_lib_bss.ld"

        *(.jlsp_bss)
    } > ram0

    .text : ALIGN(4)
    {
		#include "audio_lib_text.ld"

        *(.bark_const)
        . = ALIGN(4);
        *(.jlsp_code)
        . = ALIGN(4);
        *(.jlsp_const)
        . = ALIGN(4);
        *(.dns_16k_data)
        . = ALIGN(4);
        *(.dns_8k_data)
    } > code0

	.data_code ALIGN(32):
	{
		#include "audio_lib_data_text.ld"
	} > ram0
}

