--[[
@apiDefine DeviceGroup device
--]]


--[[
@notes
@tip:设备库，各种传感器的操作接口
@demo:step_number = device:getStepNumber()\n log:d(step_num)
]]
device = {}

device._level = 0
device._time = 0
device._step = 0
device._km = 0
device._dstep = 0
device._dkm = 0


function log_e(func, msg)
	log:e('function " '..func..'", '..msg)
end

function log_w(func, msg)
	log:w('function " '..func..'", '..msg)
end

function log_refer(msg)
    log:i('>> please refer to : '..msg)
end

function log_dev(func, msg)
	log:d('< DEVICE simulation > : ' .. func .. msg)
end


local function get_next_data(data, data_len, index)
    index = index + 1
    if (index > data_len) then
        index = data_len
        return nil, index
    end
    return data[index], index
end


local function get_prev_data(data, data_len, index)
    index = index - 1
    if (index < 1) then
        index = 1
        return nil, index
    end
    return data[index], index
end


local function get_all_data(data, data_len)
    return data
end


local function get_index_data(data, data_len, index)
    if (index > data_len) or (index < 1) then
        return nil, index
    end
    return data[index], index
end


local function get_max_data(data)
    local dat = data[1]
    local index = 1

    for k, v in ipairs(data) do
        if (v > dat) then
            dat = v
            index = k
        end
    end
    return dat, index
end


local function get_min_data(data)
    local dat = data[1]
    local index = 1

    for k, v in ipairs(data) do
        if (v < dat) then
            dat = v
            index = k
        end
    end
    return dat, index
end


local function get_data_number(data, data_len)
    local len = #data
    if (len ~= data_len) then
        data_len = len
    end
    return len
end






-- 步数模拟
local sim_step = {}
sim_step.index = 0
sim_step.data = {932, 1435, 3217, 4582, 2531, 3712, 990, 7392}
sim_step.data_num = #sim_step.data


--[[
@api {GET} setp_num=device:getStepNumber() 获取步数
@apiName getStepNumber()
@apiGroup DeviceGroup
@apiVersion 1.0.0
@apiDescription 获取实时运动步数

@apiParamExample 示例:
暂空

@apiSuccessExample {number} 成功响应: 
暂空

@apiErrorExample {number} 失败响应:
暂空

]]

--[[
@notes
@tip:获取实时运动步数
@demo:step_num = device:getStepNumber()
]]
function device:getStepNumber(...)
    local arg = {...}
    local arg_num = #arg
    local data = 0
    local index = 0

    if (arg_num == 0) then
        -- 没有参数，返回最后一个
        return sim_step.data[sim_step.data_num], sim_step.data_num
    elseif (arg_num == 1) then
        -- 只有一个参数，检查参数是否在规定范围内
        local arg_val = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40}
        if (not log:checkValue(arg[1], arg_val)) then
            return
        end
        if (arg[1] == DB.DEVICE_GET_NEXT_DATA) then
            data, index = get_next_data(sim_step.data, sim_step.data_num, sim_step.index)
        elseif (arg[1] == DB.DEVICE_GET_PREV_DATA) then
            data, index = get_prev_data(sim_step.data, sim_step.data_num, sim_step.index)
        elseif (arg[1] == DB.DEVICE_GET_ALL_DATA) then
            return sim_step.data
        elseif (arg[1] == DB.DEVICE_GET_MAX_DATA) then
            data, index = get_max_data(sim_step.data)
        elseif (arg[1] == DB.DEVICE_GET_MIN_DATA) then
            data, index = get_min_data(sim_step.data)
        elseif (arg[1] == DB.DEVICE_GET_DATA_NUM) then
            sim_step.data_num = #sim_step.data
            return sim_step.data_num
        end
    elseif (arg_num == 2) then
        if (arg[1] ~= DB.DEVICE_GET_IND_DATA) then
            log_e("getHeartRate", "arg 1 must be DB.DEVICE_GET_IND_DATA")
            return 
        end
        if (arg[2] < 0) or (arg[2] > sim_step.data_num) then
            log:w("index limit exceeded")
        end
        sim_step.index = arg[2] + 1
        data, index = get_index_data(sim_step.data, sim_step.data_num, sim_step.index)
    end

    sim_step.index = index
    self._step = data
    return data, index
end




--[[
@api {GET} sport_info=device:getSportInfo() 运动信息
@apiName getSportInfo()
@apiGroup DeviceGroup
@apiVersion 1.0.0
@apiDescription 获取运动信息

@apiParamExample 示例:
暂空

@apiSuccessExample {number} 成功响应: 
暂空

@apiErrorExample {number} 失败响应:
暂空

]]

function device:getSportInfo()
    log:emptyFunction("getSportInfo")
end





