#!/usr/bin/python 3.0


import json
import os
import re


json_block = {
    "module": "模块名",
    "type": "接口类型",
    "priority": 0,  # 优先级
    "key": "关键字",
    "tip": "提示信息",
    "demo": "示例"
}


def ltrim(msg):
    """
    忽略字符串头部的空白符，空白符包括空格，tab制表符等
    :param msg: 准备去除头部空白符的字符串
    :return: 忽略头部空白符后的字符串
    """
    return msg.lstrip()


def rtrim(msg):
    """
    忽略字符串尾部的空白符，空白符包括空格，tab制表符等
    :param msg: 准备去除尾部空白符的字符串
    :return: 已经去除尾部空白符的字符串
    """
    return msg.rstrip()


def str_trim(msg):
    """
    忽略字符串头部和尾部的空白符
    :param msg: 待去除空白符的字符串
    :return: 已经去除空白符的字符串
    """
    return msg.strip()


def get_info_from_mark(text, start_mark, end_mark):
    """
    根据标志获取信息，自动匹配最近的start_mark开始，到最近的end_mark结束。如果没有匹配到end_mark则返回空值
    :param text: 文本内容
    :param start_mark: 起始标志
    :param end_mark: 结束标志
    :return: 起止标志之间的内容列表
    """
    text_list = text.split(start_mark)
    block_list = []
    for block in text_list:
        block_list.append(block.split(end_mark)[0])
    return block_list


def encode_str(msg):
    """
    将字符串转码成json文件的字符串，自动把双引号等内容添加转义字符
    :param msg: 待转码的字符串
    :return: 完成转码的字符串
    """
    tmp_str = json.dumps(msg, sort_keys=True, indent=4, separators=(',', ': '), ensure_ascii=False)
    return eval(tmp_str)


def save_dict_to_json_file(dict, fp, first=False):
    """
    保存字典到json文件
    :param dict: 待保存的字典
    :param fp: json文件句柄
    :param first: 是否第一个json块
    :return: None
    """
    if not first:
        fp.write(",\n")     # 如果不是第一个json块，则先写入逗号在写入下一个json块
    json.dump(dict, fp, sort_keys=True, indent=4, separators=(',', ': '), ensure_ascii=False)
    # fp.write(",\n")


"""
软件逻辑：
    1、扫描指定目录
    2、打开第一个文件
    3、根据标志获取指定字符串：get_info_from_mark
    4、字符串掐头去尾：str_trim
    5、编码字符串：encode_str
    6、将json_block字典所有值赋值完毕
    7、将字典保存到json文件
"""


def scan_files(directory, prefix=None, postfix=None):
    """
    扫描指定目录下的文件，或者匹配指定后缀和前缀
    :param directory: 待扫描路径
    :param prefix: 指定前缀
    :param postfix: 指定后缀
    :return: 文件列表
    """
    files_list = []

    for root, sub_dirs, files in os.walk(directory):
        for special_file in files:
            if postfix:
                if special_file.endswith(postfix):
                    files_list.append(os.path.join(root, special_file))
            elif prefix:
                if special_file.startswith(prefix):
                    files_list.append(os.path.join(root, special_file))
            else:
                files_list.append(os.path.join(root, special_file))

    return files_list


first_block = True      # json 第一个块标记

lua_keyWord = [{
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
  }]


