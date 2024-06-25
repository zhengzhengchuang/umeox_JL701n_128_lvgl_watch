--[[
-- 所有控件公共方法
@apiDefine ObjGroup 公共方法
--]]
obj = {}


-- 检查参数类型
local function check_type(func, arg, num, typ)
    return log:checkArgType(func, num, arg, typ)
end



--[[
@api {SET} obj:show(redraw) 显示控件
@apiName show()
@apiGroup ObjGroup
@apiVersion 1.0.0
@apiDescription 显示控件

@apiParam {boolean{true, false}} [redraw] 是否立即刷新，缺省时立即刷新

@apiParamExample 示例:
obj = gui:getComponentByName("TEST_CONTROL")
obj:show()

@apiSuccessExample {number} 成功响应: 
控件由隐藏状态转为显示状态

@apiErrorExample {number} 失败响应:
控件未由隐藏状态转为显示状态
]]
function obj:show(redraw)
	log:d('show control: [ '..self._ename..' ], type: '..self._type..", redraw: "..tostring(redraw))
    if (not redraw) and (redraw ~= nil) then
        -- 如果传入false，说明不刷新
        return
    end
	self.obj:show()
end



--[[
@api {SET} obj:hide(redraw) 隐藏控件
@apiName hide()
@apiGroup ObjGroup
@apiVersion 1.0.0
@apiDescription 隐藏控件

@apiParam {boolean{true, false}} [redraw] 是否立即刷新，缺省时立即刷新

@apiParamExample 示例:
obj = gui:getComponentByName("TEST_CONTROL")
obj:hide()

@apiSuccessExample {number} 成功响应: 
控件由显示状态转为隐藏状态

@apiErrorExample {number} 失败响应:
控件未由显示状态转为隐藏状态
]]
function obj:hide(redraw)
	log:d('hide control: [ '..self._ename..' ], type: '..self._type..", redraw: "..tostring(redraw))
    if (not redraw) and (redraw ~= nil) then
        -- 如果传入false，说明不刷新
        return
    end
	self.obj:hide()
end



--[[
@api {GET} obj:isHide() 是否隐藏
@apiName isHide()
@apiGroup ObjGroup
@apiVersion 1.0.0
@apiDescription 获取控件隐藏状态

@apiParamExample 示例:
obj = gui:getComponentByName("TEST_CONTROL")
if (obj:isHide()) then
    obj:show()
else
    obj:hide()

@apiSuccessExample {number} 成功响应: 
控件在隐藏状态返回true，控件在显示状态返回false

@apiErrorExample {number} 失败响应:
返回值与控件状态不一致
]]
function obj:isHide()
    return self.obj:isHide()
end



--[[
@api {SET} obj:highLight(redraw) 高亮控件
@apiName highLight()
@apiGroup ObjGroup
@apiVersion 1.0.0
@apiDescription 设置控件显示高亮属性

@apiParam {boolean{true, false}} [redraw] 是否立即刷新，缺省时立即刷新

@apiParamExample 示例:
obj = gui:getComponentByName("TEST_CONTROL")
obj:highLight()

@apiSuccessExample {number} 成功响应: 
控件由普通状态转为高亮状态

@apiErrorExample {number} 失败响应:
控件未由普通状态转为高亮状态
]]
function obj:highLight(redraw)
	print('highLight control: [ '..self._ename..' ], type: '..self._type)

    if not redraw then
        return
    end

	if (self._type == 'ImageList') then
		print('ename: [ '..self._ename..' ] highLight: '..self._item)
		self.obj:showImageByIndex(1, self._item)
	else
		self.obj:setCss(1)
	end
	self._highlight = true
end



--[[
@api {SET} obj:noHighLight(redraw) 取消高亮
@apiName noHighLight()
@apiGroup ObjGroup
@apiVersion 1.0.0
@apiDescription 取消控件高亮

@apiParam {boolean{true, false}} [redraw] 是否立即刷新，缺省时立即刷新

@apiParamExample 示例:
obj = gui:getComponentByName("TEST_CONTROL")
obj:noHighLight()

@apiSuccessExample {number} 成功响应: 
控件由高亮状态转为普通状态

@apiErrorExample {number} 失败响应:
控件未由高亮状态转为普通状态
]]
function obj:noHighLight(redraw)
	print('noHighLight control: [ '..self._ename..' ], type: '..self._type)

    if not redraw then
        return
    end

	if (self._type == 'ImageList') then
		print('ename: [ '..self._ename..' ] noHighLight: '..self._item)
		self.obj:showImageByIndex(0, self._item)
	else
		self.obj:setCss(0)
	end
	self._highlight = false
