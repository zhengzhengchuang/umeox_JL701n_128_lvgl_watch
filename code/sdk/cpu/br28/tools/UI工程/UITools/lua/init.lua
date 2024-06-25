--[[
-- 引入log库，包含各类debug信息的打印方法
-- 注意！log方法仅用于库内部使用，开发APP时禁止使用log方法。
-- 注意！log方法仅用于库内部使用，开发APP时禁止使用log方法。
-- 注意！log方法仅用于库内部使用，开发APP时禁止使用log方法。
--]]
require('log')



--[[
-- 引入DB表
--]]
require("DB")



--[[
-- 引入gui具体控件方法
--]]
require('std/obj/Battery')
require('std/obj/Button')
require('std/obj/Compass')
require('std/obj/Grid')
require('std/obj/Image')
require('std/obj/Layout')
require('std/obj/Multiprogress')
require('std/obj/Number')
require('std/obj/obj')
require('std/obj/Progress')
require('std/obj/Slider')
require('std/obj/Text')
require('std/obj/Time')
require('std/obj/Watch')



--[[
-- 引入模拟器库
--]]
require('std/gui')
require('std/bsp')
require('std/sys')
require('std/bt')
require('std/music')
require('std/device')



--[[
-- 引入用户自定义方法
--]]
require('module')



init = {}


-- 主工程id号
init.mainProjectId = 0

-- 启动页面
init.startPage = "PAGE_4"

-- 主页面
init.mainPage  = "PAGE_0"


-- 初始化的滑动页面列表
init.slideList = {
    "PAGE_0",   -- 表盘页面
    "PAGE_7",   -- 列表菜单界面
    "PAGE_1",   -- 运动状态界面
    "PAGE_66"   -- 天气界面
}



--[[
-- 系统定时器回调
-- 启动时会自动启动该定时器
-- 用于模拟器“系统时间”更新和闹钟扫描
--]]
local function system_timer_callback(time)
    -- 更新系统时间
    log:updateTime(sys.time, true)

    -- 检测闹钟
    for k, v in ipairs(sys.alarm) do
        if v and (v.sw == 1) then
            -- 计算当前系统日期的星期
            local w = log:zeller(time)
            -- 计算当前星期对应的mode
            local w_open = 0x01 << (w + 1)
            -- 如果闹钟的mode与当前星期一致，则判断时间是否相同
            if (w_open & v.mode) then
                if (v.hour == sys.time.hour) and (v.minute == sys.time.minute) then
                    sys:runHandler("dev_event", "alarm", 0)
                end
            end
        end
    end
end



--[[
-- 全工程模拟时的启动代码
-- 由这段代码指定启动页面和主页面切换，模拟设备启动过程
--]]
function init:start(args)
    print("start run project id: "..tostring(args.projid))

    -- 如果运行的工程是模式界面工程
    if (args.projid == self.mainProjectId) then

        -- 显示启动界面
        ui:switchPageByName(self.startPage, 0)

        -- 适当延时，模拟设备启动过程
        utils:sleep(2000)

        -- 切换到主界面
        gui:switchPageByName(self.mainPage, "hide", false)

        -- 初始化页面滑动功能
        -- slide:initPage()

        -- 把滑动页面列表添加到内置滑动列表
        -- 添加完成后，内置页面切换变成：
        -- PAGE_0 --> init.slideList[1] --> init.slideList[2] --> ... --> PAGE_0
        -- for k, v in ipairs(init.slideList) do
            -- slide:insertNextPageByName(v)
        -- end
    end

    -- 清空模拟器上的事件列表
    local event_type = {"none"}
    sim_system:setSystemEventList(event_type)

    --[[
    -- 注册系统事件回调函数回调方法
    -- 回调方法在这里注册，后续只需注册回调函数即可
    -- 模拟系统事件时，会触发这个回调去调对应的回调函数
    --]]
    sim_system:bindSystemEvent(function (key, value) 
        if (not log:checkArgType("system event type", 1, key, "string")) then
            return
        end
        sys:runHandler(key, value)
    end)
    sys.system_event_callback_register = true

    --[[
    -- 注册系统定时器
    --]]
    local sys_time = os.date("%Y%m%d%H%M%S")

    local year = string.sub(sys_time, 1, 4)
    sys.time.year = tonumber(year)

    local month = string.sub(sys_time, 5, 6)
    sys.time.month = tonumber(month)

    local day = string.sub(sys_time, 7, 8)
    sys.time.day = tonumber(day)

    local hour = string.sub(sys_time, 9, 10)
    sys.time.hour = tonumber(hour)

    local minute = string.sub(sys_time, 11, 12)
    sys.time.minute = tonumber(minute)

    local second = string.sub(sys_time, 13, 14)
    sys.time.second = tonumber(second)
    sys.timeInit = true

    sys.timer = utils:createTimer(1000, system_timer_callback, sys.time)
end



return init


