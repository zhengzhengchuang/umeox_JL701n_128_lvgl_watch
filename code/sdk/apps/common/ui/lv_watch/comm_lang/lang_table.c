#include "lang_table.h"

const char lang_str_table[Lang_Num]\
    [Lang_Str_Num][Lang_Str_Len] = 
{
#if Conf_Lang_Ar
    //阿拉伯语
    {
        "الهاتف",
        "إشعارات الرسائل",
        "الكعبة نحو",
        "مشغل القران الكريم",
        "أوقات الصلاة",
        "الأذكار",
        "التذكير بالتسبيح",
        "أسماء الله الحسنى",
        "التقويم الهجري",
        "الرياضة",
        "النوم",
        "الطقس",
        "معدل ضربات القلب",
        "أكسجين الدم",
        "المنبه",
        "المزيد",
        "الإعدادات",
        "الخطي",
        "عرض القائمة",
        "الصوت",
        "العرض",
        "اللغة",
        "وحدة القياس",
        "اختصار",
        "حول",
        "تشغيل الصوت",
        "قائمة",
        "شبكة 1",
        "شبكة 2",
        "نوم الشاشة",
        "المسافة",
        "درجة حرارة",
        "كيلومتر",
        "ميل",
        "℃",
        "℉",
        "Quran Watch",
        "اسم البلوتوث",
        "عنوان ماك للبلوتوث",
        "الإصدار",
        "كود QR",
        "إعادة ضبط المصنع",
        "ستؤدي إعادة ضبط المصنع إلى مسح بيانات الساعة.",
        "اليوم",
        "غدًا",
        "مشمس",
        "سنو",
        "مطر مع احتمال عاصفة رعدية شديدة",
        "رياح",
        "ضباب",
        "عاصفة رملية",
        "غائم",
        "مطر + ثلوج",
        "مطر + ثلوج",
        "متلبد بالغيوم متلبد بالغيوم",
        "أمطار غزيرة",
        "مطر + ثلوج",
        "لا توجد بيانات",
        "كل يوم",
        "الأحد",
        "الإثنين",
        "الثلاثاء", 
        "الأربعاء",
        "الخميس",
        "الجمعة",
        "السبت",
        "سجل المكالمات",
        "شخص الاتصال",
        "الهاتف غير متصل ، يرجى المحاولة مرة أخرى بعد الاتصال",
        "بدون تسجيل",
        "نهاية المكالمة",
        "يرجى توصيل هاتفك وفتح المشغل الصوتي.",
        "غير متصل بالهاتف",
        "البحث عن الهاتف بالقرب ...",
        "وضع عدم الإزعاج قيد التشغيل",
        "وضع عدم الإزعاج مغلق",
        "لا أذكر",
        "لا رسالة",
        "اتبع تعليمات المعايرة المغناطيسية .",
        "اتصال بلوتوث إلى الهاتف في الوقت الحقيقي الموقف .",
        "الفجر",
        "الشروق",
        "الظهر",
        "العصر",
        "الغروب",
        "المغرب",
        "العشاء",
        "الوقت المتبقي",
        "الوقت المنقضي",
        "تذكير مسبقًا",
        "دقيقة .",
        "تذكير فتح",
        "صوت على",
        "الصباح التذكاري",
        "ليلة الذكرى",
        "أذكار بعد الصلاة",
        "الحج والعمرة",
        "الصيام",
        "القيام من النوم",
        "الثوب الجديد",
        "الوضوء",
        "المنزل",
        "المسجد",
        "الأذان",
        "الطعام",
        "السفر",
        "أذكار أخرى",
        "الصلاة",
        "ضبط التذكير بالتسبيح.",
        "تشغيل تذكير التسبيح",
        "إيقاف التذكير بالتسبيح",
        "نطاق الوقت",
        "الفترة",
        "يوم",
        "اذكر الله",
        "ساعة ",
        "وتجاوزت الفترة الزمنية الإطار الزمني",
        "وقت البدء",
        "نهاية الوقت",
        "يناير",
        "فبراير",
        "مارس",
        "أبريل",
        "مايو",
        "يونيو",
        "يوليو",
        "أغسطس",
        "سبتمبر",
        "أكتوبر",
        "نوفمبر",
        "ديسمبر",
        "محرم",
        "صفر",
        "ربيع الأول",
        "ربيع الثاني",
        "جماد الأول",
        "جماد الثاني",
        "رجب",
        "شعبان",
        "رمضان",
        "شوال",
        "ذو القعدة",
        "ذو الحجة",
        "تعديل التقويم الهجري",
        "رأس السنة الهجرية",
        "عاشوراء",
        "المولد النبوي",
        "ليلة الإسراء والمعراج",
        "ليلة البراءة",
        "رمضان",
        "ليلة القدر",
        "عيد الفطر",
        "يوم التروية",
        "يوم عرفة",
        "عيد الأضحى",
        "أيام التشريق",
        "تذكير المهرجان",
        "مستيقظا",
        "حركة سريعة للعين",
        "النوم الخفيف",
        "نومي عميق",
        "يرجى التحقق من ارتداء الساعة بشكل صحيح.",
        "صحة المرأة",
        "المؤقت",
        "ساعة الإيقاف",
        "مستوى الصوت",
        "حجم صوت الإعلام",
        "ابحث عن ساعتك",
        "انهض وتحرك",
        "قم بتنزيل تطبيق iQibla (Smart Qibla) واستخدمه للمسح لربط ساعة Quran Watch.",
        "حاليا أنت مستخدم جديد تم مسح بيانات الجهاز.",
        "قائمة المفضلة",
        "أذكار الصباح",
        "أذكار المساء",
    },
#endif
#if Conf_Lang_En
    //英文
    {
    },
#endif

#if Conf_Lang_Fr
    //法语
    {
    }, 
#endif

#if Conf_Lang_Ur
    //乌尔都语
    {
    },
#endif

#if Conf_Lang_Tu
    //土耳其语
    {
    },
#endif

#if Conf_Lang_Ch
    //简体中文
    {
        "电话",
        "消息通知",
        "克尔白朝向",
        "古兰经播放器",
        "礼拜时间",
        "祈祷词",
        "赞念提醒",
        "阿拉的 99 个\n尊名",
        "回历",
        "运动",
        "睡眠",
        "天气",
        "心率",
        "血氧",
        "闹钟",
        "更多",
        "设置",
        "计步",
        "菜单视图",
        "声音",
        "展示",
        "语言",
        "计量单位",
        "快捷方式",
        "关于",
        "声音 - 开启",
        "列表",
        "网格 1",
        "网格 2",
        "屏幕休眠",
        "距离",
        "温度",
        "公里",
        "英里",
        "℃",
        "℉",
        "Quran Watch",
        "蓝牙名称",
        "蓝牙 MAC",
        "版本",
        "二维码",
        "恢复出厂设置",
        "恢复出厂设置将清除手表数据。",
        "今天",
        "明天",
        "晴",
        "雪",
        "雷阵雨",
        "风",
        "雾霾",
        "沙尘暴",
        "多云",
        "雨夹雪",
        "阵雨",
        "阴天",
        "大雨",
        "小雨",
        "无数据",
        "每天",
        "周日",
        "周一",
        "周二",
        "周三",
        "周四",
        "周五",
        "周六",
        "通话记录",
        "联系人",
        "电话未连接，请在连接电话后重试。",
        "无记录",
        "通话结束",
        "请连接手机，并打开播放器。",
        "未连接到手机",
        "正在寻找附近\n的手机...",
        "勿扰模式 - 开启",
        "勿扰模式 - 关闭",
        "无提醒",
        "无消息",
        "按照磁性校准说明\n进行操作。", 
        "蓝牙连接到手机，实现\n实时定位。",
        "晨礼",
        "日出",
        "晌礼",
        "晡礼",
        "日落",
        "昏礼",
        "霄礼",
        "剩余时间",
        "经过时间",
        "提醒提前时间",
        "分钟",
        "提醒开启",
        "语音开启",
        "晨间纪念",
        "晚间纪念",
        "祈祷后赞念",
        "朝觐与乌姆拉",
        "斋月",
        "起床",
        "服装",
        "沐浴",
        "家",
        "清真寺",
        "祈祷的呼唤",
        "食物",
        "旅行",
        "其他祷告",
        "礼拜",
        "为赞念设置提醒。",
        "提醒 - 开启",
        "提醒 - 关闭",
        "时间范围",
        "间隔时间",
        "天",
        "赞念时间",
        "小时",
        "间隔时间超过了时间范围",
        "开始时间",
        "结束时间",
        "一月",
        "二月",
        "三月",
        "四月",
        "五月",
        "六月",
        "七月",
        "八月",
        "九月",
        "十月",
        "十一月",
        "十二月",
        "穆哈兰姆月",
        "色法尔月",
        "赖比尔·敖外鲁月",
        "赖比尔·阿色尼月",
        "主马达·敖外鲁月",
        "主马达·阿色尼月",
        "赖哲卜月",
        "舍尔邦月",
        "赖买丹月",
        "闪瓦鲁月",
        "都尔喀尔德月",
        "都尔黑哲月",
        "回历修改",
        "伊斯兰新年",
        "阿舒拉节",
        "圣纪节",
        "登霄夜",
        "拜拉特夜",
        "斋月",
        "盖德尔夜",
        "开斋节",
        "塔维耶日",
        "阿拉法日",
        "古尔邦节",
        "塔什里格日",
        "节日提醒",
        "清醒",
        "快速眼动",
        "浅睡",
        "深睡",
        "测量异常，请检查佩戴情况。",
        "女性健康",
        "计时器",
        "秒表",
        "通话音量",
        "媒体音量",
        "寻找到您的手表",
        "起身走动",
        "下载iQibla(Smart Qibla) APP，用APP扫描绑定Quran Watch。",
        "当前您为新用户，设备数据已清除。",
        "收藏列表",
        "早上祈祷词",
        "晚上祈祷词",
    },
#endif

#if Conf_Lang_In
    //印尼语
    {
    },
#endif

#if Conf_Lang_Pe
    //波斯语
    {
    },
#endif

#if Conf_Lang_Ge
    //德语
    {
    }, 
#endif

#if Conf_Lang_Th
    //泰语
    {
    },
#endif

#if Conf_Lang_Ru
    //俄语
    {
    },
#endif

#if Conf_Lang_Hi
    //印地语
    {
    },
#endif

#if Conf_Lang_Ma
    //马来西亚语
    {
    },
#endif

#if Conf_Lang_Uz
    //乌兹别克斯坦语
    {
    },
#endif
};

