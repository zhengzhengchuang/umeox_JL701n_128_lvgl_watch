		media_text_start = .;

	/*
		*(.text._*)
		*(.data._*)
		*(.aec_code)
		*(.aec_const)
		*(.res_code)
		*(.res_const)
		*(.ns_code)
		*(.ns_const)
		*(.fft_code)
		*(.fft_const)
*/

		*(.der_code)
		*(.der_const)
		*(.nlp_code)
		*(.nlp_const)
		*(.qmf_code)
		*(.qmf_const)
		*(.bt_audioplc_sparse_code)
		*(.bt_audioplc_code)
		*(.bt_audioplc_const)
        *(.lf_audioplc_code)
		*(.lf_audioplc_sparse_code)
		*(.lf_audioplc_const)

		. = ALIGN(4);
		*(.opcore_maskrom)

		. = ALIGN(4);
#if (!TCFG_CODE_RUN_RAM_FM_MODE)
        *(.pcm_code)
        *(.pcm_const)
		*(.fm_data_code)
#endif /*(!TCFG_CODE_RUN_RAM_FM_MODE)*/
		*(.g729_code)
		*(.g729_const)
		*(.g726_code)
		*(.g726_const)

		*(.wtg_dec_code)
		*(.wtg_dec_const)
		*(.wtg_dec_sparse_code)
		*(.wtg_dec_sparse_const)
		*(.lib_wtg_dec_code )
		*(.lib_wtg_dec_const)
		*(.wtgv2_code)
		*(.wtgv2_const)
		*(.wtgv2dec_code)
		*(.wtgv2dec_const)
		*(.wtgv2dec_str)
		*(.wtg_decv2_sparse_code)
        *(.bfilt_code)
        . = ALIGN(4);
		*(.mp3_decstream_const)
		*(.mp3_decstream_code)
		*(.mp3_decstream_sparse_code)
		*(.mp3_decstream_sparse_const)

		*(.mp3_dec_sparse_code)
		*(.mp3_dec_sparse_const)

		*(.mp3_dec_code)
		*(.mp3_dec_const)
		*(.mp3_const)
		*(.mp3_code)

		*(.msbc_code)
		*(.msbc_const)
		*(.mty_code)
		*(.mty_const)
		*(.mp3tsy_dec_code)
		*(.mp3tsy_dec_const)
		*(.mp3tsy_dec_sparse_code)
		*(.mp3tsy_dec_sparse_const)
		*(.sbc_code)
		*(.sbc_const)
		*(.sine_code)
		*(.sine_const)
		*(.speex_dec_code)
		*(.speex_dec_const)
		*(.speex_const)
		*(.speex_code)
	    *(.opus_code)
	    *(.opus_const)


        *(.wav_dec_sparse_code)
		*(.wav_dec_sparse_const)
		*(.wav_dec_code)
		*(.wav_dec_const)
	    *(.wav_const)
		*(.wav_code)

		*(.alac_const)
		*(.alac_code)
		*(.alac_dec_code)

		*(.wma_code)
		*(.wma_const)
		*(.wma_dec_code)
		*(.wma_dec_const)
		*(.wma_dec_sparse_code)
		*(.wma_dec_sparse_const)
		*(.wma_decstream_const)
		*(.wma_decstream_code)
		*(.wma_decstream_sparse_code)
		*(.wma_decstream_sparse_const)
		*(.aac_code)
		*(.aac_const)
		*(.amr_dec_sparse_code)
		*(.amr_dec_sparse_const)
		*(.amr_dec_code)
		*(.amr_dec_const)
		*(.amr_code)
		*(.amr_const)
		*(.midi_code)
		*(.midi_const)

	    *(.lfwordnan_const)
	    *(.lfwordana_const)
		*(.lfwordana_code)
		*(.lfwordana_sparse_code)

		*(.ape_dec_sparse_code)
		*(.ape_dec_sparse_const)
		*(.ape_dec_code)
		*(.ape_dec_const)
		*(.ape_code)
		*(.ape_const)
		*(.audio_decoder_code)
		*(.audio_decoder_const)
		*(.cvsd_code)
		*(.cvsd_const)
		*(.dts_dec_ff_const)
		*(.dts_dec_sparse_code)
		*(.dts_dec_sparse_const)
		*(.dts_dec_code)
		*(.dts_dec_const)
		*(.dts_code)
		*(.dts_const)
		*(.flac_dec_sparse_code)
		*(.flac_dec_sparse_const)
		*(.flac_dec_code)
		*(.flac_dec_const)
		*(.flac_const)
		*(.flac_code)
		*(.m4a_dec_code)
		*(.m4a_dec_ff_const)
		*(.m4a_dec_const)
		*(.m4a_code)
		*(.m4a_const)
        *(.mp3_encode_code)
        *(.mp3_encode_const)
		*(.media_device_code)
		*(.media_device_const)
		*(.audio_encoder_code)
		*(.audio_encoder_const)
		*(.mixer_code)
		*(.mixer_const)
