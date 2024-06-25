--[[
-- 进度条控件方法
@apiDefine SliderGroup slider
--]]
Slider = {}


Slider.min = 0
Slider.max = 100
Slider.step = 1
Slider.movable = false



--[[
@dd
api {SET} obj:setSliderAttribute(tab) 设置进度条属性
@apiName setSliderAttribute()
@apiGroup SliderGroup
@apiVersion 1.0.0
@apiDescription 设置进度条控件属性

@apiParamExample 示例:
暂空

@apiSuccessExample {number} 成功响应: 
暂空

@apiErrorExample {number} 失败响应:
暂空

]]
function Slider:setSliderAttribute(min, max, step, redraw)
    if (not log:checkArgType("setSliderAttribute", 1, min, "number")) then
        return
    end
    if (not log:checkArgType("setSliderAttribute", 2, max, "number")) then
        return
    end
    if (not log:checkArgType("setSliderAttribute", 3, step, "number")) then
        return
    end
    if (redraw ~= nil) then
        if (not log:checkArgType("setSliderAttribute", 4, redraw, "boolean")) then
            return
        end
    else
        redraw = true
    end
    self.min = min
    self.max = max
    self.step = step
    log:d("set slider attribute: "..tostring(min)..", "..tostring(max)..", "..tostring(step)..", redraw: "..tostring(redraw))
end



--[[
@dd
api {SET} obj:getSliderAttribute(tab) 获取进度条属性
@apiName getSliderAttribute() 
@apiGroup SliderGroup
@apiVersion 1.0.0
@apiDescription 获取进度条控件属性

@apiParamExample 示例:
暂空

@apiSuccessExample {number} 成功响应: 
暂空

@apiErrorExample {number} 失败响应:
暂空

]]
function Slider:getSliderAttribute()
    return self.min, self.max, self.step
end



--[[
@api {SET} obj:setSliderPercent(percent[,redraw]) 设置进度条百分比
@apiName setSliderPercent()
@apiGroup SliderGroup
@apiVersion 1.0.0
@apiDescription 设置进度条控件百分比

@apiParam {number} percent 进度条百分比
@apiParam {boolean{true, false}} [redraw] 是否立即刷新，缺省时立即刷新

@apiParamExample 示例:
slider = gui:getComponentByName("SLIDER_TEST")
slider:setSliderPercent(65)

@apiSuccessExample {number} 成功响应: 
进度条控件准确显示指定的百分比

@apiErrorExample {number} 失败响应:
进度条控件显示的百分比与指定不同
]]
function Slider:setSliderPercent(percent, redraw)
    -- 参数类型检查
    if (not log:checkArgType("setSliderPercent", 1, percent, "number")) then
        return
    end
    if (redraw ~= nil) then
        if (not log:checkArgType("setSliderPercent", 2, redraw, "boolean")) then
            return
        end
    else
        redraw = true
    end

    -- 参数范围检查
    if (percent < self.min) or (percent > self.max) then
        log_e("setSliderPercent", "\"percent\" must between "..tostring(self.min) .." to "..tostring(self.max))
        return
    end

    -- 获取子控件列表
    local sub_ctrl = self.obj:getSubControls()
    local sub_table = {}
    for k, v in ipairs(sub_ctrl) do
        local ename = v:getControlEname()
        local caption = v:getControlCaption()
        local ctl_obj = ui:getComponentByName(ename)
        sub_table[caption] = ctl_obj
    end

    if (self._type == "vslider") then
        local slider_h = sub_table["vslider_pic"]:getHeight()
        local half_pic_h = math.ceil(slider_h / 100 * percent)
        local parent_h = self.obj:getHeight() - slider_h
        local slider_y = math.floor(parent_h / 100 * percent)
        sub_table["vslider_left_pic"]:setHeight(slider_y + half_pic_h)
        sub_table["vslider_pic"]:setY(slider_y)
    else
        local slider_w = sub_table["slider_pic"]:getWidth()
        local half_pic_w = math.ceil(slider_w / 100 * percent)
        local parent_w = self.obj:getWidth() - slider_w
        local slider_x = math.floor(parent_w / 100 * percent)
        sub_table["left_pic"]:setWidth(slider_x + half_pic_w)
        sub_table["slider_pic"]:setX(slider_x)
    end
