--[[

注意！ 注意！ 注意！

这个文件仅用于生成 JLLua.json 文件，生成的 json 文件用于 ui 工具编辑 lua 代码时的自动补全
和信息提示。不是 lua 功能模块，禁止将本文件导入到 UI 工程内使用。



使用本文件生成 JLLua.json 文件方法：

1、在 std/gui.lua 中添加新方法后，按照之前有的注释添加 @notes。格式为

@notes
@tip:这里写方法说明
@demo:这里写方法使用示例

-- 具体方法函数必须紧接着块注释的后一行

2、在 UI编辑工具 中启动 lua模拟器，在模拟器中运行以下代码：
require("buildJson")
buildJson:main()

]]


-- 通过模拟器执行的绝对路径
local rootPath = "../../../UITools/lua/"



-- 文件路径表
local path_table = {
	build_file = "buildJson.lua",
	json_file = "JLLua.json",
	db_file = "DB.lua",
	
	lib_file = {
		"module.lua",
		"std/gui.lua",
		"std/bsp.lua",
		"std/bt.lua",
		"std/music.lua",
		"std/device.lua",
		"std/utils.lua"
	}
}


-- json关键字表
local json_keyword = {
	mdo_s = "\t\t\"module\": \"",
	typ_s = "\t\t\"type\": \"",
	pri_s = "\t\t\"priority\": \"",
	key_s = "\t\t\"key\": \"",
	tip_s = "\t\t\"tip\": \"",
	dem_s = "\t\t\"demo\": \"",
	line_e = "\",\n",
	block_e = "\"\n"
}


-- 忽略字符串头部的空白符
local function ltrim(str)
	return (string.gsub(str, "^[ \t\n\r]*", ""))
end

-- 忽略字符串尾部的空白符
local function rtrim(str)
	return (string.gsub(str, "[ \t\n\r]*$", ""))
end

-- 忽略字符串首尾的空白符
local function trim(str)
	return (string.gsub(str, "^%s*(.-)%s*$", "%1"))
end

-- 转义双引号
local function escapeQM(str)
	local s, n = string.gsub(str, "\"", "\\\"");
	return s;
end

-- 写一行指定内容到文件里
local function fileWriteLine(file, line)
	file:write(line)
end

-- 保存块结束到json文件
local function jsonFileSaveTableEnd(JsonFile)
	if JsonFile then
		JsonFile:write(",\n")
	end
end


-- 保存table到json文件
local function saveTableToJsonFile(tab, JsonFile)
	local line = "";
	JsonFile:write("\t{\n")
	
	line = json_keyword.mdo_s .. tab.json_module .. json_keyword.line_e;
	fileWriteLine(JsonFile, line)
	
	line = json_keyword.typ_s .. tab.json_type .. json_keyword.line_e;
	fileWriteLine(JsonFile, line)
	
	line = json_keyword.pri_s .. tab.json_priority .. json_keyword.line_e;
	fileWriteLine(JsonFile, line)
	
	line = json_keyword.key_s .. tab.json_key .. json_keyword.line_e;
	fileWriteLine(JsonFile, line)
	
	line = json_keyword.tip_s .. escapeQM(tab.json_tip) .. json_keyword.line_e;
	fileWriteLine(JsonFile, line)
	
	line = json_keyword.dem_s .. escapeQM(tab.json_demo) .. json_keyword.block_e;
	fileWriteLine(JsonFile, line)
	
	JsonFile:write("\t}")
end


-- 扫描关键字
local function scanKeyWord(keyWord_path, JsonFile)
	local keyWordFile = io.open(keyWord_path, "r+");
	local in_keyWord = false;
	if keyWordFile then
		for line in keyWordFile:lines() do
			if string.sub(line, 1, -1) == "@keyWordEnd\r" then
				-- 扫描到结束标志，扫描结束退出
				return true;
			end

			if (in_keyWord and JsonFile) then
				-- 开始标志被置位，同时有json则保存文件
				JsonFile:write(line);
			end
			
			if (string.sub(line, 1, -1) == "@keyWordStart\r") then
				-- 扫描到开始标志，标志置位
				in_keyWord = true;
			end
		end
	else
		return nil;
	end
