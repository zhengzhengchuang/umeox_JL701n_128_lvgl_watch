--[[
-- 指南针控件方法
@apiDefine CompassGroup compass
--]]
Compass = {}

Compass.bk_angle = 0
Compass.indicator_angle = 0



--[[
@api {SET} obj:setCompassAngle(bk_angle,indicator_angle) 设置指南针
@apiName setCompassAngle()
@apiGroup CompassGroup
@apiVersion 1.0.0
@apiDescription 设置指南针背景和指针角度

@apiParam {number{0 - 360}} bk_angle 背景图角度
@apiParam {number{0 - 360}} indicator_angle 指针角度

@apiParamExample 示例:
compass = gui:getComponentByName("COMPASS_TEST")
compass:setCompassAngle(30, 0)

@apiSuccessExample {number} 成功响应: 
指针和背景图被旋转到指定角度

@apiErrorExample {number} 失败响应:
指针和背景图旋转角度错误
]]
function Compass:setCompassAngle(...)
    local arg = {...}
    local arg_num = #arg

    if (arg_num ~= 2) and (arg_num ~= 3) then
        -- 参数不是两个或三个
        log_e("setCompassAngle", "arg number err!")
        return
    end
    if (not log:checkArgType("setCompassAngle", 1, arg[1], "number")) or (arg[1] < 0) or (arg[1] > 360 )then
        log:e("backgrand angle must between 0 to 360")
        return
    end
    if (not log:checkArgType("setCompassAngle", 2, arg[2], "number")) or (arg[2] < 0) or (arg[2] > 360 )then
        log:e("indicator angle must between 0 to 360")
        return
    end
 
    if (arg_num == 3) then
        if (not log:checkArgType("setCompassAngle", 3, arg[3], "boolean")) then
            return
        end
        if (arg[3] == false) then
            return
        end
    end

    self.bk_angle = arg[1]
    self.indicator_angle = arg[2]
    local sub_ctrl = self.obj:getSubControls()
    for k, v in ipairs(sub_ctrl) do
        local ename = v:getControlEname()
        local caption = v:getControlCaption()
        local ctl_obj = ui:getComponentByName(ename)
        -- print(ename, caption, ctl_obj)
        if (caption == "compass_bkimg") then
            local rx = ctl_obj:getAttrNumberByIndex(0)
            local ry = ctl_obj:getAttrNumberByIndex(1)
            local w = ctl_obj:getComponentParent():getWidth()
            local h = ctl_obj:getComponentParent():getHeight()
            -- print(rx, ry, w, h)
            -- print("set bkimg angle: "..tostring(self.bk_angle))
            ctl_obj:setRotateByPos(self.bk_angle, rx, ry, w/2, h/2)
        end

        if (caption == "compass_indicator") then
            local rx = ctl_obj:getAttrNumberByIndex(0)
            local ry = ctl_obj:getAttrNumberByIndex(1)
            local w = ctl_obj:getComponentParent():getWidth()
            local h = ctl_obj:getComponentParent():getHeight()
            -- print(rx, ry)
            -- print("set indicator angle: "..tostring(self.indicator_angle))
            ctl_obj:setRotateByPos(self.indicator_angle, rx, ry, w/2, h/2)
        end
    end
end



--[[
@api {SET} bk_angle,indicator_angle=obj:getCompassAngle() 获取指南针角度
@apiName getCompassAngle()
@apiGroup CompassGroup
@apiVersion 1.0.0
@apiDescription 获取指南针背景和指针角度

@apiParamExample 示例:
obj = gui:getComponentByName("COMPASS_TEST")
bk_angle,indicator_angle = obj:getCompassAngle()

@apiSuccessExample {number} 成功响应: 
成功获取指针和背景图角度

@apiErrorExample {number} 失败响应:
获取角度与实际不同
]]
function Compass:getCompassAngle()
    local bk_angle = self.bk_angle
    local indicator_angle = self.indicator_angle
    return bk_angle, indicator_angle
end



return Compass
