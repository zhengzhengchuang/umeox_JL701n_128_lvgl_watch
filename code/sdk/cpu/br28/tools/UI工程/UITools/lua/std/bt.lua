--[[
@apiDefine BtGroup bt
--]]

bt = {}


-- 蓝牙名称
bt.bluetoothName = "BR28"

-- 蓝牙地址
bt.bluetoothAddr = "40:B3:52:17:08"

-- 蓝牙连接状态，默认未连接
bt.connectState = false

-- 通话状态，默认未通话
bt.callState = false



function log_e(func, msg)
	log:e('function " '..func..'", '..msg)
end

function log_w(func, msg)
	log:w('function " '..func..'", '..msg)
end

function log_refer(msg)
	log:i('>> please refer to : '..msg)
end

function log_bt(fun, msg)
	log:d('< BT simulation > : \" '..fun..'\", --> '..msg)
end


--[[
@api {GET} status=bt:getBluetoothConnectState() 蓝牙连接状态
@apiName getBluetoothConnectState()
@apiGroup BtGroup
@apiVersion 1.0.0
@apiDescription 获取蓝牙连接状态，无论连接手机或耳机，均采用这个接口获取状态

@apiSuccess (返回值) {boolean{true,false}} status 蓝牙连接状态

@apiParamExample 示例:
status = bt:getBluetoothConnectState()
print(tostring(status))

@apiSuccessExample {number} 成功响应: 
正确返回蓝牙连接状态

@apiErrorExample {number} 失败响应:
返回错误状态
]]

--[[
@notes
@tip:获取蓝牙连接状态
@demo:bt_state = bt:getBluetoothConnectState()
]]
function bt:getBluetoothConnectState()
	log_bt('getBluetoothConnectState', 'return state true')
    return self.connectState
end


--[[
@api {GET} status=bt:getCallState() 通话状态
@apiName getCallState()
@apiGroup BtGroup
@apiVersion 1.0.0
@apiDescription 获取手机通话状态

@apiSuccess (返回值) {boolean{true,false}} status 手机通话状态

@apiParamExample 示例:
status = bt:getCallState()
print(tostring(status))

@apiSuccessExample {number} 成功响应: 
正确返回手机通话状态

@apiErrorExample {number} 失败响应:
返回错误手机通话状态
]]

--[[
@notes
@tip:获取手机通话状态
@demo:state = bt:getCallState()
]]
function bt:getCallState()
	log_bt('getCallState', 'return state true')
    return self.callState
end


--[[
@api {SET} bt:pickUpThePhone() 接听电话
@apiName pickUpThePhone()
@apiGroup BtGroup
@apiVersion 1.0.0
@apiDescription 当有电话接入时，通过这个接口接听电话

@apiParamExample 示例:
bt:pickUpThePhone()

@apiSuccessExample {number} 成功响应: 
呼入电话被成功接听

@apiErrorExample {number} 失败响应:
呼入的电话未被成功接听
]]

--[[
@notes
@tip:接听电话
@demo:bt:pickUpThePhone()
]]
function bt:pickUpThePhone()
	log_bt('pickUpThePhone', 'success')
    self.callState = true
    return true
end


--[[
@api {SET} bt:hangUpThePhone() 挂断电话
@apiName hangUpThePhone()
@apiGroup BtGroup
@apiVersion 1.0.0
@apiDescription 挂断当前电话

@apiParamExample 示例:
bt:hangUpThePhone()

@apiSuccessExample {number} 成功响应: 
当前通话被挂断

@apiErrorExample {number} 失败响应:
当前通话未被挂断
]]


--[[
@notes
@tip:挂断电话
@demo:bt:hangUpThePhone()
]]
function bt:hangUpThePhone()
	log_bt('pickUpThePhone', 'success')
    self.callState = false
	return true
end


--[[
@
api {SET} bt:callBackTheLastNumber() 回拨电话
@apiName callBackTheLastNumber()
@apiGroup BtGroup
@apiVersion 1.0.0
@apiDescription 回拨通讯录中最后一个电话号码

@apiParamExample 示例:
bt:callBackTheLastNumber()

@apiSuccessExample {number} 成功响应: 
通话记录中最后一个号码被回拨

@apiErrorExample {number} 失败响应:
无号码被回拨或回拨错误号码
]]


