if enable_moudles["bluetooth"] == false then
    return;
else

--[[===================================================================================
================================= 模块全局变量列表 ====================================
====================================================================================--]]

--[[===================================================================================
================================= 配置子项1: 注释 =====================================
====================================================================================--]]
local MOUDLE_COMMENT_NAME = "//                                  蓝牙配置                                       //"
local comment_begin = cfg:i32("bluetooth注释开始", 0);
local bt_comment_begin_htext = module_output_comment(comment_begin, MOUDLE_COMMENT_NAME);


--[[===================================================================================
=============================== 配置子项2: 蓝牙配置使能 ===============================
====================================================================================--]]
local bluetooth_en_init_val = 1;
if open_by_program == "fw_edit" then bluetooth_en_init_val = 1 end

local bluetooth_en = cfg:i32("蓝牙配置使能开关:", bluetooth_en_init_val);
local bt_en_comment_str = item_comment_context("蓝牙模块使能", ENABLE_SWITCH_TABLE);
-- A. 输出htext
local bt_en_htext = item_output_htext(bluetooth_en, "TCFG_BT_ENABLE", 6, ENABLE_SWITCH_TABLE, bt_en_comment_str, 1);
-- B. 输出ctext：无
-- C. 输出bin：无
-- D. 显示
local bt_module_en_hbox_view = cfg:hBox {
		cfg:stLabel(bluetooth_en.name), 
		cfg:checkBoxView(bluetooth_en),
        cfg:stSpacer();
};

-- E. 默认值, 见汇总

-- F. bindGroup：无

--[[===================================================================================
=============================== 配置子项3: 蓝牙名称配置 ===============================
====================================================================================--]]
-- 若干个蓝牙名字
local bluenames = {}
local bluename_switchs = {}
local bluename_layouts = {}
local bluenames_bin_cfgs = {}
for i = 1, 20 do
	local c = cfg:str("蓝牙名字" .. i, "AC701N_" .. i);
	local on = 0;
	if i == 1 then on = 1 end;
	local onc = cfg:i32("蓝牙名字开关" .. i, on);
	onc:setOSize(1);
	c:setOSize(32); -- 最长 32
	c:addDeps{bluetooth_en, onc};
	c:setShow(on);
	c:setDepChangeHook(function () c:setShow(bluetooth_en.val ~= 0 and onc.val ~= 0) end);
	
	local l = cfg:hBox{ cfg:stLabel("开关："), cfg:checkBoxView(onc),  cfg:stLabel(c.name), cfg:inputMaxLenView(c, 24)};
	
	-- bluenames[#bluenames+1] = c;
	-- bluename_switchs[#bluename_switchs+1] = onc;
	bluename_layouts[#bluename_layouts+1] = l;
	
	bluenames_bin_cfgs[#bluenames_bin_cfgs+1] = onc;
	bluenames_bin_cfgs[#bluenames_bin_cfgs+1] = c;
end

-- A. 输出htext
bluenames_bin_cfgs[2]:setTextOut(
	function (ty) 
		if (ty == 3) then
			return "#define TCFG_BT_NAME" .. TAB_TABLE[6] .. "\"" .. bluenames_bin_cfgs[2].val .. "\"" .. TAB_TABLE[1]  .. "//蓝牙名字" .. NLINE_TABLE[1]
		end
	end
);

local bt_name_htext = bluenames_bin_cfgs[2];

-- B. 输出ctext：无

-- C. 输出bin
local bluenames_output_bin = cfg:group("bluetooth names bin",
	MULT_AREA_CFG.bt_name.id,
	1,
	bluenames_bin_cfgs
);

-- D. 显示
local bluenames_group_view = cfg:stGroup("蓝牙名字", cfg:vaBox(bluename_layouts));

-- E. 默认值, 无

-- F. bindGroup
cfg:bindStGroup(bluenames_group_view, bluenames_output_bin); -- 绑定 UI 显示和 group


--[[===================================================================================
=============================== 配置子项4: MAC地址配置 ================================
====================================================================================--]]
local bluetooth_mac = cfg:mac("蓝牙MAC地址:", {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF});
depend_item_en_show(bluetooth_en, bluetooth_mac);
-- A. 输出htext
bluetooth_mac:setTextOut(
	function (ty) 
		if (ty == 3) then
            return "#define TCFG_BT_MAC" .. TAB_TABLE[7] .. 
            "{" .. item_val_dec2hex(bluetooth_mac.val[1]) .. ", " .. item_val_dec2hex(bluetooth_mac.val[2]) .. ", "
                .. item_val_dec2hex(bluetooth_mac.val[3]) .. ", " .. item_val_dec2hex(bluetooth_mac.val[4]) .. ", "
                .. item_val_dec2hex(bluetooth_mac.val[5]) .. ", " .. item_val_dec2hex(bluetooth_mac.val[6]) .. "}"
                .. TAB_TABLE[1] .. "//蓝牙MAC地址" .. NLINE_TABLE[1]
		end
	end
);

local bt_mac_htext = bluetooth_mac;

-- B. 输出ctext：无

-- C. 输出bin
local bt_mac_output_bin = cfg:group("bluetooth mac bin",
	MULT_AREA_CFG.bt_mac.id,
	1,
	{
		bluetooth_mac,
	}
);

-- D. 显示
local bt_mac_group_view = cfg:stGroup("",
	cfg:hBox {
        cfg:stLabel("蓝牙MAC地址："),
		cfg:macAddrView(bluetooth_mac),
        cfg:stSpacer(),
	}
);

-- E. 默认值, 见汇总

-- F. bindGroup：无
cfg:bindStGroup(bt_mac_group_view, bt_mac_output_bin); -- 绑定 UI 显示和 group

--[[===================================================================================
============================= 配置子项5: 蓝牙发射功率配置 =============================
====================================================================================--]]
local bluetooth_rf_power = cfg:i32("蓝牙发射功率：", 10);
depend_item_en_show(bluetooth_en, bluetooth_rf_power);
bluetooth_rf_power:setOSize(1);
-- A. 输出htext
local bt_rf_power_htext = item_output_htext(bluetooth_rf_power, "TCFG_BT_RF_POWER", 5, nil, "蓝牙发射功率", 2);

-- B. 输出ctext：无

-- C. 输出bin：
local bt_rf_power_output_bin = cfg:group("rf power bin",
	BIN_ONLY_CFG["BT_CFG"].rf_power.id,
	1,
	{
		bluetooth_rf_power,
	}
);

-- D. 显示
local bt_rf_power_group_view = cfg:stGroup("",
	cfg:hBox {
			cfg:stLabel(bluetooth_rf_power.name),
			cfg:ispinView(bluetooth_rf_power, 0, 10, 1),
            cfg:stLabel("(设置范围: 0 ~ 10)");
            cfg:stSpacer(),
	});

-- E. 默认值, 见汇总

-- F. bindGroup：无
cfg:bindStGroup(bt_rf_power_group_view, bt_rf_power_output_bin);

--[[===================================================================================
================================= 配置子项6: AEC配置 ==================================
====================================================================================--]]

local function aec_cfg_depend_show(dep_cfg, val_show, view)
    new_aec_cfg:addDeps({dep_cfg});
    new_aec_cfg:setDepChangeHook(function()
        if (dep_cfg.val == val_show) then
            view:setHide(false);
        else
            view:setHide(true);
        end
    end
    );
end
-- aec gloal

local aec_global = {};

local aec_cfg_type = {
    TY = { aec = 0, dms = 1, aec_dns = 2, dms_dns = 3, dms_flexible = 4 }
};

aec_cfg_type.aec_cfg_type_talbe = {
        [aec_cfg_type.TY.aec] = " 单mic模式",
        [aec_cfg_type.TY.dms] = " 双mic模式",
        [aec_cfg_type.TY.aec_dns] = " 单mic模式(DNS)",
        [aec_cfg_type.TY.dms_dns] = " 双mic模式(DNS)",
        [aec_cfg_type.TY.dms_flexible] = " 双mic话务耳机",
};

aec_cfg_type.AEC_CFG_TYPE_SEL_TABLE = cfg:enumMap("aec_cfg_type_table", aec_cfg_type.aec_cfg_type_talbe);

aec_cfg_type.cfg = cfg:enum("通话参数类型选择: ", aec_cfg_type.AEC_CFG_TYPE_SEL_TABLE, 0);
aec_cfg_type.cfg:setOSize(1);

-- A. 输出htext
-- B. 输出ctext：无
-- C. 输出bin：无
-- D. 显示
aec_cfg_type.hbox_view = cfg:hBox {
		cfg:stLabel(aec_cfg_type.cfg.name),
		cfg:enumView(aec_cfg_type.cfg),
        cfg:stSpacer();
};

-- E. 默认值, 见汇总

-- F. bindGroup：无

-- item_htext
aec_global.aec_start_comment_str = module_comment_context("AEC参数");

aec_global.reference_book_view = cfg:stButton("JL通话调试手册.pdf",
    function()
        local ret = cfg:utilsShellOpenFile(aec_reference_book_path);
        if (ret == false) then
            if cfg.lang == "zh" then
		        cfg:msgBox("info", aec_reference_book_path .. "文件不存在");
            else
		        cfg:msgBox("info", aec_reference_book_path .. " file not exist.");
            end
        end
    end);

-- 1) mic_again
aec_global.mic_analog_gain = {};
aec_global.mic_analog_gain.cfg = cfg:i32("MIC_AGAIN：", 8);
depend_item_en_show(bluetooth_en, aec_global.mic_analog_gain.cfg);
aec_global.mic_analog_gain.cfg:setOSize(1);
-- item_htext
aec_global.mic_analog_gain.comment_str = "设置范围：0 ~ 19, 默认值：8";
aec_global.mic_analog_gain.htext = item_output_htext(aec_global.mic_analog_gain.cfg, "TCFG_AEC_MIC_ANALOG_GAIN", 3, nil, aec_global.mic_analog_gain.comment_str, 1);
-- item_view
aec_global.mic_analog_gain.hbox_view = cfg:hBox {
    cfg:stLabel(aec_global.mic_analog_gain.cfg.name .. TAB_TABLE[1]),
    cfg:ispinView(aec_global.mic_analog_gain.cfg, 0, 19, 1),
    cfg:stLabel("(MIC增益，0(-8dB)) ~ 19(30dB),步进：2dB 默认值：8)"),
    cfg:stSpacer(),
};


-- 2) dac_again
aec_global.dac_analog_gain = {};
aec_global.dac_analog_gain.cfg = cfg:i32("DAC_AGAIN：", 8);
depend_item_en_show(bluetooth_en, aec_global.dac_analog_gain.cfg);
aec_global.dac_analog_gain.cfg:setOSize(1);
-- item_htext
aec_global.dac_analog_gain.comment_str = "设置范围：0 ~ 15, 默认值：8";
aec_global.dac_analog_gain.htext = item_output_htext(aec_global.dac_analog_gain.cfg, "TCFG_AEC_DAC_ANALOG_GAIN", 3, nil, aec_global.dac_analog_gain.comment_str, 1);
-- item_view
aec_global.dac_analog_gain.hbox_view = cfg:hBox {
    cfg:stLabel(aec_global.dac_analog_gain.cfg.name .. TAB_TABLE[1]),
    cfg:ispinView(aec_global.dac_analog_gain.cfg, 0, 15, 1),
    cfg:stLabel("(DAC 增益，设置范围: 0 ~ 15，步进：2dB 默认值：8)");
    cfg:stSpacer(),
};

-- 3) aec_mode
local aec_mode_sel_table = {
        [0] = " disable",
        [1] = " reduce",
        [2] = " advance",
};

local AEC_MODE_SEL_TABLE = cfg:enumMap("aec_mode_sel_table", aec_mode_sel_table);
aec_global.aec_mode = {};
aec_global.aec_mode.cfg = cfg:enum("AEC_MODE: ", AEC_MODE_SEL_TABLE, 1);
depend_item_en_show(bluetooth_en, aec_global.aec_mode.cfg);
aec_global.aec_mode.cfg:setOSize(1);
-- item_htext
aec_global.aec_mode.comment_str = item_comment_context("AEC_MODE", aec_mode_sel_table) .. "默认值：2";
aec_global.aec_mode.htext = item_output_htext(aec_global.aec_mode.cfg, "TCFG_AEC_MODE", 6, nil, aec_global.aec_mode.comment_str, 1);
-- item_view
aec_global.aec_mode.hbox_view = cfg:hBox {
    cfg:stLabel(aec_global.aec_mode.cfg.name .. TAB_TABLE[1]),
    cfg:enumView(aec_global.aec_mode.cfg),
    cfg:stLabel("(AEC 模式，默认值：reduce)"),
    cfg:stSpacer(),
};

-- 4) ul_eq_en
local ul_eq_en_sel_table = {
        [0] = " disable",
        [1] = " enable",
};
local UL_EQ_EN_SEL_TABLE = cfg:enumMap("ul_eq_en_sel_table", ul_eq_en_sel_table);

aec_global.ul_eq_en = {};
aec_global.ul_eq_en.cfg = cfg:enum("UL_EQ_EN: ", UL_EQ_EN_SEL_TABLE, 1);
depend_item_en_show(bluetooth_en, aec_global.ul_eq_en.cfg);
aec_global.ul_eq_en.cfg:setOSize(1);
-- item_htext
aec_global.ul_eq_en.comment_str = item_comment_context("UL_EQ_EN", ul_eq_en_sel_table) .. "默认值：1";
aec_global.ul_eq_en.htext = item_output_htext(aec_global.ul_eq_en.cfg, "TCFG_AEC_UL_EQ_EN", 5, nil, aec_global.ul_eq_en.comment_str, 2);
-- item_view
aec_global.ul_eq_en.hbox_view = cfg:hBox {
    cfg:stLabel(aec_global.ul_eq_en.cfg.name .. TAB_TABLE[1]),
    cfg:enumView(aec_global.ul_eq_en.cfg),
    cfg:stLabel("(上行 EQ 使能，默认值：enable)"),
    cfg:stSpacer(),
};


-- 5) fade_gain
aec_global.fade_gain = {ndt_fade_in = {}, ndt_fade_out = {}, dt_fade_in = {}, dt_fade_out = {}};
-- 5-1) ndt_fade_in
aec_global.fade_gain.ndt_fade_in.cfg = cfg:dbf("NDT_FADE_IN：", 1.3);
depend_item_en_show(bluetooth_en, aec_global.fade_gain.ndt_fade_in.cfg);
aec_global.fade_gain.ndt_fade_in.cfg:setOSize(4);
-- item_htext
aec_global.fade_gain.ndt_fade_in.comment_str = "设置范围：0.1 ~ 5 dB, float类型(4Bytes), 默认值：1.3";
aec_global.fade_gain.ndt_fade_in.htext = item_output_htext(aec_global.fade_gain.ndt_fade_in.cfg, "TCFG_NDT_FADE_IN", 4, nil, aec_global.fade_gain.ndt_fade_in.comment_str, 1);
-- item_view
aec_global.fade_gain.ndt_fade_in.hbox_view = cfg:hBox {
    cfg:stLabel(aec_global.fade_gain.ndt_fade_in.cfg.name .. TAB_TABLE[1]),
    cfg:dspinView(aec_global.fade_gain.ndt_fade_in.cfg, 0.1, 5.0, 0.1, 1),
    cfg:stLabel("dB (单端讲话淡入步进，设置范围: 0.1 ~ 5 dB，默认值：1.3 dB)"),
    cfg:stSpacer(),
};

-- 5-2) ndt_fade_out
aec_global.fade_gain.ndt_fade_out.cfg = cfg:dbf("NDT_FADE_OUT：", 0.7);
depend_item_en_show(bluetooth_en, aec_global.fade_gain.ndt_fade_out.cfg);
aec_global.fade_gain.ndt_fade_out.cfg:setOSize(4);
-- item_htext
aec_global.fade_gain.ndt_fade_out.comment_str = "设置范围：0.1 ~ 5 dB, float类型(4Bytes), 默认值：0.7";
aec_global.fade_gain.ndt_fade_out.htext = item_output_htext(aec_global.fade_gain.ndt_fade_out.cfg, "TCFG_NDT_FADE_OUT", 4, nil, aec_global.fade_gain.ndt_fade_out.comment_str, 1);
-- item_view
aec_global.fade_gain.ndt_fade_out.hbox_view = cfg:hBox {
    cfg:stLabel(aec_global.fade_gain.ndt_fade_out.cfg.name .. TAB_TABLE[1]),
    cfg:dspinView(aec_global.fade_gain.ndt_fade_out.cfg, 0.1, 5.0, 0.1, 1),
    cfg:stLabel("dB (单端讲话淡出步进，设置范围: 0.1 ~ 5 dB，默认值：0.7 dB)"),
    cfg:stSpacer(),
};

-- 5-3) dt_fade_in
aec_global.fade_gain.dt_fade_in.cfg = cfg:dbf("DT_FADE_IN：", 1.3);
depend_item_en_show(bluetooth_en, aec_global.fade_gain.dt_fade_in.cfg);
aec_global.fade_gain.dt_fade_in.cfg:setOSize(4);
-- item_htext
aec_global.fade_gain.dt_fade_in.comment_str = "设置范围：0.1 ~ 5 dB, float类型(4Bytes), 默认值：1.3";
aec_global.fade_gain.dt_fade_in.htext = item_output_htext(aec_global.fade_gain.dt_fade_in.cfg, "TCFG_DT_FADE_IN", 4, nil, aec_global.fade_gain.dt_fade_in.comment_str, 1);
-- item_view
aec_global.fade_gain.dt_fade_in.hbox_view = cfg:hBox {
    cfg:stLabel(aec_global.fade_gain.dt_fade_in.cfg.name .. TAB_TABLE[1]),
    cfg:dspinView(aec_global.fade_gain.dt_fade_in.cfg, 0.1, 5.0, 0.1, 1),
    cfg:stLabel("dB (双端讲话淡入步进，设置范围: 0.1 ~ 5 dB，默认值：1.3 dB)"),
    cfg:stSpacer(),
};

-- 5-4) dt_fade_out
aec_global.fade_gain.dt_fade_out.cfg = cfg:dbf("DT_FADE_OUT：", 0.7);
depend_item_en_show(bluetooth_en, aec_global.fade_gain.dt_fade_out.cfg);
aec_global.fade_gain.dt_fade_out.cfg:setOSize(4);
-- item_htext
aec_global.fade_gain.dt_fade_out.comment_str = "设置范围：0.1 ~ 5 dB, float类型(4Bytes), 默认值：0.7";
aec_global.fade_gain.dt_fade_out.htext = item_output_htext(aec_global.fade_gain.dt_fade_out.cfg, "TCFG_DT_FADE_OUT", 4, nil, aec_global.fade_gain.dt_fade_out.comment_str, 1);
-- item_view
aec_global.fade_gain.dt_fade_out.hbox_view = cfg:hBox {
    cfg:stLabel(aec_global.fade_gain.dt_fade_out.cfg.name .. TAB_TABLE[1]),
    cfg:dspinView(aec_global.fade_gain.dt_fade_out.cfg, 0.1, 5.0, 0.1, 1),
    cfg:stLabel("dB (双端讲话淡出步进，设置范围: 0.1 ~ 5 dB，默认值：0.7 dB)"),
    cfg:stSpacer(),
};

-- 6) ndt_max_gain
aec_global.ndt_max_gain = {};
aec_global.ndt_max_gain.cfg = cfg:dbf("NDT_MAX_GAIN：", 8.0);
depend_item_en_show(bluetooth_en, aec_global.ndt_max_gain.cfg);
aec_global.ndt_max_gain.cfg:setOSize(4);
-- item_htext
aec_global.ndt_max_gain.comment_str = "设置范围：-90.0 ~ 40.0 dB, 默认值：8.0 dB";
aec_global.ndt_max_gain.htext = item_output_htext(aec_global.ndt_max_gain.cfg, "TCFG_AEC_NDT_MAX_GAIN", 4, nil, aec_global.ndt_max_gain.comment_str, 1);
-- item_view
aec_global.ndt_max_gain.hbox_view = cfg:hBox {
    cfg:stLabel(aec_global.ndt_max_gain.cfg.name .. TAB_TABLE[1]),
    cfg:dspinView(aec_global.ndt_max_gain.cfg, -90.0, 40.0, 0.1, 1),
    cfg:stLabel("(单端讲话放大上限， 设置范围: -90.0 ~ 40.0 dB，默认值：8.0 dB)"),
    cfg:stSpacer(),
};

-- 7) ndt_min_gain
aec_global.ndt_min_gain = {};
aec_global.ndt_min_gain.cfg = cfg:dbf("NDT_MIN_GAIN：", 4.0);
depend_item_en_show(bluetooth_en, aec_global.ndt_min_gain.cfg);
aec_global.ndt_min_gain.cfg:setOSize(4);
-- item_htext
aec_global.ndt_min_gain.comment_str = "设置范围：-90.0 ~ 40.0 dB, 默认值：4.0 dB";
aec_global.ndt_min_gain.htext = item_output_htext(aec_global.ndt_min_gain.cfg, "TCFG_AEC_NDT_MIN_GAIN", 4, nil, aec_global.ndt_min_gain.comment_str, 1);
-- item_view
aec_global.ndt_min_gain.hbox_view = cfg:hBox {
    cfg:stLabel(aec_global.ndt_min_gain.cfg.name .. TAB_TABLE[1]),
    cfg:dspinView(aec_global.ndt_min_gain.cfg, -90.0, 40.0, 0.1, 1),
    cfg:stLabel("(单端讲话放大下限， 设置范围: -90.0 ~ 40.0 dB，默认值：4.0 dB)"),
    cfg:stSpacer(),
};

-- 8) ndt_speech_thr
aec_global.ndt_speech_thr = {};
aec_global.ndt_speech_thr.cfg = cfg:dbf("NDT_SPEECH_THR：", -50.0);
depend_item_en_show(bluetooth_en, aec_global.ndt_speech_thr.cfg);
aec_global.ndt_speech_thr.cfg:setOSize(4);
-- item_htext
aec_global.ndt_speech_thr.comment_str = "设置范围：-70.0 ~ -40.0 dB, 默认值：-50.0 dB";
aec_global.ndt_speech_thr.htext = item_output_htext(aec_global.ndt_speech_thr.cfg, "TCFG_AEC_NDT_SPEECH_THR", 4, nil, aec_global.ndt_speech_thr.comment_str, 1);
-- item_view
aec_global.ndt_speech_thr.hbox_view = cfg:hBox {
    cfg:stLabel(aec_global.ndt_speech_thr.cfg.name),
    cfg:dspinView(aec_global.ndt_speech_thr.cfg, -70.0, -40.0, 0.1, 1),
    cfg:stLabel("(单端讲话放大阈值， 设置范围: -70.0 ~ -40.0 dB，默认值：-50.0 dB)"),
    cfg:stSpacer(),
};


-- 9) dt_max_gain 
aec_global.dt_max_gain = {};
aec_global.dt_max_gain.cfg = cfg:dbf("DT_MAX_GAIN：", 12.0);
depend_item_en_show(bluetooth_en, aec_global.dt_max_gain.cfg);
aec_global.dt_max_gain.cfg:setOSize(4);
-- item_htext
aec_global.dt_max_gain.comment_str = "设置范围：-90.0 ~ 40.0 dB, 默认值：12.0 dB";
aec_global.dt_max_gain.htext = item_output_htext(aec_global.dt_max_gain.cfg, "TCFG_AEC_DT_MAX_GAIN", 4, nil, aec_global.dt_max_gain.comment_str, 1);
-- item_view
aec_global.dt_max_gain.hbox_view = cfg:hBox {
    cfg:stLabel(aec_global.dt_max_gain.cfg.name .. TAB_TABLE[1]),
    cfg:dspinView(aec_global.dt_max_gain.cfg, -90.0, 40.0, 0.1, 1),
    cfg:stLabel("(双端讲话放大上限， 设置范围: -90.0 ~ 40.0 dB，默认值：12.0 dB)"),
    cfg:stSpacer(),
};

-- 10) dt_min_gain
aec_global.dt_min_gain = {};
aec_global.dt_min_gain.cfg = cfg:dbf("DT_MIN_GAIN：", 0.0);
depend_item_en_show(bluetooth_en, aec_global.dt_min_gain.cfg);
aec_global.dt_min_gain.cfg:setOSize(4);
-- item_htext
aec_global.dt_min_gain.comment_str = "设置范围：-90.0 ~ 40.0 dB, 默认值：0 dB";
aec_global.dt_min_gain.htext = item_output_htext(aec_global.dt_min_gain.cfg, "TCFG_AEC_DT_MIN_GAIN", 4, nil, aec_global.dt_min_gain.comment_str, 1);
-- item_view
aec_global.dt_min_gain.hbox_view = cfg:hBox {
    cfg:stLabel(aec_global.dt_min_gain.cfg.name .. TAB_TABLE[1]),
    cfg:dspinView(aec_global.dt_min_gain.cfg, -90.0, 40.0, 0.1, 1),
    cfg:stLabel("(双端讲话放大下限， 设置范围: -90.0 ~ 40.0 dB，默认值：0 dB)"),
    cfg:stSpacer(),
};


-- 11) dt_speech_thr
aec_global.dt_speech_thr = {};
aec_global.dt_speech_thr.cfg = cfg:dbf("DT_SPEECH_THR：", -40.0);
depend_item_en_show(bluetooth_en, aec_global.dt_speech_thr.cfg);
aec_global.dt_speech_thr.cfg:setOSize(4);
-- item_htext
aec_global.dt_speech_thr.comment_str = "设置范围：-70.0 ~ -40.0 dB, 默认值：-40.0 dB";
aec_global.dt_speech_thr.htext = item_output_htext(aec_global.dt_speech_thr.cfg, "TCFG_AEC_DT_SPEECH_THR", 4, nil, aec_global.dt_speech_thr.comment_str, 1);
-- item_view
aec_global.dt_speech_thr.hbox_view = cfg:hBox {
    cfg:stLabel(aec_global.dt_speech_thr.cfg.name .. TAB_TABLE[1]),
    cfg:dspinView(aec_global.dt_speech_thr.cfg, -70.0, -40.0, 0.1, 1),
    cfg:stLabel("(双端讲话放大阈值， 设置范围: -70.0 ~ -40.0 dB，默认值：-40.0 dB)"),
    cfg:stSpacer(),
};

-- 12) echo_present_thr
aec_global.echo_present_thr = {};
aec_global.echo_present_thr.cfg = cfg:dbf("ECHO_PRESENT_THR：", -70.0);
depend_item_en_show(bluetooth_en, aec_global.echo_present_thr.cfg);
aec_global.echo_present_thr.cfg:setOSize(4);
-- item_htext
aec_global.echo_present_thr.comment_str = "设置范围：-70.0 ~ -40.0 dB, 默认值：-70.0 dB";
aec_global.echo_present_thr.htext = item_output_htext(aec_global.echo_present_thr.cfg, "TCFG_AEC_ECHO_PRESENT_THR", 4, nil, aec_global.echo_present_thr.comment_str, 1);
-- item_view
aec_global.echo_present_thr.hbox_view = cfg:hBox {
    cfg:stLabel(aec_global.echo_present_thr.cfg.name),
    cfg:dspinView(aec_global.echo_present_thr.cfg, -70.0, -40.0, 0.1, 1),
    cfg:stLabel("(单端双端讲话阈值， 设置范围: -70.0 ~ -40.0 dB，默认值：-70.0 dB)"),
    cfg:stSpacer(),
};

-- 13) aec_dt_aggress
aec_global.aec_dt_aggress = {};
aec_global.aec_dt_aggress.cfg = cfg:dbf("AEC_DT_AGGRES：", 1.0);
depend_item_en_show(bluetooth_en, aec_global.aec_dt_aggress.cfg);
aec_global.aec_dt_aggress.cfg:setOSize(4);
-- item_htext
aec_global.aec_dt_aggress.comment_str = "设置范围：1.0 ~ 5.0, float类型(4Bytes), 默认值：1.0";
aec_global.aec_dt_aggress.htext = item_output_htext(aec_global.aec_dt_aggress.cfg, "TCFG_AEC_AEC_DT_AGGRESS", 4, nil, aec_global.aec_dt_aggress.comment_str, 1);
-- item_view
aec_global.aec_dt_aggress.hbox_view = cfg:hBox {
    cfg:stLabel(aec_global.aec_dt_aggress.cfg.name),
    cfg:dspinView(aec_global.aec_dt_aggress.cfg, 1.0, 5.0, 0.1, 1),
    cfg:stLabel("(原音回音追踪等级， 设置范围: 1.0 ~ 5.0，默认值：1.0)"),
    cfg:stSpacer(),
};

-- 14) aec_refengthr
aec_global.aec_refengthr = {};
aec_global.aec_refengthr.cfg = cfg:dbf("AEC_REFENGTHR：", -70.0);
depend_item_en_show(bluetooth_en, aec_global.aec_refengthr.cfg);
aec_global.aec_refengthr.cfg:setOSize(4);
-- item_htext
aec_global.aec_refengthr.comment_str = "设置范围：-90.0 ~ -60.0 dB, float类型(4Bytes), 默认值：-70.0";
aec_global.aec_refengthr.htext = item_output_htext(aec_global.aec_refengthr.cfg, "TCFG_AEC_REFENGTHR", 4, nil, aec_global.aec_refengthr.comment_str, 1);
-- item_view
aec_global.aec_refengthr.hbox_view = cfg:hBox {
    cfg:stLabel(aec_global.aec_refengthr.cfg.name .. TAB_TABLE[1]),
    cfg:dspinView(aec_global.aec_refengthr.cfg, -90.0, -60.0, 0.1, 1),
    cfg:stLabel("(进入回音消除参考值， 设置范围: -90.0 ~ -60.0 dB，默认值：-70.0 dB)"),
    cfg:stSpacer(),
};

