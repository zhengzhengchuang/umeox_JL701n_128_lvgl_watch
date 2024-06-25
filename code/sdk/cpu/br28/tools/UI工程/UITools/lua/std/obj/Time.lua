--[[
-- 时间控件方法
@apiDefine TimeGroup 时间控件
--]]
Time = {}


Time.year   = 0
Time.month  = 0
Time.day    = 0
Time.hour   = 0
Time.minute = 0
Time.second = 0
Time.timer_run = false


-- 拆分整数的每一位
local function get_split_inter(num)
    if not num then
        return nil
    end

    local tb = {}   -- 存放拆分的数字
    repeat
        table.insert(tb, num % 10)
        num = math.floor(num / 10)
    until(num == 0)

    -- 把table倒序
    local tmp = {}
    for i = 1, #tb do
        local key = #tb
        tmp[i] = table.remove(tb)
    end

    return tmp
end



--[[
-- 时间控件回调
-- 用于创建定时器，当时间控件选择自动计时时会用于自动走时
--]]
local function timeCountUpCallback(self_tab)
    log:updateTime(self_tab, false)
	-- 秒计数
    --[[
	-- self_tab.second = self_tab.second + 1
	if (self_tab.second >= 60) then
		self_tab.second = 0
		self_tab.minute = self_tab.minute + 1
	end
	-- 分钟
	if (self_tab.minute >= 60) then
		self_tab.minute = 0
		self_tab.hour = self_tab.hour + 1
	end
	-- 小时
	if (self_tab.hour >= 24) then
		self_tab.hour = 0
		self_tab.day = self_tab.day + 1
	end
	-- 日期
	if ((self_tab.month == 1) or
		(self_tab.month == 3) or
		(self_tab.month == 5) or
		(self_tab.month == 7) or
		(self_tab.month == 8) or
		(self_tab.month == 10) or
		(self_tab.month == 12)) then
		-- 大月31天
		if (self_tab.day >= 31) then
			self_tab.day  =0
			self_tab.month = self_tab.month + 1
		end
	elseif ((self_tab.month == 4) or
			(self_tab.month == 6) or
			(self_tab.month == 9) or
			(self_tab.month == 11)) then
		-- 小月30天
		if (self_tab.day >= 30) then
			self_tab.day  =0
			self_tab.month = self_tab.month + 1
		end
	else
		-- 二月
		if (((not (self_tab.year % 4)) and (self_tab.year % 100)) or 
			(not (self_tab.year % 400))) then
			-- 闰年29天
			if (self_tab.day >= 29) then
				self_tab.day  =0
				self_tab.month = self_tab.month + 1
			end
		else
			-- 平年28天
			if (self_tab.day >= 28) then
				self_tab.day  =0
				self_tab.month = self_tab.month + 1
			end
		end
	end

	if (self_tab.month >= 12) then
		self_tab.year = self_tab.year + 1
	end
    ]]

    local time_form = self_tab.obj:getAttrTextByIndex(1)
    if (self_tab.show_image) then
        local image_list = {}

        for i=1, string.len(time_form) do
            local c = string.sub(time_form, i, i)
            if (c == "Y") then
                local tab = get_split_inter(self_tab.year)
                for k, v in ipairs(tab) do
                    local tb = {}
                    tb.pic = 0
                    tb.idx = v
                    table.insert(image_list, tb)
                end
            elseif (c == "M") then
                local tab = get_split_inter(self_tab.month)
                for k, v in ipairs(tab) do
                    local tb = {}
                    tb.pic = 0
                    tb.idx = v
                    table.insert(image_list, tb)
                end
            elseif (c == "D") then
                local tab = get_split_inter(self_tab.day)
                for k, v in ipairs(tab) do
                    local tb = {}
                    tb.pic = 0
                    tb.idx = v
                    table.insert(image_list, tb)
                end
            elseif (c == "h") then
                local tab = get_split_inter(self_tab.hour)
                for k, v in ipairs(tab) do
                    local tb = {}
                    tb.pic = 0
                    tb.idx = v
                    table.insert(image_list, tb)
                end
            elseif (c == "m") then
                local tab = get_split_inter(self_tab.minute)
                for k, v in ipairs(tab) do
                    local tb = {}
                    tb.pic = 0
                    tb.idx = v
                    table.insert(image_list, tb)
                end
            elseif (c == "s") then
                local tab = get_split_inter(self_tab.second)
                for k, v in ipairs(tab) do
                    local tb = {}
                    tb.pic = 0
                    tb.idx = v
                    table.insert(image_list, tb)
                end
            elseif (c == "/") then
                local tb = {}
                tb.pic = 1
                tb.idx = 0
                table.insert(image_list, tb)
            elseif (c == ":") then
                local tb = {}
                tb.pic = 1
                tb.idx = 1
                table.insert(image_list, tb)
            end
        end
        self_tab.obj:setTextByImageList(image_list)
    else
        -- 格式
        local form = self_tab.obj:getAttrTextByIndex(1)
        local dat_time = ""
        for i=1, string.len(form) do
            local c = string.sub(form, i, i)
            if (c == "Y") then
                dat_time = dat_time .. string.format('%04d', self_tab.year)
            elseif (c == "M") then
                dat_time = dat_time .. string.format('%02d', self_tab.month)
            elseif (c == "D") then
                dat_time = dat_time .. string.format('%02d', self_tab.day)
            elseif (c == "h") then
                dat_time = dat_time .. string.format('%02d', self_tab.hour)
            elseif (c == "m") then
                dat_time = dat_time .. string.format('%02d', self_tab.minute)
            elseif (c == "s") then
                dat_time = dat_time .. string.format('%02d', self_tab.second)
            else
                dat_time = dat_time .. tostring(c)
            end
        end
        -- 显示ascii字符串
        self_tab.obj:showCaption(dat_time)
    end 
