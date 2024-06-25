--[[
--
-- DB数据表文件
--
-- 类似与C语言的宏定义，用于定义常量数据
--
-- 在windwos系统使用时直接调用，UI资源中LUA代码编译时会自动转为对应值
--
--]]



--[[
--
-- DB表table声明
--
--]]
DB = {}


--[[
-- lua数据类型名称
--]]

-- lua 空值：nil
DB.LUA_NONE = nil;

-- lua 布尔值：true
DB.LUA_TRUE = true;

-- lua 布尔值：false
DB.LUA_FALSE = false;

-- lua 数据类型：布尔型
DB.LUA_TBOOLEAN = "boolean";

-- lua 数据类型：数字
DB.LUA_TNUMBER = "number";

-- lua 数据类型：字符串
DB.LUA_TSTRING = "string";

-- lua 数据类型：表
DB.LUA_TTABLE = "table";

-- lua 数据类型：用户类型
DB.LUA_TUSERDATA = "userdata";

-- lua 数据类型：函数
DB.LUA_TFUNCTION = "function";

-- lua 数据类型：线程
DB.LUA_TTHREAD = "thread";

-- lua 数据类型：空值 / 无效值
DB.LUA_TNIL = "nil";


--[[
-- 页面切换方向
--]]

-- gui 页面从左边切入
DB.PAGE_SWITCH_LEFT = "left";

-- gui 页面从右边切入
DB.PAGE_SWITCH_RIGHT = "right";

-- gui 页面隐藏式切换
DB.PAGE_SWITCH_HIDE = "hide";


--[[
-- 列表滑动方向标志
--]]

-- gui 列表方向：水平列表
DB.SCROLL_DIRECTION_LR = 1;

-- gui 列表方向：垂直列表
DB.SCROLL_DIRECTION_UP = 2;


--[[
-- 音乐播放设备
--]]

-- music 音乐播放设备：手表
DB.MUSIC_PLAY_DEVICE_WATCH = 1;

-- music 音乐播放设备：手机
DB.MUSIC_PLAY_DEVICE_PHONE = 2;

-- music 音乐播放设备：耳机
DB.MUSIC_PLAY_DEVICE_EARPHONE = 3;


--[[
-- 音乐播放控制参数
--]]

--开始播放音乐
DB.MUSIC_PLAY_START = 1;

-- 播放指定音乐
DB.MUSIC_PLAY_INDEX = 2;

-- 暂停播放音乐
DB.MUSIC_PLAY_PAUSE = 3;

-- 停止播放音乐
DB.MUSIC_PLAY_STOP = 4;

-- 播放上一曲
DB.MUSIC_PLAY_LAST = 5;

-- 播放下一曲
DB.MUSIC_PLAY_NEXT = 6;

-- 删除音乐
DB.MUSIC_PLAY_DELETE = 7;

-- 通过info方法获取当前音乐名称
DB.MUSIC_GET_NAME_BY_INFO = true;


--[[
-- 音乐播放状态
--]]

-- music 音乐播放状态：正在播放
DB.MUSIC_PLAY_STATE_STARTING = 1;

-- music 音乐播放状态：停止播放
DB.MUSIC_PLAY_STATE_STOP = 0;


--[[
-- 音乐播放模式
--]]

-- music 音乐播放模式：列表顺序
DB.MUSIC_PLAY_MODE_FCYCLE_LIST = 0;

-- music 音乐播放模式：列表循环
DB.MUSIC_PLAY_MODE_FCYCLE_ALL = 1;

-- music 音乐播放模式：单曲循环
DB.MUSIC_PLAY_MODE_FCYCLE_ONE = 2;

-- music 音乐播放模式：随机播放
DB.MUSIC_PLAY_MODE_FCYCLE_RANDOM = 4;

-- 音乐列表文件类型是：文件夹
DB.DIR_TYPE_FORLDER = 0;

-- 音乐列表文件类型是：文件
DB.DIR_TYPE_FILE = 1;


--[[
-- 文本控件字符显示控制
--]]

-- 文本控件字体，获取宽度显示
DB.FONT_GET_WIDTH = 0x01;

-- 文本控件字体，像素显示
DB.FONT_SHOW_PIXEL = 0x02;

-- 文本控件字体，多行显示
DB.FONT_SHOW_MULTI_LINE = 0x04;

-- 文本控件字体，滚动显示
DB.FONT_SHOW_SCROLL = 0x08;

-- 文本控件字体，高亮滚动显示
DB.FONT_HIGHLIGHT_SCROLL = 0x10;

-- 文本控件字体，默认显示方式（像素显示）
DB.FONT_DEFAULT = DB.FONT_SHOW_PIXEL;

