

        media_data_code_start = .;
        . = ALIGN(4);
	  	*(.media.aec.text)
#if (TCFG_CODE_RUN_RAM_FM_MODE)
        *(.pcm_code)
        *(.pcm_const)
		*(.fm_data_code)
		*(.audio_dec_pcm_code)
		*(.audio_dec_pcm_const)
		*(.stream_code_nor_run)			// 数据流普通输出处理
		*(.mixer_code_nor_run)			// mixer数据流直通输出
		*(.audio_codec_code)			// 解码任务流程
		*(.audio_codec_code_read_file)	// 解码读文件
		*(.audio_src_code_nor_run)		// audio_src运行处理
		*(.src_base_code_nor_run)		// src_base运行处理
		*(.audio_dac_code_nor_run)		// audio_dac普通输出运行处理
		*(.audio_cfifo_code_nor_run)	// audio_cfifo普通运行处理
		*(.audio_track_code)
		*(.audio_track_const)
#endif /*(TCFG_CODE_RUN_RAM_FM_MODE)*/
        . = ALIGN(4);


 AudioEffects_data_code_begin = .;
#if AUDIO_EFFECTS_REVERB_AT_RAM
		*(.platereverb_code)
		*(.platereverb_const)
#endif/*AUDIO_EFFECTS_REVERB_AT_RAM*/

#if AUDIO_EFFECTS_AFC_AT_RAM
	    *(.howlings_phf_code)
		*(.howlings_phf_const)
		/*AFC_Lib_陷波*/
		*(.notchhowling_code)
		*(.notchhowling_const)
		*(.notchhowling_sparse_code)
		*(.notchhowling_sparse_const)
        . = ALIGN(4);
#endif/*AUDIO_EFFECTS_AFC_AT_RAM*/

#if AUDIO_EFFECTS_DRC_AT_RAM
		*(.audio_drc_code)
		*(.audio_drc_const)
		*(.drc_const)
		*(.drc_code)
	    *(.sw_drc_code)
		*(.sw_drc_const)
        . = ALIGN(4);
#endif/*AUDIO_EFFECTS_DRC_AT_RAM*/

#if AUDIO_EFFECTS_EQ_AT_RAM
		*(.audio_eq_code)
		*(.audio_eq_const)
        . = ALIGN(4);
#endif/*AUDIO_EFFECTS_EQ_AT_RAM*/

#if AUDIO_EFFECTS_NOISEGATE_AT_RAM
		*(.audio_noisegate_code)
		*(.audio_noisegate_const)
		/*Lib_NoiseGate*/
		*(.noisegate_code)
		*(.noisegate_const)
		*(.noisegate_sparse_code)
		*(.noisegate_sparse_const)
#endif/*AUDIO_EFFECTS_NOISEGATE_AT_RAM*/

#if AUDIO_EFFECTS_MIC_EFFECT_AT_RAM
		*(.audio_mic_effect_const)
		*(.audio_mic_effect_code)
#endif/*AUDIO_EFFECTS_MIC_EFFECT_AT_RAM*/

#if AUDIO_EFFECTS_MIC_STREAM_AT_RAM
		*(.audio_mic_stream_const)
		*(.audio_mic_stream_code)
#endif/*AUDIO_EFFECTS_MIC_STREAM_AT_RAM*/

#if AUDIO_EFFECTS_DEBUG_AT_RAM
		*(.audio_effect_adj_const)
		*(.audio_effect_adj_code)
        . = ALIGN(4);
#endif/*AUDIO_EFFECTS_DEBUG_AT_RAM*/

#if AUDIO_EFFECTS_DYNAMIC_EQ_AT_RAM
		*(.dynamic_eq_code)
		*(.dynamic_eq_const)
	    *(.dynamic_eq_detect_code)
		*(.dynamic_eq_detect_const)
        *(.dynamic_eq_sparse_code)
		*(.dynamic_eq_sparse_const)
	    *(.dynamic_eq_detect_sparse_code)
		*(.dynamic_eq_detect_sparse_const)
	    . = ALIGN(4);
#endif/*AUDIO_EFFECTS_DYNAMIC_EQ_AT_RAM*/
#if AUDIO_EFFECTS_VBASS_AT_RAM 
		*(.audio_vbass_code)
		*(.audio_vbass_const)
		*(.vbss_code)
		*(.vbss_const)
	    *(.vbss_sparse_code)
        *(.vbss_sparse_const)
	    . = ALIGN(4);
#endif/*AUDIO_EFFECTS_VBASS_AT_RAM*/

#if AUDIO_STREAM_AT_RAM 
		*(.stream_code)
		*(.stream_const)
#endif/*AUDIO_STREAM_AT_RAM*/
#if AUDIO_EFFECTS_GAIN_AT_RAM
		*(.audio_gain_code)
		*(.audio_gain_const)
#endif/*AUDIO_EFFECTS_GAIN_AT_RAM*/

        AudioEffects_data_code_end = .;
        AudioEffects_data_code_size = AudioEffects_data_code_end - AudioEffects_data_code_begin;




        media_data_code_end = .;