--[[
@notes
@tip:回拨最后一个电话
@demo:bt:callbackTheLastNumber()
]]
function bt:callbackTheLastNumber()
	local phone_number = '18666668880'
	log_bt('callbackTheLastNumber', 'call number: '..phone_number)
    self.callState = true
	return phone_number
end


--[[
@
api {GET} phone_list=bt:getPhoneMailList() 获取联系人
@apiName getCallState()
@apiGroup BtGroup
@apiVersion 1.0.0
@apiDescription 获取联系人列表

@apiParamExample 示例:
phone_list = bt:getPhoneMailList()
print(phone_list[1].name)   -- 打印名字
print(phone_list[1].number) -- 打印电话号码

@apiSuccessExample {number} 成功响应: 
成功获取联系人列表

@apiErrorExample {number} 失败响应:
获取联系人列表失败
]]


--[[
@notes
@tip:获取联系人列表，返回双重列表结构，每个联系人为一个元素，元素内包含名字和电话号码
@demo:tab = bt:getPhoneMailList()\n print(tab[1].name)\n print(tab[1].number)
]]
function bt:getPhoneMailList()
	local tab = {
		{name="小红", number="1866668880"},
		{name="小黄", number="1866668881"},
		{name="小白", number="1866668882"},
		{name="小黑", number="1866668883"},
		{name="小蓝", number="1866668884"}
	}
	log_bt('getPhoneMailList', 'mail list: ')
	for k, v in pairs(tab) do
		print(tostring(k).." : ".. v.name ..", ".. tostring(v.number))
	end
	return tab
end


--[[
@
api {GET} status=bt:getCallState() 获取手机通话状态
@apiName getCallState()
@apiGroup BtGroup
@apiVersion 1.0.0
@apiDescription 获取手机通话状态

@apiParamExample 示例:
status = bt:getCallState()
print(tostring(status))

@apiSuccessExample {number} 成功响应: 
正确返回手机通话状态

@apiErrorExample {number} 失败响应:
返回错误手机通话状态
]]


--[[
@notes
@tip:获取通话记录，每个记录为一个元素，元素内包含名字，电话号码，通话时间，type和mask信息
@demo:tab = bt:getPhoneCallLog()
]]
function bt:getPhoneCallLog()
	local tab = {
		{name="小红", number="1866668880", date="2021/08/30 12:30:00", type=1, mask=0},
		{name="小黄", number="1866668881", date="2021/08/30 12:30:29", type=1, mask=1},
		{name="小白", number="1866668882", date="2021/08/31 12:32:29", type=2, mask=0},
		{name="小黑", number="1866668883", date="2021/08/31 12:33:29", type=1, mask=0},
		{name="小蓝", number="1866668884", date="2021/08/31 12:35:29", type=2, mask=1},
	}
	log_bt('getPhoneCallLog', 'call log: ')
	for k, v in pairs(tab) do
		print(tostring(k)..": ".. v.name .. ", ".. v.number .. ", ".. v.date)
	end
	return tab
end



--[[
@api {GET} bt_addr=bt:GetBtAddr() 获取蓝牙地址
@apiName GetBtAddr()
@apiGroup BtGroup
@apiVersion 1.0.0
@apiDescription 获取蓝牙的mac地址

@apiParamExample 示例:
mac = bt:GetBtAddr()
print(mac)

@apiSuccessExample {number} 成功响应: 
获取正确的蓝牙mac地址

@apiErrorExample {number} 失败响应:
获取蓝牙地址错误或异常
]]


--[[
@notes
@tip:获取蓝牙地址
@demo:addr = bt:GetBtAddr()
]]
function bt:GetBtAddr()
    return self.bluetoothAddr
end



--[[
@api {GET} bt_name=bt:GetBtName() 获取蓝牙名称
@apiName GetBtName()
@apiGroup BtGroup
@apiVersion 1.0.0
@apiDescription 获取手表的蓝牙名称

@apiParamExample 示例:
name = bt:GetBtName()
print(name)

@apiSuccessExample {number} 成功响应: 
正确获取手表的蓝牙名称

@apiErrorExample {number} 失败响应:
获取手表蓝牙名称失败
]]


--[[
@notes
@tip:获取蓝牙名称
@demo:name = bt:GetBtName()
]]
function bt:GetBtName()
    return self.bluetoothName
end