-- 文本编码格式：内码编码
DB.FONT_ENCODE_ANSI = 0x00;

-- 文本编码格式：unicode编码
DB.FONT_ENCODE_UNICODE = 0x01;

-- 文本编码格式：utf-8编码
DB.FONT_ENCODE_UTF8 = 0x02;

-- 文本大小端存储：大端
DB.FONT_ENDIAN_BIG = 0x00;

-- 文本大小端存储：小端
DB.FONT_ENDIAN_SMALL = 0x01;


--[[
-- 蓝牙 bt 库常量
--]]

-- gui 蓝牙：scan end
DB.HCI_EVENT_INQUIRY_COMPLETE = 0x01;

-- gui 蓝牙：connection
DB.HCI_EVENT_CONNECTION_COMPLETE = 0x03;

-- gui 蓝牙：disconnection
DB.HCI_EVENT_DISCONNECTION_COMPLETE = 0x05;

-- gui 蓝牙：pin
DB.HCI_EVENT_PIN_CODE_REQUEST = 0x16;

-- gui 蓝牙：
DB.HCI_EVENT_IO_CAPABILITY_REQUEST = 0x31;

-- gui 蓝牙：
DB.HCI_EVENT_USER_CONFIRMATION_REQUEST = 0x33;

-- gui 蓝牙：
DB.HCI_EVENT_USER_PASSKEY_REQUEST = 0x34;

-- gui 蓝牙：
DB.HCI_EVENT_USER_PRESSKEY_NOTIFICATION = 0x3B;

-- gui 蓝牙：
DB.HCI_EVENT_VENDOR_NO_RECONN_ADDR = 0xF8;

-- gui 蓝牙：
DB.HCI_EVENT_VENDOR_REMOTE_TEST = 0xFE;

-- gui 蓝牙：
DB.BTSTACK_EVENT_HCI_CONNECTIONS_DELETE = 0x6D;

-- gui 蓝牙：
DB.ERROR_CODE_SUCCESS = 0x00;

-- gui 蓝牙：
DB.ERROR_CODE_PAGE_TIMEOUT = 0x04;

-- gui 蓝牙：
DB.ERROR_CODE_AUTHENTICATION_FAILURE = 0x05;

-- gui 蓝牙：
DB.ERROR_CODE_PIN_OR_KEY_MISSING = 0x06;

-- gui 蓝牙：
DB.ERROR_CODE_CONNECTION_TIMEOUT = 0x08;

-- gui 蓝牙：
DB.ERROR_CODE_SYNCHRONOUS_CONNECTION_LIMIT_TO_A_DEVICE_EXCEEDED = 0x0A;

-- gui 蓝牙：
DB.ERROR_CODE_ACL_CONNECTION_ALREADY_EXISTS = 0x0B;

-- gui 蓝牙：
DB.ERROR_CODE_CONNECTION_REJECTED_DUE_TO_LIMITED_RESOURCES = 0x0D;

-- gui 蓝牙：
DB.ERROR_CODE_CONNECTION_REJECTED_DUE_TO_UNACCEPTABLE_BD_ADDR = 0x0F;

-- gui 蓝牙：
DB.ERROR_CODE_CONNECTION_ACCEPT_TIMEOUT_EXCEEDED = 0x10;

-- gui 蓝牙：
DB.ERROR_CODE_REMOTE_USER_TERMINATED_CONNECTION = 0x13;

-- gui 蓝牙：
DB.ERROR_CODE_CONNECTION_TERMINATED_BY_LOCAL_HOST = 0x16;

-- gui 蓝牙：
DB.CUSTOM_BB_AUTO_CANCEL_PAGE = 0xFD;

-- gui 蓝牙：
DB.BB_CANCEL_PAGE = 0xFE;

-- 获取蓝牙耳机连接状态
DB.BT_EMITTER_CONNECT_STATUS = 0x01;

-- 获取手机蓝牙连接状态
DB.BT_PHONE_CONNECT_STATUS = 0x02;



--[[
-- 设备 device 库常量
--]]

-- 获取下一个数据，从指定索引开始获取，直到最后一个数据
DB.DEVICE_GET_NEXT_DATA = 0x01;

-- 获取上一个数据，从指定索引开始获取，直到第一个数据
DB.DEVICE_GET_PREV_DATA = 0x02;

-- 获取所有数据，直接获取索要获取数据的全部内容，注意内存消耗！
DB.DEVICE_GET_ALL_DATA = 0x04;

-- device 获取数据：\n1. 获取指定索引的数据\n2. 为获取上/下一个数据设定索引位置。
DB.DEVICE_GET_IND_DATA = 0x08;