#if (AUDIO_STREAM_AT_RAM == 0)
		*(.stream_code)
		*(.stream_const)
#endif/*AUDIO_STREAM_AT_RAM*/
		*(.dec_server_code)
		*(.dec_server_const)
		*(.rec_server_code)
		*(.rec_server_const)
		*(.auto_mute_code)
		*(.auto_mute_const)
		*(.plc_code)
		*(.plc_const)
		*(.wireless_sync_code)
		*(.wireless_sync_const)


		*(.mp3_enc_code)
		*(.mp3_enc_const)
		*(.mp3_enc_sparse_code)
		*(.mp3_enc_sparse_const)
		. = ALIGN(4);
		*(.mp2_encode_code)
		*(.mp2_encode_const)
		*(.mp2_encode_sparse_code)
		*(.mp2_encode_sparse_const)
		. = ALIGN(4);
		*(.adpcm_encode_code)
		*(.adpcm_encode_const)
		*(.adpcm_encode_sparse_code)
		*(.adpcm_encode_sparse_const)
		. = ALIGN(4);


		*(.m4a_dec_sparse_code)
		*(.m4a_dec_sparse_const)

	 	*(.aac_dec_sparse_code)
		*(.aac_dec_sparse_const)
		*(.bt_aac_dec_core_const)
		*(.bt_aac_dec_eng_const)
		*(.bt_aac_dec_eng_code)
		*(.bt_aac_dec_core_code)
		*(.bt_aac_dec_core_sparse_code)


		*(.res_sparse_code)
		*(.ns_sparse_code )
		*(.aec_sparse_code)
		*(.nlp_sparse_code)
		*(.der_sparse_code)
		*(.qmf_sparse_code)

		*(.sms_const);
		*(.sms_sparse_code);
		*(.sms_code);


		*(.resample_cal_code)
		*(.resample_cal_sparse_code)
		*(.resample_cal_const)

		*(.bt_compressor_sparse_const)
		*(.bt_compressor_sparse_code)
		*(.compressor_sparse_code)
		*(.compressor_sparse_const)

		*(.bt_limiter_sparse_const)
		*(.bt_limiter_sparse_code)
		*(.limiter_sparse_code)
		*(.limiter_sparse_const)

		*(.bt_crossOver_sparse_const)
		*(.bt_crossOver_sparse_code)
		*(.crossOver_sparse_code)
		*(.crossOver_sparse_const)

		*(.iap_code)
		*(.iap_const)
		*(.audio_bfilt_code)
		*(.audio_bfilt_const)
		*(.audio_buf_sync_code)
		*(.audio_buf_sync_const)
		*(.audio_dec_app_code)
		*(.audio_dec_app_const)
		*(.audio_dec_app_sync_code)
		*(.audio_dec_app_sync_const)
		*(.audio_dec_app_event_callback)
		*(.audio_dec_app_dec_run)
		*(.audio_dec_app_normal_run)
		*(.audio_dec_tone_event_callback)
		*(.audio_dig_vol_code)
		*(.audio_dig_vol_const)
		*(.audio_echo_reverb_code)
		*(.audio_echo_reverb_const)
        *(.echo_cal_code)
		*(.echo_cal_const)

#if (AUDIO_EFFECTS_REVERB_AT_RAM == 0)
		*(.platereverb_code)
		*(.platereverb_const)
#endif/*AUDIO_EFFECTS_REVERB_AT_RAM*/
		*(.reverb0_code)
		*(.reverb0_const)
		*(.reverb_cal_sparse_code)
		*(.audio_echo_src_code)
		*(.audio_echo_src_const)
		*(.audio_energy_detect_code)
		*(.audio_energy_detect_const)
#if (AUDIO_EFFECTS_EQ_AT_RAM == 0)
		*(.audio_eq_code)
		*(.audio_eq_const)
