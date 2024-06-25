if enable_moudles["anc"] == false then
    return;
else


local anc_config_version       = "ANCGAIN01"; -- 固定10字节长
local anc_coeff_config_version = "ANCCOEF01"; -- 固定10字节长

local anc_coeff_size = 588 * 4;

--[[===================================================================================
=============================== 配置子项8: ANC参数配置 ================================
====================================================================================--]]
local anc_config = {
	coeff_header = {},
	coeff = {}, -- a fix size binary, store coeffes

	header = {}, -- a fix size string

    version = {},       --u16 当前结构体版本号
    dac_gain = {},      --u8 dac模拟增益 			range 0-3;  default 3
    l_ffmic_gain = {},  --u8 ANCL FFmic	  			range 0-19;  default 8
    l_fbmic_gain = {},  --u8 ANCL FBmic	  			range 0-19;  default 8
    cmp_en = {},        --u8 音乐补偿使能			range 0-1;   default 1
    drc_en = {},        --u8 DRC使能				range 0-1;   default 0
    ahs_en = {},        --u8 AHS使能				range 0-1;   default 1
    ff_1st_dcc = {},    --u8 DCC选择 				range 0-8;   default 1
    gain_sign = {},     --u8 ANC各类增益的符号     range 0-255; default 0
    noise_lvl = {},     --u8 训练的噪声等级		range 0-255; default 0
    fade_step = {},     --u8 淡入淡出步进			range 0-15;  default 1
    sr = {},            --u32 ANC采样率
    trans_sr = {},      --u32 通透模式采样率

    --调试DB, 存储Float, float = lg(db/20)
    l_ffgain = {},      --float ANCL FF增益			range 0.0316(-30db) - 31.622(+30db); default 1.0(0db)
    l_fbgain = {},      --float ANCL FB增益			range 0.0316(-30db) - 31.622(+30db); default 1.0(0db)
    l_transgain = {},   --float ANCL 通透增益			range 0.0316(-30db) - 31.622(+30db); default 1.0(0db)
    l_cmpgain = {},     --float ANCL 音乐补偿增益		range 0.0316(-30db) - 31.622(+30db); default 1.0(0db)
    r_ffgain = {},      --float ANCR FF增益			range 0.0316(-30db) - 31.622(+30db); default 1.0(0db)
    r_fbgain = {},      --float ANCR FB增益			range 0.0316(-30db) - 31.622(+30db); default 1.0(0db)
    r_transgain = {},   --float ANCR 通透增益			range 0.0316(-30db) - 31.622(+30db); default 1.0(0db)
    r_cmpgain = {},     --float ANCR 音乐补偿增益		range 0.0316(-30db) - 31.622(+30db); default 1.0(0db)

    --以下所有配置项不显示
    drcff_zero_det = {},      --u8 DRCFF过零检测使能	range 0-1; 	   default 1;
    drcff_dat_mode = {},      --u8 DRCFF_DAT模式		range 0-3; 	   default 0;
    drcff_lpf_sel = {},        --u8 DRCFF_LPF档位		range 0-3; 	   default 0;
    drcfb_zero_det = {},	    --u8 DRCFB过零检测使    range 0-1; 	   default 1;
    drcfb_dat_mode = {},      --u8 DRCFB_DAT模式		range 0-3;	   default 0;
    drcfb_lpf_sel = {},       --u8 DRCFB_LPF档位		range 0-3;	   default 0;

    drcff_lthr = {},		    --u16 DRCFF_LOW阈值		range 0-32767; default 0;
    drcff_hthr = {},         --u16 DRCFF_HIGH阈值		range 0-32767; default 0;
    drcff_lgain = {},        --s16 DRCFF_LOW增益		range 0-32767; default 0;
    drcff_hgain = {},	    --s16 DRCFF_HIGH增益		range 0-32767; default 0;
    drcff_norgain = {},      --s16 DRCFF_NOR增益		range 0-32767; default 0;

    drcfb_lthr = {},        --u16 u16 DRCFB_LOW阈值		range 0-32767; default 0;
    drcfb_hthr = {},         --u16 DRCFB_HIGH阈值		range 0-32767; default 0;
    drcfb_lgain = {},	    --s16 DRCFB_LOW增益		range 0-32767; default 0;
    drcfb_hgain = {},  	    --s16 DRCFB_HIGH增益		range 0-32767; default 0;
    drcfb_norgain = {},      --s16 DRCFB_NOR增益		range 0-32767; default 0;

    drctrans_lthr = {},      --u16 DRCTRANS_LOW阈值	range 0-32767; default 0;
    drctrans_hthr = {},      --u16 DRCTRANS_HIGH阈值	range 0-32767; default 0;
    drctrans_lgain = {},     --s16 DRCTRANS_LOW增益	range 0-32767; default 0;
    drctrans_hgain = {},     --s16 DRCTRANS_HIGH增益	range 0-32767; default 0;
    drctrans_norgain = {},   --s16 DRCTRANS_NOR增益	range 0-32767; default 0;

    ahs_dly = {},			    --u8 AHS_DLY			range 0-15;	   default 1
    ahs_tap = {},			    --u8 AHS_TAP			range 0-255;   default 100
    ahs_wn_shift = {},	    --u8 AHS_WN_SHIFT		range 0-15;	   default 9
    ahs_wn_sub = {},	  	    --u8 AHS_WN_SUB  		range 0-1;	   default 1
    ahs_shift = {},		    --u16 AHS_SHIFT   		range 0-65536; default 210;
    ahs_u = {},			    --u16 AHS步进			range 0-65536; default 4000
    ahs_gain = {},		    --s16 AHS增益			range -32767-32767;default -1024;

    ahs_nlms_sel = {},      -- AHS_NLMS range 0 - 1, default 0
    developer_mode = {},    -- GAIN 开发者模式 range 0 - 1, default 0

    audio_drc_thr = {},		--u16 Audio DRC阈值     range -6.0-0; default -6.0dB

    r_ffmic_gain = {},      -- ANCR FFmic增益 range 0-19, default 8
    r_fbmic_gain = {},      -- ANCR FBmic增益 range 0-19, default 8
    fb_1st_dcc = {},        -- FB 1阶DCC档位， range 0-8, default 1
    ff_2nd_dcc = {},        -- FF 2阶DCC档位 range 0-15, default 4
    fb_2nd_dcc = {},        -- FB 2阶DCC档位 range 0-15, default 4
    drc_ff_2dcc = {},       -- DRC FF动态DCC目标值 range 0-15, default 0
    drc_fb_2dcc = {},       -- DRC FB动态DCC目标值 range 0-15, default 0
    reserver_1bytes_1 = {},

    drc_dcc_det_time = {},  -- DRC DCC 检测时间 ms range 0-32767 default 200
    drc_dcc_res_time = {},  -- DRC DCC 恢复时间 ms range 0-32767 default 10

    dummy_cfg = {},		    --dummy byte;

	ui = {},

    output = {
        anc_config_item_table = {},
        anc_config_output_view_table = {},
        anc_config_htext_output_table = {},
    },
};


anc_config.reference_book_view = cfg:stButton("JL_701N_ANC调试手册.pdf",
    function()
        local ret = cfg:utilsShellOpenFile(anc_reference_book_path);
        if (ret == false) then
            if cfg.lang == "zh" then
		        cfg:msgBox("info", anc_reference_book_path .. "文件不存在");
            else
		        cfg:msgBox("info", anc_reference_book_path .. " file not exist.");
            end
        end
    end);


anc_config.header.cfg = cfg:fixbin("anc-header", 10, anc_config_version);
anc_config.header.view = cfg:hBox {
	cfg:stLabel("版本: " .. anc_config_version)
};

anc_config.coeff_header.cfg = cfg:fixbin("anc-coeff-header", 10, anc_coeff_config_version);
anc_config.coeff_header.view = cfg:hBox {
	cfg:stLabel("系数版本：" .. anc_coeff_config_version)
};