-- 15) es_aggress_factor
aec_global.es_aggress_factor = {};
aec_global.es_aggress_factor.cfg = cfg:dbf("ES_AGGRESS_FACTOR：", -3.0);
depend_item_en_show(bluetooth_en, aec_global.es_aggress_factor.cfg);
aec_global.es_aggress_factor.cfg:setOSize(4);
-- item_htext
aec_global.es_aggress_factor.comment_str = "设置范围：-1.0 ~ -5.0, float类型(4Bytes), 默认值：-3.0";
aec_global.es_aggress_factor.htext = item_output_htext(aec_global.es_aggress_factor.cfg, "TCFG_ES_AGGRESS_FACTOR", 4, nil, aec_global.es_aggress_factor.comment_str, 1);
-- item_view
aec_global.es_aggress_factor.hbox_view = cfg:hBox {
    cfg:stLabel(aec_global.es_aggress_factor.cfg.name),
    cfg:dspinView(aec_global.es_aggress_factor.cfg, -5.0, -1.0, 0.1, 1),
    cfg:stLabel("(回音前级动态压制,越小越强， 设置范围: -5.0 ~ -1.0，默认值：-3.0)"),
    cfg:stSpacer(),
};

-- 16) es_min_suppress
--      //回音后级动态压制,越大越强,default: 4.f(0 ~ 10)
aec_global.es_min_suppress = {};
aec_global.es_min_suppress.cfg = cfg:dbf("ES_MIN_SUPPRESS：", 4.0);
depend_item_en_show(bluetooth_en, aec_global.es_min_suppress.cfg);
aec_global.es_min_suppress.cfg:setOSize(4);
-- item_htext
aec_global.es_min_suppress.comment_str = "设置范围：0 ~ 10.0, float类型(4Bytes), 默认值：4.0";
aec_global.es_min_suppress.htext = item_output_htext(aec_global.es_min_suppress.cfg, "TCFG_ES_MIN_SUPPRESS", 4, nil, aec_global.es_min_suppress.comment_str, 1);
-- item_view
aec_global.es_min_suppress.hbox_view = cfg:hBox {
    cfg:stLabel(aec_global.es_min_suppress.cfg.name),
    cfg:dspinView(aec_global.es_min_suppress.cfg, 0.0, 10.0, 0.1, 1),
    cfg:stLabel("(回音后级静态压制,越大越强， 设置范围: 0 ~ 10.0，默认值：4.0)"),
    cfg:stSpacer(),
};

-- 17) ans_aggress
--  //噪声前级动态压制,越大越强default: 1.25f(1 ~ 2)
aec_global.ans_aggress = {};
aec_global.ans_aggress.cfg = cfg:dbf("ANS_AGGRESS：", 1.25);
depend_item_en_show(bluetooth_en, aec_global.ans_aggress.cfg);
aec_global.ans_aggress.cfg:setOSize(4);
-- item_htext
aec_global.ans_aggress.comment_str = "设置范围：1 ~ 2, float类型(4Bytes), 默认值：1.25";
aec_global.ans_aggress.htext = item_output_htext(aec_global.ans_aggress.cfg, "TCFG_AEC_ANS_AGGRESS", 4, nil, aec_global.ans_aggress.comment_str, 1);
-- item_view
aec_global.ans_aggress.hbox_view = cfg:hBox {
    cfg:stLabel(aec_global.ans_aggress.cfg.name .. TAB_TABLE[1]),
    cfg:dspinView(aec_global.ans_aggress.cfg, 1.0, 2.0, 0.01, 2),
    cfg:stLabel("(噪声前级动态压制,越大越强， 设置范围: 1 ~ 2.0，默认值：1.25)"),
    cfg:stSpacer(),
};

-- 18) ans_suppress
-- float ans_suppress; //噪声后级动态压制,越小越强default: 0.04f(0 ~ 1)
aec_global.ans_suppress = {};
aec_global.ans_suppress.cfg = cfg:dbf("ANS_SUPPRESS：", 0.09);
depend_item_en_show(bluetooth_en, aec_global.ans_suppress.cfg);
aec_global.ans_suppress.cfg:setOSize(4);
-- item_htext
aec_global.ans_suppress.comment_str = "设置范围：0 ~ 1, float类型(4Bytes), 默认值：0.09";
aec_global.ans_suppress.htext = item_output_htext(aec_global.ans_suppress.cfg, "TCFG_AEC_ANS_SUPPRESS", 4, nil, aec_global.ans_suppress.comment_str, 1);
-- item_view
aec_global.ans_suppress.hbox_view = cfg:hBox {
    cfg:stLabel(aec_global.ans_suppress.cfg.name .. TAB_TABLE[1]),
    cfg:dspinView(aec_global.ans_suppress.cfg, 0.0, 1.0, 0.01, 2),
    cfg:stLabel("(噪声后级静态压制,越小越强， 设置范围: 0 ~ 1.0，默认值：0.09)"),
    cfg:stSpacer(),
};

--========================= AEC输出汇总  ============================
aec_global.aec_item_table = {
    -- aec cfg
        aec_global.mic_analog_gain.cfg, -- 1 Bytes
        aec_global.dac_analog_gain.cfg, -- 1 Bytes
        aec_global.aec_mode.cfg,        -- 1 Bytes
        aec_global.ul_eq_en.cfg,        -- 1 Bytes
    -- AGC 
        aec_global.fade_gain.ndt_fade_in.cfg,        -- 4 Bytes
        aec_global.fade_gain.ndt_fade_out.cfg,        -- 4 Bytes
        aec_global.fade_gain.dt_fade_in.cfg,        -- 4 Bytes
        aec_global.fade_gain.dt_fade_out.cfg,        -- 4 Bytes
        aec_global.ndt_max_gain.cfg,     -- 4 Bytes
        aec_global.ndt_min_gain.cfg,     -- 4 Bytes
        aec_global.ndt_speech_thr.cfg,   -- 4 Bytes
        aec_global.dt_max_gain.cfg,      -- 4 Bytes
        aec_global.dt_min_gain.cfg,      -- 4 Bytes
        aec_global.dt_speech_thr.cfg,      -- 4 Bytes
        aec_global.echo_present_thr.cfg,   -- 4 Bytes
    -- AEC
        aec_global.aec_dt_aggress.cfg,   -- 4 Bytes
        aec_global.aec_refengthr.cfg,    -- 4 Bytes
    -- ES
        aec_global.es_aggress_factor.cfg,  -- 4 Bytes
        aec_global.es_min_suppress.cfg,    -- 4 Bytes
    -- ANS
        aec_global.ans_aggress.cfg,    -- 4 Bytes
        aec_global.ans_suppress.cfg,   -- 4 Bytes
};

aec_global.aec_output_type_group_view = {};

aec_global.aec_output_type_group_view.agc_type_group_view = cfg:stGroup("AGC",
    cfg:vaBox {
        aec_global.fade_gain.ndt_fade_in.hbox_view,
        aec_global.fade_gain.ndt_fade_out.hbox_view,
        aec_global.fade_gain.dt_fade_in.hbox_view,
        aec_global.fade_gain.dt_fade_out.hbox_view,
        aec_global.ndt_max_gain.hbox_view,
        aec_global.ndt_min_gain.hbox_view,
        aec_global.ndt_speech_thr.hbox_view,
        aec_global.dt_max_gain.hbox_view,
        aec_global.dt_min_gain.hbox_view,
        aec_global.dt_speech_thr.hbox_view,
        aec_global.echo_present_thr.hbox_view,
    }
);

aec_global.aec_output_type_group_view.aec_type_group_view = cfg:stGroup("AEC",
    cfg:vaBox {
        aec_global.aec_dt_aggress.hbox_view,
        aec_global.aec_refengthr.hbox_view,
    }
);

aec_global.aec_output_type_group_view.es_type_group_view = cfg:stGroup("ES",
    cfg:vaBox {
        aec_global.es_aggress_factor.hbox_view,
        aec_global.es_min_suppress.hbox_view,
    }
);

aec_global.aec_output_type_group_view.ans_type_group_view = cfg:stGroup("ANS",
    cfg:vaBox {
        aec_global.ans_aggress.hbox_view,
        aec_global.ans_suppress.hbox_view,
    }
);

aec_global.aec_output_view_table = {
    aec_global.reference_book_view,
    -- aec cfg
    aec_global.mic_analog_gain.hbox_view,
    aec_global.dac_analog_gain.hbox_view,
    aec_global.aec_mode.hbox_view,
    aec_global.ul_eq_en.hbox_view,
    -- AGC 
    aec_global.aec_output_type_group_view.agc_type_group_view,
    -- AEC
    aec_global. aec_output_type_group_view.aec_type_group_view,
    -- ES
    aec_global.aec_output_type_group_view.es_type_group_view,
    -- ANS
    aec_global.aec_output_type_group_view.ans_type_group_view,
};

-- A. 输出htext
aec_global.aec_output_htext_table = {
    aec_global.aec_start_comment_str,
-- aec cfg
    aec_global.mic_analog_gain.htext,
    aec_global.dac_analog_gain.htext,
    aec_global.aec_mode.htext,
    aec_global.ul_eq_en.htext,
    -- AGC 
    aec_global.fade_gain.ndt_fade_in.htext,
    aec_global.fade_gain.ndt_fade_out.htext,
    aec_global.fade_gain.dt_fade_in.htext,
    aec_global.fade_gain.dt_fade_out.htext,
    aec_global.ndt_max_gain.htext,
    aec_global.ndt_min_gain.htext,
    aec_global.ndt_speech_thr.htext,
    aec_global.dt_max_gain.htext,
    aec_global.dt_min_gain.htext,
    aec_global.dt_speech_thr.htext,
    aec_global.echo_present_thr.htext,
    -- AEC
    aec_global.aec_dt_aggress.htext,
    aec_global.aec_refengthr.htext,
    -- ES
    aec_global.es_aggress_factor.htext,
    aec_global.es_min_suppress.htext,
    -- ANS
    aec_global.ans_aggress.htext,
    aec_global.ans_suppress.htext,
};


aec_global.aec_output_view_table_group_view = cfg:vBox(aec_global.aec_output_view_table);

-- B. 输出ctext：无

aec_global.layoutHook = function ()
    if (aec_cfg_type.cfg.val == aec_cfg_type.TY.aec) then
        return aec_global.aec_item_table;
    else
        return {};
    end
end;

aec_global.parserHook = function (item)
    return aec_global.aec_item_table;
end;


-- C. 输出bin：无
local aec_output_bin = cfg:dynGroup("AEC_OUTPUT_BIN",
	BIN_ONLY_CFG["BT_CFG"].aec.id,
    1,
    aec_global.layoutHook,
    aec_global.parserHook,
    nil
);



--[[===================================================================================
================================= 配置子项6-2: DMS配置 ================================
====================================================================================--]]
aec_global.dms_reference_book_view = cfg:stButton("JL DMS通话调试手册.pdf",
    function()
        local ret = cfg:utilsShellOpenFile(dms_reference_book_path);
        if (ret == false) then
            if cfg.lang == "zh" then
		        cfg:msgBox("info", dms_reference_book_path .. "文件不存在");
            else
		        cfg:msgBox("info", dms_reference_book_path .. " file not exist.");
            end
        end
    end);

local dms_global = {}

-- 1) dms_mic_again
dms_global.dms_mic_analog_gain = {};
dms_global.dms_mic_analog_gain.cfg = cfg:i32("MIC_AGAIN:  ", 8);
depend_item_en_show(bluetooth_en, dms_global.dms_mic_analog_gain.cfg);
dms_global.dms_mic_analog_gain.cfg:setOSize(1);
dms_global.dms_mic_analog_gain.hbox_view = cfg:hBox {
    cfg:stLabel(dms_global.dms_mic_analog_gain.cfg.name .. TAB_TABLE[1]),
    cfg:ispinView(dms_global.dms_mic_analog_gain.cfg, 0, 19, 1),
    cfg:stLabel("(MIC增益，0(-8dB)) ~ 19(30dB),步进：2dB 默认值：8)"),
    cfg:stSpacer(),
};

-- 2) dms_dac_again
dms_global.dms_dac_analog_gain = {};
dms_global.dms_dac_analog_gain.cfg = cfg:i32("DAC_AGAIN:  ", 8);
depend_item_en_show(bluetooth_en, dms_global.dms_dac_analog_gain.cfg);
dms_global.dms_dac_analog_gain.cfg:setOSize(1);
dms_global.dms_dac_analog_gain.hbox_view = cfg:hBox {
    cfg:stLabel(dms_global.dms_dac_analog_gain.cfg.name .. TAB_TABLE[1]),
    cfg:ispinView(dms_global.dms_dac_analog_gain.cfg, 0, 15, 1),
    cfg:stLabel("(DAC增益，设置范围: 0 ~ 15，步进：2dB 默认值：8)"),
    cfg:stSpacer(),
};


-- 3) aec_mode
dms_global.dms_aec_mode = {};
dms_global.dms_aec_mode.aec_en = cfg:i32("AEC_EN:", 0);
depend_item_en_show(bluetooth_en, dms_global.dms_aec_mode.aec_en);

dms_global.dms_aec_mode.nlp_en = cfg:i32("NLP_EN:", 1);
depend_item_en_show(bluetooth_en, dms_global.dms_aec_mode.nlp_en);

dms_global.dms_aec_mode.ans_en = cfg:i32("ANS_EN:", 1);
depend_item_en_show(bluetooth_en, dms_global.dms_aec_mode.ans_en);

dms_global.dms_aec_mode.enc_en = cfg:i32("ENC_EN:", 1);
depend_item_en_show(bluetooth_en, dms_global.dms_aec_mode.enc_en);

dms_global.dms_aec_mode.agc_en = cfg:i32("AGC_EN:", 1);
depend_item_en_show(bluetooth_en, dms_global.dms_aec_mode.agc_en);

dms_global.dms_aec_mode.cfg = cfg:i32("ModuleEnable:", 0x1E);
depend_item_en_show(bluetooth_en, dms_global.dms_aec_mode.cfg);
dms_global.dms_aec_mode.cfg:setOSize(1);

dms_global.dms_aec_mode.cfg:addDeps{dms_global.dms_aec_mode.aec_en, dms_global.dms_aec_mode.nlp_en, dms_global.dms_aec_mode.ans_en, dms_global.dms_aec_mode.enc_en, dms_global.dms_aec_mode.agc_en};

dms_global.dms_aec_mode.cfg:setEval(function () return ((dms_global.dms_aec_mode.aec_en.val << 0) | 
                                             (dms_global.dms_aec_mode.nlp_en.val << 1) | 
                                             (dms_global.dms_aec_mode.ans_en.val << 2) | 
                                             (dms_global.dms_aec_mode.enc_en.val << 3) |
                                             (dms_global.dms_aec_mode.agc_en.val << 4)
                                             ); end);

-- item_view
dms_global.dms_aec_mode.hbox_view = cfg:hBox {
    cfg:stLabel(dms_global.dms_aec_mode.cfg.name),
    --cfg:labelView(dms_global.dms_aec_mode.cfg),

    --cfg:stLabel(TAB_TABLE[1]),
    cfg:stLabel("  "),

    cfg:stLabel(dms_global.dms_aec_mode.aec_en.name),
	cfg:checkBoxView(dms_global.dms_aec_mode.aec_en),

    --cfg:stLabel(TAB_TABLE[1]),
    cfg:stLabel(" "),

    cfg:stLabel(dms_global.dms_aec_mode.nlp_en.name),
	cfg:checkBoxView(dms_global.dms_aec_mode.nlp_en),
    cfg:stLabel(" "),

    --cfg:stLabel(TAB_TABLE[1]),

    cfg:stLabel(dms_global.dms_aec_mode.ans_en.name),
	cfg:checkBoxView(dms_global.dms_aec_mode.ans_en),
    cfg:stLabel(" "),

    --cfg:stLabel(TAB_TABLE[1]),

    cfg:stLabel(dms_global.dms_aec_mode.enc_en.name),
	cfg:checkBoxView(dms_global.dms_aec_mode.enc_en),
    cfg:stLabel(" "),

    cfg:stLabel(dms_global.dms_aec_mode.agc_en.name),
	cfg:checkBoxView(dms_global.dms_aec_mode.agc_en),
    --cfg:stLabel(TAB_TABLE[1]),
    cfg:stLabel(" "),

    cfg:stLabel("(模块使能)"),

    cfg:stSpacer(),
};

dms_global.dms_aec_mode_bit_en_table = {
    dms_global.dms_aec_mode.aec_en,
    dms_global.dms_aec_mode.nlp_en,
    dms_global.dms_aec_mode.ans_en,
    dms_global.dms_aec_mode.enc_en,
    dms_global.dms_aec_mode.agc_en,
};

-- 4) dms_ul_eq_en
dms_global.dms_ul_eq_en = {};

dms_global.dms_ul_eq_en.ul_eq_en_sel_table = {
        [0] = " disable",
        [1] = " enable",
};
dms_global.dms_ul_eq_en.UL_EQ_EN_SEL_TABLE = cfg:enumMap("dms_ul_eq_en_sel_table", dms_global.dms_ul_eq_en.ul_eq_en_sel_table);

dms_global.dms_ul_eq_en.cfg = cfg:enum("UL_EQ_EN:  ", dms_global.dms_ul_eq_en.UL_EQ_EN_SEL_TABLE, 1);
depend_item_en_show(bluetooth_en, dms_global.dms_ul_eq_en.cfg);
dms_global.dms_ul_eq_en.cfg:setOSize(1);
dms_global.dms_ul_eq_en.hbox_view = cfg:hBox {
    cfg:stLabel(dms_global.dms_ul_eq_en.cfg.name .. TAB_TABLE[1]),
    cfg:enumView(dms_global.dms_ul_eq_en.cfg),
    cfg:stLabel("(上行 EQ 使能，默认值：enable)"),
    cfg:stSpacer(),
};

-- 5) dms_agc
dms_global.dms_agc = {
    dms_ndt_fade_in = {},
    dms_ndt_fade_out = {},
    dms_dt_fade_in = {},
    dms_dt_fade_out = {},
    dms_ndt_max_gain = {},
    dms_ndt_min_gain = {},
    dms_ndt_speech_thr = {},
    dms_dt_max_gain = {},
    dms_dt_min_gain = {},
    dms_dt_speech_thr = {},
    dms_echo_present_thr = {},
};

-- 5-1) dms_ndt_fade_in
dms_global.dms_agc.dms_ndt_fade_in.cfg = cfg:dbf("NDT_FADE_IN:  ", 1.3);
depend_item_en_show(bluetooth_en, dms_global.dms_agc.dms_ndt_fade_in.cfg);
dms_global.dms_agc.dms_ndt_fade_in.cfg:setOSize(4);
dms_global.dms_agc.dms_ndt_fade_in.hbox_view = cfg:hBox {
    cfg:stLabel(dms_global.dms_agc.dms_ndt_fade_in.cfg.name .. TAB_TABLE[1]),
    cfg:dspinView(dms_global.dms_agc.dms_ndt_fade_in.cfg, 0.1, 5.0, 0.1, 1),
    cfg:stLabel("dB (单端讲话淡入步进，设置范围: 0.1 ~ 5 dB，默认值：1.3 dB)"),
    cfg:stSpacer(),
};

-- 5-2) dms_ndt_fade_out
dms_global.dms_agc.dms_ndt_fade_out.cfg = cfg:dbf("NDT_FADE_OUT:  ", 0.7);
depend_item_en_show(bluetooth_en, dms_global.dms_agc.dms_ndt_fade_out.cfg);
dms_global.dms_agc.dms_ndt_fade_out.cfg:setOSize(4);
dms_global.dms_agc.dms_ndt_fade_out.hbox_view = cfg:hBox {
    cfg:stLabel(dms_global.dms_agc.dms_ndt_fade_out.cfg.name .. TAB_TABLE[1]),
    cfg:dspinView(dms_global.dms_agc.dms_ndt_fade_out.cfg, 0.1, 5.0, 0.1, 1),
    cfg:stLabel("dB (单端讲话淡出步进，设置范围: 0.1 ~ 5 dB，默认值：0.7 dB)"),
    cfg:stSpacer(),
};


-- 5-3) dms_dt_fade_in
dms_global.dms_agc.dms_dt_fade_in.cfg = cfg:dbf("DT_FADE_IN:  ", 1.3);
depend_item_en_show(bluetooth_en, dms_global.dms_agc.dms_dt_fade_in.cfg);
dms_global.dms_agc.dms_dt_fade_in.cfg:setOSize(4);
dms_global.dms_agc.dms_dt_fade_in.hbox_view = cfg:hBox {
    cfg:stLabel(dms_global.dms_agc.dms_dt_fade_in.cfg.name .. TAB_TABLE[1]),
    cfg:dspinView(dms_global.dms_agc.dms_dt_fade_in.cfg, 0.1, 5.0, 0.1, 1),
    cfg:stLabel("dB (双端讲话淡入步进，设置范围: 0.1 ~ 5 dB，默认值：1.3 dB)"),
    cfg:stSpacer(),
};

-- 5-4) dms_dt_fade_out
dms_global.dms_agc.dms_dt_fade_out.cfg = cfg:dbf("DT_FADE_OUT:  ", 0.7);
depend_item_en_show(bluetooth_en, dms_global.dms_agc.dms_dt_fade_out.cfg);
dms_global.dms_agc.dms_dt_fade_out.cfg:setOSize(4);
dms_global.dms_agc.dms_dt_fade_out.hbox_view = cfg:hBox {
    cfg:stLabel(dms_global.dms_agc.dms_dt_fade_out.cfg.name .. TAB_TABLE[1]),
    cfg:dspinView(dms_global.dms_agc.dms_dt_fade_out.cfg, 0.1, 5.0, 0.1, 1),
    cfg:stLabel("dB (双端讲话淡出步进，设置范围: 0.1 ~ 5 dB，默认值：0.7 dB)"),
    cfg:stSpacer(),
};

-- 5-5) dms_ndt_max_gain
dms_global.dms_agc.dms_ndt_max_gain.cfg = cfg:dbf("NDT_MAX_GAIN:  ", 8.0);
depend_item_en_show(bluetooth_en, dms_global.dms_agc.dms_ndt_max_gain.cfg);
dms_global.dms_agc.dms_ndt_max_gain.cfg:setOSize(4);
dms_global.dms_agc.dms_ndt_max_gain.hbox_view = cfg:hBox {
    cfg:stLabel(dms_global.dms_agc.dms_ndt_max_gain.cfg.name .. TAB_TABLE[1]),
    cfg:dspinView(dms_global.dms_agc.dms_ndt_max_gain.cfg, -90.0, 40.0, 0.1, 1),
    cfg:stLabel("(单端讲话放大上限， 设置范围: -90.0 ~ 40.0 dB，默认值：8.0 dB)"),
    cfg:stSpacer(),
};

-- 5-6) dms_ndt_min_gain
dms_global.dms_agc.dms_ndt_min_gain.cfg = cfg:dbf("NDT_MIN_GAIN:  ", 4.0);
depend_item_en_show(bluetooth_en, dms_global.dms_agc.dms_ndt_min_gain.cfg);
dms_global.dms_agc.dms_ndt_min_gain.cfg:setOSize(4);
dms_global.dms_agc.dms_ndt_min_gain.hbox_view = cfg:hBox {
    cfg:stLabel(dms_global.dms_agc.dms_ndt_min_gain.cfg.name .. TAB_TABLE[1]),
    cfg:dspinView(dms_global.dms_agc.dms_ndt_min_gain.cfg, -90.0, 40.0, 0.1, 1),
    cfg:stLabel("(单端讲话放大下限， 设置范围: -90.0 ~ 40.0 dB，默认值：4.0 dB)"),
    cfg:stSpacer(),
};


-- 5-7) dms_ndt_speech_thr
dms_global.dms_agc.dms_ndt_speech_thr.cfg = cfg:dbf("NDT_SPEECH_THR:  ", -50.0);
depend_item_en_show(bluetooth_en, dms_global.dms_agc.dms_ndt_speech_thr.cfg);
dms_global.dms_agc.dms_ndt_speech_thr.cfg:setOSize(4);
dms_global.dms_agc.dms_ndt_speech_thr.hbox_view = cfg:hBox {
    cfg:stLabel(dms_global.dms_agc.dms_ndt_speech_thr.cfg.name),
    cfg:dspinView(dms_global.dms_agc.dms_ndt_speech_thr.cfg, -70.0, -40.0, 0.1, 1),
    cfg:stLabel("(单端讲话放大阈值， 设置范围: -70.0 ~ -40.0 dB，默认值：-50.0 dB)"),
    cfg:stSpacer(),
};

-- 5-8) dms_dt_max_gain
dms_global.dms_agc.dms_dt_max_gain.cfg = cfg:dbf("DT_MAX_GAIN:  ", 12.0);
depend_item_en_show(bluetooth_en, dms_global.dms_agc.dms_dt_max_gain.cfg);
dms_global.dms_agc.dms_dt_max_gain.cfg:setOSize(4);
dms_global.dms_agc.dms_dt_max_gain.hbox_view = cfg:hBox {
    cfg:stLabel(dms_global.dms_agc.dms_dt_max_gain.cfg.name .. TAB_TABLE[1]),
    cfg:dspinView(dms_global.dms_agc.dms_dt_max_gain.cfg, -90.0, 40.0, 0.1, 1),
    cfg:stLabel("(双端讲话放大上限， 设置范围: -90.0 ~ 40.0 dB，默认值：12.0 dB)"),
    cfg:stSpacer(),
};

-- 5-9) dms_dt_min_gain
dms_global.dms_agc.dms_dt_min_gain.cfg = cfg:dbf("DT_MIN_GAIN:  ", 0.0);
depend_item_en_show(bluetooth_en, dms_global.dms_agc.dms_dt_min_gain.cfg);
dms_global.dms_agc.dms_dt_min_gain.cfg:setOSize(4);
dms_global.dms_agc.dms_dt_min_gain.hbox_view = cfg:hBox {
    cfg:stLabel(dms_global.dms_agc.dms_dt_min_gain.cfg.name .. TAB_TABLE[1]),
    cfg:dspinView(dms_global.dms_agc.dms_dt_min_gain.cfg, -90.0, 40.0, 0.1, 1),
    cfg:stLabel("(双端讲话放大下限， 设置范围: -90.0 ~ 40.0 dB，默认值：0 dB)"),
    cfg:stSpacer(),
};

-- 5-10) dms_dt_speech_thr
dms_global.dms_agc.dms_dt_speech_thr.cfg = cfg:dbf("DT_SPEECH_THR:  ", -40.0);
depend_item_en_show(bluetooth_en, dms_global.dms_agc.dms_dt_speech_thr.cfg);
dms_global.dms_agc.dms_dt_speech_thr.cfg:setOSize(4);
dms_global.dms_agc.dms_dt_speech_thr.hbox_view = cfg:hBox {
    cfg:stLabel(dms_global.dms_agc.dms_dt_speech_thr.cfg.name),
    cfg:dspinView(dms_global.dms_agc.dms_dt_speech_thr.cfg, -70.0, -40.0, 0.1, 1),
    cfg:stLabel("(双端讲话放大阈值， 设置范围: -70.0 ~ -40.0 dB，默认值：-40.0 dB)"),
    cfg:stSpacer(),
};


-- 5-11) dms_echo_present_thr
dms_global.dms_agc.dms_echo_present_thr.cfg = cfg:dbf("ECHO_PRESENT_THR:  ", -70.0);
depend_item_en_show(bluetooth_en, dms_global.dms_agc.dms_echo_present_thr.cfg);
dms_global.dms_agc.dms_echo_present_thr.cfg:setOSize(4);
dms_global.dms_agc.dms_echo_present_thr.hbox_view = cfg:hBox {
    cfg:stLabel(dms_global.dms_agc.dms_echo_present_thr.cfg.name),
    cfg:dspinView(dms_global.dms_agc.dms_echo_present_thr.cfg, -70.0, -40.0, 0.1, 1),
    cfg:stLabel("(单端双端讲话阈值， 设置范围: -70.0 ~ -40.0 dB，默认值：-70.0 dB)"),
    cfg:stSpacer(),
};

-- 6) dms_aec
dms_global.dms_aec = {
    dms_aec_process_maxfrequency = {},
    dms_aec_process_minfrequency = {},
    dms_af_length = {},
};

-- 6-1) dms_aec_process_maxfrequency
dms_global.dms_aec.dms_aec_process_maxfrequency.cfg= cfg:i32("AEC_Process_MaxFrequency:  ", 8000);
depend_item_en_show(bluetooth_en, dms_global.dms_aec.dms_aec_process_maxfrequency.cfg);
dms_global.dms_aec.dms_aec_process_maxfrequency.cfg:setOSize(4);
dms_global.dms_aec.dms_aec_process_maxfrequency.hbox_view = cfg:hBox {
    cfg:stLabel(dms_global.dms_aec.dms_aec_process_maxfrequency.cfg.name),
    cfg:ispinView(dms_global.dms_aec.dms_aec_process_maxfrequency.cfg, 3000, 8000, 1),
    cfg:stLabel("(设置范围: 3000 ~ 8000，默认值：8000)"),
    cfg:stSpacer(),
};

