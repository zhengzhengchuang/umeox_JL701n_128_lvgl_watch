--[[
	注意！ 注意！ 注意！
	
	本文档并不实际被lua导入，主要用来同步提示信息和自动补全功能
]]


--[[
@notes
@tip:utils库，包含延时函数、打印解释器版本等，但大部分只能在模拟器中看效果，小机不提供具体效果，如 help 方法等
@demo:utils:help()\n utils:version()\n utils:print(table)
]]
utils = {}


--[[
@notes
@tip:打印帮助信息，模拟器有效，小机不提供具体内容
@demo:utils:help()
]]
function utils:help()
	print("print help func")
end


--[[
@notes
@tip:打印当前lua解释器版本
@demo:utils:version()
]]
function utils:version()
	print("lua version 5.3.6")
end


--[[
@notes
@tip:打印调试信息到控制台（数字、文本、table等）
@demo:utils:print(arg)
]]
function utils:print()
	print('utils module print')
end

return utils