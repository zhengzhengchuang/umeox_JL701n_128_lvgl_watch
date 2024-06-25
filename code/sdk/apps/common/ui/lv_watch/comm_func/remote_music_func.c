#include "../lv_watch.h"

RmusicInfoPara_t Rmusic_Info;

static const RmusicInfoPara_t InitInfo = {
    .volume = 8,
    .state = Rmusic_puase,
    .title = {0},
};

void DevReqOpRmusicHandle(Rmusic_cmd_t OpCmd, u8 *para)
{
    DevSendRmusicOpCmd(OpCmd, para);

    return;
}

void RmusicInfoParaRead(void)
{
    int vm_op_len = \
        sizeof(RmusicInfoPara_t);

    memcpy(&Rmusic_Info, &InitInfo, \
            vm_op_len);

    return;
}