anc_config.coeff.cfg = cfg:varbin("anc-coeff", anc_coeff_size, ""); -- default empty
anc_config.coeff.view = cfg:hBox {
	cfg:stLabel("系数"), cfg:labelView(anc_config.coeff.cfg)
};

function db_2_float(db)
    local f_val;
    --f_val = math.pow(10, (db / 20));
    f_val = (10 ^ (db / 20));
    return f_val;
end

function float_2_db(f)
    local db_val;
    db_val = 20 * (math.log(f) / math.log(10)); --以e为低
    return db_val;
end

function db_2_int(db)
    local int_val;
    --f_val = math.pow(10, (db / 20));
    if db == -30 then
        int_val = 0;
    else
        int_val = math.floor(1024 * (10 ^ (db / 20))); --向下取整, 向上取整为math.ceil
    end
    return int_val;
end

function int_2_db(int)
    local db_val;
    db_val = 20 * ((math.log(int) / math.log(10)) - (math.log(1024) / math.log(10))); --以e为低
    return db_val;
end

-- 8-0 version 当前结构体版本号
anc_config.version.cfg = cfg:i32("anc_version:  ", 0x7010);
anc_config.version.cfg:setOSize(2);
anc_config.version.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.version.cfg.name .. TAB_TABLE[1]),
    --cfg:ispinView(anc_config.version.cfg, 0, 15, 1),
    cfg:stLabel("当前结构体版本号"),
    cfg:stSpacer(),
};

-- 8-1 dac增益
anc_config.dac_gain.cfg = cfg:i32("dac_gain:  ", 3);
anc_config.dac_gain.cfg:setOSize(1);
anc_config.dac_gain.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.dac_gain.cfg.name .. TAB_TABLE[1]),
    cfg:ispinView(anc_config.dac_gain.cfg, 0, 3, 1),
    cfg:stLabel("(dac模拟增益，设置范围: 0 ~ 3，步进：1，默认值：3)"),
    cfg:stSpacer(),
};

-- 8-2 l_ffmic_gain
anc_config.l_ffmic_gain.cfg = cfg:i32("l_ffmic_gain:  ", 8);
anc_config.l_ffmic_gain.cfg:setOSize(1);
anc_config.l_ffmic_gain.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.l_ffmic_gain.cfg.name .. TAB_TABLE[1]),
    cfg:ispinView(anc_config.l_ffmic_gain.cfg, 0, 19, 1),
    cfg:stLabel("(mic0模拟增益，设置范围: 0 ~ 19，步进：1，默认值：8)"),
    cfg:stSpacer(),
};

-- 8-3 l_fbmic_gain
anc_config.l_fbmic_gain.cfg = cfg:i32("l_fbmic_gain:  ", 8);
anc_config.l_fbmic_gain.cfg:setOSize(1);
anc_config.l_fbmic_gain.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.l_fbmic_gain.cfg.name .. TAB_TABLE[1]),
    cfg:ispinView(anc_config.l_fbmic_gain.cfg, 0, 19, 1),
    cfg:stLabel("(mic1模拟增益，设置范围: 0 ~ 19，步进：1，默认值：8)"),
    cfg:stSpacer(),
};

-- 8-4 音乐补偿使能
anc_config.cmp_en.cfg = cfg:i32("cmp_en:  ", 1);
anc_config.cmp_en.cfg:setOSize(1);
anc_config.cmp_en.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.cmp_en.cfg.name .. TAB_TABLE[1]),
    cfg:ispinView(anc_config.cmp_en.cfg, 0, 1, 1),
    cfg:stLabel("(音乐补偿使能，设置范围: 0 ~ 1，步进：1，默认值：1)"),
    cfg:stSpacer(),
};

-- 8-5 DRC使能
anc_config.drc_en.cfg = cfg:i32("drc_en:  ", 0);
anc_config.drc_en.cfg:setOSize(1);
anc_config.drc_en.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.drc_en.cfg.name .. TAB_TABLE[1]),
    cfg:ispinView(anc_config.drc_en.cfg, 0, 1, 1),
    cfg:stLabel("(DRC使能，设置范围: 0 ~ 1，步进：1，默认值：0)"),
    cfg:stSpacer(),
};

-- 8-6 AHS使能
anc_config.ahs_en.cfg = cfg:i32("ahs_en:  ", 1);
anc_config.ahs_en.cfg:setOSize(1);
anc_config.ahs_en.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.ahs_en.cfg.name .. TAB_TABLE[1]),
    cfg:ispinView(anc_config.ahs_en.cfg, 0, 1, 1),
    cfg:stLabel("(AHS使能，设置范围: 0 ~ 1，步进：1，默认值：1)"),
    cfg:stSpacer(),
};

-- 8-7 DCC选择
anc_config.ff_1st_dcc.cfg = cfg:i32("ff_1st_dcc:  ", 1);
anc_config.ff_1st_dcc.cfg:setOSize(1);
anc_config.ff_1st_dcc.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.ff_1st_dcc.cfg.name .. TAB_TABLE[1]),
    cfg:ispinView(anc_config.ff_1st_dcc.cfg, 0, 8, 1),
    cfg:stLabel("(DCC选择，设置范围: 0 ~ 8，步进：1，默认值：1)"),
    cfg:stSpacer(),
};

-- 8-8 ANC各类增益的符号
anc_config.gain_sign.cfg = cfg:i32("gain_sign:  ", 0);
anc_config.gain_sign.cfg:setOSize(1);
anc_config.gain_sign.cfg:addConstraint(
    function ()
        local warn_str;
        if cfg.lang == "zh" then
            warn_str = "请输入 1 Bytes 配置值";
        else
            warn_str = "Please input 1 Bytes value.";
        end
        return ((anc_config.gain_sign.cfg.val) >= 0 and (anc_config.gain_sign.cfg.val) <= 0xFF)  or warn_str;
    end
);
anc_config.gain_sign.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.gain_sign.cfg.name .. TAB_TABLE[1] .. '0x'),
    cfg:hexInputView(anc_config.gain_sign.cfg),
    --cfg:ispinView(anc_config.gain_sign.cfg, 0, 255, 1),
    cfg:stLabel("(ANC各类增益的符号，设置范围: 0x0 ~ 0xFF, 默认值：0)"),
    cfg:stSpacer(),
};

-- 8-9 训练的噪声等级
anc_config.noise_lvl.cfg = cfg:i32("noise_lvl:  ", 0);
anc_config.noise_lvl.cfg:setOSize(1);
anc_config.noise_lvl.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.noise_lvl.cfg.name .. TAB_TABLE[1]),
    cfg:ispinView(anc_config.noise_lvl.cfg, 0, 255, 1),
    cfg:stLabel("(训练的噪声等级，设置范围: 0 ~ 255，步进：1，默认值：0)"),
    cfg:stSpacer(),
};

-- 8-10 淡入淡出步进
anc_config.fade_step.cfg = cfg:i32("fade_step:  ", 1);
anc_config.fade_step.cfg:setOSize(1);
anc_config.fade_step.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.fade_step.cfg.name .. TAB_TABLE[1]),
    cfg:ispinView(anc_config.fade_step.cfg, 0, 15, 1),
    cfg:stLabel("(淡入淡出步进，设置范围: 0 ~ 15，步进：1，默认值：1)"),
    cfg:stSpacer(),
};

-- 8-11 ANC算法模式
anc_config.sr.table = cfg:enumMap("sr_table",
	{
		[5] = "MODE1",
		[6] = "MODE2",
	}
)
anc_config.sr.cfg = cfg:enum("降噪MODE:  ", anc_config.sr.table, 5);
anc_config.sr.cfg:setOSize(4);
anc_config.sr.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.sr.cfg.name .. TAB_TABLE[1]),
    cfg:enumView(anc_config.sr.cfg),
    cfg:stLabel("算法模式， 默认值：MODE1"),
    cfg:stSpacer(),
};