#endif/*AUDIO_EFFECTS_EQ_AT_RAM*/
#if (AUDIO_EFFECTS_DRC_AT_RAM == 0)
	    *(.audio_drc_code)
		*(.audio_drc_const)
        *(.drc_const)
		*(.drc_code)
		*(.sw_drc_code)
		*(.sw_drc_const)
#endif/*AUDIO_EFFECTS_DRC_AT_RAM*/
		*(.audio_eq_drc_apply_code)
		*(.audio_eq_drc_apply_const)
		*(.audio_equalloudness_code)
		*(.audio_equalloudness_const)
		*(.audio_howling_code)
		*(.audio_howling_const)
        *(.howlings_phf_sparse_code)
#if (AUDIO_EFFECTS_AFC_AT_RAM == 0)
        *(.howlings_phf_code)
		*(.howlings_phf_const)
        /*AFC_Lib_陷波*/
        *(.notchhowling_code)
		*(.notchhowling_const)
		*(.notchhowling_sparse_code)
		*(.notchhowling_sparse_const)
#endif/*AUDIO_EFFECTS_AFC_AT_RAM*/

#if (AUDIO_EFFECTS_NOISEGATE_AT_RAM == 0)
		*(.audio_noisegate_code)
		*(.audio_noisegate_const)
		/*Lib_NoiseGate*/
		*(.noisegate_code)
		*(.noisegate_const)
		*(.noisegate_sparse_code)
		*(.noisegate_sparse_const)
#endif/*AUDIO_EFFECTS_NOISEGATE_AT_RAM*/

		*(.audio_output_dac_code)
		*(.audio_output_dac_const)
		*(.audio_pemafrow_code)
		*(.audio_pemafrow_const)
		*(.audio_pitch_code)
		*(.audio_pitch_const)
		*(.audio_pitchspeed_code)
		*(.audio_pitchspeed_const)
		*(.audio_surround_code)
		*(.audio_surround_const)
		
		*(.audio_vocal_remove_code)
		*(.audio_vocal_remove_const)
		*(.audio_vocal_tract_synthesis_code)
		*(.audio_vocal_tract_synthesis_const)
		*(.eq_config_code)
		*(.eq_config_const)
		*(.eq_config_protocol_code)
		*(.eq_config_protocol_const)
		*(.spectrum_eq_code)
		*(.spectrum_eq_const)
		*(.spectrum_fft_code)
		*(.spectrum_fft_const)
		*(.audio_localtws_code)
		*(.audio_localtws_const)
		*(.audio_dec_localtws_code)
		*(.audio_dec_localtws_const)
		*(.audio_dec_a2dp_code)
		*(.audio_dec_a2dp_const)
		*(.audio_dec_esco_code)
		*(.audio_dec_esco_const)
		*(.audio_dec_file_code)
		*(.audio_dec_file_const)
#if (!TCFG_CODE_RUN_RAM_FM_MODE)
		*(.audio_dec_pcm_code)
		*(.audio_dec_pcm_const)
#endif /*(!TCFG_CODE_RUN_RAM_FM_MODE)*/
		*(.audio_resample_code)
		*(.audio_resample_const)
		*(.audio_splicing_code)
		*(.audio_splicing_const)
		*(.channel_switch_code)
		*(.channel_switch_const)
		
		*(.aptx_decoder_code)
		*(.aptx_decoder_const)
		*(.ldac_decoder_code)
		*(.ldac_decoder_const)
		*(.speex_encoder_code)
		*(.speex_encoder_const)
		*(.sbc_encoder_code)
		*(.sbc_encoder_const)
		*(.sbc_eng_code)
		*(.sbc_eng_const)
		*(.opus_encoder_code)
		*(.opus_encoder_const)
		*(.prevent_task_fill_code)
		*(.prevent_task_fill_const)
		*(.audio_cfifo_code)
		*(.audio_cfifo_const)
#if (!TCFG_CODE_RUN_RAM_FM_MODE)
		*(.audio_track_code)
		*(.audio_track_const)
