--[[
-- 表格控件方法
@apiDefine GridGroup 表格控件
--]]

Grid = {}


-- 动态列表标志，将静态列表初始化为动态列表后，该标志会被置为true
Grid.dynamic = false



--[[
@api {SET} obj:initGridDynamic(row,col[,redraw]) 动态列表
@apiName initGridDynamic()
@apiGroup GridGroup
@apiVersion 1.0.0
@apiDescription 将静态列表初始化为动态列表，当需要用列表显示的内容较多时，可以考虑使用动态列表的方式实现，但需注意，动态列表效率不如静态列表

@apiParam {number} row 动态列表行数
@apiParam {number} col 动态列表列数
@apiParam {boolean{true, false}} [redraw] 是否立即刷新，缺省时立即刷新

@apiParamExample 示例:
vlist = gui:getComponentByName("VLIST_TEST")    -- 创建垂直列表实例
vlist:initGridDynamic(3, 1, false)      -- 将静态列表初始化为3行1列的动态列表，且不立即刷新

@apiSuccessExample {number} 成功响应: 
静态列表成功被初始化为动态列表

@apiErrorExample {number} 失败响应:
静态列表未被设置未圆弧列表显示
]]
function Grid:initGridDynamic(...)
    local arg = {...}
    local arg_num = #arg

    local row = 0
    local col = 0
    local redraw = true

    if (not log:checkArgType("initGridDynamic", 1, arg[1], "number")) then
        return
    end
    if (not log:checkArgType("initGridDynamic", 2, arg[2], "number")) then
        return
    end

    if (arg_num == 2) then
        row = arg[1]
        col = arg[2]
    elseif (arg_num == 3) then
        row = arg[1]
        col = arg[2]
        if (not log:checkArgType("initGridDynamic", 3, arg[3], "boolean")) then
            return
        end
        redraw = arg[3]
    else
        log:e("function \"initGridDynamic\" arg number must be 2 or 3")
        return
    end

	log:d('initGridDynamic: [ '..self._ename..' ], type: '..self._type .. ", init ( "..tostring(row) .. " x " .. tostring(col).." ), ".."redraw: "..tostring(redraw))
    local sub_ctrl_num = self.obj:getSubControlsNumber()
    log:d("base sub controls number is "..tostring(sub_ctrl_num))

    if (sub_ctrl_num <= 0) or (row <= 0) then
        log:w("base sub controls or arg \"row\" must be > 0")
    end
    self.obj:initDynamicItem()

    if (self._type == "VerticalList") then
        -- 垂直列表
        row = row - sub_ctrl_num
        col = 0 --col - 1
    elseif (self._type == "HorizontalList") then
        -- 水平列表
        row = 0 --row - 1
        col = col - sub_ctrl_num
    elseif (self._type == "NewGrid") then
        -- 表格控件
        sub = self.obj:getDynamicInitItem()
        row = row - sub.row
        col = col - sub.col
    end
    print(self._type, row, col)
    self.obj:appendDynamicItem(row, col)

    -- 注册上下滑动事件
    self.obj:bindDynamicVerticalEvent(function(row, col)
        print(">>>>>>>>>>>>>> v Grid".. row.." "..col)
        self.obj:hide()
        self.obj:show()
    end)

    --[[
    if (sub_ctrl_num < row) then
        for i = sub_ctrl_num, row - 1, 1 do
            -- 初始化比原本控件少，把最后一个子控件拷贝到末尾
            -- print("copy: "..tostring(i))
            self.obj:copySubControlItemByIndex(sub_ctrl_num - 1)
        end
    elseif (sub_ctrl_num > row) then
        while (true) do
            sub_ctrl_num = self.obj:getSubControlsNumber()
            if (sub_ctrl_num > row) then
                -- 初始化比原本控件多，删除row到sub_ctrl_num的子控件
                -- print("remove: "..tostring(sub_ctrl_num))
                self.obj:removeSubControlItemByIndex(sub_ctrl_num - 1)
            else
                break
            end
        end
    end
    ]]
    self.dynamic = true
end