-- 6-2) dms_aec_process_minfrequency
dms_global.dms_aec.dms_aec_process_minfrequency.cfg= cfg:i32("AEC_Process_MinFrequency:  ", 0);
depend_item_en_show(bluetooth_en, dms_global.dms_aec.dms_aec_process_minfrequency.cfg);
dms_global.dms_aec.dms_aec_process_minfrequency.cfg:setOSize(4);
dms_global.dms_aec.dms_aec_process_minfrequency.hbox_view = cfg:hBox {
    cfg:stLabel(dms_global.dms_aec.dms_aec_process_minfrequency.cfg.name),
    cfg:ispinView(dms_global.dms_aec.dms_aec_process_minfrequency.cfg, 0, 1000, 1),
    cfg:stLabel("(设置范围: 0 ~ 1000，默认值：0)"),
    cfg:stSpacer(),
};


-- 6-3) dms_af_length
dms_global.dms_aec.dms_af_length.cfg= cfg:i32("AF_Length:  ", 128);
depend_item_en_show(bluetooth_en, dms_global.dms_aec.dms_af_length.cfg);
dms_global.dms_aec.dms_af_length.cfg:setOSize(4);
dms_global.dms_aec.dms_af_length.hbox_view = cfg:hBox {
    cfg:stLabel(dms_global.dms_aec.dms_af_length.cfg.name),
    cfg:ispinView(dms_global.dms_aec.dms_af_length.cfg, 128, 256, 1),
    cfg:stLabel("(设置范围: 128 ~ 256，默认值：128)"),
    cfg:stSpacer(),
};


-- 7) dms_nlp
dms_global.dms_nlp = {
    dms_nlp_process_maxfrequency = {},
    dms_nlp_process_minfrequency = {},
    dms_overdrive = {},
};

-- 7-1) dms_nlp_process_maxfrequency
dms_global.dms_nlp.dms_nlp_process_maxfrequency.cfg= cfg:i32("NLP_Process_MaxFrequency:  ", 8000);
depend_item_en_show(bluetooth_en, dms_global.dms_nlp.dms_nlp_process_maxfrequency.cfg);
dms_global.dms_nlp.dms_nlp_process_maxfrequency.cfg:setOSize(4);
dms_global.dms_nlp.dms_nlp_process_maxfrequency.hbox_view = cfg:hBox {
    cfg:stLabel(dms_global.dms_nlp.dms_nlp_process_maxfrequency.cfg.name),
    cfg:ispinView(dms_global.dms_nlp.dms_nlp_process_maxfrequency.cfg, 3000, 8000, 1),
    cfg:stLabel("(设置范围: 3000 ~ 8000，默认值：8000)"),
    cfg:stSpacer(),
};

-- 7-2) dms_nlp_process_minfrequency
dms_global.dms_nlp.dms_nlp_process_minfrequency.cfg= cfg:i32("NLP_Process_MinFrequency:  ", 0);
depend_item_en_show(bluetooth_en, dms_global.dms_nlp.dms_nlp_process_minfrequency.cfg);
dms_global.dms_nlp.dms_nlp_process_minfrequency.cfg:setOSize(4);
dms_global.dms_nlp.dms_nlp_process_minfrequency.hbox_view = cfg:hBox {
    cfg:stLabel(dms_global.dms_nlp.dms_nlp_process_minfrequency.cfg.name),
    cfg:ispinView(dms_global.dms_nlp.dms_nlp_process_minfrequency.cfg, 0, 1000, 1),
    cfg:stLabel("(设置范围: 0 ~ 1000，默认值：0)"),
    cfg:stSpacer(),
};

-- 7-3) dms_overdrive
dms_global.dms_nlp.dms_overdrive.cfg= cfg:dbf("OverDrive:  ", 1);
depend_item_en_show(bluetooth_en, dms_global.dms_nlp.dms_overdrive.cfg);
dms_global.dms_nlp.dms_overdrive.cfg:setOSize(4);
dms_global.dms_nlp.dms_overdrive.hbox_view = cfg:hBox {
    cfg:stLabel(dms_global.dms_nlp.dms_overdrive.cfg.name),
    cfg:dspinView(dms_global.dms_nlp.dms_overdrive.cfg, 0.0, 30.0, 0.1, 1),
    cfg:stLabel("(设置范围: 0 ~ 30，默认值：1)"),
    cfg:stSpacer(),
};


-- 8) dms_ans
dms_global.dms_ans = {
    dms_aggressfactor = {},
    dms_minsuppress = {},
    dms_init_noise_lvl = {},
};

-- 8-1) dms_aggressfactor
dms_global.dms_ans.dms_aggressfactor.cfg= cfg:dbf("AggressFactor:  ", 1.25);
depend_item_en_show(bluetooth_en, dms_global.dms_ans.dms_aggressfactor.cfg);
dms_global.dms_ans.dms_aggressfactor.cfg:setOSize(4);
dms_global.dms_ans.dms_aggressfactor.hbox_view = cfg:hBox {
    cfg:stLabel(dms_global.dms_ans.dms_aggressfactor.cfg.name),
    cfg:dspinView(dms_global.dms_ans.dms_aggressfactor.cfg, 0.0, 2.0, 0.01, 2),
    cfg:stLabel("(设置范围: 0 ~ 2，默认值：1.25)"),
    cfg:stSpacer(),
};


-- 8-2) dms_minsuppress
dms_global.dms_ans.dms_minsuppress.cfg= cfg:dbf("minSuppress:  ", 0.04);
depend_item_en_show(bluetooth_en, dms_global.dms_ans.dms_minsuppress.cfg);
dms_global.dms_ans.dms_minsuppress.cfg:setOSize(4);
dms_global.dms_ans.dms_minsuppress.hbox_view = cfg:hBox {
    cfg:stLabel(dms_global.dms_ans.dms_minsuppress.cfg.name),
    cfg:dspinView(dms_global.dms_ans.dms_minsuppress.cfg, 0.01, 0.1, 0.01, 2),
    cfg:stLabel("(设置范围: 0.01 ~ 0.1，默认值：0.04)"),
    cfg:stSpacer(),
};

-- 8-3) dms_init_noise_lvl
dms_global.dms_ans.dms_init_noise_lvl.cfg= cfg:dbf("MicNoiseLevel:  ", -75);
depend_item_en_show(bluetooth_en, dms_global.dms_ans.dms_init_noise_lvl.cfg);
dms_global.dms_ans.dms_init_noise_lvl.cfg:setOSize(4);
dms_global.dms_ans.dms_init_noise_lvl.hbox_view = cfg:hBox {
    cfg:stLabel(dms_global.dms_ans.dms_init_noise_lvl.cfg.name),
    cfg:dspinView(dms_global.dms_ans.dms_init_noise_lvl.cfg, -100, -30, 0.1, 1),
    cfg:stLabel("(设置范围: -100 ~ -30，默认值：-75 dB)"),
    cfg:stSpacer(),
};

-- 9) dms_enc
dms_global.dms_enc = {
    dms_enc_process_maxfreq = {},
    dms_enc_process_minfreq = {},
    dms_sir_maxfreq = {},
    dms_mic_distance = {},
    dms_target_singal_degradation = {},
	dms_mic_rms_diff = {},
    dms_enc_aggressfactor = {},
    dms_enc_minsuppress = {},
};

-- 9-1) dms_enc_process_maxfreq
dms_global.dms_enc.dms_enc_process_maxfreq.cfg= cfg:i32("ENC_Process_MaxFreq:  ", 8000);
depend_item_en_show(bluetooth_en, dms_global.dms_enc.dms_enc_process_maxfreq.cfg);
dms_global.dms_enc.dms_enc_process_maxfreq.cfg:setOSize(4);
dms_global.dms_enc.dms_enc_process_maxfreq.hbox_view = cfg:hBox {
    cfg:stLabel(dms_global.dms_enc.dms_enc_process_maxfreq.cfg.name),
    cfg:ispinView(dms_global.dms_enc.dms_enc_process_maxfreq.cfg, 3000, 8000, 1),
    cfg:stLabel("(设置范围: 3000 ~ 8000，默认值：8000)"),
    cfg:stSpacer(),
};

-- 9-2) dms_enc_process_minfreq
dms_global.dms_enc.dms_enc_process_minfreq.cfg= cfg:i32("ENC_Process_MinFreq:  ", 0);
depend_item_en_show(bluetooth_en, dms_global.dms_enc.dms_enc_process_minfreq.cfg);
dms_global.dms_enc.dms_enc_process_minfreq.cfg:setOSize(4);
dms_global.dms_enc.dms_enc_process_minfreq.hbox_view = cfg:hBox {
    cfg:stLabel(dms_global.dms_enc.dms_enc_process_minfreq.cfg.name),
    cfg:ispinView(dms_global.dms_enc.dms_enc_process_minfreq.cfg, 0, 1000, 1),
    cfg:stLabel("(设置范围: 0 ~ 1000，默认值：0)"),
    cfg:stSpacer(),
};

-- 9-3) dms_sir_maxfreq
dms_global.dms_enc.dms_sir_maxfreq.cfg= cfg:i32("SIR_MaxFreq:  ", 3000);
depend_item_en_show(bluetooth_en, dms_global.dms_enc.dms_sir_maxfreq.cfg);
dms_global.dms_enc.dms_sir_maxfreq.cfg:setOSize(4);
dms_global.dms_enc.dms_sir_maxfreq.hbox_view = cfg:hBox {
    cfg:stLabel(dms_global.dms_enc.dms_sir_maxfreq.cfg.name),
    cfg:ispinView(dms_global.dms_enc.dms_sir_maxfreq.cfg, 1000, 8000, 1),
    cfg:stLabel("(设置范围: 1000 ~ 8000，默认值：3000)"),
    cfg:stSpacer(),
};

-- 9-4) dms_mic_distance
dms_global.dms_enc.dms_mic_distance.cfg= cfg:dbf("Mic_Distance:  ", 0.015);
depend_item_en_show(bluetooth_en, dms_global.dms_enc.dms_mic_distance.cfg);
dms_global.dms_enc.dms_mic_distance.cfg:setOSize(4);
dms_global.dms_enc.dms_mic_distance.int_cfg = cfg:i32("Mic_Distance:i32:", 15);
dms_global.dms_enc.dms_mic_distance.hbox_view = cfg:hBox {
    cfg:stLabel("Mic_Distance:  "),
    cfg:ispinView(dms_global.dms_enc.dms_mic_distance.int_cfg, 15, 35, 1),
    cfg:stLabel("(设置范围: 15 ~ 35，默认值：15，单位:mm)"),
    cfg:stSpacer(),
};
dms_global.dms_enc.dms_mic_distance.cfg:addDeps({dms_global.dms_enc.dms_mic_distance.int_cfg});
dms_global.dms_enc.dms_mic_distance.cfg:setEval(function ()
    local v = dms_global.dms_enc.dms_mic_distance.int_cfg.val / 1000.0;
    return v;
end);

--    ) dms_mic_rms_diff
dms_global.dms_enc.dms_mic_rms_diff.cfg = cfg:dbf("Mic RMS diff:  ", 1);
depend_item_en_show(bluetooth_en, dms_global.dms_enc.dms_mic_rms_diff.cfg);

dms_global.dms_enc.dms_mic_rms_diff.hbox_view = cfg:hBox {
    cfg:stLabel(dms_global.dms_enc.dms_mic_rms_diff.cfg.name),
    cfg:dspinView(dms_global.dms_enc.dms_mic_rms_diff.cfg, -6, 0, 0.01, 2);
    cfg:stLabel("（设置范围：-6 ~ 0，默认值：0）"),
	cfg:stSpacer(),
};

-- 9-5) dms_target_singal_degradation
dms_global.dms_enc.dms_target_singal_degradation.cfg= cfg:dbf("Target_Signal_Degradation:  ", 1);
depend_item_en_show(bluetooth_en, dms_global.dms_enc.dms_target_singal_degradation.cfg);
dms_global.dms_enc.dms_target_singal_degradation.cfg:setOSize(4);
dms_global.dms_enc.dms_target_singal_degradation.hbox_view = cfg:hBox {
    cfg:stLabel(dms_global.dms_enc.dms_target_singal_degradation.cfg.name),
    cfg:labelView(dms_global.dms_enc.dms_target_singal_degradation.cfg),
    cfg:stLabel("(设置范围: 0 ~ 1，默认值：1)"),
    cfg:stSpacer(),
};

dms_global.dms_enc.dms_target_singal_degradation.cfg:addDeps({dms_global.dms_enc.dms_mic_rms_diff.cfg});
dms_global.dms_enc.dms_target_singal_degradation.cfg:setEval(function () return 10.0 ^ (dms_global.dms_enc.dms_mic_rms_diff.cfg.val / 20.0); end);

-- 9-6) dms_enc_aggressfactor
dms_global.dms_enc.dms_enc_aggressfactor.cfg= cfg:dbf("ENC_Aggressfactor:  ", 4);
depend_item_en_show(bluetooth_en, dms_global.dms_enc.dms_enc_aggressfactor.cfg);
dms_global.dms_enc.dms_enc_aggressfactor.cfg:setOSize(4);
dms_global.dms_enc.dms_enc_aggressfactor.hbox_view = cfg:hBox {
    cfg:stLabel(dms_global.dms_enc.dms_enc_aggressfactor.cfg.name),
    cfg:dspinView(dms_global.dms_enc.dms_enc_aggressfactor.cfg, 0, 4, 0.0001, 4),
    cfg:stLabel("(设置范围: 0 ~ 4，默认值：4.0)"),
    cfg:stSpacer(),
};

-- 9-7) dms_enc_minsuppress
dms_global.dms_enc.dms_enc_minsuppress.cfg= cfg:dbf("ENC_Minsuppress:  ", 0.09);
depend_item_en_show(bluetooth_en, dms_global.dms_enc.dms_enc_minsuppress.cfg);
dms_global.dms_enc.dms_enc_minsuppress.cfg:setOSize(4);
dms_global.dms_enc.dms_enc_minsuppress.hbox_view = cfg:hBox {
    cfg:stLabel(dms_global.dms_enc.dms_enc_minsuppress.cfg.name),
    cfg:dspinView(dms_global.dms_enc.dms_enc_minsuppress.cfg, 0, 0.1, 0.0001, 4),
    cfg:stLabel("(设置范围: 0 ~ 0.1，默认值：0.09)"),
    cfg:stSpacer(),
};

-- 10) dms_common 
dms_global.dms_common = {
    global_minsuppress = {},
};
dms_global.dms_common.global_minsuppress.cfg= cfg:dbf("global_minsuppress:  ", 0.04);
depend_item_en_show(bluetooth_en, dms_global.dms_common.global_minsuppress.cfg);
dms_global.dms_common.global_minsuppress.cfg:setOSize(4);
dms_global.dms_common.global_minsuppress.hbox_view = cfg:hBox {
    cfg:stLabel(dms_global.dms_common.global_minsuppress.cfg.name),
    cfg:dspinView(dms_global.dms_common.global_minsuppress.cfg, 0.0, 0.09, 0.01, 2),
    cfg:stLabel("(设置范围: 0.0 ~ 0.09，默认值：0.04)"),
    cfg:stSpacer(),
};

--========================= DMS 输出汇总  ============================
dms_global.dms_item_table = {
    -- aec cfg
        dms_global.dms_mic_analog_gain.cfg, -- 1 Bytes
        dms_global.dms_dac_analog_gain.cfg, -- 1 Bytes
        dms_global.dms_aec_mode.cfg,        -- 1 Bytes
        dms_global.dms_ul_eq_en.cfg,        -- 1 Bytes

        dms_global.dms_agc.dms_ndt_fade_in.cfg,  -- 4 Bytes
        dms_global.dms_agc.dms_ndt_fade_out.cfg,  -- 4 Bytes
        dms_global.dms_agc.dms_dt_fade_in.cfg,  -- 4 Bytes
        dms_global.dms_agc.dms_dt_fade_out.cfg,  -- 4 Bytes
        dms_global.dms_agc.dms_ndt_max_gain.cfg,  -- 4 Bytes
        dms_global.dms_agc.dms_ndt_min_gain.cfg,  -- 4 Bytes
        dms_global.dms_agc.dms_ndt_speech_thr.cfg,  -- 4 Bytes
        dms_global.dms_agc.dms_dt_max_gain.cfg,  -- 4 Bytes
        dms_global.dms_agc.dms_dt_min_gain.cfg,  -- 4 Bytes
        dms_global.dms_agc.dms_dt_speech_thr.cfg,  -- 4 Bytes
        dms_global.dms_agc.dms_echo_present_thr.cfg,  -- 4 Bytes

        dms_global.dms_aec.dms_aec_process_maxfrequency.cfg,  -- 4 Bytes
        dms_global.dms_aec.dms_aec_process_minfrequency.cfg,  -- 4 Bytes
        dms_global.dms_aec.dms_af_length.cfg,  -- 4 Bytes

        dms_global.dms_nlp.dms_nlp_process_maxfrequency.cfg,  -- 4 Bytes
        dms_global.dms_nlp.dms_nlp_process_minfrequency.cfg,  -- 4 Bytes
        dms_global.dms_nlp.dms_overdrive.cfg,  -- 4 Bytes

        dms_global.dms_ans.dms_aggressfactor.cfg,  -- 4 Bytes
        dms_global.dms_ans.dms_minsuppress.cfg,  -- 4 Bytes
        dms_global.dms_ans.dms_init_noise_lvl.cfg,  -- 4 Bytes

        dms_global.dms_enc.dms_enc_process_maxfreq.cfg,  -- 4 Bytes
        dms_global.dms_enc.dms_enc_process_minfreq.cfg,  -- 4 Bytes
        dms_global.dms_enc.dms_sir_maxfreq.cfg,  -- 4 Bytes
        dms_global.dms_enc.dms_mic_distance.cfg,  -- 4 Bytes
        dms_global.dms_enc.dms_target_singal_degradation.cfg,  -- 4 Bytes
        dms_global.dms_enc.dms_enc_aggressfactor.cfg,  -- 4 Bytes
        dms_global.dms_enc.dms_enc_minsuppress.cfg,  -- 4 Bytes

        dms_global.dms_common.global_minsuppress.cfg, -- 4 Bytes
};


dms_global.dms_output_type_group_view = {};
dms_global.dms_output_type_group_view.agc_type_group_view = cfg:stGroup(" AGC ",
    cfg:vaBox {
        dms_global.dms_agc.dms_ndt_fade_in.hbox_view,
        dms_global.dms_agc.dms_ndt_fade_out.hbox_view,
        dms_global.dms_agc.dms_dt_fade_in.hbox_view,
        dms_global.dms_agc.dms_dt_fade_out.hbox_view,
        dms_global.dms_agc.dms_ndt_max_gain.hbox_view,
        dms_global.dms_agc.dms_ndt_min_gain.hbox_view,
        dms_global.dms_agc.dms_ndt_speech_thr.hbox_view,
        dms_global.dms_agc.dms_dt_max_gain.hbox_view,
        dms_global.dms_agc.dms_dt_min_gain.hbox_view,
        dms_global.dms_agc.dms_dt_speech_thr.hbox_view,
        dms_global.dms_agc.dms_echo_present_thr.hbox_view,
    }
);

dms_global.dms_output_type_group_view.aec_type_group_view = cfg:stGroup(" AEC ",
    cfg:vaBox {
        dms_global.dms_aec.dms_aec_process_maxfrequency.hbox_view,
        dms_global.dms_aec.dms_aec_process_minfrequency.hbox_view,
        dms_global.dms_aec.dms_af_length.hbox_view,
    }
);

dms_global.dms_output_type_group_view.nlp_type_group_view = cfg:stGroup(" NLP ",
    cfg:vaBox {
        dms_global.dms_nlp.dms_nlp_process_maxfrequency.hbox_view,
        dms_global.dms_nlp.dms_nlp_process_minfrequency.hbox_view,
        dms_global.dms_nlp.dms_overdrive.hbox_view,
    }
);

dms_global.dms_output_type_group_view.ans_type_group_view = cfg:stGroup(" ANS ",
    cfg:vaBox {
        dms_global.dms_ans.dms_aggressfactor.hbox_view,
        dms_global.dms_ans.dms_minsuppress.hbox_view,
        dms_global.dms_ans.dms_init_noise_lvl.hbox_view,
    }
);

dms_global.dms_output_type_group_view.enc_type_group_view = cfg:stGroup(" ENC ",
    cfg:vaBox {
        dms_global.dms_enc.dms_enc_process_maxfreq.hbox_view,
        dms_global.dms_enc.dms_enc_process_minfreq.hbox_view,
        dms_global.dms_enc.dms_sir_maxfreq.hbox_view,
        dms_global.dms_enc.dms_mic_distance.hbox_view,
        dms_global.dms_enc.dms_mic_rms_diff.hbox_view,
        dms_global.dms_enc.dms_target_singal_degradation.hbox_view,
        dms_global.dms_enc.dms_enc_aggressfactor.hbox_view,
        dms_global.dms_enc.dms_enc_minsuppress.hbox_view,
    }
);

dms_global.dms_output_view_table = {
        aec_global.dms_reference_book_view,
    -- aec cfg
        dms_global.dms_mic_analog_gain.hbox_view,
        dms_global.dms_dac_analog_gain.hbox_view,
        dms_global.dms_aec_mode.hbox_view,
        dms_global.dms_ul_eq_en.hbox_view, 

        dms_global.dms_common.global_minsuppress.hbox_view,

        dms_global.dms_output_type_group_view.agc_type_group_view,
        dms_global.dms_output_type_group_view.aec_type_group_view,
        dms_global.dms_output_type_group_view.nlp_type_group_view,
        dms_global.dms_output_type_group_view.ans_type_group_view,
        dms_global.dms_output_type_group_view.enc_type_group_view,
};

aec_global.dms_output_view_table_group_view = cfg:vBox(dms_global.dms_output_view_table);
--aec_cfg_depend_show(dms_mic_analog_gain.cfg, aec_cfg_type.cfg, 1, aec_global.dms_output_view_table_group_view);


-- A. 输出htext
dms_global.dms_output_htext_table = {
};

-- B. 输出ctext：无


dms_global.layoutHook = function ()
    if (aec_cfg_type.cfg.val == aec_cfg_type.TY.dms) then
        return dms_global.dms_item_table;
    else
        return {};
    end
end;

dms_global.parserHook = function (item)
    return dms_global.dms_item_table;
end;

-- C. 输出bin：无
local dms_output_bin = cfg:dynGroup("DMS_OUTPUT_BIN",
	BIN_ONLY_CFG["BT_CFG"].dms.id,
    1,
    dms_global.layoutHook,
    dms_global.parserHook,
    nil
);

dms_output_bin:setRecoverHook(function ()
    local ov = dms_global.dms_aec_mode.cfg.val;
    cfg:set(dms_global.dms_aec_mode.aec_en, ov & 0x1);
    cfg:set(dms_global.dms_aec_mode.nlp_en, (ov >> 1) & 0x1);
    cfg:set(dms_global.dms_aec_mode.ans_en, (ov >> 2) & 0x1);
    cfg:set(dms_global.dms_aec_mode.enc_en, (ov >> 3) & 0x1);
    cfg:set(dms_global.dms_aec_mode.agc_en, (ov >> 4) & 0x1);
    ov = dms_global.dms_enc.dms_target_singal_degradation.cfg.val;
    cfg:set(dms_global.dms_enc.dms_mic_rms_diff.cfg, 20.0 * math.log(ov, 10.0));
    cfg:set(dms_global.dms_enc.dms_mic_distance.int_cfg,
        math.floor(1000.0 * (dms_global.dms_enc.dms_mic_distance.cfg.val + 0.0005)));
end);

-- E. 默认值, 见汇总

--cfg:bindStGroup(aec_group_view, aec_output_bin);
--]]


--[[====================================================================================
================================ AEC(DNS) ==============================================
====================================================================================--]]
local aec_dns_global = {};

-- item_htext
aec_dns_global.aec_start_comment_str = module_comment_context("AEC(DNS)参数");

aec_dns_global.reference_book_view = cfg:stButton("JL通话调试手册.pdf",
    function()
        local ret = cfg:utilsShellOpenFile(aec_reference_book_path);
        if (ret == false) then
            if cfg.lang == "zh" then
                cfg:msgBox("info", aec_reference_book_path .. "文件不存在");
            else
                cfg:msgBox("info", aec_reference_book_path .. " file not exist.");
            end
        end
    end);

-- 1) mic_again
aec_dns_global.mic_analog_gain = {};
aec_dns_global.mic_analog_gain.cfg = cfg:i32("AEC_DNS_MIC_AGAIN：", 8);
depend_item_en_show(bluetooth_en, aec_dns_global.mic_analog_gain.cfg);
aec_dns_global.mic_analog_gain.cfg:setOSize(1);
-- item_htext
aec_dns_global.mic_analog_gain.comment_str = "(MIC增益，0(-8dB)) ~ 19(30dB),步进：2dB 默认值：8)";
aec_dns_global.mic_analog_gain.htext = item_output_htext(aec_dns_global.mic_analog_gain.cfg,
    "TCFG_AEC_MIC_ANALOG_GAIN", 3, nil, aec_dns_global.mic_analog_gain.comment_str, 1);
-- item_view
aec_dns_global.mic_analog_gain.hbox_view = cfg:hBox {
    cfg:stLabel("MIC_AGAIN：" .. TAB_TABLE[1]),
    cfg:ispinView(aec_dns_global.mic_analog_gain.cfg, 0, 19, 1),
    cfg:stLabel(aec_dns_global.mic_analog_gain.comment_str),
    cfg:stSpacer(),
};


-- 2) dac_again
aec_dns_global.dac_analog_gain = {};
aec_dns_global.dac_analog_gain.cfg = cfg:i32("AEC_DNS_DAC_AGAIN：", 8);
depend_item_en_show(bluetooth_en, aec_dns_global.dac_analog_gain.cfg);
aec_dns_global.dac_analog_gain.cfg:setOSize(1);
-- item_htext
aec_dns_global.dac_analog_gain.comment_str = "(DAC 增益，设置范围: 0 ~ 15，步进：2dB 默认值：8)";
aec_dns_global.dac_analog_gain.htext = item_output_htext(aec_dns_global.dac_analog_gain.cfg,
    "TCFG_AEC_DAC_ANALOG_GAIN", 3, nil, aec_dns_global.dac_analog_gain.comment_str, 1);
-- item_view
aec_dns_global.dac_analog_gain.hbox_view = cfg:hBox {
    cfg:stLabel("DAC_AGAIN：" .. TAB_TABLE[1]),
    cfg:ispinView(aec_dns_global.dac_analog_gain.cfg, 0, 15, 1),
    cfg:stLabel(aec_dns_global.dac_analog_gain.comment_str);
    cfg:stSpacer(),
};

-- 3) aec_mode
aec_dns_global.aec_mode = {};
aec_dns_global.aec_mode.cfg = cfg:enum("AEC_DNS_AEC_MODE: ", AEC_MODE_SEL_TABLE, 1);
depend_item_en_show(bluetooth_en, aec_dns_global.aec_mode.cfg);
aec_dns_global.aec_mode.cfg:setOSize(1);
-- item_htext
aec_dns_global.aec_mode.comment_str = item_comment_context("AEC_MODE", aec_mode_sel_table) .. "默认值：2";
aec_dns_global.aec_mode.htext = item_output_htext(aec_dns_global.aec_mode.cfg,
    "TCFG_AEC_MODE", 6, nil, aec_dns_global.aec_mode.comment_str, 1);
-- item_view
aec_dns_global.aec_mode.hbox_view = cfg:hBox {
    cfg:stLabel("AEC_MODE: " .. TAB_TABLE[1]),
    cfg:enumView(aec_dns_global.aec_mode.cfg),
    cfg:stLabel("(AEC 模式，默认值：reduce)"),
    cfg:stSpacer(),
};

aec_dns_global.ul_eq_en = {};
aec_dns_global.ul_eq_en.cfg = cfg:enum("AEC_DNS_UL_EQ_EN: ", UL_EQ_EN_SEL_TABLE, 1);
depend_item_en_show(bluetooth_en, aec_dns_global.ul_eq_en.cfg);
aec_dns_global.ul_eq_en.cfg:setOSize(1);
-- item_htext
aec_dns_global.ul_eq_en.comment_str = item_comment_context("UL_EQ_EN", ul_eq_en_sel_table) .. "默认值：1";
aec_dns_global.ul_eq_en.htext = item_output_htext(aec_dns_global.ul_eq_en.cfg,
    "TCFG_AEC_UL_EQ_EN", 5, nil, aec_dns_global.ul_eq_en.comment_str, 2);
-- item_view
aec_dns_global.ul_eq_en.hbox_view = cfg:hBox {
    cfg:stLabel("UL_EQ_EN: " .. TAB_TABLE[1]),
    cfg:enumView(aec_dns_global.ul_eq_en.cfg),
    cfg:stLabel("(上行 EQ 使能，默认值：enable)"),
    cfg:stSpacer(),
};


-- 5) fade_gain
aec_dns_global.fade_gain = {ndt_fade_in = {}, ndt_fade_out = {}, dt_fade_in = {}, dt_fade_out = {}};
-- 5-1) ndt_fade_in
aec_dns_global.fade_gain.ndt_fade_in.cfg = cfg:dbf("AEC_DNS_NDT_FADE_IN：", 1.3);
depend_item_en_show(bluetooth_en, aec_dns_global.fade_gain.ndt_fade_in.cfg);
aec_dns_global.fade_gain.ndt_fade_in.cfg:setOSize(4);
-- item_htext
aec_dns_global.fade_gain.ndt_fade_in.comment_str = "设置范围：0.1 ~ 5 dB, float类型(4Bytes), 默认值：1.3";
aec_dns_global.fade_gain.ndt_fade_in.htext = item_output_htext(aec_dns_global.fade_gain.ndt_fade_in.cfg,
    "TCFG_NDT_FADE_IN", 4, nil, aec_dns_global.fade_gain.ndt_fade_in.comment_str, 1);
