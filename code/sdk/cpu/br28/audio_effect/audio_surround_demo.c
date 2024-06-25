#include "audio_surround_demo.h"
#include "app_config.h"


#if AUDIO_SURROUND_CONFIG
SurroundEffect_TOOL_SET sur_parm[mode_add] = {0};
#endif
/*----------------------------------------------------------------------------*/
/**@brief    环绕音效模块打开例子
   @param
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
surround_hdl *surround_open_demo(u32 sur_name, u8 ch_type)
{
    surround_hdl *surround = NULL;
#if AUDIO_SURROUND_CONFIG
    surround_open_parm parm = {0};
    u8 nch = EFFECT_CH_L;
    if (ch_type == AUDIO_CH_L) {
        nch = EFFECT_CH_L;
    } else if (ch_type == AUDIO_CH_R) {
        nch = EFFECT_CH_R;
    } else if (ch_type == AUDIO_CH_LR) {
        nch = 2;
    } else {
        log_e("surround ch_type err 0x%x, nch 0x%x\n", ch_type, nch);
        return NULL;
    }
    u8 tar = 0;
    if (sur_name == AEID_MUSIC_SURROUND) {
        tar = 0;
    } else if (sur_name == AEID_AUX_SURROUND) {
#if defined(LINEIN_MODE_SOLE_EQ_EN) && LINEIN_MODE_SOLE_EQ_EN
        tar = 1;
#endif
    }

    //printf("nch 0x%x, ch_type 0x%x\n", nch,ch_type);
    parm.channel = nch;
    parm.sur_name = sur_name;
    parm.surround_effect_type = EFFECT_3D_PANORAMA;//打开时默认使用3d全景音,使用者，根据需求修改
    surround = audio_surround_open(&parm);

    audio_surround_effect_update_demo(parm.sur_name, &sur_parm[tar].parm, sur_parm[tar].is_bypass);
    clock_add(DEC_3D_CLK);

    //void test_sur(void *p);
    //sys_timer_add(NULL,test_sur, 5000);
#endif
    return surround;
}

void test_sur(void *p)
{
#if AUDIO_SURROUND_CONFIG
    static u8 tttf = 0;
    audio_surround_effect_switch_demo(AEID_MUSIC_SURROUND, tttf);
    tttf++;
    if (tttf >= KARAOKE_SPK_MAX) {
        tttf = 0;
    }
#endif
}
/*----------------------------------------------------------------------------*/
/**@brief    环绕音效关闭例子
   @param
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
void surround_close_demo(surround_hdl *surround)
{
#if AUDIO_SURROUND_CONFIG
    if (surround) {
        audio_surround_close(surround);
        surround = NULL;
    }
    clock_remove(DEC_3D_CLK);
#endif
}

void audio_surround_effect_update_demo(u32 sur_name, surround_update_parm *parm, u8 bypass)
{
#if AUDIO_SURROUND_CONFIG
    audio_surround_parm_update(sur_name, EFFECT_3D_PANORAMA, parm);
    audio_surround_bypass(sur_name, bypass);
#endif
}

/*----------------------------------------------------------------------------*/
/**@brief    环绕音效切换测试例子
   @param   surround:句柄
   @param   effect_type:
   						EFFECT_3D_PANORAMA,             //3d全景
    					EFFECT_3D_ROTATES,                  //3d环绕
    					EFFECT_FLOATING_VOICE,              //流动人声
    					EFFECT_GLORY_OF_KINGS,              //王者荣耀
    					EFFECT_FOUR_SENSION_BATTLEFIELD,    //四季战场
    					EFFECT_OFF,//原声
   @return
   @note
*/
/*----------------------------------------------------------------------------*/
void audio_surround_effect_switch(u32 sur_name, u32 effect_type)
{
#if AUDIO_SURROUND_CONFIG
    surround_update_parm parm = {0};
    parm.rotatestep = 2;//旋转速度
    parm.damping = 120;//高频衰减速度
    parm.feedback = 110;//整体衰减速度
    parm.roomsize = 128;//空间大小
    if (EFFECT_OFF == effect_type) {
        //中途关测试
        /* u8 en = 0; */
        /* audio_surround_parm_update(surround, effect_type, (surround_update_parm *)en); */

        //此处关闭音效处理使用surround_type = 0的方式
        u8 en = 1;
        audio_surround_parm_update(sur_name, effect_type, (surround_update_parm *)en);
        parm.surround_type = 0;
        audio_surround_parm_update(sur_name, EFFECT_3D_PANORAMA, &parm);
    } else {
        //音效切换测试
        u8 en = 1;
        audio_surround_parm_update(sur_name, EFFECT_OFF, (surround_update_parm *)en);
        if (effect_type == EFFECT_3D_PANORAMA) {
            parm.surround_type = EFFECT_3D_TYPE1;
            audio_surround_parm_update(sur_name, EFFECT_3D_PANORAMA, &parm);
        } else {
            audio_surround_parm_update(sur_name, effect_type, NULL);
        }
    }

#endif
}