-- 8-12 通透算法模式
anc_config.trans_sr.table = cfg:enumMap("trans_sr_table",
	{
		[3] = "MODE1",
		[4] = "MODE2",
	}
)
anc_config.trans_sr.cfg = cfg:enum("通透MODE:  ", anc_config.trans_sr.table, 3);
anc_config.trans_sr.cfg:setOSize(4);
anc_config.trans_sr.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.trans_sr.cfg.name .. TAB_TABLE[1]),
    cfg:enumView(anc_config.trans_sr.cfg),
    cfg:stLabel("算法模式， 默认值：MODE1"),
    cfg:stSpacer(),
};

-- 8-13 l_ffgain lg(db/20)
anc_config.l_ffgain.show_cfg = cfg:dbf("l_ffgain：", 0);
anc_config.l_ffgain.show_cfg:setOSize(4);
anc_config.l_ffgain.cfg = cfg:dbf("l_ffgain_out：", 1.0);
anc_config.l_ffgain.cfg:setOSize(4);
anc_config.l_ffgain.cfg:addDeps({anc_config.l_ffgain.show_cfg});
anc_config.l_ffgain.cfg:setEval(function() 
    --print('set Eval, l_ffgain show val: ' .. anc_config.l_ffgain.show_cfg.val .. ' l_ffgain val: ' .. db_2_float(anc_config.l_ffgain.show_cfg.val));
    --print('l_ffgain cfg val: ' .. anc_config.l_ffgain.cfg.val .. ', set Eval, l_ffgain show val: ' .. anc_config.l_ffgain.show_cfg.val .. ' l_ffgain val: ' .. db_2_float(anc_config.l_ffgain.show_cfg.val));
    return (db_2_float(anc_config.l_ffgain.show_cfg.val)); end);

anc_config.l_ffgain.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.l_ffgain.show_cfg.name .. TAB_TABLE[1]),
    cfg:dspinView(anc_config.l_ffgain.show_cfg, -30, 30, 0.01, 2),
    cfg:stLabel("ANCL FF增益，设置范围: -30 dB ~ 30 dB，默认值：0dB)"),
    cfg:stSpacer(),
};

-- 8-14 l_fbgain lg(db/20)
anc_config.l_fbgain.show_cfg = cfg:dbf("l_fbgain：", 0);
anc_config.l_fbgain.show_cfg:setOSize(4);
anc_config.l_fbgain.cfg = cfg:dbf("l_fbgain_out：", 1.0);
anc_config.l_fbgain.cfg:setOSize(4);
anc_config.l_fbgain.cfg:addDeps({anc_config.l_fbgain.show_cfg});
anc_config.l_fbgain.cfg:setEval(function() 
    return (db_2_float(anc_config.l_fbgain.show_cfg.val)); end);

anc_config.l_fbgain.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.l_fbgain.show_cfg.name .. TAB_TABLE[1]),
    cfg:dspinView(anc_config.l_fbgain.show_cfg, -30, 30, 0.01, 2),
    cfg:stLabel("ANCL FB增益，设置范围: -30 dB ~ 30 dB，默认值：0dB)"),
    cfg:stSpacer(),
};

-- 8-15 l_transgain lg(db/20)
anc_config.l_transgain.show_cfg = cfg:dbf("l_transgain：", 0);
anc_config.l_transgain.show_cfg:setOSize(4);
anc_config.l_transgain.cfg = cfg:dbf("l_transgain_out：", 1.0);
anc_config.l_transgain.cfg:setOSize(4);
anc_config.l_transgain.cfg:addDeps({anc_config.l_transgain.show_cfg});
anc_config.l_transgain.cfg:setEval(function() 
    return (db_2_float(anc_config.l_transgain.show_cfg.val)); end);

anc_config.l_transgain.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.l_transgain.show_cfg.name .. TAB_TABLE[1]),
    cfg:dspinView(anc_config.l_transgain.show_cfg, -30, 30, 0.01, 2),
    cfg:stLabel("ANCL 通透增益，设置范围: -30 dB ~ 30 dB，默认值：0dB)"),
    cfg:stSpacer(),
};

-- 8-16 l_cmpgain lg(db/20)
anc_config.l_cmpgain.show_cfg = cfg:dbf("l_cmpgain ：", 0);
anc_config.l_cmpgain.show_cfg:setOSize(4);
anc_config.l_cmpgain.cfg = cfg:dbf("l_cmpgain_out：", 1.0);
anc_config.l_cmpgain.cfg:setOSize(4);
anc_config.l_cmpgain.cfg:addDeps({anc_config.l_cmpgain.show_cfg});
anc_config.l_cmpgain.cfg:setEval(function() 
    return (db_2_float(anc_config.l_cmpgain.show_cfg.val)); end);

anc_config.l_cmpgain.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.l_cmpgain.show_cfg.name .. TAB_TABLE[1]),
    cfg:dspinView(anc_config.l_cmpgain.show_cfg, -30, 30, 0.01, 2),
    cfg:stLabel("ANCL 音乐补偿增益，设置范围: -30 dB ~ 30 dB，默认值：0dB)"),
    cfg:stSpacer(),
};

-- 8-17 r_ffgain lg(db/20)
anc_config.r_ffgain.show_cfg = cfg:dbf("r_ffgain：", 0);
anc_config.r_ffgain.show_cfg:setOSize(4);
anc_config.r_ffgain.cfg = cfg:dbf("r_ffgain_out：", 1.0);
anc_config.r_ffgain.cfg:setOSize(4);
anc_config.r_ffgain.cfg:addDeps({anc_config.r_ffgain.show_cfg});
anc_config.r_ffgain.cfg:setEval(function() 
    return (db_2_float(anc_config.r_ffgain.show_cfg.val)); end);

anc_config.r_ffgain.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.r_ffgain.show_cfg.name .. TAB_TABLE[1]),
    cfg:dspinView(anc_config.r_ffgain.show_cfg, -30, 30, 0.01, 2),
    cfg:stLabel("ANCR FF增益，设置范围: -30 dB ~ 30 dB，默认值：0dB)"),
    cfg:stSpacer(),
};

-- 8-18 r_fbgain lg(db/20)
anc_config.r_fbgain.show_cfg = cfg:dbf("r_fbgain：", 0);
anc_config.r_fbgain.show_cfg:setOSize(4);
anc_config.r_fbgain.cfg = cfg:dbf("r_fbgain_out：", 1.0);
anc_config.r_fbgain.cfg:setOSize(4);
anc_config.r_fbgain.cfg:addDeps({anc_config.r_fbgain.show_cfg});
anc_config.r_fbgain.cfg:setEval(function() 
    return (db_2_float(anc_config.r_fbgain.show_cfg.val)); end);

anc_config.r_fbgain.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.r_fbgain.show_cfg.name .. TAB_TABLE[1]),
    cfg:dspinView(anc_config.r_fbgain.show_cfg, -30, 30, 0.01, 2),
    cfg:stLabel("ANCR FB增益，设置范围: -30 dB ~ 30 dB，默认值：0dB)"),
    cfg:stSpacer(),
};

-- 8-19 r_transgain lg(db/20)
anc_config.r_transgain.show_cfg = cfg:dbf("r_transgain：", 0);
anc_config.r_transgain.show_cfg:setOSize(4);
anc_config.r_transgain.cfg = cfg:dbf("r_transgain_out：", 1.0);
anc_config.r_transgain.cfg:setOSize(4);
anc_config.r_transgain.cfg:addDeps({anc_config.r_transgain.show_cfg});
anc_config.r_transgain.cfg:setEval(function() 
    return (db_2_float(anc_config.r_transgain.show_cfg.val)); end);

anc_config.r_transgain.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.r_transgain.show_cfg.name .. TAB_TABLE[1]),
    cfg:dspinView(anc_config.r_transgain.show_cfg, -30, 30, 0.01, 2),
    cfg:stLabel("ANCR 通透增益，设置范围: -30 dB ~ 30 dB，默认值：0dB)"),
    cfg:stSpacer(),
};