--[[
@api {SET} obj:addGridDynamic(row,col[,redraw]) 添加列表项
@apiName addGridDynamic()
@apiGroup GridGroup
@apiVersion 1.0.0
@apiDescription 动态列表末尾添加项数。注意，仅当列表转为动态列表后接口才生效

@apiParam {number} row 添加的行数
@apiParam {number} col 添加的列数
@apiParam {boolean{true, false}} [redraw] 是否立即刷新，缺省时立即刷新

@apiParamExample 示例:
vlist = gui:getComponentByName("VLIST_TEST")    -- 创建垂直列表实例
vlist:initGridDynamic(0, 0, false)      -- 初始化为动态列表
vlist:addGridDynamic(3, 0, true)    -- 添加3行0列项，立即刷新

@apiSuccessExample {number} 成功响应: 
动态列表项数增加

@apiErrorExample {number} 失败响应:
动态列表项数未增加，或者显示异常
]]
function Grid:addGridDynamic(...)
    local arg = {...}
    local arg_num = #arg

    if (not self.dynamicGrid) then
        -- 如果没有初始化成动态列表
        log:e("dynamic grid controls not init")
        return
    end

    local row = 0
    local col = 0
    local redraw = true

    if (not log:checkArgType("addGridDynamic", 1, arg[1], "number")) then
        return
    end
    if (not log:checkArgType("addGridDynamic", 2, arg[2], "number")) then
        return
    end

    if (arg_num == 2) then
        row = arg[1]
        col = arg[2]
    elseif (arg_num == 3) then
        row = arg[1]
        col = arg[2]
        redraw = arg[3]
        if (not log:checkArgType("addGridDynamic", 3, arg[3], "boolean")) then
            return
        end
    else
        -- 限制参数只能是两个或者三个
        log:e("function \"addGridDynamic\" arg number must be 2 or 3")
        return
    end

	log:d('addGridDynamic: [ '..self._ename..' ], type: '..self._type .. ", add ( "..tostring(row) .. " x " .. tostring(col).." ), ".."redraw: "..tostring(redraw))

    self.obj:appendDynamicItem(row, col)
    --[[
    local sub_ctrl_num = self.obj:getSubControlsNumber()
    log:d("base sub controls number is "..tostring(sub_ctrl_num))
    if (sub_ctrl_num <= 0) or (row < 0) then
        log:e("base sub controls or arg \"row\" must be > 0")
        return 
    end
    for i = sub_ctrl_num, sub_ctrl_num + row - 1, 1 do
        print("add: "..tostring(i))
        self.obj:copySubControlItemByIndex(sub_ctrl_num - 1)
    end
    ]]
end




--[[
@api {SET} obj:delGridDynamic(row,col[,redraw]) 删除列表项
@apiName delGridDynamic()
@apiGroup GridGroup
@apiVersion 1.0.0
@apiDescription 删除动态列表的项数，从列表末尾开始删除，删除后，列表能上下滑动的项数减少

@apiParam {number} row 需要删除的行数
@apiParam {number} col 需要删除的列数
@apiParam {boolean{true, false}} [redraw] 是否立即刷新，缺省时立即刷新

@apiParamExample 示例:
vlist = gui:getComponentByName("VLIST_TEST")    -- 创建垂直列表实例
vlist:initGridDynamic(5, 1, true)   -- 初始化为动态列表
vlist:delGridDynamic(3, 0, true)    -- 删除3行

@apiSuccessExample {number} 成功响应: 
列表项数减少

@apiErrorExample {number} 失败响应:
列表项数未减少，或者显示异常
]]
function Grid:delGridDynamic(...)
    local arg = {...}
    local arg_num = #arg

    if (not self.dynamicGrid) then
        -- 如果没有初始化成动态列表
        log:e("dynamic grid controls not init")
        return
    end

    local row = 0
    local col = 0
    local redraw = true

    if (not log:checkArgType("delGridDynamic", 1, arg[1], "number")) then
        return
    end
    if (not log:checkArgType("delGridDynamic", 2, arg[2], "number")) then
        return
    end

    if (arg_num == 2) then
        row = arg[1]
        col = arg[2]
    elseif (arg_num == 3) then
        row = arg[1]
        col = arg[2]
        redraw = arg[3]
        if (not log:checkArgType("delGridDynamic", 3, arg[3], "boolean")) then
            return
        end
    else
        -- 限制参数只能是两个或者三个
        log:e("function \"delGridDynamic\" arg number must be 2 or 3")
        return
    end

    log:d('delGridDynamic: [ '..self._ename..' ], type: '..self._type .. ", del ( "..tostring(row) .. " x " .. tostring(col).." ), ".."redraw: "..tostring(redraw))

    self.obj:removeDynamicItem(row, col)
    --[[
    local base_sub_num = self.obj:getSubControlsNumber()
    if (base_sub_num <= 0) then
        log:e("base sub controls number has <= 0")
        return
    end
    for i = base_sub_num - 1, base_sub_num - row, -1 do
        if (i < 0) then
            break
        end
        log:d("delete sub: ".. tostring(i))
        self.obj:removeSubControlItemByIndex(i)
    end
    base_sub_num = self.obj:getSubControlsNumber()
    if (base_sub_num <= 0) then
        log:w("base sub controls number will be < 0")
    end
    ]]
end



