--[[
-- bsp 芯片外设库
@apiDefine BspGroup bsp
--]]

bsp = {}


-- 打印错误信息
local function log_e(func, msg)
	log:e('function " '..func..'", '..msg)
end

-- 打印警告信息
local function log_w(func, msg)
	log:w('function " '..func..'", '..msg)
end

-- 打印调试信息
local function gpio_log(func, msg)
    local log_msg = "GPIO simulation : function \" "..func.."\", "..msg
	log:d(log_msg)
end


-- 打印参数参考
local function log_refer(msg)
	log:i('>> please refer to : '..msg)
end



--[[
-- gpio 库
@apiDefine gpioGroup gpio
]]
Gpio = {}


--[[
@api {SET} gpio:setOutput() gpio输出模式
@apiName setOutput()
@apiGroup BspGroup
@apiVersion 1.0.0
@apiDescription 设置指定GPIO对象的工作模式为输出模式，此时不能读取GPIO的输入电平。

@apiParamExample 示例:
gpio = bsp:getGpioControl("A", 5)   -- 创建PA5为操作对象
gpio:setOutput()    -- PA5设置为输出模式
gpio:outputHigh()   -- PA5输出高电平

@apiSuccessExample {number} 成功响应: 
PA5 可以控制输出高/低电平

@apiErrorExample {number} 失败响应:
PA5 不可控制输出高/低电平
]]
function Gpio:setOutput(mode)
	gpio_log("setOutput", self._name..' --> setOutput')
end



--[[
@api {SET} gpio:outputHigh() gpio输出高
@apiName outputHigh()
@apiGroup BspGroup
@apiVersion 1.0.0
@apiDescription 设置指定GPIO对象输出高电平

@apiParamExample 示例:
gpio = bsp:getGpioControl("A", 5)   -- 创建PA5为操作对象
gpio:outputHigh()    -- PA5设置输出高电平

@apiSuccessExample {number} 成功响应: 
PA5 可以检测到高电平

@apiErrorExample {number} 失败响应:
PA5 检测到非高电平
]]
function Gpio:outputHigh()
	gpio_log("outputHigh", self._name..' --> outputHigh')
end



--[[
@api {SET} gpio:outputLow() gpio输出低
@apiName outputLow()
@apiGroup BspGroup
@apiVersion 1.0.0
@apiDescription 设置指定GPIO对象输出低电平

@apiParamExample 示例:
gpio = bsp:getGpioControl("A", 5)   -- 创建PA5为操作对象
gpio:outputLow()    -- PA5设置输出低电平

@apiSuccessExample {number} 成功响应: 
PA5 可以检测到低电平

@apiErrorExample {number} 失败响应:
PA5 检测到非低电平
]]
function Gpio:outputLow()
	gpio_log("outputLow", self._name..' --> outputLow')
end



--[[
@api {GET} gpio:setInput() gpio输入模式
@apiName setInput()
@apiGroup BspGroup
@apiVersion 1.0.0
@apiDescription 设置指定GPIO对象为输入模式

@apiParamExample 示例:
gpio = bsp:getGpioControl("A", 5)   -- 创建PA5为操作对象
gpio:setInput()    -- PA5设置为输入模式

@apiSuccessExample {number} 成功响应: 
PA5 可以正确读取输入电平

@apiErrorExample {number} 失败响应:
PA5 读取到PA5电平错误
]]
function Gpio:setInput()
	gpio_log("setInput", self._name..' --> setInput')
end



--[[
@api {GET} gpio:read() gpio读取
@apiName read()
@apiGroup BspGroup
@apiVersion 1.0.0
@apiDescription 读取指定GPIO对象的输入电平

@apiParamExample 示例:
gpio = bsp:getGpioControl("A", 5)   -- 创建PA5为操作对象
gpio:read()    -- PA5设置为输入模式

@apiSuccess (返回值) {number} 1 读取到高电平
@apiSuccess (返回值) {number} 0 读取到低电平

@apiSuccessExample {number} 成功响应: 
PA5 读取的电平值与输入一致

@apiErrorExample {number} 失败响应:
PA5 读取的电平值与输入不一致
]]
function Gpio:read()
	gpio_log("read", self._name..' --> read')
	return true
end