-- 8-20 r_cmpgain lg(db/20)
anc_config.r_cmpgain.show_cfg = cfg:dbf("r_cmpgain ：", 0);
anc_config.r_cmpgain.show_cfg:setOSize(4);
anc_config.r_cmpgain.cfg = cfg:dbf("r_cmpgain_out：", 1.0);
anc_config.r_cmpgain.cfg:setOSize(4);
anc_config.r_cmpgain.cfg:addDeps({anc_config.r_cmpgain.show_cfg});
anc_config.r_cmpgain.cfg:setEval(function() 
    return (db_2_float(anc_config.r_cmpgain.show_cfg.val)); end);

anc_config.r_cmpgain.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.r_cmpgain.show_cfg.name .. TAB_TABLE[1]),
    cfg:dspinView(anc_config.r_cmpgain.show_cfg, -30, 30, 0.01, 2),
    cfg:stLabel("ANCR 音乐补偿增益，设置范围: -30 dB ~ 30 dB，默认值：0dB)"),
    cfg:stSpacer(),
};

-- 8-21 DRCFF过零检测使能
anc_config.drcff_zero_det.cfg = cfg:i32("drcff_zero_det:  ", 0);
anc_config.drcff_zero_det.cfg:setOSize(1);
anc_config.drcff_zero_det.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.drcff_zero_det.cfg.name .. TAB_TABLE[0]),
    cfg:ispinView(anc_config.drcff_zero_det.cfg, 0, 1, 1),
    cfg:stLabel("(DRCFF过零检测使能，设置范围: 0 ~ 1，步进：1，默认值：1)"),
    cfg:stSpacer(),
};

-- 8-22 DRCFF_DAT模式
anc_config.drcff_dat_mode.cfg = cfg:i32("drcff_dat_mode:  ", 0);
anc_config.drcff_dat_mode.cfg:setOSize(1);
anc_config.drcff_dat_mode.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.drcff_dat_mode.cfg.name .. TAB_TABLE[0]),
    cfg:ispinView(anc_config.drcff_dat_mode.cfg, 0, 3, 1),
    cfg:stLabel("(DRCFF_DAT模式，设置范围: 0 ~ 3，步进：1，默认值：0)"),
    cfg:stSpacer(),
};


-- 8-23 DRCFF_LPF档位
anc_config.drcff_lpf_sel.cfg = cfg:i32("drcff_lpf_sel:  ", 0);
anc_config.drcff_lpf_sel.cfg:setOSize(1);
anc_config.drcff_lpf_sel.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.drcff_lpf_sel.cfg.name .. TAB_TABLE[0]),
    cfg:ispinView(anc_config.drcff_lpf_sel.cfg, 0, 3, 1),
    cfg:stLabel("(DRCFF_LPF档位，设置范围: 0 ~ 3，步进：1，默认值：0)"),
    cfg:stSpacer(),
};

-- 8-24 DRCFB过零检测使
anc_config.drcfb_zero_det.cfg = cfg:i32("drcfb_zero_det:  ", 0);
anc_config.drcfb_zero_det.cfg:setOSize(1);
anc_config.drcfb_zero_det.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.drcfb_zero_det.cfg.name .. TAB_TABLE[0]),
    cfg:ispinView(anc_config.drcfb_zero_det.cfg, 0, 1, 1),
    cfg:stLabel("(DRCFB过零检测使，设置范围: 0 ~ 1，步进：1，默认值：1)"),
    cfg:stSpacer(),
};

-- 8-25 DRCFB_DAT模式
anc_config.drcfb_dat_mode.cfg = cfg:i32("drcfb_dat_mode:  ", 0);
anc_config.drcfb_dat_mode.cfg:setOSize(1);
anc_config.drcfb_dat_mode.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.drcfb_dat_mode.cfg.name .. TAB_TABLE[0]),
    cfg:ispinView(anc_config.drcfb_dat_mode.cfg, 0, 3, 1),
    cfg:stLabel("(DRCFB_DAT模式，设置范围: 0 ~ 3，步进：1，默认值：0)"),
    cfg:stSpacer(),
};

-- 8-26 DRCFB_LPF档位
anc_config.drcfb_lpf_sel.cfg = cfg:i32("drcfb_lpf_sel:  ", 0);
anc_config.drcfb_lpf_sel.cfg:setOSize(1);
anc_config.drcfb_lpf_sel.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.drcfb_lpf_sel.cfg.name .. TAB_TABLE[0]),
    cfg:ispinView(anc_config.drcfb_lpf_sel.cfg, 0, 3, 1),
    cfg:stLabel("(DRCFF过零检测使能，设置范围: 0 ~ 3，步进：1，默认值：0)"),
    cfg:stSpacer(),
};

-- 8-27 DRCFF_LOW阈值 
anc_config.drcff_lthr.cfg = cfg:i32("drcff_lthr:  ", 0);
anc_config.drcff_lthr.cfg:setOSize(2);
anc_config.drcff_lthr.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.drcff_lthr.cfg.name .. TAB_TABLE[1]),
    cfg:ispinView(anc_config.drcff_lthr.cfg, 0, 32767, 1),
    cfg:stLabel("(DRCFF_LOW阈值，设置范围: 0 ~ 32767，步进：1，默认值：0)"),
    cfg:stSpacer(),
};

-- 8-28 DRCFF_HIGH阈值
anc_config.drcff_hthr.cfg = cfg:i32("drcff_hthr:  ", 6000);
anc_config.drcff_hthr.cfg:setOSize(2);
anc_config.drcff_hthr.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.drcff_hthr.cfg.name .. TAB_TABLE[1]),
    cfg:ispinView(anc_config.drcff_hthr.cfg, 0, 32767, 1),
    cfg:stLabel("(DRCFF_HIGH阈值，设置范围: 0 ~ 32767，步进：1，默认值：6000)"),
    cfg:stSpacer(),
};

-- 8-29 DRCFF_LOW增益
anc_config.drcff_lgain.show_cfg = cfg:dbf("drcff_lgain:  ", 0);
anc_config.drcff_lgain.show_cfg:setOSize(4);
anc_config.drcff_lgain.cfg = cfg:i32("drcff_lgain_out:  ", 1024);
anc_config.drcff_lgain.cfg:setOSize(2);
anc_config.drcff_lgain.cfg:addDeps({anc_config.drcff_lgain.show_cfg});
anc_config.drcff_lgain.cfg:setEval(function()
    --print('drcff_lgain cfg val: ' .. anc_config.drcff_lgain.cfg.val .. ', set Eval, drcff_lgain show val: ' .. anc_config.drcff_lgain.show_cfg.val .. ' drcff_lgain val: ' .. db_2_int(anc_config.drcff_lgain.show_cfg.val));
    return (db_2_int(anc_config.drcff_lgain.show_cfg.val)); end);
anc_config.drcff_lgain.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.drcff_lgain.show_cfg.name .. TAB_TABLE[1]),
    cfg:dspinView(anc_config.drcff_lgain.show_cfg, -30, 0, 0.01, 2),
    cfg:stLabel("DRCFF_LOW增益，设置范围: -30 dB ~ 0 dB，默认值：0dB)"),
    cfg:stSpacer(),
};

-- 8-30 DRCFF_HIGH增益
anc_config.drcff_hgain.show_cfg = cfg:dbf("drcff_hgain:  ", -6);
anc_config.drcff_hgain.show_cfg:setOSize(4);
anc_config.drcff_hgain.cfg = cfg:i32("drcff_hgain_out:  ", 512);
anc_config.drcff_hgain.cfg:setOSize(2);
anc_config.drcff_hgain.cfg:addDeps({anc_config.drcff_hgain.show_cfg});
anc_config.drcff_hgain.cfg:setEval(function()
    return (db_2_int(anc_config.drcff_hgain.show_cfg.val)); end);
anc_config.drcff_hgain.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.drcff_hgain.show_cfg.name .. TAB_TABLE[1]),
    cfg:dspinView(anc_config.drcff_hgain.show_cfg, -30, 0, 0.01, 2),
    cfg:stLabel("DRCFF_HIGH增益，设置范围: -30 dB ~ 0 dB，默认值：-6 dB)"),
    cfg:stSpacer(),
};