-- item_view
aec_dns_global.fade_gain.ndt_fade_in.hbox_view = cfg:hBox {
    cfg:stLabel("NDT_FADE_IN：" .. TAB_TABLE[1]),
    cfg:dspinView(aec_dns_global.fade_gain.ndt_fade_in.cfg, 0.1, 5.0, 0.1, 1),
    cfg:stLabel("dB (单端讲话淡入步进，设置范围: 0.1 ~ 5 dB，默认值：1.3 dB)"),
    cfg:stSpacer(),
};

-- 5-2) ndt_fade_out
aec_dns_global.fade_gain.ndt_fade_out.cfg = cfg:dbf("AEC_DNS_NDT_FADE_OUT：", 0.7);
depend_item_en_show(bluetooth_en, aec_dns_global.fade_gain.ndt_fade_out.cfg);
aec_dns_global.fade_gain.ndt_fade_out.cfg:setOSize(4);
-- item_htext
aec_dns_global.fade_gain.ndt_fade_out.comment_str = "设置范围：0.1 ~ 5 dB, float类型(4Bytes), 默认值：0.7";
aec_dns_global.fade_gain.ndt_fade_out.htext = item_output_htext(aec_dns_global.fade_gain.ndt_fade_out.cfg,
    "TCFG_NDT_FADE_OUT", 4, nil, aec_dns_global.fade_gain.ndt_fade_out.comment_str, 1);
-- item_view
aec_dns_global.fade_gain.ndt_fade_out.hbox_view = cfg:hBox {
    cfg:stLabel("NDT_FADE_OUT：" .. TAB_TABLE[1]),
    cfg:dspinView(aec_dns_global.fade_gain.ndt_fade_out.cfg, 0.1, 5.0, 0.1, 1),
    cfg:stLabel("dB (单端讲话淡出步进，设置范围: 0.1 ~ 5 dB，默认值：0.7 dB)"),
    cfg:stSpacer(),
};

-- 5-3) dt_fade_in
aec_dns_global.fade_gain.dt_fade_in.cfg = cfg:dbf("AEC_DNS_DT_FADE_IN：", 1.3);
depend_item_en_show(bluetooth_en, aec_dns_global.fade_gain.dt_fade_in.cfg);
aec_dns_global.fade_gain.dt_fade_in.cfg:setOSize(4);
-- item_htext
aec_dns_global.fade_gain.dt_fade_in.comment_str = "设置范围：0.1 ~ 5 dB, float类型(4Bytes), 默认值：1.3";
aec_dns_global.fade_gain.dt_fade_in.htext = item_output_htext(aec_dns_global.fade_gain.dt_fade_in.cfg,
    "TCFG_DT_FADE_IN", 4, nil, aec_dns_global.fade_gain.dt_fade_in.comment_str, 1);
-- item_view
aec_dns_global.fade_gain.dt_fade_in.hbox_view = cfg:hBox {
    cfg:stLabel("DT_FADE_IN：" .. TAB_TABLE[1]),
    cfg:dspinView(aec_dns_global.fade_gain.dt_fade_in.cfg, 0.1, 5.0, 0.1, 1),
    cfg:stLabel("dB (双端讲话淡入步进，设置范围: 0.1 ~ 5 dB，默认值：1.3 dB)"),
    cfg:stSpacer(),
};

-- 5-4) dt_fade_out
aec_dns_global.fade_gain.dt_fade_out.cfg = cfg:dbf("AEC_DNS_DT_FADE_OUT：", 0.7);
depend_item_en_show(bluetooth_en, aec_dns_global.fade_gain.dt_fade_out.cfg);
aec_dns_global.fade_gain.dt_fade_out.cfg:setOSize(4);
-- item_htext
aec_dns_global.fade_gain.dt_fade_out.comment_str = "设置范围：0.1 ~ 5 dB, float类型(4Bytes), 默认值：0.7";
aec_dns_global.fade_gain.dt_fade_out.htext = item_output_htext(aec_dns_global.fade_gain.dt_fade_out.cfg,
    "TCFG_DT_FADE_OUT", 4, nil, aec_dns_global.fade_gain.dt_fade_out.comment_str, 1);
-- item_view
aec_dns_global.fade_gain.dt_fade_out.hbox_view = cfg:hBox {
    cfg:stLabel("DT_FADE_OUT：" .. TAB_TABLE[1]),
    cfg:dspinView(aec_dns_global.fade_gain.dt_fade_out.cfg, 0.1, 5.0, 0.1, 1),
    cfg:stLabel("dB (双端讲话淡出步进，设置范围: 0.1 ~ 5 dB，默认值：0.7 dB)"),
    cfg:stSpacer(),
};

-- 6) ndt_max_gain
aec_dns_global.ndt_max_gain = {};
aec_dns_global.ndt_max_gain.cfg = cfg:dbf("AEC_DNS_NDT_MAX_GAIN：", 8.0);
depend_item_en_show(bluetooth_en, aec_dns_global.ndt_max_gain.cfg);
aec_dns_global.ndt_max_gain.cfg:setOSize(4);
-- item_htext
aec_dns_global.ndt_max_gain.comment_str = "设置范围：-90.0 ~ 40.0 dB, 默认值：8.0 dB";
aec_dns_global.ndt_max_gain.htext = item_output_htext(aec_dns_global.ndt_max_gain.cfg,
    "TCFG_AEC_NDT_MAX_GAIN", 4, nil, aec_dns_global.ndt_max_gain.comment_str, 1);
-- item_view
aec_dns_global.ndt_max_gain.hbox_view = cfg:hBox {
    cfg:stLabel("NDT_MAX_GAIN：" .. TAB_TABLE[1]),
    cfg:dspinView(aec_dns_global.ndt_max_gain.cfg, -90.0, 40.0, 0.1, 1),
    cfg:stLabel("(单端讲话放大上限， 设置范围: -90.0 ~ 40.0 dB，默认值：8.0 dB)"),
    cfg:stSpacer(),
};

-- 7) ndt_min_gain
aec_dns_global.ndt_min_gain = {};
aec_dns_global.ndt_min_gain.cfg = cfg:dbf("ANS_DNS_NDT_MIN_GAIN：", 4.0);
depend_item_en_show(bluetooth_en, aec_dns_global.ndt_min_gain.cfg);
aec_dns_global.ndt_min_gain.cfg:setOSize(4);
-- item_htext
aec_dns_global.ndt_min_gain.comment_str = "设置范围：-90.0 ~ 40.0 dB, 默认值：4.0 dB";
aec_dns_global.ndt_min_gain.htext = item_output_htext(aec_dns_global.ndt_min_gain.cfg,
    "TCFG_AEC_NDT_MIN_GAIN", 4, nil, aec_dns_global.ndt_min_gain.comment_str, 1);
-- item_view
aec_dns_global.ndt_min_gain.hbox_view = cfg:hBox {
    cfg:stLabel("NDT_MIN_GAIN：" .. TAB_TABLE[1]),
    cfg:dspinView(aec_dns_global.ndt_min_gain.cfg, -90.0, 40.0, 0.1, 1),
    cfg:stLabel("(单端讲话放大下限， 设置范围: -90.0 ~ 40.0 dB，默认值：4.0 dB)"),
    cfg:stSpacer(),
};

-- 8) ndt_speech_thr
aec_dns_global.ndt_speech_thr = {};
aec_dns_global.ndt_speech_thr.cfg = cfg:dbf("AEC_DNS_NDT_SPEECH_THR：", -50.0);
depend_item_en_show(bluetooth_en, aec_dns_global.ndt_speech_thr.cfg);
aec_dns_global.ndt_speech_thr.cfg:setOSize(4);
-- item_htext
aec_dns_global.ndt_speech_thr.comment_str = "设置范围：-70.0 ~ -40.0 dB, 默认值：-50.0 dB";
aec_dns_global.ndt_speech_thr.htext = item_output_htext(aec_dns_global.ndt_speech_thr.cfg,
    "TCFG_AEC_NDT_SPEECH_THR", 4, nil, aec_dns_global.ndt_speech_thr.comment_str, 1);
-- item_view
aec_dns_global.ndt_speech_thr.hbox_view = cfg:hBox {
    cfg:stLabel("NDT_SPEECH_THR："),
    cfg:dspinView(aec_dns_global.ndt_speech_thr.cfg, -70.0, -40.0, 0.1, 1),
    cfg:stLabel("(单端讲话放大阈值， 设置范围: -70.0 ~ -40.0 dB，默认值：-50.0 dB)"),
    cfg:stSpacer(),
};


-- 9) dt_max_gain 
aec_dns_global.dt_max_gain = {};
aec_dns_global.dt_max_gain.cfg = cfg:dbf("AEC_DNS_DT_MAX_GAIN：", 12.0);
depend_item_en_show(bluetooth_en, aec_dns_global.dt_max_gain.cfg);
aec_dns_global.dt_max_gain.cfg:setOSize(4);
-- item_htext
aec_dns_global.dt_max_gain.comment_str = "设置范围：-90.0 ~ 40.0 dB, 默认值：12.0 dB";
aec_dns_global.dt_max_gain.htext = item_output_htext(aec_dns_global.dt_max_gain.cfg,
    "TCFG_AEC_DT_MAX_GAIN", 4, nil, aec_dns_global.dt_max_gain.comment_str, 1);
-- item_view
aec_dns_global.dt_max_gain.hbox_view = cfg:hBox {
    cfg:stLabel("DT_MAX_GAIN：" .. TAB_TABLE[1]),
    cfg:dspinView(aec_dns_global.dt_max_gain.cfg, -90.0, 40.0, 0.1, 1),
    cfg:stLabel("(双端讲话放大上限， 设置范围: -90.0 ~ 40.0 dB，默认值：12.0 dB)"),
    cfg:stSpacer(),
};

-- 10) dt_min_gain
aec_dns_global.dt_min_gain = {};
aec_dns_global.dt_min_gain.cfg = cfg:dbf("AEC_DNS_DT_MIN_GAIN：", 0.0);
depend_item_en_show(bluetooth_en, aec_dns_global.dt_min_gain.cfg);
aec_dns_global.dt_min_gain.cfg:setOSize(4);
-- item_htext
aec_dns_global.dt_min_gain.comment_str = "设置范围：-90.0 ~ 40.0 dB, 默认值：0 dB";
aec_dns_global.dt_min_gain.htext = item_output_htext(aec_dns_global.dt_min_gain.cfg,
    "TCFG_AEC_DT_MIN_GAIN", 4, nil, aec_dns_global.dt_min_gain.comment_str, 1);
-- item_view
aec_dns_global.dt_min_gain.hbox_view = cfg:hBox {
    cfg:stLabel("DT_MIN_GAIN：" .. TAB_TABLE[1]),
    cfg:dspinView(aec_dns_global.dt_min_gain.cfg, -90.0, 40.0, 0.1, 1),
    cfg:stLabel("(双端讲话放大下限， 设置范围: -90.0 ~ 40.0 dB，默认值：0 dB)"),
    cfg:stSpacer(),
};


-- 11) dt_speech_thr
aec_dns_global.dt_speech_thr = {};
aec_dns_global.dt_speech_thr.cfg = cfg:dbf("AEC_DNS_DT_SPEECH_THR：", -40.0);
depend_item_en_show(bluetooth_en, aec_dns_global.dt_speech_thr.cfg);
aec_dns_global.dt_speech_thr.cfg:setOSize(4);
-- item_htext
aec_dns_global.dt_speech_thr.comment_str = "设置范围：-70.0 ~ -40.0 dB, 默认值：-40.0 dB";
aec_dns_global.dt_speech_thr.htext = item_output_htext(aec_dns_global.dt_speech_thr.cfg,
    "TCFG_AEC_DT_SPEECH_THR", 4, nil, aec_dns_global.dt_speech_thr.comment_str, 1);
-- item_view
aec_dns_global.dt_speech_thr.hbox_view = cfg:hBox {
    cfg:stLabel("DT_SPEECH_THR：" .. TAB_TABLE[1]),
    cfg:dspinView(aec_dns_global.dt_speech_thr.cfg, -70.0, -40.0, 0.1, 1),
    cfg:stLabel("(双端讲话放大阈值， 设置范围: -70.0 ~ -40.0 dB，默认值：-40.0 dB)"),
    cfg:stSpacer(),
};

-- 12) echo_present_thr
aec_dns_global.echo_present_thr = {};
aec_dns_global.echo_present_thr.cfg = cfg:dbf("AEC_DNS_ECHO_PRESENT_THR：", -70.0);
depend_item_en_show(bluetooth_en, aec_dns_global.echo_present_thr.cfg);
aec_dns_global.echo_present_thr.cfg:setOSize(4);
-- item_htext
aec_dns_global.echo_present_thr.comment_str = "设置范围：-70.0 ~ -40.0 dB, 默认值：-70.0 dB";
aec_dns_global.echo_present_thr.htext = item_output_htext(aec_dns_global.echo_present_thr.cfg,
    "TCFG_AEC_ECHO_PRESENT_THR", 4, nil, aec_dns_global.echo_present_thr.comment_str, 1);
-- item_view
aec_dns_global.echo_present_thr.hbox_view = cfg:hBox {
    cfg:stLabel("ECHO_PRESENT_THR："),
    cfg:dspinView(aec_dns_global.echo_present_thr.cfg, -70.0, -40.0, 0.1, 1),
    cfg:stLabel("(单端双端讲话阈值， 设置范围: -70.0 ~ -40.0 dB，默认值：-70.0 dB)"),
    cfg:stSpacer(),
};

-- 13) aec_dt_aggress
aec_dns_global.aec_dt_aggress = {};
aec_dns_global.aec_dt_aggress.cfg = cfg:dbf("AEC_DNS_AEC_DT_AGGRES：", 1.0);
depend_item_en_show(bluetooth_en, aec_dns_global.aec_dt_aggress.cfg);
aec_dns_global.aec_dt_aggress.cfg:setOSize(4);
-- item_htext
aec_dns_global.aec_dt_aggress.comment_str = "设置范围：1.0 ~ 5.0, float类型(4Bytes), 默认值：1.0";
aec_dns_global.aec_dt_aggress.htext = item_output_htext(aec_dns_global.aec_dt_aggress.cfg,
    "TCFG_AEC_AEC_DT_AGGRESS", 4, nil, aec_dns_global.aec_dt_aggress.comment_str, 1);
-- item_view
aec_dns_global.aec_dt_aggress.hbox_view = cfg:hBox {
    cfg:stLabel("AEC_DT_AGGRES："),
    cfg:dspinView(aec_dns_global.aec_dt_aggress.cfg, 1.0, 5.0, 0.1, 1),
    cfg:stLabel("(原音回音追踪等级， 设置范围: 1.0 ~ 5.0，默认值：1.0)"),
    cfg:stSpacer(),
};

-- 14) aec_refengthr
aec_dns_global.aec_refengthr = {};
aec_dns_global.aec_refengthr.cfg = cfg:dbf("AEC_DNS_AEC_REFENGTHR：", -70.0);
depend_item_en_show(bluetooth_en, aec_dns_global.aec_refengthr.cfg);
aec_dns_global.aec_refengthr.cfg:setOSize(4);
-- item_htext
aec_dns_global.aec_refengthr.comment_str = "设置范围：-90.0 ~ -60.0 dB, float类型(4Bytes), 默认值：-70.0";
aec_dns_global.aec_refengthr.htext = item_output_htext(aec_dns_global.aec_refengthr.cfg,
    "TCFG_AEC_REFENGTHR", 4, nil, aec_dns_global.aec_refengthr.comment_str, 1);
-- item_view
aec_dns_global.aec_refengthr.hbox_view = cfg:hBox {
    cfg:stLabel("AEC_REFENGTHR：" .. TAB_TABLE[1]),
    cfg:dspinView(aec_dns_global.aec_refengthr.cfg, -90.0, -60.0, 0.1, 1),
    cfg:stLabel("(进入回音消除参考值， 设置范围: -90.0 ~ -60.0 dB，默认值：-70.0 dB)"),
    cfg:stSpacer(),
};

-- 15) es_aggress_factor
aec_dns_global.es_aggress_factor = {};
aec_dns_global.es_aggress_factor.cfg = cfg:dbf("AEC_DNS_ES_AGGRESS_FACTOR：", -3.0);
depend_item_en_show(bluetooth_en, aec_dns_global.es_aggress_factor.cfg);
aec_dns_global.es_aggress_factor.cfg:setOSize(4);
-- item_htext
aec_dns_global.es_aggress_factor.comment_str = "设置范围：-1.0 ~ -5.0, float类型(4Bytes), 默认值：-3.0";
aec_dns_global.es_aggress_factor.htext = item_output_htext(aec_dns_global.es_aggress_factor.cfg,
    "TCFG_ES_AGGRESS_FACTOR", 4, nil, aec_dns_global.es_aggress_factor.comment_str, 1);
-- item_view
aec_dns_global.es_aggress_factor.hbox_view = cfg:hBox {
    cfg:stLabel("ES_AGGRESS_FACTOR："),
    cfg:dspinView(aec_dns_global.es_aggress_factor.cfg, -5.0, -1.0, 0.1, 1),
    cfg:stLabel("(回音前级动态压制,越小越强， 设置范围: -5.0 ~ -1.0，默认值：-3.0)"),
    cfg:stSpacer(),
};

-- 16) es_min_suppress
--      //回音后级动态压制,越大越强,default: 4.f(0 ~ 10)
aec_dns_global.es_min_suppress = {};
aec_dns_global.es_min_suppress.cfg = cfg:dbf("AEC_DNS_ES_MIN_SUPPRESS：", 4.0);
depend_item_en_show(bluetooth_en, aec_dns_global.es_min_suppress.cfg);
aec_dns_global.es_min_suppress.cfg:setOSize(4);
-- item_htext
aec_dns_global.es_min_suppress.comment_str = "设置范围：0 ~ 10.0, float类型(4Bytes), 默认值：4.0";
aec_dns_global.es_min_suppress.htext = item_output_htext(aec_dns_global.es_min_suppress.cfg,
    "TCFG_ES_MIN_SUPPRESS", 4, nil, aec_dns_global.es_min_suppress.comment_str, 1);
-- item_view
aec_dns_global.es_min_suppress.hbox_view = cfg:hBox {
    cfg:stLabel("ES_MIN_SUPPRESS："),
    cfg:dspinView(aec_dns_global.es_min_suppress.cfg, 0.0, 10.0, 0.1, 1),
    cfg:stLabel("(回音后级静态压制,越大越强， 设置范围: 0 ~ 10.0，默认值：4.0)"),
    cfg:stSpacer(),
};

-- 17) dns_aggress
--  //噪声前级动态压制,越大越强default: 1.25f(1 ~ 2)
aec_dns_global.dns_aggress = {};
aec_dns_global.dns_aggress.cfg = cfg:dbf("AEC_DNS_DNS_AGGRESS：", 1.0);
depend_item_en_show(bluetooth_en, aec_dns_global.dns_aggress.cfg);
aec_dns_global.dns_aggress.cfg:setOSize(4);
-- item_htext
aec_dns_global.dns_aggress.comment_str = "设置范围：1 ~ 2, float类型(4Bytes), 默认值：1.25";
aec_dns_global.dns_aggress.htext = item_output_htext(aec_dns_global.dns_aggress.cfg,
    "TCFG_AEC_DNS_AGGRESS：", 4, nil, aec_dns_global.dns_aggress.comment_str, 1);
-- item_view
aec_dns_global.dns_aggress.hbox_view = cfg:hBox {
    cfg:stLabel("DNS_OverDrive：" .. TAB_TABLE[1]),
    cfg:dspinView(aec_dns_global.dns_aggress.cfg, 0.0, 6.0, 0.1, 1),
    cfg:stLabel("(降噪强度，越大降噪越强，范围: 0 ~ 6.0，默认值：1.0)"),
    cfg:stSpacer(),
};

-- 18) dns_suppress
-- float dns_suppress; //噪声后级动态压制,越小越强default: 0.04f(0 ~ 1)
aec_dns_global.dns_suppress = {};
aec_dns_global.dns_suppress.cfg = cfg:dbf("AEC_DNS_DNS_SUPPRESS：", 0.1);
depend_item_en_show(bluetooth_en, aec_dns_global.dns_suppress.cfg);
aec_dns_global.dns_suppress.cfg:setOSize(4);
-- item_htext
aec_dns_global.dns_suppress.comment_str = "设置范围：0 ~ 1, float类型(4Bytes), 默认值：0.09";
aec_dns_global.dns_suppress.htext = item_output_htext(aec_dns_global.dns_suppress.cfg,
    "TCFG_AEC_DNS_SUPPRESS", 4, nil, aec_dns_global.dns_suppress.comment_str, 1);
-- item_view
aec_dns_global.dns_suppress.hbox_view = cfg:hBox {
    cfg:stLabel("DNS_GainFloor：" .. TAB_TABLE[1]),
    cfg:dspinView(aec_dns_global.dns_suppress.cfg, 0.0, 1.0, 0.01, 2),
    cfg:stLabel("(增益最小值控制,越小降噪越强，范围: 0 ~ 1.0，默认值：0.1)"),
    cfg:stSpacer(),
};

--========================= AEC输出汇总  ============================
aec_dns_global.aec_item_table = {
    -- aec cfg
    aec_dns_global.mic_analog_gain.cfg, -- 1 Bytes
    aec_dns_global.dac_analog_gain.cfg, -- 1 Bytes
    aec_dns_global.aec_mode.cfg,        -- 1 Bytes
    aec_dns_global.ul_eq_en.cfg,        -- 1 Bytes
    -- AGC 
    aec_dns_global.fade_gain.ndt_fade_in.cfg,        -- 4 Bytes
    aec_dns_global.fade_gain.ndt_fade_out.cfg,        -- 4 Bytes
    aec_dns_global.fade_gain.dt_fade_in.cfg,        -- 4 Bytes
    aec_dns_global.fade_gain.dt_fade_out.cfg,        -- 4 Bytes
    aec_dns_global.ndt_max_gain.cfg,     -- 4 Bytes
    aec_dns_global.ndt_min_gain.cfg,     -- 4 Bytes
    aec_dns_global.ndt_speech_thr.cfg,   -- 4 Bytes
    aec_dns_global.dt_max_gain.cfg,      -- 4 Bytes
    aec_dns_global.dt_min_gain.cfg,      -- 4 Bytes
    aec_dns_global.dt_speech_thr.cfg,      -- 4 Bytes
    aec_dns_global.echo_present_thr.cfg,   -- 4 Bytes
    -- AEC
    aec_dns_global.aec_dt_aggress.cfg,   -- 4 Bytes
    aec_dns_global.aec_refengthr.cfg,    -- 4 Bytes
    -- ES
    aec_dns_global.es_aggress_factor.cfg,  -- 4 Bytes
    aec_dns_global.es_min_suppress.cfg,    -- 4 Bytes
    -- DNS
    aec_dns_global.dns_aggress.cfg,    -- 4 Bytes
    aec_dns_global.dns_suppress.cfg,   -- 4 Bytes
};

aec_dns_global.aec_output_type_group_view = {};

aec_dns_global.aec_output_type_group_view.agc_type_group_view = cfg:stGroup("AGC",
    cfg:vaBox {
        aec_dns_global.fade_gain.ndt_fade_in.hbox_view,
        aec_dns_global.fade_gain.ndt_fade_out.hbox_view,
        aec_dns_global.fade_gain.dt_fade_in.hbox_view,
        aec_dns_global.fade_gain.dt_fade_out.hbox_view,
        aec_dns_global.ndt_max_gain.hbox_view,
        aec_dns_global.ndt_min_gain.hbox_view,
        aec_dns_global.ndt_speech_thr.hbox_view,
        aec_dns_global.dt_max_gain.hbox_view,
        aec_dns_global.dt_min_gain.hbox_view,
        aec_dns_global.dt_speech_thr.hbox_view,
        aec_dns_global.echo_present_thr.hbox_view,
    }
);

aec_dns_global.aec_output_type_group_view.aec_type_group_view = cfg:stGroup("AEC",
    cfg:vaBox {
        aec_dns_global.aec_dt_aggress.hbox_view,
        aec_dns_global.aec_refengthr.hbox_view,
    }
);

aec_dns_global.aec_output_type_group_view.es_type_group_view = cfg:stGroup("ES",
    cfg:vaBox {
        aec_dns_global.es_aggress_factor.hbox_view,
        aec_dns_global.es_min_suppress.hbox_view,
    }
);

aec_dns_global.aec_output_type_group_view.dns_type_group_view = cfg:stGroup("DNS",
    cfg:vaBox {
        aec_dns_global.dns_aggress.hbox_view,
        aec_dns_global.dns_suppress.hbox_view,
    }
);

aec_dns_global.aec_output_view_table = {
    aec_dns_global.reference_book_view,
    -- aec cfg
    aec_dns_global.mic_analog_gain.hbox_view,
    aec_dns_global.dac_analog_gain.hbox_view,
    aec_dns_global.aec_mode.hbox_view,
    aec_dns_global.ul_eq_en.hbox_view,
    -- AGC 
    aec_dns_global.aec_output_type_group_view.agc_type_group_view,
    -- AEC
    aec_dns_global.aec_output_type_group_view.aec_type_group_view,
    -- ES
    aec_dns_global.aec_output_type_group_view.es_type_group_view,
    -- DNS
    aec_dns_global.aec_output_type_group_view.dns_type_group_view,
};

-- A. 输出htext
aec_dns_global.aec_output_htext_table = {
    aec_global.aec_start_comment_str,
    -- aec cfg
    aec_dns_global.mic_analog_gain.htext,
    aec_dns_global.dac_analog_gain.htext,
    aec_dns_global.aec_mode.htext,
    aec_dns_global.ul_eq_en.htext,
    -- AGC 
    aec_dns_global.fade_gain.ndt_fade_in.htext,
    aec_dns_global.fade_gain.ndt_fade_out.htext,
    aec_dns_global.fade_gain.dt_fade_in.htext,
    aec_dns_global.fade_gain.dt_fade_out.htext,
    aec_dns_global.ndt_max_gain.htext,
    aec_dns_global.ndt_min_gain.htext,
    aec_dns_global.ndt_speech_thr.htext,
    aec_dns_global.dt_max_gain.htext,
    aec_dns_global.dt_min_gain.htext,
    aec_dns_global.dt_speech_thr.htext,
    aec_dns_global.echo_present_thr.htext,
    -- AEC
    aec_dns_global.aec_dt_aggress.htext,
    aec_dns_global.aec_refengthr.htext,
    -- ES
    aec_dns_global.es_aggress_factor.htext,
    aec_dns_global.es_min_suppress.htext,
    -- ANS
    aec_dns_global.dns_aggress.htext,
    aec_dns_global.dns_suppress.htext,
};


aec_global.aec_dns_output_view_table_group_view = cfg:vBox(aec_dns_global.aec_output_view_table);
-- B. 输出ctext：无

aec_dns_global.layoutHook = function ()
    if (aec_cfg_type.cfg.val == aec_cfg_type.TY.aec_dns) then
        return aec_dns_global.aec_item_table;
    else
        return {};
    end
end;

aec_dns_global.parserHook = function (item)
    return aec_dns_global.aec_item_table;
end;

-- C. 输出bin：无
aec_dns_global.aec_output_bin = cfg:dynGroup("AEC_DNS_OUTPUT_BIN",
	BIN_ONLY_CFG["BT_CFG"].aec_dns.id,
    1,
    aec_dns_global.layoutHook,
    aec_dns_global.parserHook,
    nil
);

--[[================================================================================
============================== DMS(DNS) ============================================
================================================================================--]]
local dms_dns_global = {};

dms_dns_global.dms_reference_book_view = cfg:stButton("JL DMS通话调试手册.pdf",
    function()
        local ret = cfg:utilsShellOpenFile(dms_reference_book_path);
        if (ret == false) then
            if cfg.lang == "zh" then
		        cfg:msgBox("info", dms_reference_book_path .. "文件不存在");
            else
		        cfg:msgBox("info", dms_reference_book_path .. " file not exist.");
            end
        end
    end);

-- 1) dms_mic_again
dms_dns_global.dms_mic_analog_gain = {};
dms_dns_global.dms_mic_analog_gain.cfg = cfg:i32("DMS_DNS_MIC_AGAIN:  ", 8);
depend_item_en_show(bluetooth_en, dms_dns_global.dms_mic_analog_gain.cfg);
dms_dns_global.dms_mic_analog_gain.cfg:setOSize(1);
dms_dns_global.dms_mic_analog_gain.hbox_view = cfg:hBox {
    cfg:stLabel("MIC_AGAIN:  " .. TAB_TABLE[1]),
    cfg:ispinView(dms_dns_global.dms_mic_analog_gain.cfg, 0, 19, 1),
    cfg:stLabel("(MIC增益，0(-8dB)) ~ 19(30dB),步进：2dB 默认值：8)"),
    cfg:stSpacer(),
};

-- 2) dms_dac_again
dms_dns_global.dms_dac_analog_gain = {};
dms_dns_global.dms_dac_analog_gain.cfg = cfg:i32("DMS_DNS_DAC_AGAIN:  ", 8);
depend_item_en_show(bluetooth_en, dms_dns_global.dms_dac_analog_gain.cfg);
dms_dns_global.dms_dac_analog_gain.cfg:setOSize(1);
dms_dns_global.dms_dac_analog_gain.hbox_view = cfg:hBox {
    cfg:stLabel("DAC_AGAIN:  " .. TAB_TABLE[1]),
    cfg:ispinView(dms_dns_global.dms_dac_analog_gain.cfg, 0, 15, 1),
    cfg:stLabel("(DAC增益，设置范围: 0 ~ 15，步进：2dB 默认值：8)"),
    cfg:stSpacer(),
};


