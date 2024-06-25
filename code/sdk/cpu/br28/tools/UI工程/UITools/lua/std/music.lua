--[[
@apiDefine MusicGroup music
--]]
music = {}


-- 默认播放模式
music.play_mode = DB.MUSIC_PLAY_MODE_FCYCLE_ONE;

-- 默认播放设备
music.play_device = DB.MUSIC_PLAY_DEVICE_WATCH;

-- 默认播放状态
music.play_status = DB.MUSIC_PLAY_STATE_STOP;

-- 模拟音乐路径
music.music_path = nil;

-- 模拟文件簇号
music.music_sclust = 1;



-- 模拟音乐列表
local simulationMusicList = {
	"刘德华 - 奉陪到底",
	"王靖雯不胖 - 善变",
	"Camila Cabello - Don't Go Yet",
	"韩小欠 - 一开始说陪你到老的人现在他还在吗(DJheap九天版)",
	"来一碗老于 - 解药",
	"The Kid LAROI、Justin BieBer - Stay(Explicit)",
	"我是土豆 - 云压雨",
	"房东的猫 - New Boy",
	"IN-K、王忻辰、苏星婕 - 落日与晚风"
}


music.music_list = simulationMusicList;
music.music_number = #music.music_list;



--[[
@api {SET} music:controlMusicPlay(cmd,[sclust]) 音乐播放控制
@apiName controlMusicPlay()
@apiGroup MusicGroup
@apiVersion 1.0.0
@apiDescription 控制音乐播放、暂停、上一曲、下一曲等

@apiParam (命令参数) {number} DB.MUSIC_PLAY_START 开始播放音乐
@apiParam (命令参数) {number} DB.MUSIC_PLAY_STOP  暂停播放音乐
@apiParam (命令参数) {number} DB.MUSIC_PLAY_LAST  播放上一曲
@apiParam (命令参数) {number} DB.MUSIC_PLAY_NEXT  播放下一曲
@apiParam (命令参数) {number} DB.MUSIC_PLAY_INDEX  播放指定音乐
@apiParam (可选参数) {number} [sclust] 音乐文件簇，仅MUSIC_PLAY_INDEX命令下必须传入，其它命令无需此参数。这个参数可以通过获取音乐列表获取 

@apiParamExample 示例:
-- 开始播放音乐
music:controlMusicPlay(DB.MUSIC_PLAY_START)     

-- 暂停播放音乐
music:controlMusicPlay(DB.MUSIC_PLAY_STOP)     

music:controlMusicPlay(DB.MUSIC_PLAY_LAST)     -- 播放上一曲
music:controlMusicPlay(DB.MUSIC_PLAY_NEXT)     -- 播放下一曲

-- 播放指定音乐，sclust为指定音乐的文件簇
music:controlMusicPlay(DB.MUSIC_PLAY_INDEX, sclust)     

@apiSuccessExample {number} 成功响应: 
成功控制音乐播放

@apiErrorExample {number} 失败响应:
音乐播放控制异常
]]
function music:controlMusicPlay(cmd, index)
    if (cmd == DB.MUSIC_PLAY_START) then
        self.play_status = DB.MUSIC_PLAY_STATE_STARTING;
    elseif (cmd == DB.MUSIC_PLAY_INDEX) then
        if (index < 1) or (index > self.music_number) then
            log:e("error music index: "..tostring(index))
            return
        end
        self.music_sclust = index
    elseif (cmd == DB.MUSIC_PLAY_PAUSE) then
        self.play_status = DB.MUSIC_PLAY_STATE_STOP;
    elseif (cmd == DB.MUSIC_PLAY_STOP) then
        self.play_status = DB.MUSIC_PLAY_STATE_STOP;
    elseif (cmd == DB.MUSIC_PLAY_LAST) then
        self.play_status = DB.MUSIC_PLAY_STATE_STARTING;
        if (self.play_mode == DB.MUSIC_PLAY_MODE_FCYCLE_RANDOM) then
            self.music_sclust = math.random(1, self.music_number);
        else
            self.music_sclust = self.music_sclust - 1
            if (self.music_sclust < 1) then
                self.music_sclust = self.music_number;
            end
        end
    elseif (cmd == DB.MUSIC_PLAY_NEXT) then
        self.play_status = DB.MUSIC_PLAY_STATE_STARTING;
        if (self.play_mode == DB.MUSIC_PLAY_MODE_FCYCLE_RANDOM) then
            self.music_sclust = math.random(1, self.music_number);
        else
            self.music_sclust = self.music_sclust + 1
            if (self.music_sclust > self.music_number) then
                self.music_sclust = 1;
            end
        end
    elseif (cmd == DB.MUSIC_PLAY_DELETE) then
        log:i("Sorry, we can't delete music yet");
    end
    log:d("controlMusicPlay, cmd: "..tostring(self.play_mode)..", music index: "..tostring(self.play_status))
    sys:runHandler("music_status", "status", self.play_status);
end