-- 8-31 DRCFF_NOR增益
anc_config.drcff_norgain.show_cfg = cfg:dbf("drcff_norgain:  ", 0);
anc_config.drcff_norgain.show_cfg:setOSize(4);
anc_config.drcff_norgain.cfg = cfg:i32("drcff_norgain_out:  ", 1024);
anc_config.drcff_norgain.cfg:setOSize(2);
anc_config.drcff_norgain.cfg:addDeps({anc_config.drcff_norgain.show_cfg});
anc_config.drcff_norgain.cfg:setEval(function()
    return (db_2_int(anc_config.drcff_norgain.show_cfg.val)); end);
anc_config.drcff_norgain.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.drcff_norgain.show_cfg.name .. TAB_TABLE[0]),
    cfg:dspinView(anc_config.drcff_norgain.show_cfg, -30, 30, 0.01, 2),
    cfg:stLabel("DRCFF_NOR增益，设置范围: -30 dB ~ 30 dB，默认值：0dB)"),

    cfg:stSpacer(),
};

-- 8-32 DRCFB_LOW阈值
anc_config.drcfb_lthr.cfg = cfg:i32("drcfb_lthr:  ", 0);
anc_config.drcfb_lthr.cfg:setOSize(2);
anc_config.drcfb_lthr.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.drcfb_lthr.cfg.name .. TAB_TABLE[1]),
    cfg:ispinView(anc_config.drcfb_lthr.cfg, 0, 32767, 1),
    cfg:stLabel("(DRCFB_LOW阈值，设置范围: 0 ~ 32767，步进：1，默认值：0)"),
    cfg:stSpacer(),
};

-- 8-33 DRCFB_HIGH阈值
anc_config.drcfb_hthr.cfg = cfg:i32("drcfb_hthr:  ", 6000);
anc_config.drcfb_hthr.cfg:setOSize(2);
anc_config.drcfb_hthr.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.drcfb_hthr.cfg.name .. TAB_TABLE[1]),
    cfg:ispinView(anc_config.drcfb_hthr.cfg, 0, 32767, 1),
    cfg:stLabel("(DRCFB_HIGH阈值，设置范围: 0 ~ 32767，步进：1，默认值：6000)"),
    cfg:stSpacer(),
};

-- 8-34 DRCFB_LOW增益
anc_config.drcfb_lgain.show_cfg = cfg:dbf("drcfb_lgain:  ", 0);
anc_config.drcfb_lgain.show_cfg:setOSize(4);
anc_config.drcfb_lgain.cfg = cfg:i32("drcfb_lgain_out:  ", 1024);
anc_config.drcfb_lgain.cfg:setOSize(2);
anc_config.drcfb_lgain.cfg:addDeps({anc_config.drcfb_lgain.show_cfg});
anc_config.drcfb_lgain.cfg:setEval(function()
    return (db_2_int(anc_config.drcfb_lgain.show_cfg.val)); end);
anc_config.drcfb_lgain.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.drcfb_lgain.show_cfg.name .. TAB_TABLE[1]),
    cfg:dspinView(anc_config.drcfb_lgain.show_cfg, -30, 0, 0.01, 2),
    cfg:stLabel("DRCFB_LOW增益，设置范围: -30 dB ~ 0 dB，默认值：0dB)"),
    cfg:stSpacer(),
};

-- 8-35 DRCFB_HIGH增益
anc_config.drcfb_hgain.show_cfg = cfg:dbf("drcfb_hgain:  ", -6);
anc_config.drcfb_hgain.show_cfg:setOSize(4);
anc_config.drcfb_hgain.cfg = cfg:i32("drcfb_hgain_out:  ", 512);
anc_config.drcfb_hgain.cfg:setOSize(2);
anc_config.drcfb_hgain.cfg:addDeps({anc_config.drcfb_hgain.show_cfg});
anc_config.drcfb_hgain.cfg:setEval(function()
    return (db_2_int(anc_config.drcfb_hgain.show_cfg.val)); end);
anc_config.drcfb_hgain.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.drcfb_hgain.show_cfg.name .. TAB_TABLE[1]),
    cfg:dspinView(anc_config.drcfb_hgain.show_cfg, -30, 0, 0.01, 2),
    cfg:stLabel("DRCFB_HIGH增益，设置范围: -30 dB ~ 0 dB，默认值：-6dB)"),

    cfg:stSpacer(),
};

-- 8-36 DRCFB_NOR增益
anc_config.drcfb_norgain.show_cfg = cfg:dbf("drcfb_norgain:  ", 0);
anc_config.drcfb_norgain.show_cfg:setOSize(4);
anc_config.drcfb_norgain.cfg = cfg:i32("drcfb_norgain_out:  ", 1024);
anc_config.drcfb_norgain.cfg:setOSize(2);
anc_config.drcfb_norgain.cfg:addDeps({anc_config.drcfb_norgain.show_cfg});
anc_config.drcfb_norgain.cfg:setEval(function()
    return (db_2_int(anc_config.drcfb_norgain.show_cfg.val)); end);
anc_config.drcfb_norgain.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.drcfb_norgain.show_cfg.name .. TAB_TABLE[0]),
    cfg:dspinView(anc_config.drcfb_norgain.show_cfg, -30, 30, 0.01, 2),
    cfg:stLabel("DRCFB_NOR增益，设置范围: -30 dB ~ 30 dB，默认值：0dB)"),

    cfg:stSpacer(),
};

-- 8-37 DRCTRANS_LOW阈值
anc_config.drctrans_lthr.cfg = cfg:i32("drctrans_lthr:  ", 0);
anc_config.drctrans_lthr.cfg:setOSize(2);
anc_config.drctrans_lthr.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.drctrans_lthr.cfg.name .. TAB_TABLE[0]),
    cfg:ispinView(anc_config.drctrans_lthr.cfg, 0, 32767, 1),
    cfg:stLabel("(DRCTRANS_LOW阈值，设置范围: 0 ~ 32767，步进：1，默认值：0)"),
    cfg:stSpacer(),
};

-- 8-38 DRCTRANS_HIGH阈值
anc_config.drctrans_hthr.cfg = cfg:i32("drctrans_hthr:  ", 32767);
anc_config.drctrans_hthr.cfg:setOSize(2);
anc_config.drctrans_hthr.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.drctrans_hthr.cfg.name .. TAB_TABLE[0]),
    cfg:ispinView(anc_config.drctrans_hthr.cfg, 0, 32767, 1),
    cfg:stLabel("(DRCTRANS_HIGH阈值，设置范围: 0 ~ 32767，步进：1，默认值：32767)"),
    cfg:stSpacer(),
};

-- 8-39 DRCTRANS_LOW增益
anc_config.drctrans_lgain.show_cfg = cfg:dbf("drctrans_lgain:  ", 0);
anc_config.drctrans_lgain.show_cfg:setOSize(4);
anc_config.drctrans_lgain.cfg = cfg:i32("drctrans_lgain_out:  ", 1024);
anc_config.drctrans_lgain.cfg:setOSize(2);
anc_config.drctrans_lgain.cfg:addDeps({anc_config.drctrans_lgain.show_cfg});
anc_config.drctrans_lgain.cfg:setEval(function()
    return (db_2_int(anc_config.drctrans_lgain.show_cfg.val)); end);
anc_config.drctrans_lgain.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.drctrans_lgain.show_cfg.name .. TAB_TABLE[0]),
    cfg:dspinView(anc_config.drctrans_lgain.show_cfg, -30, 0, 0.01, 2),
    cfg:stLabel("DRCTRANS_LOW增益，设置范围: -30 dB ~ 0 dB，默认值：0dB)"),

    cfg:stSpacer(),
};

-- 8-40 DRCTRANS_HIGH增益
anc_config.drctrans_hgain.show_cfg = cfg:dbf("drctrans_hgain:  ", 0);
anc_config.drctrans_hgain.show_cfg:setOSize(4);
anc_config.drctrans_hgain.cfg = cfg:i32("drctrans_hgain_out:  ", 1024);
anc_config.drctrans_hgain.cfg:setOSize(2);
anc_config.drctrans_hgain.cfg:addDeps({anc_config.drctrans_hgain.show_cfg});
anc_config.drctrans_hgain.cfg:setEval(function()
    return (db_2_int(anc_config.drctrans_hgain.show_cfg.val)); end);