end


-- 扫描DB表
local function scanDBFile(DBFile_path, JsonFile)
	local DBFile = io.open(DBFile_path, "r+");
	local previous_line = ""; -- 上一行
	local json_value = {
		json_module = "DB",
		json_type = "value",
		json_priority = "1",
		json_key = "",
		json_tip = "",
		json_demo = ""
	}
	if (DBFile) then
		-- 保存 DB table
		json_value.json_key = "DB.";
		json_value.json_tip = "DB 常量定义表";
		json_value.json_demo = "DB.xxx";
		saveTableToJsonFile(json_value, JsonFile);
		jsonFileSaveTableEnd(JsonFile);
		
		json_value.json_priority = "10";
		for current_line in DBFile:lines() do
			if (string.sub(current_line, 1, 3) == "DB.") then
				-- 解析常量
				--local s, e, key = string.find(current_line, "DB.(%u+%p%u+)%s*=");
				local s, e, key = string.find(current_line, "DB.([%u+%p]*)%s*=");
				if key then
					-- 保存 key
					--json_value.json_key = "DB."..key;
					json_value.json_key = key;
				end
				local demo = string.sub(current_line, 1, -2);
				json_value.json_demo = demo;
				if (string.sub(previous_line, 1, 2) == "--") then
					-- 解析注释
					local tip = ltrim(string.sub(previous_line, 3, -2));
					json_value.json_tip = tip;
				end
				saveTableToJsonFile(json_value, JsonFile);
				jsonFileSaveTableEnd(JsonFile);
			end
			previous_line = current_line;
		end
	else
		DBFile:close();
		return nil;
	end
	DBFile:close();
end

-- ~~~~~~~~~~~~~~~~~~~~~~~~~~ 库文件扫描

local module_start = "\t\t\"module\": \""
local type_start   = "\t\t\"type\": \""
local priority	   = "\t\t\"priority\": \""
local key_start    = "\t\t\"key\": \""
local tip_start    = "\t\t\"tip\": \""
local demo_start   = "\t\t\"demo\": \""
local line_end	   = "\",\n"
local block_end	   = "\"\n"


-- 从指定传里找模块名
local function findTable(code)
	if string.find(code, "local") then
		-- 本地table不管
		return nil
	else
		local s, e = string.find(code, "=")
		if s and e then
			local mod = string.sub(code, 1, s-2)
			return mod
		else
			return nil
		end
	end
end

-- 从字符串里面找function关键字
local function findType(code)
	if string.find(code, "local") then
		-- 有local说明是本地函数，不管它
		return nil
	else
		local s, e = string.find(code, "function")
		if s and e then
			return "function"
		else
			return findTable(code)
		end
	end
end

-- 从字符串中找到标识符
local function findKey(code, isfunc)
	if string.find(code, "local") then
		-- 有local说明是本地函数，不管它
		return nil
	else
		if isfunc then
			-- 如果是函数
			local start = 0
			if string.find(code, ":") then
				-- 有冒号说明是模块内的函数
				local s0, e0 = string.find(code, ":")
				start = s0 + 1
			else
				-- 普通函数
				local s0, e0 = string.find(code, "function")
				start = e0 + 2
			end
			local s1, e1 = string.find(code, "%(")
			local key = string.sub(code, start, s1-1)
			return key
		else
			-- 否则是模块
			local s0, e0 = string.find(code, "=")
			local key = string.sub(code, 1, s0-2)
			return key
		end
	end
end

-- 查找模块名
local function findModule(code, isfunc)
	if string.find(code, "local") then
		-- 有local说明是本地函数，不管它
		return nil
	else
		if isfunc then
			if string.find(code, ":") then
				-- 函数找module
				local s0, e0 = string.find(code, "function")
				local s1, e1 = string.find(code, ":")
				local mode = string.sub(code, e0 + 2, e1 - 1)
				return mode
			else
				return " "
			end
		elseif string.find(code, "=") then
			-- 模块本身
			local s0, e0 = string.find(code, "=")
			local mode = string.sub(code, 1, s0 - 2)
			return mode
		else
			return " "
		end
	end
