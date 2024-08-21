#include "../lv_watch.h"

/* 电池容量 单位:mah*/
#define Bat_Cap 390.0f

/* 电池内阻 单位：欧姆 */
#define Bat_R 0.35f

/* 最大充电电流 单位：毫安 */
#define Bat_Max_I 200.0f

/* 最小放电电流 单位：毫安 */
#define Bat_Min_I -200.0f

/* 充电系数 */
#define Charge_Factor 0.6f


#define Div_Points 20
static const float bat_soc[Div_Points] = 
{
    1.0000f, 0.9500f, 0.9000f, 0.8501f, 0.8000f, 0.7502f, 0.7003f, 0.6316f, 0.5789f, 0.5263f,
    0.4737f, 0.4211f, 0.3684f, 0.3158f, 0.2632f, 0.2105f, 0.1579f, 0.1053f, 0.0526f, 0.0000f,
};

static const float bat_ocv[Div_Points] = 
{
    4.3477f, 4.2916f, 4.2283f, 4.1793f, 4.1322f, 4.0776f, 4.0398f, 3.9837f, 3.9428f, 3.9012f,
    3.8668f, 3.8423f, 3.8191f, 3.8014f, 3.7981f, 3.7614f, 3.7406f, 3.7183f, 3.6914f, 3.4400f,
};

#define VM_MASK (0x55ab)

BatPower_t BatPower;
#define __this_module (&BatPower)
#define __this_module_size (sizeof(BatPower_t))

#define abs(x) ((x)>0?(x):-(x))

static float LinearOcv(float soc)
{
    float ocv = 3.4400f;

    u8 lidx = Div_Points - 1;
    if(soc <= bat_soc[lidx])
    {
        ocv = bat_ocv[lidx];
        goto __end;
    }else if(soc >= bat_soc[0])
    {
        ocv = bat_ocv[0];
        goto __end;
    }

    for(u8 i = 0; i < lidx; i++) 
    {
        if(soc <= bat_soc[i] && soc >= bat_soc[i+1]) 
        {
            float x1 = bat_soc[i];
            float y1 = bat_ocv[i];
            float x2 = bat_soc[i+1];
            float y2 = bat_ocv[i+1];
            if(x2 - x1 == 0)
                goto __end;
            float k = (y2 - y1)/(x2 - x1);
            ocv = y1 + k*(soc - x1);
            goto __end;
        }
    }

__end:
    return ocv;
}

static float LinearSoc(float ocv)
{
    float soc = 0.0f;

    u8 lidx = Div_Points - 1;
    if(ocv <= bat_ocv[lidx])
    {
        soc = bat_soc[lidx];
        goto __end;
    }else if(ocv >= bat_ocv[0])
    {
        soc = bat_soc[0];
        goto __end;
    }

    for(u8 i = 0; i < lidx; i++) 
    {
        if(ocv <= bat_ocv[i] && ocv >= bat_ocv[i+1]) 
        {
            float x1 = bat_ocv[i];
            float y1 = bat_soc[i];
            float x2 = bat_ocv[i+1];
            float y2 = bat_soc[i+1];
            if(x2 - x1 == 0)
                goto __end;
            float k = (y2 - y1)/(x2 - x1);
            soc = y1 + k*(ocv - x1);
            goto __end;
        }
    }
__end:
    return soc;
}

//user 控制电池使能管教 0:不使能 1:使能
int BatEnableCtrl(u8 en)
{
#if 1
    en = !en;
    gpio_set_die(IO_PORTB_00, 1);
    gpio_direction_output(IO_PORTB_00, en);

    printf("%s:en = %d\n", __func__, en);
#else
    if(en) 
    {
        gpio_set_pull_up(IO_PORTB_00, 0);
        gpio_set_pull_down(IO_PORTB_00, 0);
        gpio_set_direction(IO_PORTB_00, 0);
        gpio_set_output_value(IO_PORTB_00, 0);
	}else 
    {
        gpio_set_pull_up(IO_PORTB_00, 0);
        gpio_set_pull_down(IO_PORTB_00, 0);
        gpio_set_direction(IO_PORTB_00, 1);
	}
#endif

    return 0;
}

int GetBatLevel(void)
{
    int level = 0;
    int power = __this_module->power;
    if(power <= 10) level = 0;
    else if(power <= 20) level = 1;
    else if(power <= 30) level = 2;
    else if(power <= 40) level = 3;
    else if(power <= 50) level = 4;
    else if(power <= 60) level = 5;
    else if(power <= 70) level = 6;
    else if(power <= 80) level = 7;
    else if(power <= 90) level = 8;
    else if(power <= 100) level = 9;

    return level;
}

int GetBatPower(void)
{
    return __this_module->power;
}

void BatPowerSetZero(void)
{
    __this_module->power = 0;
    __this_module->soc = 0.0000f;
    VmPowerWrite();
}

u8 BatPowerIsAllowOta(void)
{
    int power = GetBatPower();
    if(power >= 40) return 1;

    return 0;
}

void BatPowerUpdateHandle(void)
{
    VmPowerWrite();

    return;
}