anc_config.drctrans_hgain.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.drctrans_hgain.show_cfg.name .. TAB_TABLE[0]),
    cfg:dspinView(anc_config.drctrans_hgain.show_cfg, -30, 0, 0.01, 2),
    cfg:stLabel("DRCTRANS_HIGH增益，设置范围: -30 dB ~ 0 dB，默认值：0dB)"),

    cfg:stSpacer(),
};

-- 8-41 DRCTRANS_NOR增益
anc_config.drctrans_norgain.show_cfg = cfg:dbf("drctrans_norgain:  ", 0);
anc_config.drctrans_norgain.show_cfg:setOSize(4);
anc_config.drctrans_norgain.cfg = cfg:i32("drctrans_norgain_out:  ", 1024);
anc_config.drctrans_norgain.cfg:setOSize(2);
anc_config.drctrans_norgain.cfg:addDeps({anc_config.drctrans_norgain.show_cfg});
anc_config.drctrans_norgain.cfg:setEval(function()
    return (db_2_int(anc_config.drctrans_norgain.show_cfg.val)); end);
anc_config.drctrans_norgain.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.drctrans_norgain.show_cfg.name .. TAB_TABLE[0]),
    cfg:dspinView(anc_config.drctrans_norgain.show_cfg, -30, 30, 0.01, 2),
    cfg:stLabel("DRCTRANS_NOR增益，设置范围: -30 dB ~ 30 dB，默认值：0dB)"),

    cfg:stSpacer(),
};

-- 8-42 AHS_DLY
anc_config.ahs_dly.cfg = cfg:i32("ahs_dly:  ", 1);
anc_config.ahs_dly.cfg:setOSize(1);
anc_config.ahs_dly.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.ahs_dly.cfg.name .. TAB_TABLE[1]),
    cfg:ispinView(anc_config.ahs_dly.cfg, 0, 15, 1),
    cfg:stLabel("(AHS_DLY，设置范围: 0 ~ 15，步进：1，默认值：1)"),
    cfg:stSpacer(),
};

-- 8-43 AHS_TAP
anc_config.ahs_tap.cfg = cfg:i32("ahs_tap:  ", 100);
anc_config.ahs_tap.cfg:setOSize(1);
anc_config.ahs_tap.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.ahs_tap.cfg.name .. TAB_TABLE[1]),
    cfg:ispinView(anc_config.ahs_tap.cfg, 0, 255, 1),
    cfg:stLabel("(AHS_TAP，设置范围: 0 ~ 255，步进：1，默认值：100)"),
    cfg:stSpacer(),
};

-- 8-44 AHS_WN_SHIFT
anc_config.ahs_wn_shift.cfg = cfg:i32("ahs_wn_shift:  ", 9);
anc_config.ahs_wn_shift.cfg:setOSize(1);
anc_config.ahs_wn_shift.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.ahs_wn_shift.cfg.name .. TAB_TABLE[1]),
    cfg:ispinView(anc_config.ahs_wn_shift.cfg, 0, 15, 1),
    cfg:stLabel("(啸叫抑制参数，越大越强，过大不稳定，设置范围：0 ~ 15，步进： 1，默认值：9)"),
    cfg:stSpacer(),
};

-- 8-45 AHS_WN_SUB
anc_config.ahs_wn_sub.cfg = cfg:i32("ahs_wn_sub:  ", 1);
anc_config.ahs_wn_sub.cfg:setOSize(1);
anc_config.ahs_wn_sub.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.ahs_wn_sub.cfg.name .. TAB_TABLE[1]),
    cfg:ispinView(anc_config.ahs_wn_sub.cfg, 0, 1, 1),
    cfg:stLabel("(AHS_WN_SUB，设置范围: 0 ~ 1，步进：1，默认值：1)"),
    cfg:stSpacer(),
};

-- 8-46 AHS_SHIFT
anc_config.ahs_shift.cfg = cfg:i32("ahs_shift:  ", 210);
anc_config.ahs_shift.cfg:setOSize(2);
anc_config.ahs_shift.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.ahs_shift.cfg.name .. TAB_TABLE[1]),
    cfg:ispinView(anc_config.ahs_shift.cfg, 0, 65535, 1),
    cfg:stLabel("(AHS_SHIFT，设置范围: 0 ~ 65535，步进：1，默认值：210)"),
    cfg:stSpacer(),
};

-- 8-47 AHS步进
anc_config.ahs_u.cfg = cfg:i32("ahs_u:  ", 4000);
anc_config.ahs_u.cfg:setOSize(2);
anc_config.ahs_u.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.ahs_u.cfg.name .. TAB_TABLE[1]),
    cfg:ispinView(anc_config.ahs_u.cfg, 0, 65535, 1),
    cfg:stLabel("(AHS步进，设置范围: 0 ~ 65535，步进：1，默认值：4000)"),
    cfg:stSpacer(),
};

-- 8-48 AHS增益
anc_config.ahs_gain.cfg = cfg:i32("ahs_gain:  ", -1024);
anc_config.ahs_gain.cfg:setOSize(2);
anc_config.ahs_gain.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.ahs_gain.cfg.name .. TAB_TABLE[1]),
    cfg:ispinView(anc_config.ahs_gain.cfg, -32767, 32767, 1),
    cfg:stLabel("(AHS增益，设置范围: -32767 ~ 32767，步进：1，默认值：-1024)"),
    cfg:stSpacer(),
};

local make_ispin_value = function (title, size, defVal, minVal, maxVal, helperText)
    local f = {}
    f.cfg = cfg:i32(title, defVal);
    f.cfg:setOSize(size);
    f.hbox_view = cfg:hBox {
        cfg:stLabel(title),
        cfg:ispinView(f.cfg, minVal, maxVal, 1),
        cfg:stLabel("(" .. helperText .. "，设置范围：" .. minVal  .. " ~ " .. maxVal .. "，步进：1，默认值：" .. defVal .. ")"),
        cfg:stSpacer(),
    };
    return f;
end;

anc_config.ahs_nlms_sel = make_ispin_value("ahs_nlms_sel: ", 1, 0, 0, 1, "AHS_NLMS");
anc_config.developer_mode = make_ispin_value("developer_mode: ", 1, 0, 0, 1, "GAIN 开发者模式");

-- 8-50 audio_drc_thr
anc_config.audio_drc_thr.cfg = cfg:dbf("audio_drc_thr:  ", -6.0);
anc_config.audio_drc_thr.cfg:setOSize(4);
anc_config.audio_drc_thr.hbox_view = cfg:hBox {
    cfg:stLabel(anc_config.audio_drc_thr.cfg.name .. TAB_TABLE[0]),
    cfg:dspinView(anc_config.audio_drc_thr.cfg, -6.0, 0, 0.01, 2),
    cfg:stLabel("(Audio DRC阈值，设置范围: -6.0dB ~ 0dB，步进：0.01，默认值：-6.0dB)"),
    cfg:stSpacer(),
};

anc_config.r_ffmic_gain = make_ispin_value("r_ffmic_gain: ", 1, 8, 0, 19, "ANCR FFmic 增益");
anc_config.r_fbmic_gain = make_ispin_value("r_fbmic_gain: ", 1, 8, 0, 19, "ANCR FBmic 增益");
anc_config.fb_1st_dcc = make_ispin_value("fb_1st_dcc: ", 1, 1, 0, 8, "FB 1阶DCC档位");
anc_config.ff_2nd_dcc = make_ispin_value("ff_2nd_dcc: ", 1, 4, 0, 15, "FF 2阶DCC档位");
anc_config.fb_2nd_dcc = make_ispin_value("fb_2nd_dcc: ", 1, 4, 0, 15, "FB 2阶DCC档位");
anc_config.drc_ff_2dcc = make_ispin_value("drc_ff_2dcc: ", 1, 0, 0, 15, "DRC FF动态DCC目标值");
anc_config.drc_fb_2dcc = make_ispin_value("drc_fb_2dcc: ", 1, 0, 0, 15, "DRC FB动态DCC目标值");

