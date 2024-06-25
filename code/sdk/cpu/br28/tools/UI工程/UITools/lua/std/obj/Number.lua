--[[
-- 数字控件方法
@apiDefine NumberGroup 数字控件
--]]
Number = {}


-- 拆分整数的每一位
local function get_split_inter(num, cnt)
    if not num or not cnt then
        return nil
    end

    local tb = {}   -- 存放拆分的数字
    local temp_num = num
    repeat
        table.insert(tb, num % 10)
        num = math.floor(num / 10)
    until(num == 0)

    local tmp = {}
    local tb_index = 1
    local tb_number = #tb

    -- tb倒序，并在高位补0
    for i = cnt, 1, -1 do
        if tb_index <= tb_number then
            tmp[i] = tb[tb_index]
            tb_index = tb_index + 1
        else
            tmp[i] = 0
        end
    end

    return tmp
end



--[[
@api {SET} obj:setNumber(num1,num2,redraw) 设置数字
@apiName setNumber()
@apiGroup NumberGroup
@apiVersion 1.0.0
@apiDescription 设置数字控件显示数字

@apiParam (显示单个数字) {number} num1 待显示数字
@apiParam (显示单个数字) {boolean{true, false}} [redraw] 是否立即刷新，缺省时立即刷新

@apiParam (显示两个数字) {number} num1 待显示的左边数字
@apiParam (显示两个数字) {number} num2 待显示的右边数字
@apiParam (显示两个数字) {boolean{true, false}} [redraw] 是否立即刷新，缺省时立即刷新

--api_Param (显示字符串) {string} str 待显示字符串
--api_Param (显示字符串) {boolean{true, false}} [redraw] 是否立即刷新，缺省时立即刷新

@apiParamExample 示例:
test_number = gui:getComponentByName("NUMBER_TEST")

-- 显示单个数字，立即刷新
test_number:setNumber(10)

-- 显示两个数字，不立即刷新
test_number:setnumber(20, 32, false)

@apiSuccessExample {number} 成功响应: 
数字控件显示指定数值

@apiErrorExample {number} 失败响应:
数字控件未显示指定数值
]]
function Number:setNumber(...)
    local arg = {...}
    local arg_num = #arg

    if (not log:checkArgType("setNumber", 1, arg[1], "number", "string")) then
        return
    end

    local num1 = 0
    local num2 = 0
    local str = ""
    local redraw = true

    -- 解析参数
    if (arg_num < 1) or (arg_num > 3) then
        log_e("setNumber", "arg number err")
        return
    end

    -- 获取图片列表中图片的数量
    local img1_list_num = self.obj:getImageNumber(0)
    local img2_list_num = self.obj:getImageNumber(1)
    local img3_list_num = self.obj:getImageNumber(2)

    -- 判断是否用图片显示
    local show_image = false
    if (img1_list_num > 0) then
        show_image = true
    end

    num1 = arg[1]

    -- 如果传入字符串
    if (type(num1) == "string") then
        if (arg_num == 2) then
            str = num1
            redraw = arg[2]
        else
            str = num1
        end

        -- 如果用图片显示
        if (show_image) then
            -- 字符串长度
            local str_len = #str
            local str_img = {}
            -- 解析字符串
            for i = 1, str_len, 1 do
                local val = string.sub(str, i, i)
                local pic = 0
                local idx = tonumber(val)

                if not idx then
                    if val == " " then
                        pic = 2
                        idx = 0
                    else
                        pic = 1
                        idx = 0
                    end
                end
                local pic_idx = {}
                pic_idx.pic = pic
                pic_idx.idx = idx
                table.insert(str_img, pic_idx)
            end
            self.obj:setTextByImageList(str_img)
        else
            self.obj.setText(str)
        end

        log:d("setNumber", "string: "..tostring(str)..", redraw: "..tostring(redraw))
        return
    end

    if (arg_num == 2) then
        num2 = arg[2]
        if (type(num2) == "boolean") then
            redraw = num2
            num2 = nil
            arg_num = 1
        end
    elseif (arg_num == 3) then
        num2 = arg[2]
        redraw = arg[3]
    end

    -- 数据格式，后面根据格式配置显示内容
    local data_format = self.obj:getAttrTextByIndex(1)
    local show_format = {}  -- 显示格式
    local pre_c = ""
    local right = false

    for i = 1, #data_format do
        -- 字符迭代
        local c = string.sub(data_format, i, i)
        local num = tonumber(c)
        if (num and num ~= 0) then
            -- 如果是数字，且不等于0
            if (right) then
                show_format.right_number = num
            else
                show_format.left_number = num
                right = true
            end
        end

        if (c == "%") and (i ~= 1) then
            -- 保存间隔符
            show_format.spacer = pre_c
        end
        pre_c = c -- 记录上一个字符，如果传入两个数字时要用来判断间隔符
    end

    if not show_format.left_number or (show_format.left_number == 0) then
        -- 默认4位
        show_format.left_number = 4
    end

    -- 显示配置
    if (show_image) then
        local num1_tab = get_split_inter(num1, show_format.left_number)
        local num2_tab = {}

        -- if (arg_num == 1) then
        if not num2 then
            num2_tab = nil
        else
            num2_tab = get_split_inter(num2, show_format.right_number)
        end

        local pic_img = {}
        for k, v in ipairs(num1_tab) do
            local tb = {}
            tb.pic = 0
            tb.idx = v
            table.insert(pic_img, tb)
        end

        if num2_tab then
            local cm = {}
            cm.pic = 1
            cm.idx = 0
            table.insert(pic_img, cm)
            for k, v in ipairs(num2_tab) do
                local tb = {}
                tb.pic = 0
                tb.idx = v
                table.insert(pic_img, tb)
            end
        end
        self.obj:setTextByImageList(pic_img)
    else
        if (arg_num == 1) then
            self.obj:setNumber(num1)
        elseif (arg_num == 2) then
            local msg = tostring(num1) .. tostring(show_format.spacer) .. tostring(num2)
            self.obj:setText(msg)
        end
    end
    log:d("setNumber", "num1: "..tostring(num1)..", num2: "..tostring(num2)..", redraw: "..tostring(redraw))
end



--[[
@api {GET} num=obj:getNumber() 获取数字
@apiName getNumber()
@apiGroup NumberGroup
@apiVersion 1.0.0
@apiDescription 获取数字控件正在显示的数字

@apiParamExample 示例:
test_number = gui:getComponentByName("NUMBER_TEST")
number = test_number:getNumber()

@apiSuccessExample {number} 成功响应: 
获取到数字控件显示的数值

@apiErrorExample {number} 失败响应:
获取到数值为 nil
]]
function Number:getNumber()
	local num = self.obj:getNumber()
	log:d('ename: [ '..self._ename..' ] getNumber: ' .. num)
	return math.floor(num)
end





return Number

