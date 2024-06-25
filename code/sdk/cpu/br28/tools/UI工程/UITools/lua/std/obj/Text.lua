--[[
-- 文本控件方法
@apiDefine TextGroup 文本控件
--]]
Text = {}

Text.index = 0



--[[
@api {SET} obj:setText(str,redraw,encode,scroll,endian,strlen) 设置文本
@apiName setText()
@apiGroup TextGroup
@apiVersion 1.0.0
@apiPermission 文本控件
@apiDescription 设置文本控件显示指定文字

@apiParam (设置内码) {string} str 待显示字符串
@apiParam (设置内码) {boolean{true, false}} [redraw] 是否立即刷新，缺省时立即刷新

@apiParam (内码和长度) {string} str 待显示字符串
@apiParam (内码和长度) {number} strlen 待显示字符串的长度
@apiParam (内码和长度) {boolean{true, false}} [redraw] 是否立即刷新，缺省时立即刷新

@apiParam (指定编码) {string} str 待显示的字符串
@apiParam (指定编码) {boolean{true,false}} [redraw] 是否立即刷新
@apiParam (指定编码) {number{DB.FONT_ENCODE_ANSI, DB.FONT_ENCODE_UNICODE, DB.FONT_ENCODE_UTF8}} [encode] 字符串编码格式
@apiParam (指定编码) {number} [scroll] 显示模式
@apiParam (指定编码) {number{DB.FONT_ENDIAN_BIG, DB.FONT_ENDIAN_SMALL}} [endian] 编码大/小端控制
@apiParam (指定编码) {number} [strlen] 字符串长度

@apiParamExample 示例:
-- 设置文本控件显示内码
text = gui:getComponentByName("TEXT_TEST")  -- 获取文本控件实例，参数为文本控件名称
text:setText("hello")   -- 设置文本控件显示字符串“hello”，会立即刷新并显示

-- 设置文本控件显示内码，并控制不立即刷新
text = gui:getComponentByName("TEXT_TEST")  -- 获取文本控件实例，参数为文本控件名称
text:setText("hello", false)   -- 设置文本控件显示字符串“hello”，不立即刷新并显示

@apiSuccessExample {number} 成功响应: 
文本控件成功显示指定字符串

@apiErrorExample {number} 失败响应:
文本控件显示指定字符串乱码或无显示
]]
function Text:setText(...)
    -- 参数
    local arg = {...}
    local arg_num = #arg

    -- 字符串
    local text = ""
    -- redraw 标志
    local redraw = true
    -- 编码
    local encode = 0
    -- 滚动
    local scroll = 0
    -- 大小端
    local endian = 0
    -- 字符串长度
    local strlen = -1 

    -- 参数数量检查
    if (arg_num < 1) or (arg_num > 6) then
        log_e("setText", "arg number err!")
        return 
    end

    -- 第一个参数类型检查，必须是字符串
    if (not log:checkArgType("setText", 1, arg[1], "string")) then
        return
    end
    text = arg[1]

    -- 有两个参数，可能第二个参数是redraw或strlen
    if (arg_num == 2) then
        if (not log:checkArgType("setText", 2, arg[2], "boolean", "number")) then
            return
        end
        if (type(arg[2]) == "boolean") then
            redraw = arg[2]
        elseif (type(arg[2]) == "number") then
            strlen = arg[2]
        end
    end

    -- 三个参数，可能(string, strlen, redraw)或(string, redraw, encode)
    if (arg_num == 3) then
        if (type(arg[3]) == "boolean") then
            redraw = arg[3]
            strlen = arg[2]
        elseif (type(arg[3]) == "number") then
            encode = arg[3]
            redraw = arg[2]
        end
    end

    -- 四个参数，可能(string, redraw, encode, scroll)
    if (arg_num >= 4) then
        text = arg[1]
        redraw = arg[2]
        encode = arg[3]
        scroll = arg[4]
        endian = arg[5]
        strlen = arg[6]
    end

    -- 如果有指定字符串长度，那么对字符串进行剪切
    if (strlen ~= -1) and (strlen ~= nil) then
        if (strlen > #text) then
            -- 指定的字符串长度大于字符串本身长度
            log_e("setText", "\""..text.."\" length is: "..tostring(#text)..", less then set text length: "..tostring(strlen))
            return
        end
        -- 剪切字符串
        text = string.sub(text, 1, strlen)
    end

    local msg = tostring(text)..", redraw: "..tostring(redraw)..", encode: "..tostring(encode)..", scroll: "..tostring(scroll)..", endian: "..tostring(endian)..", strlen: "..tostring(strlen)
	log:d('ename: [ '..self._ename..' ] setText: ' .. msg)
	self.obj:setText(text)

    -- 滚动效果
    if (scroll == 0x08) then
        log:d("create scroll timer...")
        local offset = 0
        utils:createTimer(100, function() 
            offset = offset - 10;
            self.obj:setTextOffset(offset)
        end);
    end
end



--[[
@api {GET} string=obj:getText() 获取文本
@apiName getText()
@apiGroup TextGroup
@apiVersion 1.0.0
@apiDescription 获取文本控件显示的文本内容

@apiParamExample 示例:
text = gui:getComponentByName("TEXT_TEST")  -- 获取文本控件实例，参数为文本控件名称
str = text:getText()

@apiSuccessExample {number} 成功响应: 
准确获取文本控件显示的内容

@apiErrorExample {number} 失败响应:
获取文本控件显示的内容错误
]]
function Text:getText()
	local text = self.obj:getText()
	log:d('ename: [ '..self._ename..' ] getText: ' .. text)
	return text
end



--[[
@ddapi
{SET} obj:setUtf8Text(string,strlen,scroll,redraw) UTF-8显示
@apiName setUtf8Text()
@apiGroup TextGroup
@apiVersion 1.0.0
@apiDescription 设置文本控件显示UTF-8编码的字符串

@apiParamExample 示例:
暂空

@apiSuccessExample {number} 成功响应: 
暂空

@apiErrorExample {number} 失败响应:
暂空

]]
function Text:setUtf8Text(...)
    self:setText(...)
end



--[[
@ddapi
{SET} obj:setUnicodeText(string) unicode显示
@apiName setUnicodeText()
@apiGroup TextGroup
@apiVersion 1.0.0
@apiDescription 设置控件显示unicode编码的文本

@apiParamExample 示例:
暂空

@apiSuccessExample {number} 成功响应: 
暂空

@apiErrorExample {number} 失败响应:
暂空

]]
function Text:setUnicodeText(...)
    self:setText(...)
end






--[[
@api {SET} obj:showTextByIndex(index,redraw) 显示文本
@apiName showTextByIndex()
@apiGroup TextGroup
@apiVersion 1.0.0
@apiDescription 设置文本控件显示文本列表指定索引的文本

@apiParam {number} index 待显示文本在文字列表中的索引
@apiParam {boolean{true, false}} [redraw] 是否立即刷新，缺省时立即刷新

@apiParamExample 示例:
text = gui:getComponentByName("TEXT_TEST")
text:showTextByIndex(3)

@apiSuccessExample {number} 成功响应: 
文本控件显示指定文本

@apiErrorExample {number} 失败响应:
文本控件未显示指定文本
]]
function Text:showTextByIndex(index, redraw)
    if (not log:checkArgType("showTextByIndex", 1, index, "number")) then
        return
    end
	log:d('ename: [ '..self._ename..' ] showTextByIndex: ' .. tostring(index)..", redraw: "..tostring(redraw))
    if (redraw == false) then
        return
    end
	self.obj:showTextFontByIndex(index)
	self.index = index
end



--[[
@api {SET} obj:showMultitextByIndex(tab,num[,redraw]) 多行文本
@apiName showMultiTextByIndex()
@apiGroup TextGroup
@apiVersion 1.0.0
@apiDescription 设置文本控件显示多行文本

@apiParam {table} tab 多个文本的索引列表
@apiParam {number} num 需显示的所有文本数量
@apiParam {boolean{true, false}} [redraw] 是否立即刷新，缺省时立即刷新

@apiParamExample 示例:
text = gui:getComponentByName("TEXT_TEST")
local text_list = {1, 2, 3}
text:showMultiTextByIndex(text_list, 3)     -- 显示文本列表第1、2、3共3个文本拼接

@apiSuccessExample {number} 成功响应: 
文本控件显示指定文本拼接内容

@apiErrorExample {number} 失败响应:
文本控件未显示指定文本拼接内容
]]
function Text:showMultiTextByIndex(text_list, number, redraw)
    if (not log:checkArgType("showMultiTextByIndex", 1, text_list, "table")) then
        return
    end
    if (not log:checkArgType("showMultiTextByIndex", 2, number, "number")) then
        return
    end
    if (not log:checkArgType("showMultiTextByIndex", 3, redraw, "nil", "boolean")) then
        return
    end
    -- if (redraw == false) then
        -- return
    -- end
    log:d("showMultiTextByIndex, number:"..tostring(number)..", list:")
    for k, v in ipairs(text_list) do
        print(k, v)
    end

    local week = {}
    for k, v in ipairs(text_list) do
        week[k] = {}
        week[k].font = 0
        week[k].idx = v
    end
    --[[
    for k, v in ipairs(week) do
        print(k, v)
        for i, j in pairs(v) do
            print(i, j)
        end
    end
    ]]
    self.obj:setTextByTextFontList(week)
end



--[[
@api {GET} index=obj:getTextIndex() 获取文本索引
@apiName getTextIndex()
@apiGroup TextGroup
@apiVersion 1.0.0
@apiDescription 获取文本控件当前显示的文本索引

@apiParamExample 示例:
text = gui:getComponentByName("TEXT_TEST")
ind = text:getTextIndex()

@apiSuccessExample {number} 成功响应: 
获取到正确的文本索引

@apiErrorExample {number} 失败响应:
获取到文本索引错误或失败
]]
function Text:getTextIndex()
	-- local index = self.index
	local index = self.obj:getTextFontIndex()
	log:d('ename: [ '..self._ename..' ] getTextIndex: ' .. index)
	return index
end



--[[
@dd
api {GET} format=obj:getTextFormat() 文本格式
@apiName getTextFormat()
@apiGroup TextGroup
@apiVersion 1.0.0
@apiDescription 获取文本控件的文本格式

@apiParamExample 示例:
text = gui:getComponentByName("TEXT_TEST")
format = text:getTextFormat()

@apiSuccessExample {number} 成功响应: 
获取到正确的文本控件编码格式

@apiErrorExample {number} 失败响应:
获取到文本控件编码格式错误
]]
function Text:getTextFormat()
	local forma = self.obj:getTextType()
	log:d('ename: [ '..self._ename..' ] getTextFormat: ' .. forma)
	return forma
end



--[[
@api {GET} number=obj:getTextNumber() 文本数量
@apiName getTextNumber()
@apiGroup TextGroup
@apiVersion 1.0.0
@apiDescription 获取文本控件的文本数量

@apiParamExample 示例:
text = gui:getComponentByName("TEXT_TEST")
text_num = text:getTextNumber()

@apiSuccessExample {number} 成功响应: 
获取到正确的文本列表中文本数量

@apiErrorExample {number} 失败响应:
获取到文本列表中文本数量错误
]]
function Text:getTextNumber()
	local number = self.obj:getTextFontNumber()
	log:d('ename: [ '..self._ename..' ] getTextNumber: ' .. number)
	return number
end




return Text