-- 3) aec_mode
dms_dns_global.dms_aec_mode = {};
dms_dns_global.dms_aec_mode.aec_en = cfg:i32("DMS_DNS_AEC_EN:", 0);
depend_item_en_show(bluetooth_en, dms_dns_global.dms_aec_mode.aec_en);

dms_dns_global.dms_aec_mode.nlp_en = cfg:i32("DMS_DNS_NLP_EN:", 1);
depend_item_en_show(bluetooth_en, dms_dns_global.dms_aec_mode.nlp_en);

dms_dns_global.dms_aec_mode.ans_en = cfg:i32("DMS_DNS_ANS_EN:", 1);
depend_item_en_show(bluetooth_en, dms_dns_global.dms_aec_mode.ans_en);

dms_dns_global.dms_aec_mode.enc_en = cfg:i32("DMS_DNS_ENC_EN:", 1);
depend_item_en_show(bluetooth_en, dms_dns_global.dms_aec_mode.enc_en);

dms_dns_global.dms_aec_mode.agc_en = cfg:i32("DMS_DNS_AGC_EN:", 1);
depend_item_en_show(bluetooth_en, dms_dns_global.dms_aec_mode.agc_en);

dms_dns_global.dms_aec_mode.cfg = cfg:i32("DMS_DNS_ModuleEnable:", 0x1E);
depend_item_en_show(bluetooth_en, dms_dns_global.dms_aec_mode.cfg);
dms_dns_global.dms_aec_mode.cfg:setOSize(1);
-- item_htext
--mic_analog_gain.comment_str = "设置范围：0 ~ 19, 默认值：8";
--mic_analog_gain.htext = item_output_htext(mic_analog_gain.cfg, "TCFG_AEC_MIC_ANALOG_GAIN", 3, nil, mic_analog_gain.comment_str, 1);

dms_dns_global.dms_aec_mode.cfg:addDeps{dms_dns_global.dms_aec_mode.aec_en,
    dms_dns_global.dms_aec_mode.nlp_en, dms_dns_global.dms_aec_mode.ans_en,
    dms_dns_global.dms_aec_mode.enc_en, dms_dns_global.dms_aec_mode.agc_en};

dms_dns_global.dms_aec_mode.cfg:setEval(function () return ((dms_dns_global.dms_aec_mode.aec_en.val << 0) | 
                                             (dms_dns_global.dms_aec_mode.nlp_en.val << 1) | 
                                             (dms_dns_global.dms_aec_mode.ans_en.val << 2) | 
                                             (dms_dns_global.dms_aec_mode.enc_en.val << 3) |
                                             (dms_dns_global.dms_aec_mode.agc_en.val << 4)
                                             ); end);

-- item_view
dms_dns_global.dms_aec_mode.hbox_view = cfg:hBox {
    cfg:stLabel("ModuleEnable:"),
    --cfg:labelView(dms_dns_global.dms_aec_mode.cfg),

    cfg:stLabel("  "),

    cfg:stLabel("AEC_EN:"),
	cfg:checkBoxView(dms_dns_global.dms_aec_mode.aec_en),

    cfg:stLabel(" "),

    cfg:stLabel("NLP_EN:"),
	cfg:checkBoxView(dms_dns_global.dms_aec_mode.nlp_en),
    cfg:stLabel(" "),

    cfg:stLabel("ANS_EN:"),
	cfg:checkBoxView(dms_dns_global.dms_aec_mode.ans_en),
    cfg:stLabel(" "),

    cfg:stLabel("ENC_EN:"),
	cfg:checkBoxView(dms_dns_global.dms_aec_mode.enc_en),
    cfg:stLabel(" "),

    cfg:stLabel("AGC_EN:"),
	cfg:checkBoxView(dms_dns_global.dms_aec_mode.agc_en),
    cfg:stLabel(" "),

    cfg:stLabel("(模块使能)"),

    cfg:stSpacer(),
};

dms_dns_global.dms_aec_mode_bit_en_table = {
    dms_dns_global.dms_aec_mode.aec_en,
    dms_dns_global.dms_aec_mode.nlp_en,
    dms_dns_global.dms_aec_mode.ans_en,
    dms_dns_global.dms_aec_mode.enc_en,
    dms_dns_global.dms_aec_mode.agc_en,
};

-- 4) dms_ul_eq_en
dms_dns_global.dms_ul_eq_en = {};

dms_dns_global.dms_ul_eq_en.ul_eq_en_sel_table = {
        [0] = " disable",
        [1] = " enable",
};
dms_dns_global.dms_ul_eq_en.UL_EQ_EN_SEL_TABLE = cfg:enumMap("dms_ul_eq_en_sel_table",
    dms_dns_global.dms_ul_eq_en.ul_eq_en_sel_table);

dms_dns_global.dms_ul_eq_en.cfg = cfg:enum("DMS_DNS_UL_EQ_EN:  ", dms_dns_global.dms_ul_eq_en.UL_EQ_EN_SEL_TABLE, 1);
depend_item_en_show(bluetooth_en, dms_dns_global.dms_ul_eq_en.cfg);
dms_dns_global.dms_ul_eq_en.cfg:setOSize(1);
dms_dns_global.dms_ul_eq_en.hbox_view = cfg:hBox {
    cfg:stLabel("UL_EQ_EN:  " .. TAB_TABLE[1]),
    cfg:enumView(dms_dns_global.dms_ul_eq_en.cfg),
    cfg:stLabel("(上行 EQ 使能，默认值：enable)"),
    cfg:stSpacer(),
};

-- 5) dms_agc
dms_dns_global.dms_agc = {
    dms_ndt_fade_in = {},
    dms_ndt_fade_out = {},
    dms_dt_fade_in = {},
    dms_dt_fade_out = {},
    dms_ndt_max_gain = {},
    dms_ndt_min_gain = {},
    dms_ndt_speech_thr = {},
    dms_dt_max_gain = {},
    dms_dt_min_gain = {},
    dms_dt_speech_thr = {},
    dms_echo_present_thr = {},
};

-- 5-1) dms_ndt_fade_in
dms_dns_global.dms_agc.dms_ndt_fade_in.cfg = cfg:dbf("DMS_DNS_NDT_FADE_IN:  ", 1.3);
depend_item_en_show(bluetooth_en, dms_dns_global.dms_agc.dms_ndt_fade_in.cfg);
dms_dns_global.dms_agc.dms_ndt_fade_in.cfg:setOSize(4);
dms_dns_global.dms_agc.dms_ndt_fade_in.hbox_view = cfg:hBox {
    cfg:stLabel("NDT_FADE_IN:  " .. TAB_TABLE[1]),
    cfg:dspinView(dms_dns_global.dms_agc.dms_ndt_fade_in.cfg, 0.1, 5.0, 0.1, 1),
    cfg:stLabel("dB (单端讲话淡入步进，设置范围: 0.1 ~ 5 dB，默认值：1.3 dB)"),
    cfg:stSpacer(),
};

-- 5-2) dms_ndt_fade_out
dms_dns_global.dms_agc.dms_ndt_fade_out.cfg = cfg:dbf("DMS_DNS_NDT_FADE_OUT:  ", 0.7);
depend_item_en_show(bluetooth_en, dms_dns_global.dms_agc.dms_ndt_fade_out.cfg);
dms_dns_global.dms_agc.dms_ndt_fade_out.cfg:setOSize(4);
dms_dns_global.dms_agc.dms_ndt_fade_out.hbox_view = cfg:hBox {
    cfg:stLabel("NDT_FADE_OUT:  " .. TAB_TABLE[1]),
    cfg:dspinView(dms_dns_global.dms_agc.dms_ndt_fade_out.cfg, 0.1, 5.0, 0.1, 1),
    cfg:stLabel("dB (单端讲话淡出步进，设置范围: 0.1 ~ 5 dB，默认值：0.7 dB)"),
    cfg:stSpacer(),
};


-- 5-3) dms_dt_fade_in
dms_dns_global.dms_agc.dms_dt_fade_in.cfg = cfg:dbf("DMS_DNS_DT_FADE_IN:  ", 1.3);
depend_item_en_show(bluetooth_en, dms_dns_global.dms_agc.dms_dt_fade_in.cfg);
dms_dns_global.dms_agc.dms_dt_fade_in.cfg:setOSize(4);
dms_dns_global.dms_agc.dms_dt_fade_in.hbox_view = cfg:hBox {
    cfg:stLabel("DT_FADE_IN:  " .. TAB_TABLE[1]),
    cfg:dspinView(dms_dns_global.dms_agc.dms_dt_fade_in.cfg, 0.1, 5.0, 0.1, 1),
    cfg:stLabel("dB (双端讲话淡入步进，设置范围: 0.1 ~ 5 dB，默认值：1.3 dB)"),
    cfg:stSpacer(),
};

-- 5-4) dms_dt_fade_out
dms_dns_global.dms_agc.dms_dt_fade_out.cfg = cfg:dbf("DMS_DNS_DT_FADE_OUT:  ", 0.7);
depend_item_en_show(bluetooth_en, dms_dns_global.dms_agc.dms_dt_fade_out.cfg);
dms_dns_global.dms_agc.dms_dt_fade_out.cfg:setOSize(4);
dms_dns_global.dms_agc.dms_dt_fade_out.hbox_view = cfg:hBox {
    cfg:stLabel("DT_FADE_OUT:  " .. TAB_TABLE[1]),
    cfg:dspinView(dms_dns_global.dms_agc.dms_dt_fade_out.cfg, 0.1, 5.0, 0.1, 1),
    cfg:stLabel("dB (双端讲话淡出步进，设置范围: 0.1 ~ 5 dB，默认值：0.7 dB)"),
    cfg:stSpacer(),
};

-- 5-5) dms_ndt_max_gain
dms_dns_global.dms_agc.dms_ndt_max_gain.cfg = cfg:dbf("DMS_DNS_NDT_MAX_GAIN:  ", 8.0);
depend_item_en_show(bluetooth_en, dms_dns_global.dms_agc.dms_ndt_max_gain.cfg);
dms_dns_global.dms_agc.dms_ndt_max_gain.cfg:setOSize(4);
dms_dns_global.dms_agc.dms_ndt_max_gain.hbox_view = cfg:hBox {
    cfg:stLabel("NDT_MAX_GAIN:  " .. TAB_TABLE[1]),
    cfg:dspinView(dms_dns_global.dms_agc.dms_ndt_max_gain.cfg, -90.0, 40.0, 0.1, 1),
    cfg:stLabel("(单端讲话放大上限， 设置范围: -90.0 ~ 40.0 dB，默认值：8.0 dB)"),
    cfg:stSpacer(),
};

-- 5-6) dms_ndt_min_gain
dms_dns_global.dms_agc.dms_ndt_min_gain.cfg = cfg:dbf("DMS_DNS_NDT_MIN_GAIN:  ", 4.0);
depend_item_en_show(bluetooth_en, dms_dns_global.dms_agc.dms_ndt_min_gain.cfg);
dms_dns_global.dms_agc.dms_ndt_min_gain.cfg:setOSize(4);
dms_dns_global.dms_agc.dms_ndt_min_gain.hbox_view = cfg:hBox {
    cfg:stLabel("NDT_MIN_GAIN:  " .. TAB_TABLE[1]),
    cfg:dspinView(dms_dns_global.dms_agc.dms_ndt_min_gain.cfg, -90.0, 40.0, 0.1, 1),
    cfg:stLabel("(单端讲话放大下限， 设置范围: -90.0 ~ 40.0 dB，默认值：4.0 dB)"),
    cfg:stSpacer(),
};


-- 5-7) dms_ndt_speech_thr
dms_dns_global.dms_agc.dms_ndt_speech_thr.cfg = cfg:dbf("DMS_DNS_NDT_SPEECH_THR:  ", -50.0);
depend_item_en_show(bluetooth_en, dms_dns_global.dms_agc.dms_ndt_speech_thr.cfg);
dms_dns_global.dms_agc.dms_ndt_speech_thr.cfg:setOSize(4);
dms_dns_global.dms_agc.dms_ndt_speech_thr.hbox_view = cfg:hBox {
    cfg:stLabel("NDT_SPEECH_THR:  "),
    cfg:dspinView(dms_dns_global.dms_agc.dms_ndt_speech_thr.cfg, -70.0, -40.0, 0.1, 1),
    cfg:stLabel("(单端讲话放大阈值， 设置范围: -70.0 ~ -40.0 dB，默认值：-50.0 dB)"),
    cfg:stSpacer(),
};

-- 5-8) dms_dt_max_gain
dms_dns_global.dms_agc.dms_dt_max_gain.cfg = cfg:dbf("DMS_DNS_DT_MAX_GAIN:  ", 12.0);
depend_item_en_show(bluetooth_en, dms_dns_global.dms_agc.dms_dt_max_gain.cfg);
dms_dns_global.dms_agc.dms_dt_max_gain.cfg:setOSize(4);
dms_dns_global.dms_agc.dms_dt_max_gain.hbox_view = cfg:hBox {
    cfg:stLabel("DT_MAX_GAIN:  " .. TAB_TABLE[1]),
    cfg:dspinView(dms_dns_global.dms_agc.dms_dt_max_gain.cfg, -90.0, 40.0, 0.1, 1),
    cfg:stLabel("(双端讲话放大上限， 设置范围: -90.0 ~ 40.0 dB，默认值：12.0 dB)"),
    cfg:stSpacer(),
};

-- 5-9) dms_dt_min_gain
dms_dns_global.dms_agc.dms_dt_min_gain.cfg = cfg:dbf("DMS_DNS_DT_MIN_GAIN:  ", 0.0);
depend_item_en_show(bluetooth_en, dms_dns_global.dms_agc.dms_dt_min_gain.cfg);
dms_dns_global.dms_agc.dms_dt_min_gain.cfg:setOSize(4);
dms_dns_global.dms_agc.dms_dt_min_gain.hbox_view = cfg:hBox {
    cfg:stLabel("DT_MIN_GAIN:  " .. TAB_TABLE[1]),
    cfg:dspinView(dms_dns_global.dms_agc.dms_dt_min_gain.cfg, -90.0, 40.0, 0.1, 1),
    cfg:stLabel("(双端讲话放大下限， 设置范围: -90.0 ~ 40.0 dB，默认值：0 dB)"),
    cfg:stSpacer(),
};

-- 5-10) dms_dt_speech_thr
dms_dns_global.dms_agc.dms_dt_speech_thr.cfg = cfg:dbf("DMS_DNS_DT_SPEECH_THR:  ", -40.0);
depend_item_en_show(bluetooth_en, dms_dns_global.dms_agc.dms_dt_speech_thr.cfg);
dms_dns_global.dms_agc.dms_dt_speech_thr.cfg:setOSize(4);
dms_dns_global.dms_agc.dms_dt_speech_thr.hbox_view = cfg:hBox {
    cfg:stLabel("DT_SPEECH_THR:  "),
    cfg:dspinView(dms_dns_global.dms_agc.dms_dt_speech_thr.cfg, -70.0, -40.0, 0.1, 1),
    cfg:stLabel("(双端讲话放大阈值， 设置范围: -70.0 ~ -40.0 dB，默认值：-40.0 dB)"),
    cfg:stSpacer(),
};


-- 5-11) dms_echo_present_thr
dms_dns_global.dms_agc.dms_echo_present_thr.cfg = cfg:dbf("DMS_DNS_ECHO_PRESENT_THR:  ", -70.0);
depend_item_en_show(bluetooth_en, dms_dns_global.dms_agc.dms_echo_present_thr.cfg);
dms_dns_global.dms_agc.dms_echo_present_thr.cfg:setOSize(4);
dms_dns_global.dms_agc.dms_echo_present_thr.hbox_view = cfg:hBox {
    cfg:stLabel("ECHO_PRESENT_THR:  "),
    cfg:dspinView(dms_dns_global.dms_agc.dms_echo_present_thr.cfg, -70.0, -40.0, 0.1, 1),
    cfg:stLabel("(单端双端讲话阈值， 设置范围: -70.0 ~ -40.0 dB，默认值：-70.0 dB)"),
    cfg:stSpacer(),
};

-- 6) dms_aec
dms_dns_global.dms_aec = {
    dms_aec_process_maxfrequency = {},
    dms_aec_process_minfrequency = {},
    dms_af_length = {},
};

-- 6-1) dms_aec_process_maxfrequency
dms_dns_global.dms_aec.dms_aec_process_maxfrequency.cfg= cfg:i32("DMS_DNS_AEC_Process_MaxFrequency:  ", 8000);
depend_item_en_show(bluetooth_en, dms_dns_global.dms_aec.dms_aec_process_maxfrequency.cfg);
dms_dns_global.dms_aec.dms_aec_process_maxfrequency.cfg:setOSize(4);
dms_dns_global.dms_aec.dms_aec_process_maxfrequency.hbox_view = cfg:hBox {
    cfg:stLabel("AEC_Process_MaxFrequency:  "),
    cfg:ispinView(dms_dns_global.dms_aec.dms_aec_process_maxfrequency.cfg, 3000, 8000, 1),
    cfg:stLabel("(设置范围: 3000 ~ 8000，默认值：8000)"),
    cfg:stSpacer(),
};

-- 6-2) dms_aec_process_minfrequency
dms_dns_global.dms_aec.dms_aec_process_minfrequency.cfg= cfg:i32("DMS_DNS_AEC_Process_MinFrequency:  ", 0);
depend_item_en_show(bluetooth_en, dms_dns_global.dms_aec.dms_aec_process_minfrequency.cfg);
dms_dns_global.dms_aec.dms_aec_process_minfrequency.cfg:setOSize(4);
dms_dns_global.dms_aec.dms_aec_process_minfrequency.hbox_view = cfg:hBox {
    cfg:stLabel("AEC_Process_MinFrequency:  "),
    cfg:ispinView(dms_dns_global.dms_aec.dms_aec_process_minfrequency.cfg, 0, 1000, 1),
    cfg:stLabel("(设置范围: 0 ~ 1000，默认值：0)"),
    cfg:stSpacer(),
};


-- 6-3) dms_af_length
dms_dns_global.dms_aec.dms_af_length.cfg= cfg:i32("DMS_DNS_AF_Length:  ", 128);
depend_item_en_show(bluetooth_en, dms_dns_global.dms_aec.dms_af_length.cfg);
dms_dns_global.dms_aec.dms_af_length.cfg:setOSize(4);
dms_dns_global.dms_aec.dms_af_length.hbox_view = cfg:hBox {
    cfg:stLabel("AF_Length:  "),
    cfg:ispinView(dms_dns_global.dms_aec.dms_af_length.cfg, 128, 256, 1),
    cfg:stLabel("(设置范围: 128 ~ 256，默认值：128)"),
    cfg:stSpacer(),
};


-- 7) dms_nlp
dms_dns_global.dms_nlp = {
    dms_nlp_process_maxfrequency = {},
    dms_nlp_process_minfrequency = {},
    dms_overdrive = {},
};

-- 7-1) dms_nlp_process_maxfrequency
dms_dns_global.dms_nlp.dms_nlp_process_maxfrequency.cfg= cfg:i32("DMS_DNS_NLP_Process_MaxFrequency:  ", 8000);
depend_item_en_show(bluetooth_en, dms_dns_global.dms_nlp.dms_nlp_process_maxfrequency.cfg);
dms_dns_global.dms_nlp.dms_nlp_process_maxfrequency.cfg:setOSize(4);
dms_dns_global.dms_nlp.dms_nlp_process_maxfrequency.hbox_view = cfg:hBox {
    cfg:stLabel("NLP_Process_MaxFrequency:  "),
    cfg:ispinView(dms_dns_global.dms_nlp.dms_nlp_process_maxfrequency.cfg, 3000, 8000, 1),
    cfg:stLabel("(设置范围: 3000 ~ 8000，默认值：8000)"),
    cfg:stSpacer(),
};

-- 7-2) dms_nlp_process_minfrequency
dms_dns_global.dms_nlp.dms_nlp_process_minfrequency.cfg= cfg:i32("DMS_DNS_NLP_Process_MinFrequency:  ", 0);
depend_item_en_show(bluetooth_en, dms_dns_global.dms_nlp.dms_nlp_process_minfrequency.cfg);
dms_dns_global.dms_nlp.dms_nlp_process_minfrequency.cfg:setOSize(4);
dms_dns_global.dms_nlp.dms_nlp_process_minfrequency.hbox_view = cfg:hBox {
    cfg:stLabel("NLP_Process_MinFrequency:  "),
    cfg:ispinView(dms_dns_global.dms_nlp.dms_nlp_process_minfrequency.cfg, 0, 1000, 1),
    cfg:stLabel("(设置范围: 0 ~ 1000，默认值：0)"),
    cfg:stSpacer(),
};

-- 7-3) dms_overdrive
dms_dns_global.dms_nlp.dms_overdrive.cfg= cfg:dbf("DMS_DNS_OverDrive:  ", 1);
depend_item_en_show(bluetooth_en, dms_dns_global.dms_nlp.dms_overdrive.cfg);
dms_dns_global.dms_nlp.dms_overdrive.cfg:setOSize(4);
dms_dns_global.dms_nlp.dms_overdrive.hbox_view = cfg:hBox {
    cfg:stLabel("OverDrive:  "),
    cfg:dspinView(dms_dns_global.dms_nlp.dms_overdrive.cfg, 0.0, 30.0, 0.1, 1),
    cfg:stLabel("(设置范围: 0 ~ 30，默认值：1)"),
    cfg:stSpacer(),
};


-- 8) dms_ans
dms_dns_global.dms_ans = {
    dns_aggress = {},
    dns_suppress = {},
    dns_init_noise_lvl = {},
};

-- 8-1) dns_aggress
dms_dns_global.dms_ans.dns_aggress.cfg= cfg:dbf("DMS_DNS_DNS_AGGRESS:  ", 1.0);
depend_item_en_show(bluetooth_en, dms_dns_global.dms_ans.dns_aggress.cfg);
dms_dns_global.dms_ans.dns_aggress.cfg:setOSize(4);
dms_dns_global.dms_ans.dns_aggress.hbox_view = cfg:hBox {
    cfg:stLabel("DNS_OverDrive：" .. TAB_TABLE[1]),
    cfg:dspinView(dms_dns_global.dms_ans.dns_aggress.cfg, 0.3, 6.0, 0.1, 1),
    cfg:stLabel("(降噪强度,越大降噪越强，设置范围: 0.3 ~ 6.0，默认值：1.0)"),
    cfg:stSpacer(),
};


-- 8-2) dns_suppress
dms_dns_global.dms_ans.dns_suppress.cfg= cfg:dbf("DMS_DNS_DNS_SUPPRESS：", 0.1);
depend_item_en_show(bluetooth_en, dms_dns_global.dms_ans.dns_suppress.cfg);
dms_dns_global.dms_ans.dns_suppress.cfg:setOSize(4);
dms_dns_global.dms_ans.dns_suppress.hbox_view = cfg:hBox {
    cfg:stLabel("DNS_GainFloor：" .. TAB_TABLE[1]),
    cfg:dspinView(dms_dns_global.dms_ans.dns_suppress.cfg, 0.0, 1.0, 0.01, 2),
    cfg:stLabel("(增益最小值控制,越小降噪越强，设置范围: 0 ~ 1.0，默认值：0.1)"),
    cfg:stSpacer(),
};

-- 8-3) dns_init_noise_lvl
dms_dns_global.dms_ans.dns_init_noise_lvl.cfg= cfg:dbf("DNS_NoiseLevel:  ", -75);
depend_item_en_show(bluetooth_en, dms_dns_global.dms_ans.dns_init_noise_lvl.cfg);
dms_dns_global.dms_ans.dns_init_noise_lvl.cfg:setOSize(4);
dms_dns_global.dms_ans.dns_init_noise_lvl.hbox_view = cfg:hBox {
    cfg:stLabel(dms_dns_global.dms_ans.dns_init_noise_lvl.cfg.name),
    cfg:dspinView(dms_dns_global.dms_ans.dns_init_noise_lvl.cfg, -100, -30, 0.1, 1),
    cfg:stLabel("(设置范围: -100 ~ -30，默认值：-75 dB)"),
    cfg:stSpacer(),
};

-- 9) dms_enc
dms_dns_global.dms_enc = {
    dms_enc_process_maxfreq = {},
    dms_enc_process_minfreq = {},
    dms_sir_maxfreq = {},
    dms_mic_distance = {},
    dms_target_singal_degradation = {},
	dms_mic_rms_diff = {},
    dms_enc_aggressfactor = {},
    dms_enc_minsuppress = {},
};

-- 9-1) dms_enc_process_maxfreq
dms_dns_global.dms_enc.dms_enc_process_maxfreq.cfg= cfg:i32("DMS_DNS_ENC_Process_MaxFreq:  ", 8000);
depend_item_en_show(bluetooth_en, dms_dns_global.dms_enc.dms_enc_process_maxfreq.cfg);
dms_dns_global.dms_enc.dms_enc_process_maxfreq.cfg:setOSize(4);
dms_dns_global.dms_enc.dms_enc_process_maxfreq.hbox_view = cfg:hBox {
    cfg:stLabel("ENC_Process_MaxFreq:  "),
    cfg:ispinView(dms_dns_global.dms_enc.dms_enc_process_maxfreq.cfg, 3000, 8000, 1),
    cfg:stLabel("(设置范围: 3000 ~ 8000，默认值：8000)"),
    cfg:stSpacer(),
};

-- 9-2) dms_enc_process_minfreq
dms_dns_global.dms_enc.dms_enc_process_minfreq.cfg= cfg:i32("DMS_DNS_ENC_Process_MinFreq:  ", 0);
depend_item_en_show(bluetooth_en, dms_dns_global.dms_enc.dms_enc_process_minfreq.cfg);
dms_dns_global.dms_enc.dms_enc_process_minfreq.cfg:setOSize(4);
dms_dns_global.dms_enc.dms_enc_process_minfreq.hbox_view = cfg:hBox {
    cfg:stLabel("ENC_Process_MinFreq:  "),
    cfg:ispinView(dms_dns_global.dms_enc.dms_enc_process_minfreq.cfg, 0, 1000, 1),
    cfg:stLabel("(设置范围: 0 ~ 1000，默认值：0)"),
    cfg:stSpacer(),
};

-- 9-3) dms_sir_maxfreq
dms_dns_global.dms_enc.dms_sir_maxfreq.cfg= cfg:i32("DMS_DNS_SIR_MaxFreq:  ", 3000);
depend_item_en_show(bluetooth_en, dms_dns_global.dms_enc.dms_sir_maxfreq.cfg);
dms_dns_global.dms_enc.dms_sir_maxfreq.cfg:setOSize(4);
dms_dns_global.dms_enc.dms_sir_maxfreq.hbox_view = cfg:hBox {
    cfg:stLabel("SIR_MaxFreq:  "),
    cfg:ispinView(dms_dns_global.dms_enc.dms_sir_maxfreq.cfg, 1000, 8000, 1),
    cfg:stLabel("(设置范围: 1000 ~ 8000，默认值：3000)"),
    cfg:stSpacer(),
};

-- 9-4) dms_mic_distance
dms_dns_global.dms_enc.dms_mic_distance.cfg= cfg:dbf("DMS_DNS_Mic_Distance:  ", 0.015);
depend_item_en_show(bluetooth_en, dms_dns_global.dms_enc.dms_mic_distance.cfg);
dms_dns_global.dms_enc.dms_mic_distance.cfg:setOSize(4);
dms_dns_global.dms_enc.dms_mic_distance.int_cfg = cfg:i32("DMS_DNS_Mic_Distance:i32:", 15);
dms_dns_global.dms_enc.dms_mic_distance.hbox_view = cfg:hBox {
    cfg:stLabel("Mic_Distance:  "),
    cfg:ispinView(dms_dns_global.dms_enc.dms_mic_distance.int_cfg, 15, 35, 1),
    cfg:stLabel("(设置范围: 15 ~ 35，默认值：15，单位:mm)"),
    cfg:stSpacer(),
};
dms_dns_global.dms_enc.dms_mic_distance.cfg:addDeps({dms_dns_global.dms_enc.dms_mic_distance.int_cfg});
dms_dns_global.dms_enc.dms_mic_distance.cfg:setEval(function ()
    return dms_dns_global.dms_enc.dms_mic_distance.int_cfg.val / 1000.0;
end);

--    ) dms_mic_rms_diff
dms_dns_global.dms_enc.dms_mic_rms_diff.cfg = cfg:dbf("DMS_DNS_Mic RMS diff:  ", 1);
depend_item_en_show(bluetooth_en, dms_dns_global.dms_enc.dms_mic_rms_diff.cfg);

dms_dns_global.dms_enc.dms_mic_rms_diff.hbox_view = cfg:hBox {
    cfg:stLabel("Mic RMS diff:  "),
    cfg:dspinView(dms_dns_global.dms_enc.dms_mic_rms_diff.cfg, -6, 0, 0.01, 2);
    cfg:stLabel("（设置范围：-6 ~ 0，默认值：0）"),
	cfg:stSpacer(),
};

-- 9-5) dms_target_singal_degradation
dms_dns_global.dms_enc.dms_target_singal_degradation.cfg= cfg:dbf("DMS_DNS_Target_Signal_Degradation:  ", 1);
depend_item_en_show(bluetooth_en, dms_dns_global.dms_enc.dms_target_singal_degradation.cfg);
dms_dns_global.dms_enc.dms_target_singal_degradation.cfg:setOSize(4);
dms_dns_global.dms_enc.dms_target_singal_degradation.hbox_view = cfg:hBox {
    cfg:stLabel("Target_Signal_Degradation:  "),
    cfg:labelView(dms_dns_global.dms_enc.dms_target_singal_degradation.cfg),
    cfg:stLabel("(设置范围: 0 ~ 1，默认值：1)"),
    cfg:stSpacer(),
};