-- device 获取数据：获取存储的数据中的最大值
DB.DEVICE_GET_MAX_DATA = 0x10;

-- device 获取数据：获取存储的数据中的最小值
DB.DEVICE_GET_MIN_DATA = 0x20;

-- device 获取数据：获取存储的数据个数
DB.DEVICE_GET_DATA_NUM = 0x40;


--[[
-- 系统 sys 库常量
--]]

-- 创建定时器，选择创建 timer 实例
DB.SYS_TIMER_HANDLER = 0x01;

-- 创建定时器，选择创建 timeout 实例
DB.SYS_TIMEOUT_HANDLER = 0x02;

-- 创建联系人操作句柄
DB.SYS_OPERATE_CONTACTS = 0;

-- 创建通话记录操作句柄
DB.SYS_OPERATE_CALLLOG  = 1;

-- 获取联系人/通话记录列表中元素数量
DB.SYS_VM_READ_LIST_COUNT = 0x01;

-- 获取指定所以的联系人/通话记录
DB.SYS_VM_READ_LIST_INDEX = 0x02;

-- 通过号码获取名称
DB.SYS_VM_READ_NAME_BY_NUMBER = 0x03;

-- 保存联系人/通话记录到列表中
DB.SYS_VM_SAVE_LIST_BOOK = 0x01;

-- 已选中的卡片数量
DB.CardSetNum = 0x00;

-- 屏幕亮度等级
DB.LightLevel = 0x01;

-- 屏幕熄屏时间
DB.DarkTime = 0x02;

-- 快捷键选项
DB.ShortcutKey = 0x03;

-- 上一次系统音量
DB.LastSysVol = 0x04;

-- 屏幕常亮使能
DB.LightAlwayEn = 0x05;

-- 屏幕常亮时间
DB.LightTime = 0x06;

-- 系统静音标志
DB.SysVoiceMute = 0x07;

-- 全天勿扰使能
DB.AllDayUndisturbEn = 0x08;

-- 定时勿扰使能
DB.TimeUndisturbEn = 0x09;

-- 定时勿扰起始时间（时）
DB.UndisturbStimeH = 0x0A;

-- 定时勿扰起始时间（分）
DB.UndisturbStimeM = 0x0B;

-- 定时勿扰结束时间（时）
DB.UndisturbEtimeH = 0x0C;

-- 定时勿扰结束时间（分）
DB.UndisturbEtimeM = 0x0D;

-- 训练自动识别使能
DB.TrainAutoEn = 0x0E;

-- 是否连接新手机
DB.ConnNewPhone = 0x0F;

-- 语言选择
DB.Language = 0x10;

-- 菜单风格
DB.MenuStyle = 0x11;



--[[
-- 闹钟相关
--]]

-- 周一打开
DB.SYS_ALARM_MONDAY_OPEN = 0x02;
-- 周一关闭
DB.SYS_ALARM_MONDAY_CLOSE = 0xfd;

-- 周二打开
DB.SYS_ALARM_TUESDAY_OPEN = 0x04;
-- 周二关闭
DB.SYS_ALARM_TUESDAY_CLOSE = 0xfb;

-- 周三打开
DB.SYS_ALARM_WEDNESDAY_OPEN = 0x08;
-- 周三关闭
DB.SYS_ALARM_WEDNESDAY_CLOSE = 0xf7;

-- 周四打开
DB.SYS_ALARM_THURSDAY_OPEN = 0x10;
-- 周四关闭
DB.SYS_ALARM_THURSDAY_CLOSE = 0xef;

-- 周五打开
DB.SYS_ALARM_FRIDAY_OPEN = 0x20;
-- 周五关闭
DB.SYS_ALARM_FRIDAY_CLOSE = 0xdf;

-- 周六打开
DB.SYS_ALARM_SATURDAY_OPEN = 0x40;
-- 周六关闭
DB.SYS_ALARM_SATURDAY_CLOSE = 0xbf;

-- 周日打开
DB.SYS_ALARM_SUNDAY_OPEN = 0x80;
-- 周日关闭
DB.SYS_ALARM_SUNDAY_CLOSE = 0x7f;

-- 最大闹钟数量
DB.SYS_ALARM_MAX_NUMBER = 5;

-- 设置闹钟开关
DB.SYS_ALARM_SET_ONOFF = 0x01;

-- 设置闹钟时间
DB.SYS_ALARM_SET_TIME = 0x02;

-- 设置闹钟模式
DB.SYS_ALARM_SET_MODE = 0x04;

-- 设置闹钟贪睡
DB.SYS_ALARM_SET_SNOOZE = 0x08;

-- 设置闹钟赞数关闭
DB.SYS_ALARM_SET_CLOSE = 0x10;





return DB