--[[
@api {SET} gpio:setPullUp(onoff) gpio上拉
@apiName setPullUp(onoff)
@apiGroup BspGroup
@apiVersion 1.0.0
@apiDescription 设置指定GPIO对象的上拉电阻开关

@apiParam {number{0, 1}} onoff GPIO的上拉电阻开关标志

@apiParamExample 示例:
gpio = bsp:getGpioControl("A", 5)   -- 创建PA5为操作对象
gpio:setPullUp(1)    -- PA5上拉电阻打开
gpio:setPullUp(0)    -- PA5上拉电阻关闭

@apiSuccessExample {number} 成功响应: 
PA5 上拉电阻成功打开或关闭

@apiErrorExample {number} 失败响应:
PA5 上拉电阻无法打开或关闭
]]
function Gpio:setPullUp(onoff)
	if (not log:checkArgType('setPullUp', 1, onoff, 'number')) then
		return
	end
	gpio_log("setPullUp", self._name..' --> setPullUp: '..tostring(onoff))
end



--[[
@api {SET} gpio:setPullDown(onoff) gpio下拉
@apiName setPullDown()
@apiGroup BspGroup
@apiVersion 1.0.0
@apiDescription 设置指定GPIO对象的下拉电阻开关

@apiParam {number{0, 1}} onoff GPIO的下拉电阻开关标志

@apiParamExample 示例:
gpio = bsp:getGpioControl("A", 5)   -- 创建PA5为操作对象
gpio:setPullDown(1)    -- PA5下拉电阻打开
gpio:setPullDown(0)    -- PA5下拉电阻关闭

@apiSuccessExample {number} 成功响应: 
PA5 下拉电阻成功打开或关闭

@apiErrorExample {number} 失败响应:
PA5 下拉电阻无法打开或关闭
]]
function Gpio:setPullDown(onoff)
	if (not log:checkArgType('setPullDown', 1, onoff, 'number')) then
		return
	end
	gpio_log("setPullDown", self._name..' --> setPullDown: '..tostring(onoff))
end



--[[
@api {SET} gpio:setDie() gpio die控制
@apiName setDie()
@apiGroup BspGroup
@apiVersion 1.0.0
@apiDescription 设置指定GPIO对象的die功能

@apiParamExample 示例:
gpio = bsp:getGpioControl("A", 5)   -- 创建PA5为操作对象
gpio:setDie()    

@apiSuccessExample {number} 成功响应: 
PA5 die功能正常

@apiErrorExample {number} 失败响应:
PA5 die功能异常
]]
function Gpio:setDie()
	gpio_log("setDie", self._name..' --> setDie')
end



--[[
@api {GET} gpio=bsp:getGpioCOntrol(grop,prot) gpio对象
@apiName getGpioControl()
@apiGroup BspGroup
@apiVersion 1.0.0
@apiDescription 创建指定gpio为操作对象

@apiParam {string{A-G}} grop gpio组，不同芯片范围可能略有不同
@apiParam {number{0-15}} prot gpio号，如果一组IO全部引出，则IO号为0到15，具体看封装

@apiParamExample 示例:
gpio = bsp:getGpioControl("A", 5)   -- 创建PA5为操作对象

@apiSuccessExample {number} 成功响应: 
gpio控制方法可以正常调用
PA5 可以被正确控制

@apiErrorExample {number} 失败响应:
gpio控制方法调用出现 call nil等错误
PA5 不能被正确控制
]]
function bsp:getGpioControl(grop, port)
	-- 参数类型检查
	if (not log:checkArgType('getGpioControl', 1, grop, 'string')) then
		return nil
	end
	if (not log:checkArgType('getGpioControl', 2, port, 'number')) then
		return nil
	end
	-- 参数范围判断
	if (grop < 'A' or grop > 'G') then
		log_e('getGpioControl', 'arg 1 error, must between A to G')
		log_refer('\'A\', \'B\', \'C\', etc')
		return nil
	end
	if (port > 15) then
		log_e('getGpioControl', 'arg 2 must not greater then 16')
		return nil
	end
	-- 产生实例
	local self = {}
	gpio = (string.byte(grop) - string.byte('A')) * 16 + port
	if (port < 10) then
		name = 'IO_PORT'..grop..'_0'..port
	else
		name = 'IO_PORT'..grop..'_'..port
	end
	print('create GPIO simulation : '..name..', '..gpio)
	self._gpio = gpio
	self._name = name
	setmetatable(self, {__index = Gpio})
	return self