end



--[[
@api {GET} obj:isHighLight() 高亮状态
@apiName isHighLight()
@apiGroup ObjGroup
@apiVersion 1.0.0
@apiDescription 获取控件高亮状态

@apiParamExample 示例:
obj = gui:getComponentByName("TEST_CONTROL")
if (obj:isHighLight()) then
    obj:noHighLight()
else
    obj:highLight()

@apiSuccessExample {number} 成功响应: 
控件高亮状态返回true，控件非高亮状态返回false

@apiErrorExample {number} 失败响应:
返回值与控件是否高亮状态不一致
]]
function obj:isHighLight()
	return self._highlight
end



--[[
@api {SET} obj:redrawSelf() 刷新控件
@apiName redrawSelf()
@apiGroup ObjGroup
@apiVersion 1.0.0
@apiDescription 刷新控件自身

@apiParamExample 示例:
obj = gui:getComponentByName("TEST_CONTROL")
obj:redrawSelf()

@apiSuccessExample {number} 成功响应: 
控件刷新自身

@apiErrorExample {number} 失败响应:
控件未刷新自身
]]
function obj:redrawSelf()
    local parent = self.obj:getComponentParent()
    parent:hide()
    parent:show()
end




--[[
@notes
@tip:绑定 touch down 事件回调
@demo:obj:bindTouchDown(function () print("touch down event") end)
]]
function obj:bindTouchDown(func)
	if (not check_type('bindTouchDown', func, 1, 'function')) then
		return
	end
	print('bindTouchDown control: [ '..self._ename..' ], type: '..self._type)
	self.obj:bindTouchDown(func)
end




--[[
@notes
@tip:主动触发 touch down 回调
@demo:obj:touchDown()
]]
function obj:touchDown()
	print('ename: [ '..self._ename..' ], touchDown')
	self.obj:touchDown()
end



--[[
@notes
@tip:绑定 touch up 事件回调
@demo:obj:bindTouchUp(function () print("touch up event") end)
]]
function obj:bindTouchUp(func)
	if (not check_type('bindTouchUp', func, 1, 'function')) then
		return
	end
	print('bindTouchUp control: [ '..self._ename..' ], type: '..self._type)
	self.obj:bindTouchUp(func)
end



--[[
@notes
@tip:主动触发 touch up 回调
@demo:obj:touchUp()
]]
function obj:touchUp()
	print('ename: [ '..self._ename..' ], touchUp')
	self.obj:touchUp()
end




--[[
@notes
@tip:绑定 touch up move 事件回调
@demo:obj:bindTouchUMove(function () print("touch up move event") end)
]]
function obj:bindTouchUMove(func)
	if (not check_type('bindTouchUMove', func, 1, 'function')) then
		return
	end
	print('bindTouchUMove control: [ '..self._ename..' ], type: '..self._type)
	self.obj:bindTouchUMove(func)
end




--[[
@notes
@tip:主动触发 touch up move 回调
@demo:obj:touchUMove()
]]
function obj:touchUMove()
	print('ename: [ '..self._ename..' ], touchUMove')
	self.obj:touchUMove()
end



--[[
@notes
@tip:绑定 touch down move 事件回调
@demo:obj:bindTouchDMove(function () print("touch down move event") end)
]]
function obj:bindTouchDMove(func)
	if (not check_type('bindTouchDMove', func, 1, 'function')) then
		return
	end
	print('bindTouchDMove control: [ '..self._ename..' ], type: '..self._type)
	self.obj:bindTouchDMove(func)
end



--[[
@notes
@tip:主动触发 touch down move 回调
@demo:obj:touchDMove()
]]
function obj:touchDMove()
	print('ename: [ '..self._ename..' ], touchDMove')
	self.obj:touchDMove()
end




--[[
@notes
@tip:绑定 touch left move 事件回调
@demo:obj:bindTouchLMove(function () print("touch left move event") end)
]]
function obj:bindTouchLMove(func)
	if (not check_type('bindTouchLMove', func, 1, 'function')) then
		return
	end
	print('bindTouchLMove control: [ '..self._ename..' ], type: '..self._type)
	self.obj:bindTouchLMove(func)
end