--[[
@api {GET} device:setSportStatus() 设置运动状态
@apiName setSportStatus()
@apiGroup DeviceGroup
@apiVersion 1.0.0
@apiDescription 设置运动状态

@apiParamExample 示例:
暂空

@apiSuccessExample {number} 成功响应: 
暂空

@apiErrorExample {number} 失败响应:
暂空

]]

function device:setSportStatus()
    log:emptyFunction("setSportStatus")
end







local sim_heart = {}
sim_heart.index = 0
sim_heart.data = {65, 72, 83, 96, 76, 72, 67, 91, 85, 70}
sim_heart.data_num = #sim_heart.data



--[[
@api {GET} heart=device:getHeartRate() 获取实时心率
@apiName getHeartRate()
@apiGroup DeviceGroup
@apiVersion 1.0.0
@apiDescription 获取实时心率

@apiParamExample 示例:
暂空

@apiSuccessExample {number} 成功响应: 
暂空

@apiErrorExample {number} 失败响应:
暂空

]]


--[[
@notes
@tip:获取实时心率
@demo:heart = device:getHeartRate()
]]
function device:getHeartRate(...)
    local arg = {...}
    local arg_num = #arg
    local data = 0
    local index = 0

    if (arg_num == 0) then
        -- 没有参数，返回最后一个
        return sim_heart.data[sim_heart.data_num], sim_heart.data_num
    elseif (arg_num == 1) then
        -- 只有一个参数，检查参数是否在规定范围内
        local arg_val = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40}
        if (not log:checkValue(arg[1], arg_val)) then
            return
        end
        if (arg[1] == DB.DEVICE_GET_NEXT_DATA) then
            data, index = get_next_data(sim_heart.data, sim_heart.data_num, sim_heart.index)
        elseif (arg[1] == DB.DEVICE_GET_PREV_DATA) then
            data, index = get_prev_data(sim_heart.data, sim_heart.data_num, sim_heart.index)
        elseif (arg[1] == DB.DEVICE_GET_ALL_DATA) then
            return sim_heart.data
        elseif (arg[1] == DB.DEVICE_GET_MAX_DATA) then
            data, index = get_max_data(sim_heart.data)
        elseif (arg[1] == DB.DEVICE_GET_MIN_DATA) then
            data, index = get_min_data(sim_heart.data)
        elseif (arg[1] == DB.DEVICE_GET_DATA_NUM) then
            sim_heart.data_num = #sim_heart.data
            return sim_heart.data_num
        end
    elseif (arg_num == 2) then
        if (arg[1] ~= DB.DEVICE_GET_IND_DATA) then
            log_e("getHeartRate", "arg 1 must be DB.DEVICE_GET_IND_DATA")
            return 
        end
        if (arg[2] < 0) or (arg[2] > sim_heart.data_num) then
            log:w("index limit exceeded")
        end
        sim_heart.index = arg[2] + 1
        data, index = get_index_data(sim_heart.data, sim_heart.data_num, sim_heart.index)
    end

    sim_heart.index = index
    return data, index
end



--[[
@api {GET} rest_heart=device:getRestingHeartRate() 获取静息心率
@apiName getRestingHeartRate()
@apiGroup DeviceGroup
@apiVersion 1.0.0
@apiDescription 获取静息心率

@apiParamExample 示例:
暂空

@apiSuccessExample {number} 成功响应: 
暂空

@apiErrorExample {number} 失败响应:
暂空

]]


--[[
@notes
@tip:获取静息心率
@demo:heart = device:getRestingHeartRate()
]]
function device:getRestingHeartRate()
	local number = math.random(60, 120)	-- 生成1K-1W的随机数
	log:d('create random resting heart rate --> '..number)
	return number
end



--[[
@api {GET} oxy=device:getBloodOxygen() 血氧浓度
@apiName getBloodOxygen()
@apiGroup DeviceGroup
@apiVersion 1.0.0
@apiDescription 获取血氧浓度

@apiParamExample 示例:
暂空

@apiSuccessExample {number} 成功响应: 
暂空

@apiErrorExample {number} 失败响应:
暂空

]]


--[[
@notes
@tip:获取血氧浓度
@demo:oxygen = device:getBloodOxygen()
]]
function device:getBloodOxygen()
	-- math.randomseed(tostring(os.time()):reverse():sub(1, 7))
	local number = math.random(80, 100)	-- 生成随机数
	log:d('< DEVICE simulation > : create random blood oxygen --> '..number)
	return number
end


local sim_pres = {}
sim_pres.index = 0
sim_pres.data = {990, 995, 992, 996, 993, 1000, 998, 996, 994}
sim_pres.data_num = #sim_pres.data


--[[
@api {GET} pressure=device:getPressure() 大气压强
@apiName getPressure()
@apiGroup DeviceGroup
@apiVersion 1.0.0
@apiDescription 获取大气压强

@apiParamExample 示例:
暂空

@apiSuccessExample {number} 成功响应: 
暂空

@apiErrorExample {number} 失败响应:
暂空

]]


