#include "generic/ascii.h"
#include "uC-Shell/Terminal/Source/terminal.h"
#include "system/init.h"
#include "asm/power/power_api.h"
#include  "app_config.h"

#if TCFG_UC_SHELL_EN

extern u32 HEAP_BEGIN;

static void u8hex2str(char *buf, u8 dat)
{
    u8 c = dat >> 4;
    if (c > 9) {
        *buf++ = c - 10 + 'A';
    } else {
        *buf++ = c + '0';
    }
    c = dat & 0xf;
    if (c > 9) {
        *buf++ = c - 10 + 'A';
    } else {
        *buf++ = c + '0';
    }
    *buf = ' ';
}

static int hex2str(char *dest, int buf_len, char *src, int len)
{
    int offset = 0;
    for (int i = 0; i < len; i++) {
        if (i != 0 && i % 16 == 0) {
            dest[offset++] = '\r';
            dest[offset++] = '\n';
        }
        u8hex2str(dest + offset, *src++);
        offset += 3;
    }

    dest[offset++] = '\r';
    dest[offset++] = '\n';
    return offset;
}

/*
 * 读内存数据, 示例：cmd_rd 0x100000 4
 *
 */
static  CPU_INT16S  ShShell_cmd_read(CPU_INT16U        argc,
                                     CPU_CHAR         *argv[],
                                     SHELL_OUT_FNCT    out_fnct,
                                     SHELL_CMD_PARAM  *pcmd_param)
{
    if (argc < 3) {
        return SHELL_ERR_NONE;
    }

    /* 获取长度 */
    u32 len;
    ASCII_StrToInt(argv[2], &len, strlen(argv[2]));
    if (len > 128) {
        const char *err_str = "len must < 128";
        Terminal_WrStr((void *)err_str, strlen(err_str));
        return SHELL_ERR_NONE;
    }

    /* 获取地址, 需要根据校验地址是否在有效范围, 否则可能导致读异常
     * 有效地址范围请根据实际芯片修改
     */
    u32 addr = ASCII_StrToHex(argv[1]);
    do {
#ifdef CONFIG_CPU_BR27
        if (addr >= 0x100000 && addr < (u32)&HEAP_BEGIN) {
            break;
        }
        if (addr >= 0xff0000 || addr < 0xff0800) {
            break;
        }
#elif defined CONFIG_CPU_BR28
        if (addr >= 0x100000 && addr < (u32)&HEAP_BEGIN) {
            break;
        }
        if (addr >= 0xff0000 || addr < 0xff0800) {
            break;
        }
#else
        const char *err_str = "unknow cpu";
        Terminal_WrStr((void *)err_str, strlen(err_str));
        return SHELL_ERR_NONE;
#endif

        const char *err_str = "addr rang err";
        Terminal_WrStr((void *)err_str, strlen(err_str));
        return SHELL_ERR_NONE;
    } while (0);

    /*put_buf((u8 *)addr, len);*/

    char strbuf[512];
    int ret_len = hex2str(strbuf, 512, (char *)addr, len);

    Terminal_WrStr((void *)strbuf, ret_len);

    return SHELL_ERR_NONE;
}


/*
 * 写数据, 示例：cmd_wt 0x100000 4 0x12345678
 *
 */
static  CPU_INT16S  ShShell_cmd_write(CPU_INT16U        argc,
                                      CPU_CHAR         *argv[],
                                      SHELL_OUT_FNCT    out_fnct,
                                      SHELL_CMD_PARAM  *pcmd_param)
{
    if (argc < 3) {
        return SHELL_ERR_NONE;
    }

    /* 获取地址, 需要根据校验地址是否在有效范围, 否则可能导致读异常
     * 有效地址范围请根据实际芯片修改
     */
    u32 addr = ASCII_StrToHex(argv[1]);
    do {
#ifdef CONFIG_CPU_BR27
        if (addr >= 0x100000 && addr < (u32)&HEAP_BEGIN) {
            break;
        }
        if (addr >= 0xff0000 || addr < 0xff0800) {
            break;
        }
#elif defined CONFIG_CPU_BR28
        if (addr >= 0x100000 && addr < (u32)&HEAP_BEGIN) {
            break;
        }
        if (addr >= 0xff0000 || addr < 0xff0800) {
            break;
        }
#else
        const char *err_str = "unknow cpu";
        Terminal_WrStr((void *)err_str, strlen(err_str));
        return SHELL_ERR_NONE;
#endif

        const char *err_str = "addr rang err";
        Terminal_WrStr((void *)err_str, strlen(err_str));
        return SHELL_ERR_NONE;
    } while (0);

    u32 value = ASCII_StrToHex(argv[3]);

    if (argv[2][0] == '1') {
        *(u8 *)addr = value;
    } else if (argv[2][0] == '2') {
        *(u16 *)addr = value;
    } else if (argv[2][0] == '4') {
        *(u32 *)addr = value;
    } else {
        const char *err_str = "len must be 1, 2, 4";
        Terminal_WrStr((void *)err_str, strlen(err_str));
        return SHELL_ERR_NONE;
    }

    char strbuf[128];
    int offset = snprintf(strbuf, 128, "[%s] = %s", argv[1], argv[3]);
    printf("0000 %s\n", strbuf);

    Terminal_WrStr((void *)strbuf, offset);

    return SHELL_ERR_NONE;
}

static u8 g_sleep_disable = 0;
static u8 shell_lp_query()
{
    return !g_sleep_disable;
}

REGISTER_LP_TARGET(shell_lp_target) = {
    .name = "shell",
    .is_idle = shell_lp_query,
};

static  CPU_INT16S  ShShell_cmd_sleep(CPU_INT16U        argc,
                                      CPU_CHAR         *argv[],
                                      SHELL_OUT_FNCT    out_fnct,
                                      SHELL_CMD_PARAM  *pcmd_param)
{
    if (argc < 2) {
        return SHELL_ERR_NONE;
    }
    if (argv[1][0] == '0') {
        g_sleep_disable = 1;
    } else {
        g_sleep_disable = 0;
    }
    return SHELL_ERR_NONE;
}

/*
 * 命令列表, 必须使用cmd_前缀
 */
static SHELL_CMD  ShShell_CmdTbl [] = {
    {"cmd_rd", ShShell_cmd_read},
    {"cmd_wt", ShShell_cmd_write},
    {"cmd_sleep", ShShell_cmd_sleep},

    {0,         0           }
};

static u32 shell_rw_test = 0xAABBCCDD;


/*
 * 说明:
 * 1. 终端串口配置在terminal_serial.c
 *
 */

static int uc_shell_init()
{
    SHELL_ERR err;

    Terminal_Init();

    Shell_Init();
    Shell_CmdTblAdd((CPU_CHAR *)"cmd", ShShell_CmdTbl, &err);

    printf("uc_shell_init: %x\n", (u32)&shell_rw_test);

    return 0;
}
late_initcall(uc_shell_init);


#endif


