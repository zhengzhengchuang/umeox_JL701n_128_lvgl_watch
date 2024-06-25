#include "typedef.h"
#include "asm/clock.h"
#include "asm/adc_api.h"
#include "timer.h"
#include "init.h"
#include "asm/efuse.h"
#include "irq.h"
#include "asm/power/p33.h"
#include "asm/power/p11.h"
#include "asm/power_interface.h"
#include "jiffies.h"
#include "gpio.h"

static u8 cur_ch = 0;
static u8 adc_div = 0;
struct adc_info_t {
    u32 jiffies;
    u32 ch;
    union {
        u16 value;
        u16 voltage;
    } v;
    u16 adc_voltage_mode;
    u16 sample_period;
};


#define     ENABLE_PREEMPTIVE_MODE 1
#define     VBG_CENTER  801
#define     VBG_RES     3

static struct adc_info_t adc_queue[ADC_MAX_CH + ENABLE_PREEMPTIVE_MODE ];

#define PMU_CH_VALUE_ARRAY_SIZE   (16 + 1)
#define PMU_CH_SAMPLE_FREQ    500 //ms

 u16 vbat_voltage_array[PMU_CH_VALUE_ARRAY_SIZE];
static u16 vbg_value_array[PMU_CH_VALUE_ARRAY_SIZE];

static void adc_value_push(u16 *array, u16 adc_value)
{
    u16 pos = array[0];
    pos++;
    if (pos >= PMU_CH_VALUE_ARRAY_SIZE) {
        pos = 1;
    }
    array[pos] = adc_value;
    /* printf("push %s %d %d ", array == vbg_value_array ? "vbg" : "vbat", pos, array[pos]); */
    array[0] = pos;
}

static u16 adc_value_avg(u16 *array)
{
    u32 i, j, sum = 0;
    for (i = 1, j = 0; i < PMU_CH_VALUE_ARRAY_SIZE; i++) {
        if (array[i]) {
            sum += array[i];
            j += 1;
        }
    }
    if (sum) {
        return (sum / j);
    }
    return 1;
}

static void adc_value_array_reset(u16 *array)
{
    for (int i = 0; i < PMU_CH_VALUE_ARRAY_SIZE; i++) {
        array[i] = 0;
    }
}

