#include "typedef.h"
#include "asm/clock.h"
#include "asm/adc_api.h"
#include "asm/timer.h"
#include "init.h"
#include "asm/efuse.h"
#include "log.h"
//FIXME:
//#include "p33.h"
//
struct adc_info_t {
    u32 ch;
    u16 value;
};

#define     ENABLE_OCCUPY_MODE 1

extern u16 sys_timer_add(void *priv, void (*func)(void *priv), u32 msec);
static struct adc_info_t adc_queue[ADC_MAX_CH + ENABLE_OCCUPY_MODE];
static u16 vbg_value;
#define     ADC_SRC_CLK clk_get("adc")

/*config adc clk according to sys_clk*/
static const u32 sys2adc_clk_info[] = {
    128000000L,
    96000000L,
    72000000L,
    48000000L,
    24000000L,
    12000000L,
    6000000L,
    1000000L,
};

u32 adc_add_sample_ch(u32 ch)
{
    u32 i = 0;
    for (i = 0; i < ADC_MAX_CH; i++) {
        /* printf("%s() %d %x %x\n", __func__, i, ch, adc_queue[i].ch); */
        if (adc_queue[i].ch == ch) {
            break;
        } else if (adc_queue[i].ch == -1) {
            adc_queue[i].ch = ch;
            printf("add sample ch %x\n", ch);
            break;
        }
    }
    return i;
}
u32 adc_remove_sample_ch(u32 ch)
{
    u32 i = 0;
    for (i = 0; i < ADC_MAX_CH; i++) {
        if (adc_queue[i].ch == ch) {
            adc_queue[i].ch = -1;
            break;
        }
    }
    return i;
}
static u32 adc_get_next_ch(u32 cur_ch)
{
    for (int i = cur_ch + 1; i < ADC_MAX_CH; i++) {
        if (adc_queue[i].ch != -1) {
            return i;
        }
    }
    return 0;
}
u32 adc_get_value(u32 ch)
{
    if (ch == AD_CH_LDOREF) {
        return vbg_value;
    }

    for (int i = 0; i < ADC_MAX_CH; i++) {
        /* printf("%s() ch:%x %x \n",__func__,ch,adc_queue[i].ch); */
        if (adc_queue[i].ch == ch) {
            /* printf("find ch %x\n",adc_queue[i].value); */
            return adc_queue[i].value;
        }
    }
    return 0;
}
extern u32 get_vbg_trim();
u32 adc_get_voltage(u32 ch)
{
#ifdef CONFIG_FPGA_ENABLE
    return 1000;
#endif

#define     CENTER 1200
    u32 adc_vbg = adc_get_value(AD_CH_LDOREF);
    u32 adc_res = adc_get_value(ch);

    u32 adc_trim = get_vbg_trim();
    u32 tmp, tmp1;

    tmp1 = adc_trim & 0x1f;
    tmp = (adc_trim & BIT(5)) ? CENTER - tmp1 * 3 : CENTER + tmp1 * 3;
    adc_res = adc_res * tmp / adc_vbg;
    return adc_res;
}

u32 adc_check_vbat_lowpower()
{
    u32 vbat = adc_get_value(AD_CH_VBAT);
    return __builtin_abs(vbat - 255) < 5;
}