--[[
@api {GET} tab=bt:emitterConnect(name,mac) 发射连接
@apiName emitterConnect()
@apiGroup BtGroup
@apiVersion 1.0.0
@apiDescription 蓝牙发射连接耳机

@apiParam {string} name 蓝牙设备名称
@apiParam {table} mac 蓝牙设备MAC地址

@apiParamExample 示例:
local bt_name = "test"  -- 蓝牙设备名称
local bt_mac = {0x40,0xB3,0x52,0x17,0x08}   -- 蓝牙设备mac
bt:emitterConnect(bt_name, bt_mac)  -- 连接指定蓝牙设备

@apiSuccessExample {number} 成功响应: 
成功与指定蓝牙设备配对

@apiErrorExample {number} 失败响应:
与指定蓝牙设备配对失败或无响应
]]


--[[
@notes
@tip:蓝牙发射连接耳机
@demo:tab = bt:emitterConnect(name, mac)
]]
function bt:emitterConnect(name, mac)
    self.connectState = true
	print('emitterConnect: '.. name)
    print(tostring(mac[1]))
    print(tostring(mac[2]))
    print(tostring(mac[3]))
    print(tostring(mac[4]))
    print(tostring(mac[5]))
    print(tostring(mac[6]))
end



--[[
@api {GET} tab=bt:emitterDisConnect(name,mac) 发射断开
@apiName emitterDisConnect(name,mac)
@apiGroup BtGroup
@apiVersion 1.0.0
@apiDescription 蓝牙发射断开连接


@apiParam {string} name 蓝牙设备名称
@apiParam {table} mac 蓝牙设备MAC地址

@apiParamExample 示例:
local bt_name = "test"  -- 蓝牙设备名称
local bt_mac = {0x40,0xB3,0x52,0x17,0x08}   -- 蓝牙设备mac
bt:emitterDisConnect(bt_name, bt_mac)  -- 断开指定设备

@apiSuccessExample {number} 成功响应: 
成功与指定设备断开连接

@apiErrorExample {number} 失败响应:
设备连接未断开
]]


--[[
@notes
@tip:蓝牙发射断开连接
@demo:tab = bt:emitterDisConnect(name, mac)
]]
function bt:emitterDisConnect(name, mac)
    self.connectState = false
	print('emitterDisConnect: '.. name)
    print(tostring(mac[1]))
    print(tostring(mac[2]))
    print(tostring(mac[3]))
    print(tostring(mac[4]))
    print(tostring(mac[5]))
    print(tostring(mac[6]))
end


--[[
@api {GET} tab=bt:emitterReleaseConnect(name,mac) 发射解除
@apiName emitterReleaseConnect(name,mac)
@apiGroup BtGroup
@apiVersion 1.0.0
@apiDescription 蓝牙发射解除连接

@apiParamExample 示例:
local bt_name = "test"  -- 蓝牙设备名称
local bt_mac = {0x40,0xB3,0x52,0x17,0x08}   -- 蓝牙设备mac
bt:emitterReleaseConnect(bt_name, bt_mac)   -- 解除指定设备配对

@apiSuccessExample {number} 成功响应: 
成功与指定设备解除配对

@apiErrorExample {number} 失败响应:
设备配对未解除
]]


--[[
@notes
@tip:蓝牙发射解除连接
@demo:tab = bt:emitterReleaseConnect(name, mac)
]]
function bt:emitterReleaseConnect(name, mac)
    self.connectState = false
	print('emitterReleaseConnect: '.. name)
    print(tostring(mac[1]))
    print(tostring(mac[2]))
    print(tostring(mac[3]))
    print(tostring(mac[4]))
    print(tostring(mac[5]))
    print(tostring(mac[6]))
end


--[[
@api {GET} tab=bt:emitterConnectStatus() 发射连接状态
@apiName emitterConnectStatus()
@apiGroup BtGroup
@apiVersion 1.0.0
@apiDescription 蓝牙发射连接状态：1-连接中，2-连接成功，3-连接失败

@apiParamExample 示例:
status = bt:emitterConnectStatus()
print(tostring(status))

@apiSuccessExample {number} 成功响应: 
正确返回连接状态

@apiErrorExample {number} 失败响应:
返回连接状态错误
]]


--[[
@notes
@tip:蓝牙发射连接状态。1-连接中，2-连接成功，3-连接失败
@demo:tab = bt:emitterConnectStatus()
]]
function bt:emitterConnectStatus()
    local status = 2
	print('emitterConnectStatus: ')
    return status
