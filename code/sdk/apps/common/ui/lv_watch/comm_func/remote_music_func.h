#ifndef __REMOTE_MUSIC_FUNC_H__
#define __REMOTE_MUSIC_FUNC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

#define Rmusic_Min_Vol (0)
#define Rmusic_Max_Vol (16)

#define Rmusic_Title_Len (60)

enum
{
    Rmusic_puase = 0x00,
    Rmusic_play,
};
typedef u8 Rmusic_state_t;

enum
{
    Cmd_Get_Rmusic_Para = 0x00,   //同步参数
    Cmd_Set_Rmusic_State,         //播放/暂停
    Cmd_Set_Rmusic_Prev,          //上一曲
    Cmd_Set_Rmusic_Next,          //下一曲
    Cmd_Set_Rmusic_Volume,        //音量
};
typedef u8 Rmusic_cmd_t;

typedef struct
{
    u16 vm_mask;

    uint8_t volume;
    Rmusic_state_t state;
    char title[Rmusic_Title_Len + 1];
}RmusicInfoPara_t;
extern RmusicInfoPara_t Rmusic_Info;

//bool GetRmusicUpdateState(void);

void RmusicInfoParaRead(void);
void DevReqOpRmusicHandle(Rmusic_cmd_t OpCmd, u8 *para);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
