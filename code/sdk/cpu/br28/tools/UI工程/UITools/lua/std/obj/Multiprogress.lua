--[[
-- 多重圆弧控件方法
@apiDefine MulproGroup multiprogress
--]]
Multiprogress = {}

Multiprogress.fp = 0    -- first percent
Multiprogress.sp = 0    -- second percent
Multiprogress.tp = 0    -- third percent



--[[
@api {SET} obj:setFirstPercent(percent[,redraw]) 第一圆弧百分比
@apiName setFirstPercent()
@apiGroup MulproGroup
@apiVersion 1.0.0
@apiDescription 设置多重圆弧控件第一圆弧的百分比

@apiParam {number{0 - 100}} percent 圆弧高亮的百分比
@apiParam {boolean{true, false}} [redraw] 是否立即刷新，缺省时立即刷新

@apiParamExample 示例:
mul_progress = gui:getComponentByName("MULTIPROGRESS_TEST")
mul_progress:setFirstPercent(50)    -- 第一个圆弧绘制50%

@apiSuccessExample {number} 成功响应: 
第一个圆弧绘制50%

@apiErrorExample {number} 失败响应:
第一个圆弧未绘制，或者绘制百分比与指定不同
]]
function Multiprogress:setFirstPercent(percent, redraw)
    -- 参数类型为数值
    if (not log:checkArgType("setFirstPercent", 1, percent, "number")) then
        return 
    end
    -- 参数范围 0-100
    if (percent < 0) or (percent > 100) then
        log_e("setFirstPercent", "arg must between 0 to 100")
        return
    end

    -- 获取progressbar_highlight控件
    local pro_sub = {}
    local sub_ctrl = self.obj:getSubControls()
    for k, v in ipairs(sub_ctrl) do
        local caption = v:getControlCaption()
        if (string.find(caption, "multiprogressbar_highlight")) then
            local ename = v:getControlEname()
            pro_sub = ui:getComponentByName(ename)
        end
    end
    -- 如果没有获取到子控件
    if (not pro_sub) then
        log_e("setProgressPercent", "not find sub controls \"progressbar_highlight\"")
        return
    end

    -- 圆环个数
    local round_num = pro_sub:getAttrNumberByIndex(0)

    -- 圆心坐标
    local x = pro_sub:getAttrNumberByIndex(1)
    local y = pro_sub:getAttrNumberByIndex(2)

    -- 圆环个数至少1个，大小圆半径
    local big_r = 0
    local small_r = 0
    if (round_num >= 1) then
        big_r   = pro_sub:getAttrNumberByIndex(3)
        small_r = pro_sub:getAttrNumberByIndex(4)
    else 
        log_e("setFirstPercent", "round number is: "..tostring(round_num))
        return
    end

    -- 起始、终止角度
    local start_angle = pro_sub:getAttrNumberByIndex(9)
    local end_angle   = pro_sub:getAttrNumberByIndex(10)
    print(round_num, x, y, small_r, big_r, start_angle, end_angle, percent)

    -- 配置圆环信息
    local arc = {}
    arc.x = x
    arc.y = x--y
    arc.size = big_r - small_r
    arc.r = small_r + math.ceil(arc.size / 2)
    arc.start   = start_angle
    arc.span    = math.ceil((end_angle - start_angle) / 100 * percent)
    arc.color   = "deepskyblue"
    arc.pencap  = "round"
    -- for k, v in pairs(arc) do
        -- print(k, v)
    -- end

    -- 画圆环
    self.obj:clearDraw()
    if (self.secondArc) then
        self.obj:drawArc(self.secondArc)
    end
    if (self.thirdArc) then
        self.obj:drawArc(self.thirdArc)
    end
    self.obj:drawArc(arc)

    self.firstArc = arc
    self.fp = percent
end