end


--[[
@api {GET} bt:emitterScanStart() 搜索开始
@apiName emitterScanStart()
@apiGroup BtGroup
@apiVersion 1.0.0
@apiDescription 蓝牙发射开始搜索

@apiParamExample 示例:
bt:emitterScanStart()

@apiSuccessExample {number} 成功响应: 
成功启动蓝牙设备搜索

@apiErrorExample {number} 失败响应:
未启动蓝牙设备搜索
]]


--[[
@notes
@tip:蓝牙发射开始搜索
@demo:tab = bt:emitterScanStart()
]]
function bt:emitterScanStart()
	print('emitterScanStart')
    return true
end


--[[
@api {GET} bt:emitterScanStop() 搜索停止
@apiName emitterScanStop()
@apiGroup BtGroup
@apiVersion 1.0.0
@apiDescription 蓝牙发射停止搜索

@apiParamExample 示例:
bt:emitterScanStop()

@apiSuccessExample {number} 成功响应: 
停止搜索蓝牙设备

@apiErrorExample {number} 失败响应:
未停止搜索蓝牙设备
]]


--[[
@notes
@tip:蓝牙发射停止搜索
@demo:tab = bt:emitterScanStop()
]]
function bt:emitterScanStop()
	print('emitterScanStop')
    return true
end


--[[
@api {GET} status=bt:emitterScanStatus() 搜索状态
@apiName emitterScanStatus()
@apiGroup BtGroup
@apiVersion 1.0.0
@apiDescription 蓝牙发射搜索状态。1-扫描中，2-扫描结束

@apiParamExample 示例:
status = bt:emitterScanStatus()
print(tostring(status))

@apiSuccessExample {number} 成功响应: 
获取到正确的扫描状态

@apiErrorExample {number} 失败响应:
获取到扫描状态错误
]]


--[[
@notes
@tip:蓝牙发射扫描状态。1-扫描中，2-扫描结束
@demo:tab = bt:emitterScanStatus()
]]
function bt:emitterScanStatus()
    local status = 2
	print('emitterScanStatus: ')
    return status
end


--[[
@api {GET} bt:emitterListClean() 列表清除
@apiName emitterListClean()
@apiGroup BtGroup
@apiVersion 1.0.0
@apiDescription 蓝牙发射搜索列表清除

@apiParamExample 示例:
bt:emitterListClean()

@apiSuccessExample {number} 成功响应: 
成功清空设备列表

@apiErrorExample {number} 失败响应:
设备列表未清空
]]


--[[
@notes
@tip:蓝牙发射搜索列表清除
@demo:tab = bt:emitterListClean()
]]
function bt:emitterListClean()
	print('emitterListClean')
    return true
end


--[[
@api {GET} total=bt:emitterGetTotal() 列表总数
@apiName emitterGetTotal()
@apiGroup BtGroup
@apiVersion 1.0.0
@apiDescription 蓝牙发射搜索列表中的总数

@apiParamExample 示例:
total = bt:emitterGetTotal()

@apiSuccessExample {number} 成功响应: 
获取到正确的列表内设备数量

@apiErrorExample {number} 失败响应:
获取到列表内设备数量错误
]]


--[[
@notes
@tip:蓝牙发射搜索列表中的总数
@demo:tab = bt:emitterGetTotal()
]]
function bt:emitterGetTotal()
    local total = 5
	print('emitterGetTotal: '.. tostring(total))
    return total
end

--[[
@api {GET} name=bt:emitterGetName(index) 获取名字
@apiName emitterGetName()
@apiGroup BtGroup
@apiVersion 1.0.0
@apiDescription 蓝牙发射名字

@apiParam {number} index 待获取设备名字的设备在列表中的索引

@apiParamExample 示例:
name = bt:emitterGetName(1)
print(name)

@apiSuccessExample {number} 成功响应: 
获取到的设备名称与预期一致

@apiErrorExample {number} 失败响应:
获取到设备名称错误
]]


--[[
@notes
@tip:蓝牙发射名字
@demo:tab = bt:emitterGetName(index)
]]
function bt:emitterGetName(index)
	local tab = {
		{name="ac6950", mac={0x1,0x2,0x3,0x4,0x5,0x0}},
		{name="ac6951", mac={0x1,0x2,0x3,0x4,0x5,0x1}},
		{name="ac6952", mac={0x1,0x2,0x3,0x4,0x5,0x2}},
		{name="ac6953", mac={0x1,0x2,0x3,0x4,0x5,0x3}},
		{name="ac6954", mac={0x1,0x2,0x3,0x4,0x5,0x4}},
	}
    print('emitterGetName: '.. tab[index].name)
    return 
