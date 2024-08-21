#include "../lv_watch.h"
#include "../../../../watch/include/ui/lcd_spi/lcd_drive.h"

static u8 upgrade_state = upgrade_none;
u8 GetOtaUpgradeState(void)
{
    return upgrade_state;
}

void SetOtaUpgradeState(u8 state)
{
    upgrade_state = state;
    return;
}

void OtaUpgradeHandle(void)
{
    DisableGmModule();
    DisablePpgModule(); 
    UserStopwatchReset();
    UserCountdownReset();
    ManualEndSleepHandle();

    if(lcd_sleep_status() == 0)
    {
        int bl_val = TCFG_BACKLIGHT_MIN_VAL;
        struct lcd_interface *lcd = lcd_get_hdl();
        if(lcd->backlight_ctrl) lcd->backlight_ctrl((u8)bl_val);
    }
    
    return;
}

void OtaUpgradeFailHandle(void)
{
    AppCtrlLcdEnterSleep(1);
    cpu_reset();

    return;
}