dms_dns_global.dms_enc.dms_target_singal_degradation.cfg:addDeps({dms_dns_global.dms_enc.dms_mic_rms_diff.cfg});
dms_dns_global.dms_enc.dms_target_singal_degradation.cfg:setEval(function ()
    return 10.0 ^ (dms_dns_global.dms_enc.dms_mic_rms_diff.cfg.val / 20.0); end);

-- 9-6) dms_enc_aggressfactor
dms_dns_global.dms_enc.dms_enc_aggressfactor.cfg= cfg:dbf("DMS_DNS_ENC_Aggressfactor:  ", 4);
depend_item_en_show(bluetooth_en, dms_dns_global.dms_enc.dms_enc_aggressfactor.cfg);
dms_dns_global.dms_enc.dms_enc_aggressfactor.cfg:setOSize(4);
dms_dns_global.dms_enc.dms_enc_aggressfactor.hbox_view = cfg:hBox {
    cfg:stLabel("ENC_Aggressfactor:  "),
    cfg:dspinView(dms_dns_global.dms_enc.dms_enc_aggressfactor.cfg, 0, 4, 0.0001, 4),
    cfg:stLabel("(设置范围: 0 ~ 4，默认值：4.0)"),
    cfg:stSpacer(),
};

-- 9-7) dms_enc_minsuppress
dms_dns_global.dms_enc.dms_enc_minsuppress.cfg= cfg:dbf("DMS_DNS_ENC_Minsuppress:  ", 0.09);
depend_item_en_show(bluetooth_en, dms_dns_global.dms_enc.dms_enc_minsuppress.cfg);
dms_dns_global.dms_enc.dms_enc_minsuppress.cfg:setOSize(4);
dms_dns_global.dms_enc.dms_enc_minsuppress.hbox_view = cfg:hBox {
    cfg:stLabel("ENC_Minsuppress:  "),
    cfg:dspinView(dms_dns_global.dms_enc.dms_enc_minsuppress.cfg, 0, 0.1, 0.0001, 4),
    cfg:stLabel("(设置范围: 0 ~ 0.1，默认值：0.09)"),
    cfg:stSpacer(),
};

-- 10) dms_common 
dms_dns_global.dms_common = {
    global_minsuppress = {},
};
dms_dns_global.dms_common.global_minsuppress.cfg= cfg:dbf("DMS_DNS_global_minsuppress:  ", 0.04);
depend_item_en_show(bluetooth_en, dms_dns_global.dms_common.global_minsuppress.cfg);
dms_dns_global.dms_common.global_minsuppress.cfg:setOSize(4);
dms_dns_global.dms_common.global_minsuppress.hbox_view = cfg:hBox {
    cfg:stLabel("global_minsuppress:  "),
    cfg:dspinView(dms_dns_global.dms_common.global_minsuppress.cfg, 0.0, 1.0, 0.01, 2),
    cfg:stLabel("(设置范围: 0.0 ~ 1.0，默认值：0.04)"),
    cfg:stSpacer(),
};

--========================= DMS 输出汇总  ============================
dms_dns_global.dms_item_table = {
    -- aec cfg
        dms_dns_global.dms_mic_analog_gain.cfg, -- 1 Bytes
        dms_dns_global.dms_dac_analog_gain.cfg, -- 1 Bytes
        dms_dns_global.dms_aec_mode.cfg,        -- 1 Bytes
        dms_dns_global.dms_ul_eq_en.cfg,        -- 1 Bytes

        dms_dns_global.dms_agc.dms_ndt_fade_in.cfg,  -- 4 Bytes
        dms_dns_global.dms_agc.dms_ndt_fade_out.cfg,  -- 4 Bytes
        dms_dns_global.dms_agc.dms_dt_fade_in.cfg,  -- 4 Bytes
        dms_dns_global.dms_agc.dms_dt_fade_out.cfg,  -- 4 Bytes
        dms_dns_global.dms_agc.dms_ndt_max_gain.cfg,  -- 4 Bytes
        dms_dns_global.dms_agc.dms_ndt_min_gain.cfg,  -- 4 Bytes
        dms_dns_global.dms_agc.dms_ndt_speech_thr.cfg,  -- 4 Bytes
        dms_dns_global.dms_agc.dms_dt_max_gain.cfg,  -- 4 Bytes
        dms_dns_global.dms_agc.dms_dt_min_gain.cfg,  -- 4 Bytes
        dms_dns_global.dms_agc.dms_dt_speech_thr.cfg,  -- 4 Bytes
        dms_dns_global.dms_agc.dms_echo_present_thr.cfg,  -- 4 Bytes

        dms_dns_global.dms_aec.dms_aec_process_maxfrequency.cfg,  -- 4 Bytes
        dms_dns_global.dms_aec.dms_aec_process_minfrequency.cfg,  -- 4 Bytes
        dms_dns_global.dms_aec.dms_af_length.cfg,  -- 4 Bytes

        dms_dns_global.dms_nlp.dms_nlp_process_maxfrequency.cfg,  -- 4 Bytes
        dms_dns_global.dms_nlp.dms_nlp_process_minfrequency.cfg,  -- 4 Bytes
        dms_dns_global.dms_nlp.dms_overdrive.cfg,  -- 4 Bytes

        dms_dns_global.dms_ans.dns_aggress.cfg,  -- 4 Bytes
        dms_dns_global.dms_ans.dns_suppress.cfg,  -- 4 Bytes
        dms_dns_global.dms_ans.dns_init_noise_lvl.cfg,  -- 4 Bytes

        dms_dns_global.dms_enc.dms_enc_process_maxfreq.cfg,  -- 4 Bytes
        dms_dns_global.dms_enc.dms_enc_process_minfreq.cfg,  -- 4 Bytes
        dms_dns_global.dms_enc.dms_sir_maxfreq.cfg,  -- 4 Bytes
        dms_dns_global.dms_enc.dms_mic_distance.cfg,  -- 4 Bytes
        dms_dns_global.dms_enc.dms_target_singal_degradation.cfg,  -- 4 Bytes
        dms_dns_global.dms_enc.dms_enc_aggressfactor.cfg,  -- 4 Bytes
        dms_dns_global.dms_enc.dms_enc_minsuppress.cfg,  -- 4 Bytes

        dms_dns_global.dms_common.global_minsuppress.cfg, -- 4 Bytes
};


dms_dns_global.dms_output_type_group_view = {};
dms_dns_global.dms_output_type_group_view.agc_type_group_view = cfg:stGroup(" AGC ",
    cfg:vaBox {
        dms_dns_global.dms_agc.dms_ndt_fade_in.hbox_view,
        dms_dns_global.dms_agc.dms_ndt_fade_out.hbox_view,
        dms_dns_global.dms_agc.dms_dt_fade_in.hbox_view,
        dms_dns_global.dms_agc.dms_dt_fade_out.hbox_view,
        dms_dns_global.dms_agc.dms_ndt_max_gain.hbox_view,
        dms_dns_global.dms_agc.dms_ndt_min_gain.hbox_view,
        dms_dns_global.dms_agc.dms_ndt_speech_thr.hbox_view,
        dms_dns_global.dms_agc.dms_dt_max_gain.hbox_view,
        dms_dns_global.dms_agc.dms_dt_min_gain.hbox_view,
        dms_dns_global.dms_agc.dms_dt_speech_thr.hbox_view,
        dms_dns_global.dms_agc.dms_echo_present_thr.hbox_view,
    }
);

dms_dns_global.dms_output_type_group_view.aec_type_group_view = cfg:stGroup(" AEC ",
    cfg:vaBox {
        dms_dns_global.dms_aec.dms_aec_process_maxfrequency.hbox_view,
        dms_dns_global.dms_aec.dms_aec_process_minfrequency.hbox_view,
        dms_dns_global.dms_aec.dms_af_length.hbox_view,
    }
);

dms_dns_global.dms_output_type_group_view.nlp_type_group_view = cfg:stGroup(" NLP ",
    cfg:vaBox {
        dms_dns_global.dms_nlp.dms_nlp_process_maxfrequency.hbox_view,
        dms_dns_global.dms_nlp.dms_nlp_process_minfrequency.hbox_view,
        dms_dns_global.dms_nlp.dms_overdrive.hbox_view,
    }
);

dms_dns_global.dms_output_type_group_view.ans_type_group_view = cfg:stGroup(" DNS ",
    cfg:vaBox {
        dms_dns_global.dms_ans.dns_aggress.hbox_view,
        dms_dns_global.dms_ans.dns_suppress.hbox_view,
        dms_dns_global.dms_ans.dns_init_noise_lvl.hbox_view,
    }
);

dms_dns_global.dms_output_type_group_view.enc_type_group_view = cfg:stGroup(" ENC ",
    cfg:vaBox {
        dms_dns_global.dms_enc.dms_enc_process_maxfreq.hbox_view,
        dms_dns_global.dms_enc.dms_enc_process_minfreq.hbox_view,
        dms_dns_global.dms_enc.dms_sir_maxfreq.hbox_view,
        dms_dns_global.dms_enc.dms_mic_distance.hbox_view,
        dms_dns_global.dms_enc.dms_mic_rms_diff.hbox_view,
        dms_dns_global.dms_enc.dms_target_singal_degradation.hbox_view,
        dms_dns_global.dms_enc.dms_enc_aggressfactor.hbox_view,
        dms_dns_global.dms_enc.dms_enc_minsuppress.hbox_view,
    }
);

dms_dns_global.dms_output_view_table = {
    dms_dns_global.dms_reference_book_view,
    -- aec cfg
    dms_dns_global.dms_mic_analog_gain.hbox_view,
    dms_dns_global.dms_dac_analog_gain.hbox_view,
    dms_dns_global.dms_aec_mode.hbox_view,
    dms_dns_global.dms_ul_eq_en.hbox_view, 

    dms_dns_global.dms_common.global_minsuppress.hbox_view,

    dms_dns_global.dms_output_type_group_view.agc_type_group_view,
    dms_dns_global.dms_output_type_group_view.aec_type_group_view,
    dms_dns_global.dms_output_type_group_view.nlp_type_group_view,
    dms_dns_global.dms_output_type_group_view.ans_type_group_view,
    dms_dns_global.dms_output_type_group_view.enc_type_group_view,
};

aec_global.dms_dns_output_view_table_group_view = cfg:vBox(dms_dns_global.dms_output_view_table);

-- A. 输出htext
dms_dns_global.dms_output_htext_table = {
};

-- B. 输出ctext：无

dms_dns_global.layoutHook = function ()
    if (aec_cfg_type.cfg.val == aec_cfg_type.TY.dms_dns) then
        return dms_dns_global.dms_item_table;
    else
        return {};
    end
end;

dms_dns_global.parserHook = function (item)
    return dms_dns_global.dms_item_table;
end;

-- C. 输出bin：无
dms_dns_global.dms_output_bin = cfg:dynGroup("DMS_DNS_OUTPUT_BIN",
    BIN_ONLY_CFG["BT_CFG"].dms_dns.id,
    1,
    dms_dns_global.layoutHook,
    dms_dns_global.parserHook,
    nil
);

dms_dns_global.dms_output_bin:setRecoverHook(function ()
    local ov = dms_dns_global.dms_aec_mode.cfg.val;
    cfg:set(dms_dns_global.dms_aec_mode.aec_en, ov & 0x1);
    cfg:set(dms_dns_global.dms_aec_mode.nlp_en, (ov >> 1) & 0x1);
    cfg:set(dms_dns_global.dms_aec_mode.ans_en, (ov >> 2) & 0x1);
    cfg:set(dms_dns_global.dms_aec_mode.enc_en, (ov >> 3) & 0x1);
    cfg:set(dms_dns_global.dms_aec_mode.agc_en, (ov >> 4) & 0x1);
    ov = dms_dns_global.dms_enc.dms_target_singal_degradation.cfg.val;
    cfg:set(dms_dns_global.dms_enc.dms_mic_rms_diff.cfg, 20.0 * math.log(ov, 10.0));
    cfg:set(dms_dns_global.dms_enc.dms_mic_distance.int_cfg,
        math.floor(1000.0 * (dms_dns_global.dms_enc.dms_mic_distance.cfg.val + 0.0005)));
end);


--[[================================================================================
============================== DMS_FLEXIBLE ========================================
================================================================================--]]
local dms_flexible_global = {};

dms_flexible_global.dms_reference_book_view = cfg:stButton("JL DMS通话调试手册.pdf",
    function()
        local ret = cfg:utilsShellOpenFile(dms_reference_book_path);
        if (ret == false) then
            if cfg.lang == "zh" then
		        cfg:msgBox("info", dms_reference_book_path .. "文件不存在");
            else
		        cfg:msgBox("info", dms_reference_book_path .. " file not exist.");
            end
        end
    end);

-- 0) ver
dms_flexible_global.ver = {};
dms_flexible_global.ver.cfg = cfg:i32("DMS_FLEXIBLE_VER", 1);
dms_flexible_global.ver.cfg:setOSize(1);

-- 1) dms_mic_again
dms_flexible_global.dms_mic_analog_gain = {};
dms_flexible_global.dms_mic_analog_gain.cfg = cfg:i32("DMS_FLEXIBLE_MIC_AGAIN:  ", 8);
depend_item_en_show(bluetooth_en, dms_flexible_global.dms_mic_analog_gain.cfg);
dms_flexible_global.dms_mic_analog_gain.cfg:setOSize(1);
dms_flexible_global.dms_mic_analog_gain.hbox_view = cfg:hBox {
    cfg:stLabel("MIC_AGAIN:  " .. TAB_TABLE[1]),
    cfg:ispinView(dms_flexible_global.dms_mic_analog_gain.cfg, 0, 19, 1),
    cfg:stLabel("(MIC增益，0(-8dB)) ~ 19(30dB),步进：2dB 默认值：8)"),
    cfg:stSpacer(),
};

-- ) dms_mic1_gain
dms_flexible_global.dms_mic1_again = {};
dms_flexible_global.dms_mic1_again.cfg = cfg:i32("DMS_FLEXIBLE_MIC1_AGAIN:  ", 10);
depend_item_en_show(bluetooth_en, dms_flexible_global.dms_mic1_again.cfg);
dms_flexible_global.dms_mic1_again.cfg:setOSize(1);
dms_flexible_global.dms_mic1_again.hbox_view = cfg:hBox {
    cfg:stLabel("MIC1_AGAIN: " .. TAB_TABLE[1]),
    cfg:ispinView(dms_flexible_global.dms_mic1_again.cfg, 0, 19, 1);
    cfg:stLabel("(MIC增益，0(-8dB)) ~ 19(30dB),步进：2dB 默认值：10)"),
    cfg:stSpacer(),
};

-- 2) dms_dac_again
dms_flexible_global.dms_dac_analog_gain = {};
dms_flexible_global.dms_dac_analog_gain.cfg = cfg:i32("DMS_FLEXIBLE_DAC_AGAIN:  ", 10);
depend_item_en_show(bluetooth_en, dms_flexible_global.dms_dac_analog_gain.cfg);
dms_flexible_global.dms_dac_analog_gain.cfg:setOSize(1);
dms_flexible_global.dms_dac_analog_gain.hbox_view = cfg:hBox {
    cfg:stLabel("DAC_AGAIN:  " .. TAB_TABLE[1]),
    cfg:ispinView(dms_flexible_global.dms_dac_analog_gain.cfg, 0, 15, 1),
    cfg:stLabel("(DAC增益，设置范围: 0 ~ 15，步进：2dB 默认值：10)"),
    cfg:stSpacer(),
};


-- 3) aec_mode
dms_flexible_global.dms_aec_mode = {};
dms_flexible_global.dms_aec_mode.aec_en = cfg:i32("DMS_FLEXIBLE_AEC_EN:", 0);
depend_item_en_show(bluetooth_en, dms_flexible_global.dms_aec_mode.aec_en);

dms_flexible_global.dms_aec_mode.nlp_en = cfg:i32("DMS_FLEXIBLE_NLP_EN:", 1);
depend_item_en_show(bluetooth_en, dms_flexible_global.dms_aec_mode.nlp_en);

dms_flexible_global.dms_aec_mode.ans_en = cfg:i32("DMS_FLEXIBLE_ANS_EN:", 1);
depend_item_en_show(bluetooth_en, dms_flexible_global.dms_aec_mode.ans_en);

dms_flexible_global.dms_aec_mode.enc_en = cfg:i32("DMS_FLEXIBLE_ENC_EN:", 1);
depend_item_en_show(bluetooth_en, dms_flexible_global.dms_aec_mode.enc_en);

dms_flexible_global.dms_aec_mode.agc_en = cfg:i32("DMS_FLEXIBLE_AGC_EN:", 1);
depend_item_en_show(bluetooth_en, dms_flexible_global.dms_aec_mode.agc_en);

dms_flexible_global.dms_aec_mode.cfg = cfg:i32("DMS_FLEXIBLE_ModuleEnable:", 0x1E);
depend_item_en_show(bluetooth_en, dms_flexible_global.dms_aec_mode.cfg);
dms_flexible_global.dms_aec_mode.cfg:setOSize(1);
-- item_htext
--mic_analog_gain.comment_str = "设置范围：0 ~ 19, 默认值：8";
--mic_analog_gain.htext = item_output_htext(mic_analog_gain.cfg, "TCFG_AEC_MIC_ANALOG_GAIN", 3, nil, mic_analog_gain.comment_str, 1);

dms_flexible_global.dms_aec_mode.cfg:addDeps{dms_flexible_global.dms_aec_mode.aec_en,
    dms_flexible_global.dms_aec_mode.nlp_en, dms_flexible_global.dms_aec_mode.ans_en,
    dms_flexible_global.dms_aec_mode.enc_en, dms_flexible_global.dms_aec_mode.agc_en};

dms_flexible_global.dms_aec_mode.cfg:setEval(function () return ((dms_flexible_global.dms_aec_mode.aec_en.val << 0) | 
                                             (dms_flexible_global.dms_aec_mode.nlp_en.val << 1) | 
                                             (dms_flexible_global.dms_aec_mode.ans_en.val << 2) | 
                                             (dms_flexible_global.dms_aec_mode.enc_en.val << 3) |
                                             (dms_flexible_global.dms_aec_mode.agc_en.val << 4)
                                             ); end);

-- item_view
dms_flexible_global.dms_aec_mode.hbox_view = cfg:hBox {
    cfg:stLabel("ModuleEnable:"),
    --cfg:labelView(dms_flexible_global.dms_aec_mode.cfg),

    cfg:stLabel("  "),

    cfg:stLabel("AEC_EN:"),
	cfg:checkBoxView(dms_flexible_global.dms_aec_mode.aec_en),

    cfg:stLabel(" "),

    cfg:stLabel("NLP_EN:"),
	cfg:checkBoxView(dms_flexible_global.dms_aec_mode.nlp_en),
    cfg:stLabel(" "),

    cfg:stLabel("ANS_EN:"),
	cfg:checkBoxView(dms_flexible_global.dms_aec_mode.ans_en),
    cfg:stLabel(" "),

    cfg:stLabel("ENC_EN:"),
	cfg:checkBoxView(dms_flexible_global.dms_aec_mode.enc_en),
    cfg:stLabel(" "),

    cfg:stLabel("AGC_EN:"),
	cfg:checkBoxView(dms_flexible_global.dms_aec_mode.agc_en),
    cfg:stLabel(" "),

    cfg:stLabel("(模块使能)"),

    cfg:stSpacer(),
};

dms_flexible_global.dms_aec_mode_bit_en_table = {
    dms_flexible_global.dms_aec_mode.aec_en,
    dms_flexible_global.dms_aec_mode.nlp_en,
    dms_flexible_global.dms_aec_mode.ans_en,
    dms_flexible_global.dms_aec_mode.enc_en,
    dms_flexible_global.dms_aec_mode.agc_en,
};

-- 4) dms_ul_eq_en
dms_flexible_global.dms_ul_eq_en = {};

dms_flexible_global.dms_ul_eq_en.ul_eq_en_sel_table = {
        [0] = " disable",
        [1] = " enable",
};
dms_flexible_global.dms_ul_eq_en.UL_EQ_EN_SEL_TABLE = cfg:enumMap("dms_ul_eq_en_sel_table",
    dms_flexible_global.dms_ul_eq_en.ul_eq_en_sel_table);

dms_flexible_global.dms_ul_eq_en.cfg = cfg:enum("DMS_FLEXIBLE_UL_EQ_EN:  ", dms_flexible_global.dms_ul_eq_en.UL_EQ_EN_SEL_TABLE, 1);
depend_item_en_show(bluetooth_en, dms_flexible_global.dms_ul_eq_en.cfg);
dms_flexible_global.dms_ul_eq_en.cfg:setOSize(1);
dms_flexible_global.dms_ul_eq_en.hbox_view = cfg:hBox {
    cfg:stLabel("UL_EQ_EN:  " .. TAB_TABLE[1]),
    cfg:enumView(dms_flexible_global.dms_ul_eq_en.cfg),
    cfg:stLabel("(上行 EQ 使能，默认值：enable)"),
    cfg:stSpacer(),
};

dms_flexible_global.pad1 = cfg:i32("DMS_FLEXIBLE_PAD1", 0);
dms_flexible_global.pad1:setOSize(1);
dms_flexible_global.pad2 = cfg:i32("DMS_FLEXIBLE_PAD2", 0);
dms_flexible_global.pad2:setOSize(1);


-- 5) agc
dms_flexible_global.agc = {
    dms_ndt_fade_in = {},
    dms_ndt_fade_out = {},
    dms_dt_fade_in = {},
    dms_dt_fade_out = {},
    dms_ndt_max_gain = {},
    dms_ndt_min_gain = {},
    dms_ndt_speech_thr = {},
    dms_dt_max_gain = {},
    dms_dt_min_gain = {},
    dms_dt_speech_thr = {},
    dms_echo_present_thr = {},
};

-- 5-1) dms_ndt_fade_in
dms_flexible_global.agc.dms_ndt_fade_in.cfg = cfg:dbf("DMS_FLEXIBLE_NDT_FADE_IN:  ", 1.3);
depend_item_en_show(bluetooth_en, dms_flexible_global.agc.dms_ndt_fade_in.cfg);
dms_flexible_global.agc.dms_ndt_fade_in.cfg:setOSize(4);
dms_flexible_global.agc.dms_ndt_fade_in.hbox_view = cfg:hBox {
    cfg:stLabel("NDT_FADE_IN:  " .. TAB_TABLE[1]),
    cfg:dspinView(dms_flexible_global.agc.dms_ndt_fade_in.cfg, 0.1, 5.0, 0.1, 1),
    cfg:stLabel("dB (单端讲话淡入步进，设置范围: 0.1 ~ 5 dB，默认值：1.3 dB)"),
    cfg:stSpacer(),
};

-- 5-2) dms_ndt_fade_out
dms_flexible_global.agc.dms_ndt_fade_out.cfg = cfg:dbf("DMS_FLEXIBLE_NDT_FADE_OUT:  ", 0.7);
depend_item_en_show(bluetooth_en, dms_flexible_global.agc.dms_ndt_fade_out.cfg);
dms_flexible_global.agc.dms_ndt_fade_out.cfg:setOSize(4);
dms_flexible_global.agc.dms_ndt_fade_out.hbox_view = cfg:hBox {
    cfg:stLabel("NDT_FADE_OUT:  " .. TAB_TABLE[1]),
    cfg:dspinView(dms_flexible_global.agc.dms_ndt_fade_out.cfg, 0.1, 5.0, 0.1, 1),
    cfg:stLabel("dB (单端讲话淡出步进，设置范围: 0.1 ~ 5 dB，默认值：0.7 dB)"),
    cfg:stSpacer(),
};


-- 5-3) dms_dt_fade_in
dms_flexible_global.agc.dms_dt_fade_in.cfg = cfg:dbf("DMS_FLEXIBLE_DT_FADE_IN:  ", 1.3);
depend_item_en_show(bluetooth_en, dms_flexible_global.agc.dms_dt_fade_in.cfg);
dms_flexible_global.agc.dms_dt_fade_in.cfg:setOSize(4);
dms_flexible_global.agc.dms_dt_fade_in.hbox_view = cfg:hBox {
    cfg:stLabel("DT_FADE_IN:  " .. TAB_TABLE[1]),
    cfg:dspinView(dms_flexible_global.agc.dms_dt_fade_in.cfg, 0.1, 5.0, 0.1, 1),
    cfg:stLabel("dB (双端讲话淡入步进，设置范围: 0.1 ~ 5 dB，默认值：1.3 dB)"),
    cfg:stSpacer(),
};

-- 5-4) dms_dt_fade_out
dms_flexible_global.agc.dms_dt_fade_out.cfg = cfg:dbf("DMS_FLEXIBLE_DT_FADE_OUT:  ", 0.7);
depend_item_en_show(bluetooth_en, dms_flexible_global.agc.dms_dt_fade_out.cfg);
dms_flexible_global.agc.dms_dt_fade_out.cfg:setOSize(4);
dms_flexible_global.agc.dms_dt_fade_out.hbox_view = cfg:hBox {
    cfg:stLabel("DT_FADE_OUT:  " .. TAB_TABLE[1]),
    cfg:dspinView(dms_flexible_global.agc.dms_dt_fade_out.cfg, 0.1, 5.0, 0.1, 1),
    cfg:stLabel("dB (双端讲话淡出步进，设置范围: 0.1 ~ 5 dB，默认值：0.7 dB)"),
    cfg:stSpacer(),
};

-- 5-5) dms_ndt_max_gain
dms_flexible_global.agc.dms_ndt_max_gain.cfg = cfg:dbf("DMS_FLEXIBLE_NDT_MAX_GAIN:  ", 8.0);
depend_item_en_show(bluetooth_en, dms_flexible_global.agc.dms_ndt_max_gain.cfg);
dms_flexible_global.agc.dms_ndt_max_gain.cfg:setOSize(4);
dms_flexible_global.agc.dms_ndt_max_gain.hbox_view = cfg:hBox {
    cfg:stLabel("NDT_MAX_GAIN:  " .. TAB_TABLE[1]),
    cfg:dspinView(dms_flexible_global.agc.dms_ndt_max_gain.cfg, -90.0, 40.0, 0.1, 1),
    cfg:stLabel("(单端讲话放大上限， 设置范围: -90.0 ~ 40.0 dB，默认值：8.0 dB)"),
    cfg:stSpacer(),
};

-- 5-6) dms_ndt_min_gain
dms_flexible_global.agc.dms_ndt_min_gain.cfg = cfg:dbf("DMS_FLEXIBLE_NDT_MIN_GAIN:  ", 4.0);
depend_item_en_show(bluetooth_en, dms_flexible_global.agc.dms_ndt_min_gain.cfg);
dms_flexible_global.agc.dms_ndt_min_gain.cfg:setOSize(4);
dms_flexible_global.agc.dms_ndt_min_gain.hbox_view = cfg:hBox {
    cfg:stLabel("NDT_MIN_GAIN:  " .. TAB_TABLE[1]),
    cfg:dspinView(dms_flexible_global.agc.dms_ndt_min_gain.cfg, -90.0, 40.0, 0.1, 1),
    cfg:stLabel("(单端讲话放大下限， 设置范围: -90.0 ~ 40.0 dB，默认值：4 dB)"),
    cfg:stSpacer(),
};


-- 5-7) dms_ndt_speech_thr
dms_flexible_global.agc.dms_ndt_speech_thr.cfg = cfg:dbf("DMS_FLEXIBLE_NDT_SPEECH_THR:  ", -50.0);
depend_item_en_show(bluetooth_en, dms_flexible_global.agc.dms_ndt_speech_thr.cfg);
dms_flexible_global.agc.dms_ndt_speech_thr.cfg:setOSize(4);
dms_flexible_global.agc.dms_ndt_speech_thr.hbox_view = cfg:hBox {
    cfg:stLabel("NDT_SPEECH_THR: "),
    cfg:dspinView(dms_flexible_global.agc.dms_ndt_speech_thr.cfg, -70.0, -40.0, 0.1, 1),
    cfg:stLabel("(单端讲话放大阈值， 设置范围: -70.0 ~ -40.0 dB，默认值：-50.0 dB)"),
    cfg:stSpacer(),
};

-- 5-8) dms_dt_max_gain
dms_flexible_global.agc.dms_dt_max_gain.cfg = cfg:dbf("DMS_FLEXIBLE_DT_MAX_GAIN:  ", 12.0);
depend_item_en_show(bluetooth_en, dms_flexible_global.agc.dms_dt_max_gain.cfg);
dms_flexible_global.agc.dms_dt_max_gain.cfg:setOSize(4);
dms_flexible_global.agc.dms_dt_max_gain.hbox_view = cfg:hBox {
    cfg:stLabel("DT_MAX_GAIN:  " .. TAB_TABLE[1]),
    cfg:dspinView(dms_flexible_global.agc.dms_dt_max_gain.cfg, -90.0, 40.0, 0.1, 1),
    cfg:stLabel("(双端讲话放大上限， 设置范围: -90.0 ~ 40.0 dB，默认值：12.0 dB)"),
    cfg:stSpacer(),
};

