#ifndef _MSG_H_
#define _MSG_H_

#include "typedef.h"

#define MAX_POOL	128
enum {
    MSG_NO_ERROR = 0,
    MSG_EVENT_EXIST = -1,
    MSG_NOT_EVENT = -2,
    MSG_EVENT_PARAM_ERROR = -3,
    MSG_BUF_NOT_ENOUGH = -4,
};

#define Q_USER          0x400000
enum {
    LMP_EVENT = Q_USER +  1,
    LMP_HCI_CMD,
    LMP_HCI_CMD_TO_CONN,
    HCI_COMMON_CMD,
    LL_EVENT,
    HCI_CMD_TO_LL,
    HCI_CMD_TO_LL_CONN,
    TWS_LMP_EVENT,

    BTSTACK_HCI_EVENT,
    BTSTACK_HCI_ACL,

    //MSG_BT_UPDATA_START = 0x80,
    //MSG_BT_UPDATE_LOADER_DOWNLOAD_START,
    MSG_JL_SPEECH_START_PREPARE = 0xB00,
    MSG_JL_SPEECH_START,
    MSG_JL_SPEECH_STOP,
    MSG_JL_GET_DEV_INFO,
    MSG_JL_GET_DEV_UPDATE_FILE_INFO_OFFSET,
    MSG_JL_INQUIRE_DEVEICE_IF_CAN_UPDATE,

    MSG_JL_ENTER_UPDATE_MODE,
    MSG_JL_EXIT_UPDATE_MODE,
    MSG_JL_UPDATE_DISCONNECT,
    MSG_JL_UPDATE_REVICE_REBOOT,
    MSG_JL_SWITCH_CH_SPP,

    MSG_JL_SUCCESS_STATUS_HANDLE,
    MSG_JL_ERR_STATUS_HANDLE,

    MSG_JL_GET_PRIVATE_INFO,

    MSG_UBOOT_SOFT_POWEROFF,


};

void task_message_init(void);
int task_post_msg(char *name, int argc, ...);
int task_post_msg_base(const char *name, int argc, int cmd, int *argv);
int task_get_msg(u16 timeout, int len, int *msg);
#endif
