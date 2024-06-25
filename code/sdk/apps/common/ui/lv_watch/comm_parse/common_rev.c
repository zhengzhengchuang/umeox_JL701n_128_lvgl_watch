#include "../lv_watch.h"

/*********************************************************************************
                                  注册用户接收接口                                 
*********************************************************************************/
void umeox_common_le_rev_cb(void *priv, u8 *buf, u8 len)
{
    if(buf == NULL || len == 0)
        return;

    printf("rev_data(%d):", len);
    put_buf(buf, len);

    umeox_common_le_cmd_parse(buf, len);

    return;
}