/*----------------------------------------------------------------------------*/
/**@brief    环绕音效、虚拟低音切换例子
   @param    eff_mode:
			KARAOKE_SPK_OST,//原声
			KARAOKE_SPK_DBB,//重低音
			KARAOKE_SPK_SURROUND,//全景环绕
			KARAOKE_SPK_3D,//3d环绕
			KARAOKE_SPK_FLOW_VOICE,//流动人声
			KARAOKE_SPK_KING,//王者荣耀
			KARAOKE_SPK_WAR,//四季战场
			KARAOKE_SPK_MAX,

   @return
   @note
*/
/*----------------------------------------------------------------------------*/
void audio_surround_effect_switch_demo(u32 sur_name, u8 eff_mode)
{
    log_i("SPK eff***************:%d ", eff_mode);
#if AUDIO_SURROUND_CONFIG
#if AUDIO_VBASS_CONFIG
    u32 vbass_name = AEID_MUSIC_VBASS;
    if (eff_mode != KARAOKE_SPK_DBB) {
        audio_vbass_bypass(vbass_name, RUN_BYPASS);
    }
#endif

    switch (eff_mode) {
    case KARAOKE_SPK_OST://原声
        audio_surround_effect_switch(sur_name, EFFECT_OFF);
        log_i("spk_OST\n");
        break;
    case KARAOKE_SPK_DBB://重低音
        audio_surround_effect_switch(sur_name, EFFECT_OFF);//这里要用回原声处理的地方不一样
#if AUDIO_VBASS_CONFIG
        audio_vbass_bypass(vbass_name, RUN_NORMAL);
#endif
        log_i("spk_DDB\n");
        break;
    case KARAOKE_SPK_SURROUND://全景环绕
        audio_surround_effect_switch(sur_name, EFFECT_3D_PANORAMA);
        log_i("spk_SURRROUND\n");
        break;
    case KARAOKE_SPK_3D://3d旋转
        audio_surround_effect_switch(sur_name, EFFECT_3D_ROTATES);
        log_i("spk_3D\n");
        break;
    case KARAOKE_SPK_FLOW_VOICE://流动人声
        audio_surround_effect_switch(sur_name, EFFECT_FLOATING_VOICE);
        log_i("spk_FLOW\n");
        break;
    case KARAOKE_SPK_KING://王者荣耀
        audio_surround_effect_switch(sur_name, EFFECT_GLORY_OF_KINGS);
        log_i("spk_KING\n");
        break;
    case KARAOKE_SPK_WAR://四季战场
        audio_surround_effect_switch(sur_name, EFFECT_FOUR_SENSION_BATTLEFIELD);
        log_i("spk_WAR\n");
        break;
    default:
        log_i("spk_ERROR\n");
        break;
    }
#endif
}