-- 5-9) dms_dt_min_gain
dms_flexible_global.agc.dms_dt_min_gain.cfg = cfg:dbf("DMS_FLEXIBLE_DT_MIN_GAIN:  ", 0.0);
depend_item_en_show(bluetooth_en, dms_flexible_global.agc.dms_dt_min_gain.cfg);
dms_flexible_global.agc.dms_dt_min_gain.cfg:setOSize(4);
dms_flexible_global.agc.dms_dt_min_gain.hbox_view = cfg:hBox {
    cfg:stLabel("DT_MIN_GAIN:  " .. TAB_TABLE[1]),
    cfg:dspinView(dms_flexible_global.agc.dms_dt_min_gain.cfg, -90.0, 40.0, 0.1, 1),
    cfg:stLabel("(双端讲话放大下限， 设置范围: -90.0 ~ 40.0 dB，默认值：0 dB)"),
    cfg:stSpacer(),
};

-- 5-10) dms_dt_speech_thr
dms_flexible_global.agc.dms_dt_speech_thr.cfg = cfg:dbf("DMS_FLEXIBLE_DT_SPEECH_THR:  ", -40.0);
depend_item_en_show(bluetooth_en, dms_flexible_global.agc.dms_dt_speech_thr.cfg);
dms_flexible_global.agc.dms_dt_speech_thr.cfg:setOSize(4);
dms_flexible_global.agc.dms_dt_speech_thr.hbox_view = cfg:hBox {
    cfg:stLabel("DT_SPEECH_THR:  "),
    cfg:dspinView(dms_flexible_global.agc.dms_dt_speech_thr.cfg, -70.0, -40.0, 0.1, 1),
    cfg:stLabel("(双端讲话放大阈值， 设置范围: -70.0 ~ -40.0 dB，默认值：-40.0 dB)"),
    cfg:stSpacer(),
};


-- 5-11) dms_echo_present_thr
dms_flexible_global.agc.dms_echo_present_thr.cfg = cfg:dbf("DMS_FLEXIBLE_ECHO_PRESENT_THR:  ", -70.0);
depend_item_en_show(bluetooth_en, dms_flexible_global.agc.dms_echo_present_thr.cfg);
dms_flexible_global.agc.dms_echo_present_thr.cfg:setOSize(4);
dms_flexible_global.agc.dms_echo_present_thr.hbox_view = cfg:hBox {
    cfg:stLabel("ECHO_PRESENT_THR: "),
    cfg:dspinView(dms_flexible_global.agc.dms_echo_present_thr.cfg, -70.0, -40.0, 0.1, 1),
    cfg:stLabel("(单端双端讲话阈值， 设置范围: -70.0 ~ -40.0 dB，默认值：-70.0 dB)"),
    cfg:stSpacer(),
};

-- 6) aec
dms_flexible_global.aec = {
    aec_process_maxfrequency = {},
    aec_process_minfrequency = {},
    af_length = {},
};

-- 6-1) aec_process_maxfrequency
dms_flexible_global.aec.aec_process_maxfrequency.cfg= cfg:i32("DMS_FLEXIBLE_AEC_Process_MaxFrequency:  ", 8000);
depend_item_en_show(bluetooth_en, dms_flexible_global.aec.aec_process_maxfrequency.cfg);
dms_flexible_global.aec.aec_process_maxfrequency.cfg:setOSize(4);
dms_flexible_global.aec.aec_process_maxfrequency.hbox_view = cfg:hBox {
    cfg:stLabel(dms_flexible_global.aec.aec_process_maxfrequency.cfg.name),
    cfg:ispinView(dms_flexible_global.aec.aec_process_maxfrequency.cfg, 3000, 8000, 1),
    cfg:stLabel("(设置范围: 3000 ~ 8000，默认值：8000)"),
    cfg:stSpacer(),
};

-- 6-2) aec_process_minfrequency
dms_flexible_global.aec.aec_process_minfrequency.cfg= cfg:i32("DMS_FLEXIBLE_AEC_Process_MinFrequency:  ", 0);
depend_item_en_show(bluetooth_en, dms_flexible_global.aec.aec_process_minfrequency.cfg);
dms_flexible_global.aec.aec_process_minfrequency.cfg:setOSize(4);
dms_flexible_global.aec.aec_process_minfrequency.hbox_view = cfg:hBox {
    cfg:stLabel(dms_flexible_global.aec.aec_process_minfrequency.cfg.name),
    cfg:ispinView(dms_flexible_global.aec.aec_process_minfrequency.cfg, 0, 1000, 1),
    cfg:stLabel("(设置范围: 0 ~ 1000，默认值：0)"),
    cfg:stSpacer(),
};


-- 6-3) af_length
dms_flexible_global.aec.af_length.cfg= cfg:i32("DMS_FLEXIBLE_AEC_AF_Length:  ", 128);
depend_item_en_show(bluetooth_en, dms_flexible_global.aec.af_length.cfg);
dms_flexible_global.aec.af_length.cfg:setOSize(4);
dms_flexible_global.aec.af_length.hbox_view = cfg:hBox {
    cfg:stLabel(dms_flexible_global.aec.af_length.cfg.name),
    cfg:ispinView(dms_flexible_global.aec.af_length.cfg, 128, 256, 1),
    cfg:stLabel("(设置范围: 128 ~ 256，默认值：128)"),
    cfg:stSpacer(),
};

-- 7) dms_nlp
dms_flexible_global.nlp = {
    nlp_process_maxfrequency = {},
    nlp_process_minfrequency = {},
    overdrive = {},
};

-- 7-1) nlp_process_maxfrequency
dms_flexible_global.nlp.nlp_process_maxfrequency.cfg= cfg:i32("DMS_FLEXIBLE_NLP_Process_MaxFrequency:  ", 8000);
depend_item_en_show(bluetooth_en, dms_flexible_global.nlp.nlp_process_maxfrequency.cfg);
dms_flexible_global.nlp.nlp_process_maxfrequency.cfg:setOSize(4);
dms_flexible_global.nlp.nlp_process_maxfrequency.hbox_view = cfg:hBox {
    cfg:stLabel(dms_flexible_global.nlp.nlp_process_maxfrequency.cfg.name),
    cfg:ispinView(dms_flexible_global.nlp.nlp_process_maxfrequency.cfg, 3000, 8000, 1),
    cfg:stLabel("(设置范围: 3000 ~ 8000，默认值：8000)"),
    cfg:stSpacer(),
};

-- 7-2) nlp_process_minfrequency
dms_flexible_global.nlp.nlp_process_minfrequency.cfg= cfg:i32("DMS_FLEXIBLE_NLP_Process_MinFrequency:  ", 0);
depend_item_en_show(bluetooth_en, dms_flexible_global.nlp.nlp_process_minfrequency.cfg);
dms_flexible_global.nlp.nlp_process_minfrequency.cfg:setOSize(4);
dms_flexible_global.nlp.nlp_process_minfrequency.hbox_view = cfg:hBox {
    cfg:stLabel(dms_flexible_global.nlp.nlp_process_minfrequency.cfg.name),
    cfg:ispinView(dms_flexible_global.nlp.nlp_process_minfrequency.cfg, 0, 1000, 1),
    cfg:stLabel("(设置范围: 0 ~ 1000，默认值：0)"),
    cfg:stSpacer(),
};

-- 7-3) overdrive
dms_flexible_global.nlp.overdrive.cfg= cfg:dbf("DMS_FLEXIBLE_NLP_OverDrive:  ", 1);
depend_item_en_show(bluetooth_en, dms_flexible_global.nlp.overdrive.cfg);
dms_flexible_global.nlp.overdrive.cfg:setOSize(4);
dms_flexible_global.nlp.overdrive.hbox_view = cfg:hBox {
    cfg:stLabel(dms_flexible_global.nlp.overdrive.cfg.name),
    cfg:dspinView(dms_flexible_global.nlp.overdrive.cfg, 0.0, 30.0, 0.1, 1),
    cfg:stLabel("(设置范围: 0 ~ 30，默认值：1)"),
    cfg:stSpacer(),
};

-- 8) ans
dms_flexible_global.ans = {
    aggress = {},
    suppress = {},
    init_noise_lvl = {},
};

-- 8-1) aggress
dms_flexible_global.ans.aggress.cfg= cfg:dbf("DMS_FLEXIBLE_DNS_AGGRESS:  ", 1.25);
depend_item_en_show(bluetooth_en, dms_flexible_global.ans.aggress.cfg);
dms_flexible_global.ans.aggress.cfg:setOSize(4);
dms_flexible_global.ans.aggress.hbox_view = cfg:hBox {
    cfg:stLabel("AggressFactor：" .. TAB_TABLE[1]),
    cfg:dspinView(dms_flexible_global.ans.aggress.cfg, 0.0, 2.0, 0.01, 2),
    cfg:stLabel("(设置范围: 0 ~ 2，默认值：1.25)"),
    cfg:stSpacer(),
};


-- 8-2) suppress
dms_flexible_global.ans.suppress.cfg= cfg:dbf("DMS_FLEXIBLE_DNS_SUPPRESS：", 0.04);
depend_item_en_show(bluetooth_en, dms_flexible_global.ans.suppress.cfg);
dms_flexible_global.ans.suppress.cfg:setOSize(4);
dms_flexible_global.ans.suppress.hbox_view = cfg:hBox {
    cfg:stLabel("MinSuppress：" .. TAB_TABLE[1]),
    cfg:dspinView(dms_flexible_global.ans.suppress.cfg, 0.01, 0.1, 0.01, 2),
    cfg:stLabel("(设置范围: 0.01 ~ 0.1，默认值：0.04)"),
    cfg:stSpacer(),
};


-- 8-3) noise_lvl
dms_flexible_global.ans.init_noise_lvl.cfg= cfg:dbf("DMS_FLEXIBLE_DNS_NOISE_LVL：", -75);
depend_item_en_show(bluetooth_en, dms_flexible_global.ans.init_noise_lvl.cfg);
dms_flexible_global.ans.suppress.cfg:setOSize(4);
dms_flexible_global.ans.init_noise_lvl.hbox_view = cfg:hBox {
    cfg:stLabel("MicNoiseLevel：" .. TAB_TABLE[1]),
    cfg:dspinView(dms_flexible_global.ans.init_noise_lvl.cfg, -100, -30, 0.1, 1),
    cfg:stLabel("(Mic初始噪声水平，设置范围: -100 ~ -30，单位：dB 默认值：-75)"),
    cfg:stSpacer(),
};


-- 9) enc
dms_flexible_global.enc = {
    enc_suppress_pre = {},
    enc_suppress_post = {},
    enc_minsuppress = {},
    enc_disconverge_erle_thr = {},
};

-- 9-1) enc_process_maxfreq
dms_flexible_global.enc.enc_suppress_pre.cfg= cfg:dbf("DMS_FLEXIBLE_ENC_Suppress_pre:  ", 0.6);
depend_item_en_show(bluetooth_en, dms_flexible_global.enc.enc_suppress_pre.cfg);
dms_flexible_global.enc.enc_suppress_pre.cfg:setOSize(4);
dms_flexible_global.enc.enc_suppress_pre.hbox_view = cfg:hBox {
    cfg:stLabel("ENC_Suppress_Pre:     "),
    cfg:dspinView(dms_flexible_global.enc.enc_suppress_pre.cfg, 0, 1, 0.01, 2),
    cfg:stLabel("(前级降噪压制，越大越强，设置范围: 0 ~ 1，默认值：0.6)"),
    cfg:stSpacer(),
};

-- 9-2) enc_process_minfreq
dms_flexible_global.enc.enc_suppress_post.cfg= cfg:dbf("DMS_FLEXIBLE_ENC_Suppress_post:  ", 0.15);
depend_item_en_show(bluetooth_en, dms_flexible_global.enc.enc_suppress_post.cfg);
dms_flexible_global.enc.enc_suppress_post.cfg:setOSize(4);
dms_flexible_global.enc.enc_suppress_post.hbox_view = cfg:hBox {
    cfg:stLabel("ENC_Suppress_Post:    "),
    cfg:dspinView(dms_flexible_global.enc.enc_suppress_post.cfg, 0, 1, 0.01, 2),
    cfg:stLabel("(后级降噪压制，越大越强，设置范围: 0 ~ 1，默认值：0.15)"),
    cfg:stSpacer(),
};

-- 9-3) dms_sir_maxfreq
dms_flexible_global.enc.enc_minsuppress.cfg= cfg:dbf("DMS_FLEXIBLE_ENC_Minsuppress:  ", 0.09);
depend_item_en_show(bluetooth_en, dms_flexible_global.enc.enc_minsuppress.cfg);
dms_flexible_global.enc.enc_minsuppress.cfg:setOSize(4);
dms_flexible_global.enc.enc_minsuppress.hbox_view = cfg:hBox {
    cfg:stLabel("ENC_Minsuppress:      "),
    cfg:dspinView(dms_flexible_global.enc.enc_minsuppress.cfg, 0, 1, 0.01, 2),
    cfg:stLabel("(后级降噪压制下限，设置范围: 0 ~ 1，默认值：0.09)"),
    cfg:stSpacer(),
};

-- 9-4) dms_disconverge_erle_thr
dms_flexible_global.enc.enc_disconverge_erle_thr.cfg= cfg:dbf("DMS_FLEXIBLE_ENC_Disconverge_ERLE_Thr:  ", 0.09);
depend_item_en_show(bluetooth_en, dms_flexible_global.enc.enc_disconverge_erle_thr.cfg);
dms_flexible_global.enc.enc_disconverge_erle_thr.cfg:setOSize(4);
dms_flexible_global.enc.enc_disconverge_erle_thr.hbox_view = cfg:hBox {
    cfg:stLabel("ENC_Disconverge_Thr:  "),
    cfg:dspinView(dms_flexible_global.enc.enc_disconverge_erle_thr.cfg, -20, 5, 0.1, 1),
    cfg:stLabel("(滤波器发散控制阈值，设置范围: -20 ~ 5，默认值：-6)"),
    cfg:stSpacer(),
};



--========================= DMS 输出汇总  ============================
dms_flexible_global.dms_item_table = {
    -- aec cfg
        -- dms_flexible_global.ver.cfg, -- 1 Bytes
        dms_flexible_global.dms_mic_analog_gain.cfg, -- 1 Bytes
        dms_flexible_global.dms_mic1_again.cfg, -- 1 bytes
        dms_flexible_global.dms_dac_analog_gain.cfg, -- 1 Bytes
        dms_flexible_global.dms_aec_mode.cfg,        -- 1 Bytes
        dms_flexible_global.dms_ul_eq_en.cfg,        -- 1 Bytes
        dms_flexible_global.pad1,
        dms_flexible_global.pad2,

        dms_flexible_global.agc.dms_ndt_fade_in.cfg,  -- 4 Bytes
        dms_flexible_global.agc.dms_ndt_fade_out.cfg,  -- 4 Bytes
        dms_flexible_global.agc.dms_dt_fade_in.cfg,  -- 4 Bytes
        dms_flexible_global.agc.dms_dt_fade_out.cfg,  -- 4 Bytes
        dms_flexible_global.agc.dms_ndt_max_gain.cfg,  -- 4 Bytes
        dms_flexible_global.agc.dms_ndt_min_gain.cfg,  -- 4 Bytes
        dms_flexible_global.agc.dms_ndt_speech_thr.cfg,  -- 4 Bytes
        dms_flexible_global.agc.dms_dt_max_gain.cfg,  -- 4 Bytes
        dms_flexible_global.agc.dms_dt_min_gain.cfg,  -- 4 Bytes
        dms_flexible_global.agc.dms_dt_speech_thr.cfg,  -- 4 Bytes
        dms_flexible_global.agc.dms_echo_present_thr.cfg,  -- 4 Bytes

        dms_flexible_global.aec.aec_process_maxfrequency.cfg,  -- 4 Bytes
        dms_flexible_global.aec.aec_process_minfrequency.cfg,  -- 4 Bytes
        dms_flexible_global.aec.af_length.cfg,  -- 4 Bytes

        dms_flexible_global.nlp.nlp_process_maxfrequency.cfg,  -- 4 Bytes
        dms_flexible_global.nlp.nlp_process_minfrequency.cfg,  -- 4 Bytes
        dms_flexible_global.nlp.overdrive.cfg,  -- 4 Bytes

        dms_flexible_global.ans.aggress.cfg,  -- 4 Bytes
        dms_flexible_global.ans.suppress.cfg,  -- 4 Bytes
        dms_flexible_global.ans.init_noise_lvl.cfg, -- 4 Bytes

        dms_flexible_global.enc.enc_suppress_pre.cfg,  -- 4 Bytes
        dms_flexible_global.enc.enc_suppress_post.cfg,  -- 4 Bytes
        dms_flexible_global.enc.enc_minsuppress.cfg,  -- 4 Bytes
        dms_flexible_global.enc.enc_disconverge_erle_thr.cfg,  -- 4 Bytes
};

dms_flexible_global.dms_item_table_all = { dms_flexible_global.ver.cfg };
for _, v in ipairs(dms_flexible_global.dms_item_table) do
    table.insert(dms_flexible_global.dms_item_table_all, v)
end


dms_flexible_global.dms_output_type_group_view = {};
dms_flexible_global.dms_output_type_group_view.agc_type_group_view = cfg:stGroup(" AGC ",
    cfg:vBox {
        dms_flexible_global.agc.dms_ndt_fade_in.hbox_view,
        dms_flexible_global.agc.dms_ndt_fade_out.hbox_view,
        dms_flexible_global.agc.dms_dt_fade_in.hbox_view,
        dms_flexible_global.agc.dms_dt_fade_out.hbox_view,
        dms_flexible_global.agc.dms_ndt_max_gain.hbox_view,
        dms_flexible_global.agc.dms_ndt_min_gain.hbox_view,
        dms_flexible_global.agc.dms_ndt_speech_thr.hbox_view,
        dms_flexible_global.agc.dms_dt_max_gain.hbox_view,
        dms_flexible_global.agc.dms_dt_min_gain.hbox_view,
        dms_flexible_global.agc.dms_dt_speech_thr.hbox_view,
        dms_flexible_global.agc.dms_echo_present_thr.hbox_view,
    }
);

dms_flexible_global.dms_output_type_group_view.aec_type_group_view = cfg:stGroup(" AEC ",
    cfg:vaBox {
        dms_flexible_global.aec.aec_process_maxfrequency.hbox_view,
        dms_flexible_global.aec.aec_process_minfrequency.hbox_view,
        dms_flexible_global.aec.af_length.hbox_view,
    }
);

dms_flexible_global.dms_output_type_group_view.nlp_type_group_view = cfg:stGroup(" NLP ",
    cfg:vaBox {
        dms_flexible_global.nlp.nlp_process_maxfrequency.hbox_view,
        dms_flexible_global.nlp.nlp_process_minfrequency.hbox_view,
        dms_flexible_global.nlp.overdrive.hbox_view,
    }
);

dms_flexible_global.dms_output_type_group_view.ans_type_group_view = cfg:stGroup(" ANS ",
    cfg:vBox {
        dms_flexible_global.ans.aggress.hbox_view,
        dms_flexible_global.ans.suppress.hbox_view,
        dms_flexible_global.ans.init_noise_lvl.hbox_view,
    }
);

dms_flexible_global.dms_output_type_group_view.enc_type_group_view = cfg:stGroup(" ENC ",
    cfg:vBox {
        dms_flexible_global.enc.enc_suppress_pre.hbox_view,
        dms_flexible_global.enc.enc_suppress_post.hbox_view,
        dms_flexible_global.enc.enc_minsuppress.hbox_view,
        dms_flexible_global.enc.enc_disconverge_erle_thr.hbox_view,
    }
);

dms_flexible_global.dms_output_view_table = {
    dms_flexible_global.dms_reference_book_view,
    -- aec cfg
    dms_flexible_global.dms_mic_analog_gain.hbox_view,
    dms_flexible_global.dms_mic1_again.hbox_view,
    dms_flexible_global.dms_dac_analog_gain.hbox_view,
    dms_flexible_global.dms_aec_mode.hbox_view,
    dms_flexible_global.dms_ul_eq_en.hbox_view, 

    dms_flexible_global.dms_output_type_group_view.agc_type_group_view,
    dms_flexible_global.dms_output_type_group_view.aec_type_group_view,
    dms_flexible_global.dms_output_type_group_view.nlp_type_group_view,
    dms_flexible_global.dms_output_type_group_view.ans_type_group_view,
    dms_flexible_global.dms_output_type_group_view.enc_type_group_view,
};

aec_global.dms_flexible_output_view_table_group_view = cfg:vBox(dms_flexible_global.dms_output_view_table);

-- A. 输出htext
dms_flexible_global.dms_output_htext_table = {
};

-- B. 输出ctext：无

dms_flexible_global.layoutHook = function ()
    if (aec_cfg_type.cfg.val == aec_cfg_type.TY.dms_flexible) then
        return dms_flexible_global.dms_item_table_all;
    else
        return {}
    end
end;

dms_flexible_global.parserHook = function (i)
    return dms_flexible_global.dms_item_table;
end

-- C. 输出bin：无
dms_flexible_global.dms_output_bin = cfg:dynGroup("DMS_FLEXIBLE_OUTPUT_BIN",
    BIN_ONLY_CFG["BT_CFG"].dms_flexible.id,
    1,
    dms_flexible_global.layoutHook,
    dms_flexible_global.parserHook,
    dms_flexible_global.ver.cfg
);

dms_flexible_global.dms_output_bin:setRecoverHook(function ()
    local ov = dms_flexible_global.dms_aec_mode.cfg.val;
    cfg:set(dms_flexible_global.dms_aec_mode.aec_en, ov & 0x1);
    cfg:set(dms_flexible_global.dms_aec_mode.nlp_en, (ov >> 1) & 0x1);
    cfg:set(dms_flexible_global.dms_aec_mode.ans_en, (ov >> 2) & 0x1);
    cfg:set(dms_flexible_global.dms_aec_mode.enc_en, (ov >> 3) & 0x1);
    cfg:set(dms_flexible_global.dms_aec_mode.agc_en, (ov >> 4) & 0x1);
end);

---- [END]-----


aec_cfg_type.cfg_hook = function()
        if (aec_cfg_type.cfg.val == 0) then
            aec_global.aec_output_view_table_group_view:setHide(false);
            aec_global.dms_output_view_table_group_view:setHide(true);
            aec_global.aec_dns_output_view_table_group_view:setHide(true);
            aec_global.dms_dns_output_view_table_group_view:setHide(true);
            aec_global.dms_flexible_output_view_table_group_view:setHide(true);
        elseif (aec_cfg_type.cfg.val == 1) then
            aec_global.aec_output_view_table_group_view:setHide(true);
            aec_global.dms_output_view_table_group_view:setHide(false);
            aec_global.aec_dns_output_view_table_group_view:setHide(true);
            aec_global.dms_dns_output_view_table_group_view:setHide(true);
            aec_global.dms_flexible_output_view_table_group_view:setHide(true);
        elseif (aec_cfg_type.cfg.val == 2) then
            aec_global.aec_output_view_table_group_view:setHide(true);
            aec_global.dms_output_view_table_group_view:setHide(true);
            aec_global.aec_dns_output_view_table_group_view:setHide(false);
            aec_global.dms_dns_output_view_table_group_view:setHide(true);
            aec_global.dms_flexible_output_view_table_group_view:setHide(true);
        elseif (aec_cfg_type.cfg.val == 3) then
            aec_global.aec_output_view_table_group_view:setHide(true);
            aec_global.dms_output_view_table_group_view:setHide(true);
            aec_global.aec_dns_output_view_table_group_view:setHide(true);
            aec_global.dms_dns_output_view_table_group_view:setHide(false);
            aec_global.dms_flexible_output_view_table_group_view:setHide(true);
        elseif (aec_cfg_type.cfg.val == 4) then
            aec_global.aec_output_view_table_group_view:setHide(true);
            aec_global.dms_output_view_table_group_view:setHide(true);
            aec_global.aec_dns_output_view_table_group_view:setHide(true);
            aec_global.dms_dns_output_view_table_group_view:setHide(true);
            aec_global.dms_flexible_output_view_table_group_view:setHide(false);
        end
end;

aec_cfg_type.cfg_hook();

aec_cfg_type.cfg:setValChangeHook(aec_cfg_type.cfg_hook);



-- AEC & DMS 显示
local aec_group_view = cfg:stGroup(" 通话参数配置",
    cfg:vBox {
        aec_cfg_type.hbox_view,
        aec_global.aec_output_view_table_group_view,
        aec_global.aec_dns_output_view_table_group_view,
        aec_global.dms_output_view_table_group_view,
        aec_global.dms_dns_output_view_table_group_view,
        aec_global.dms_flexible_output_view_table_group_view,
    }
);

-- F. bindGroup
cfg:bindStGroup(aec_group_view, aec_output_bin);




--[[===================================================================================
=============================== 配置子项7: MIC类型配置 ================================
====================================================================================--]]
local mic_capless_sel_table = {
    [0] = " 单端隔直电容模式 ",
    [1] = " 差分隔直电容模式 ",
    [2] = " 单端省电容模式 ",
};

local MIC_CAPLESS_SEL_TABLE = cfg:enumMap("电容选择", mic_capless_sel_table);

local mic_bias_vol_sel_table = {
    [1] = " 10.0 K",
    [2] = " 9.4 K",
    [3] = " 8.9 K",
    [4] = " 8.3 K",
    [5] = " 7.7 K",
    [6] = " 7.2 K",
    [7] = " 6.6 K",
    [8] = " 6.0 K",
    [9] = " 5.3 K",
    [10] = " 4.8 K",
    [11] = " 4.3 K",
    [12] = " 3.9 K",
    [13] = " 3.6 K",
    [14] = " 3.2 K",
    [15] = " 2.7 K",
    [16] = " 2.2 K",
    [17] = " 1.8 K",
    [18] = " 1.4 K",
    [19] = " 1.2 K",
};

local MIC_BIAS_VOL_SEL_TABLE = cfg:enumMap("电容选择", mic_bias_vol_sel_table);

local mic_ldo_vol_sel_table = {
    [0] = " 1.3 V",
    [1] = " 1.4 V",
    [2] = " 1.5 V",
    [3] = " 2.0 V",
    [4] = " 2.2 V",
    [5] = " 2.4 V",
    [6] = " 2.6 V",
    [7] = " 2.8 V",
};

local MIC_LDO_VOL_SEL_TABLE = cfg:enumMap("电容选择", mic_ldo_vol_sel_table);

-- 0) MIC开头注释
-- item_htext
local mic_type_sel_comment_str = module_comment_context("MIC类型配置");

-- 1) 省电容选择
local mic_capless_sel = cfg:enum("MIC 电容方案选择：", MIC_CAPLESS_SEL_TABLE, 0);
depend_item_en_show(bluetooth_en, mic_capless_sel);
mic_capless_sel:setOSize(1);
-- item_htext
local mic_capless_sel_comment_str = item_comment_context("MIC电容方案选择", mic_capless_sel_table);
local mic_capless_sel_htext = item_output_htext(mic_capless_sel, "TCFG_MIC_CAPLESS", 5, nil, mic_capless_sel_comment_str, 1);
-- item_view
local mic_capless_sel_hbox_view = cfg:hBox {
	cfg:stLabel(mic_capless_sel.name),
    cfg:enumView(mic_capless_sel),
	cfg:stLabel(" (硅 MIC 要选不省电容模式)"),
    cfg:stSpacer();
};

-- 2) 省电容偏置电压
local mic_bias_vol_sel = cfg:enum("MIC 省电容方案偏置电压选择：", MIC_BIAS_VOL_SEL_TABLE, 17);
mic_bias_vol_sel:setOSize(1);
-- item_htext
local mic_bias_vol_sel_comment_str = item_comment_context("MIC省电容方案偏置电压选择", mic_bias_vol_sel_table);
local mic_bias_vol_sel_htext = item_output_htext(mic_bias_vol_sel, "TCFG_MIC_BIAS_RES", 5, nil, mic_bias_vol_sel_comment_str, 1);
--mic_bias_vol_sel:addDeps{bluetooth_en, mic_capless_sel};
--mic_bias_vol_sel:setDepChangeHook(function ()
--    mic_bias_vol_sel:setShow(((bluetooth_en.val == 1) and (mic_capless_sel.val == 2)) ~= false);
--end);
-- item_view
local mic_bias_vol_sel_hbox_view = cfg:hBox {
	cfg:stLabel(mic_bias_vol_sel.name),
    cfg:enumView(mic_bias_vol_sel),
    cfg:stSpacer();
};

-- 3) MIC ldo电压选择
local mic_ldo_vol_sel = cfg:enum("MIC LDO 电压选择：", MIC_LDO_VOL_SEL_TABLE, 3);
depend_item_en_show(bluetooth_en, mic_ldo_vol_sel);
mic_ldo_vol_sel:setOSize(1);
-- item_htext
local mic_ldo_vol_sel_comment_str = item_comment_context("MIC LDO电压选择", mic_ldo_vol_sel_table);
local mic_ldo_vol_sel_htext = item_output_htext(mic_ldo_vol_sel, "TCFG_MIC_LDO_VSEL", 5, nil, mic_ldo_vol_sel_comment_str, 2);
-- item_view
local mic_ldo_vol_sel_hbox_view = cfg:hBox {
	cfg:stLabel(mic_ldo_vol_sel.name),
    cfg:enumView(mic_ldo_vol_sel),
    cfg:stSpacer();
};

--========================= MIC 类型输出汇总  ============================
local mic_type_item_table = {
    mic_capless_sel,
    mic_bias_vol_sel,
    mic_ldo_vol_sel,
};

local mic_type_output_view_table = {
};

-- A. 输出htext
local mic_type_htext_output_table = {
    mic_type_sel_comment_str,
    mic_capless_sel_htext,
    mic_bias_vol_sel_htext,
    mic_ldo_vol_sel_htext,
};