#endif /*(!TCFG_CODE_RUN_RAM_FM_MODE)*/
		*(.audio_adc_code)
		*(.audio_adc_const)
		*(.audio_dac_code)
		*(.audio_dac_const)
		*(.audio_linein_code)
		*(.audio_linein_const)
		*(.audio_link_code)
		*(.audio_link_const)
		*(.audio_sample_sync_code)
		*(.audio_sample_sync_const)
		*(.audio_wireless_sync_code)
		*(.audio_wireless_sync_const)
		*(.audio_spdif_code)
		*(.audio_spdif_const)
		*(.audio_src_code)
		*(.audio_src_const)
		*(.audio_src_base_code)
		*(.audio_src_base_const)
		*(.effectrs_sync_code)
		*(.effectrs_sync_const)
		*(.eq_design_code)
		*(.eq_design_const)
		*(.hw_eq_code)
		*(.hw_eq_const)
		*(.iic_code)
		*(.iic_const)
		*(.mic_cap_code)
		*(.mic_cap_const)
		*(.sbc_hwaccel_code)
		*(.sbc_hwaccel_const)
#if (AUDIO_EFFECTS_GAIN_AT_RAM == 0)
		*(.audio_gain_code)
		*(.audio_gain_const)
#endif

		. = ALIGN(4);
    	_audio_decoder_begin = .;
    	PROVIDE(audio_decoder_begin = .);
    	    KEEP(*(.audio_decoder))
    	_audio_decoder_end = .;
    	PROVIDE(audio_decoder_end = .);

    	_audio_encoder_begin = .;
    	PROVIDE(audio_encoder_begin = .);
    	    KEEP(*(.audio_encoder))
    	_audio_encoder_end = .;
    	PROVIDE(audio_encoder_end = .);

    	_audio_package_begin = .;
    	PROVIDE(audio_package_begin = .);
    	    KEEP(*(.audio_package))
    	_audio_package_end = .;
    	PROVIDE(audio_package_end = .);

    	_audio_dev_begin = .;
    	PROVIDE(audio_dev_begin = .);
    	    KEEP(*(.audio_device))
    	_audio_dev_end = .;
    	PROVIDE(audio_dev_end = .);

    	_audio_hwaccel_begin = .;
    	PROVIDE(audio_hwaccel_begin = .);
    	    KEEP(*(.audio_hwaccel))
    	_audio_hwaccel_end = .;
    	PROVIDE(audio_hwaccel_end = .);

#if ((defined CONFIG_SOUND_PLATFORM_ENABLE) && CONFIG_SOUND_PLATFORM_ENABLE)
        _sound_platform_driver_begin = .;
        PROVIDE(sound_platform_driver_begin = .);
            KEEP(*(.sound_platform_driver))
        _sound_platform_driver_end = .;
        PROVIDE(sound_platform_driver_end = .);
#endif /*((defined CONFIG_SOUND_PLATFORM_ENABLE) && CONFIG_SOUND_PLATFORM_ENABLE)*/

		. = ALIGN(4);
    	media_code_begin = .;
    	    *(.media.*.text)

		. = ALIGN(4);
		*(.crossOver_code)
		*(.crossOver_const)
		*(.bt_crossOver_const)
		*(.bt_crossOver_code)
		. = ALIGN(4);

		*(.compressor_code)
		*(.compressor_const)
		*(.bt_compressor_const)
		*(.bt_compressor_code)
		. = ALIGN(4);

		*(.limiter_code)
		*(.limiter_const)
		*(.bt_limiter_const)
		*(.bt_limiter_code)
				*(.drc_sparse_code)
		*(.drc_sparse_const)
        *(.lc3_decoder_const)
        *(.lc3_decoder_code)
        *(.lc3_dec_const)
        *(.lc3_dec_code)
	    *(.lc3_codec_c_const)
	    *(.lc3_codec_c_code)
	    *(.lc3_codec_ari_c_const)
	    *(.lc3_codec_ari_c_code)
	    *(.lc3_codec_d_code)
	    *(.lc3_codec_e_code)
	    *(.lc3_codec_c_sparse_code)
	    *(.lc3_codec_t8k_code)
	    *(.lc3_codec_t16k_code)
	    *(.lc3_codec_t24k_code)
	    *(.lc3_codec_t32k_code)
	    *(.lc3_codec_t441k_code)

		. = ALIGN(4);
		*(.lib_pitchshift_code)
		*(.lib_pitchshift_const)
		. = ALIGN(4);
		*(.sur_cal_const)
		*(.sur_cal_code)
		. = ALIGN(4);
		*(.reverb_cal_sparse_code)
        . = ALIGN(4);
		*(.dvol_ram_code)
        . = ALIGN(4);
		*(.reverb_cal_const)
        . = ALIGN(4);
	    *(.reverb_cal_code)
        . = ALIGN(4);
	
		/* 把下面的段放入ram中，能加速数据流处理 */