u32 adc_add_sample_ch(u32 ch)
{
    u32 i = 0;
    for (i = 0; i < ADC_MAX_CH; i++) {
        if (adc_queue[i].ch == ch) {
            break;
        } else if (adc_queue[i].ch == -1) {
            adc_queue[i].ch = ch;
            adc_queue[i].v.value = 1;
            adc_queue[i].sample_period = 0;

            switch (ch) {
            case AD_CH_VBAT:
                adc_queue[i].adc_voltage_mode = 1;
                break;
            case AD_CH_DTEMP:
                adc_queue[i].adc_voltage_mode = 1;
                break;
            default:
                adc_queue[i].adc_voltage_mode = 0;
                break;
            }
            printf("add sample ch %x\n", ch);
            break;
        }
    }
    return i;
}
u32 adc_set_sample_freq(u32 ch, u32 ms)
{
    u32 i;
    for (i = 0; i < ADC_MAX_CH; i++) {
        if (adc_queue[i].ch == ch) {
            adc_queue[i].sample_period = msecs_to_jiffies(ms);
            adc_queue[i].jiffies = msecs_to_jiffies(ms) + jiffies;
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
static u32 adc_get_next_ch()
{
    if (cur_ch == (u8) - 1) {
        cur_ch = 0;
    }
    for (int i = cur_ch; i < ADC_MAX_CH; i++) {
        if (adc_queue[i].ch != -1) {
            if (adc_queue[i].sample_period) {
                if (time_before(adc_queue[i].jiffies, jiffies)) {
                    adc_queue[i].jiffies += adc_queue[i].sample_period;
                    /* printf("prd---> %d %d\n", jiffies, adc_queue[i].jiffies); */
                } else {
                    continue;
                }
            }

            return i;
        }
    }
    return -1;
}


u32 adc_value_to_voltage(u32 adc_vbg, u32 adc_ch_val)
{
    u32 adc_res = adc_ch_val;
    u32 adc_trim = get_vbg_trim();
    u32 tmp, tmp1;

    tmp1 = adc_trim & 0x0f;
    tmp = (adc_trim & BIT(4)) ? VBG_CENTER - tmp1 * VBG_RES : VBG_CENTER + tmp1 * VBG_RES;
    /* printf("adc_res %d mv vbg:%d adc:%d adc_trim:%x\n", adc_res, adc_vbg, adc_ch_val, adc_trim); */

    if (adc_vbg == 0) {
        adc_vbg = 1;        //防止div0异常
    }
    adc_res = adc_res * tmp / adc_vbg;
    return adc_res;
}

u32 adc_raw_to_voltage(u32 adc_raw_value)
{
    u32 adc_vbg = adc_get_value(AD_CH_LDOREF);
    return adc_value_to_voltage(adc_vbg, adc_raw_value);
}

u32 adc_check_vbat_lowpower()
{
    return 0;
}
static void adc_audio_ch_select(u32 ch)
{
    SFR(JL_ADDA->ADDA_CON1, 1, 1, 1);
    SFR(JL_ADDA->ADDA_CON0, 0, 12, 0);

    switch (ch << 16) {
    case AD_AUDIO_VADADC:
        SFR(JL_ADDA->ADDA_CON0, 12, 1, 1);
        break;
    case AD_AUDIO_VCM:
        SFR(JL_ADDA->ADDA_CON0, 11, 1, 1);
        break;
    case AD_AUDIO_VBGLDO:
        SFR(JL_ADDA->ADDA_CON0, 10, 1, 1);
        break;
    case AD_AUDIO_HPVDD:
        SFR(JL_ADDA->ADDA_CON0, 9, 1, 1);
        break;
    case AD_AUDIO_RN:
        SFR(JL_ADDA->ADDA_CON0, 4, 1, 1);
        break;
    case AD_AUDIO_RP:
        SFR(JL_ADDA->ADDA_CON0, 3, 1, 1);
        break;
    case AD_AUDIO_LN:
        SFR(JL_ADDA->ADDA_CON0, 2, 1, 1);
        break;
    case AD_AUDIO_LP:
        SFR(JL_ADDA->ADDA_CON0, 1, 1, 1);
        break;
    case AD_AUDIO_DACVDD:
        SFR(JL_ADDA->ADDA_CON0, 0, 1, 1);
        break;
    case AD_AUDIO_MICIN0:
        SFR(JL_ADDA->ADDA_CON0, 5, 4, 0);
        break;
    case AD_AUDIO_MICIN1:
        SFR(JL_ADDA->ADDA_CON0, 5, 4, 1);
        break;
    case AD_AUDIO_MICIN2:
        SFR(JL_ADDA->ADDA_CON0, 5, 4, 2);
        break;
    case AD_AUDIO_MICIN3:
        SFR(JL_ADDA->ADDA_CON0, 5, 4, 3);
        break;
    default:
        break;
    }
}
static void adc_sample(u32 ch)
{
    u32 adc_con = 0;
    SFR(adc_con, 0, 3, adc_div);//div 96

    adc_con |= (0x1 << 12); //启动延时控制，实际启动延时为此数值*8个ADC时钟
    adc_con |= BIT(3); //ana en
    adc_con |= BIT(6); //clr pend
    adc_con |= BIT(5);//ie
    adc_con |= BIT(17);//clk en
    /* adc_con |= BIT(18)|BIT(19);//clk en */

    SFR(adc_con, 21, 3, 0b010);

    switch (ch & 0xff) {
    case AD_CH_BT:
        SFR(adc_con, 18, 3, AD_CH_BT & 0xf);
        break;
    case AD_CH_PMU:
        SFR(adc_con, 18, 3, AD_CH_PMU & 0xf);
        adc_pmu_ch_select(ch >> 16);
        break;

    case AD_CH_AUDIO:
        SFR(adc_con, 18, 3, AD_CH_AUDIO & 0xf);
        adc_audio_ch_select(ch >> 16);
        break;

    case AD_CH_LPCTM:
        SFR(adc_con, 18, 3, AD_CH_LPCTM & 0xf);
        break;

    case AD_CH_X32K:
        SFR(adc_con, 18, 3, AD_CH_X32K & 0xf);
        break;

    case AD_CH_PLL1:
        SFR(adc_con, 18, 3, AD_CH_PLL1 & 0xf);
        break;

    default:
        SFR(adc_con, 21, 3, 0b001);
        SFR(adc_con, 8, 4, ch & 0xf);
        break;
    }

    JL_ADC->CON = adc_con;
    JL_ADC->CON |= BIT(4);//en


    JL_ADC->CON |= BIT(6);//kistart
//   printf("ch %x %x %x %x",ch,ch >> 16,JL_ADC->CON,P3_ANA_CON4);
}
static u8 adc_ch_io_table[] = {
    IO_PORTA_00,
    IO_PORTA_05,
    IO_PORTA_06,
    IO_PORTA_08,
    IO_PORTC_04,
    IO_PORTC_05,
    IO_PORTB_01,
    IO_PORTB_02,
    IO_PORTB_05,
    IO_PORTB_09,
    IO_PORT_DP,
    IO_PORT_DM,
    IO_PORTG_00,
    IO_PORTG_01,
    IO_PORTG_05,
    IO_PORTG_07,
};

u8 adc_ch_for_io_port(int gpio)
{
    for (u8 ch = 0; ch < ARRAY_SIZE(adc_ch_io_table); ch++) {
        if (adc_ch_io_table[ch] == gpio) {
            return ch;
        }
    }
    return 0xff;
}

void adc_analog_2_io(u32 analog_ch, u32 adc_io)
{
    gpio_set_direction(adc_io, 1);
    gpio_set_die(adc_io, 0);
    gpio_set_pull_up(adc_io, 0);
    gpio_set_pull_down(adc_io, 0);

    adc_sample(analog_ch);

    SFR(JL_ADC->CON, 8, 4, adc_ch_for_io_port(adc_io));
    SFR(JL_ADC->CON, 21, 3, 0b111);
}

static u16 adc_wait_pnd()
{
    asm("csync");
    while (!(JL_ADC->CON & BIT(7)));
    asm("csync");
    u32 adc_res = JL_ADC->RES;
    JL_ADC->CON |= BIT(6);
    return adc_res;
}
static void adc_close()
{
    JL_ADC->CON = BIT(17);//clock_en
    JL_ADC->CON = BIT(17) | BIT(6);
    JL_ADC->CON = BIT(6);
}

___interrupt
static void adc_isr()
{
    u32 adc_value = JL_ADC->RES;
    u8 t_ch = cur_ch;

    if (t_ch == (u8) - 1) {
        adc_close();
        return;
    }

    if (adc_queue[t_ch].adc_voltage_mode == 1) {
        u32 vbg_value = adc_queue[0].v.value;
        adc_queue[t_ch].v.voltage = adc_value_to_voltage(vbg_value, adc_value);
        if (adc_queue[t_ch].ch == AD_CH_VBAT) {
            adc_value_push(vbat_voltage_array, adc_queue[t_ch].v.voltage);
        }
        /* printf("%d ad[%x]: %d %d", cur_ch, adc_queue[cur_ch].ch, adc_value, adc_queue[cur_ch].v.voltage); */
    } else  if (adc_queue[t_ch].ch == AD_CH_LDOREF) {
        adc_queue[0].v.value = adc_value;
        adc_value_push(vbg_value_array, adc_value);
    } else {
        ASSERT(t_ch < ADC_MAX_CH + ENABLE_PREEMPTIVE_MODE, "%d ", t_ch)
        adc_queue[t_ch].v.value = adc_value;
    }
    /* printf("%d ad[%x]: %d %d", cur_ch, adc_queue[cur_ch].ch, adc_value, adc_queue[cur_ch].v.voltage); */
    if (adc_queue[ADC_MAX_CH].ch != -1) {
        adc_close();
        return;
    }

    cur_ch = t_ch + 1;
    cur_ch = adc_get_next_ch();
    if (cur_ch == (u8) - 1) {
        adc_close();
    } else {
        adc_sample(adc_queue[cur_ch].ch);
    }
}

static void adc_scan()
{
    if (adc_queue[ADC_MAX_CH].ch != -1) {
        return;
    }

    //上次采样没结束
    if (JL_ADC->CON & BIT(4)) {
        return;
    }

    cur_ch = adc_get_next_ch();
    if (cur_ch == (u8) - 1) {
        return;
    }

    adc_sample(adc_queue[cur_ch].ch);
}

u32 adc_get_value_preemption(u32 ch)
{
    adc_queue[ADC_MAX_CH].ch = ch;

    while (JL_ADC->CON & BIT(4)) {
    }


    local_irq_disable();

    cur_ch = ADC_MAX_CH;
    adc_sample(adc_queue[ADC_MAX_CH].ch);

    adc_queue[ADC_MAX_CH].v.value = adc_wait_pnd();

    adc_queue[ADC_MAX_CH].ch = -1;

    JL_ADC->CON &= ~BIT(4);//en

    local_irq_enable();


    return adc_queue[ADC_MAX_CH].v.value;
}

u32 adc_get_value(u32 ch)
{
    if (ch == AD_CH_VBAT) {
        return 1;
    } else if (ch == AD_CH_LDOREF) {
        return adc_value_avg(vbg_value_array);
    }

    for (int i = 0; i < ADC_MAX_CH; i++) {
        if (adc_queue[i].ch == ch) {
            return adc_queue[i].v.value;
        }
    }
    return 1;
}

u32 adc_get_voltage(u32 ch)
{
    if (ch == AD_CH_VBAT) {
        return adc_value_avg(vbat_voltage_array);
    }

    for (int i = 0; i < ADC_MAX_CH + ENABLE_PREEMPTIVE_MODE; i++) {
        if (ch == adc_queue[i].ch && adc_queue[i].adc_voltage_mode == 1) {
            return adc_queue[i].v.voltage;
        }
    }

    u32 adc_vbg = adc_get_value(AD_CH_LDOREF);
    u32 adc_res;
    if (ch == AD_CH_IOVDD) {
        adc_res = 1023;
    } else {
        adc_res = adc_get_value(ch);
    }
    return adc_value_to_voltage(adc_vbg, adc_res);
}

u32 adc_get_voltage_preemption(u32 ch)
{
    u32 vbg_adc_value = 0;
    u32 vbg_min_value = -1;
    u32 vbg_max_value = 0;

    u32 ch_adc_value = 0;
    u32 ch_min_value = -1;
    u32 ch_max_value = 0;

    for (int i = 0; i < 12; i++) {

        u32 adc_value = adc_get_value_preemption(AD_CH_LDOREF);
        if (adc_value > vbg_max_value) {
            vbg_max_value = adc_value;
        } else if (adc_value < vbg_min_value) {
            vbg_min_value = adc_value;
        }

        vbg_adc_value += adc_value;

        if (ch == AD_CH_IOVDD) {
            continue;
        }

        adc_value = adc_get_value_preemption(ch);

        if (adc_value > ch_max_value) {
            ch_max_value = adc_value;
        } else if (adc_value < ch_min_value) {
            ch_min_value = adc_value;
        }
        ch_adc_value += adc_value;
    }

    vbg_adc_value -= vbg_max_value;
    vbg_adc_value -= vbg_min_value;

    vbg_adc_value /= 10;
    /* printf("%s() %d vbg: %d min: %d max: %d  ", __func__, __LINE__, vbg_adc_value, vbg_min_value, vbg_max_value); */


    if (ch == AD_CH_IOVDD) {
        ch_adc_value = 1023;
    } else {
        ch_adc_value -= ch_max_value;
        ch_adc_value -= ch_min_value;

        ch_adc_value /= 10;
    }

    /* printf("%s() %d ch: %d min: %d max: %d  ", __func__, __LINE__, ch_adc_value, ch_min_value, ch_max_value); */
    return adc_value_to_voltage(vbg_adc_value, ch_adc_value);
}

static int hpvdd_voltage = 0;
static void read_hpvdd_voltage(void)
{
    hpvdd_voltage = adc_get_voltage_preemption(AD_CH_AUDIO_HPVDD);
    printf("hpvdd = %d mV\n", hpvdd_voltage);
}

int get_hpvdd_voltage(void)
{
    return hpvdd_voltage;
}

static void clock_critical_enter(void)
{
}
static void clock_critical_exit(void)
{
    const u8 adc_div_table[] = {1, 6, 12, 24, 48, 72, 96, 128};
    const u32 lsb_clk = clk_get("lsb");
    adc_div = 7;
    for (int i = 0; i < ARRAY_SIZE(adc_div_table); i++) {
        if (lsb_clk / adc_div_table[i] <= 1000000) {
            adc_div = i;
            break;
        }
    }
}
CLOCK_CRITICAL_HANDLE_REG(saradc, clock_critical_enter, clock_critical_exit)

static void _adc_init(u32 sys_lvd_en)
{
    memset(adc_queue, 0xff, sizeof(adc_queue));
    memset(vbg_value_array, 0x0, sizeof(vbg_value_array));
    memset(vbat_voltage_array, 0x0, sizeof(vbat_voltage_array));

    adc_close();

    adc_add_sample_ch(AD_CH_LDOREF);
    adc_set_sample_freq(AD_CH_LDOREF, PMU_CH_SAMPLE_FREQ);

    adc_add_sample_ch(AD_CH_VBAT);
    adc_set_sample_freq(AD_CH_VBAT, PMU_CH_SAMPLE_FREQ);

    adc_add_sample_ch(AD_CH_DTEMP);
    adc_set_sample_freq(AD_CH_DTEMP, PMU_CH_SAMPLE_FREQ);

    //add user
    // adc_add_sample_ch(AD_CH_PB1);
    // adc_set_sample_freq(AD_CH_PB1, PMU_CH_SAMPLE_FREQ);

    u32 vbg_adc_value = 0;
    u32 vbg_min_value = -1;
    u32 vbg_max_value = 0;


    for (int i = 0; i < PMU_CH_VALUE_ARRAY_SIZE; i++) {
        u32 adc_value = adc_get_value_preemption(AD_CH_LDOREF);
        if (adc_value > vbg_max_value) {
            vbg_max_value = adc_value;
        } else if (adc_value < vbg_min_value) {
            vbg_min_value = adc_value;
        }
        vbg_adc_value += adc_value;
    }
    vbg_adc_value -= vbg_max_value;
    vbg_adc_value -= vbg_min_value;

    vbg_adc_value /= (PMU_CH_VALUE_ARRAY_SIZE - 2);
    adc_value_push(vbg_value_array, vbg_adc_value);
    printf("vbg_adc_value = %d\n", adc_value_avg(vbg_value_array));

    u32 voltage = adc_get_voltage_preemption(AD_CH_VBAT);
    adc_value_push(vbat_voltage_array, voltage);

    printf("vbat = %d mv\n", adc_get_voltage(AD_CH_VBAT) * 4);

    u32 dtemp_voltage = adc_get_voltage_preemption(AD_CH_DTEMP);
    printf("dtemp = %d mv\n", dtemp_voltage);

    read_hpvdd_voltage();
    //切换到vbg通道
    adc_get_value_preemption(AD_CH_IOVDD);


    request_irq(IRQ_GPADC_IDX, 1, adc_isr, 0);

    usr_timer_add(NULL, adc_scan, 5, 0);

    /* void adc_test();                           */
    /* sys_timer_add(NULL, adc_test, 1000); //2ms */
}

void adc_init()
{
    adc_div = 7;
    adc_close();

    adc_pmu_detect_en(1);

    volatage_trim_init();

    _adc_init(1);
}
void adc_test()
{
    /* printf("%s() vbat trim:%x\n", __func__, get_vbat_trim());  */

    adc_add_sample_ch(AD_CH_PA5);
    adc_add_sample_ch(AD_CH_PA6);
    adc_add_sample_ch(AD_CH_PB1);


    JL_PORTA->DIR |= BIT(0);
    JL_PORTA->PU  &= ~BIT(0);
    JL_PORTA->DIE &= ~BIT(0);
    JL_PORTA->PD &= ~BIT(0);

    u32 adc_value;

    printf("\n\n\n%s() adc_div %d, VBG: %d VBAT: %d mv", __func__, adc_div, adc_get_value(AD_CH_LDOREF), adc_get_voltage(AD_CH_VBAT) * 4);
    printf("%s() vbg trim: %x iovdd %d mv", __func__, get_vbg_trim(), adc_get_voltage(AD_CH_IOVDD));
    printf("%s() Dtemp %d ", __func__,  adc_get_voltage(AD_CH_DTEMP));

    printf("P3_ANA_CON5 %x", P3_ANA_CON5);

    adc_value = adc_get_value_preemption(AD_CH_PA0);
    printf("%s() preemption mode %d %d mv\n\n", __func__, adc_value, adc_raw_to_voltage(adc_value));
}

void adc_vbg_init()
{
    return ;
}