--[[
@notes
@tip:主动触发 touch left move 回调
@demo:obj:touchLMove()
]]
function obj:touchLMove()
	print('ename: [ '..self._ename..' ], touchLMove')
	self.obj:touchLMove()
end



--[[
@notes
@tip:绑定 touch right move 事件回调
@demo:obj:bindTouchRMove(function () print("touch right move event") end)
]]
function obj:bindTouchRMove(func)
	if (not check_type('bindTouchRMove', func, 1, 'function')) then
		return
	end
	print('bindTouchRMove control: [ '..self._ename..' ], type: '..self._type)
	self.obj:bindTouchRMove(func)
end




--[[
@notes
@tip:主动触发 touch right move 回调
@demo:obj:touchRMove()
]]
function obj:touchRMove()
	print('ename: [ '..self._ename..' ], touchRMove')
	self.obj:touchRMove()
end



--[[
@api {SET} obj:setRect(x,y,w,h) 设置控件属性
@apiName setRect()
@apiGroup ObjGroup
@apiVersion 1.0.0
@apiDescription 设置控件显示的矩形区域

@apiParamExample 示例:
obj = gui:getComponentByName("TEST_CONTROL")
obj:setRect(0, 0, 30, 30)

@apiSuccessExample {number} 成功响应: 
控件被移动到指定区域显示

@apiErrorExample {number} 失败响应:
控件未显示在指定区域
]]
function obj:setRect(x, y, w, h)
	-- 数据类型判断，x, y, w, h 都应该为number
	if (not check_type('setRect', x, 1, 'number')) then
		return
	end
	if (not check_type('setRect', y, 2, 'number')) then
		return
	end
	if (not check_type('setRect', w, 3, 'number')) then
		return
	end
	if (not check_type('setRect', h, 4, 'number')) then
		return
	end
	print('ename: [ '..self._ename..' ], setRect('..x..', '..y..', '..w..', '..h..')')
	self.obj:setRect(x, y, w, h)
end



--[[
@api {SET} obj:setX(x) 设置X坐标
@apiName setX()
@apiGroup ObjGroup
@apiVersion 1.0.0
@apiDescription 设置控件的X坐标

@apiParam {boolean{true, false}} [redraw] 是否立即刷新，缺省时立即刷新

@apiParamExample 示例:
obj = gui:getComponentByName("TEST_CONTROL")
obj:setX(20)

@apiSuccessExample {number} 成功响应: 
控件X坐标移动到指定位置

@apiErrorExample {number} 失败响应:
控件X坐标位置未改变
]]
function obj:setX(x)
	if (not check_type('setX', x, 1, 'number')) then
		return
	end
	print('ename: [ '..self._ename..' ], setX: '..x)
	self.obj:setX(x)
end



--[[
@api {SET} obj:setY(y) 设置Y坐标
@apiName setY()
@apiGroup ObjGroup
@apiVersion 1.0.0
@apiDescription 设置控件的Y坐标

@apiParam {boolean{true, false}} [redraw] 是否立即刷新，缺省时立即刷新

@apiParamExample 示例:
obj = gui:getComponentByName("TEST_CONTROL")
obj:setY(10)

@apiSuccessExample {number} 成功响应: 
控件Y左边移动到指定位置

@apiErrorExample {number} 失败响应:
控件Y坐标位置未改变
]]
function obj:setY(y)
	if (not check_type('setY', y, 1, 'number')) then
		return
	end
	print('ename: [ '..self._ename..' ], setY: '..y)
	self.obj:setY(y)
end



--[[
@api {SET} obj:setWidth(w) 设置宽度
@apiName setWidth()
@apiGroup ObjGroup
@apiVersion 1.0.0
@apiDescription 设置控件的宽度

@apiParam {boolean{true, false}} [redraw] 是否立即刷新，缺省时立即刷新

@apiParamExample 示例:
obj = gui:getComponentByName("TEST_CONTROL")
obj:setWidth(25)

@apiSuccessExample {number} 成功响应: 
控件宽度显示为设置宽度

@apiErrorExample {number} 失败响应:
控件宽度没有显示设置宽度
]]
function obj:setWidth(w)
	if (not check_type('setWidth', w, 1, 'number')) then
		return
	end
	print('ename: [ '..self._ename..' ], setWidth: '..w)
	self.obj:setWidth(w)
end