-- B. 输出ctext：无

-- C. 输出bin
local mic_type_output_bin = cfg:group("MIC_TYPE_BIN",
	BIN_ONLY_CFG["HW_CFG"].mic_type.id,
	1,
    mic_type_item_table
);

-- D. 显示
local mic_type_group_view = cfg:stGroup("MIC 类型配置",
    cfg:vaBox {
        mic_capless_sel_hbox_view,
        mic_bias_vol_sel_hbox_view,
        mic_ldo_vol_sel_hbox_view,
    }
);

-- E. 默认值, 见汇总

-- F. bindGroup
cfg:bindStGroup(mic_type_group_view, mic_type_output_bin);

--[[===================================================================================
=============================== 配置子项8: 对耳配对码 =================================
====================================================================================--]]
local tws_pair_code = cfg:i32("对耳配对码(2字节)", 0xFFFF);
depend_item_en_show(bluetooth_en, tws_pair_code);
tws_pair_code:setOSize(2);
-- 约束
tws_pair_code:addConstraint(
    function ()
        local warn_str;
        if cfg.lang == "zh" then
            warn_str = "请输入 2 Bytes 对耳配对码";
        else
            warn_str = "Please input 2 Bytes Pair Code";
        end

        return (tws_pair_code.val >= 0 and tws_pair_code.val <= 0xFFFF)  or warn_str;
    end
);

-- A. 输出htext
local tws_pair_code_comment_str = "对耳配对码(2 Bytes)";
local tws_pair_code_htext = item_output_htext_hex(tws_pair_code, "TCFG_TWS_PAIR_CODE", 5, nil, tws_pair_code_comment_str, 2);

-- B. 输出ctext：无

-- C. 输出bin：无
local tws_pair_code_output_bin = cfg:group("TWS_PAIR_CODE_CFG",
	BIN_ONLY_CFG["BT_CFG"].tws_pair_code.id,
	1,
	{
	    tws_pair_code,
	}
);

-- D. 显示
local tws_pair_code_group_view = cfg:stGroup("",
    cfg:hBox {
        cfg:stLabel("对耳配对码配置：0x"),
        cfg:hexInputView(tws_pair_code),
        cfg:stLabel("(2字节)"),
        cfg:stSpacer(),
    }
);

-- E. 默认值, 见汇总

-- F. bindGroup：无
cfg:bindStGroup(tws_pair_code_group_view, tws_pair_code_output_bin);


--[[===================================================================================
=============================== 配置子项8: 按键开机配置 ===============================
====================================================================================--]]
local need_key_on_table = {
    [0] = " 不需要按按键开机",
    [1] = " 需要按按键开机",
};

local NEED_KEY_ON_TABLE = cfg:enumMap("是否需要按键开机列表", need_key_on_table);

local power_on_need_key_on = cfg:enum("是否需要按键开机配置（只输出宏）：", NEED_KEY_ON_TABLE, 1);
depend_item_en_show(bluetooth_en, power_on_need_key_on);
-- A. 输出htext
local power_on_need_key_on_comment_str = item_comment_context("是否需要按键开机配置", need_key_on_table);
local power_on_need_key_on_htext = item_output_htext(power_on_need_key_on, "TCFG_POWER_ON_KEY_ENABLE", 3, nil, power_on_need_key_on_comment_str, 2);

-- B. 输出ctext：无
-- C. 输出bin：无

-- D. 显示
local power_on_need_key_on_group_view = cfg:stGroup("",
    cfg:hBox {
        cfg:stLabel(power_on_need_key_on.name),
        cfg:enumView(power_on_need_key_on),
        cfg:stSpacer(),
    }
);

-- E. 默认值, 见汇总

-- F. bindGroup：无

--[[===================================================================================
=============================== 配置子项9: 蓝牙特性配置 ===============================
====================================================================================--]]
--蓝牙类配置, 1t2和tws是互斥的
local blue_1t2_en = cfg:i32("一拖二使能开关: ", 1);
depend_item_en_show(bluetooth_en, blue_1t2_en);
-- item_text
local blue_1t2_en_comment_str = item_comment_context("一拖二使能开关", FUNCTION_SWITCH_TABLE) .. "与TWS功能 TCFG_TWS_ENABLE 互斥";
local blue_1t2_en_htext = item_output_htext(blue_1t2_en, "TCFG_BD_NUM_ENABLE", 5, FUNCTION_SWITCH_TABLE, blue_1t2_en_comment_str, 1);
-- item_view
local blue_1t2_en_hbox_view = cfg:hBox {
    cfg:stLabel(blue_1t2_en.name),
    cfg:checkBoxView(blue_1t2_en),
    cfg:stSpacer(),
};

local blue_1t2_stop_page_scan_time = cfg:i32("自动取消可连接时间: ", 20);
-- item_text
local blue_1t2_stop_page_scan_time_comment_str = "自动取消可连接时间, 单位：秒";
local blue_1t2_stop_page_scan_time_htext = item_output_htext(blue_1t2_stop_page_scan_time, "TCFG_AUTO_STOP_PAGE_SCAN_TIME", 2, nil, blue_1t2_stop_page_scan_time_comment_str, 1);
blue_1t2_stop_page_scan_time:addDeps{bluetooth_en, blue_1t2_en};
blue_1t2_stop_page_scan_time:setDepChangeHook(function ()
    blue_1t2_stop_page_scan_time:setShow(((bluetooth_en.val == 1) and (blue_1t2_en.val == 1)) ~= false);
end);

local blue_1t2_stop_page_scan_time_hbox_view = cfg:hBox {
    cfg:stLabel(blue_1t2_stop_page_scan_time.name),
    cfg:ispinView(blue_1t2_stop_page_scan_time, 0, 99999, 1),
    cfg:stLabel("单位: 秒"),
    cfg:stSpacer(),
};

-- item_view
local blue_1t2_group_view = cfg:stGroup("一拖二参数配置",
    cfg:vBox {
        blue_1t2_en_hbox_view,
        blue_1t2_stop_page_scan_time_hbox_view,
    }
);

--蓝牙类配置, 1t2和tws是互斥的
local blue_tws_en = cfg:i32("对耳使能开关: ", 0);
depend_item_en_show(bluetooth_en, blue_tws_en);
-- item_text
local blue_tws_en_comment_str = item_comment_context("对耳使能开关", FUNCTION_SWITCH_TABLE) .. "与一拖二功能 TCFG_BD_NUM_ENABLE 互斥";
local blue_tws_en_htext = item_output_htext(blue_tws_en, "TCFG_TWS_ENABLE", 6, FUNCTION_SWITCH_TABLE, blue_tws_en_comment_str, 1);
-- item_view
local blue_tws_en_hbox_view = cfg:hBox {
    cfg:stLabel(blue_tws_en.name),
    cfg:checkBoxView(blue_tws_en),
    cfg:stSpacer(),
};


local blue_tws_pair_timeout = cfg:i32("对耳配对超时: ", 30);
local blue_tws_pair_timeout_conmment_str = "对耳配对超时, 单位: 秒";
-- item_text
local blue_tws_pair_timeout_htext = item_output_htext(blue_tws_pair_timeout, "TCFG_TWS_PAIR_TIMEOUT", 4, nil, blue_tws_pair_timeout_conmment_str, 1);
blue_tws_pair_timeout:addDeps{bluetooth_en, blue_tws_en};
blue_tws_pair_timeout:setDepChangeHook(function ()
    blue_tws_pair_timeout:setShow(((bluetooth_en.val == 1) and (blue_tws_en.val == 1)) ~= false);
end);
local blue_tws_pair_timeout_hbox_view = cfg:hBox{
    cfg:stLabel(blue_tws_pair_timeout.name),
    cfg:ispinView(blue_tws_pair_timeout, 0, 99999, 1),
    cfg:stLabel("单位: 秒"),
    cfg:stSpacer(),
};

-- item_view
local blue_tws_group_view = cfg:stGroup("对耳参数配置",
    cfg:vBox {
        blue_tws_en_hbox_view,
        blue_tws_pair_timeout_hbox_view,
    }
);

cfg:addGlobalConstraint(function()
    return not(blue_tws_en.val == 1 and blue_1t2_en.val == 1) or "对耳和一拖二不能同时使能";
end);

--蓝牙类配置: BLE使能
local blue_ble_en = cfg:i32("BLE功能使能: ", 0);
depend_item_en_show(bluetooth_en, blue_ble_en);
-- item_text
local blue_ble_en_comment_str = item_comment_context("BLE功能使能", FUNCTION_SWITCH_TABLE);
local blue_ble_en_htext = item_output_htext(blue_ble_en, "TCFG_BLE_ENABLE", 6, FUNCTION_SWITCH_TABLE, blue_ble_en_comment_str, 1);
-- item_view
local blue_ble_en_group_view = cfg:stGroup("",
    cfg:hBox {
        cfg:stLabel(blue_ble_en.name),
        cfg:checkBoxView(blue_ble_en),
        cfg:stSpacer(),
    }
);

--蓝牙类配置: 来电报号使能, 来电报号和播手机自己提示音互斥
local phone_in_play_num_en = cfg:i32("来电报号使能: ", 1);
depend_item_en_show(bluetooth_en, phone_in_play_num_en);
-- item_htext
local phone_in_play_num_en_comment_str = item_comment_context("来电报号使能", FUNCTION_SWITCH_TABLE);
local phone_in_play_num_en_htext = item_output_htext(phone_in_play_num_en, "TCFG_BT_PHONE_NUMBER_ENABLE", 3, FUNCTION_SWITCH_TABLE, phone_in_play_num_en_comment_str, 1);
-- item_view
local phone_in_play_num_en_hbox_view = cfg:hBox {
    cfg:stLabel(phone_in_play_num_en.name),
    cfg:checkBoxView(phone_in_play_num_en),
    cfg:stSpacer(),
};

--蓝牙类配置: 播放手机自带来电提示音使能, 来电报号和播手机自己提示音互斥
local phone_in_play_ring_tone_en = cfg:i32("播放手机自带来电提示音使能: ", 0);
depend_item_en_show(bluetooth_en, phone_in_play_ring_tone_en);
-- item_htext
local phone_in_play_ring_tone_en_comment_str = item_comment_context("播放手机自带来电提示音使能", FUNCTION_SWITCH_TABLE);
local phone_in_play_ring_tone_en_htext = item_output_htext(phone_in_play_ring_tone_en, "TCFG_BT_INBAND_RINGTONE_ENABLE", 2, FUNCTION_SWITCH_TABLE, phone_in_play_ring_tone_en_comment_str, 1);
local phone_in_play_ring_tone_en_hbox_view = cfg:hBox {
    cfg:stLabel(phone_in_play_ring_tone_en.name),
    cfg:checkBoxView(phone_in_play_ring_tone_en),
    cfg:stSpacer(),
};

-- item_view
local phone_in_play_group_view = cfg:stGroup("",
    cfg:vBox {
        phone_in_play_num_en_hbox_view,
        phone_in_play_ring_tone_en_hbox_view,
    }
);

-- 约束
cfg:addGlobalConstraint(function()
    return not(phone_in_play_num_en.val == 1 and phone_in_play_ring_tone_en.val == 1) or "来电报号和播手机自带提示音不能同时使能";
end);

local sys_auto_shut_down_time = cfg:i32("没有连接自动关机时间配置: ", 3);
sys_auto_shut_down_time:setOSize(1);
depend_item_en_show(bluetooth_en, sys_auto_shut_down_time);
-- item_htext
local sys_auto_shut_down_time_comment_str = "没有连接自动关机时间配置, 单位：分钟";
local sys_auto_shut_down_time_htext = item_output_htext(sys_auto_shut_down_time, "TCFG_SYS_AUTO_SHUT_DOWN_TIME", 2, nil, sys_auto_shut_down_time_comment_str, 1);
-- 输出bin
local sys_auto_shut_down_time_output_bin = cfg:group("auto_shut_down",
	BIN_ONLY_CFG["BT_CFG"].auto_shut_down_time.id,
	1,
	{
		sys_auto_shut_down_time,
	}
);

-- item_view
local sys_auto_shut_time_group_view = cfg:stGroup("",
    cfg:hBox {
        cfg:stLabel(sys_auto_shut_down_time.name),
        cfg:ispinView(sys_auto_shut_down_time, 0, 99999, 1),
        cfg:stLabel("单位: 分钟(为0不打开)"),
        cfg:stSpacer(),
    }
);

-- F. bindGroup：
cfg:bindStGroup(sys_auto_shut_time_group_view, sys_auto_shut_down_time_output_bin);
--========================= 蓝牙 Feature 输出汇总  ============================
local bt_feature_item_table = {
    blue_1t2_en,
    blue_1t2_stop_page_scan_time,
    blue_tws_en,
    blue_tws_pair_timeout,
    blue_ble_en,
    phone_in_play_num_en,
    phone_in_play_ring_tone_en,
    sys_auto_shut_down_time,
};

-- A. 输出htext
local bt_feature_output_htext_table = {
    blue_1t2_en_htext,
    blue_1t2_stop_page_scan_time_htext,
    blue_tws_en_htext,
    blue_tws_pair_timeout_htext,
    blue_ble_en_htext,
    phone_in_play_num_en_htext,
    phone_in_play_ring_tone_en_htext,
    sys_auto_shut_down_time_htext,
};
-- B. 输出ctext：无
-- C. 输出bin：无

-- D. 显示
local bt_feature_group_view = cfg:stGroup("",
    cfg:vBox {
        blue_1t2_group_view,
        blue_tws_group_view,
        blue_ble_en_group_view,
        phone_in_play_group_view,
        --sys_auto_shut_time_group_view,
    }
);

-- E. 默认值, 见汇总

-- F. bindGroup：无

--[[===================================================================================
=============================== 配置子项10: LRC参数配置 ================================
====================================================================================--]]
local lrc_change_mode_table = {
    [0] = " disable ",
    [1] = " enable ",
};

local LRC_CHANGE_MODE_TABLE = cfg:enumMap("LRC切换使能", lrc_change_mode_table);

-- item_htext
local lrc_start_comment_str = module_comment_context("LRC参数配置");

-- 1) lrc_ws_inc
local lrc_ws_inc = cfg:i32("lrc_ws_inc", 400);
depend_item_en_show(bluetooth_en, lrc_ws_inc);
lrc_ws_inc:setOSize(2);
-- item_htext
local lrc_ws_inc_comment_str = "设置范围：0 ~ 1000, 默认值：400";
local lrc_ws_inc_htext = item_output_htext(lrc_ws_inc, "TCFG_LRC_WS_INC", 4, nil, lrc_ws_inc_comment_str, 1);
-- item_view
local lrc_ws_inc_hbox_view = cfg:hBox {
    cfg:stLabel(lrc_ws_inc.name .. "：" .. TAB_TABLE[1]),
    cfg:ispinView(lrc_ws_inc, 0, 1000, 1),
    cfg:stLabel("（lrc窗口步进值，单位：us，设置范围: 0 ~ 1000, 默认值400）");
    cfg:stSpacer(),
};

-- 2) lrc_ws_init
local lrc_ws_init = cfg:i32("lrc_ws_init", 400);
depend_item_en_show(bluetooth_en, lrc_ws_init);
lrc_ws_init:setOSize(2);
-- item_htext
local lrc_ws_init_comment_str = "设置范围：0 ~ 1000, 默认值：400";
local lrc_ws_init_htext = item_output_htext(lrc_ws_init, "TCFG_LRC_WS_INIT", 4, nil, lrc_ws_init_comment_str, 1);
-- item_view
local lrc_ws_init_hbox_view = cfg:hBox {
    cfg:stLabel(lrc_ws_init.name .. "：" .. TAB_TABLE[1]),
    cfg:ispinView(lrc_ws_init, 0, 1000, 1),
    cfg:stLabel("（lrc窗口初始值，单位：us，设置范围: 0 ~ 1000, 默认值400）");
    cfg:stSpacer(),
};

-- 3) btosc_ws_inc
local btosc_ws_inc = cfg:i32("btosc_ws_inc", 400);
depend_item_en_show(bluetooth_en, btosc_ws_inc);
btosc_ws_inc:setOSize(2);
-- item_htext
local btosc_ws_inc_comment_str = "设置范围：0 ~ 1000, 默认值：400";
local btosc_ws_inc_htext = item_output_htext(btosc_ws_inc, "TCFG_BTOSC_WS_INC", 4, nil, btosc_ws_inc_comment_str, 1);
-- item_view
local btosc_ws_inc_hbox_view = cfg:hBox {
    cfg:stLabel(btosc_ws_inc.name .. "：" .. TAB_TABLE[1]),
    cfg:ispinView(btosc_ws_inc, 0, 1000, 1),
    cfg:stLabel("（btosc窗口步进值，单位：us，设置范围: 0 ~ 1000, 默认值400）");
    cfg:stSpacer(),
};

-- 4) btosc_ws_init
local btosc_ws_init = cfg:i32("btosc_ws_init", 400);
depend_item_en_show(bluetooth_en, btosc_ws_init);
btosc_ws_init:setOSize(2);
-- item_htext
local btosc_ws_init_comment_str = "设置范围：0 ~ 1000, 默认值：400";
local btosc_ws_init_htext = item_output_htext(btosc_ws_init, "TCFG_BTOSC_WS_INIT", 4, nil, btosc_ws_init_comment_str, 1);
-- item_view
local btosc_ws_init_hbox_view = cfg:hBox {
    cfg:stLabel(btosc_ws_init.name .. "：" .. TAB_TABLE[1]),
    cfg:ispinView(btosc_ws_init, 0, 1000, 1),
    cfg:stLabel("（btosc窗口初始值，单位：us，设置范围: 0 ~ 1000, 默认值400）");
    cfg:stSpacer(),
};

-- 5) lrc切换使能
local lrc_change_mode = cfg:enum("lrc_change_mode", LRC_CHANGE_MODE_TABLE, 1);
depend_item_en_show(bluetooth_en, lrc_change_mode);
lrc_change_mode:setOSize(1);
-- item_htext
local lrc_change_mode_comment_str = item_comment_context("LRC切换使能", lrc_change_mode_table);
local lrc_change_mode_htext = item_output_htext(lrc_change_mode, "TCFG_LRC_CHANGE_MODE", 4, nil, lrc_change_mode_comment_str, 1);
-- item_view
local lrc_change_mode_hbox_view = cfg:hBox {
	cfg:stLabel(lrc_change_mode.name .. "："),
    cfg:enumView(lrc_change_mode),
	cfg:stLabel(" （lrc切换使能，默认值enable）"),
    cfg:stSpacer();
};

--========================= lrc cfg 参数输出汇总  ============================
local lrc_cfg_item_table = {
    lrc_ws_inc,
    lrc_ws_init,
    btosc_ws_inc,
    btosc_ws_init,
    lrc_change_mode,
};

-- A. 输出htext
local lrc_htext_output_table = {
    lrc_start_comment_str,
    lrc_ws_inc_htext,
    lrc_ws_init_htext,
    btosc_ws_inc_htext,
    btosc_ws_init_htext,
    lrc_change_mode_htext,
};

-- B. 输出ctext：无

-- C. 输出bin
local lrc_cfg_output_bin = cfg:group("LRC_CFG_BIN",
	BIN_ONLY_CFG["BT_CFG"].lrc_cfg.id,
	1,
    lrc_cfg_item_table
);

-- D. 显示
local lrc_cfg_group_view = cfg:stGroup("lrc 参数配置",
    cfg:vaBox {
        lrc_ws_inc_hbox_view,
        lrc_ws_init_hbox_view,
        btosc_ws_inc_hbox_view,
        btosc_ws_init_hbox_view,
        lrc_change_mode_hbox_view,
    }
);

-- E. 默认值, 见汇总

-- F. bindGroup
cfg:bindStGroup(lrc_cfg_group_view, lrc_cfg_output_bin);

--[[===================================================================================
=============================== 配置子项11: BLE参数配置 ================================
====================================================================================--]]
local function ble_cfg_depend_show(ble_cfg, dep_cfg)
    ble_cfg:addDeps{bluetooth_en, dep_cfg};
    ble_cfg:setDepChangeHook(function()
        ble_cfg:setShow((bluetooth_en.val ~= 0) and (dep_cfg.val ~= 0));
        end
    );
end
-- 1) ble_en
local ble_en = cfg:i32("BLE 配置使能", 1);
ble_en:setOSize(1);
depend_item_en_show(bluetooth_en, ble_en);
-- A. 输出htext: 无
-- B. 输出ctext：无
-- C. 输出bin:见汇总
-- D. 显示
local ble_en_hbox_view = cfg:hBox {
		cfg:stLabel(ble_en.name .. "："),
		cfg:checkBoxView(ble_en),
        cfg:stLabel("设置为不使能BLE配置将跟随EDR配置");
        cfg:stSpacer();
};
-- E. 默认值: 见汇总
-- F. bindGroup：见汇总

-- 2) ble_name
local ble_name = cfg:str("BLE 蓝牙名字", "AC701N-BLE");
ble_name:setOSize(32);
ble_cfg_depend_show(ble_name, ble_en);
-- A. 输出htext: 无
-- B. 输出ctext：无
-- C. 输出bin:见汇总
-- D. 显示
local ble_name_hbox_view = cfg:hBox {
		cfg:stLabel(ble_name.name .. "："),
		cfg:inputMaxLenView(ble_name, 32),
        cfg:stSpacer();
};
-- E. 默认值: 见汇总
-- F. bindGroup：见汇总

-- 3) ble_rf_power
local ble_rf_power = cfg:i32("BLE 蓝牙发射功率", 5);
ble_rf_power:setOSize(1);
ble_cfg_depend_show(ble_rf_power, ble_en);
-- A. 输出htext: 无
-- B. 输出ctext：无
-- C. 输出bin:见汇总
-- D. 显示
local ble_rf_power_hbox_view = cfg:hBox {
		cfg:stLabel(ble_rf_power.name .. "："),
        cfg:ispinView(ble_rf_power, 0, 10, 1),
        cfg:stLabel("(设置范围：0 ~ 10)");
        cfg:stSpacer();
};
-- E. 默认值: 见汇总
-- F. bindGroup：见汇总

-- 4) ble_addr_en
local ble_addr_en = cfg:i32("BLE MAC 地址配置使能", 1);
ble_addr_en:setOSize(1);
ble_cfg_depend_show(ble_addr_en, ble_en);
-- A. 输出htext: 无
-- B. 输出ctext：无
-- C. 输出bin:见汇总
-- D. 显示
local ble_addr_en_hbox_view = cfg:hBox {
		cfg:stLabel(ble_addr_en.name .. "："),
		cfg:checkBoxView(ble_addr_en),
        cfg:stLabel("设置为不使能BLE配置将随机生成或者烧写时指定");
        cfg:stSpacer();
};
-- E. 默认值: 见汇总
-- F. bindGroup：见汇总

-- 5) ble_mac_addr
local ble_mac_addr = cfg:mac("BLE 蓝牙MAC地址", {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF});
ble_mac_addr:addDeps{bluetooth_en, ble_en, ble_addr_en};
ble_mac_addr:setDepChangeHook(function()
    ble_mac_addr:setShow((ble_en.val ~= 0) and (ble_addr_en.val ~= 0) and (bluetooth_en.val ~= 0));
    end
);
-- A. 输出htext: 无
-- B. 输出ctext：无
-- C. 输出bin:见汇总
-- D. 显示
local ble_mac_addr_hbox_view = cfg:hBox {
		cfg:stLabel(ble_mac_addr.name .. "："),
		cfg:macAddrView(ble_mac_addr),
        cfg:stSpacer();
};

-- addr 汇总显示
local ble_mac_addr_group_view = cfg:stGroup("",
    cfg:vBox {
        ble_addr_en_hbox_view,
        ble_mac_addr_hbox_view,
    }
);
-- E. 默认值: 见汇总
-- F. bindGroup：见汇总

--============================== ble cfg 参数输出汇总  ================================
local ble_cfg_item_table = {
    ble_en,
    ble_name,
    ble_rf_power,
    ble_addr_en,
    ble_mac_addr,
};

-- A. 输出htext
local ble_htext_output_table = {};

-- B. 输出ctext：无

-- C. 输出bin
local ble_cfg_output_bin = cfg:group("BLE_CFG_BIN",
	BIN_ONLY_CFG["BT_CFG"].ble_cfg.id,
	1,
    ble_cfg_item_table
);

-- D. 显示
local ble_cfg_group_view = cfg:stGroup("BLE 参数配置",
    cfg:vBox {
        ble_en_hbox_view,
        ble_name_hbox_view,
        ble_rf_power_hbox_view,
        ble_mac_addr_group_view,
    }
);

-- E. 默认值, 见汇总

-- F. bindGroup
cfg:bindStGroup(ble_cfg_group_view, ble_cfg_output_bin);

--[[===================================================================================
==================================== 模块返回汇总 =====================================
====================================================================================--]]
-- A. 输出htext
--[[
insert_item_to_list(bt_output_htext_tabs, bt_comment_begin_htext);
insert_item_to_list(bt_output_htext_tabs, bt_en_htext);
insert_item_to_list(bt_output_htext_tabs, bt_name_htext);
insert_item_to_list(bt_output_htext_tabs, bt_mac_htext);
insert_item_to_list(bt_output_htext_tabs, bt_rf_power_htext);
-- AEC
insert_list_to_list(bt_output_htext_tabs, aec_output_htext_table);
-- MIC
insert_list_to_list(bt_output_htext_tabs, mic_type_htext_output_table);
insert_item_to_list(bt_output_htext_tabs, tws_pair_code_htext);
-- key_on
insert_item_to_list(bt_output_htext_tabs, power_on_need_key_on_htext);
-- Feature
insert_list_to_list(bt_output_htext_tabs, bt_feature_output_htext_table);
--]]
-- B. 输出ctext：无
-- C. 输出bin：无
insert_item_to_list(bt_output_bin_tabs, bluenames_output_bin);
insert_item_to_list(bt_output_bin_tabs, bt_mac_output_bin);
insert_item_to_list(bt_output_bin_tabs, bt_rf_power_output_bin);
insert_item_to_list(bt_output_bin_tabs, aec_output_bin);
insert_item_to_list(bt_output_bin_tabs, dms_output_bin);
insert_item_to_list(bt_output_bin_tabs, aec_dns_global.aec_output_bin);
insert_item_to_list(bt_output_bin_tabs, dms_dns_global.dms_output_bin);
insert_item_to_list(bt_output_bin_tabs, dms_flexible_global.dms_output_bin);
insert_item_to_list(bt_output_bin_tabs, mic_type_output_bin);
insert_item_to_list(bt_output_bin_tabs, tws_pair_code_output_bin);
insert_item_to_list(bt_output_bin_tabs, sys_auto_shut_down_time_output_bin);
insert_item_to_list(bt_output_bin_tabs, lrc_cfg_output_bin);
if enable_moudles["ble_config"] == true then
insert_item_to_list(bt_output_bin_tabs, ble_cfg_output_bin);
end

-- E. 默认值
local bt_cfg_default_table = {};
for k, v in pairs(bluenames_bin_cfgs) do
	insert_item_to_list(bt_cfg_default_table, v);
end

if open_by_program == "create" then
	--insert_item_to_list(bt_cfg_default_table, bluetooth_en);
    insert_item_to_list(bt_cfg_default_table, power_on_need_key_on);
    insert_list_to_list(bt_cfg_default_table, bt_feature_item_table);
end

insert_item_to_list(bt_cfg_default_table, bluetooth_mac);
insert_item_to_list(bt_cfg_default_table, bluetooth_rf_power);
-- aec
insert_list_to_list(bt_cfg_default_table, aec_global.aec_item_table);
insert_list_to_list(bt_cfg_default_table, dms_global.dms_item_table);
insert_list_to_list(bt_cfg_default_table, dms_global.dms_aec_mode_bit_en_table);
--ans config
-- mic
insert_list_to_list(bt_cfg_default_table, mic_type_item_table);

insert_item_to_list(bt_cfg_default_table, tws_pair_code);
-- lrc_cfg
insert_list_to_list(bt_cfg_default_table, lrc_cfg_item_table);

-- ble_cfg
insert_list_to_list(bt_cfg_default_table, ble_cfg_item_table);

local bt_default_button_view = cfg:stButton(" 蓝牙配置恢复默认值 ", reset_to_default(bt_cfg_default_table));

-- D. 显示
local bt_base_group_view = cfg:stGroup("基本配置",
    cfg:vBox {
        bluenames_group_view,
        bt_mac_group_view,
        bt_rf_power_group_view,
        sys_auto_shut_time_group_view,
    }
);

local bt_output_group_view_table = {};

if open_by_program == "create" then
    insert_item_to_list(bt_output_group_view_table, bt_module_en_hbox_view);
end

insert_item_to_list(bt_output_group_view_table, bt_base_group_view);
if enable_moudles["ble_config"] == true then
    insert_item_to_list(bt_output_group_view_table, ble_cfg_group_view);
end
insert_item_to_list(bt_output_group_view_table, tws_pair_code_group_view);
insert_item_to_list(bt_output_group_view_table, aec_group_view);
insert_item_to_list(bt_output_group_view_table, mic_type_group_view);
insert_item_to_list(bt_output_group_view_table, lrc_cfg_group_view);

if open_by_program == "create" then
    --insert_item_to_list(bt_output_group_view_table, power_on_need_key_on_group_view);
    --insert_item_to_list(bt_output_group_view_table, bt_feature_group_view);
end

bt_output_vbox_view = cfg:vBox {
    cfg:stHScroll(cfg:vBox(bt_output_group_view_table)),
    bt_default_button_view,
};

-- F. bindGroup：item单独处理

end

