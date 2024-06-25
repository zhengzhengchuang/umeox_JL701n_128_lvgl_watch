--[[
@apiDefine GuiGroup gui
--]]



--[[
@notes
@tip:gui库，包含所有gui相关的操作方法，包含创建实例，页面切换及具体控件方法等
@demo:gui:getComponentByName("BUTTON_1")\n gui:switchPageByName("PAGE_3", "left")\n gui:backToPreviousPage("right")
]]
gui = {}

gui.dial_item = 1



-- 表盘预览图路径
local watchPreviewImage = "/模式界面/project/config/蓝牙智能手表切图/表盘预览图/"

-- 表盘背景图路径
local watchBgPreviewImage = "/模式界面/project/config/蓝牙智能手表切图/表盘切换/"



-- 打印错误信息
function log_e(func, msg)
	log:e('function \"'..func..'\", '..msg)
end

-- 打印警告信息
function log_w(func, msg)
	log:w('function \"'..func..'\", '..msg)
end

-- 打印参数参考
function log_refer(msg)
	log:i('>> please refer to : '..msg)
end

-- 检查参数类型
local function check_type(func, arg, num, typ)
    return log:checkArgType(func, num, arg, typ)
end




-- 获取工作目录
local function getRootPath()
    local config = sim_system:getWatchConfig()
    return config.workspace_path
end

-- 获取模式界面路径
local function getModulePath()
    local config = sim_system:getWatchConfig()
    return config.workspace_path .. config.project_list.mode_interface
end

-- 获取升级界面路径
local function getUpdatePath()
    local config = sim_system:getWatchConfig()
    return config.workspace_path .. config.project_list.update_interface
end

-- 获取侧边栏页面路径
local function getSidebarPath()
    local config = sim_system:getWatchConfig()
    return config.workspace_path .. config.project_list.sidebar_interface
end

-- 获取表盘页面路径
local function getDialPath(num)
    local config = sim_system:getWatchConfig()
    return (config.workspace_path)..(config.project_list.dial_interface[num])
end

-- 获取表盘数量
local function getDialNum()
    local config = sim_system:getWatchConfig()
    return #config.project_list.dial_interface
end




-- 获取表盘预览图数量
local function getWatchPreviewNum()
    local watchPreviewPath = getRootPath() .. watchPreviewImage
    local previewList = sim_file:getListFile(watchPreviewPath)
    return #previewList
end

-- 获取表盘预览图路径
local function getWatchPreviewPath(num)
    local watchPreviewPath = getRootPath() .. watchPreviewImage
    local previewList = sim_file:getListFile(watchPreviewPath)
    return previewList[num].path
end


-- 获取表盘背景图数量
local function getWatchBgPreviewNum()
    local watchBgPreviewPath = getRootPath() .. watchBgPreviewImage
    local bgPreviewList = sim_file:getListFile(watchBgPreviewPath)
    return #bgPreviewList
end

-- 获取表盘背景图路径
local function getWatchBgPreviewPath(num)
    local watchBgPreviewPath = getRootPath() .. watchBgPreviewImage
    local previewList = sim_file:getListFile(watchBgPreviewPath)
    return previewList[num].path
end




--[[
@api {SET} gui:touchEventDefault(takeover) 默认消息
@apiName touchEventDefault()
@apiGroup GuiGroup
@apiVersion 1.0.0
@apiDescription touch消息是否默认接管

@apiParam {boolean{true, false}} [takeover] 是否默认接管touch消息，缺省时接管。

@apiParamExample 示例:
gui:touchEventDefault()     -- 默认接管touch消息
gui:touchEventDefault(false)    -- 默认不接管touch消息

@apiSuccessExample {number} 成功响应: 
所有touch消息不再传给父控件

@apiErrorExample {number} 失败响应:
父控件依然接收到touch消息
]]
function gui:touchEventDefault(take)
	if (not check_type('touchEventTakeover', take, 1, 'boolean')) then
		return
	end
	log:d('ename: [ GUI ], touchEventTakeover: '..tostring(take))
	if take then
		log:d("take over on")
		ui:touchEventTakeOverOn()
	else
		log:d("take over off")
		ui:touchEventTakeOverOff()
	end
end