--[[
@api {SET} obj:setHeight(h) 设置高度
@apiName setHeight()
@apiGroup ObjGroup
@apiVersion 1.0.0
@apiDescription 设置控件的高度

@apiParam {boolean{true, false}} [redraw] 是否立即刷新，缺省时立即刷新

@apiParamExample 示例:
obj = gui:getComponentByName("TEST_CONTROL")
obj:setHeight(30)

@apiSuccessExample {number} 成功响应: 
控件高度显示为设置高度

@apiErrorExample {number} 失败响应:
控件高度没有显示设置高度
]]
function obj:setHeight(h)
	if (not check_type('setHeight', h, 1, 'number')) then
		return
	end
	print('ename: [ '..self._ename..' ], setHeight: '..h)
	self.obj:setHeight(h)
end



--[[
@api {GET} rect=obj:getRect() 获取矩形
@apiName getRect()
@apiGroup ObjGroup
@apiVersion 1.0.0
@apiDescription 获取控件矩形区域，包含起始X,Y坐标，宽度和高度

@apiParam (返回值) {table} rect 控件的矩形属性
@apiParam (rect) {number} x 控件的X坐标
@apiParam (rect) {number} y 控件的y坐标
@apiParam (rect) {number} width 控件的宽度
@apiParam (rect) {number} height 控件的高度

@apiParamExample 示例:
obj = gui:getComponentByName("TEST_CONTROL")
obj_rect = obj:getRect()

@apiSuccessExample {number} 成功响应: 
成功获取控件的矩形区域属性

@apiErrorExample {number} 失败响应:
获取到控件的矩形区域错误或失败
]]
function obj:getRect()
	local tab = self.obj:getRect()
	print('ename: [ '..self._ename..' ], getRect: x'.. tab.x ..', '.. tab.y ..', '.. tab.width ..', '.. tab.height)
	return tab
end



--[[
@api {GET} x=obj:getX() 获取X坐标
@apiName getX()
@apiGroup ObjGroup
@apiVersion 1.0.0
@apiDescription 获取控件的X坐标值

@apiParamExample 示例:
obj = gui:getComponentByName("TEST_CONTROL")
x = obj:getX()

@apiSuccessExample {number} 成功响应: 
成功获取控件的X坐标

@apiErrorExample {number} 失败响应:
获取控件的X坐标错误或失败
]]
function obj:getX()
	local n = self.obj:getX()
	print('ename: [ '..self._ename..' ], getX: '..n)
	return n
end



--[[
@api {GET} y=obj:getY() 获取Y坐标
@apiName getY()
@apiGroup ObjGroup
@apiVersion 1.0.0
@apiDescription 获取控件的Y坐标值

@apiParamExample 示例:
obj = gui:getComponentByName("TEST_CONTROL")
y = obj:getY()

@apiSuccessExample {number} 成功响应: 
成功获取控件的Y坐标

@apiErrorExample {number} 失败响应:
获取控件的Y坐标错误或失败
]]
function obj:getY()
	local n = self.obj:getY()
	print('ename: [ '..self._ename..' ], getY: '..n)
	return n
end



--[[
@api {GET} w=obj:getWidth() 获取宽度
@apiName getWidth()
@apiGroup ObjGroup
@apiVersion 1.0.0
@apiDescription 获取控件的宽度

@apiParamExample 示例:
obj = gui:getComponentByName("TEST_CONTROL")
w = obj:getWidth()

@apiSuccessExample {number} 成功响应: 
成功获取控件的宽度

@apiErrorExample {number} 失败响应:
获取控件的宽度错误或失败
]]
function obj:getWidth()
	local n = self.obj:getWidth()
	print('ename: [ '..self._ename..' ], getWidth: '..n)
	return n
end



--[[
@api {GET} h=obj:getHeight() 获取高度
@apiName getHeight()
@apiGroup ObjGroup
@apiVersion 1.0.0
@apiDescription 获取控件的高度

@apiParamExample 示例:
obj = gui:getComponentByName("TEST_CONTROL")
h = obj:getHeight()

@apiSuccessExample {number} 成功响应: 
成功获取控件的高度

@apiErrorExample {number} 失败响应:
获取控件的高度错误或失败
]]
function obj:getHeight()
	local n = self.obj:getHeight()
	print('ename: [ '..self._ename..' ], getHeight: '..n)
	return n
end


--[[
function obj:setBackgroundColor(color)
	if (not check_type('setBackgroundColor', color, 1, 'string')) then
		return
	end
	print('ename: [ '..self._ename..' ], setBackgroundColor: '..color)
	self.obj:setBackgroundColor(color)
end
]]




return obj
