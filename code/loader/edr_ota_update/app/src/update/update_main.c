#include "common.h"
#include "asm/crc.h"
#include "update_main.h"
#include "lib_include.h"
#include "asm/power/p33.h"
#include "dec.h"
#include "jlfs.h"


#define LOG_TAG_CONST       UPDATE_MAIN
#define LOG_TAG             "[UPDATE_MAIN]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
#define LOG_DUMP_ENABLE
#define LOG_CLI_ENABLE
#include "debug.h"

extern update_mode_info_t update_mode_info;
extern void update_get_record_form_flash(u8 *ram);
extern u8 *update_param_ext_get(UPDATA_PARM *p, u8 ext_type);
extern int update_loop(update_op_api_t *update_op);
extern void ex_api_start_show(u8 state);
extern u8 ex_api_code_load(u32 opt);
extern void reserved_area_update_file_disable(void);

void ram_protect_close(void);
void bredr_bd_close();
void ll_hci_destory(void);
void update_enter_jump_maskrom(void);
void latch_reset(void);
static u8 *latch_flag = NULL;
u8 jump_mode_check(u8 *p);

#define __this  (&update_mode_info)

// 从扩展升级参数中取出信息，判断是否不升级升级预留区域
static u8 reserved_update_check(u8 *p)
{
    u8 *reserved_zone_update_flag = update_param_ext_get(p, EXT_RESERVED_UPDATE);
    if (reserved_zone_update_flag && reserved_zone_update_flag[0]) {
        reserved_area_update_file_disable();
    }
    return 0;
}

//检查ram里的update信息
bool update_check(void)
{
    u16 crc_cal;
    //check_updata_parm is valid
    UPDATA_PARM *p  = (u8 *)UPDATE_FLAG_ADDR; //定位到ram1最后地址
    log_info("update param:");
    log_info_hexdump(p, sizeof(UPDATA_PARM));
    log_info("update param priv:");
    log_info_hexdump(p->parm_priv, 32);
    crc_cal = chip_crc16(((u8 *)p) + 2, sizeof(UPDATA_PARM) - 2);
    if (crc_cal && crc_cal == p->parm_crc) {
        if ((NON_DEV_UPDATA != p->parm_type) && (UPDATA_READY == p->parm_result)) {
            log_info("update_check ture\n");
            return TRUE;
        }
    }
    return FALSE;
}

//写升级标志到ram to SDK, 通知SDK升级结果
void set_updata_result(u16 type, u16 result)
{
    UPDATA_PARM *p;

    log_info("set update result :%d %d\n", type, result);
    p = (u8 *)UPDATE_FLAG_ADDR;
    memset(p, 0x0, sizeof(UPDATA_PARM));
    p->parm_result = result;
    p->magic = type;
    p->parm_crc = chip_crc16(((u8 *)p) + 2, sizeof(UPDATA_PARM) - 2);
}

void update_main()
{
    UPDATA_PARM *p  = (u8 *)UPDATE_FLAG_ADDR;
    ex_api_code_load(0);
    ex_api_start_show(UPDATA_READY == p->parm_result);
    UPDATE_MODE mode = UPDATE_JUMP;
    UPDATA_RESULT result = UPDATA_RESULT_FAIL;
#if !UART_UPDATE_ONLY_TEST_MODE
    //update_check
    if (update_check() == FALSE) {			                   //RAM中的结构体不存在， 判断为断电重新上电
        log_info("Ota Start PowerOn\n");
        mode = UPDATE_POWERON;
    }

    update_get_record_form_flash(&p);       //从flash里把参数读出来
#if 0
    /* TODO */
    u8 *data = update_param_ext_get(p, EXT_RF_PA_INFO);
    put_buf(data, 3);
#endif

#endif

    jump_mode_check(p);

    reserved_update_check(p);

    /* mode = UPDATE_POWERON; */
    //给各升级模块传递参数
    __this->state_cbk(UPDATE_PARM, p);

    __this->state_cbk(UPDATE_START, &mode);
#if !UART_UPDATE_ONLY_TEST_MODE
    //loop
    result = update_loop(__this->file_op);

    //end
    __this->state_cbk(UPDATE_END,  &result);                    //不同升级完成之后可能需要执行不同操作
#endif
}



void uart_close_all(void)
{
    JL_UART0->CON0 = 0;
    JL_UART1->CON0 = 0;
    JL_UART2->CON0 = 0;
}


u32 get_update_jump_flag()
{
    u8 *ptr = jlfs_get_isd_cfg_ptr();
    u32 u32Val;
    u8 val[16] = {0};
    printf("\n >>>[test]:func = %s,line= %d\n", __FUNCTION__, __LINE__);
    if (dec_isd_cfg_ini("UPDATE_JUMP", (void *)val, ptr)) {
        put_buf(val, 16);
        /* u32Val = (u32) (((u8 *)val)[0] - '0'); */
        u32Val = (u32)val[0];
        printf("update_jump = %d, vol[0] = %d\n", u32Val, val[0]);
        return u32Val;
    } else {
        return 0;
    }
}

u8 jump_mode_check(u8 *p)
{
    if (latch_flag == NULL) {
        /* UPDATA_PARM *p; */
        /* p = (u8 *)UPDATE_FLAG_ADDR; //定位到ram1最后地址 */
        latch_flag = update_param_ext_get(p, EXT_JUMP_FLAG);
    }
    if (latch_flag) {
        printf(">>>[test]:flag = %d\n", latch_flag[0]);
        return latch_flag[0];
    }
    printf(">>>[test]:latch_flag == NULL\n");
    return 0;
}

void update_reset(void)
{
    printf("\n >>>[test]:func = %s,line= %d\n", __FUNCTION__, __LINE__);
    /* 寄存器不会复位 */
#if (0 == UART_UPDATE_ONLY_TEST_MODE)
    u8 *p = NULL;
    update_get_record_form_flash(&p);       //从flash里把参数读出来
    u8 latch_reset = jump_mode_check(p);
    if (latch_reset) {
        puts("latch reset...\n");
        void latch_reset(void);
        latch_reset();
    } else
#endif
    {
        /* uart_close_all(); */
        cpu_reset();
    }
}

