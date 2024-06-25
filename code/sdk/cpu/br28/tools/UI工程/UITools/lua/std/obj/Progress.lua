--[[
-- 圆弧控件方法
@apiDefine ProGroup progress
--]]
Progress = {}

Progress.percent = 0



--[[
@api {SET} obj:setProgressPercent(percent[,redraw]) 圆弧进度条
@apiName setProgressPercent()
@apiGroup ProGroup
@apiVersion 1.0.0
@apiDescription 设置圆弧进度条百分比

@apiParam {number{0 - 100}} percent 圆弧高亮的百分比
@apiParam {boolean{true, false}} [redraw] 是否立即刷新，缺省时立即刷新

@apiParamExample 示例:
progress = gui:getComponentByName("TEST_PROGRESS")
progress:setProgressPercent(50)

@apiSuccessExample {number} 成功响应: 
圆弧绘制50%的高亮部分

@apiErrorExample {number} 失败响应:
圆弧未绘制高亮，或者高亮百分比与设置不同
]]
function Progress:setProgressPercent(percent)
    -- 参数类型为数值
    if (not log:checkArgType("setProgressPercent", 1, percent, "number")) then
        return 
    end

    -- 参数范围 0-100
    if (percent < 0) or (percent > 100) then
        log_e("setProgressPercent", "arg must between 0 to 100")
        return
    end

    -- 获取progressbar_highlight控件
    local pro_sub = {}
    local sub_ctrl = self.obj:getSubControls()
    for k, v in ipairs(sub_ctrl) do
        local caption = v:getControlCaption()
        if (string.find(caption, "progressbar_highlight")) then
            local ename = v:getControlEname()
            pro_sub = ui:getComponentByName(ename)
        end
    end
    -- 如果没有获取到子控件
    if (not pro_sub) then
        log_e("setProgressPercent", "not find sub controls \"progressbar_highlight\"")
        return
    end
    -- 圆心坐标
    local x = pro_sub:getAttrNumberByIndex(0)
    local y = pro_sub:getAttrNumberByIndex(1)
    -- 大小圆半径
    local big_r   = pro_sub:getAttrNumberByIndex(2)
    local small_r = pro_sub:getAttrNumberByIndex(3)
    -- 起始、终止角度
    local start_angle = pro_sub:getAttrNumberByIndex(4)
    local end_angle   = pro_sub:getAttrNumberByIndex(5)
    print(x, y, small_r, big_r, start_angle, end_angle)

    -- 配置圆环信息
    local arc = {}
    arc.x = x
    arc.y = y
    arc.size = big_r - small_r
    arc.r = small_r + math.ceil(arc.size / 2)
    arc.start   = start_angle
    arc.span    = math.ceil((end_angle - start_angle) / 100 * percent)
    arc.color   = "magenta"
    arc.pencap  = "round"
    -- for k, v in pairs(arc) do
        -- print(k, v)
    -- end

    -- 画圆环
    self.obj:clearDraw()
    self.obj:drawArc(arc)

    -- 获取控件的圆心、半径、圆环大小和起始、结束角度
    self.percent = percent
end



--[[
@api {GET} percent=obj:getProgressPercent() 获取圆弧百分比
@apiName getProgressPercent()
@apiGroup ProGroup
@apiVersion 1.0.0
@apiDescription 获取一重圆弧进度条百分比

@apiParamExample 示例:
progress = gui:getComponentByName("TEST_PROGRESS")
percent = progress:getProgressPercent()

@apiSuccessExample {number} 成功响应: 
准确获取圆弧高亮部分的百分比

@apiErrorExample {number} 失败响应:
获取圆弧高亮部分的百分比错误或失败
]]
function Progress:getProgressPercent()
    return self.percent
end



return Progress
