#ifndef SPT511X_FIRMWARE_UPDATE
#define SPT511X_FIRMWARE_UPDATE
#include "stdint.h"

#define UPDATE_INFO_EN 1

#if (UPDATE_INFO_EN)
#define update_info(fmt,...)			printf(fmt, ##__VA_ARGS__)
#else
#define update_info(fmt,...)
#endif

typedef enum {
    EXEC_OK = 0,
    CHIP_CODE_ERR,
    UPGRADE_HOLD_ERR,
    UNLOCK_ERR,
    ERASE_OVERTIME_ERR,
    PG_PARAMS_ERR,
    PG_OVERTIME_ERR,
    PG_VERIFY_ERR,
    RD_PARAMS_ERR,
    EEP_RD_PARAMS_ERR,
    EEP_WR_PARAMS_ERR,
    EEP_WR_OVERTIME_ERR,
    CONFIG_VERIFY_ERR,

} FIRMWARE_UPDATE_STATUS;

void spt511x_firmware_update(void);


#endif //SPT511X_FIRMWARE_UPDATE