void adc_audio_ch_select(u32 ch)
{
    SFR(JL_ADDA->DAA_CON0, 12, 4, ch);
}
u32 adc_sample(u32 ch)
{
    /* printf("%s() %x\n",__func__,ch); */
    while (!(JL_ADC->CON & BIT(7))) { //wait pending
    }

    u32 value = JL_ADC->RES;

    SFR(JL_ADC->CON, 8, 4, ch & 0xf);

    if ((ch & 0xffff) == AD_CH_PMU) {
        adc_pmu_ch_select(ch >> 16);
    } else if ((ch & 0xffff) == AD_CH_AUDIO) {
        adc_audio_ch_select(ch >> 16);
    } else if ((ch & 0xffff) == AD_CH_BT) {

    }

    JL_ADC->CON |= BIT(6);
    return value;
}
void adc_close()
{
    JL_ADC->CON = 0;
    JL_ADC->CON = 0;
}
void adc_suspend()
{
    JL_ADC->CON &= ~BIT(4);
}
void adc_resume()
{
    JL_ADC->CON |= BIT(4);
}
void adc_enter_occupy_mode(u32 ch)
{
    adc_queue[ADC_MAX_CH].ch = ch;
    adc_sample(ch);
}
void adc_exit_occupy_mode()
{
    adc_queue[ADC_MAX_CH].ch = -1;
}
u32 adc_occupy_run()
{
    if (adc_queue[ADC_MAX_CH].ch != -1) {
        while (!(JL_ADC->CON & BIT(7))) { //wait pending
        }
        adc_queue[ADC_MAX_CH].value = JL_ADC->RES;

        JL_ADC->CON |= BIT(6);
        return adc_queue[ADC_MAX_CH].value;
    }
    return 0;
}
u32 adc_get_occupy_value()
{
    if (adc_queue[ADC_MAX_CH].ch != -1) {
        return adc_queue[ADC_MAX_CH].value;
    }
    return 0;
}
u32 get_adc_div(u32 src_clk)
{
    u32 adc_clk;
    u32 adc_clk_idx;
    u32 cnt;
    adc_clk = src_clk;
    cnt = ARRAY_SIZE(sys2adc_clk_info);
    for (adc_clk_idx = 0; adc_clk_idx < cnt; adc_clk_idx ++) {
        if (adc_clk > sys2adc_clk_info[adc_clk_idx]) {
            break;
        }
    }

    if (adc_clk_idx < cnt) {
        adc_clk_idx = cnt - adc_clk_idx;
    } else {
        adc_clk_idx = cnt - 1;
    }
    return adc_clk_idx;
}
void adc_scan(void *priv)
{
    /* printf("%s()\n",__func__); */
    if (!(JL_ADC->CON & BIT(4))) { //adc disable
        return;
    }

    static u8 cur_ch = 0;
    u8 next_ch;
    if (adc_queue[ADC_MAX_CH].ch != -1) {//occupy mode
        return;
    }
    next_ch = adc_get_next_ch(cur_ch);

    u32 adc_con = JL_ADC->CON;
    u32 value = adc_sample(adc_queue[next_ch].ch);
    if ((value == 0) && (adc_queue[cur_ch].ch == AD_CH_LDOREF)) {
        log_e("adc value error ch%x %x\n", adc_queue[cur_ch].ch, adc_con);
    } else {
        adc_queue[cur_ch].value = value;
    }

    cur_ch = next_ch;

}
void _adc_init(u32 sys_lvd_en)
{
    memset(adc_queue, 0xff, sizeof(adc_queue));

    adc_add_sample_ch(AD_CH_VBAT);
    adc_add_sample_ch(AD_CH_LDOREF);

    //TODO:获取adctrim值
    if (sys_lvd_en) {
        adc_pmu_detect_en(ADC_PMU_CH_VBAT >> 16);
    }

    /* u32 clk_div = get_adc_div(ADC_SRC_CLK); */

    JL_ADC->CON = 0;
    JL_ADC->CON = 0;
    SFR(JL_ADC->CON, 0, 3, 0b110);//div 96

    JL_ADC->CON |= (0xf << 12); //启动延时控制，实际启动延时为此数值*8个ADC时钟
    JL_ADC->CON |= (adc_queue[0].ch & 0xf) << 8;
    JL_ADC->CON |= BIT(3);
    JL_ADC->CON |= BIT(4);
    JL_ADC->CON |= BIT(6);

    adc_queue[0].value = adc_sample(AD_CH_LDOREF);
    if (adc_queue[ADC_MAX_CH].ch != -1) {
        adc_queue[1].value = adc_sample(adc_queue[ADC_MAX_CH].ch);
    } else {
        adc_queue[1].value = adc_sample(AD_CH_VBAT);
    }

    sys_timer_add(NULL, adc_scan, 2);
    /* sys_s_hi_timer_add(NULL, adc_scan, 2); //2ms */
}
void adc_init()
{
    _adc_init(1);
    /* void efuse_test(); */
    /* efuse_test();      */
}
//late_initcall(adc_init);

