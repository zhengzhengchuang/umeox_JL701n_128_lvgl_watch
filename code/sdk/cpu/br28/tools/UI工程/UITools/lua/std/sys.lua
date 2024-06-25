--[[
-- 系统库
@apiDefine sysGroup sys
--]]

sys = {}

sys.time = {}   -- 模拟器系统时间
sys.time.year   = 0
sys.time.month  = 0
sys.time.day    = 0
sys.time.hour   = 0
sys.time.minute = 0
sys.time.second = 0
sys.timeInit = false


sys.timer = 0   -- 模拟器系统级定时器


sys.alarm = {}  -- 模拟器闹钟列表
sys.MaxAlarm = 5 -- 最大闹钟个数


sys.system_event_callback_register = false


-- 打印错误信息
function log_e(func, msg)
	log:e('function " '..func..'", '..msg)
end

-- 打印警告信息
function log_w(func, msg)
	log:w('function " '..func..'", '..msg)
end

-- 打印参数参考
function log_refer(msg)
	log:i('>> please refer to : '..msg)
end


Setup = {}

-- 默认音量值为0
Setup.volume = 0;


--[[
@api {GET} setup:setLightLevel(value,update) 屏幕亮度
@apiName setLightLevel()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 设置屏幕亮度挡位

@apiParam {number} value 挡位值
@apiParam {boolean{true, false}} update 是否更新到系统变量

@apiParamExample 示例:
setupHandler = sys:getSetupHandler()
setupHandler:setLightLevel(light_level, true) --value:档位值  update:是否把值更新到系统变量

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function Setup:setLightLevel(value, update)
    if (not log:checkArgType("setLightLevel", 1, value, "number")) then
        log_e('setLightLevel', 'value must an integer')
		return
	end
    if (not log:checkArgType("setLightLevel", 2, update, "boolean")) then
        log_e('setLightLevel', 'update must an boolean')
		return
	end
end



--[[
@api {GET} setup:setDarkTime(value) 息屏时间
@apiName setDarkTime()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 设置息屏时间挡位

@apiParam {number} value 息屏时间挡位

@apiParamExample 示例:
setupHandler = sys:getSetupHandler()
setupHandler:setDarkTime(dark_time_sel)

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function Setup:setDarkTime(value)
    if (not log:checkArgType("setDarkTime", 1, value, "number")) then
        log_e('setDarkTime', 'value must an integer')
		return
	end
end



--[[
@api {SET} setup:setAutoDark(onoff) 自动息屏
@apiName setAutoDark()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 自动息屏功能开关

@apiParam {boolean{true, false}} onoff 自动息屏功能打开、关闭标志

@apiParamExample 示例:
setupHandler = sys:getSetupHandler()
setupHandler:setAutoDark(true) -- 打开自动息屏功能

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function Setup:setAutoDark(onoff)
    if (not log:checkArgType("setAutoDark", 1, onoff, "boolean")) then
        return
    end
end




--[[
@api {SET} setup:setSysVolByPercent(percent) 音量设置
@apiName setSysVolByPercent()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 把百分比转换成系统音量

@apiParam {number{0, 1, 2, ..., 100}} percent 音量百分比

@apiParamExample 示例:
setupHandler = sys:getSetupHandler()
setupHandler:setSysVolByPercent(vslider_percent)

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function Setup:setSysVolByPercent(percent)
    if (not log:checkArgType("setSysVolByPercent", 1, percent, "number")) then
        log_e('setSysVolByPercent', 'percent must an integer')
		return
	end
    self.volume = percent
end



--[[
@api {GET} percent=setup:getSysVolPercent() 获取音量
@apiName getSysVolPercent()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 获取系统音量百分比

@apiParamExample 示例:
setupHandler = sys:getSetupHandler()
voice_percent = setupHandler:getSysVolPercent()

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function Setup:getSysVolPercent()
    -- local percent = math.random(0, 100)
    local percent = self.volume
    print("Vol percent:"..tostring(percent))
    return percent
end


--[[
@api {SET} setup:setSysVolUp(value) 音量加
@apiName setSysVolUp()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 系统音量增加

@apiParam {number} value 音量增加百分比

@apiParamExample 示例:
setupHandler = sys:getSetupHandler()
setupHandler:setSysVolUp(1) --音量加1(value)

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function Setup:setSysVolUp(value)
    if (not log:checkArgType('setSysVolUp', 1, value, 'number')) then
        log_e('setSysVolUp', 'value must an integer')
		return
	end
    self.volume = self.volume + value;
    if (self.volume > 100) then
        self.volume = 100
    end
end



--[[
@api {SET} setup:setSysVolDown(value) 音量减
@apiName setSysVolDown()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 系统音量减小

@apiParam {number} value 音量减小百分比

@apiParamExample 示例:
setupHandler = sys:getSetupHandler()
setupHandler:setSysVolDown(1) --音量减1(value)

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function Setup:setSysVolDown(value)
    if (not log:checkArgType('setSysVolDown', 1, value, 'number')) then
        log_e('setSysVolDown', 'value must an integer')
		return
	end
    self.volume = self.volume - value;
    if (self.volume < 0) then
        self.volume = 0
    end
end



--[[
@api {SET} setup:setSysVoiceMute(flag) 系统静音
@apiName setSysVoiceMute()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 设置系统静音标志

@apiParam {number} flag 静音标志

@apiParamExample 示例:
setupHandler = sys:getSetupHandler()
setupHandler:setSysVoiceMute(voice_mute_sel)

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function Setup:setSysVoiceMute(flag)
    if (not log:checkArgType('setSysVoiceMute', 1, flag, 'number')) then
        log_e('setSysVoiceMute', 'flag must an integer')
		return
	end
    self.volume = 0
end



--[[
@api {GET} name=setup:getWatchName(typ) 手表名称
@apiName getWatchName()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 获取手表名称

@apiParam {number{0, 1}} typ 0获取待机表盘名字，1获取表盘名字

@apiParamExample 示例:
setupHandler = sys:getSetupHandler()
watch_name = setupHandler:getWatchName(0) --type:0为获取待机表盘名字，非0为获取表盘名字

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function Setup:getWatchName(typ)
    if (not log:checkArgType('getWatchName', 1, typ, 'number')) then
        log_e('getWatchName', 'type must an integer')
		return
	end
end


--[[
@api {GET} card=setup:getCardSelTab() 获取已选卡片
@apiName getCardSelTab()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 获取已经选中的卡片组

@apiParamExample 示例:
setupHandler = sys:getSetupHandler()
card_sel_tab = setupHandler:getCardSelTab()

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function Setup:getCardSelTab()
    local table = {1, 2, 3, 4}
    print("table value:"..tablep[1]..","..table[2]..","..table[3]..","..table[4])
    return table
end


--[[
@api {SET} setup:setCardSelTab(tab) 设置选中卡片
@apiName setCardSelTab()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 同步已选卡片组到系统

@apiParam {table} tab 已选卡片组

@apiParamExample 示例:
setupHandler = sys:getSetupHandler()
setupHandler:setCardSelTab(card_sel_tab)

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function Setup:setCardSelTab(table)
    if (not log:checkArgType('setCardTab', 1, table, 'table')) then
        log_e('setCardSelTab', 'table must an table')
		return
	end
end



--[[
@api {GET} card_tab=setup:getCardTab() 获取可选卡片
@apiName getCardTab()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 从系统获取可选卡片组

@apiParamExample 示例:
setupHandler = sys:getSetupHandler()
card_tab = setupHandler:getCardTab()

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function Setup:getCardTab()
    local table = {5, 6, 7, 8, 9, 10, 11, 12, 13, 14}
    return table
end



--[[
@api {SET} setup:setCardTab(tab) 设置可选卡片组
@apiName setCardTab()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 设置可选卡片组给系统

@apiParam {table} tab 可选卡片组

@apiParamExample 示例:
setupHandler = sys:getSetupHandler()
setupHandler:setCardTab(card_tab)

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function Setup:setCardTab(table)
    if (not log:checkArgType('setCardTab', 1, table, 'table')) then
        log_e('setCardTab', 'table must an table')
		return
	end
end



--[[
@api {SET} setup:ResetOrShutDown(action) 复位或关机
@apiName ResetOrShutDown()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 系统复位或关机

@apiParam {number{0, 1}} action 0为关机，非0为复位

@apiParamExample 示例:
setupHandler = sys:getSetupHandler()
setupHandler:ResetOrShutdown(1) --action:0为关机，非0为复位

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function Setup:ResetOrShutdown(action)
    if (not log:checkArgType('ResetOrShutdown', 1, action, 'number')) then
        log_e('ResetOrShutdown', 'action must an integer')
		return
	end
end



--[[
@api {SET} setup:sysParamVmOpt(action) VM操作
@apiName sysParamVmOpt()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 系统参数VM读、写、擦除操作

@apiParam {number{0, 1, 2}} action 0为擦除，1为读，2为写

@apiParamExample 示例:
setupHandler = sys:getSetupHandler()
setupHandler:sysParamVmOpt(0) --action:0为擦除，1为读，2为写

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function Setup:sysParamVmOpt(action)
    if (not log:checkArgType('sysParamVmOpt', 1, action, 'number')) then
        log_e('sysParamVmOpt', 'action must an integer')
		return
	end
end



--[[
@api {SET} setup:syncSetupParam(name,value,action) 同步参数
@apiName syncSetupParam()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 同步C代码系统参数

@apiParam {string} name 参数名
@apiParam {number} value 数值
@apiParam {boolean{true, false}} true为获取，false为设置

@apiParamExample 示例:
setupHandler = sys:getSetupHandler()
ShortcutKey = setupHandler:syncSetupParam(DB.ShortcutKey, 0, 1) --name:参数名   value:数值  action:false为设置，true为获取

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function Setup:syncSetupParam(name, value, action)
    if (not log:checkArgType('syncSetupParam', 1, name, 'number')) then
        log_e('syncSetupParam', 'name must an integer')
		return
	end
    if (not log:checkArgType('syncSetupParam', 2, value, 'number')) then
        log_e('syncSetupParam', 'value must an integer')
		return
	end
    if (not log:checkArgType('syncSetupParam', 3, action, 'boolean')) then
        log_e('syncSetupParam', 'action must an boolean')
		return
	end
end



--[[
@api {GET} setup=sys:getSetupHandler() 设置句柄
@apiName getSetupHandler()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 获取系统设置句柄

@apiParamExample 示例:
setupHandler = sys:getSetupHandler()

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function sys:getSetupHandler()
    local self = {}
	setmetatable(self, {__index = Setup})
    return self
end




--[[
@api {GET} time_tab=sys:getTime() 获取系统时间
@apiName getTime()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 获取系统时间

@apiParamExample 示例:
time_tab = sys:getTime()
for k, v in pairs(time_tab) do
    print(k, v)
end

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function sys:getTime()
    if (not sys.timeInit) then
        local time_tab = {}
        local sys_time = os.date("%Y%m%d%H%M%S")
        log:d("get system time: "..sys_time)

        local year = string.sub(sys_time, 1, 4)
        sys.time.year = tonumber(year)

        local month = string.sub(sys_time, 5, 6)
        sys.time.month = tonumber(month)

        local day = string.sub(sys_time, 7, 8)
        sys.time.day = tonumber(day)

        local hour = string.sub(sys_time, 9, 10)
        sys.time.hour = tonumber(hour)

        local minute = string.sub(sys_time, 11, 12)
        sys.time.minute = tonumber(minute)

        local second = string.sub(sys_time, 13, 14)
        sys.time.second = tonumber(second)
    end

    return sys.time
end



--[[
@api {SET} sys:setTime(time_tab) 设置系统时间
@apiName setTime()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 设置系统时间

@apiParam {table} time_tab 时间table表

@apiParam (table) {number} year 设置年
@apiParam (table) {number{1 - 12}} month 设置月
@apiParam (table) {number{1 - 31}} day 设置日
@apiParam (table) {number{0 - 23}} hour 设置时
@apiParam (table) {number{0 - 59}} minute 设置分
@apiParam (table) {number{0 - 59}} second 设置秒

@apiParamExample 示例:
local time_tab = {}
time_tab.year   = 2022
time_tab.month  = 1
time_tab.day    = 1
time_tab.hour   = 0
time_tab.minute = 0
time_tab.second = 0
sys:setTime(time_tab)

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function sys:setTime(time_tab)
    if (not log:checkArgType("setTime", 1, time_tab, "table")) then
        return
    end
    for k, v in pairs(time_tab) do
        if (not log:checkArgType("setTime", tostring(k), v, "number")) then
            -- 检查所有参数，如果有个参数值不为数值类型就退出
            return
        end
    end

    sys.time.year   = time_tab.year
    sys.time.month  = time_tab.month
    sys.time.day    = time_tab.day
    sys.time.hour   = time_tab.hour
    sys.time.minute = time_tab.minute
    sys.time.second = time_tab.second
end



--[[
@api {SET} sys:sleep(ms) 系统延时
@apiName sleep()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 系统延时

@apiParam {number} ms 延时的毫秒数

@apiParamExample 示例:
sys:sleep(100)  -- 延时100毫秒

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function sys:sleep(ms)
    if (not log:checkArgType("sleep", 1, ms, "number")) then
        return 
    end
	log:d("sys sleep: ".. ms .."ms")
	utils:sleep(ms)
end



--[[
@api {GET} sys:random(min,max) 获取随机数
@apiName random()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 获取min到max之间的随机整数，若省略min和max，则获取int范围内的随机整数

@apiParam {number} [min] 最小值，可省略
@apiParam {number} [max] 最大值，可省略

@apiParamExample 示例:
rand = sys:random(0, 100)

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function sys:random(min, max)
    if (not log:checkArgType("random", 1, min, "nil", "number")) then
        return
    end
    if (not log:checkArgType("random", 2, max, "nil", "number")) then
        return
    end

    if (min and max) then
        if min > max then
            log:e("function \"rand\" arg must be (min, max)")
            return
        end
        rand = math.random(min, max)
    else
        -- 没传进范围，生成32bit int型随机数
        rand = math.random(0, 2147483647)
    end

    log:d("get random: ".. tostring(rand))
    return rand
end





Alarm = {}


--[[
@api {SET} alarm:addAlarm(tab) 添加闹钟
@apiName addAlarm()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 添加闹钟

@apiParam {table} tab 闹钟table

@apiParam (table) {number{0, 1}} sw 闹钟开关标志
@apiParam (table) {number} mode 闹钟循环模式
@apiParam (table) {number{0 - 23}} hour 闹钟小时
@apiParam (table) {number{0 - 59}} minute 闹钟分钟

@apiParamExample 示例:
local tab = {}
tab.sw = 1
tab.mode = 0x01
tab.hour = 8
tab.minute = 0

alarm = sys:getAlarmHandler()
alarm:addAlarm(tab)

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function Alarm:addAlarm(alarm)
    if (not log:checkArgType("addAlarm", 1, alarm, "table")) then
        return
    end
    if (not log:checkArgType("addAlarm", 2, alarm.sw, "number")) then
        return
    end
    if (not log:checkArgType("addAlarm", 3, alarm.mode, "number")) then
        return
    end
    if (not log:checkArgType("addAlarm", 4, alarm.hour, "number")) then
        return
    end
    if (not log:checkArgType("addAlarm", 5, alarm.minute, "number")) then
        return
    end

    -- 如果超过最大闹钟数，则不能再添加闹钟
    local alarm_number = #sys.alarm
    if (alarm_number >= sys.MaxAlarm) then
        log:e("The maximum number of alarms has been reached")
        return
    end

    log:d("addAlarm:")
    -- 添加闹钟到闹钟列表
    table.insert(sys.alarm, alarm)
    sys.alarm[alarm_number + 1]["snooze"] = false
end



--[[
@api {GET} alarm_tab=alarm:readAlarm(index) 读取闹钟
@apiName readAlarm()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 读取系统闹钟

@apiParam {number} [index] 闹钟索引，缺省时获取所有闹钟

@apiParamExample 示例:
alarm = sys:getAlarmHandler()
alarm_tab = alarm:readAlarm()
for k, v in pairs(alarm_tab) do
    print(k, v)
end

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function Alarm:readAlarm(index)
    if (not log:checkArgType("readAlarm", 1, index, "nil", "number")) then
        return
    end

    local alarm_number = #sys.alarm
    if index and ((index < 0) or (index > alarm_number)) then
        -- 索引必须在0到Maxalarm之间
        log:e("readAlarm index: "..tostring(index).."must between 0 to "..tostring(alarm_number))
        return
    end

    if index then
        -- 有索引返回索引指定的闹钟
        return sys.alarm[index + 1]
    else
        -- 没有索引返回全部闹钟
        return sys.alarm
    end
end



--[[
@api {SET} alarm:delAlarm(index) 删除闹钟
@apiName delAlarm()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 删除指定索引的闹钟

@apiParam {number} index 准备删除的闹钟索引

@apiParamExample 示例:
alarm = sys:getAlarmHandler()
alarm:delAlarm(1)   -- 删除索引为1的闹钟

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function Alarm:delAlarm(index)
    if (not log:checkArgType("delAlarm", 1, index, "number")) then
        return
    end

    local alarm_number = #sys.alarm
    if (index < 0) or (index > alarm_number) then
        log:e("delAlarm index: "..tostring(index).."must between 0 to "..tostring(alarm_number))
        return
    end
    table.remove(sys.alarm, index + 1)
end



--[[
@api {SET} alarm:setAlarm(cmd,index,hour,minute) 设置闹钟
@apiName setAlarm()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 闹钟设置接口，通过cmd参数区分设置内容，可以分别设置开关、时间、重复模式

@apiParam (开关设置) {number{0x01}} cmd 选择开关设置模式
@apiParam (开关设置) {number} index 选择要设置的闹钟索引
@apiParam (开关设置) {number{0, 1}} onoff 选择关闭、开启闹钟

@apiParam (时间设置) {number{0x02}} cmd 选择时间设置模式
@apiParam (时间设置) {number} index 选择要设置的闹钟索引
@apiParam (时间设置) {number{0-23}} hour 选择闹钟小时数
@apiParam (时间设置) {number{0-59}} minute 选择闹钟分钟数

@apiParam (模式设置) {number{0x04}} cmd 选择模式设置模式
@apiParam (模式设置) {number} index 选择要设置的闹钟索引
@apiParam (模式设置) {number} mode 选择闹钟循环模式

@apiParam (贪睡模式) {number{0x08}} cmd 选择模式设置模式，贪睡十分钟

@apiParam (关闭闹钟) {number{0x10}} cmd 选择模式设置模式，今天关闭闹钟

@apiParamExample 示例:
alarm = sys:getAlarmHandler()
alarm:setAlarm(1, 3, 0)     -- 关闭索引为3的闹钟

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function Alarm:setAlarm(cmd, index, ...)
    if (not log:checkArgType("setAlarm", 1, cmd, "number")) then
        return
    end
    if (not log:checkArgType("setAlarm", 2, index, "number")) then
        return
    end

    local arg = {...}
    local arg_num = #arg
    index = index + 1

    if (cmd == DB.SYS_ALARM_SET_ONOFF) then
        -- 设置开关
        if (not log:checkArgType("setAlarm", 3, arg[1], "boolean", "number")) then
            return
        end
        log:d("setAlarm, sw:"..tostring(arg[1]))
        sys.alarm[index].sw = arg[1]
    elseif (cmd == DB.SYS_ALARM_SET_TIME) then
        -- 设置时间
        if (not log:checkArgType("setAlarm", 3, arg[1], "number")) then
            return
        end
        if (not log:checkArgType("setAlarm", 4, arg[2], "number")) then
            return
        end
        log:d("setAlarm, hour:"..tostring(arg[1])..", minute:"..tostring(arg[2]))
        sys.alarm[index].hour = arg[1]
        sys.alarm[index].minute = arg[2]
    elseif (cmd == DB.SYS_ALARM_SET_MODE) then
        -- 设置模式
        if (not log:checkArgType("setAlarm", 3, arg[1], "number")) then
            return
        end
        log:d("setAlarm, mode:"..tostring(arg[1]))
        sys.alarm[index].mode = arg[1]
    elseif (cmd == DB.SYS_ALARM_SET_SNOOZE) then
        -- 贪睡
        for k, v in pairs(sys.alarm) do
            if (v.hour == sys.time.hour) and (v.minute == sys.time.minute) then
                sys.alarm[k].snooze = true
            end
        end
    elseif (cmd == DB.SYS_ALARM_SET_CLOSE) then
        -- 关闭闹钟
        for k, v in pairs(sys.alarm) do
            if (v.hour == sys.time.hour) and (v.minute == sys.time.minute) then
                sys.alarm[k].snooze = false
            end
        end
    end
end



function Alarm:orderDate()
    log:emptyFunction("orderDate")
end


function Alarm:stataWrite()
    log:emptyFunction("stataWrite")
end


function Alarm:stataRead()
    log:emptyFunction("stataRead")
end



--[[
@api {GET} alarmHandler=sys:getAlarmHandler() 闹钟句柄
@apiName getAlarmHandler()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 获取闹钟句柄

@apiParamExample 示例:
alarm = sys:getAlarmHandler()

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function sys:getAlarmHandler()
    local self = {}
    setmetatable(self, {__index = Alarm})
    return self
end





sysLib_FileIo = {}


--[[
@api {GET} file_io:open(path,mode) 打开文件
@apiName open()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 打开指定文件

@apiParam (通话记录、通讯录) {number{0, 1}} typ 选择打开1通话记录，或者0通讯录

@apiParam (打开文件) {string} path 文件路径
@apiParam (打开文件) {string} mode 文件打开模式

@apiParamExample 示例:
file_io = sys:getFileIoHandler()
path = "C:test.txt"
file_io:open(path, "r+")

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function sysLib_FileIo:open(filename, mode)
    if (type(filename) == "number") then
        self.hdl_typ = filename
        local file_count = 0
        if (filename == 0) then
            -- 选择打开通话记录
        elseif (filename == 1) then
            -- 选择打开通讯录
        end
        return file_count
    elseif (type(filename) == "string") then
        -- 普通方式打开文件
        if (not log:checkArgType("open", 2, mode, "string")) then
            return
        end
        self.file = io:open(filename, mode)
    else
        log:e("open err, arg 1th type err")
        return
    end
end



--[[
@api {GET} data=file_io:read(file,arg) 读取文件
@apiName read()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 读取指定文件内容

@apiParam (通话记录、通讯录) {number} mode 读取模式
@apiParam (通话记录、通讯录) {number} arg 读取参数

@apiParam (打开文件) {FILE} file 文件句柄
@apiParam (打开文件) {arg} arg 读取参数

@apiParamExample 示例:
无

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function sysLib_FileIo:read()
    return self.file:read()
    -- log:emptyFunction("io:read")
end



--[[
@api {SET} file_io:write(arg) 写文件
@apiName write()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 将指定内容写入文件

@apiParamExample 示例:
无

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function sysLib_FileIo:write(str)
    self.file:write(str)
    -- log:emptyFunction("io:write")
end


--[[
@api {SET} file_io:delete(arg) 删除文件
@apiName delete()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 删除指定文件

@apiParamExample 示例:
无

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function sysLib_FileIo:delete(file)
    self.file:seek(whence, offset)
    -- log:emptyFunction("io:seek")
end



--[[
@api {SET} file_io:seek(offset,mode) 文件seek
@apiName seek()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 文件seek操作

@apiParamExample 示例:
无

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function sysLib_FileIo:seek(whence, offset)
    self.file:seek(whence, offset)
    -- log:emptyFunction("io:seek")
end


--[[
@api {GET} attr=file_io:attrs() 文件属性
@apiName attrs()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 获取文件属性

@apiParamExample 示例:
无

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function sysLib_FileIo:attrs()
    self.file:close()
    -- log:emptyFunction("io:close")
end


--[[
@api {SET} file_io:close(file) 关闭文件
@apiName close()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 关闭指定文件

@apiParamExample 示例:
无

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function sysLib_FileIo:close()
    self.file:close()
    -- log:emptyFunction("io:close")
end



--[[
@api {GET} file_io=sys:getFileIoHandler() 文件句柄
@apiName getFileIoHandler()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 获取文件句柄

@apiParamExample 示例:
无

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function sys:getFileIoHandler()
    local self = {}
    setmetatable(self, {__index = sysLib_FileIo})
    return self
end







--[[
-- 定时器部分
--]]

Timer = {}

-- 定时器打印
function timer_db(msg)
	log:d('< Timer simulation > :'..msg)
end

-- 定时器回调函数，timeout和timer不同之处在于timeout执行一遍即停止
local function timer_callback(arg)
    arg._func(arg._arg)
    if (arg._type == DB.SYS_TIMEOUT_HANDLER) then
        -- 如果是timeout事件，执行一次后删除定时器
        utils:clearTimer(arg._idx)
        arg._idx = 0
    end
end



--[[
@api {SET} timer:run() 运行定时器
@apiName run()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 启动定时器运行

@apiParamExample 示例:
-- 定时器测试回调函数
function test_timer()
    print("test")
end
timer = sys:createTimer(test_timer, 500)    -- 创建500毫秒的定时器
timer:run()     -- 运行定时器

@apiSuccessExample {number} 成功响应: 
定时器执行

@apiErrorExample {number} 失败响应:
定时器不执行
]]
function Timer:run()
    if (self._idx ~= 0) then
        timer_db("timer has runing: "..tostring(self._idx))
    else
        if (self._type == DB.SYS_TIMER_HANDLER) then
            timer_db('timer run: '..self._idx..', '..self._ms..'ms')
            -- 创建timer
            -- self._idx = utils:createTimer(self._ms, self._func, self._arg)
        elseif (self._type == DB.SYS_TIMEOUT_HANDLER) then
            timer_db('timeout run: '..self._idx..', '..self._ms..'ms')
            -- 创建timeout
        end
        self._idx = utils:createTimer(self._ms, timer_callback, self)
    end
    return self._idx
end



--[[
@api {SET} timer:stop() 暂停定时器
@apiName stop()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 暂停定时器运行

@apiParamExample 示例:
-- 定时器测试回调函数
function test_timer()
    print("test")
end
timer = sys:createTimer(test_timer, 500)    -- 创建500毫秒的定时器
timer:run()     -- 运行定时器
sys:sleep(2000)     -- 系统延时2000ms
timer:stop()       -- 暂停定时器

@apiSuccessExample {number} 成功响应: 
定时器暂停运行

@apiErrorExample {number} 失败响应:
定时器继续运行
]]
function Timer:stop()
    if (self._idx ~= 0) then
        if (self._type == SYS_TIMER_HANDLER) then
            timer_db('timer stop: '..tostring(self._idx)..', '..tostring(self._ms))
            -- 删除timer
            -- utils:clearTimer(self._idx)
        elseif (self._type == SYS_TIMEOUT_HANDLER) then
            timer_db("timeout stop: "..tostring(self._idx)..", "..tostring(self._ms))
            -- 删除timeout
        end
        utils:clearTimer(self._idx)
        self._idx = 0
    else
		timer_db('timer not runing')
	end
end



--[[
@api {GET} timer=sys:createTimer([arg,]callback,ms[,flag]) 创建定时器
@apiName createTimer()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 创建定时器

@apiParam {table} [arg] 传给回调函数的参数，需要全局的table，无参数传给回调时可省略
@apiParam {function} callback 回调函数，必须是全局函数
@apiParam {number} ms 定时时间，单位毫秒
@apiParam {number{DB.SYS_TIMER_HANDLER, DB.SYS_TIMEOUT_HANDLER}} [flag] 创建timer 或者 timeout回调，省略时默认timer

@apiParamExample 示例:
-- 定时器测试回调函数
function test_timer()
    print("test")
end
timer = sys:createTimer(test_timer, 500)    -- 创建500毫秒的定时器
timer:run()     -- 运行定时器

timeout = sys:createTimer(test_timer, 500, DB.SYS_TIMEOUT_HANDLER)  -- 创建500毫秒的timeout定时器

@apiSuccessExample {number} 成功响应: 
成功创建 timer 或者 timeout 句柄

@apiErrorExample {number} 失败响应:
创建句柄返回值为 nil
]]
function sys:createTimer(...)
    local arg = {...}
    local arg_num = #arg

    if (arg_num < 2) then
        -- 如果少于两个参数，说明参数肯定不对
        log:e("createTimer arg err!")
        return nil
    end

	local self = {}

    if (type(arg[1]) == "table") then
        -- 如果第一个参数是table，说明有参数给回调
        if (not log:checkArgType("createTimer", 2, arg[2], "function")) then
            -- 第二个参数必须是回调函数
            return nil
        end
        if (not log:checkArgType("createTimer", 3, arg[3], "number")) then
            -- 第三个函数必须是定时的时间ms
            return nil
        end

        self._arg = arg[1]
        self._func = arg[2] 
        self._ms = arg[3]

        if (arg[4]) then
            -- 如果有指定注册的事件类型
            self._type = arg[4]
        else
            -- 默认是定时器
            self._type = DB.SYS_TIMER_HANDLER
        end
    elseif (type(arg[1]) == "function") then
        -- 如果第一个参数是function，说明没有参数给回调
        if (not log:checkArgType("createTimer", 2, arg[2], "number")) then
            -- 第二个参数必须是定时时间ms
            return nil
        end

        self._func = arg[1] 
        self._ms = arg[2]
        self._arg = nil

        if (arg[3]) then
            -- 如果有指定注册的事件类型
            self._type = arg[3]
        else
            -- 默认是定时器
            self._type = DB.SYS_TIMER_HANDLER
        end
    end

	self._idx = 0
	timer_db('create '..tostring(self._ms)..'ms timer')
    -- 继承timer的方法
	setmetatable(self, {__index = Timer})
	return self
end



--[[
@api {SET} sys:deleteTimer(timer) 删除定时器
@apiName deleteTimer()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 删除指定定时器。注意，所有timer和timeout都需要删除

@apiParam {table} timer 需删除的timer或者timeout

@apiParamExample 示例:
sys:deleteTimer(timer)

@apiSuccessExample {number} 成功响应: 
定时器停止，内存释放

@apiErrorExample {number} 失败响应:
定时器不停止，或者内存依旧占用
]]
function sys:deleteTimer(Timer)
    if (not log:checkArgType("deleteTimer", 1, Timer, "table")) then
        return
    end

    local event_type = Timer._type
    local event_idx = Timer._idx
    if (not event_type) then
        log:e("unknow event type, arg is not a timer or timeout class")
    elseif (event_type == DB.SYS_TIMER_HANDLER) then
        -- 删除timer
        -- utils:clearTimer(event_idx)
        timer_db("delete timer event handler")
    elseif (event_type == DB.SYS_TIMEOUT_HANDLER) then
        -- 删除timeout
        timer_db("delete timeout event handler")
    end
    utils:clearTimer(event_idx)
    -- 清空id
    Timer._idx = 0
end



local system_event = {}

-- 系统事件类型，用于判断是否有对应事件可操作
system_event.event_type = {
    "bt_list", 
    "bt_status", 
    "bt_connect_info", 
    "message_status", 
    "music_start", 
    "music_status",
    "heart_rate",
    "weather_info",
    "phone_num",
    "upgrade",
    "dev_event",
    "key_event",
    "bt_event"
}

system_event.callback = {}



--[[
@api {SET} sys:registerHandler(type,callback) 注册事件
@apiName registerHandler()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 注册指定类型的系统事件回调

@apiParam {string} type 事件类型
@apiParam {function} callback 回调函数

@apiParamExample 示例:
function bt_status_callback()
    print("this is bt status event callback")
end
sys:registerHandler("bt_status", bt_status_callback)

@apiSuccessExample {number} 成功响应: 
系统触发事件时，执行相应回调

@apiErrorExample {number} 失败响应:
系统触发事件时，相应回调未执行
]]
function sys:registerHandler(event_type, callback)
    -- 如果事件类型不在事件列表里面则退出
    if (not log:checkValue(event_type, system_event.event_type)) then
        return
    end

    -- 如果第二个参数不是回调函数则退出
    if (not log:checkArgType("registerHandler", 2, callback, "function")) then
        return
    end
    system_event.callback[event_type] = callback

    -- 打印事件
    local event_list = {}
    for k, v in pairs(system_event.callback) do
        log:d("event type: "..k.." --> "..tostring(v))
        table.insert(event_list, k);
    end

    -- 注册事件列表到模拟器事件列表
    sim_system:setSystemEventList(event_list)
    -- utils:print(sim_system:getSystemEventList())

    -- 如果没有注册事件回调却开始注册事件，那么需要把回调机制注册进去
    if (not sys.system_event_callback_register) then
        sim_system:bindSystemEvent(function (key, value) 
            if (not log:checkArgType("system event type", 1, key, "string")) then
                return
            end
            sys:runHandler(key, value)
        end)
        sys.system_event_callback_register = true
    end
end


--[[
@api {SET} sys:releaseHandler(type) 释放事件
@apiName releaseHandler()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 释放指定类型的事件回调

@apiParam {string} type 待释放的事件类型

@apiParamExample 示例:
sys:releaseHandler("bt_status")

@apiSuccessExample {number} 成功响应: 
系统触发事件时，对应的回调已被释放而不执行

@apiErrorExample {number} 失败响应:
系统触发事件时，对应回调依然被执行
]]
function sys:releaseHandler(event_type)
    -- 如果事件类型不在事件列表里面则退出
    if (not log:checkValue(event_type, system_event.event_type)) then
        return
    end
    system_event.callback[event_type] = nil

    -- 打印事件
    local event_list = {}
    for k, v in pairs(system_event.callback) do
        log:d("event type: "..k.." --> "..tostring(v))
        table.insert(event_list, k)
    end

    -- 重新注册事件列表到模拟器
    -- 刷新内部列表，这样取消某个事件注册时
    -- 对应的事件类型也会在事件列表中删除
    sim_system:setSystemEventList(event_list)
end



--[[
@api {GET} hdl=sys:checkHandler(type) 检查事件
@apiName checkHandler()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 检查指定事件回调是否存在

@apiParam {string} type 事件名称

@apiParamExample 示例:
sys:checkHandler("bt_status")

@apiSuccessExample {number} 成功响应: 
true 事件已注册

@apiErrorExample {number} 失败响应:
false 事件未注册；nil 事件不存在
]]
function sys:checkHandler(event_type)
    -- 如果检查的事件类型没有在事件类型列表里面，直接退出
    if (not log:checkValue(event_type, system_event.event_type)) then
        log_e("checkHandler", "event type \""..tostring(event_type).."\" is not system event!")
        return
    end

    -- 如果事件在内部事件列表里，同时事件有回调函数
    local event_list = sim_system:getSystemEventList()
    for k, v in ipairs(event_list) do
        if (event_type == v) and (system_event.callback[event_type]) then
            return true
        end
    end
    -- if (log:checkValue(event_type, event_list)) and (system_event.callback[event_type]) then
        -- return true
    -- end
    return false
end



-- 运行系统事件，注意这个函数只用于模拟器模拟触发事件，不能用于开发
function sys:runHandler(event_type, ...)
    if (not log:checkValue(event_type, system_event.event_type)) then
        return
    end

    local arg = {...}
    -- local arg_num = #arg
    system_event.callback[event_type](...)
end




--[[
@api {GET} sys:loadMessage() 加载消息
@apiName loadMessage()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 加载微信、QQ、钉钉等通知消息

@apiParamExample 示例:
无

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function sys:loadMessage()
    local msg = {}
    msg[1].title = "测试"
    msg[1].content = "这是一则测试消息！"
    msg[1].type = 1

    msg[2].title = "测试"
    msg[2].content = "这是一则测试消息！"
    msg[2].type = 1

    msg[3].title = "测试"
    msg[3].content = "这是一则测试消息！"
    msg[3].type = 1

    return msg
end




--[[
@api {GET} len=sys:getMessageLen(index) 消息长度
@apiName getMessageLen()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 获取指定消息的数据长度

@apiParamExample 示例:
无

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function sys:getMessageLen(index)
    if (not log:checkArgType("getMessageLen", 1, index, "number")) then
        return
    end

    local msg_len = {}
    msg_len.title_len = 2--#"测试"
    msg_len.content_len = 10--#"这是一则测试消息！"
    return msg_len
end






sysLib_Weather = {}

local simulation_weather = {}
simulation_weather.weather = "晴天"
simulation_weather.temp = "27"
simulation_weather.humidity = "35"
simulation_weather.wind_dir = "东南"
simulation_weather.wind_power = "3"
simulation_weather.update_time = "20220101"
simulation_weather.provice = "广东"
simulation_weather.city = "珠海"


--[[
@api {GET} weather=weatherHandler:getWeatherHandle() 天气句柄
@apiName getWeatherHandle()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 获取天气句柄

@apiParamExample 示例:
weatherHandler = sys:loadWeather()
weather=weatherHandler:getWeatherHandle()

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function sysLib_Weather:getWeatherHandle()
    return simulation_weather
end


--[[
@api {GET} weat=weatherHandler:getWeather(weather) 获取天气
@apiName getWeather()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 获取天气状态

@apiParam {userDate} weather 天气数据指针

@apiParamExample 示例:
weather_info = weatherHandler:getWeather(weather)

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function sysLib_Weather:getWeather(weather)
    return simulation_weather.weather
end


--[[
@api {GET} temp=weatherHandler:getTemperature(weather) 获取气温
@apiName getTemperature()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 获取天气的气温数据

@apiParam {userDate} weather 天气数据指针

@apiParamExample 示例:
temp=weatherHandler:getTemperature(weather)

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function sysLib_Weather:getTemperature(weather)
    return simulation_weather.temp
end


--[[
@api {GET} hum=weatherHandler:getHumidity(weather) 获取湿度
@apiName getHumidity()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 获取天气中的湿度数据

@apiParam {userDate} weather 天气数据指针

@apiParamExample 示例:
hum=weatherHandler:getHumidity(weather)

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function sysLib_Weather:getHumidity(weather)
    return simulation_weather.humidity
end


--[[
@api {GET} wind_dir=weatherHandler:getWindDirection(weather) 获取风向
@apiName getWindDirection()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 获取天气数据中的风向

@apiParam {userDate} weather 天气数据指针

@apiParamExample 示例:
wind_dir=weatherHandler:getWindDirection(weather)

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function sysLib_Weather:getWindDirection(weather)
    return simulation_weather.wind_dir
end


--[[
@api {GET} wind_power=weatherHandler:getWindPower(weather) 获取风力
@apiName getWindPower()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 获取天气数据中的风力等级

@apiParam {userDate} weather 天气数据指针

@apiParamExample 示例:
wind_power=weatherHandler:getWindPower(weather)

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function sysLib_Weather:getWindPower(weather)
    return simulation_weather.wind_power
end


--[[
@api {GET} uptime=weatherHandler:getUpdateTime(weather) 更新时间
@apiName getUpdateTime()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 获取天气数据的更新时间

@apiParam {userDate} weather 天气数据指针

@apiParamExample 示例:
uptime=weatherHandler:getUpdateTime(weather)

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function sysLib_Weather:getUpdateTime(weather)
    return simulation_weather.update_time
end


--[[
@api {GET} provice=weatherHandler:getProvinceData(weather) 天气省份
@apiName getProvinceData()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 获取天气数据中的省份数据

@apiParam {userDate} weather 天气数据指针

@apiParamExample 示例:
provice=weatherHandler:getProvinceData(weather)

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function sysLib_Weather:getProvinceData(weather)
    return simulation_weather.provice
end


--[[
@api {GET} city=weatherHandler:getCityData(weather) 天气城市
@apiName getCityData()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 获取天气数据中的城市

@apiParam {userDate} weather 天气数据指针

@apiParamExample 示例:
city=weatherHandler:getCityData(weather)

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function sysLib_Weather:getCityData(weather)
    return simulation_weather.city
end


--[[
@api {GET} weatherHandler=sys:loadWeather() 天气句柄
@apiName loadWeather()
@apiGroup sysGroup
@apiVersion 1.0.0
@apiDescription 获取天气信息操作句柄

@apiParamExample 示例:
weatherHandler=sys:loadWeather()

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function sys:loadWeather()
    local self = {}
    setmetatable(self, {__index = sysLib_Weather})
    return self
end








return sys