--[[
@api {SET} gui:touchEventTakeover(takeover) 本次消息
@apiName touchEventTakeover()
@apiGroup GuiGroup
@apiVersion 1.0.0
@apiDescription 本次消息接管设置，只设置本次消息是否接管，一般用在touchDown，在touchDown中接管消息后，会将ui焦点聚焦到该控件

@apiParam {boolean{true, false}} [takeover] 是否接管本次消息，缺省时接管

@apiParamExample 示例:
gui:touchEventTakeover()    -- 接管本次消息
gui:touchEventTakeover(false)   -- 不接管本次消息

@apiSuccessExample {number} 成功响应: 
本次touch消息不再传给父控件

@apiErrorExample {number} 失败响应:
父控件依然接收到touch消息
]]
function gui:touchEventTakeover(take)
	if (not check_type('touchEventTakeover', take, 1, 'boolean')) then
		return
	end
	log:d('ename: [ GUI ], touchEventTakeover: '..tostring(take))
	if take then
		log:d("take over on")
		ui:touchEventTakeOverOn()
	else
		log:d("take over off")
		ui:touchEventTakeOverOff()
	end
end





WatchDial = {}


--[[
@api {GET} number=obj:getWatchNum() 表盘数量
@apiName getWatchNum()
@apiGroup GuiGroup
@apiVersion 1.0.0
@apiDescription 获取内存中表盘的数量

@apiParamExample 示例:
watchdial = gui:getWatchHandler()   -- 获取表盘句柄
number = watchdial:getWatchNum()    -- 获取表盘数量
print(number)   --打印表盘数量

@apiSuccessExample {number} 成功响应: 
准确获取内存中的表盘数量

@apiErrorExample {number} 失败响应:
获取到内存中表盘数量错误或程序异常
]]
function WatchDial:getWatchNum()

    -- 获取表盘工程数量
    local watchNum = getDialNum()

    -- 获取预览图数量
    local previewNum = getWatchPreviewNum()

    -- 如果表盘工程数量和预览图数量不匹配，需要提示异常
    if (watchNum ~= previewNum) then
        log_e("getWatchNum", "watch dial project number no equal to dial preview number, please check it!")
        log:c(self.previewPath, "orange")
        for k, v in ipairs(gui.project_list.dial_interface) do
            log:c(v, "orange")
        end
    end

    log:d("watch number: "..tostring(watchNum))
    return watchNum
end