end


--[[
@api {GET} mac=bt:emitterGetMac(index) 发射地址
@apiName emitterGetMac()
@apiGroup BtGroup
@apiVersion 1.0.0
@apiDescription 蓝牙发射地址

@apiParam {number} index 待获取MAC的设备在列表中的索引

@apiParamExample 示例:
mac = bt:emitterGetMac(1)
for k, v in ipairs(mac) do
    print(k, v)
end

@apiSuccessExample {number} 成功响应: 
成功获取到指定索引的设备MAC

@apiErrorExample {number} 失败响应:
获取MAC失败或获取到MAC错误
]]


--[[
@notes
@tip:蓝牙发射地址
@demo:tab = bt:emitterGetMac(index)
]]
function bt:emitterGetMac(index)
	local tab = {
		{name="ac6950", mac={0x1,0x2,0x3,0x4,0x5,0x0}},
		{name="ac6951", mac={0x1,0x2,0x3,0x4,0x5,0x1}},
		{name="ac6952", mac={0x1,0x2,0x3,0x4,0x5,0x2}},
		{name="ac6953", mac={0x1,0x2,0x3,0x4,0x5,0x3}},
		{name="ac6954", mac={0x1,0x2,0x3,0x4,0x5,0x4}},
	}
	print('emitterGetMac')
    print(tostring(tab[index].mac[1]))
    print(tostring(tab[index].mac[2]))
    print(tostring(tab[index].mac[3]))
    print(tostring(tab[index].mac[4]))
    print(tostring(tab[index].mac[5]))
    print(tostring(tab[index].mac[6]))
    return 
end


--[[
@api {GET} tab=bt:emitterGetList() 搜索列表
@apiName emitterGetList()
@apiGroup BtGroup
@apiVersion 1.0.0
@apiDescription 获取蓝牙发射搜索列表，每个记录为一个元素，元素内包含名字、mac地址信息

@apiParamExample 示例:
bt_list = bt:emitterGetList()

@apiSuccessExample {number} 成功响应: 
成功获取蓝牙设备列表

@apiErrorExample {number} 失败响应:
获取蓝牙设备列表失败
]]


--[[
@notes
@tip:获取蓝牙发射搜索列表，每个记录为一个元素，元素内包含名字，MAC地址信息
@demo:tab = bt:emitterGetList()
]]
function bt:emitterGetList()
	local tab = {
		{name="ac6950", mac={0x1,0x2,0x3,0x4,0x5,0x0}},
		{name="ac6951", mac={0x1,0x2,0x3,0x4,0x5,0x1}},
		{name="ac6952", mac={0x1,0x2,0x3,0x4,0x5,0x2}},
		{name="ac6953", mac={0x1,0x2,0x3,0x4,0x5,0x3}},
		{name="ac6954", mac={0x1,0x2,0x3,0x4,0x5,0x4}},
	}
	print('emitterGetList')
	for k, v in pairs(tab) do
		print(tostring(k)..": ".. v.name)
		print(tostring(v.mac[1]))
		print(tostring(v.mac[2]))
		print(tostring(v.mac[3]))
		print(tostring(v.mac[4]))
		print(tostring(v.mac[5]))
		print(tostring(v.mac[6]))
	end
	return tab
end


--[[
@api {GET} bt:emitterSaveListClean() 列表清除
@apiName emitterSaveListClean()
@apiGroup BtGroup
@apiVersion 1.0.0
@apiDescription 蓝牙发射收藏夹列表清除

@apiParamExample 示例:
bt:emitterSaveListClean()

@apiSuccessExample {number} 成功响应: 
收藏夹列表内所有设备信息被清除

@apiErrorExample {number} 失败响应:
清除列表内设备信息失败
]]


--[[
@notes
@tip:蓝牙发射收藏夹列表清除
@demo:tab = bt:emitterSaveListClean()
]]
function bt:emitterSaveListClean()
	print('emitterSaveListClean')
    return true
end



