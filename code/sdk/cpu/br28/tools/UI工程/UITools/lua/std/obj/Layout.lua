--[[
-- 布局控件方法
@apiDefine LayGroup 布局控件
--]]
Layout = {}



--[[
@api {SET} obj:drawSpot(spot) 画点
@apiName drawSpot()
@apiGroup LayGroup
@apiVersion 1.0.0
@apiDescription 在布局控件上画点

@apiParam {table} spot 点table

@apiParam (spot) {number} x X坐标
@apiParam (spot) {number} y Y坐标
@apiParam (spot) {number} size 点半径
@apiParam (spot) {number} color 点颜色

@apiParamExample 示例:
local spot = {}
spot.x = 10
spot.y = 10
spot.size = 3
spot.color = "red"
lay = gui:getComponentByName("LAY_TEST")
lay:drawSpot(spot)

@apiSuccessExample {number} 成功响应: 
在布局上成功画点

@apiErrorExample {number} 失败响应:
布局上未成功画点
]]
function Layout:drawSpot(object)
    if (not log:checkArgType("drawSpot", 1, object, "table")) then
        return
    end

    if (not log:checkArgType("drawSpot", "x", object.x, "number")) then
        return
    end

    if (not log:checkArgType("drawSpot", "y", object.y, "number")) then
        return
    end


    if (not log:checkArgType("drawSpot", "size", object.size, "number")) then
        return
    end


    if (not log:checkArgType("drawSpot", "color", object.color, "number", "string")) then
        return
    end

	log:d('draw spot: '..'('..object.x..','..object.y..')'..', size = '..object.size..', color = '..object.color)
	self.obj:drawPoint(object)
end



--[[
@api {SET} obj:drawLine(line) 画线
@apiName drawLine()
@apiGroup LayGroup
@apiVersion 1.0.0
@apiDescription 在布局控件上画线

@apiParam {table} line 线table

@apiParam (line) {number} x1 起始X坐标
@apiParam (line) {number} y1 起始Y坐标
@apiParam (line) {number} x2 终止X坐标
@apiParam (line) {number} y2 终止Y坐标
@apiParam (line) {number} size 大小
@apiParam (line) {number} color 颜色

@apiParamExample 示例:
lay = gui:getComponentByName("LAY_TEST")
local line = {}
line.x1 = 0
line.y1 = 0
line.x2 = 50
line.y2 = 50
line.size = 3
line.color = "green"
lay:drawLine(line)

@apiSuccessExample {number} 成功响应: 
布局上成功绘制指定线段

@apiErrorExample {number} 失败响应:
指定布局上未绘制线段
]]
function Layout:drawLine(object)
    if (not log:checkArgType("drawLine", 1, object, "table")) then
        return
    end

    if (not log:checkArgType("drawLine", "x1", object.x1, "number")) then
        return
    end

    if (not log:checkArgType("drawLine", "y1", object.y1, "number")) then
        return
    end

    if (not log:checkArgType("drawLine", "x2", object.x2, "number")) then
        return
    end

    if (not log:checkArgType("drawLine", "y2", object.y2, "number")) then
        return
    end

    if (not log:checkArgType("drawLine", "size", object.size, "number")) then
        return
    end

    if (not log:checkArgType("drawLine", "color", object.color, "number", "string")) then
        return
    end

	log:d('draw line: start ('..object.x1..', '..object.y1..'), end ('..object.x2..', '..object.y2..'), size = '..object.size..', color = '..object.color)
	self.obj:drawLine(object)
end



--[[
@api {SET} obj:drawRect(rect) 画矩形
@apiName drawRect()
@apiGroup LayGroup
@apiVersion 1.0.0
@apiDescription 在布局控件上画矩形

@apiParam {table} rect 矩形table

@apiParam (rect) {number} x 起始X坐标
@apiParam (rect) {number} y 起始Y坐标
@apiParam (rect) {number} w 矩形宽度
@apiParam (rect) {number} h 矩形高度
@apiParam (rect) {number} size 大小
@apiParam (rect) {number} color 颜色

@apiParamExample 示例:
lay = gui:getComponentByName("LAY_TEST")
local rect = {}
rect.x = 10
rect.y = 10
rect.w = 20
rect.h = 20
rect.size = 1
rect.color = "blue"
lay:drawRect(rect)

@apiSuccessExample {number} 成功响应: 
在布局上成功画矩形

@apiErrorExample {number} 失败响应:
布局上未成功画矩形
]]
function Layout:drawRect(object)
    if (not log:checkArgType("drawRect", 1, object, "table")) then
        return
    end

    if (not log:checkArgType("drawRect", "x", object.x, "number")) then
        return
    end

    if (not log:checkArgType("drawRect", "y", object.y, "number")) then
        return
    end

    if (not log:checkArgType("drawRect", "w", object.w, "number")) then
        return
    end

    if (not log:checkArgType("drawRect", "h", object.h, "number")) then
        return
    end

    if (not log:checkArgType("drawRect", "size", object.size, "number")) then
        return
    end

    if (not log:checkArgType("drawRect", "color", object.color, "number", "string")) then
        return
    end

	log:d('draw rect: (x = '..object.x..', y = '..object.y..', w = '..object.w..', h = '..object.h..'), size = '..object.size..', color = '..object.color)
	self.obj:drawRect(object)
end



