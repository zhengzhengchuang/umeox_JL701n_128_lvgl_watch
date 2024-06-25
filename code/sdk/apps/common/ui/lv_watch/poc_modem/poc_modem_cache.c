#include "syscfg_id.h"
#include "app_config.h"
#include "poc_modem_cache.h"

ui_info_cache_t *p_ui_info_cache = NULL;
static ui_info_cache_t ui_info_cache = {0};

void ui_info_cache_init(ui_act_id_t act_id)
{
    p_ui_info_cache = &ui_info_cache;
    
    if(!p_ui_info_cache) ASSERT(0);

    memset(p_ui_info_cache, 0, \
        sizeof(ui_info_cache_t));

    p_ui_info_cache->cur_act_id = act_id;

    return;
}



