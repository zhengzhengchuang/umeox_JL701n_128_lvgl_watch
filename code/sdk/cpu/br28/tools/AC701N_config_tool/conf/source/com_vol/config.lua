
com_vol_config = {}

-- 数据表
com_vol_config.table = {
	{avol =  3, rdb = 0.000},
	{avol =  2, rdb = -6.000},
	{avol =  1, rdb = -12.000},
	{avol =  0, rdb = -18.000},
};

com_vol_config.max_config_rdb = com_vol_config.table[1].rdb;
com_vol_config.min_config_rdb = -100; -- com_vol_config.table[#com_vol_config.table].rdb;
com_vol_config.max_dvol = 16384; -- 最大数字音量
com_vol_config.call_vol_levels = 15;