--[[
@api {SET} obj:openWatchPreview(number) 打开表盘预览
@apiName openWatchPreview()
@apiGroup GuiGroup
@apiVersion 1.0.0
@apiDescription 打开指定的表盘预览文件

@apiParam {number} number 需要打开的表盘预览图索引

@apiParamExample 示例:
watchdial = gui:getWatchHandler()   -- 获取表盘句柄
watchdial:openWatchPreview(0)    -- 打开第0个表盘预览

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function WatchDial:openWatchPreview(num)
    if (not log:checkArgType("openWatchPreview", 1, num, "number")) then
        return
    end

    local watchNum = getWatchPreviewNum()
    if (num < 0) or (num >= watchNum) then
        log_e("openWatchPreview", "arg must between 0 to "..tostring(watchNum))
        return
    end
    log:d("openWatchPreview")
end



--[[
@api {SET} obj:closeWatchPreview(number) 关闭表盘预览
@apiName closeWatchPreview()
@apiGroup GuiGroup
@apiVersion 1.0.0
@apiDescription 关闭指定的表盘预览图

@apiParam {number} number 准备关闭的表盘文件索引

@apiParamExample 示例:
watchdial = gui:getWatchHandler()   -- 获取表盘句柄
watchdial:closeWatchPreview(0)      -- 关闭第0个表盘

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function WatchDial:closeWatchPreview(num)
    if (not log:checkArgType("closeWatchPreview", 1, num, "number")) then
        return
    end

    local watchNum = getWatchPreviewNum()
    if (num < 0) or (num >= watchNum) then
        log_e("closeWatchPreview", "arg must between 0 to "..tostring(watchNum))
        return
    end
    log:d("closeWatchPreview")
end


--[[
@api {SET} obj:selectWatchPreview(number) 选中表盘预览
@apiName selectWatchPreview()
@apiGroup GuiGroup
@apiVersion 1.0.0
@apiDescription 选中指定的表盘预览图

@apiParam {number} number 选中的表盘文件索引

@apiParamExample 示例:
watchdial = gui:getWatchHandler()   -- 获取表盘句柄
watchdial:selectWatchPreview(0)     -- 选中第0个表盘预览

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function WatchDial:selectWatchPreview(num)
    if (not log:checkArgType("selectWatchPreview", 1, num, "number")) then
        return
    end

    local watchNum = getDialNum()
    if (num < 0) or (num >= watchNum) then
        log_e("selectWatchPreview", "arg must between 0 to "..tostring(watchNum))
        return
    end

    -- 返回文件路径
    return getWatchPreviewPath(num + 1)
end



--[[
@api {SET} obj:switchWatch(index) 切换表盘
@apiName switchWatch()
@apiGroup GuiGroup
@apiVersion 1.0.0
@apiDescription 切换选中表盘使用

@apiParam {number} index 切换的目标表盘索引

@apiParamExample 示例:
watchdial = gui:getWatchHandler()   -- 获取表盘句柄
watchdial:switchWatch(1)    -- 切换到索引为1的表盘

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function WatchDial:switchWatch(item)
    if (not log:checkArgType("switchWatch", 1, item, "number")) then
        return
    end

    local watchNum = getDialNum()
    if (item < 0) or (item >= watchNum) then
        log_e("switchWatch", "arg must between 0 to "..tostring(watchNum))
        return
    end
    gui.dial_item = item

    -- 替换PAGE_0为对应的表盘，并切换页面
    local dialPath = getDialPath(num + 1)
    ui:replaceProjectPage(dialPath, 0, 0)
    ui:switchPage(0)
end


--[[
@api {SET} number=obj:getWatchBgpNum() 表盘背景图数量
@apiName getWatchBgpNum()
@apiGroup GuiGroup
@apiVersion 1.0.0
@apiDescription 获取表盘背景图数量

@apiParamExample 示例:
watchdial = gui:getWatchHandler()   -- 获取表盘句柄
number = watchdial:getWatchBgpNum() -- 获取表盘背景图数量

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function WatchDial:getWatchBgpNum()
    -- 获取背景图数量
    local bgppreviewNum = getWatchBgPreviewNum()
    log:d("watch backgrand image number: "..tostring(bgppreviewNum))
    return bgppreviewNum
end



--[[
@api {SET} obj:openWatchBgpPreview(number) 打开表盘背景
@apiName openWatchBgpPreview()
@apiGroup GuiGroup
@apiVersion 1.0.0
@apiDescription 打开指定表盘背景图

@apiParam {number} number 打开的表盘背景预览图索引

@apiParamExample 示例:
watchdial = gui:getWatchHandler()   -- 获取表盘句柄
watchdial:openWatchBgpPreview(0)    -- 打开第0个表盘背景预览图

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function WatchDial:openWatchBgpPreview(num)
    if (not log:checkArgType("openWatchBgpPreview", 1, num, "number")) then
        return
    end

    local number = getWatchBgPreviewNum()
    if (num < 0) or (num >= number) then
        log_e("openWatchBgpPreview", "arg must between 0 to "..tostring(number))
        return
    end

    log:d("openWatchBgpPreview")
end



--[[
@api {SET} obj:closeWatchBgpPreview(index) 关闭表盘背景
@apiName closeWatchBgpPreview()
@apiGroup GuiGroup
@apiVersion 1.0.0
@apiDescription 关闭指定的表盘背景图

@apiParam {number} index 关闭的表盘背景预览图索引

@apiParamExample 示例:
watchdial = gui:getWatchHandler()   -- 获取表盘句柄
watchdial:closeWatchBgpPreview(0)   -- 关闭第0个表盘背景预览图

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function WatchDial:closeWatchBgpPreview(num)
    if (not log:checkArgType("closeWatchBgpPreview", 1, num, "number")) then
        return
    end

    local number = getWatchBgPreviewNum()
    if (num < 0) or (num >= number) then
        log_e("closeWatchBgpPreview", "arg must between 0 to "..tostring(number))
        return
    end

    log:d("closeWatchBgpPreview")
end



--[[
@api {SET} obj:selectWatchBgpPreview(index) 选择表盘背景
@apiName selectWatchBgpPreview()
@apiGroup GuiGroup
@apiVersion 1.0.0
@apiDescription 选中指定的表盘背景图

@apiParam {number} index 选择的表盘背景预览图索引

@apiParamExample 示例:
watchdial = gui:getWatchHandler()   -- 获取表盘句柄
watchdial:selectWatchBgpPreview(0)  -- 选择第0个表盘背景图

@apiSuccessExample {number} 成功响应: 
暂空

@apiErrorExample {number} 失败响应:
暂空
]]
function WatchDial:selectWatchBgpPreview(num)
    if (not log:checkArgType("selectWatchBgpPreview", 1, num, "number")) then
        return
    end

    local number = getWatchBgPreviewNum()
    if (num < 0) or (num >= number) then
        log_e("selectWatchBgpPreview", "arg must between 0 to "..tostring(number))
        return
    end

    -- 返回文件路径
    return getWatchBgPreviewPath(num + 1)
end



--[[
@api {SET} watch=obj:getWatchHandler() 表盘句柄
@apiName getWatchHandler()
@apiGroup GuiGroup
@apiVersion 1.0.0
@apiDescription 获取表盘操作句柄

@apiParamExample 示例:
watchdial = gui:getWatchHandler()   -- 获取表盘句柄

@apiSuccessExample {number} 成功响应: 
暂空

@apiErrorExample {number} 失败响应:
暂空
]]
function gui:getWatchHandler()
    local watch = {}
    setmetatable(watch, {__index = WatchDial})
    return watch
end




-- 页面切换列表
local pageSwitch = {}
pageSwitch.index = 0
pageSwitch.list = {}
pageSwitch.maxPageNumber = 8



--[[
@api {SET} gui:switchPageByName(PAGE,mode[,record]) 页面跳转
@apiName switchPageByName()
@apiGroup GuiGroup
@apiVersion 1.0.0
@apiDescription 按照指定的方式进行页面跳转

@apiParam {string{"PAGE_0", "PAGE_1", "PAGE_2", ...}} PAGE 目标页面名称
@apiParam {string{"left", "right", "hide"}} mode 切换方式
@apiParam {boolean{true, false}} [record] 是否记录切换路径，缺省时记录

@apiParamExample 示例:
-- 直接覆盖方式切换到PAGE1
gui:switchPageByName("PAGE_1", "hide")

-- 从右边滑入方式切换到PAGE3
gui:switchPageByName("PAGE_3", "right")

@apiSuccessExample {number} 成功响应: 
成功按照指定方式跳转到目标页面

@apiErrorExample {number} 失败响应:
页面未跳转，或未按照指定方式跳转到目标页面
]]
function gui:switchPageByName(...)
    local arg = {...}
    local arg_num = #arg

    for k, v in pairs(arg) do
        print(k, v)
    end


    -- 参数只能两个或三个
    if (arg_num ~= 2) and (arg_num ~= 3) then
        log_e("switchPageByName", "arg number, error")
        return
    end

    local ename = arg[1]
    local mode = arg[2]
    local record = true
	local s, e, str
	-- 判断ename是否为字符串
    if (not log:checkArgType("switchPageByName", 1, ename, "string", "number")) then
		log_refer('"PAGE_0", "PAGE_1", "PAGE_2" etc.')
        return
    else
        -- 正则匹配是否为页面
        if (type(ename) == "string") then
            s, e, str = string.find(ename, '(%bPAGE_)%d')
            if ((s ~= 1) and (str ~= 'PAGE_')) then
                log_e('switchPageByName', 'arg0 is not a page name!')
                log_refer('"PAGE_0", "PAGE_1", "PAGE_2" etc.')
                return
            end
        end
    end

	-- 判断mode是否为字符串
    if (not log:checkArgType("switchPageByName", 2, mode, "string")) then
		log_refer('"hide", "left", "right"')
        return 
    end

	-- 判断mode是否在指定范围内
    local mode_val = {"left", "right", "hide"}
    if (not log:checkValue(mode, mode_val)) then
        log_refer('"hide", "left", "right"')
        return
    end

    -- 如果有第三个参数，检查第三个参数
    if (arg_num == 3) then
        if (not log:checkArgType("switchPageByName", 3, arg[3], "boolean")) then
            return
        end
        record = arg[3]
    end

    -- 记录页面跳转
    if (record) then
        -- 当前页面
        local current_page = ui:getCurrPageName()
        -- 目标页面
        local target_page  = ename

        -- 如果列表里最新的页面不是当前页面，那么清空列表重新开始记录
        if (not (pageSwitch.list[pageSwitch.index] ~= current_page)) then
            pageSwitch.list[1] = current_page
            pageSwitch.index = 1
        end

        -- 列表索引加一
        pageSwitch.index = pageSwitch.index + 1
        -- 如果列表索引超过最大页面记录数量，跳转到表盘页面
        if (pageSwitch.index >= pageSwitch.maxPageNumber) then
            pageSwitch.index = 1
            local dialPath = getDialPath(gui.dial_item)
            log:d("show dial: "..dialPath)
            ui:replaceProjectPage(dialPath, 0, 0)
            ui:switchPage(0)
            return
        end

        -- 记录目标页面
        pageSwitch.list[pageSwitch.index] = target_page
        -- table.insert(pageSwitch.list, ename)
        -- pageSwitch.index = pageSwitch.index + 1
        for k, v in ipairs(pageSwitch.list) do
            print(k, v)
        end
    end
	
	-- 参数检查正确再模拟切换页面
	log:d('switch page by name: '.. ename..', '..mode)

    if (ename == "PAGE_0") then
        -- 页面0表示表盘页面
        local dialPath = getDialPath(gui.dial_item)
        log:d("show dial: "..dialPath)
        ui:switchPage(0)
        ui:replaceProjectPage(dialPath, 0, 0)
    else
        if (mode == "hide") then
            ui:switchPageByName(ename, 0)
        else
            ui:switchPageByName(ename, mode)
        end
    end
end



--[[
@api {SET} gui:backToPreviousPage(mode) 返回上一级页面
@apiName backToPreviousPage()
@apiGroup GuiGroup
@apiVersion 1.0.0
@apiDescription 返回上一级页面，必须是通过switchPageByName跳转并选择记录的页面才能返回，否则直接返回表盘页面

@apiParam {string{"left", "right", "hide"}} mode 返回时切换方式

@apiParamExample 示例:
gui:switchPageByName("PAGE_5", "left")  -- 从左边滑入切换到第5页
gui:backToPreviousPage("left")      -- 从右边滑入返回上一页

@apiSuccessExample {number} 成功响应: 
成功返回上一次页面

@apiErrorExample {number} 失败响应:
返回表盘页面，或未按指定方式返回上一级页面
]]
function gui:backToPreviousPage(mode)
    if (not log:checkArgType("backToPreviousPage", 1, mode, "string")) then
		log_refer('"hide", "left", "right"')
        return
    end

    local mode_val = {"hide", "left", "right"}
    if (not log:checkValue(mode, mode_val)) then
		log_refer('"hide", "left", "right"')
		return
    end

    local current_page = ui:getCurrPageName()
    -- 如果当前页面不是记录中最新的页面，则跳转到表盘页面
    if (current_page ~= pageSwitch.list[pageSwitch.index]) then
        pageSwitch.index = 1
        local dialPath = getDialPath(gui.dial_item)
        log:d("show dial: "..dialPath)
        ui:replaceProjectPage(dialPath, 0, 0)
        ui:switchPage(0)
        return
    end
    pageSwitch.index = pageSwitch.index - 1
    -- 如果索引小于1，则跳转到表盘页面
    if (pageSwitch.index <= 1) then
        pageSwitch.index = 1
        local dialPath = getDialPath(gui.dial_item)
        log:d("show dial: "..dialPath)
        ui:replaceProjectPage(dialPath, 0, 0)
        ui:switchPage(0)
        return
    end
    -- 获取上一个页面并跳转
    local prev_page = pageSwitch.list[pageSwitch.index]
    ui:switchPageByName(prev_page, mode)

	log:d('back to previous page: '..mode)

	-- ui:backToPreviousPage(mode)
    -- table.remove(pageSwitch.list)
    -- pageSwitch.index = pageSwitch.index - 1
end


--[[
@api {SET} gui:checkPreviousPage(PAGE) 检查上一级页面
@apiName checkPreviousPage()
@apiGroup GuiGroup
@apiVersion 1.0.0
@apiDescription 检查上一级页面是否为指定页面

@apiParam {string{"PAGE_0", "PAGE_1", "PAGE_2", ...}} PAGE 上一级页面名称

@apiParamExample 示例:
if (gui:checkPreviousPage("PAGE_7")) then
    -- 如果上一级页面是PAGE7执行这里
else
    -- 否则执行这里
end

@apiSuccessExample {number} 成功响应: 
返回 true，上一页是指定页面
返回 false，上一页不是指定页面

@apiErrorExample {number} 失败响应:
无
]]
function gui:checkPreviousPage(page)
    if (not log:checkArgType("checkPreviousPage", 1, page, "string")) then
        return 
    end

	-- 正则匹配是否为页面
	local s, e, str
	s, e, str = string.find(page, '(%bPAGE_)%d')
	if ((s ~= 1) and (str ~= 'PAGE_')) then
		log_e('checkPreviousPage', 'arg0 is not a page name!')
		log_refer('"PAGE_0", "PAGE_1", "PAGE_2" etc.')
		return
	end
	
    local prev_page = pageSwitch.list[pageSwitch.index - 1]
	log:d("check page: "..tostring(page)..", prev_page: "..tostring(prev_page))
	if prev_page == page then
		return true
	end
	return false
end



--[[
@api {SET} type=gui:getControlType(ename) 获取控件类型
@apiName getControlType()
@apiGroup GuiGroup
@apiVersion 1.0.0
@apiDescription 获取控件的类型名称

@apiParam {string} ename 控件名称

@apiParamExample 示例:
control_type = gui:getControlType("TEXT_TEST") -- 获取名称为“TEXT_TEST”的控件类型

@apiSuccessExample {number} 成功响应: 
返回控件名称字符串

@apiErrorExample {number} 失败响应:
返回值为 nil
]]
function gui:getControlType(ename)
    if (not log:checkArgType("getControlType", 1, ename, "string")) then
        return nil
    end

    local ctr_type = nil
	ctr_type = ui:getControlType(ename)
	if ((ctr_type == "VerticalList") or (ctr_type == "HorizontalList")) then
		ctr_type = "List"
	end
	log:d('ename: [ '..ename..' ], control type: '..ctr_type)
	return ctr_type
end


-- 页面滑动列表
local pageSlide = {index = 0, list = {}}



--[[
@api {SET} gui:pageSlideInit() 页面滑动初始化
@apiName pageSlideInit()
@apiGroup GuiGroup
@apiVersion 1.0.0
@apiDescription 页面循环滑动初始化

@apiParamExample 示例:
gui:pageSlideInit()

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function gui:pageSlideInit()
    log:d("pageSlideInit")
	slide:initPage()
end



--[[
@api {SET} gui:pageSlideAdd(PAGE) 页面增加
@apiName pageSlideAdd()
@apiGroup GuiGroup
@apiVersion 1.0.0
@apiDescription 增加指定页面到滑动列表

@apiParam {string{"PAGE_1", "PAGE_2", "PAGE_3", ...}} PAGE 需增加到循环滑动列表的页面

@apiParamExample 示例:
gui:pageSlideAdd("PAGE_5") -- 增加PAGE5到循环滑动列表

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function gui:pageSlideAdd(page)
	-- 正则匹配是否为页面
	local s, e, str
	s, e, str = string.find(page, '(%bPAGE_)%d')
	if ((s ~= 1) and (str ~= 'PAGE_')) then
		log_e('checkPreviousPage', 'arg0 is not a page name!')
		log_refer('"PAGE_0", "PAGE_1", "PAGE_2" etc.')
		return
	end
    log:d("pageSlideAdd: "..page)
    slide:insertNextPageByName(page)
end



--[[
@api {SET} gui:pageSlideDel(PAGE) 页面删除
@apiName pageSlideDel()
@apiGroup GuiGroup
@apiVersion 1.0.0
@apiDescription 从页面滑动列表中删除指定页面

@apiParam {string{"PAGE_0", "PAGE_1", "PAGE_2", ...}} PAGE 需从循环滑动页面列表删除的页面

@apiParamExample 示例:
gui:pageSlideDel("PAGE_3")  -- 将PAGE3从循环滑动页面列表删除

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function gui:pageSlideDel(page)
	-- 正则匹配是否为页面
	local s, e, str
	s, e, str = string.find(page, '(%bPAGE_)%d')
	if ((s ~= 1) and (str ~= 'PAGE_')) then
		log_e('checkPreviousPage', 'arg0 is not a page name!')
		log_refer('"PAGE_0", "PAGE_1", "PAGE_2" etc.')
		return
	end
    log:d("pageSlideDel: "..page)
    slide:deletePageByName(page)
end



--[[
@api {SET} gui:pageSlideFree() 释放页面
@apiName pageSlideFree()
@apiGroup GuiGroup
@apiVersion 1.0.0
@apiDescription 释放页面滑动列表

@apiParamExample 示例:
gui:pageSlideFree()     --释放掉所有滑动列表的页面，关闭页面滑动功能

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function gui:pageSlideFree()
    log:d("pageSlideFree")
	slide:freePage()
end



--[[
@api {SET} gui:pageSlideNext(PAGE) 插入下一页
@apiName pageSlideNext()
@apiGroup GuiGroup
@apiVersion 1.0.0
@apiDescription 从当前页面的后一页插入指定页面到滑动列表

@apiParam {string{"PAGE_0", "PAGE_1", "PAGE_2", ...}} PAGE 下一页页面名称

@apiParamExample 示例:
gui:pageSlideNext("PAGE_10")

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function gui:pageSlideNext(page)
	-- 正则匹配是否为页面
	local s, e, str
	s, e, str = string.find(page, '(%bPAGE_)%d')
	if ((s ~= 1) and (str ~= 'PAGE_')) then
		log_e('checkPreviousPage', 'arg0 is not a page name!')
		log_refer('"PAGE_0", "PAGE_1", "PAGE_2" etc.')
		return
	end
    log:d("pageSlideNext: "..page)
    slide:insertNextPageByName(page)
end



--[[
@api {SET} gui:pageSlidePrev(PAGE) 插入上一页
@apiName pageSlidePrev()
@apiGroup GuiGroup
@apiVersion 1.0.0
@apiDescription 在当前页面上一页插入指定页面到滑动列表

@apiParam {string{"PAGE_0", "PAGE_1", "PAGE_2", ...}} PAGE 上一页页面名称

@apiParamExample 示例:
gui:pageSlidePrev("PAGE_7")

@apiSuccessExample {number} 成功响应: 
无

@apiErrorExample {number} 失败响应:
无
]]
function gui:pageSlidePrev(page)
	-- 正则匹配是否为页面
	local s, e, str
	s, e, str = string.find(page, '(%bPAGE_)%d')
	if ((s ~= 1) and (str ~= 'PAGE_')) then
		log_e('checkPreviousPage', 'arg0 is not a page name!')
		log_refer('"PAGE_0", "PAGE_1", "PAGE_2" etc.')
		return
	end
    log:d("pageSlidePrev: "..page)
    slide:insertPrevPageByName(page)
end



--[[
@api {SET} obj=gui:getComponentByName(ename) 创建控件实例
@apiName getComponentByName()
@apiGroup GuiGroup
@apiVersion 1.0.0
@apiDescription 创建控件实例（注意：所有控件都用这个方法创建实例）

@apiParam {string} ename 控件名称

@apiParamExample 示例:
text = gui:getComponentByName("TEXT_TEST")     -- 获取控件名为TEXT_TEST的文本控件实例
text:setText("hello")   -- 让控件TEXT_TEST显示字符串“hello”

@apiSuccessExample {number} 成功响应: 
重构获取控件对象，不同类型的控件包含不同的方法

@apiErrorExample {number} 失败响应:
获取到对象内容为 nil
]]
function gui:getComponentByName(ename)
    if (not log:checkArgType("getComponentByName", 1, ename, "string")) then
        log:i("This function only allows the control name to be passed in")
        return
    end

	-- 获取控件类型
	local ctr_type = ui:getControlType(ename)
	
	log:d('get component by name: '..ctr_type)
	
	-- 创建self table
	local self = {}
	
	-- 根据控件类型让gui继承对应控件库
	if (ctr_type == 'Button') then
        -- 按钮
		setmetatable(self, {__index = Button})
        setmetatable(Button, {__index = obj})
    elseif (ctr_type == 'NewLayout') then
        -- 布局
        setmetatable(self, {__index = Layout})
        setmetatable(Layout, {__index = obj})
    elseif (ctr_type == 'ImageList') then
        -- 图片
        self._item = 0
        setmetatable(self, {__index = Image})
        setmetatable(Image, {__index = obj})
    elseif (ctr_type == 'Battery') then
        -- 电池电量
        setmetatable(self, {__index = Battery})
        setmetatable(Battery, {__index = obj})
    elseif (ctr_type == 'Number') then
        -- 数字
        setmetatable(self, {__index = Number})
        setmetatable(Number, {__index = obj})
    elseif (ctr_type == 'Text') then
        -- 文本
        setmetatable(self, {__index = Text})
        setmetatable(Text, {__index = obj})
    elseif (ctr_type == 'Time') then
        -- 时间
        setmetatable(self, {__index = Time})
        setmetatable(Time, {__index = obj})
    elseif (ctr_type == 'watch') then
        -- 表盘
        setmetatable(self, {__index = Watch})
        setmetatable(Watch, {__index = obj})
    elseif (ctr_type == 'vslider') or (ctr_type == "slider") then
        -- 进度条
        setmetatable(self, {__index = Slider})
        setmetatable(Slider, {__index = obj})
    elseif (ctr_type == 'progressbar') then
        -- 圆弧
        setmetatable(self, {__index = Progress})
        setmetatable(Progress, {__index = obj})
    elseif (ctr_type == 'multiprogressbar') then
        -- 多重圆弧
        setmetatable(self, {__index = Multiprogress})
        setmetatable(Multiprogress, {__index = obj})
    elseif (ctr_type == 'compass') then
        -- 指南针
        setmetatable(self, {__index = Compass})
        setmetatable(Compass, {__index = obj})
	end
	
	local obj = {}
	if ((ctr_type == "VerticalList") or (ctr_type == "HorizontalList")) then
		-- 列表控件
		setmetatable(self, {__index = Grid})
		setmetatable(Grid, {__index = obj})
		obj = ui:getScrollListByName(ename)
	elseif (ctr_type == "NewGrid") then
		-- 表格控件
		setmetatable(self, {__index = Grid})
		setmetatable(Grid, {__index = obj})
		obj = ui:getScrollGridByName(ename)
	else
		-- 使用ui库创建方法，用于重封装
		obj = ui:getComponentByName(ename)
	end
	
	-- 保存控件名称和类型，注意：这两参数只用于模拟器调试时的打印
	self._ename = ename
	self._type  = ctr_type
	
	-- 控件id，在小机SDK中会保存控件ID
	self.__id = 0x5a5a5a5a
	
	-- 控件高亮状态标志
	self._highlight = false
	
    -- self table的obj继承ui库
    if (not obj) then
        log:e("function \"getComponentByName\" create object error!, please check control: "..tostring(ename))
        return nil
    else
        self.obj = obj
    end
	
	return self
end


-- 根据路径获取文件名称和类型
local function lua_get_file_name_and_type(path)
    local file_name = ""
    local file_type = ""

    -- 获取文件完整名称
    local file = path:match(".+/([^/]*%.%w+)$")

    -- 去除文件扩展名
    local idx  = file:match(".+()%.%w+$")
    if (idx) then
        file_name = file:sub(1, idx - 1)
    else
        file_name = file
    end

    -- 获取文件扩展名
    file_type = path:match(".+%.(%w+)$")
    -- print(file, idx, file_name, file_type)
    return file_name, file_type
end



--[[
@api {SET} gui:dynamicUILoading(ui_table) 动态加载UI页面
@apiName dynamicUILoading()
@apiGroup GuiGroup
@apiVersion 1.0.0
@apiDescription 动态加载UI页面，主要用于跨工程页面的加载，入设置界面，消息界面等

@apiParam {table{tab_file, page_id, layout_id, parent_id}} ui_table 待加载的页面信息，使用table形式

@apiParamExample 示例:
暂空

@apiSuccessExample {number} 成功响应: 
暂空

@apiErrorExample {number} 失败响应:
暂空
]]
function gui:dynamicUILoading(ui_table)
    -- 参数检查
    if (not log:checkArgType("dynamicUILoading", 1, ui_table, "table")) then
        return
    end

    -- table里面的参数检查
    if (not log:checkArgType("dynamicUILoading", "tab_file", ui_table.tab_file, "string")) then
        return
    end
    if (not log:checkArgType("dynamicUILoading", "page_id", ui_table.page_id, "number")) then
        return
    end
    if (not log:checkArgType("dynamicUILoading", "layout_id", ui_table.layout_id, "number")) then
        return
    end
    if (not log:checkArgType("dynamicUILoading", "parent_id", ui_table.parent_id, "number")) then
        return
    end

    log:d(ui_table.tab_file)
    -- 解析路径，确定使用的是什么工程
    local file_name, file_type = lua_get_file_name_and_type(ui_table.tab_file)

    -- 获取当前页面id
    local id = ui:getCurrPageID()
    local current_page = utils:tranHexAddrToControlID(id).pageid

    -- 获取目标页面id
    id = ui_table.page_id
    local target_page = utils:tranHexAddrToControlID(id).pageid

    -- 根据文件名称找到对应工程加载页面替换
    if (file_name == "sidebar") then
        ui:replaceProjectPage(gui.project_list.sidebar_interface, target_page, current_page)
        ui:switchPage(0)
    elseif (file_name == "update") then
        ui:replaceProjectPage(gui.project_list.mode_interface, target_page, current_page)
        ui:switchPage(0)
    -- elseif (file_name == "dial") then
        -- ui:replaceProjectPage(gui.project_list.sidebar_interface, target_page, current_page)
        -- ui:switchPage(0)
    else
        log_e("dynamicUILoading", "unknow project path: "..ui_table.tab_file)
    end

    self.loadLayout = ui_table.layout_id
    self.loadPage = target_page
    self.currPage = current_page
end



--[[
@api {SET} gui:dynamicUIRealese(layout_id) 释放动态页面
@apiName dynamicUIRealese()
@apiGroup GuiGroup
@apiVersion 1.0.0
@apiDescription 释放动态加载的页面

@apiParamExample 示例:
暂空

@apiSuccessExample {number} 成功响应: 
暂空

@apiErrorExample {number} 失败响应:
暂空
]]
function gui:dynamicUIRealese(layout_id)
    if (not log:checkArgType("dynamicUIRealese", 1, layout_id, "number")) then
        return
    end
    if (layout_id ~= self.loadLayout) then
        log_e("dynamicUIRealese", "release layout: "..tostring(layout_id).." is not dyanmic load layout: "..tostring(self.loadLayout))
        return
    end
    -- print(self.currPage, self.loadPage)

    -- 用页面动态加载的方法，把原本的页面加载回来来实现页面释放的模拟效果
    ui:replaceProjectPage(gui.project_list.mode_interface, self.currPage, self.currPage)
    ui:switchPage(0)
    -- log:emptyFunction("dynamicUIRealese")
end




return gui

