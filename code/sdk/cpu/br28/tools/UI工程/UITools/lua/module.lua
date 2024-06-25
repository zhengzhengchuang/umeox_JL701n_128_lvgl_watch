-- 文件名为 module.lua
-- 定义一个名为 module 的模块
module = {}
 
-- 定义一个常量
module.constant = "这是一个常量"
 
-- 定义一个函数
function module.func1()
	print("test")
	return 5
end
 
-- 定义一个私有函数
local function func2()
    print("这是一个私有函数！")
end
 
function module.func3()
    func2()
end
 
return module