--[[
@notes
@tip:获取大气压强
@demo:oxygen = device:getPressure()
]]
function device:getPressure(...)
    local arg = {...}
    local arg_num = #arg
    local data = 0
    local index = 0

    if (arg_num == 0) then
        -- 没有参数，返回最后一个
        return sim_pres.data[sim_pres.data_num], sim_pres.data_num
    elseif (arg_num == 1) then
        -- 只有一个参数，检查参数是否在规定范围内
        local arg_val = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40}
        if (not log:checkValue(arg[1], arg_val)) then
            return
        end
        if (arg[1] == DB.DEVICE_GET_NEXT_DATA) then
            data, index = get_next_data(sim_pres.data, sim_pres.data_num, sim_pres.index)
        elseif (arg[1] == DB.DEVICE_GET_PREV_DATA) then
            data, index = get_prev_data(sim_pres.data, sim_pres.data_num, sim_pres.index)
        elseif (arg[1] == DB.DEVICE_GET_ALL_DATA) then
            return sim_pres.data
        elseif (arg[1] == DB.DEVICE_GET_MAX_DATA) then
            data, index = get_max_data(sim_pres.data)
        elseif (arg[1] == DB.DEVICE_GET_MIN_DATA) then
            data, index = get_min_data(sim_pres.data)
        elseif (arg[1] == DB.DEVICE_GET_DATA_NUM) then
            sim_pres.data_num = #sim_pres.data
            return sim_pres.data_num
        end
    elseif (arg_num == 2) then
        if (arg[1] ~= DB.DEVICE_GET_IND_DATA) then
            log_e("getHeartRate", "arg 1 must be DB.DEVICE_GET_IND_DATA")
            return 
        end
        if (arg[2] < 0) or (arg[2] > sim_pres.data_num) then
            log:w("index limit exceeded")
        end
        sim_pres.index = arg[2] + 1
        data, index = get_index_data(sim_pres.data, sim_pres.data_num, sim_pres.index)
    end

    sim_pres.index = index
    return data, index
end


local sim_alti = {}
sim_alti.index = 0
sim_alti.data = {6, 7, 9, 3, 2, 5, 10, 5, 3, 2}
sim_alti.data_num = #sim_alti.data



--[[
@api {GET} altitude=device:getAltitude() 获取海拔高度
@apiName getAltitude()
@apiGroup DeviceGroup
@apiVersion 1.0.0
@apiDescription 获取海拔高度

@apiParamExample 示例:
暂空

@apiSuccessExample {number} 成功响应: 
暂空

@apiErrorExample {number} 失败响应:
暂空

]]


--[[
@notes
@tip:获取海拔高度
@demo:oxygen = device:getAltitude()
]]
function device:getAltitude(...)
    local arg = {...}
    local arg_num = #arg
    local data = 0
    local index = 0

    if (arg_num == 0) then
        -- 没有参数，返回最后一个
        return sim_alti.data[sim_alti.data_num], sim_alti.data_num
    elseif (arg_num == 1) then
        -- 只有一个参数，检查参数是否在规定范围内
        local arg_val = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40}
        if (not log:checkValue(arg[1], arg_val)) then
            return
        end
        if (arg[1] == DB.DEVICE_GET_NEXT_DATA) then
            data, index = get_next_data(sim_alti.data, sim_alti.data_num, sim_alti.index)
        elseif (arg[1] == DB.DEVICE_GET_PREV_DATA) then
            data, index = get_prev_data(sim_alti.data, sim_alti.data_num, sim_alti.index)
        elseif (arg[1] == DB.DEVICE_GET_ALL_DATA) then
            return sim_alti.data
        elseif (arg[1] == DB.DEVICE_GET_MAX_DATA) then
            data, index = get_max_data(sim_alti.data)
        elseif (arg[1] == DB.DEVICE_GET_MIN_DATA) then
            data, index = get_min_data(sim_alti.data)
        elseif (arg[1] == DB.DEVICE_GET_DATA_NUM) then
            sim_alti.data_num = #sim_alti.data
            return sim_alti.data_num
        end
    elseif (arg_num == 2) then
        if (arg[1] ~= DB.DEVICE_GET_IND_DATA) then
            log_e("getHeartRate", "arg 1 must be DB.DEVICE_GET_IND_DATA")
            return 
        end
        if (arg[2] < 0) or (arg[2] > sim_alti.data_num) then
            log:w("index limit exceeded")
        end
        sim_alti.index = arg[2] + 1
        data, index = get_index_data(sim_alti.data, sim_alti.data_num, sim_alti.index)
    end

    sim_alti.index = index
    return data, index
end



return device