--[[
@api {GET} music_list=music:loadMusicList(start,end) 加载音乐列表
@apiName loadMusicList()
@apiGroup MusicGroup
@apiVersion 1.0.0
@apiDescription 扫描SD卡中的MP3文件，并生成文件列表table

@apiParam {number} start 需要获取的音乐列表开始索引
@apiParam {number} end 需要获取的音乐列表结束索引

@apiParamExample 示例:
music_list = music:loadMusicList(1, 5)  -- 扫描第1个开始，到第5个结束的MPA文件
music_num = #music_list     -- 计算获取到的MP3文件数量
for i=1, music_num do
    print(music_list[i].name)   -- 逐个打印音乐名称
end

@apiSuccess (返回值) {table{name,len,sclust,dir_type,fn}} music_list 获取到的音乐列表
@apiSuccess (music_list) {string} name 文件名称
@apiSuccess (music_list) {number} len 文件名长度
@apiSuccess (music_list) {number} sclust 件簇号
@apiSuccess (music_list) {number{0,1}} dir_type 文件类型，文件或文件夹
@apiSuccess (music_list) {number{0,1}} fn 长文件名或短文件名

@apiSuccessExample {number} 成功响应: 
成功获取音乐列表

@apiErrorExample {number} 失败响应:
API返回nil
]]
function music:loadMusicList()
    local music_list = {}
    if (self.music_path) then
        -- 如果有指定路径，扫描路径中的文件模拟
    else
        -- 否则用模拟音乐列表作为列表返回
        local music_table = simulationMusicList;
        local music_number = #music_table;
        for i = 1, music_number do
            local music = {};
            music.name = music_table[i];
            music.len = #music_table[i];
            music.sclust = i;
            music.dir_type = DB.DIR_TYPE_FILE;
            music.fn = 0;
            table.insert(music_list, music);
        end
    end
    return music_list;
end




--[[
@api {SET} music:freeMusicList(music_list) 释放音乐列表
@apiName freeMusicList()
@apiGroup MusicGroup
@apiVersion 1.0.0
@apiDescription 释放从loadMusicList获取到的音乐列表

@apiParam {table} music_list 从loadMusicList方法获取到的音乐列表

@apiParamExample 示例:
music_list = music:loadMusicList(1, 5)  -- 扫描第1个开始，到第5个结束的MPA文件
music_num = #music_list     -- 计算获取到的MP3文件数量
for i=1, music_num do
    print(music_list[i].name)   -- 逐个打印音乐名称
end
music:freeMusicList(music_list) -- 释放music_list

@apiSuccessExample {number} 成功响应: 
music_list.name内存被成功释放

@apiErrorExample {number} 失败响应:
music_list.name内存未被成功释放，最后可能导致内存溢出
]]
function music:freeMusicList(music_table)
    local music_number = #music_table;
    if music_number > 1 then
        for i = 1, music_number do
            -- for k, v in pairs(music_table[i]) do
                log:d("free music name: " .. music_table[i].name)
            -- end
        end
    else
        for k, v in pairs(music_table[1]) do
            log:d("free music name: " .. music_table[1].name)
        end
    end
end



--[[
@api {GET} name,dev,mode,status=music:getMusicPlayInfo([only_get_name]) 获取播放器信息
@apiName getMusicPlayInfo()
@apiGroup MusicGroup
@apiVersion 1.0.0
@apiDescription 获取播放器信息，如音乐名称、播放设备、播放模式、播放状态等
@apiParam (可选参数) {number{DB.MUSIC_GET_NAME_BY_INFO}} [only_get_name] 仅获取歌曲名称，可以节省lua内部压栈操作，缩短操作时间

@apiParamExample 示例:
-- 一次获取所有播放信息
music_name, play_dev, play_mode, play_status = music:getMusicPlayInfo()

-- 仅获取歌曲名称
music_name = music:getMusicPlayInfo(DB.MUSIC_GET_NAME_BY_INFO)

@apiSuccessExample {number} 成功响应: 
成功获取所需信息

@apiErrorExample {number} 失败响应:
获取到信息为nil
]]
function music:getMusicPlayInfo(get_cur_name)
    if (get_cur_name == DB.MUSIC_GET_NAME_BY_INFO) then
        return self.music_list[self.music_sclust]
    end
    return self.music_list[self.music_sclust], self.play_device, self.play_mode, play_status
end



--[[
@api {SET} music:setMusicPlayInfo(mode,device) 设置播放信息
@apiName setMusicPlayInfo()
@apiGroup MusicGroup
@apiVersion 1.0.0
@apiDescription 设置播放器设备、模式等

@apiParam {number{nil,DB.MUSIC_PLAY_MODE_FCYCLE_ONE,DB.MUSIC_PLAY_MODE_FCYCLE_ALL,DB.MUSIC_PLAY_MODE_FCYCLE_RANDOM}} mode 播放模式
@apiParam {number{nil,DB.MUSIC_PLAY_DEVICE_PHONE,DB.MUSIC_PLAY_DEVICE_WATCH,DB.MUSIC_PLAY_DEVICE_EARPHONE}} device 播放设备

@apiParamExample 示例:
-- 同时设置播放手表音乐和循环播放模式
music:setMusicPlayInfo(DB.MUSIC_PLAY_MODE_FCYCLE_ONE, DB.MUSIC_PLAY_DEVICE_WATCH)

-- 仅设置播放手机音乐
music:setMusicPlayInfo(nil, DB.MUSIC_PLAY_DEVICE_PHONE)

-- 仅设置列表循环模式播放
music:setMusicPlayInfo(DB.MUSIC_PLAY_MODE_FCYCLE_ALL, nil)

@apiSuccessExample {number} 成功响应: 
音乐播放设置正常

@apiErrorExample {number} 失败响应:
音乐播放设置异常
]]
function music:setMusicPlayInfo(music_mode, music_device)
    self.play_mode = music_mode;
    self.play_device = music_device;
end






return music