end


--[[
@api {GET} obj:getSliderPercent() 获取进度条百分比
@apiName getSliderPercent()
@apiGroup SliderGroup
@apiVersion 1.0.0
@apiDescription 获取进度条控件百分比

@apiParamExample 示例:
slider = gui:getComponentByName("SLIDER_TEST")
percent = slider"getSliderPercent()

@apiSuccessExample {number} 成功响应: 
准确获取进度条控件显示的百分比

@apiErrorExample {number} 失败响应:
获取进度条控件显示的百分比失败或错误
]]
function Slider:getSliderPercent()
    local sub_ctrl = self.obj:getSubControls()
    local sub_table = {}
    local percent = 0

    -- 获取子控件列表
    for k, v in ipairs(sub_ctrl) do
        local ename = v:getControlEname()
        local caption = v:getControlCaption()
        local ctl_obj = ui:getComponentByName(ename)
        sub_table[caption] = ctl_obj
    end 

    if (self._type == "vslider") then
        local slider_pic = sub_table["vslider_pic"]
        local slider_h = slider_pic:getHeight()
        percent = math.ceil(slider_pic:getY() / (self.obj:getHeight() - slider_h) * 100)
    else
        local slider_pic = sub_table["slider_pic"]
        local slider_w = slider_pic:getWidth()
        percent = math.ceil(slider_pic:getX() / (self.obj:getWidth() - slider_w) * 100)
    end
    return percent
end



--[[
@api {SET} obj:setSliderMove(move) 进度条控件移动
@apiName setSliderMove()
@apiGroup SliderGroup
@apiVersion 1.0.0
@apiDescription 使能进度条控件滑动

@apiParam {boolean{true, false}} move 可滑动标志

@apiParamExample 示例:
slider = gui:getComponentByName("SLIDER_TEST")
slider:setSliderMove(true)      -- 进度条滑块可以滑动

@apiSuccessExample {number} 成功响应: 
进度条控件滑块可以滑动

@apiErrorExample {number} 失败响应:
进度条控件滑块不能滑动
]]
function Slider:setSliderMove(move)
    if move == nil then
        local move = true
    end

    if (not log:checkArgType("setSliderMove", 1, move, "boolean")) then
        return 
    end

    local sub_ctrl = self.obj:getSubControls()
    local sub_table = {}

    -- 获取子控件列表
    for k, v in ipairs(sub_ctrl) do
        local ename = v:getControlEname()
        local caption = v:getControlCaption()
        local ctl_obj = ui:getComponentByName(ename)
        sub_table[caption] = ctl_obj
        -- print(ename, caption)
    end 

    -- 设置可移动
    if (self._type == "vslider") then
        sub_table["vslider_pic"]:setMovableRange(move)
        sub_table["vslider_pic"]:bindMovable(function(x, y)
            sub_table["vslider_left_pic"]:setHeight(y)
        end)
    else
        sub_table["slider_pic"]:setMovableRange(move)
        sub_table["slider_pic"]:bindMovable(function(x, y)
            sub_table["left_pic"]:setWidth(x)
        end)
    end
    self.movable = move
    log:d("set slider control movable: "..tostring(move))
end




--[[
@api {GET} obj:getSliderMove() 获取进度条移动属性
@apiName getSliderMove()
@apiGroup SliderGroup
@apiVersion 1.0.0
@apiDescription 获取进度条控件可移动状态

@apiParamExample 示例:
slider = gui:getComponentByName("SLIDER_TEST")
if not slider:getSliderMove() then
    slider:setSliderMove(true)
end

@apiSuccessExample {number} 成功响应: 
准确获取进度条滑块滑动状态

@apiErrorExample {number} 失败响应:
获取进度条滑块滑动状态错误
]]
function Slider:getSliderMove()
    return self.movable
end




return Slider