anc_config.drc_dcc_det_time = make_ispin_value("drc_dcc_det_time: ", 2, 200, 0, 32767, "DRC DCC检测时间");
anc_config.drc_dcc_res_time = make_ispin_value("drc_dcc_res_time: ", 2, 10,  0, 32767, "DRC DCC恢复时间");

anc_config.reserver_1bytes_1.cfg = cfg:fixbin("anc-reserved_cfg1", 1, "");
anc_config.dummy_cfg.cfg = cfg:fixbin("anc-dummy_cfg", 120, ""); -- default empty

function recover_show_cfg_value()
    --print('enter: anc_config.l_ffgain.cfg.val: ' .. anc_config.l_ffgain.cfg.val .. " anc_config.l_ffgain.show_cfg: " .. anc_config.l_ffgain.show_cfg.val);
    cfg:set(anc_config.l_ffgain.show_cfg, float_2_db(anc_config.l_ffgain.cfg.val));
    --print('after: anc_config.l_ffgain.cfg.val: ' .. anc_config.l_ffgain.cfg.val .. " anc_config.l_ffgain.show_cfg: " .. anc_config.l_ffgain.show_cfg.val);
    cfg:set(anc_config.l_fbgain.show_cfg, float_2_db(anc_config.l_fbgain.cfg.val));
    --print('anc_config.l_fbgain.cfg.val: ' .. anc_config.l_fbgain.cfg.val .. " anc_config.l_fbgain.show_cfg: " .. anc_config.l_fbgain.show_cfg.val);
    cfg:set(anc_config.l_transgain.show_cfg, float_2_db(anc_config.l_transgain.cfg.val));
    cfg:set(anc_config.l_cmpgain.show_cfg, float_2_db(anc_config.l_cmpgain.cfg.val));
    cfg:set(anc_config.r_ffgain.show_cfg, float_2_db(anc_config.r_ffgain.cfg.val));
    cfg:set(anc_config.r_fbgain.show_cfg, float_2_db(anc_config.r_fbgain.cfg.val));
    cfg:set(anc_config.r_transgain.show_cfg, float_2_db(anc_config.r_transgain.cfg.val));
    cfg:set(anc_config.r_cmpgain.show_cfg, float_2_db(anc_config.r_cmpgain.cfg.val));

    cfg:set(anc_config.drcff_lgain.show_cfg, int_2_db(anc_config.drcff_lgain.cfg.val));
    cfg:set(anc_config.drcff_hgain.show_cfg, int_2_db(anc_config.drcff_hgain.cfg.val));
    cfg:set(anc_config.drcff_norgain.show_cfg, int_2_db(anc_config.drcff_norgain.cfg.val));
    cfg:set(anc_config.drcfb_lgain.show_cfg, int_2_db(anc_config.drcfb_lgain.cfg.val));
    cfg:set(anc_config.drcfb_hgain.show_cfg, int_2_db(anc_config.drcfb_hgain.cfg.val));
    cfg:set(anc_config.drcfb_norgain.show_cfg, int_2_db(anc_config.drcfb_norgain.cfg.val));
    cfg:set(anc_config.drctrans_lgain.show_cfg, int_2_db(anc_config.drctrans_lgain.cfg.val));
    cfg:set(anc_config.drctrans_hgain.show_cfg, int_2_db(anc_config.drctrans_hgain.cfg.val));
    cfg:set(anc_config.drctrans_norgain.show_cfg, int_2_db(anc_config.drctrans_norgain.cfg.val));
end

--========================= ANC 参数输出汇总  ============================

anc_config.output.anc_coeff_config_items = {
	anc_config.coeff_header.cfg,
	anc_config.coeff.cfg,
};

anc_config.output.anc_config_items = {
	anc_config.header.cfg,
    anc_config.version.cfg,
    anc_config.dac_gain.cfg,
    anc_config.l_ffmic_gain.cfg,
    anc_config.l_fbmic_gain.cfg,
    anc_config.cmp_en.cfg,
    anc_config.drc_en.cfg,
    anc_config.ahs_en.cfg,
    anc_config.ff_1st_dcc.cfg,
    anc_config.gain_sign.cfg,
    anc_config.noise_lvl.cfg,
    anc_config.fade_step.cfg,
    anc_config.sr.cfg,
    anc_config.trans_sr.cfg,
    anc_config.l_ffgain.cfg,
    anc_config.l_fbgain.cfg,
    anc_config.l_transgain.cfg,
    anc_config.l_cmpgain.cfg,
    anc_config.r_ffgain.cfg,
    anc_config.r_fbgain.cfg,
    anc_config.r_transgain.cfg,
    anc_config.r_cmpgain.cfg,
    anc_config.drcff_zero_det.cfg,
    anc_config.drcff_dat_mode.cfg,
    anc_config.drcff_lpf_sel.cfg,
    anc_config.drcfb_zero_det.cfg,
    anc_config.drcfb_dat_mode.cfg,
    anc_config.drcfb_lpf_sel.cfg,
    anc_config.drcff_lthr.cfg,
    anc_config.drcff_hthr.cfg,
    anc_config.drcff_lgain.cfg,
    anc_config.drcff_hgain.cfg,
    anc_config.drcff_norgain.cfg,
    anc_config.drcfb_lthr.cfg,
    anc_config.drcfb_hthr.cfg,
    anc_config.drcfb_lgain.cfg,
    anc_config.drcfb_hgain.cfg,
    anc_config.drcfb_norgain.cfg,
    anc_config.drctrans_lthr.cfg,
    anc_config.drctrans_hthr.cfg,
    anc_config.drctrans_lgain.cfg,
    anc_config.drctrans_hgain.cfg,
    anc_config.drctrans_norgain.cfg,
    anc_config.ahs_dly.cfg,
    anc_config.ahs_tap.cfg,
    anc_config.ahs_wn_shift.cfg,
    anc_config.ahs_wn_sub.cfg,
    anc_config.ahs_shift.cfg,
    anc_config.ahs_u.cfg,
    anc_config.ahs_gain.cfg,
    anc_config.ahs_nlms_sel.cfg,
    anc_config.developer_mode.cfg,
    anc_config.audio_drc_thr.cfg,
    anc_config.r_ffmic_gain.cfg,
    anc_config.r_fbmic_gain.cfg,
    anc_config.fb_1st_dcc.cfg,
    anc_config.ff_2nd_dcc.cfg,
    anc_config.fb_2nd_dcc.cfg,
    anc_config.drc_ff_2dcc.cfg,
    anc_config.drc_fb_2dcc.cfg,
    anc_config.reserver_1bytes_1.cfg,
    anc_config.drc_dcc_det_time.cfg,
    anc_config.drc_dcc_res_time.cfg,
    anc_config.dummy_cfg.cfg,
};

anc_config.output.anc_coeff_config_output_views = {
	anc_config.coeff_header.view,
	anc_config.coeff.view,
};

anc_config.output.anc_common_config_output_group_views = cfg:stGroup("COMMON",
    cfg:vaBox {
        anc_config.dac_gain.hbox_view,
        anc_config.l_ffmic_gain.hbox_view,
        anc_config.l_fbmic_gain.hbox_view,
        anc_config.r_ffmic_gain.hbox_view,
        anc_config.r_fbmic_gain.hbox_view,

        anc_config.cmp_en.hbox_view,
        anc_config.gain_sign.hbox_view,
        anc_config.noise_lvl.hbox_view,
        anc_config.fade_step.hbox_view,
        anc_config.sr.hbox_view,
        anc_config.trans_sr.hbox_view,

        --anc_config.ff_1st_dcc.hbox_view,
        --anc_config.fb_1st_dcc.hbox_view,
        anc_config.ff_2nd_dcc.hbox_view,
        anc_config.fb_2nd_dcc.hbox_view,

        --调试DB, 存储Float
        anc_config.l_ffgain.hbox_view,
        anc_config.l_fbgain.hbox_view,
        anc_config.l_transgain.hbox_view,
        anc_config.l_cmpgain.hbox_view,
        anc_config.r_ffgain.hbox_view,
        anc_config.r_fbgain.hbox_view,
        anc_config.r_transgain.hbox_view,
        anc_config.r_cmpgain.hbox_view,
    }
);

