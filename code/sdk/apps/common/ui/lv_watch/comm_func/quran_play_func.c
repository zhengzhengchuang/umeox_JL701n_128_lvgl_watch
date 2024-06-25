#include "../lv_watch.h"
#include "../../../music/music_player.h"
#include "../../../dev_manager/dev_manager.h"
#include "../../../file_operate/file_bs_deal.h"
#include "../../../../watch/include/app_task.h"
#include "../../../../watch/include/key_event_deal.h"
#include "../../../../watch/include/task_manager/music/music.h"

#define VM_MASK (0x66b2)

QpUserPara_t QpUser;
static FS_DIR_INFO QpDirInfo;

static QpFsPara_t *Qp;

#define __this (Qp)
#define __this_size (sizeof(QpFsPara_t))

#define __dir_size (sizeof(FS_DIR_INFO))

#define __this_path "storage/sd0/C/"

static const QpUserPara_t InitQp = {
    .SelIdx = 0,
    .FavFlag = {false},
};

int QpGetTotalNum(void)
{
    if(!__this) return 0;

    return __this->total_num;
}

FS_DIR_INFO *QpGetFsDirInfo(void)
{
    return (&QpDirInfo);
}

int QpIsRunning(void)
{
    if(app_get_curr_task() == APP_MUSIC_TASK) 
    {
        int status = music_player_get_play_status();
        if(status == FILE_DEC_STATUS_PLAY)
            return true;
    }else
    {
        if(bt_media_is_running())
            return true;
    }

    return false;
}

void QpUpdateSelIdx(void)
{
    s8 SelIdx = QpSelIdxInvalid;

    u16 CurIdx = music_player_get_file_cur();
    if(CurIdx >= 1 && CurIdx <= QpMaxNum)
        SelIdx = CurIdx - 1;
    else if(CurIdx == QpMaxNum + 1)
        SelIdx = Qp_Mor_R;
    else if(CurIdx == QpMaxNum + 2)
        SelIdx = Qp_Eve_R;
    else
        return;

    QpUser.SelIdx = SelIdx;
    QpUserInfoParaUpdate();

    return;
}

int QpGetDecCurTime(void)
{
    return music_player_get_dec_cur_time();
}

int QpGetDecTotalTime(void)
{
    return music_player_get_dec_total_time();
}

u8 QpGetCycleMode(void)
{
    return music_player_get_repeat_mode();
}

void QpSetCycleMode(u8 mode)
{
    music_player_set_repeat_mode(mode);

    return;  
}

void QpPPHandle(void)
{
    if(app_get_curr_task() != APP_MUSIC_TASK) 
    {
        music_set_start_auto_play(1);
        app_task_switch_to(APP_MUSIC_TASK);
    }else 
    {
        app_task_put_key_msg(KEY_MUSIC_PP, 0);
    }

    return;
}

void QpPrevHandle(void)
{
    app_task_put_key_msg(KEY_MUSIC_PREV, 0);

    return;
}

void QpNextHandle(void)
{
    app_task_put_key_msg(KEY_MUSIC_NEXT, 0);

    return;
}

void QpAudioBySclust(FS_DIR_INFO *info)
{
    if(!info) return;

    if(app_get_curr_task() != APP_MUSIC_TASK) 
    {
        music_task_set_parm(MUSIC_TASK_START_BY_SCLUST, info->sclust);
        music_set_start_auto_play(1);
        app_task_switch_to(APP_MUSIC_TASK);
    }else 
    {
        app_task_put_key_msg(KEY_MUSIC_PLAYE_BY_DEV_SCLUST, info->sclust);
    }

    return;
}

void QpOpenFile(void)
{
    QpCloseFile();

    __this = zalloc(__this_size); 
    if(!__this)
        return;

    __this->dir_buf = zalloc(__dir_size);
    if(!__this->dir_buf)
        return;

    fset_ext_type(__this_path, "MP3");

    fopen_dir_info(__this_path, &__this->file, NULL);
    if(!__this->file)
        return;

    __this->total_num = fenter_dir_info(__this->file, __this->dir_buf);

    printf("total_num = %d\n", __this->total_num);

    return;
}

void QpCloseFile(void)
{
    if(!__this) return;
  
    if(__this->file){
        fclose(__this->file);
        __this->file = NULL;
    }

    if(__this->dir_buf){
        free(__this->dir_buf);
        __this->dir_buf = NULL;
    }

    free(__this);
    __this = NULL;

    return;
}

void QpFileRead(u8 idx)
{
    QpOpenFile();

    if(!__this) return;

    if(!__this->file)
        return;

    if(!__this->dir_buf)
        return;

    if(!__this->total_num)
        return;

    if(idx > __this->total_num)
        return;

    fget_dir_info(__this->file, idx, 1, __this->dir_buf);

    memcpy(&QpDirInfo, __this->dir_buf, sizeof(FS_DIR_INFO));

    QpCloseFile();

    return;
}

void QpUserInfoParaRead(void)
{
    int vm_op_len = \
        sizeof(QpUserPara_t);

    int ret = syscfg_read(CFG_QPUSER_PARA_INFO, \
        &QpUser, vm_op_len);
    if(ret != vm_op_len || QpUser.vm_mask != VM_MASK)
        QpUserInfoParaReset();

    return;
}

void QpUserInfoParaWrite(void)
{
    int vm_op_len = \
        sizeof(QpUserPara_t);
    
    for(u8 i = 0; i < 3; i++)
    {
        int ret = syscfg_write(CFG_QPUSER_PARA_INFO, \
            &QpUser, vm_op_len);
        if(ret == vm_op_len)
            break;
    }

    return;
}

void QpUserInfoParaReset(void)
{
    int vm_op_len = \
        sizeof(QpUserPara_t);

    memcpy(&QpUser, &InitQp, vm_op_len);

    QpUser.vm_mask = VM_MASK;

    QpUserInfoParaUpdate();

    return;
}

void QpUserInfoParaUpdate(void)
{
    QpUserInfoParaWrite();

    return;
}