#include "audio_ch_swap_demo.h"
#include "app_config.h"
#include "audio_eff_default_parm.h"

#if defined(CH_SWAP_OLD) && CH_SWAP_OLD
ChannelSwap_TOOL_SET music_ch_swap[mode_add];
struct audio_ch_swap *audio_ch_swap_open_demo(u32 swap_name, u8 channel)
{
    if (channel != 2) {
        return NULL;
    }
    struct aud_ch_swap parm = {0};

    u8 bypass = 0;
    u8 tar = 0;
    if (swap_name == AEID_MUSIC_CH_SWAP) {
        tar = 0;
    } else if (swap_name == AEID_AUX_CH_SWAP) {
        tar = 1;
    }
    bypass = music_ch_swap[tar].is_bypass;

    parm.channel = channel;
    parm.indata_inc = 2;//(channel == 1) ? 1 : 2;
    parm.bit_wide = 0; //16bit_wide
    parm.swap_name = swap_name;
    struct audio_ch_swap *hdl = audio_ch_swap_process_open(&parm);
    audio_ch_swap_bypass(parm.swap_name, bypass);
    return hdl;
}


void audio_ch_swap_close_demo(struct audio_ch_swap *hdl)
{
    if (!hdl) {
        return;
    }
    audio_ch_swap_close(hdl);
    hdl = NULL;
}

void audio_ch_swap_update_parm(u16 swap_name, void *parm, int bypass)
{
    audio_ch_swap_bypass(swap_name, bypass);
}
#endif