anc_config.output.anc_drc_config_output_group_views = cfg:stGroup("DRC",
    cfg:vaBox {
        --以下所有配置项不显示
        -- anc_config.drcff_zero_det.hbox_view,
        -- anc_config.drcff_dat_mode.hbox_view,
        -- anc_config.drcff_lpf_sel.hbox_view,
        -- anc_config.drcfb_zero_det.hbox_view,
        -- anc_config.drcfb_dat_mode.hbox_view,
        -- anc_config.drcfb_lpf_sel.hbox_view,

        anc_config.drc_en.hbox_view,
        anc_config.drcff_lthr.hbox_view,
        anc_config.drcff_hthr.hbox_view,
        anc_config.drcff_lgain.hbox_view,
        anc_config.drcff_hgain.hbox_view,
        anc_config.drcff_norgain.hbox_view,
        anc_config.drcfb_lthr.hbox_view,
        anc_config.drcfb_hthr.hbox_view,
        anc_config.drcfb_lgain.hbox_view,
        anc_config.drcfb_hgain.hbox_view,
        anc_config.drcfb_norgain.hbox_view,
        anc_config.drctrans_lthr.hbox_view,
        anc_config.drctrans_hthr.hbox_view,
        anc_config.drctrans_lgain.hbox_view,
        anc_config.drctrans_hgain.hbox_view,
        anc_config.drctrans_norgain.hbox_view,

        anc_config.drc_ff_2dcc.hbox_view,
        anc_config.drc_fb_2dcc.hbox_view,
        anc_config.drc_dcc_det_time.hbox_view,
        anc_config.drc_dcc_res_time.hbox_view,
    }
);

anc_config.output.anc_ahs_config_output_group_views = cfg:stGroup("AHS",
    cfg:vaBox {
        -- anc_config.ahs_dly.hbox_view,
        -- anc_config.ahs_tap.hbox_view,
        anc_config.ahs_en.hbox_view,
        anc_config.ahs_wn_shift.hbox_view,
        -- anc_config.ahs_nlms_sel.hbox_view,
        -- anc_config.ahs_wn_sub.hbox_view,
        -- anc_config.ahs_shift.hbox_view,
        -- anc_config.ahs_u.hbox_view,
        -- anc_config.ahs_gain.hbox_view,
    }
);

anc_config.output.anc_config_output_views = {
	anc_config.header.view,
    anc_config.reference_book_view,	
    --anc_config.version.hbox_view,
    
    anc_config.output.anc_common_config_output_group_views,
    anc_config.output.anc_drc_config_output_group_views,
    anc_config.output.anc_ahs_config_output_group_views,
 
    -- anc_config.reserved_1.hbox_view,
    -- anc_config.audio_drc_thr.hbox_view,

	cfg:stSpacer()
};

-- A. 输出htext
anc_config.output.anc_config_htext_output_table = {
};

-- B. 输出ctext：无

-- C. 输出bin
anc_config.output.anc_config_output_bin = cfg:group("anc_config",
	BIN_ONLY_CFG["HW_CFG"].anc_config.id,
	1,
    anc_config.output.anc_config_items
);

anc_config.output.anc_coeff_config_output_bin = cfg:group("anc_coeff_config",
	BIN_ONLY_CFG["HW_CFG"].anc_coeff_config.id,
	1,
	anc_config.output.anc_coeff_config_items
);

anc_config.output.layout = cfg:vBox(anc_config.output.anc_config_output_views);
anc_config.output.coeff_layout = cfg:vBox(anc_config.output.anc_coeff_config_output_views);

-- D. 显示
anc_config.output.anc_config_group_view = cfg:stGroup("ANC 参数配置",
	anc_config.output.layout
);

anc_config.output.anc_coeff_config_group_view = cfg:stGroup("ANC 系数配置",
	anc_config.output.coeff_layout
);

-- E. 默认值, 见汇总

-- F. bindGroup
cfg:bindStGroup(anc_config.output.anc_config_group_view, anc_config.output.anc_config_output_bin);
cfg:bindStGroup(anc_config.output.anc_coeff_config_group_view, anc_config.output.anc_coeff_config_output_bin);

--[[===================================================================================
==================================== 模块返回汇总 =====================================
====================================================================================--]]
-- A. 输出htext
--[[
-- AEC
insert_list_to_list(anc_output_htext_tabs, aec_output_htext_table);
--]]
-- B. 输出ctext：无
-- C. 输出bin：无

-- E. 默认值

local anc_default_button_view = cfg:stButton(" ANC配置恢复默认值 ", reset_to_default_with_hook(anc_config.output.anc_config_items, recover_show_cfg_value));


local anc_file_info = cfg:addFirmwareFileNoLayout("ANCIF",
		"ANC配置",
		2, -- 文件类型，是个bin文件
		{ anc_config.output.anc_config_output_bin } -- 组的列表
);

local anc_coeff_file_info = cfg:addFirmwareFileNoLayout("ANCIF1",
		"ANC系数",
		2, -- 文件类型，是个bin文件
		{ anc_config.output.anc_coeff_config_output_bin } -- 组的列表
);


anc_config.ui.load_button = cfg:stButton("加载ANC参数", function ()
	local filepath = cfg:utilsGetOpenFilePath("选择ANC参数", "", "ANC 参数 (*.bin)");
	if string.len(filepath) ~= 0 then
		anc_file_info:load(filepath);
	end
    recover_show_cfg_value();
end);

anc_config.ui.load_coeff_button = cfg:stButton("加载ANC系数", function ()
	local filepath = cfg:utilsGetOpenFilePath("选择ANC系数", "", "ANC 系数 (*.bin)");
	if string.len(filepath) ~= 0 then
		anc_coeff_file_info:load(filepath);
	end
end);


anc_config.ui.save_button = cfg:stButton("保存ANC参数", function ()
	if open_by_program == "create" then
		anc_file_info:save(bin_out_path .. 'anc_gains.bin');
	else
		local filepath = cfg:utilsGetSaveFilePath("保存ANC参数文件", "anc_gains.bin", "ANC 参数 (*.bin)");
		if string.len(filepath) ~= 0 then
			anc_file_info:save(filepath);
		end
	end
end);

anc_config.ui.save_coeff_button = cfg:stButton("保存ANC系数", function ()
	if open_by_program == "create" then
		anc_coeff_file_info:save(bin_out_path .. 'anc_coeff.bin');
	else
		local filepath = cfg:utilsGetOpenFilePath("保存ANC系数文件", "anc_coeff.bin", "ANC 系数 (*.bin)");
		if string.len(filepath) ~= 0 then
			anc_coeff_file_info:save(filepath);
		end
	end
end);

if open_by_program == "create" then
end

anc_output_vbox_view = cfg:vBox {
    cfg:stHScroll(cfg:vBox{ anc_config.output.anc_config_group_view }),
	cfg:hBox{ anc_config.ui.load_button, anc_config.ui.save_button },
    anc_default_button_view,
};

anc_coeff_output_box_view = cfg:vBox {
	cfg:vBox{ anc_config.output.anc_coeff_config_group_view },
	cfg:hBox{ anc_config.ui.load_coeff_button },
};

anc_file_info:setAttr("layout", anc_output_vbox_view);
anc_file_info:setAttr("binaryFormat", "old");

anc_coeff_file_info:setAttr("layout", anc_coeff_output_box_view);
anc_coeff_file_info:setAttr("binaryFormat", "old");

anc_output_combine_vbox_view = cfg:stTab {
	{ "ANC参数", anc_output_vbox_view },
	{ "ANC系数", anc_coeff_output_box_view },
};

end