--[[
@api {GET} item=obj:getGridCurItem() 获取当前刷新项
@apiName getGridCurItem()
@apiGroup GridGroup
@apiVersion 1.0.0
@apiDescription 获取动态列表当前刷新项的序号，用于动态列表子项内容刷新时，确定需显示的内容

@apiParamExample 示例:
vlist = gui:getComponentByName("VLIST_TEST")    -- 创建垂直列表实例
vlist:initGridDynamic(5, 1, true)
cur_item = vlist:getGridCurItem()   -- 获取当前刷新项，仅动态列表有效

@apiSuccessExample {number} 成功响应: 
获取到正确的当前刷新项值

@apiErrorExample {number} 失败响应:
获取到值为 -1
]]
function Grid:getGridCurItem()
    local current_item = -1 

    -- 动态列表才有这个项
    if (self.dynamic) then
        current_item = self.obj:getMiddleScrollAreaItemID()
    else
        log:e("dynamic grid controls not init")
    end

    return current_item
end



--[[
@api {SET} obj:setHighLightItem(item[,redraw]) 设置高亮项
@apiName setHighLightItem()
@apiGroup GridGroup
@apiVersion 1.0.0
@apiDescription 设置动态列表高亮项

@apiParam {number} item 需高亮的项
@apiParam {boolean{true, false}} [redraw] 是否立即刷新，缺省时立即刷新

@apiParamExample 示例:
vlist = gui:getComponentByName("VLIST_TEST")    -- 创建垂直列表实例
vlist:setHighLightItem(5)   -- 设置第五项为高亮，立即刷新

@apiSuccessExample {number} 成功响应: 
对应项显示高亮状态

@apiErrorExample {number} 失败响应:
设置高亮状态无响应
]]
function Grid:setHighLightItem(item, redraw)
    -- 检查参数类型
    if (not log:checkArgType("setHighLightItem", 1, item, "number")) then
        return
    end
    if (not log:checkArgType("setHighLightItem", 2, redraw, "nil", "boolean")) then
        return
    end

    local sub_num = self.obj:getSubControlsNumber()
    if (item < 1) or (item >= sub_num) then
        log_e("setGridDynamicHiIndex", "arg must between 1 to "..tostring(sub_num))
        return 
    end

    if (self.dynamic) then
        -- 动态列表
        self.obj:setMiddleScrollAreaItemID(item)
    else
        -- 静态列表
    end
end



--[[
@api {GET} item=obj:getHighLightItem() 获取高亮项
@apiName getHighLightItem()
@apiGroup GridGroup
@apiVersion 1.0.0
@apiDescription 获取动态列表高亮项

@apiParamExample 示例:
vlist = gui:getComponentByName("VLIST_TEST")    -- 创建垂直列表实例
highlight_item = vlist:getHighLightItem()

@apiSuccessExample {number} 成功响应: 
返回值与当前高亮状态的项一致

@apiErrorExample {number} 失败响应:
返回值与当前高亮状态项不一致，或者返回 -1
]]
function Grid:getHighLightItem()
    local highlight_item = 0

    if (self.dynamic) then
        highlight_item = self.obj:getMiddleScrollAreaItemID()
    else
    end

    return highlight_item
end



--[[
@api {GET} item=obj:getTouchItem() 获取触摸项
@apiName getTouchItem()
@apiGroup GridGroup
@apiVersion 1.0.0
@apiDescription 获取列表控件被触摸项

@apiParamExample 示例:
vlist = gui:getComponentByName("VLIST_TEST")    -- 创建垂直列表实例
touch_item = vlist:getTouchItem()

@apiSuccessExample {number} 成功响应: 
返回值与touch项一致

@apiErrorExample {number} 失败响应:
返回值与touch项不一致
]]
function Grid:getTouchItem()
    local touch_item = 0
    log:d("getTouchItem: "..tostring(touch_item))
    return touch_item
end



--[[
@api {SET} obj:setScrollarea(left,top,right,bottom) 设置滑动区域
@apiName setScrollArea()
@apiGroup GridGroup
@apiVersion 1.0.0
@apiDescription 设置列表控件的滑动区域。注意，此方法仅在设备端有效，模拟器端不生效

@apiParam {number} left 滑动区域相对于父控件的X左边
@apiParam {number} top 滑动区域相对于父控件的Y左边
@apiParam {number} right 滑动区域宽度
@apiParam {number} bottom 滑动区域高度

@apiParamExample 示例:
vlist = gui:getComponentByName("VLIST_TEST")    -- 创建垂直列表实例
vlist:setScrollArea(0, 0, 10000, 10000)     -- 设置全屏区域滑动，10000相对于全屏像素

@apiSuccessExample {number} 成功响应: 
列表控件在指定区域内滑动

@apiErrorExample {number} 失败响应:
列表控件可以滑出指定区域外
]]
function Grid:setScrollArea(left, top, right, bottom)
    -- 参数类型检查
    if (not log:checkArgType("setScrollArea", 1, left, "number")) then
        return
    end
    if (not log:checkArgType("setScrollArea", 2, top, "number")) then
        return
    end
    if (not log:checkArgType("setScrollArea", 3, right, "number")) then
        return
    end
    if (not log:checkArgType("setScrollArea", 4, bottom, "number")) then
        return
    end
    log:d("setScrollArea: ("..tostring(left)..", "..tostring(top)..", "..tostring(right)..", "..tostring(bottom)..")")
