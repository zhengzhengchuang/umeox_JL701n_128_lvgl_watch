--[[
-- 表盘控件方法
@apiDefine WatchGroup watch
--]]
Watch = {}

Watch.hour = 0
Watch.minute = 0
Watch.second = 0



--[[
@api {SET} obj:setWatchTime(hour,minute,second[,redraw]) 设置表盘时间
@apiName setWatchTime()
@apiGroup WatchGroup
@apiVersion 1.0.0
@apiDescription 设置表盘控件时间

@apiParam {number} hour 时
@apiParam {number} minute 分
@apiParam {number} second 秒
@apiParam {boolean{true, false}} [redraw] 是否立即刷新，缺省时立即刷新

@apiParamExample 示例:
watch = gui:getComponentByName("WATCH_TEST")
watch:setWatchTime(13, 23, 25)      -- 设置表盘显示时间 13:23:25

@apiSuccessExample {number} 成功响应: 
表盘正确显示指定时间

@apiErrorExample {number} 失败响应:
表盘显示时间错误
]]
function Watch:setWatchTime(hour, min, sec, redraw)
    -- 参数类型检查
    if (not log:checkArgType("setWatchTime", 1, hour, "number")) then
        return
    end
    if (not log:checkArgType("setWatchTime", 2, min, "number")) then
        return
    end
    if (not log:checkArgType("setWatchTime", 3, sec, "number")) then
        return
    end
    if (not log:checkArgType("setWatchTime", 4, redraw, "nil", "boolean")) then
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
    if (not log:checkValue(min, minute_range)) then
        return
    end

    -- 秒钟限制，0-59
    if (not log:checkValue(sec, second_range)) then
        return
    end

    -- 记录设置的时间
    self.hour = hour
    self.minute = min
    self.second = sec

    -- 计算每个指针对应的角度
    local angle_step = 6 -- 每小格6度
    local angle_hour = 30 -- 每大格30度
    local sec_angle = sec * angle_step
    local min_angle = min * angle_step
    local hour_angle = hour * angle_hour

    -- 时针除了自身小时对应度数，分钟每走一分钟，时针应该增加2度
    hour_angle = hour_angle + (min / 2)

    -- 刷新指针
    local sub_ctrl = self.obj:getSubControls()
    for k, v in ipairs(sub_ctrl) do
        local ename = v:getControlEname()
        local caption = v:getControlCaption()
        local ctl_obj = ui:getComponentByName(ename)
        -- print(v:getControlEname())
        -- print(v:getControlCaption())
        if (caption == "watch_sec") then
            local rx = ctl_obj:getAttrNumberByIndex(0)
            local ry = ctl_obj:getAttrNumberByIndex(1)
            local w = ctl_obj:getComponentParent():getWidth()
            local h = ctl_obj:getComponentParent():getHeight()
            -- print(rx, ry)
            ctl_obj:setRotateByPos(sec_angle, rx, ry, w/2, h/2)
        end

        if (caption == "watch_min") then
            local rx = ctl_obj:getAttrNumberByIndex(0)
            local ry = ctl_obj:getAttrNumberByIndex(1)
            local w = ctl_obj:getComponentParent():getWidth()
            local h = ctl_obj:getComponentParent():getHeight()
            -- print(rx, ry)
            ctl_obj:setRotateByPos(min_angle, rx, ry, w/2, h/2)
        end

        if (caption == "watch_hour") then
            local rx = ctl_obj:getAttrNumberByIndex(0)
            local ry = ctl_obj:getAttrNumberByIndex(1)
            local w = ctl_obj:getComponentParent():getWidth()
            local h = ctl_obj:getComponentParent():getHeight()
            -- print(rx, ry)
            ctl_obj:setRotateByPos(hour_angle, rx, ry, w/2, h/2)
        end
    end
end



--[[
@api {GET} hour,minute,second=obj:getWatchTime() 获取表盘时间
@apiName getWatchTime()
@apiGroup WatchGroup
@apiVersion 1.0.0
@apiDescription 获取表盘控件的时间

@apiParamExample 示例:
watch = gui:getComponentByName("WATCH_TEST")
hour, minute, second = watch:getWatchTime()

@apiSuccessExample {number} 成功响应: 
获取到正确的表盘时间

@apiErrorExample {number} 失败响应:
获取到错误的表盘显示时间
]]
function Watch:getWatchTime(...)
    return self.hour, self.minute, self.second
end
 


return Watch