--[[
@api {GET} tab=bt:emitterSaveGetTotal() 收藏总数
@apiName emitterSaveGetTotal()
@apiGroup BtGroup
@apiVersion 1.0.0
@apiDescription 蓝牙发射收藏夹中的总数

@apiParamExample 示例:
total = bt:emitterSaveGetTotal()

@apiSuccessExample {number} 成功响应: 
正确获取到收藏夹列表中设备数量

@apiErrorExample {number} 失败响应:
获取到收藏夹设备数量错误
]]

--[[
@notes
@tip:蓝牙发射收藏夹中的总数
@demo:tab = bt:emitterSaveGetTotal()
]]
function bt:emitterSaveGetTotal()
    local total = 5
	print('emitterSaveGetTotal'..": ".. total)
    return total
end




--[[
@api {GET} name=bt:emitterSaveGetName(index) 获取收藏夹名字
@apiName emitterSaveGetName()
@apiGroup BtGroup
@apiVersion 1.0.0
@apiDescription 获取收藏夹中的蓝牙名字

@apiParam {number} index 收藏夹列表中索引

@apiParamExample 示例:
name = bt:emitterSaveGetName(1)

@apiSuccessExample {number} 成功响应: 
正确获取收藏夹中索引为1的设备名称

@apiErrorExample {number} 失败响应:
获取设备名称失败或名称错误
]]

--[[
@notes
@tip:获取收藏夹中的蓝牙名字
@demo:tab = bt:emitterSaveGetName(index)
]]
function bt:emitterSaveGetName(index)
	local tab = {
		{name="ac6960", mac={0x5,0x4,0x3,0x2,0x1,0x0}, connect=1},
		{name="ac6961", mac={0x5,0x4,0x3,0x2,0x1,0x1}, connect=0},
		{name="ac6962", mac={0x5,0x4,0x3,0x2,0x1,0x2}, connect=0},
		{name="ac6963", mac={0x5,0x4,0x3,0x2,0x1,0x3}, connect=0},
		{name="ac6964", mac={0x5,0x4,0x3,0x2,0x1,0x4}, connect=0},
	}
	print('emitterSaveGetName:'.. tab[index].name)
    return 
end


--[[
@api {GET} tab=bt:emitterSaveGetMac(index) 获取收藏地址
@apiName emitterSaveGetMac()
@apiGroup BtGroup
@apiVersion 1.0.0
@apiDescription 获取收藏夹中的蓝牙地址

@apiParam {number} index 收藏夹列表中索引

@apiParamExample 示例:
mac = bt:emitterSaveGetMac(1)

@apiSuccessExample {number} 成功响应: 
正确获取到收藏夹列表中索引为1的设备MAC

@apiErrorExample {number} 失败响应:
获取到设备MAC错误
]]

--[[
@notes
@tip:获取收藏夹中的蓝牙地址
@demo:tab = bt:emitterSaveGetMac(index)
]]
function bt:emitterSaveGetMac(index)
	local tab = {
		{name="ac6960", mac={0x5,0x4,0x3,0x2,0x1,0x0}, connect=1},
		{name="ac6961", mac={0x5,0x4,0x3,0x2,0x1,0x1}, connect=0},
		{name="ac6962", mac={0x5,0x4,0x3,0x2,0x1,0x2}, connect=0},
		{name="ac6963", mac={0x5,0x4,0x3,0x2,0x1,0x3}, connect=0},
		{name="ac6964", mac={0x5,0x4,0x3,0x2,0x1,0x4}, connect=0},
	}
	print('emitterSaveGetMac')
    print(tostring(tab[index].mac[1]))
    print(tostring(tab[index].mac[2]))
    print(tostring(tab[index].mac[3]))
    print(tostring(tab[index].mac[4]))
    print(tostring(tab[index].mac[5]))
    print(tostring(tab[index].mac[6]))
    return 
end



--[[
@api {GET} tab=bt:emitterSaveGetList() 获取收藏夹列表
@apiName emitterSaveGetList()
@apiGroup BtGroup
@apiVersion 1.0.0
@apiDescription 获取蓝牙发射收藏夹列表，每个记录为一个元素，元素内包含名字，mac地址，连接状态信息

@apiParamExample 示例:
save_list = bt:emitterSaveGetList()

@apiSuccessExample {number} 成功响应: 
正确获取到收藏夹列表

@apiErrorExample {number} 失败响应:
获取收藏夹列表失败
]]