#if (!TCFG_CODE_RUN_RAM_FM_MODE)
		*(.stream_code_nor_run)			// 数据流普通输出处理
		*(.mixer_code_nor_run)			// mixer数据流直通输出
		*(.audio_codec_code)			// 解码任务流程
		*(.audio_codec_code_read_file)	// 解码读文件
		*(.audio_src_code_nor_run)		// audio_src运行处理
		*(.src_base_code_nor_run)		// src_base运行处理
		*(.audio_dac_code_nor_run)		// audio_dac普通输出运行处理
		*(.audio_cfifo_code_nor_run)	// audio_cfifo普通运行处理
#endif /*(!TCFG_CODE_RUN_RAM_FM_MODE)*/

		*(.stream_code_copy_run)		// 数据流分支输出处理
		*(.mixer_code_out_run)			// mixer数据流叠加后输出
		
		*(.audio_codec_code_frame_file)	// 解码读流数据

		*(.buf_sync_code_nor_run)		// buf_sync采样率实时调整处理

		*(.resample_fastcal_const)
		*(.resample_fastcal_code)
		*(.resample_fastcal_sparse_code)

#if (AUDIO_EFFECTS_DYNAMIC_EQ_AT_RAM == 0)
		*(.dynamic_eq_code)
		*(.dynamic_eq_const)
	    *(.dynamic_eq_detect_code)
		*(.dynamic_eq_detect_const)
		*(.dynamic_eq_sparse_code)
		*(.dynamic_eq_sparse_const)
	    *(.dynamic_eq_detect_sparse_code)
		*(.dynamic_eq_detect_sparse_const)

#endif/*AUDIO_EFFECTS_DYNAMIC_EQ_AT_RAM*/

#if (AUDIO_EFFECTS_MIC_EFFECT_AT_RAM == 0)
		*(.audio_mic_effect_const)
		*(.audio_mic_effect_code)
#endif/*AUDIO_EFFECTS_MIC_EFFECT_AT_RAM*/
#if (AUDIO_EFFECTS_MIC_STREAM_AT_RAM == 0)
		*(.audio_mic_stream_const)
		*(.audio_mic_stream_code)
#endif/*AUDIO_EFFECTS_MIC_STREAM_AT_RAM*/
#if (AUDIO_EFFECTS_DEBUG_AT_RAM == 0)
		*(.audio_effect_adj_const)
		*(.audio_effect_adj_code)
#endif/*AUDIO_EFFECTS_DEBUG_AT_RAM*/

	    *(.pitchshifter_code)
	    *(.pitchshifter_const)
        *(.pitchshifter_sparse_code)

#if (AUDIO_EFFECTS_VBASS_AT_RAM == 0)
		*(.audio_vbass_code)
		*(.audio_vbass_const)
		*(.vbss_code)
		*(.vbss_const)
        *(.vbss_sparse_code)
		*(.vbss_sparse_const)
#endif/*AUDIO_EFFECTS_VBASS_AT_RAM*/

        *(.specshow_code)
        *(.specshow_const)
        *(.specshow_sparse_code)
        *(.specshow_sparse_const)

        *(.src_ram_code)

		*(.media.audio*)
		*(.media.mixer*)
		*(.media.media*)


		/*CVP*/
        *(.bark_const)
        . = ALIGN(4);
        *(.jlsp_code)
        . = ALIGN(4);
        *(.jlsp_const)
        . = ALIGN(4);
        *(.dns_16k_data)
        . = ALIGN(4);
        *(.dns_8k_data)

		aec_code_begin  = . ;
		*(.text._*)
		*(.aec_code)
		*(.aec_const)
		*(.res_code)
		*(.res_const)
		*(.ns_code)
		*(.ns_const)
		*(.fft_code)
		*(.fft_const)
		aec_code_end = . ;
		aec_code_size = aec_code_end - aec_code_begin ;


    	audio_sync_code_begin = .;
        *(.audio_sync_code)
    	audio_sync_code_end = .;

        . = ALIGN(4);

    	media_code_end = .;
		. = ALIGN(4);
		media_code_size = media_code_end - media_code_begin;

		. = ALIGN(4);
        media_text_end = .;

        media_code_total_size = media_text_end - media_text_start;