--[[
@api {SET} obj:setSecondPercent(percent[,redraw]) 第二圆弧百分比
@apiName setSecondPercent()
@apiGroup MulproGroup
@apiVersion 1.0.0
@apiDescription 设置多重圆弧第二圆弧百分比

@apiParam {number{0 - 100}} percent 圆弧高亮的百分比
@apiParam {boolean{true, false}} [redraw] 是否立即刷新，缺省时立即刷新

@apiParamExample 示例:
mul_progress = gui:getComponentByName("MULTIPROGRESS_TEST")
mul_progress:setSecondPercent(30)   -- 第二个圆弧绘制30%

@apiSuccessExample {number} 成功响应: 
第二个圆弧成功绘制30%

@apiErrorExample {number} 失败响应:
第一个圆弧未绘制，或者绘制百分比与指定不同
]]
function Multiprogress:setSecondPercent(percent, redraw)
    -- 参数类型为数值
    if (not log:checkArgType("setSecondPercent", 1, percent, "number")) then
        return 
    end
    -- 参数范围 0-100
    if (percent < 0) or (percent > 100) then
        log_e("setSecondPercent", "arg must between 0 to 100")
        return
    end

    -- 获取progressbar_highlight控件
    local pro_sub = {}
    local sub_ctrl = self.obj:getSubControls()
    for k, v in ipairs(sub_ctrl) do
        local caption = v:getControlCaption()
        if (string.find(caption, "multiprogressbar_highlight")) then
            local ename = v:getControlEname()
            pro_sub = ui:getComponentByName(ename)
        end
    end
    -- 如果没有获取到子控件
    if (not pro_sub) then
        log_e("setProgressPercent", "not find sub controls \"progressbar_highlight\"")
        return
    end

    -- 圆环个数
    local round_num = pro_sub:getAttrNumberByIndex(0)

    -- 圆心坐标
    local x = pro_sub:getAttrNumberByIndex(1)
    local y = pro_sub:getAttrNumberByIndex(2)

    -- 圆环个数至少1个，大小圆半径
    local big_r = 0
    local small_r = 0
    if (round_num >= 2) then
        big_r   = pro_sub:getAttrNumberByIndex(5)
        small_r = pro_sub:getAttrNumberByIndex(6)
    else 
        log_e("setSecondPercent", "round number is: "..tostring(round_num))
        return
    end

    -- 起始、终止角度
    local start_angle = pro_sub:getAttrNumberByIndex(9)
    local end_angle   = pro_sub:getAttrNumberByIndex(10)
    print(round_num, x, y, small_r, big_r, start_angle, end_angle, percent)

    -- 配置圆环信息
    local arc = {}
    arc.x = x
    arc.y = x--y
    arc.size = big_r - small_r
    arc.r = small_r + math.ceil(arc.size / 2)
    arc.start   = start_angle
    arc.span    = math.ceil((end_angle - start_angle) / 100 * percent)

    arc.color   = "springgreen"
    arc.pencap  = "round"
    for k, v in pairs(arc) do
        print(k, v)
    end

    -- 画圆环
    self.obj:clearDraw()
    if (self.firstArc) then
        self.obj:drawArc(self.firstArc)
    end
    if (self.thirdArc) then
        self.obj:drawArc(self.thirdArc)
    end
    self.obj:drawArc(arc)

    self.secondArc = arc
    self.sp = percent
end