--[[
@api {SET} obj:drawCircle(circle) 画圆
@apiName drawCircle()
@apiGroup LayGroup
@apiVersion 1.0.0
@apiDescription 在布局控件上画圆

@apiParam {table} circle 源table

@apiParam (circle) {number} x 圆心X坐标
@apiParam (circle) {number} y 圆心Y坐标
@apiParam (circle) {number} r 圆半径
@apiParam (circle) {number} size 大小
@apiParam (circle) {number} color 颜色

@apiParamExample 示例:
lay = gui:getComponentByName("LAY_TEST")
local circle = {}
circle.x = 50
circle.y = 50
circle.r = 20
circle.size = 2
circle.color = "red"
lay:drawCircle(circle)

@apiSuccessExample {number} 成功响应: 
在布局上成功画圆

@apiErrorExample {number} 失败响应:
未在布局上成功画圆
]]
function Layout:drawCircle(object)
    if (not log:checkArgType("drawRect", 1, object, "table")) then
        return
    end

    if (not log:checkArgType("drawRect", "x", object.x, "number")) then
        return
    end

    if (not log:checkArgType("drawRect", "y", object.y, "number")) then
        return
    end

    if (not log:checkArgType("drawRect", "r", object.r, "number")) then
        return
    end

    if (not log:checkArgType("drawRect", "size", object.size, "number")) then
        return
    end

    if (not log:checkArgType("drawRect", "color", object.color, "number", "string")) then
        return
    end

	log:d('draw circle: ('..object.x..', '..object.y..', r = '..object.r..'), size = '..object.size..', color = '..object.color)
	self.obj:drawCircle(object)
end



--[[
@api {SET} obj:clearDraw(tab) 清空绘图
@apiName clearDraw()
@apiGroup LayGroup
@apiVersion 1.0.0
@apiDescription 清空在布局控件上的绘图

@apiParamExample 示例:
lay = gui:getComponentByName("LAY_TEST")
lay:clearDraw()

@apiSuccessExample {number} 成功响应: 
布局上的自定义绘图被清理

@apiErrorExample {number} 失败响应:
布局上的自定义绘图未被清理
]]
function Layout:clearDraw()
	log:d('clear draw by control: '..self._ename..', type: '..self._type)
	self.obj:clearDraw()
end



--[[
@api {SET} obj:setMovable(move) 布局移动
@apiName setMovable()
@apiGroup LayGroup
@apiVersion 1.0.0
@apiDescription 设置控件为可移动

@apiParam {boolean{true, false}} move 移动使能标志

@apiParamExample 示例:
lay = gui:getComponentByName("LAY_TEST")
lay:setMovable(true)

@apiSuccessExample {number} 成功响应: 
布局可以滑动

@apiErrorExample {number} 失败响应:
布局不能滑动
]]
function Layout:setMovable(move)
    self.movable = true
    if (move ~= nil) then
        self.movable = move
    end
    self.obj:setMovable(self.movable)
end



--[[
@api {SET} obj:starMenuInit() 满天星布局
@apiName starMenuInit()
@apiGroup LayGroup
@apiVersion 1.0.0
@apiDescription 初始化为满天星布局

@apiParamExample 示例:
lay = gui:getComponentByName("LAY_TEST")
lay:starMenuInit()

@apiSuccessExample {number} 成功响应: 
满天星功能生效

@apiErrorExample {number} 失败响应:
满天星功能不生效
]]
function Layout:starMenuInit()
    if not self.movable then
        log_e("starMenuInit", "The layout can't move, please \"setMovable\" first")
        return
    end

    local layer = self.obj:getComponentParent(); --父控件图层
    local dw = layer:getWidth() / 2;
    local dh = layer:getHeight() / 2;
    local dl = (dw + dh) / 2; --计算最远距离
    local ctrls = self.obj:getSubControls();
    local count = #ctrls;

    -- 初始设置各个图片的大小
    for i=1, count do
        local ctrl = ctrls[i];
        --计算控件的中心点坐标
        cx = ctrl:getX() + ctrl:getWidth() / 2;
        cy = ctrl:getY() + ctrl:getHeight() / 2;
        --控件相对于layer的距离
        dst = math.sqrt(((cx-dw) * (cx-dw)) + ((cy-dh) * (cy-dh)))
        local bl = 1 - (dst / dl); --缩放比例
        if bl <= 0.0001 then
            bl = 0.1
        end
        if bl >= 1.0 then
            bl = 1;
        end
        ctrl:setBackgroundScale(bl)
    end

    -- 注册滑动回调
    self.obj:bindMovable(function(x, y)
        for i=1, count do
            local ctrl = ctrls[i];
            --计算控件的中心点坐标
            cx = ctrl:getX() + ctrl:getWidth() / 2;
            cy = ctrl:getY() + ctrl:getHeight() / 2;
            --控件相对于layer的距离
            cx = cx + x;
            cy = cy + y;
            dst = math.sqrt(((cx-dw) * (cx-dw)) + ((cy-dh) * (cy-dh)))
            local bl = 1 - (dst / dl); --缩放比例
            if bl <= 0.0001 then
                bl = 0.1
            end
            if bl >= 1.0 then
                bl = 1;
            end
            ctrl:setBackgroundScale(bl)
        end
    end)
    log:d("starMenuInit")
end



--[[
@api {SET} obj:starMenuFree() 释放满天星
@apiName starMenuFree()
@apiGroup LayGroup
@apiVersion 1.0.0
@apiDescription 释放满天星布局

@apiParamExample 示例:
lay = gui:getComponentByName("LAY_TEST")
lay:starMenuFree()

@apiSuccessExample {number} 成功响应: 
释放满天星资源，满天星功能失效

@apiErrorExample {number} 失败响应:
满天星功能不失效
]]
function Layout:starMenuFree()
    if (self.movable) then
        self.movable = false
        self.obj:setMovable(self.movable)
    end
    log:d("starMenuFree")
end



return Layout