def analysis_file(path, json_file):
    """
    解析文件
    :param json_file: json文件名称
    :param path: 待解析文件
    :return: None
    """
    print(path)
    module = re.compile(r'([^<>/\\\|:""\*\?]+)\.\w+$')      # 使用正则表达式获取文件名
    module_name = module.findall(path)
    if module_name:
        json_block["module"] = module_name[0]
    json_block["type"] = "function"     # 接口类型是函数
    json_block["priority"] = 5          # 优先级 5

    files = open(path, "r", encoding="utf-8")     # 以只读方式打开lua源码文件
    file = files.read()     # 整个文件读入作为字符串

    blocks = get_info_from_mark(file, "--[[", "]]")     # 查找注释的文本块，返回文本块列表
    for block in blocks:
        block = block.strip()       # 去掉首尾的空白符
        start_str = block[0:4]      # 截取前面四个字符
        if start_str == "@api":     # 如果前面四个字符是 @api 才说明是文档注释
            # 先获取API名称
            api_name = get_info_from_mark(block, "apiName ", "@")
            if len(api_name) > 1:
                api_name = str_trim(api_name[1])
            else:
                api_name = ""
            if api_name:
                json_block["key"] = encode_str(api_name)        # 把API名称赋值给json_block字典

            # 获取demo提示信息
            demo_info = get_info_from_mark(block, "apiParamExample 示例:", "@")
            if len(demo_info) > 1:
                demo_info = str_trim(demo_info[1])
            else:
                demo_info = ""
            if demo_info:
                json_block["demo"] = encode_str(demo_info)

            # 获取tip提示信息
            tip_info = get_info_from_mark(block, "apiDescription ", "@")
            if len(tip_info) > 1:
                tip_info = str_trim(tip_info[1])
            else:
                tip_info = ""
            if tip_info:
                json_block["tip"] = encode_str(tip_info)

            # 保存字典到json文件
            global first_block
            save_dict_to_json_file(json_block, json_file, first=first_block)   # 保存字典到json文件
            if first_block:
                first_block = False

    files.close()   # 关闭当前的源码文件


def analysis_db_file(db_file, json_file):
    """
    解析DB表文件
    :param db_file: DB表文件
    :param json_file: JSON文件
    :return: None
    """
    print(db_file)
    db_data = open(db_file, "r", encoding="utf-8")  # 以只读方式打开lua源码文件
    db = db_data.read()  # 整个文件读入作为字符串
    blocks = get_info_from_mark(db, "--", ";")      # 匹配注释符和分号
    json_block["module"] = "DB"
    json_block["priority"] = 3
    json_block["type"] = "constant"
    for block in blocks:
        block = str_trim(block)
        info = block.split("DB.")       # 以DB.为标志对字符串切片
        if len(info) > 1:
            json_block["tip"] = info[0]
            json_block["key"] = str_trim(info[1].split("=")[0])
            json_block["demo"] = block
            save_dict_to_json_file(json_block, json_file)


def main(root_path, db_file, json_file):
    """
    主函数
    :param root_path: 跟目录
    :param db_file: DB表文件
    :param json_file: 输出的json文件
    :return: None
    """
    path_list = scan_files(root_path, postfix=".lua")       # 扫描目录下所有.lua文件
    json_fp = open(json_file, "w+")
    json_fp.write("[\n")    # 写入json的中括号头"["

    for i in range(len(path_list)):
        path = path_list[i]
        analysis_file(path, json_fp)     # 逐个解析文件，并把它保存到json文件

    # 解析DB表文件
    analysis_db_file(db_file, json_fp)

    # 保存lua关键字
    for keyword in lua_keyWord:
        save_dict_to_json_file(keyword, json_fp, first=first_block)

    json_fp.write("\n]")    # 写入json的中括号尾"]"
    json_fp.close()         # 关闭json文件


if __name__ == "__main__":
    # test_path = r"F:\杰理项目文件\FPGA\br28_watch\tools\br28_download_watch\UI工程\UITools\lua\std"
    obj_path = os.getcwd() + r"/std"
    tmp_file = os.getcwd() + r"/temp.json"
    db_file = os.getcwd() + r"/DB.lua"

    main(obj_path, db_file, tmp_file)

    # 将文件编码转换为utf-8
    json_file = os.getcwd() + r"/JLLua.json"  # 输出的json文件
    print(json_file)
    fp = open(json_file, "w", encoding="UTF-8")
    for row in open(tmp_file, "r"):
        fp.write(row)
    fp.close()
    os.remove(tmp_file)     # 删除过度文件