end


-- 保存标记块文件
local function saveNotes(json_file, code_file)
	local modul, typ, key, tip, demo
	local str
	for line in code_file:lines() do
		if (string.sub(line, 1, 2) == "]]") then
			break
		elseif (string.sub(line, 1, 5) == "@tip:") then
			-- tip
			tip = string.sub(line, 6, -2)
			str = tip_start..tip..line_end
		elseif (string.sub(line, 1, 6) == "@demo:") then
			-- demo
			demo = string.sub(line, 7, -2)
			str = string.gsub(demo, "\"", "\\\"")
			str = demo_start..str..line_end
		else
			str = nil
		end
		if str then
			fileWriteLine(json_file, str)
		end
		-- print('note: '..line)
	end
	-- 接下来一行包含module, key, type信息
	local code = code_file:read()
	-- 有function的是函数
	local typ = findType(code)
	if typ then
		str = type_start .. typ .. line_end
		fileWriteLine(json_file, str)
	end
	local isfunc = false
	if typ == "function" then
		isfunc = true
	end
	local key = findKey(code, isfunc)
	if key then
		str = key_start .. key .. line_end
		fileWriteLine(json_file, str)
	end
	local mode = findModule(code, isfunc)
	if (mode) then
		str = module_start .. mode .. line_end
		fileWriteLine(json_file, str)
	end
	if mode == "gui" then
		str = priority .. "5" .. block_end
	else
		str = priority .. "10" .. block_end
	end
	fileWriteLine(json_file, str)
	--print('code: '..code)
	
	return
end


-- 扫描库文件
local function scanLibFile(libFile_path, JsonFile)
	local scanf = io.open(libFile_path);
	if (not scanf) then
		print("open file faild! --> dir: "..dir)
		return nil
	end
	
	local note = false;
	local line = scanf:read();
	while line do
		-- 如果当前行是注释标记
		if (string.sub(line, 1, 6) == "@notes") then
			note = true;
		end
		-- 进入标记块处理
		if note then
			JsonFile:write("\t{\n");
			saveNotes(JsonFile, scanf);
			JsonFile:write("\t},\n");
			note = false;
		end
		-- 否则读取下一行
		line = scanf:read();
	end
end


-- 主函数
local function main(file_tab)
	if not scanKeyWord(file_tab.build_file) then
		print("can not find file: "..file_tab.build_file..", plsase check the file path!");
		return;
	end
	
	local JsonFile = io.open(file_tab.json_file, "w+");
	if not JsonFile then
		print("creat file: "..file_tab.json_file.." faild!");
		return;
	end
	-- 先创建文件头
	JsonFile:write("[\n");
	
	-- 保存关键字
	scanKeyWord(file_tab.build_file, JsonFile)
	
	-- 保存DB表
	scanDBFile(file_tab.db_file, JsonFile)
	
	-- 保存lib文件
	for k, v in ipairs(file_tab.lib_file) do
		scanLibFile(v, JsonFile)
	end
	
	-- 这里要做最后处理，删掉文件最后一行，追加成大括号“\t}”
	local num = JsonFile:seek("end", -4)
	-- 添加上结束大括号
	fileWriteLine(JsonFile, "\t}")
	-- 关闭文件
	JsonFile:write("\n]");
	JsonFile:close()
end



main(path_table)




-- 通过模拟器运行是建立模块
buildJson = {}

function buildJson:main()
	path_table.build_file = rootPath .. path_table.build_file;
	path_table.json_file = rootPath .. path_table.json_file;
	path_table.db_file = rootPath .. path_table.db_file;
	for k, v in ipairs(path_table.lib_file) do
		path_table.lib_file[k] = rootPath .. path_table.lib_file[k];
	end
	
	main(path_table)
end

-- 使用模拟器执行时
return buildJson








-- 下面是关键字的 json 不能动 ！！！！！！！

-- 下面内容不要动 ！！！！！！！

-- 下面内容不要动 ！！！！！！！

-- 下面内容不要动 ！！！！！！！

-- 下面内容不要动 ！！！！！！！

-- 下面内容不要动 ！！！！！！！