end



--[[
@api {SET} obj:setSliderDir(dir) 设置滑动方向
@apiName setSliderDir()
@apiGroup GridGroup
@apiVersion 1.0.0
@apiDescription 设置列表控件的滑动方向，此方法仅在设备端有效，模拟器不生效

@apiParam {number{DB.SCROLL_DIRECTION_LR, DB.SCROLL_DIRECTION_UP}} dir 滑动方向

@apiParamExample 示例:
vlist = gui:getComponentByName("VLIST_TEST")    -- 创建垂直列表实例
vlist:setSliderDir(DB.SCROLL_DIRECTION_UP)      -- 设置垂直列表仅上下滑动

@apiSuccessExample {number} 成功响应: 
列表控件仅可在设置方向上滑动

@apiErrorExample {number} 失败响应:
列表控件可在设置方向之外滑动
]]
function Grid:setSliderDir(dir)
    -- 参数类型检查
    if (not log:checkArgType("setSliderDir", 1, dir, "number")) then
        return
    end
    log:d("setSliderDir, dir:"..tostring(dir))
end



--[[
@api {SET} obj:setItemNumber(number) 静态列表项数
@apiName setItemNumber()
@apiGroup GridGroup
@apiVersion 1.0.0
@apiDescription 设置静态列表项数，当UI编辑工具设计的静态列表项数较多，而实际需显示较少时使用此方法

@apiParam {number} number 静态列表的目标项数，目标项数必须小于设计的列表项数

@apiParamExample 示例:
vlist = gui:getComponentByName("VLIST_TEST")    -- 创建垂直列表实例
vlist:setItemNumber(15)     -- 设置静态列表项数为15项

@apiSuccessExample {number} 成功响应: 
列表控件项数被设置为指定项数

@apiErrorExample {number} 失败响应:
列表控件项数无变化
]]
function Grid:setItemNumber(number)
    -- 参数类型检查
    if (not log:checkArgType("setItemNumber", 1, number, "number")) then
        return
    end

    -- 要设置的子控件数量不能小于0
    if (number < 0) then
        log_e("setItemNumber", "arg number must be > 0")
        return
    elseif (number == 0) then
        log_w("setItemNumber", "grid sub item will be 0")
    end

    -- 获取子控件数量
    local base_sub_num = self.obj:getSubControlsNumber()

    -- 如果设置的item数量大于子控件数量，则退出
    if (number > base_sub_num) then
        log:w("set item number: "..tostring(number).." bigger then base item number: "..tostring(base_sub_num))
        return 
    end

    -- 从最后一个子控件开始，移除超过设置数量的子控件
    log:d("set static grid item number: "..tostring(number))
    for i = base_sub_num - 1, number, -1 do
        -- print("remove: "..tostring(i))
        self.obj:removeSubControlItemByIndex(i)
    end
end




--[[
@api {SET} obj:setGridToRotate(rotate) 圆弧列表
@apiName setGridToRotate()
@apiGroup GridGroup
@apiVersion 1.0.0
@apiDescription 设置列表控件为圆弧列表

@apiParam {boolean{true, false}} [rotate] 圆弧设置开关，缺省时设置为圆弧列表

@apiParamExample 示例:
vlist = gui:getComponentByName("VLIST_TEST")    -- 创建垂直列表实例
vlist:setGridToRotate(true)     -- 设置为圆弧列表

@apiSuccessExample {number} 成功响应: 
垂直列表变为圆弧列表形式显示

@apiErrorExample {number} 失败响应:
垂直列表未显示为圆弧列表，或者显示异常
]]
function Grid:setGridToRotate(rotate)
    -- 参数类型检查
    if (not log:checkArgType("setGridToRotate", 1, rotate, "number")) then
        return
    end

    -- 参数省略或传入true，则将垂直列表设置为圆弧列表
    if (rotate == nil) or rotate then
        local rotate_size = 30  -- 弯曲大小，大于0向右弯曲，小于0向左弯曲
        self.obj:setScrollListRotateSize(rotate_size)
        log:d("set v list rotate")
    end
end




return Grid