--[[
@api {SET} obj:setThirdPercent(percent[,redraw]) 第三圆弧百分比
@apiName setThirdPercent()
@apiGroup MulproGroup
@apiVersion 1.0.0
@apiDescription 设置第三圆弧的百分比

@apiParam {number{0 - 100}} percent 圆弧高亮的百分比
@apiParam {boolean{true, false}} [redraw] 是否立即刷新，缺省时立即刷新

@apiParamExample 示例:
mul_progress = gui:getComponentByName("MULTIPROGRESS_TEST")
mul_progress:setThirdPercent(10)    -- 第三个圆弧绘制10%

@apiSuccessExample {number} 成功响应: 
第三个圆弧成功绘制10%

@apiErrorExample {number} 失败响应:
第三个圆弧未绘制，或者绘制百分比与指定不同
]]
function Multiprogress:setThirdPercent(percent, redraw)
    -- 参数类型为数值
    if (not log:checkArgType("setThirdPercent", 1, percent, "number")) then
        return 
    end
    -- 参数范围 0-100
    if (percent < 0) or (percent > 100) then
        log_e("setThirdPercent", "arg must between 0 to 100")
        return
    end

    -- 获取progressbar_highlight控件
    local pro_sub = {}
    local sub_ctrl = self.obj:getSubControls()
    for k, v in ipairs(sub_ctrl) do
        local caption = v:getControlCaption()
        if (string.find(caption, "multiprogressbar_highlight")) then
            local ename = v:getControlEname()
            pro_sub = ui:getComponentByName(ename)
        end
    end
    -- 如果没有获取到子控件
    if (not pro_sub) then
        log_e("setProgressPercent", "not find sub controls \"progressbar_highlight\"")
        return
    end

    -- 圆环个数
    local round_num = pro_sub:getAttrNumberByIndex(0)

    -- 圆心坐标
    local x = pro_sub:getAttrNumberByIndex(1)
    local y = pro_sub:getAttrNumberByIndex(2)

    -- 圆环个数至少1个，大小圆半径
    local big_r = 0
    local small_r = 0
    if (round_num >= 3) then
        big_r   = pro_sub:getAttrNumberByIndex(7)
        small_r = pro_sub:getAttrNumberByIndex(8)
    else 
        log_e("setThirdPercent", "round number is: "..tostring(round_num))
        return
    end

    -- 起始、终止角度
    local start_angle = pro_sub:getAttrNumberByIndex(9)
    local end_angle   = pro_sub:getAttrNumberByIndex(10)
    print(round_num, x, y, small_r, big_r, start_angle, end_angle, percent)

    -- 配置圆环信息
    local arc = {}
    arc.x = x
    arc.y = x--y
    arc.size = big_r - small_r
    arc.r = small_r + math.ceil(arc.size / 2)
    arc.start   = start_angle
    arc.span    = math.ceil((end_angle - start_angle) / 100 * percent)

    arc.color   = "deeppink"
    arc.pencap  = "round"
    -- for k, v in pairs(arc) do
        -- print(k, v)
    -- end

    -- 画圆环
    self.obj:clearDraw()
    if (self.firstArc) then
        self.obj:drawArc(self.firstArc)
    end
    if (self.secondArc) then
        self.obj:drawArc(self.secondArc)
    end
    self.obj:drawArc(arc)

    self.thirdArc = arc
    self.tp = percent
end



--[[
@api {GET} percent=obj:getFirstPercent() 获取第一圆弧百分比
@apiName getFirstPercent()
@apiGroup MulproGroup
@apiVersion 1.0.0
@apiDescription 获取多重圆弧的第一圆弧百分比

@apiParamExample 示例:
mul_progress = gui:getComponentByName("MULTIPROGRESS_TEST")
first_percent = mul_progress:getFirstPercent()

@apiSuccessExample {number} 成功响应: 
获取到百分比与第一个圆弧绘制的百分比一致

@apiErrorExample {number} 失败响应:
获取到百分比与第一个圆弧绘制的百分比不一致
]]
function Multiprogress:getFirstPercent()
    return self.fp
end



--[[
@api {GET} percent=obj:getSecondPercent() 获取第二圆弧百分比
@apiName getSecondPercent()
@apiGroup MulproGroup
@apiVersion 1.0.0
@apiDescription 获取多重圆弧的第二圆弧百分比

@apiParamExample 示例:
mul_progress = gui:getComponentByName("MULTIPROGRESS_TEST")
second_percent = mul_progress:getSecondPercent()

@apiSuccessExample {number} 成功响应: 
获取到百分比与第二个圆弧绘制的百分比一致

@apiErrorExample {number} 失败响应:
获取到百分比与第二个圆弧绘制的百分比不一致
]]
function Multiprogress:getSecondPercent()
    return self.sp
end



--[[
@api {GET} percent=obj:getThirdPercent() 获取第三进度条百分比
@apiName getThirdPercent()
@apiGroup MulproGroup
@apiVersion 1.0.0
@apiDescription 获取多重进度条第三进度条百分比

@apiParamExample 示例:
mul_progress = gui:getComponentByName("MULTIPROGRESS_TEST")
third_percent = mul_progress:getThirdPercent()

@apiSuccessExample {number} 成功响应: 
获取到百分比与第三个圆弧绘制的百分比一致

@apiErrorExample {number} 失败响应:
获取到百分比与第三个圆弧绘制的百分比不一致
]]
function Multiprogress:getThirdPercent()
    return self.tp
end




return Multiprogress
