/*
*********************************************************************************************************
*                                              uC/Shell
*                                            Shell utility
*
*                    Copyright 2007-2020 Silicon Laboratories Inc. www.silabs.com
*
*                                 SPDX-License-Identifier: APACHE-2.0
*
*               This software is subject to an open source license and is distributed by
*                Silicon Laboratories Inc. pursuant to the terms of the Apache License,
*                    Version 2.0 available at www.apache.org/licenses/LICENSE-2.0.
*
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                  SHELL UTILITY CONFIGURATION FILE
*
*                                              TEMPLATE
*
* Filename : shell_cfg.h
* Version  : V1.04.00
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                                 MODULE
*********************************************************************************************************
*/

#ifndef  SHELL_CFG_H
#define  SHELL_CFG_H

#define DEF_NULL (void *)0
#define DEF_OK 1
#define DEF_FAIL 0
#define DEF_YES 1
#define DEF_NO 0
#define DEF_TRUE 1
#define DEF_FALSE 0
#define DEF_ENABLED 1
#define DEF_DISABLED 0
#define DEF_INT_16U_MAX_VAL 0xffff
#define Str_Cmp strcmp
#define Str_Len strlen
#define Str_Copy strcpy
#define Str_Copy_N strncpy
#define Str_Cat strcat
#define Mem_Set memset
#define ASCII_CHAR_NULL '\0'
#define ASCII_CHAR_SPACE ' '
#define ASCII_CHAR_BACKSPACE 0x08

#ifndef TERMINAL_OS_CFG_TASK_PRIO
#define TERMINAL_OS_CFG_TASK_PRIO 1
#endif

typedef short  CPU_INT16S;
typedef unsigned short  CPU_INT16U;
typedef unsigned char CPU_BOOLEAN;
typedef char CPU_CHAR;
typedef int CPU_SIZE_T;
typedef unsigned char CPU_INT08U;


static inline int ASCII_IsPrint(char c)
{
    if (c >= 32 && c < 127) {
        return 1;
    }
    return 0;
}

/*
*********************************************************************************************************
*                                                 SHELL
*
* Note(s) : (1) Defines the size of the table used to hold the various modules' command tables.  Command
*               tables are added using the Shell_CmdTblAdd() function.  Once the table is full, it is not
*               possible to add any more unless Shell_CmdTblRem() is first called.
*
*           (2) Defines the maximum number or argument(s) a command may pass on the string holding the
*               complete command.  The minimum value is 1.
*
*           (3) Defines the maximum length for module command name, including the NULL character.
*********************************************************************************************************
*/

#define  SHELL_CFG_CMD_TBL_SIZE                            3    /* Cfg Shell cmd tbl size  (see Note #1).               */
#define  SHELL_CFG_CMD_ARG_NBR_MAX                         5    /* Cfg cmd max nbr of arg  (see Note #2).               */

#define  SHELL_CFG_MODULE_CMD_NAME_LEN_MAX                 6    /* Cfg module cmd name len (See Note #3).               */


/*
*********************************************************************************************************
*                                                TRACING
*********************************************************************************************************
*/

#ifndef  TRACE_LEVEL_OFF
#define  TRACE_LEVEL_OFF                                  0u
#endif

#ifndef  TRACE_LEVEL_INFO
#define  TRACE_LEVEL_INFO                                 1u
#endif

#ifndef  TRACE_LEVEL_DBG
#define  TRACE_LEVEL_DBG                                  2u
#endif

#define  SHELL_TRACE_LEVEL                   TRACE_LEVEL_OFF
#define  SHELL_TRACE                                  printf


/*
*********************************************************************************************************
*                                          MODULE END
*********************************************************************************************************
*/

#endif
