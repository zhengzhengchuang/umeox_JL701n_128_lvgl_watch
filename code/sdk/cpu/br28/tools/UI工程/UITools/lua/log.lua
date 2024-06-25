--[[
--
-- lua 打印方法库
--
-- 注意，本库仅用于模拟器内部库开发，包含颜色和特殊标记打印
--
-- 用于区分库打印和开发者自己使用print的打印。
--
--]]

log = {}



-- 打印红色字体 错误信息
function log:e(msg)
    utils:log("[ ERROR ]: "..msg, 'red')
end


-- 打印蓝色字体 警报信息
function log:w(msg)
    utils:log("[ WARNING ]: "..msg, 'blue')
end


-- 打印灰色字体 调试信息
function log:d(msg)
    utils:log("[ DEBUG ]: "..msg, 'gray')
end


-- 打印黑色字体 提示信息
function log:i(msg)
    utils:log("[ INFO ]: "..msg, 'black')
end


-- 指定颜色打印
function log:c(...)
    local arg = {...}
    local arg_num = #arg
    if (arg_num == 2) then
        utils:log(arg[1], arg[2])
    else
        utils:log(arg[1], "gray")
    end
end


-- 参数类型检查
function log:checkArgType(func, arg_index, arg, ...)
    local type_right = false
    local type_str = "\""
    local arg_type = {...}

    if #arg_type >= 1 then
        for k, v in ipairs(arg_type) do
            if (type(arg) == v) then
                type_right = true
            end
            type_str = type_str .. v .. "\", \""
        end
    else
        utils:log("ERROR: not arg type to check!", "red")
        return false
    end

    -- 如果参数类型错误，给出提示
    if not type_right then
        if (type(arg_index) == "string") then
            utils:log("[Parameter error] function \""..func.."\" parameter \""..arg_index.."\" type err!", "red")
        else
            utils:log("[Parameter error] function \""..func.."\" "..tostring(arg_index).."th parameter type err!", "red")
        end
        type_str = string.sub(type_str, 1, -4)
        utils:log("This parameter type must be: "..type_str, "orange")
    end

    return type_right
end


-- 检查某个参数的值是否在指定范围内
function log:checkValue(arg, arg_tab)
    local val_index = 0
    local type_str = "\""
    for k, v in ipairs(arg_tab) do
        if (arg == v) then
            val_index = k
            return true, val_index, arg_tab[val_index]
        end
        type_str = type_str .. v .. "\", \""
    end
    type_str = string.sub(type_str, 1, -4)
    utils:log("parameter \""..tostring(arg).."\" error!", "red")
    utils:log("This parameter type must be: "..type_str, "orange")
    return false, nil, nil
end


-- 驻函数，当声明空函数还未实现时，用此作为标记提示
function log:emptyFunction(functionName)
    utils:log("function \""..functionName.."\" is empty function", "green")
end


-- 时间刷新
function log:updateTime(self_tab, update)
    -- 秒计数
    if update then
        self_tab.second = self_tab.second + 1
    end

	if (self_tab.second > 59) then
		self_tab.second = 0
		self_tab.minute = self_tab.minute + 1
	end
	-- 分钟
	if (self_tab.minute > 59) then
		self_tab.minute = 0
		self_tab.hour = self_tab.hour + 1
	end
	-- 小时
	if (self_tab.hour > 23) then
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

    for k, v in pairs(sys.time) do
        print(k, v)
    end
end


-- 泰勒公式，根据日期计算星期
function log:zeller(date)
    local y = date.year % 100
    local c = date.year / 100
    local m = date.month
    local d = date.day

    if (m == 1) or (m == 2) then
        y = y - 1
        m = m + 12
    end

    -- 泰勒公式
    local w = y + y / 4 + c / 4 - 2 * c + 13 * (m + 1) / 5 + d - 1

    -- 确保余数为正
    while (w < 0) do
        w = w + 7
    end

    -- 向下取整，去除小数
    w = math.floor(w % 7)

    return w
end



return log

