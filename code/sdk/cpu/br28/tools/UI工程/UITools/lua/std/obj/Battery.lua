--[[
-- 电池控件方法
@apiDefine BatGroup 电量控件
--]]
Battery = {}



--[[
@api {SET} val=obj:getBatteryValue() 电池电量
@apiName getBatteryValue()
@apiGroup BatGroup
@apiVersion 1.0.0
@apiDescription 获取电池控件中显示的电量

@apiParamExample 示例:
暂空

@apiSuccessExample {number} 成功响应: 
暂空

@apiErrorExample {number} 失败响应:
暂空
]]
function Battery:getBatteryValue()
	local index = self.obj:getImageIndex(0)
	local number = self.obj:getImageNumber(0)
	local power = (index / number) * 100
	log:d('ename: [ '..self._ename..' ] getBatteryValue: ' .. tostring(power))
	return math.floor(power)
end



--[[
@api {SET} charging=obj:batteryIsCharging()  充电状态
@apiName batteryIsCharging()
@apiGroup BatGroup
@apiVersion 1.0.0
@apiDescription 获取电池充电状态

@apiParamExample 示例:
暂空

@apiSuccessExample {number} 成功响应: 
暂空

@apiErrorExample {number} 失败响应:
暂空
]]
function Battery:batteryIsCharging()
	local is_charging = self._highlight
	print('ename: [ '..self._ename..' ] batteryIsCharging: ' .. tostring(is_charging))
	return is_charging
end



--[[
@api {SET} obj:setBatteryLevel(percent,charging) 设置电量
@apiName setBatteryLevel()
@apiGroup BatGroup
@apiVersion 1.0.0
@apiDescription 设置电池控件电量百分比和充电状态

@apiParamExample 示例:
暂空

@apiSuccessExample {number} 成功响应: 
暂空

@apiErrorExample {number} 失败响应:
暂空
]]
function Battery:setBatteryLevel(...)
    local arg = {...}
    local arg_num = #arg

    local level = arg[1]
    local is_charging = arg[2]
    local redraw = true
    if (not log:checkArgType("setBatteryLevel", 1, level, "number")) then
        return 
    end
    if (not log:checkArgType("setBatteryLevel", 2, is_charging, "boolean")) then
        return 
    end
    if (arg_num == 3) then
        if (not log:checkArgType("setBatteryLevel", 3, arg[3], "boolean")) then
            return
        end
        redraw = arg[3]
    end

	log:d('ename: [ '..self._ename..' ] setBatteryLevel: ' .. level .. ', ' .. tostring(is_charging))
    if (not redraw) then
        return
    end
	
	if is_charging then
		-- 如果正在充电，表示显示高亮状态
		self.obj:showImageByIndex(1, 0)
		self._highlight = true
	else
		-- 否则为普通状态，看总图片跟百分比计算显示那一张图片
		local img_num = self.obj:getImageNumber(0)-- 普通图片有几张
		--print(img_num)
		local set_lev = level / (100 / img_num)	-- 计算电量等级
		--print(math.floor(set_lev))
		self.obj:showImageByIndex(0, math.floor(set_lev))
		self._highlight = false
	end
end


return Battery