end





--[[
-- iic 库
@apiDefine iicGroup iic
]]
iic = {}


--[[
@api {SET} iic:setBaud(baud) iic波特率
@apiName setBaud()
@apiGroup iicGroup
@apiVersion 1.0.0
@apiDescription 设置IIC设备的波特率

@apiParam {number} baud 设置的iic通信波特率

@apiParamExample 示例:
iic0 = bsp:getIICControl(DB.IIC0)    -- 创建IIC0对象
iic0:setBaud(300000)    -- 设置波特率为300K

@apiSuccessExample {number} 成功响应: 
iic0 通信波特率被设置为300K

@apiErrorExample {number} 失败响应:
iic0 通信波特率不等于300K
]]
function iic:setBaud(baud)
    if not log:checkArgType("setBaud", 1, baud, "number") then
        return
    end
    log:d("setBaud, iic:"..tostring(self.iic)..", baud:"..tostring(baud))
end


--[[
@api {SET} iic:start() iic开始
@apiName start()
@apiGroup iicGroup
@apiVersion 1.0.0
@apiDescription 开始IIC通信

@apiParamExample 示例:
iic0 = bsp:getIICControl(DB.IIC0)    -- 创建IIC0对象
iic0:start()

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function iic:start()
    log:d("start, iic:"..tostring(self.iic))
end


--[[
@api {SET} iic:stop() iic停止
@apiName stop()
@apiGroup iicGroup
@apiVersion 1.0.0
@apiDescription 停止IIC通信

@apiParamExample 示例:
iic0 = bsp:getIICControl(DB.IIC0)    -- 创建IIC0对象
iic0:stop()     -- 停止iic通信

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function iic:stop()
    log:d("stop, iic:"..tostring(self.iic))
end


--[[
@api {SET} iic:setByte(dat) iic发送
@apiName sendByte()
@apiGroup iicGroup
@apiVersion 1.0.0
@apiDescription IIC发送一个字节

@apiParam {number} dat 待发送的一个字节数据

@apiParamExample 示例:
iic0 = bsp:getIICControl(DB.IIC0)    -- 创建IIC0对象
iic0:sendByte(0xfc)     -- 发送0xfc

@apiSuccessExample {number} 成功响应: 
IIC输出0xfc

@apiErrorExample {number} 失败响应:
IIC无输出，或输出不等于0xfc的值
]]
function iic:sendByte(byte)
    if not log:checkArgType("sendByte", 1, byte, "number") then
        return
    end
    self.pnd = true
    self.endpnd = true
    log:d("sendByte, iic:"..tostring(self.iic)..", dat:"..tostring(byte))
end


--[[
@api {GET} iic:readByte() iic读数据
@apiName readByte()
@apiGroup iicGroup
@apiVersion 1.0.0
@apiDescription IIC读取一个字节数据

@apiParamExample 示例:
iic0 = bsp:getIICControl(DB.IIC0)    -- 创建IIC0对象
byte = iic0:readByte()      -- IIC0读取一个字节
print(byte)     -- 打印读取到的数据

@apiSuccessExample {number} 成功响应: 
读取到预期值

@apiErrorExample {number} 失败响应:
读取到值与预期不符
]]
function iic:readByte()
    log:d("readByte, iic:"..tostring(self.iic))
    self.pnd = true
    self.endpnd = true
    return 0x0f
end


--[[
@api {SET} iic:setIe(enable) iic设置IE
@apiName setIe()
@apiGroup iicGroup
@apiVersion 1.0.0
@apiDescription 设置IIC的IE功能

@apiParam {boolean{true, false}} enable IE功能使能控制

@apiParamExample 示例:
iic0 = bsp:getIICControl(DB.IIC0)    -- 创建IIC0对象
iic0:setIe(true)    -- 打开IE功能

@apiSuccessExample {number} 成功响应: 
IE功能成功打开或关闭

@apiErrorExample {number} 失败响应:
IE功能操作无效
]]
function iic:setIe(enable)
    if not log:checkArgType("setIe", 1, enable, "boolean") then
        return
    end
    log:d("setIe, iic:"..tostring(self.iic)..", en:"..tostring(enable))
end


--[[
@api {GET} iic:getPnd() iic中断
@apiName getPnd()
@apiGroup iicGroup
@apiVersion 1.0.0
@apiDescription 获取IIC中断标志

@apiParamExample 示例:
iic0 = bsp:getIICControl(DB.IIC0)    -- 创建IIC0对象
pnd = iic0:getPnd()     -- 读取中断标志
print(pnd)

@apiSuccessExample {number} 成功响应: 
获取到中断标志

@apiErrorExample {number} 失败响应:
获取到错误中断标志
]]
function iic:getPnd()
    log:d("getPnd, iic:"..tostring(self.iic)..", pnd:"..tostring(self.pnd))
    return self.pnd
end



--[[
@api {SET} iic:clrPnd() iic清除中断
@apiName clrPnd()
@apiGroup iicGroup
@apiVersion 1.0.0
@apiDescription 清除IIC的中断标志

@apiParamExample 示例:
iic0 = bsp:getIICControl(DB.IIC0)    -- 创建IIC0对象
iic0:clrPnd()   -- 清除中断标志

@apiSuccessExample {number} 成功响应: 
成功清除中断标志

@apiErrorExample {number} 失败响应:
中断标志清除失败，数据收发异常
]]
function iic:clrPnd()
    log:d("clrPnd, iic:"..tostring(self.iic))
    self.pnd = false
end



--[[
@api {SET} iic:setEndIe(enable) iic end ie
@apiName setEndIe()
@apiGroup iicGroup
@apiVersion 1.0.0
@apiDescription 设置IIC的end ie功能

@apiParam {boolean{true, false}} enable end ie功能使能控制

@apiParamExample 示例:
iic0 = bsp:getIICControl(DB.IIC0)    -- 创建IIC0对象
iic0:setEndIe(true)     -- 打开 end ie功能

@apiSuccessExample {number} 成功响应: 
END IE功能成功打开或关闭

@apiErrorExample {number} 失败响应:
END IE功能控制无效
]]
function iic:setEndIe(enable)
    if not log:checkArgType("setEndIe", 1, enable, "boolean") then
        return
    end
    log:d("setEndIe, iic:"..tostring(self.iic)..", en:"..tostring(enable))
end


--[[
@api {GET} iic:getEndPnd() iic end pnd
@apiName getEndPnd()
@apiGroup iicGroup
@apiVersion 1.0.0
@apiDescription 获取IIC的end中断标志

@apiParamExample 示例:
iic0 = bsp:getIICControl(DB.IIC0)    -- 创建IIC0对象
endpnd = iic0:getEndPnd()   -- 获取end pnd

@apiSuccessExample {number} 成功响应: 
END PND成功获取

@apiErrorExample {number} 失败响应:
END PND获取失败，数据收发异常
]]
function iic:getEndPnd()
    log:d("getEndPnd, iic:"..tostring(self.iic))
    return self.endpnd
end


--[[
@api {SET} iic:clrEndPnd() iic clr end pnd
@apiName clrEndPnd()
@apiGroup iicGroup
@apiVersion 1.0.0
@apiDescription 清空IIC的end pnd

@apiParamExample 示例:
iic0 = bsp:getIICControl(DB.IIC0)    -- 创建IIC0对象
iic0:clrEndPnd()    -- 清空end pnd

@apiSuccessExample {number} 成功响应: 
END PND成功清除

@apiErrorExample {number} 失败响应:
END PND清除失败，数据收发异常
]]
function iic:clrEndPnd()
    log:d("clrEndPnd, iic:"..tostring(self.iic))
    self.endpnd = false
end


--[[
@api {SET} iic:close() iic关闭
@apiName close()
@apiGroup iicGroup
@apiVersion 1.0.0
@apiDescription 关闭IIC设备

@apiParamExample 示例:
iic0 = bsp:getIICControl(DB.IIC0)    -- 创建IIC0对象
iic0:close()    -- 关闭IIC0

@apiSuccessExample {number} 成功响应: 
IIC 成功关闭

@apiErrorExample {number} 失败响应:
IIC 关闭失败，对应IO被占用
]]
function iic:close()
    log:d("close, iic:"..tostring(self.iic))
end



--[[
@api {GET} iic=bsp:getIICControl(dev) iic对象
@apiName getIICControl()
@apiGroup BspGroup
@apiVersion 1.0.0
@apiDescription 创建IIC对象

@apiParam {number{DB.IIC0, DB.IIC1, DB.IIC2}} dev 选择要获取的IIC设备

@apiParamExample 示例:
iic0 = bsp:getIICControl(DB.IIC0)    -- 创建IIC0对象

@apiSuccessExample {number} 成功响应: 
成功获取IIC实例

@apiErrorExample {number} 失败响应:
获取IIC实例失败，返回对象为nil，访问IIC方法提示“访问nil方法”
]]
function bsp:getIICControl(dev)
    local self = {}
    self.iic = dev
    self.pnd = true
    self.endpnd = true
    log:i("Simulate the creation of iic"..tostring(dev).." equipment. Please test the actual functions in the equipment")
    setmetatable(self, {__index = iic})
    return self
end





--[[
-- spi 库
@apiDefine SpiGroup spi
]]
spi = {}


--[[
@api {SET} spi:setBaud(baud) spi波特率
@apiName setBaud()
@apiGroup SpiGroup
@apiVersion 1.0.0
@apiDescription 设置SPI驱动波特率

@apiParam {number} baud 设置的SPI通信波特率

@apiParamExample 示例:
spi0 = bsp:getSPIControl(DB.SPI0)    -- 创建SPI0对象
spi0:setBaud(500000)

@apiSuccessExample {number} 成功响应: 
SPI 波特率成功设置为500K

@apiErrorExample {number} 失败响应:
SPI 波特率设置无效，或波特率与设置不一致
]]
function spi:setBaud(baud)
    if (not log:checkArgType("spi setBaud", 1, baud, "number")) then
        return
    end
    log:d("setBaud, spi:"..tostring(self.spi)..", baud:"..tostring(baud))
    self.baud = baud
end


--[[
@api {SET} spi:setIe(enable) spi设置IE 
@apiName setIe()
@apiGroup SpiGroup
@apiVersion 1.0.0
@apiDescription 设置SPI的IE功能

@apiParam {boolean{true,false}} enable SPI的IE功能使能控制

@apiParamExample 示例:
spi0 = bsp:getSPIControl(DB.SPI0)    -- 创建SPI0对象
spi0:setIe(true)    -- 打开IE功能

@apiSuccessExample {number} 成功响应: 
SPI IE功能成功打开或关闭

@apiErrorExample {number} 失败响应:
SPI IE功能控制无效
]]
function spi:setIe(enable)
    if (not log:checkArgType("spi setIe", 1, enable, "boolean")) then
        return
    end
    log:d("setIe, spi:"..tostring(self.spi)..", ie:"..tostring(enable))
    self.ie = enable
end


--[[
@api {GET} pnd=spi:getPnd() spi中断
@apiName getPnd()
@apiGroup SpiGroup
@apiVersion 1.0.0
@apiDescription 获取SPI中断标志

@apiParamExample 示例:
spi0 = bsp:getSPIControl(DB.SPI0)    -- 创建SPI0对象
pnd = spi0:getPnd()

@apiSuccessExample {number} 成功响应: 
SPI PND获取正确

@apiErrorExample {number} 失败响应:
SPI PND获取错误
]]
function spi:getPnd()
    log:d("getPnd, spi:"..tostring(self.spi))
    return self.pnd
end


--[[
@api {GET} spi:clrPnd() spi清除中断
@apiName clrPnd()
@apiGroup SpiGroup
@apiVersion 1.0.0
@apiDescription 清除spi的中断标志

@apiParamExample 示例:
spi0 = bsp:getSPIControl(DB.SPI0)    -- 创建SPI0对象
spi0:clrPnd()

@apiSuccessExample {number} 成功响应: 
SPI PND成功清除

@apiErrorExample {number} 失败响应:
SPI PND清除失败，数据通信异常
]]
function spi:clrPnd()
    log:d("clrPnd, spi:"..tostring(self.spi))
    self.pnd = false
end


--[[
@api {GET} spi:setBitMode(mode) spi bit模式
@apiName setBitMode()
@apiGroup SpiGroup
@apiVersion 1.0.0
@apiDescription 设置SPI的工作模式

@apiParam {number{0,1,2,3}} mode SPI的工作模式

@apiParamExample 示例:
spi0 = bsp:getSPIControl(DB.SPI0)    -- 创建SPI0对象
spi0:setBitMode(0)  -- 全双工模式

@apiSuccessExample {number} 成功响应: 
SPI 工作模式与设置一致

@apiErrorExample {number} 失败响应:
SPI 工作模式设置无效，或与设置模式不一致
]]
function spi:setBitMode(mode)
    if not log:checkArgType("spi setBitMode", 1, mode, "number") then
        return
    end

    local mode_tab = {0, 1, 2, 3}
    local correct, index, value = log:checkValue(mode, mode_tab)
    if not correct then
        return
    end

    log:d("setBitMode, spi:"..tostring(self.spi)..", mode:"..tostring(mode))
    self.bitMode = mode
end


--[[
@api {GET} spi:sendByte(dat) spi写数据
@apiName sendByte()
@apiGroup SpiGroup
@apiVersion 1.0.0
@apiDescription SPI发送一个字节

@apiParam {number} dat 待发送的一个字节数据

@apiParamExample 示例:
spi0 = bsp:getSPIControl(DB.SPI0)    -- 创建SPI0对象
spi0:sendByte(0x0e)     -- 发送0x0e

@apiSuccessExample {number} 成功响应: 
SPI 成功发送0x0e

@apiErrorExample {number} 失败响应:
SPI 发送无响应，或者发送值不等于0x0e
]]
function spi:sendByte(byte)
    if not log:checkArgType("spi sendByte", 1, byte, "number") then
        return
    end
    self.pnd = true
    log:d("sendByte, spi:"..tostring(self.spi)..", byte:"..tostring(byte))
end


--[[
@api {GET} dat=spi:readByte() spi读数据
@apiName readByte()
@apiGroup SpiGroup
@apiVersion 1.0.0
@apiDescription SPI读取一个字节数据

@apiParamExample 示例:
spi0 = bsp:getSPIControl(DB.SPI0)    -- 创建SPI0对象
dat = spi0:readByte()   -- SPI0读取一个字节
print(dat)  -- 打印读取到的数据

@apiSuccessExample {number} 成功响应: 
SPI成功读取到预期值

@apiErrorExample {number} 失败响应:
SPI读取到非预期值
]]
function spi:readByte()
    local byte = 0x0f
    log:d("readByte, spi:"..tostring(self.spi)..", byte:"..tostring(byte))
    return byte
end


--[[
@api {GET} spi:close() spi关闭
@apiName close()
@apiGroup SpiGroup
@apiVersion 1.0.0
@apiDescription 关闭SPI设备

@apiParamExample 示例:
spi0 = bsp:getSPIControl(DB.SPI0)    -- 创建SPI0对象
spi0:close()    -- 关闭spi0

@apiSuccessExample {number} 成功响应: 
SPI成功关闭，释放IO占用

@apiErrorExample {number} 失败响应:
SPI关闭失败，占用对应IO，影响IO其它功能
]]
function spi:close()
    log:d("close, spi:"..tostring(self.spi))
end



--[[
@api {GET} spi=bsp:getSPIControl(dev) spi对象
@apiName getSPIControl()
@apiGroup BspGroup
@apiVersion 1.0.0
@apiDescription 创建SPI对象

@apiParam {number{DB.SPI0, DB.SPI1, DB.SPI2}} dev 选择要获取的SPI设备

@apiParamExample {lua} 示例:
spi0 = bsp:getSPIControl(DB.SPI0)    -- 创建SPI0对象

@apiSuccessExample {number} 成功响应: 
成功获取SPI实例

@apiErrorExample {number} 失败响应:
获取SPI实例失败，返回对象为nil
]]
function bsp:getSPIControl(dev)
    local self = {}
    self.spi = dev
    self.pnd = true
    self.endpnd = true
    log:i("Simulate the creation of spi"..tostring(dev).." equipment. Please test the actual functions in the equipment")
    setmetatable(self, {__index = spi})
    return self
end




return bsp