static void BatSocLoop(void *priv)
{
    static u8 time_cnt = 1;

    u8 motor_work = get_motor_work_flag();
    if(motor_work == true)
    {
        /* 马达工作中跳过本次测量 */
        time_cnt += 1;
        return;
    }

    float cur_ocv = (float)(adc_get_voltage(AD_CH_VBAT) * 4)/1000.0f;
    printf("%s:cur ocv = %f\n", __func__, cur_ocv);
    float last_soc = __this_module->soc;
    float last_ocv = LinearOcv(last_soc);
    printf("%s:last soc = %f, ocv = %f\n", __func__, last_soc, last_ocv);
    //计算损耗电流或存储电流
    float I = (cur_ocv-last_ocv)/Bat_R*1000;
    if(I < Bat_Min_I) I = Bat_Min_I;
    else if (I > Bat_Max_I) I = Bat_Max_I;

    if(LVCMP_DET_GET() == false) 
    {       
        __this_module->soc += I*(time_cnt*8)/3600/Bat_Cap; // 积分运算，单位转换为 mAh
        printf("discharge:I = %f, soc = %f\n", I, __this_module->soc);
    }else
    {
        __this_module->soc += I*(Charge_Factor*time_cnt*8)/3600/Bat_Cap; // 积分运算，单位转换为 mAh
        printf("charge:I = %f, soc = %f\n", I, __this_module->soc);
    }
    time_cnt = 1;

    //限制soc范围
    float soc_min = bat_soc[Div_Points - 1];
    float soc_max = bat_soc[0];
    if(__this_module->soc < soc_min)
        __this_module->soc = soc_min;
    else if(__this_module->soc > soc_max)
        __this_module->soc = soc_max;

    int power = (int)(__this_module->soc*100.0f);
    u8 update = 0;
    if(LVCMP_DET_GET() == false)
    {
        /* 放电电量只会下降 */
        if(power < __this_module->power)
            update = 1;
    }else
    {
        /* 充电电量只会上升 */
        if(power > __this_module->power)
            update = 1;
    }

    printf("power = %d\n", power);

    if(update == 1)
        __this_module->power = power;
    
    int task_post[1];
    task_post[0] = comm_msg_power_update;
    PostCommTaskMsg(task_post, 1);

    return;
}

static void LowPowerIsOnHandle(void)
{
    if(!MenuSupportPopup()) 
        return;

    ui_menu_jump(ui_act_id_low_battery);

    return;
}

static u8 LPRemind;
void LPRemindProcess(struct sys_time *ptime)
{
    bool BondFlag = GetDevBondFlag();
    if(BondFlag == false) return;

    u8 charge_state = GetChargeState();
    if(charge_state == 1) return;

    int cur_power = GetBatPower();
    if(cur_power > 10){
        LPRemind = false;
        return;
    }else if(cur_power == 0)
    {
        LPRemind = false;
        return;
    }

    if(cur_power % 5 == 0)
    {
        /*电量5, 10, 15提醒一次*/
        if(LPRemind == false)
        {
            LPRemind = true;
            LowPowerIsOnHandle();
        }
    }else
    {
        LPRemind = false;
    }
    
    return;
}

void LPShutdownProcess(struct sys_time *ptime)
{
    u8 charge_state = GetChargeState();
    if(charge_state == 1) return;

    int cur_power = GetBatPower();
    if(cur_power > 0) return;

    DevOpShutdownHandle();
    
    return;
}

void BatPowerInit(void)
{
    VmPowerRead();

    int vm_power = __this_module->power;
    printf("%s:vm_power = %d\n", __func__, vm_power);
    if(vm_power <= 0)
    {
        //船运模式激活,充电器插上，此刻浮压严重，不做具体电量读取
        u8 charge_state = GetChargeState();
        if(charge_state == 1)
        {
            __this_module->power = 0xff;
            __this_module->soc = 1.0000f;
        }else
        {
            //开机读取电池真实电压，计算soc，作为后续虚拟电池曲线
            float ocv = (float)(adc_get_voltage(AD_CH_VBAT) * 4)/1000.0f;
            float soc = LinearSoc(ocv);
            printf("%s:ocv = %f, soc = %f\n", __func__, ocv, soc);
            float soc_min = bat_soc[Div_Points - 1];
            float soc_max = bat_soc[0];
            if(soc < soc_min) soc = soc_min;
            else if(soc > soc_max) soc = soc_max;
            __this_module->soc = soc; 
            __this_module->power = (int)(soc*100.0f);
        }
        VmPowerWrite();
    }else
    {
        /* 船运模式激活后，还监测到充电, 等待下一次拔出充电器 */
        u8 charge_state = GetChargeState();
        if(charge_state == 1) return;

        //开机读取电池真实电压，计算soc，作为后续虚拟电池曲线
        float ocv = (float)(adc_get_voltage(AD_CH_VBAT) * 4)/1000.0f;
        float soc = LinearSoc(ocv);
        printf("%s:ocv = %f, soc = %f\n", __func__, ocv, soc);
        float soc_min = bat_soc[Div_Points - 1];
        float soc_max = bat_soc[0];
        if(soc < soc_min) soc = soc_min;
        else if(soc > soc_max) soc = soc_max;

        if(vm_power == 0xff)
        {
            __this_module->soc = soc; 
            __this_module->power = (int)(soc*100.0f);
        }else
        {
            int cur_power = (int)(soc*100.0f);
            int diff = cur_power - vm_power;
            if(abs(diff) >= 5)
            {
                /* 电量差距太大，之前不可取，重新用最新 */
                __this_module->soc = soc;
                __this_module->power = (int)(soc*100.0f);
            }
        }
        VmPowerWrite();

        sys_timer_add(NULL, BatSocLoop, 8*1000);
    }

    return;
}

void VmPowerRead(void)
{
    int ret = syscfg_read(CFG_POWER_PARA_INFO, __this_module, __this_module_size);
    if(ret != __this_module_size || __this_module->mask != VM_MASK)
        VmPowerReset();

    return;
}

void VmPowerWrite(void)
{
    for(u8 i = 0; i < 3; i++)
    {
        int ret = syscfg_write(CFG_POWER_PARA_INFO, __this_module, __this_module_size);
        if(ret == __this_module_size)
            break;
    }
}

void VmPowerReset(void)
{
    __this_module->power = -1;
    __this_module->mask = VM_MASK;
    VmPowerWrite();
}