--[[
@keyWordStart
  {
    "module": "lua",
    "type": "keyword",
    "priority": 1,
    "key": "do",
    "tip": "Lua关键字",
    "demo": ""
  },
  {
    "module": "lua",
    "type": "keyword",
    "priority": 1,
    "key": "else",
    "tip": "Lua关键字",
    "demo": ""
  },
  {
    "module": "lua",
    "type": "keyword",
    "priority": 1,
    "key": "elseif",
    "tip": "Lua关键字",
    "demo": ""
  },
  {
    "module": "lua",
    "type": "keyword",
    "priority": 1,
    "key": "end",
    "tip": "Lua关键字",
    "demo": ""
  },
  {
    "module": "lua",
    "type": "keyword",
    "priority": 1,
    "key": "false",
    "tip": "Lua关键字",
    "demo": ""
  },
  {
    "module": "lua",
    "type": "keyword",
    "priority": 1,
    "key": "for",
    "tip": "Lua关键字",
    "demo": ""
  },
  {
    "module": "lua",
    "type": "keyword",
    "priority": 1,
    "key": "function",
    "tip": "Lua关键字",
    "demo": "function name()\n    ...\nend"
  },
  {
    "module": "lua",
    "type": "keyword",
    "priority": 1,
    "key": "if",
    "tip": "Lua关键字",
    "demo": "if (a&lt;10) then\n    ...\nelse\n    ...\nend"
  },
  {
    "module": "lua",
    "type": "keyword",
    "priority": 1,
    "key": "in",
    "tip": "Lua关键字",
    "demo": ""
  },
  {
    "module": "lua",
    "type": "keyword",
    "priority": 1,
    "key": "local",
    "tip": "Lua关键字，定义局部变量",
    "demo": ""
  },
  {
    "module": "lua",
    "type": "keyword",
    "priority": 1,
    "key": "nil",
    "tip": "Lua关键字",
    "demo": ""
  },
  {
    "module": "lua",
    "type": "keyword",
    "priority": 1,
    "key": "not",
    "tip": "Lua关键字",
    "demo": ""
  },
  {
    "module": "lua",
    "type": "keyword",
    "priority": 1,
    "key": "or",
    "tip": "Lua关键字",
    "demo": ""
  },
  {
    "module": "lua",
    "type": "keyword",
    "priority": 1,
    "key": "repeat",
    "tip": "Lua关键字",
    "demo": ""
  },
  {
    "module": "lua",
    "type": "keyword",
    "priority": 1,
    "key": "return",
    "tip": "Lua关键字",
    "demo": ""
  },
  {
    "module": "lua",
    "type": "keyword",
    "priority": 1,
    "key": "then",
    "tip": "Lua关键字",
    "demo": ""
  },
  {
    "module": "lua",
    "type": "keyword",
    "priority": 1,
    "key": "true",
    "tip": "Lua关键字",
    "demo": ""
  },
  {
    "module": "lua",
    "type": "keyword",
    "priority": 1,
    "key": "until",
    "tip": "Lua关键字",
    "demo": ""
  },
  {
    "module": "lua",
    "type": "keyword",
    "priority": 1,
    "key": "while",
    "tip": "Lua关键字",
    "demo": "while (true)\ndo\n   print(\"...\")\nend"
  },
  {
    "module": "lua",
    "type": "keyword",
    "priority": 1,
    "key": "goto",
    "tip": "Lua关键字",
    "demo": ""
  },
  {
    "module": "lua",
    "type": "keyword",
    "priority": 1,
    "key": "this",
    "tip": "Lua关键字，一般不适用this",
    "demo": ""
  },
  {
    "module": "lua",
    "type": "keyword",
    "priority": 1,
    "key": "until",
    "tip": "Lua关键字",
    "demo": ""
  },
  {
    "module": "lua",
    "type": "keyword",
    "priority": 1,
    "key": "require",
    "tip": "Lua关键字，一般不使用require导包",
    "demo": ""
  },
  {
    "module": "lua",
    "type": "keyword",
    "priority": 1,
    "key": "table",
    "tip": "Lua关键字",
    "demo": ""
  },
@keyWordEnd
]]
