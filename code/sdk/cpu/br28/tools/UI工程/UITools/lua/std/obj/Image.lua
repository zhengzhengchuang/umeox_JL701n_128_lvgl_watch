--[[
-- 图片控件方法
@apiDefine PicGroup 图片控件
--]]
Image = {}



--[[
@api {SET} obj:showImageByIndex(index) 显示图片
@apiName showImageByIndex()
@apiGroup PicGroup
@apiVersion 1.0.0
@apiDescription 显示图片控件列表中第index张图片

@apiParam {number} index 图片索引号
@apiParam {boolean{true, false}} [redraw] 是否立即刷新，缺省时立即刷新

@apiParamExample 示例:
pic = gui:getComponentByName("PIC_TEST")
pic:showImageByIndex(1)

@apiSuccessExample {number} 成功响应: 
成功显示指定索引图片

@apiErrorExample {number} 失败响应:
显示非索引指定图片
]]
function Image:showImageByIndex(...)
    local arg = {...}
    local arg_num = #arg

    local index = arg[1]
    local redraw = true

    if (not log:checkArgType("showImageByIndex", 1, index, "number")) then
        return 
    end

    if (arg_num == 2) then
        if (not log:checkArgType("showImageByIndex", 2, arg[2], "boolean")) then
            return
        end
        redraw = arg[2]
    end

	local img_num = 0
	if (self._highlight) then
		-- 判断传入参数是否会大于图片数量
		img_num = self.obj:getImageNumber(1)
		if (index >= img_num) then
			log_e('showImageByIndex', 'index max is: '..tostring(img_num-1))
			return
		end
		-- 切换图片
		log:d('ename: [ '..self._ename..' ] show highLight image index: ' .. tostring(index))
		self.obj:showImageByIndex(1, index)
	else
		img_num = self.obj:getImageNumber(0)
		if (index >= img_num) then
			log_e('showImageByIndex', 'index max is: '..tostring(img_num-1))
			return
		end
		log:d('ename: [ '..self._ename..' ] show normal image index: ' .. tostring(index))
		self.obj:showImageByIndex(0, index)
	end
	self._item = index
end



--[[
@api {SET} index=obj:getImageIndex() 获取图片
@apiName getIMageIndex()
@apiGroup PicGroup
@apiVersion 1.0.0
@apiDescription 获取图片控件当前显示的图片索引

@apiParamExample 示例:
pic = gui:getComponentByName("PIC_TEST")
index = pic:getImageIndex()

@apiSuccessExample {number} 成功响应: 
获取正在显示的图片索引与该图片实际索引相同

@apiErrorExample {number} 失败响应:
获取正在显示的图片索引与该图片实际索引不符
]]
function Image:getImageIndex()
	local index = 0
	if (self._highlight) then
		index =  self.obj:getImageIndex(1)
		log:d('ename: [ '..self._ename..' ] get highLight image index: ' .. tostring(index))
	else
		index =  self.obj:getImageIndex(0)
		log:d('ename: [ '..self._ename..' ] get normal image index: ' .. tostring(index))
	end
	return index
end



--[[
@api {SET} number=obj:getNormalNumber() 获取普通图片数量
@apiName getNormalNumber()
@apiGroup PicGroup
@apiVersion 1.0.0
@apiDescription 获取图片控件的普通图片数量

@apiParamExample 示例:
pic = gui:getComponentByName("PIC_TEST")
pic_number = pic:getNormalNumber()

@apiSuccessExample {number} 成功响应: 
成功获取普通图片数量

@apiErrorExample {number} 失败响应:
获取图片数量错误
]]
function Image:getNormalNumber()
	local number = self.obj:getImageNumber(0)
	log:d('ename: [ '..self._ename..' ] getHighLightNumber: ' .. tostring(number))
	return number
end



--[[
@api {SET} number=obj:getHighLightNumber() 获取高亮图片数量
@apiName getHighLightNumber()
@apiGroup PicGroup
@apiVersion 1.0.0
@apiDescription 获取图片控件中高亮图片列表的图片数量

@apiParamExample 示例:
pic = gui:getComponentByName("PIC_TEST")
highLight_number = pic:getHighLightNumber()

@apiSuccessExample {number} 成功响应: 
成功获取高亮图片数量

@apiErrorExample {number} 失败响应:
获取高亮图片数量错误
]]
function Image:getHighLightNumber()
	local number = self.obj:getImageNumber(1)
	log:d('ename: [ '..self._ename..' ] getHighLightNumber: ' .. tostring(number))
	return number
end



--[[
@api {SET} obj:setDrawImage(filename) 绘制图片
@apiName setDrawImage()
@apiGroup PicGroup
@apiVersion 1.0.0
@apiDescription 自定义图片控件显示指定图片

@apiParam {string} filename 图片路径

@apiParamExample 示例:
pic = gui:getComponentByName("PIC_TEST")
pic:setDrawImage("C:test.png")

@apiSuccessExample {number} 成功响应: 
成功显示指定图片

@apiErrorExample {number} 失败响应:
未成功显示指定图片
]]
function Image:setDrawImage(fileName)
    self.obj:setBackgroundImage(fileName)
end



return Image