void adc_test()
{
    /* printf("\n\n%s() chip_id :%x\n", __func__, get_chip_id()); */
    /* printf("%s() vbg trim:%x\n", __func__, get_vbg_trim()); */
    /* printf("%s() vbat trim:%x\n", __func__, get_vbat_trim()); */

    printf("%s() LDOREF:%d %d mv\n", __func__,
           adc_get_value(AD_CH_LDOREF), adc_get_voltage(AD_CH_LDOREF));

    printf("\n%s() VBAT:%d %d mv\n\n", __func__,
           adc_get_value(AD_CH_VBAT), adc_get_voltage(AD_CH_VBAT) * 4);
}
void adc_vbg_init()
{
    return;
    adc_vdc13_save();
    delay(500 * 25); //500us
    vbg_value = 0;
    u32 wla_con0 = JL_WLA->WLA_CON0;
    u32 wla_con1 = JL_WLA->WLA_CON1;

    SFR(JL_WLA->WLA_CON0, 0, 1, 1);  //BG_EN_12v
    SFR(JL_WLA->WLA_CON0, 1, 5, 8);  //BG_ISEL_12v[4:0]
    SFR(JL_WLA->WLA_CON0, 6, 1, 1);  //TRXLDO_EN_12v
    SFR(JL_WLA->WLA_CON0, 7, 4, 4);  //BTCP_EN_12v[3:0]    2: VREF
    SFR(JL_WLA->WLA_CON0, 11, 1, 0); //TB_EN_12vff
    SFR(JL_WLA->WLA_CON0, 12, 3, 0); //TB_SEL_12v[2:0]
    SFR(JL_WLA->WLA_CON0, 15, 1, 0); //TX_TEST_EN_12v
    SFR(JL_WLA->WLA_CON0, 16, 1, 0); //RX_BIAS_EN_12v
    SFR(JL_WLA->WLA_CON0, 17, 2, 0); //RX_O_EN_S_12v[1:0]
    SFR(JL_WLA->WLA_CON0, 19, 5, 0); //RX_VCM_S_12v[4:0]

    SFR(JL_WLA->WLA_CON1, 0, 1, 0);  // AVDDFE bypass enable control bit
    SFR(JL_WLA->WLA_CON1, 3, 2, 2); //AVDDFE_S_12v[1:0]   // AVDDFE_LDO output-->0x0:1.0V;0x1:1.1V;0x2:1.2V;0x3:1.3V

    JL_ADC->CON = 0;
    JL_ADC->CON = 0;
    SFR(JL_ADC->CON, 0, 3, 0b110);//div 96

    JL_ADC->CON |= (0xf << 12); //启动延时控制，实际启动延时为此数值*8个ADC时钟
    JL_ADC->CON |= BIT(3);
    JL_ADC->CON |= BIT(4);
    JL_ADC->CON |= (0b1110) << 8;
    JL_ADC->CON |= BIT(6);
    for (int i = 0; i < 10; i++) {
        while (!(JL_ADC->CON & BIT(7))) { //wait pending
        }

        vbg_value += JL_ADC->RES;
        JL_ADC->CON |= BIT(6);
    }
    JL_ADC->CON = 0;
    JL_ADC->CON = 0;

    vbg_value /= 10;
    printf("vbg value %d\n", vbg_value);
    JL_WLA->WLA_CON0 = wla_con0;
    JL_WLA->WLA_CON1 = wla_con1;

    adc_vdc13_restore();
}
//__initcall(adc_vbg_init);