--[[
@notes
@tip:获取蓝牙发射收藏夹列表，每个记录为一个元素，元素内包含名字，MAC地址，连接状态信息
@demo:tab = bt:emitterSaveGetList()
]]
function bt:emitterSaveGetList()
	local tab = {
		{name="ac6960", mac={0x5,0x4,0x3,0x2,0x1,0x0}, connect=1},
		{name="ac6961", mac={0x5,0x4,0x3,0x2,0x1,0x1}, connect=0},
		{name="ac6962", mac={0x5,0x4,0x3,0x2,0x1,0x2}, connect=0},
		{name="ac6963", mac={0x5,0x4,0x3,0x2,0x1,0x3}, connect=0},
		{name="ac6964", mac={0x5,0x4,0x3,0x2,0x1,0x4}, connect=0},
	}
	print('emitterSaveGetList')
	for k, v in pairs(tab) do
		print(tostring(k)..": ".. v.name .. ", ".. v.connect)
		print(tostring(v.mac[1]))
		print(tostring(v.mac[2]))
		print(tostring(v.mac[3]))
		print(tostring(v.mac[4]))
		print(tostring(v.mac[5]))
		print(tostring(v.mac[6]))
	end
	return tab
end


local tmp_save = 0
local tmp_tab = {name="tmp0", mac={0x0,0x0,0x0,0x0,0x0,0x0}, connect=0}


--[[
@api {GET} bt:emitterTempSet(name,mac,connect) 临时数据保存
@apiName emitterTempSet()
@apiGroup BtGroup
@apiVersion 1.0.0
@apiDescription 蓝牙发射临时数据保存

@apiParam {string} name 设备名称
@apiParam {table} mac 设备MAC
@apiParam {number} connect 连接状态

@apiParamExample 示例:
name = "test"   -- 设备名称
mac = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6}    -- 设备MAC
connect = 0     -- 未连接
bt:emitterTempSet(name, mac, 0)

@apiSuccessExample {number} 成功响应: 
成功将指定设备设置到暂存区

@apiErrorExample {number} 失败响应:
指定设备暂存失败
]]


--[[
@notes
@tip:蓝牙发射临时数据保存
@demo:tab = bt:emitterTempSet()
]]
function bt:emitterTempSet(name, mac, connect)
	print('emitterTempSet, name: '.. name)
	print('emitterTempSet, old: '.. tmp_tab.name)
    tmp_tab.name = name
    tmp_tab.mac[1] = mac[1]
    tmp_tab.mac[2] = mac[2]
    tmp_tab.mac[3] = mac[3]
    tmp_tab.mac[4] = mac[4]
    tmp_tab.mac[5] = mac[5]
    tmp_tab.mac[6] = mac[6]
    tmp_tab.connect = connect
    tmp_save = 1
	print('emitterTempSet, new: '.. tmp_tab.name)
	return true
end



--[[
@api {GET} tab=bt:emitterTempGet() 临时数据获取
@apiName emitterTempGet()
@apiGroup BtGroup
@apiVersion 1.0.0
@apiDescription 蓝牙发射临时数据获取

@apiParamExample 示例:
dev = bt:emitterTempGet()
print(dev.name)

@apiSuccessExample {number} 成功响应: 
成功获取到暂存的设备信息

@apiErrorExample {number} 失败响应:
获取暂存设备信息失败或错误
]]


--[[
@notes
@tip:蓝牙发射临时数据获取
@demo:tab = bt:emitterTempGet()
]]
function bt:emitterTempGet()
	print('emitterTempGet, name: '.. tmp_tab.name .. 'flag: ' .. tostring(tmp_save))
    if (tmp_save == 0) then
        return nil
    end
	return tmp_tab
end



--[[
@api {GET} bt:emitterTempClean() 临时数据清除
@apiName emitterTempClean()
@apiGroup BtGroup
@apiVersion 1.0.0
@apiDescription 蓝牙发射临时数据清除

@apiParamExample 示例:
bt:emitterTempClean()

@apiSuccessExample {number} 成功响应: 
暂存信息被清除

@apiErrorExample {number} 失败响应:
暂存信息清除失败
]]


--[[
@notes
@tip:蓝牙发射临时数据清除
@demo:tab = bt:emitterTempClean()
]]
function bt:emitterTempClean()
	print('emitterTempClean')
    tmp_save = 0
	return true
end


return bt