end



--[[
@api {SET} obj:setUIData(year,month,day,redraw) 设置UI日期
@apiName setUIdata()
@apiGroup TimeGroup
@apiVersion 1.0.0
@apiDescription 设置时间控件显示日期

@apiParam {number} year 设置的年份
@apiParam {number} month 设置的月份
@apiParam {number} day 设置的日份
@apiParam {boolean{true, false}} [redraw] 是否立即刷新，缺省时立即刷新

@apiParamExample 示例:
tim = gui:getComponentByName("TIME_TEST")
tim:setUIData(2021, 12, 31, true)   -- 设置显示日期为 2021-12-31

@apiSuccessExample {number} 成功响应: 
时间控件显示设置的时间

@apiErrorExample {number} 失败响应:
时间控件未显示设置的时间
]]
function Time:setUIData(year, month, day, redraw)
    -- 参数类型检查
    if (not log:checkArgType("setUIData", 1, year, "number")) then
        return
    end
    if (not log:checkArgType("setUIData", 2, month, "number")) then
        return
    end
     if (not log:checkArgType("setUIData", 3, day, "number")) then
        return
    end
     if (not log:checkArgType("setUIData", 4, redraw, "nil", "boolean")) then
        return
    end

    -- 月份限制，最小1月，最大12月
    local month_range = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}
    if (not log:checkValue(month, month_range)) then
        return
    end

    -- 天数限制，最小1日，最大31日
    local day_range = {}
    for i = 1, 31 do
        day_range[i] = i
    end
    if (not log:checkValue(day, day_range)) then
        return
    end
 
    -- 记录设置日期
    self.year = year
    self.month = month
    self.day = day
    self.show_image = false

    -- 如果redraw的话就刷新控件日期
    if redraw then
        -- 计算图片列表有多少图片
        local img1_list_num = self.obj:getImageNumber(0)
        local img2_list_num = self.obj:getImageNumber(1)
        if (img1_list_num > 0) then
            self.show_image = true
        end
        timeCountUpCallback(self)
    end
end



--[[
@api {SET} obj:setUITime(hour,minute,second,redraw) 设置UI时间
@apiName setUITime()
@apiGroup TimeGroup
@apiVersion 1.0.0
@apiDescription 设置时间控件显示时间

@apiParam {number} hour 设置的小时
@apiParam {number} minute 设置的分钟
@apiParam {number} second 设置的秒钟
@apiParam {boolean{true, false}} [redraw] 是否立即刷新，缺省时立即刷新

@apiParamExample 示例:
tim = gui:getComponentByName("TIME_TEST")
tim:setUITime(19, 23, 12)   -- 设置显示时间为 19:23:12

@apiSuccessExample {number} 成功响应: 
时间控件成功显示设置时间

@apiErrorExample {number} 失败响应:
时间控件未显示设置的时间
]]
function Time:setUITime(hour, minute, second, redraw)
    -- 参数类型检查
    if (not log:checkArgType("setUITime", 1, hour, "number")) then
        return
    end
    if (not log:checkArgType("setUITime", 2, minute, "number")) then
        return
    end
    if (not log:checkArgType("setUITime", 3, second, "number")) then
        return
    end
    if (not log:checkArgType("setUITime", 4, redraw, "nil", "boolean")) then
        return
    end

    -- 小时限制，0-23
    local hour_range = {}
    for i = 1, 24 do
        hour_range[i] = i - 1
    end
    if (not log:checkValue(hour, hour_range)) then
        return
    end

    local minute_range = {}
    local second_range = {}
    for i = 1, 60 do
        minute_range[i] = i - 1
        second_range[i] = i - 1
    end
    -- 分钟限制，0-59
    if (not log:checkValue(minute, minute_range)) then
        return
    end

    -- 秒钟限制，0-59
    if (not log:checkValue(second, second_range)) then
        return
    end

    -- 记录设置的时间
    self.hour = hour
    self.minute = minute
    self.second = second
    print(hour, minute, second, redraw)

    -- 如果redraw的话就刷新控件时间
    if (redraw or redraw == nil) then
        -- 计算图片列表有多少图片
        local img1_list_num = self.obj:getImageNumber(0)
        local img2_list_num = self.obj:getImageNumber(1)
        if (img1_list_num > 0) then
            self.show_image = true
        end
        timeCountUpCallback(self)
    end
end



--[[
@api {GET} year,month,day=obj:getUIData() 获取UI日期
@apiName getUIData()
@apiGroup TimeGroup
@apiVersion 1.0.0
@apiDescription 获取时间控件显示的日期

@apiParamExample 示例:
tim = gui:getComponentByName("TIME_TEST")
Y, M, D = tim:getUIData()

@apiSuccessExample {number} 成功响应: 
获取到正确的时间控件日期

@apiErrorExample {number} 失败响应:
获取到时间控件显示的日期错误
]]
function Time:getUIData()
    return self.year, self.month, self.day
end



--[[
@api {GET} hour,minute,second=obj:getUITime() 获取UI时间
@apiName getUITime()
@apiGroup TimeGroup
@apiVersion 1.0.0
@apiDescription 获取时间控件显示的时间

@apiParamExample 示例:
tim = gui:getComponentByName("TIME_TEST")
h, m, s = tim:getUITime()

@apiSuccessExample {number} 成功响应: 
获取到正确的时间控件时间

@apiErrorExample {number} 失败响应:
获取到时间控件显示的时间错误
]]
function Time:getUITime()
    return self.hour, self.minute, self.second
end



return Time
