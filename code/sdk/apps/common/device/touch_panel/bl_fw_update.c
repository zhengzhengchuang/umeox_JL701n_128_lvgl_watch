#include "bl_fw_update.h"
#include "bl6133_custom.h"
#include "cpu.h"

#if (BL_UPDATE_FIRMWARE_ENABLE)
/* #define LOG_TAG_CONST       BL6133 */
#define LOG_TAG             "[bl6133_update]"
#include "debug.h"
#define DBG_DIRECT        log_info

void udelay(u32 us);
void mdelay(u32 ms);
void bl_ts_set_intmode(char mode);
void bl_ts_set_intup(char level);
#define   SET_WAKEUP_HIGH    bl_ts_set_intup(1)	 // 需要设置GPIO口
#define   SET_WAKEUP_LOW	 bl_ts_set_intup(0)	 // 需要设置GPIO口
#define   bl_ts_set_irq()    bl_ts_set_intmode(1)
#define   MDELAY(n)	    mdelay(n)

u16 CTP_FLASH_I2C_WRITE(u8 slave_addr, u8 *buf, u16 len);
u16 CTP_FLASH_I2C_READ(u8 slave_addr, u8 *buf, u16 len);

static int bl_i2c_transfer(unsigned char i2c_addr, unsigned char *buf, int len, unsigned char rw)
{
    int ret;

    switch (rw) {
    case I2C_WRITE:
        ret = CTP_FLASH_I2C_WRITE(i2c_addr, buf, len);
        if (ret != len) {
            DBG_DIRECT("bl_i2c_transfer:i2c write error___.len:%d,ret:%d\n", len, ret);
            return -1;
        }

        break;
    case I2C_READ:
        ret = CTP_FLASH_I2C_READ(i2c_addr, buf, len);
        if (ret != len) {
            DBG_DIRECT("bl_i2c_transfer:i2c read error___.len:%d,ret:%d\n", len, ret);
            return -1;
        }

        break;
    }

    return 0;
}

static int bl_read_fw(unsigned char i2c_addr, unsigned char reg_addr, unsigned char *buf, int len)
{
    int ret;

    ret = CTP_FLASH_I2C_WRITE(i2c_addr, &reg_addr, 1);
    if (ret != 1) {
        DBG_DIRECT("bl_read_fw:i2c transfer error___\n");
        return -1;
    }
    ret = CTP_FLASH_I2C_READ(i2c_addr, buf, len);
    if (ret != len) {
        DBG_DIRECT("bl_read_fw:i2c read error___\n");
        return -1;
    }

    return 0;
}

int bl_soft_reset_switch_int_wakemode()
{
    unsigned char cmd[4];
    int ret = 0x00;

    cmd[0] = RW_REGISTER_CMD;
    cmd[1] = ~cmd[0];
    cmd[2] = CHIP_ID_REG;
    cmd[3] = 0xe8;

    ret = bl_i2c_transfer(CTP_SLAVE_ADDR, cmd, 4, I2C_WRITE);
    if (ret < 0) {
        DBG_DIRECT("bl_soft_reset_switch_int_wakemode failed:i2c write flash error___\n");
    }

    MDELAY(50);

    return ret;
}

int bl_get_chip_id(unsigned char *buf)
{

    unsigned char cmd[3];
    int ret = 0x00;

    DBG_DIRECT("bl_get_chip_id\n");

    SET_WAKEUP_LOW;
    MDELAY(50);
    bl_soft_reset_switch_int_wakemode();

    cmd[0] = RW_REGISTER_CMD;
    cmd[1] = ~cmd[0];
    cmd[2] = CHIP_ID_REG;

    ret = bl_i2c_transfer(BL_FLASH_I2C_ADDR, cmd, 3, I2C_WRITE);
    if (ret < 0) {
        DBG_DIRECT("bl_get_chip_id:i2c write flash error___\n");
        goto GET_CHIP_ID_ERROR;
    }

    ret = bl_i2c_transfer(BL_FLASH_I2C_ADDR, buf, 1, I2C_READ);
    if (ret < 0) {
        DBG_DIRECT("bl_get_chip_id:i2c read flash error___\n");
        goto GET_CHIP_ID_ERROR;
    }

GET_CHIP_ID_ERROR:
    SET_WAKEUP_HIGH;
    MDELAY(20);

    return ret;
}

int bl_check_chip_update_mode(void)
{
    unsigned char i = 0;
    unsigned char cmd_buf[3] = {0x0A, 0xF5};
    unsigned char reg_addr[6] = {0x8F, 0x90, 0x91, 0x92, 0x93};
    unsigned char reg_value[6] = {0x00};
    int ret = -1;
    int retry = 3;

    for (i = 0; i < 6; i++) {
        cmd_buf[2] = reg_addr[i];

        while (retry --) {
            ret = bl_i2c_transfer(BL_FLASH_I2C_ADDR, cmd_buf, 3, I2C_WRITE);
            if (ret < 0) {
                DBG_DIRECT("bl_check_chip_update_mode:i2c write flash error___\n");
                MDELAY(20);
            } else {
                break;
            }
        }


        ret = bl_i2c_transfer(BL_FLASH_I2C_ADDR, &reg_value[i], 1, I2C_READ);
        if (ret < 0) {
            DBG_DIRECT("bl_check_chip_update_mode:i2c read flash error___\n");
            return -1;
        }
    }

    return ret;
}

int bl_chip_update_mode(unsigned char bEnter)
{
    unsigned char i = 0;
    unsigned char retry = 5;
    unsigned char cmd_buf[210] = {0x00};
    int ret = -1;

    if (bEnter) {
        //enter update mode
        for (i = 0; i < sizeof(cmd_buf); i += 2) {
            cmd_buf[i] = 0x5A;
            cmd_buf[i + 1] = 0xA5;
        }

        for (i = 0; i < retry; i++) {

            ret = bl_i2c_transfer(BL_FLASH_I2C_ADDR, cmd_buf, sizeof(cmd_buf), I2C_WRITE);
            if (ret < 0) {
                continue;
            }

            ret = bl_check_chip_update_mode();
            if (0 == ret) {
                break;
            }

        }

        MDELAY(20);

    } else {
        //exit update mode
        cmd_buf[0] = 0x5A;
        cmd_buf[1] = 0xA5;

        for (i = 0; i < retry; i++) {
            MDELAY(20);
            ret = bl_i2c_transfer(BL_FLASH_I2C_ADDR, cmd_buf, 2, I2C_WRITE);
            if (0 == ret) {
                break;
            }
        }
    }

    if (i == retry) {
        return -1;
    }

    return 0;
}

int bl_send_power_state(unsigned char power_state)
{
    unsigned char buf[2];

    DBG_DIRECT("bl_send_power_state\n");

    buf[0] = AC_REG;
    buf[1] = power_state;
    return bl_i2c_transfer(CTP_SLAVE_ADDR, buf, 2, I2C_WRITE);
}


int bl_chip_switch_mode(unsigned char mode)
{
    unsigned char buf[2];

    DBG_DIRECT("bl_chip_switch_mode\n");

    buf[0] = LPM_REG;
    buf[1] = mode;
    return bl_i2c_transfer(CTP_SLAVE_ADDR, buf, 2, I2C_WRITE);
}

/* #ifdef BL_UPDATE_FIRMWARE_ENABLE */
static unsigned char bl_check_cobID(unsigned char *fw_data, unsigned char *cobID)
{
    unsigned char convertCobId[12] = {0x00};
    unsigned char i = 0;

    DBG_DIRECT("bl_check_cobID start\n");
    for (i = 0; i < sizeof(convertCobId); i++) {
        if (i % 2) {
            convertCobId[i] = cobID[i / 2] & 0x0f;
        } else {
            convertCobId[i] = (cobID[i / 2] & 0xf0) >> 4;
        }
        DBG_DIRECT("before convert:convertCobId[%d] is %x\n", i, convertCobId[i]);
        if (convertCobId[i] < 10) {
            convertCobId[i] = '0' + convertCobId[i];
        } else {
            convertCobId[i] = 'a' + convertCobId[i] - 10;
        }
        DBG_DIRECT("after convert:convertCobId[%d] is %x\n", i, convertCobId[i]);
    }

    if (memcmp(convertCobId, fw_data + BL_ARGUMENT_BASE_OFFSET + BL_COB_ID_OFFSET, 6)) {
        DBG_DIRECT("check cobID incompatible\n");
        return 0;
    } else {
        DBG_DIRECT("check cobID same\n");
        return 1;
    }
}

static int bl_get_specific_argument(unsigned int *arguOffset, unsigned char *cobID, unsigned char *fw_data, unsigned int fw_size, unsigned char arguCount)
{
    unsigned char convertCobId[12] = {0x00};
    unsigned char binCobID[12] = {0x00};
    unsigned char i = 0;
    unsigned int cobArguAddr = fw_size - arguCount * BL_ARGUMENT_FLASH_SIZE;
    DBG_DIRECT("fw_size is %x\n", fw_size);
    DBG_DIRECT("arguCount is %d\n", arguCount);
    DBG_DIRECT("cobArguAddr is %x\n", cobArguAddr);

    for (i = 0; i < sizeof(convertCobId); i++) {
        if (i % 2) {
            convertCobId[i] = cobID[i / 2] & 0x0f;
        } else {
            convertCobId[i] = (cobID[i / 2] & 0xf0) >> 4;
        }
        DBG_DIRECT("before convert:convertCobId[%d] is %x\n", i, convertCobId[i]);
        if (convertCobId[i] < 10) {
            convertCobId[i] = '0' + convertCobId[i];
        } else {
            convertCobId[i] = 'a' + convertCobId[i] - 10;
        }
        DBG_DIRECT("after convert:convertCobId[%d] is %x\n", i, convertCobId[i]);
    }

    DBG_DIRECT("convertCobId is:\n");
    for (i = 0; i < 12; i++) {
        DBG_DIRECT("%x  ", convertCobId[i]);
    }
    DBG_DIRECT("\n");

    for (i = 0; i < arguCount; i++) {

        /*memcpy((uint8_t *)binCobID, (uint8_t *)(TP_ADDR+2+cobArguAddr + i * BL_ARGUMENT_FLASH_SIZE + BL_COB_ID_OFFSET), 12);
        DBG_DIRECT("binCobID is %x %x %x %x %x %x:\n",binCobID[0],binCobID[1],binCobID[2],binCobID[3],binCobID[4],binCobID[5]);
        if(memcmp(convertCobId, binCobID, 12))
        {
            DBG_DIRECT("This argu is not the specific argu\n");
        }
        else
        {
            *arguOffset = cobArguAddr + i * BL_ARGUMENT_FLASH_SIZE;
            DBG_DIRECT("This argu is the specific argu, and arguOffset is %x\n",*arguOffset);
            break;
        }*/
        if (memcmp(convertCobId, fw_data + cobArguAddr + i * BL_ARGUMENT_FLASH_SIZE + BL_COB_ID_OFFSET, 12)) {
            DBG_DIRECT("This argu is not the specific argu\n");
        } else {
            *arguOffset = cobArguAddr + i * BL_ARGUMENT_FLASH_SIZE;
            DBG_DIRECT("This argu is the specific argu, and arguOffset is %x\n", *arguOffset);
            break;
        }
    }

    if (i == arguCount) {
        return -1;
    } else {
        return 0;
    }
}

static unsigned char bl_get_argument_count(unsigned char *fw_data, unsigned int fw_size)
{
    unsigned char i = 0;
    unsigned addr = 0;
    unsigned char arguMark[16] = {0x00};
    addr = fw_size;
    DBG_DIRECT("addr is %x\n", addr);
    while (addr > (BL_ARGUMENT_BASE_OFFSET + BL_ARGUMENT_FLASH_SIZE)) {
        addr = addr - BL_ARGUMENT_FLASH_SIZE;
        if (memcmp(fw_data + addr, ARGU_MARK, sizeof(ARGU_MARK) - 1)) {
            DBG_DIRECT("arguMark found flow complete");
            break;
        } else {
            i++;
            DBG_DIRECT("arguMark founded\n");
        }
        /*memcpy((uint8_t *)arguMark, (uint8_t *)(TP_ADDR+addr+2), sizeof(arguMark));
        DBG_DIRECT("arguMark is %s\n", arguMark);
        if(memcmp(arguMark, ARGU_MARK, sizeof(ARGU_MARK) - 1))
        {
            DBG_DIRECT("arguMark found flow complete");
            break;
        }
        else
        {
            i++;
            DBG_DIRECT("arguMark founded\n");
        }*/
    }
    DBG_DIRECT("The argument count is %d\n", i);
    return i;
}

static unsigned int bl_get_cob_project_down_size_arguCnt(unsigned char *fw_data, unsigned int fw_size, unsigned char *arguCnt)
{
    unsigned int downSize = 0;

    *arguCnt = bl_get_argument_count(fw_data, fw_size);
    downSize = fw_size - (*arguCnt) * BL_ARGUMENT_FLASH_SIZE - FLASH_PAGE_SIZE;
    return downSize;
}

static unsigned char bl_is_cob_project(unsigned char *fw_data, int fw_size)
{
    unsigned char arguKey[4] = {0xaa, 0x55, 0x09, 0x09};
    unsigned char *pfw;


//	memcpy((uint8_t *)pfw, (uint8_t *)(TP_ADDR+fw_size-2), sizeof(pfw));
//	DBG_DIRECT("pfw = %x %x %x %x\n",pfw[0],pfw[1],pfw[2],pfw[3]);
    pfw = fw_data + fw_size - 4;
    if (fw_size % FLASH_PAGE_SIZE) {
        return 0;
    } else {
        if (memcmp(arguKey, pfw, 4)) {
            return 0;
        } else {
            return 1;
        }
    }
}
static int bl_read_flash(unsigned char cmd, int flash_start_addr, unsigned char *buf, int len)
{
    char ret = 0;
    unsigned char cmd_buf[6];
    unsigned short flash_end_addr;

    flash_end_addr = flash_start_addr + len - 1;
    cmd_buf[0] = cmd;
    cmd_buf[1] = ~cmd;
    cmd_buf[2] = flash_start_addr >> 0x08;
    cmd_buf[3] = flash_start_addr & 0xff;
    cmd_buf[4] = flash_end_addr >> 0x08;
    cmd_buf[5] = flash_end_addr & 0xff;
    ret = bl_i2c_transfer(BL_FLASH_I2C_ADDR, cmd_buf, 6, I2C_WRITE);
    if (ret < 0) {
        DBG_DIRECT("bl_read_flash:i2c transfer write error\n");
        return -1;
    }
    ret = bl_i2c_transfer(BL_FLASH_I2C_ADDR, buf, len, I2C_READ);
    if (ret < 0) {
        DBG_DIRECT("bl_read_flash:i2c transfer read error\n");
        return -1;
    }

    return 0;
}

static int bl_get_flash_cob_id(unsigned char *buf)
{
    char ret = 0;
    DBG_DIRECT("bl_get_flash_cob_id\n");

    SET_WAKEUP_LOW;
    MDELAY(50);
    bl_soft_reset_switch_int_wakemode();
    ret = bl_read_flash(READ_MAIN_CMD, BL_ARGUMENT_BASE_OFFSET + BL_COB_ID_OFFSET, buf, BL_COB_ID_LEN);
    if (ret < 0) {
        DBG_DIRECT("bl_write_flash_vertify: read fail\n");
        ret = -1;
    }

    SET_WAKEUP_HIGH;
    MDELAY(60);
    return ret;
}

static int bl_get_cob_id(unsigned char *buf)
{
    DBG_DIRECT("bl_get_cob_id\n");
    return bl_read_fw(CTP_SLAVE_ADDR, COB_ID_REG, buf, 6);
}

static int bl_get_fwArgPrj_id(unsigned char *buf)
{
    DBG_DIRECT("bl_get_fwArgPrj_id\n");
    return bl_read_fw(CTP_SLAVE_ADDR, BL_FWVER_PJ_ID_REG, buf, 3);
}

static int bl_get_prj_id(unsigned char *buf)
{
    DBG_DIRECT("bl_get_prj_id\n");
    return bl_read_fw(CTP_SLAVE_ADDR, BL_PRJ_ID_REG, buf, 1);
}

static int bl_check_prj_info(unsigned char *fw_data, unsigned char prjType, int specifyArgAddr)
{
    char ret = 0;
    unsigned char prj_info[PRJ_INFO_LEN] = {0};

    ret = bl_read_fw(CTP_SLAVE_ADDR, BL_PRJ_INFO_REG, prj_info, PRJ_INFO_LEN);
    if (ret < 0) {
        DBG_DIRECT("Read project info error\n");
        return -1;
    }

    if (!prjType) {
        ret = memcmp(fw_data + specifyArgAddr + BL_PROJECT_INFO_OFFSET, prj_info, PRJ_INFO_LEN);
        if (ret == 0) {
            DBG_DIRECT("Check project info success\n");
            return 0;
        } else {
            DBG_DIRECT("Check project info fail\n");
            return -1;
        }
    } else {
        DBG_DIRECT("COB project do not check\n");
        return 0;
    }
}

static int bl_get_fw_checksum(unsigned short *fw_checksum)
{
    unsigned char buf[3];
    unsigned char checksum_ready = 0;
    int retry = 5;
    int ret = 0x00;

    DBG_DIRECT("bl_get_fw_checksum\n");

    buf[0] = CHECKSUM_CAL_REG;
    buf[1] = CHECKSUM_CAL;
    ret = bl_i2c_transfer(CTP_SLAVE_ADDR, buf, 2, I2C_WRITE);
    if (ret < 0) {
        DBG_DIRECT("bl_get_fw_checksum:write checksum cmd error___\n");
        return -1;
    }
    MDELAY(FW_CHECKSUM_DELAY_TIME);

    ret = bl_read_fw(CTP_SLAVE_ADDR, CHECKSUM_REG, buf, 3);
    if (ret < 0) {
        DBG_DIRECT("bl_get_fw_checksum:read checksum error___\n");
        return -1;
    }

    checksum_ready = buf[0];

    while ((retry--) && (checksum_ready != CHECKSUM_READY)) {

        MDELAY(50);
        ret = bl_read_fw(CTP_SLAVE_ADDR, CHECKSUM_REG, buf, 3);
        if (ret < 0) {
            DBG_DIRECT("bl_get_fw_checksum:read checksum error___\n");
            return -1;
        }

        checksum_ready = buf[0];
    }

    if (checksum_ready != CHECKSUM_READY) {
        DBG_DIRECT("bl_get_fw_checksum:read checksum fail___\n");
        return -1;
    }
    *fw_checksum = (buf[1] << 8) + buf[2];

    return 0;
}

static void bl_get_fw_bin_checksum(unsigned char *fw_data, unsigned short *fw_bin_checksum, unsigned char firmware_file_type, int fw_size, int specifyArgAddr)
{
    int i = 0, j = 0;
    int temp_checksum = 0x0;
    int len = 0x00;
    unsigned char disp_buff_8row[512] = {0x00};

    switch (firmware_file_type) {
    case HEADER_FILE_UPDATE:
    //*fw_bin_checksum = checksum_arr[0];
    //break;

    case BIN_FILE_UPDATE:
        /*for(i = 0; i < BL_ARGUMENT_BASE_OFFSET;)
        {
            if(BL_ARGUMENT_BASE_OFFSET - i > FLASH_BIN_READ_SIZE)
        		len = FLASH_BIN_READ_SIZE;
        	else
        		len = BL_ARGUMENT_BASE_OFFSET - i;

        	memcpy((uint8_t *)disp_buff_8row, (uint8_t *)(TP_ADDR+2+i), len);
        	for(j = 0; j < len; j++)
        	{
                temp_checksum += disp_buff_8row[j];
        	}
        	i += len;
        }
        for(i = specifyArgAddr; i < specifyArgAddr + BL_ARGUMENT_FLASH_SIZE;)
        {
            if(specifyArgAddr + BL_ARGUMENT_FLASH_SIZE - i > FLASH_BIN_READ_SIZE)
        		len = FLASH_BIN_READ_SIZE;
        	else
        		len = specifyArgAddr + BL_ARGUMENT_FLASH_SIZE - i;

        	memcpy((uint8_t *)disp_buff_8row, (uint8_t *)(TP_ADDR+2+i), len);
        	for(j = 0; j < len; j++)
        	{
                temp_checksum += disp_buff_8row[j];
        	}
        	i += len;
        }
        for(i = BL_ARGUMENT_BASE_OFFSET + BL_ARGUMENT_FLASH_SIZE; i < fw_size;)
        {
            if(fw_size - i > FLASH_BIN_READ_SIZE)
        		len = FLASH_BIN_READ_SIZE;
        	else
        		len = fw_size - i;

        	memcpy((uint8_t *)disp_buff_8row, (uint8_t *)(TP_ADDR+2+i), len);
        	for(j = 0; j < len; j++)
        	{
                temp_checksum += disp_buff_8row[j];
        	}
        	i += len;
        }
        for(i = fw_size; i < MAX_FLASH_SIZE; i++)
        {
        	temp_checksum += 0xff;
        }

        *fw_bin_checksum = temp_checksum & 0xffff;*/
        for (i = 0; i < BL_ARGUMENT_BASE_OFFSET; i++) {
            temp_checksum += fw_data[i];
        }
        for (i = specifyArgAddr; i < specifyArgAddr + BL_ARGUMENT_FLASH_SIZE; i++) {
            temp_checksum += fw_data[i];
        }
        for (i = BL_ARGUMENT_BASE_OFFSET + BL_ARGUMENT_FLASH_SIZE; i < fw_size; i++) {
            temp_checksum += fw_data[i];
        }
        for (i = fw_size; i < MAX_FLASH_SIZE; i++) {
            temp_checksum += 0xff;
        }

        *fw_bin_checksum = temp_checksum & 0xffff;
        break;
    }
}

static int bl_erase_flash(void)
{
    unsigned char cmd[2];

    DBG_DIRECT("bl_erase_flash\n");

    cmd[0] = ERASE_ALL_MAIN_CMD;
    cmd[1] = ~cmd[0];

    return bl_i2c_transfer(BL_FLASH_I2C_ADDR, cmd, 0x02, I2C_WRITE);
}

static int bl_write_flash(unsigned char cmd, int flash_start_addr, unsigned char *buf, int len)
{
    unsigned char cmd_buf[6 + FLASH_WSIZE];
    unsigned short flash_end_addr;
    int ret;

    DBG_DIRECT("bl_write_flash\n");

    if (!len) {
        DBG_DIRECT("___write flash len is 0x00,return___\n");
        return -1;
    }

    flash_end_addr = flash_start_addr + len - 1;

    if (flash_end_addr >= MAX_FLASH_SIZE) {
        DBG_DIRECT("___write flash end addr is overflow,return___\n");
        return -1;
    }

    cmd_buf[0] = cmd;
    cmd_buf[1] = ~cmd;
    cmd_buf[2] = flash_start_addr >> 0x08;
    cmd_buf[3] = flash_start_addr & 0xff;
    cmd_buf[4] = flash_end_addr >> 0x08;
    cmd_buf[5] = flash_end_addr & 0xff;

    memcpy(&cmd_buf[6], buf, len);

    ret = bl_i2c_transfer(BL_FLASH_I2C_ADDR, cmd_buf, len + 6, I2C_WRITE);
    if (ret < 0) {
        DBG_DIRECT("i2c transfer error___\n");
        return -1;
    }

    return 0;
}


static int bl_download_fw(unsigned char *pfwbin, int fwsize)
{
    unsigned int i;
    unsigned short size, len;
    unsigned short addr;
    //unsigned char disp_buff_8row[512] = {0x00};
    unsigned char verifyBuf[4] = {0xff, 0xff, 0x09, 0x09};
    DBG_DIRECT("bl_download_fw\n");
    if (bl_erase_flash()) {
        DBG_DIRECT("___erase flash fail___\n");
        return -1;
    }

    MDELAY(50);
#if 0
    for (i = 0; i < fwsize;) {
        size = fwsize - i;
        if (size > FLASH_WSIZE) {
            len = FLASH_WSIZE;
        } else {
            len = size;
        }

        addr = i;

        if (bl_write_flash(WRITE_MAIN_CMD, addr, &pfwbin[i], len)) {
            return -1;
        }
        i += len;
        MDELAY(5);
    }
#endif
#if 1//Write data before verifyBuf
    for (i = 0; i < (BL_ARGUMENT_BASE_OFFSET + BL_ARGUMENT_FLASH_SIZE - (sizeof(verifyBuf))) ;) {
        size = BL_ARGUMENT_BASE_OFFSET + BL_ARGUMENT_FLASH_SIZE - sizeof(verifyBuf) - i;
        if (size > FLASH_WSIZE) {
            len = FLASH_WSIZE;
        } else {
            len = size;
        }

        addr = i;


        // memcpy((uint8_t *)disp_buff_8row, (uint8_t *)(TP_ADDR+2+i), len);
        //if(bl_write_flash(WRITE_MAIN_CMD,addr, disp_buff_8row,len)){
        if (bl_write_flash(WRITE_MAIN_CMD, addr, &pfwbin[i], len)) {
            return -1;
        }
        i += len;
        MDELAY(5);
    }
#endif

#if 1//Write the four bytes verifyBuf from VERTIFY_START_OFFSET
    addr = i;
    if (bl_write_flash(WRITE_MAIN_CMD, addr, verifyBuf, sizeof(verifyBuf))) {
        return -1;
    }
    MDELAY(5);
#endif

#if 1//Write data after verityBuf from VERTIFY_START_OFFSET + 4
    for (i = (BL_ARGUMENT_BASE_OFFSET + BL_ARGUMENT_FLASH_SIZE); i < fwsize;) {
        size = fwsize - i;
        if (size > FLASH_WSIZE) {
            len = FLASH_WSIZE;
        } else {
            len = size;
        }

        addr = i;

        if (bl_write_flash(WRITE_MAIN_CMD, addr, &pfwbin[i], len)) {
            //memcpy((uint8_t *)disp_buff_8row, (uint8_t *)(TP_ADDR+2+i), len);
            //if(bl_write_flash(WRITE_MAIN_CMD,addr, disp_buff_8row,len)){
            return -1;
        }
        i += len;
        MDELAY(5);
    }

#endif
    return 0;
}

static int bl_get_lk_checksum(unsigned short *lk_checksum)
{
    unsigned char buf[3];
    unsigned char checksum_ready = 0;
    int retry = 5;
    int ret = 0x00;

    DBG_DIRECT("bl_get_lk_checksum\n");

    buf[0] = CHECKSUM_CAL_REG;
    buf[1] = CHECKSUM_ARG;
    ret = bl_i2c_transfer(CTP_SLAVE_ADDR, buf, 2, I2C_WRITE);
    if (ret < 0) {
        DBG_DIRECT("bl_get_lk_checksum:write checksum cmd error___\n");
        return -1;
    }
    MDELAY(FW_CHECKSUM_DELAY_TIME);

    ret = bl_read_fw(CTP_SLAVE_ADDR, CHECKSUM_REG, buf, 3);
    if (ret < 0) {
        DBG_DIRECT("bl_get_lk_checksum:read checksum error___\n");
        return -1;
    }

    checksum_ready = buf[0];

    while ((retry--) && (checksum_ready != CHECKSUM_READY)) {

        MDELAY(50);
        ret = bl_read_fw(CTP_SLAVE_ADDR, CHECKSUM_REG, buf, 3);
        if (ret < 0) {
            DBG_DIRECT("bl_get_lk_checksum:read checksum error___\n");
            return -1;
        }

        checksum_ready = buf[0];
    }

    if (checksum_ready != CHECKSUM_READY) {
        DBG_DIRECT("bl_get_lk_checksum:read checksum fail___\n");
        return -1;
    }
    *lk_checksum = (buf[1] << 8) + buf[2];

    return 0;
}

static void bl_get_lk_bin_checksum(unsigned char *fw_data, unsigned short *lk_bin_checksum, unsigned char firmware_file_type, int fwsize, int specificArgAddr)
{
    int i = 0, j = 0;;
    int len = 0;
    int temp_checksum = 0x0;
    unsigned char disp_buff_8row[512] = {0x00};

    DBG_DIRECT("bl_get_lk_bin_checksum:specificArgAddr = %x\n", specificArgAddr);
    switch (firmware_file_type) {
    case HEADER_FILE_UPDATE:
    //*lk_bin_checksum = checksum_1k_arr[0];
    //break;

    case BIN_FILE_UPDATE:
//		    for(i = specificArgAddr; i < (specificArgAddr + BL_ARGUMENT_FLASH_SIZE);)
//			{
//			    if(specificArgAddr + BL_ARGUMENT_FLASH_SIZE - i > FLASH_BIN_READ_SIZE)
//					len = 512;
//				else
//					len = specificArgAddr + BL_ARGUMENT_FLASH_SIZE - i;

//				memcpy((uint8_t *)disp_buff_8row, (uint8_t *)(TP_ADDR+2+i), len);
//				for(j = 0; j < len; j++)
//				{
//                    temp_checksum += disp_buff_8row[j];
//				}
//				i += len;
//			}
        for (i = specificArgAddr; i < (specificArgAddr + BL_ARGUMENT_FLASH_SIZE); i++) {
            temp_checksum += fw_data[i];
        }
        *lk_bin_checksum = temp_checksum & 0xffff;
        break;
    }
}

static int bl_erase_flash_page(unsigned char cmd, unsigned char start_page, unsigned char end_page)
{
    unsigned char cmd_buf[4];

    DBG_DIRECT("bl_erase_flash_page\n");

    cmd_buf[0] = cmd;
    cmd_buf[1] = ~cmd;
    cmd_buf[2] = start_page;
    cmd_buf[3] = end_page;

    return bl_i2c_transfer(BL_FLASH_I2C_ADDR, cmd_buf, 0x04, I2C_WRITE);
}

static int bl_download_argument(unsigned char *pfwbin, int specificArgAddr, int argsize)
{
    unsigned int i;
    unsigned short size, len;
    unsigned short addr;
    unsigned char disp_buff_8row[512] = {0x00};
    unsigned char verifyBuf[4] = {0xff, 0xff, 0x09, 0x09};

    DBG_DIRECT("bl_download_argument:specificArgAddr = %x\n", specificArgAddr);
    if (bl_erase_flash_page(ERASE_SECTOR_MAIN_CMD, 2, 2)) {
        DBG_DIRECT("___erase flash fail___\n");
        return -1;
    }

    MDELAY(50);

    if (bl_erase_flash_page(ERASE_SECTOR_MAIN_CMD, 1, 1)) {
        DBG_DIRECT("___erase flash fail___\n");
        return -1;
    }
    MDELAY(50);
#if 0
    for (i = 0; i < argsize;) {
        size = argsize - i;
        if (size > FLASH_WSIZE) {
            len = FLASH_WSIZE;
        } else {
            len = size;
        }


        addr = i + BL_ARGUMENT_BASE_OFFSET;

        if (bl_write_flash(WRITE_MAIN_CMD, addr, &pfwbin[i + specificArgAddr], len)) {
            return -1;
        }
        i += len;
        MDELAY(5);
    }
#endif

#if 1//Write the data before VERTIFY_START_OFFSET
    for (i = 0; i < (argsize - sizeof(verifyBuf));) {
        size = argsize - sizeof(verifyBuf) - i;
        if (size > FLASH_WSIZE) {
            len = FLASH_WSIZE;
        } else {
            len = size;
        }


        addr = i + BL_ARGUMENT_BASE_OFFSET;

//		    memcpy((uint8_t *)disp_buff_8row, (uint8_t *)(TP_ADDR+2+i+specificArgAddr), len);
//			if(bl_write_flash(WRITE_MAIN_CMD,addr, disp_buff_8row,len)){
        if (bl_write_flash(WRITE_MAIN_CMD, addr, &pfwbin[i + specificArgAddr], len)) {
            return -1;
        }
        i += len;
        MDELAY(5);
    }
#endif

#if 1//Write the four bytes verifyBuf from VERTIFY_START_OFFSET
    addr = i + BL_ARGUMENT_BASE_OFFSET;
    if (bl_write_flash(WRITE_MAIN_CMD, addr, verifyBuf, sizeof(verifyBuf))) {
        return -1;
    }
    MDELAY(5);
#endif

    return 0;
}

static int bl_read_flash_vertify(void)
{
    unsigned char cnt = 0;
    int ret = 0;
    unsigned char vertify[2] = {0xAA, 0x55};
    unsigned char vertify1[2] = {0};
    SET_WAKEUP_LOW;
    MDELAY(50);
    bl_soft_reset_switch_int_wakemode();
    while (cnt < 3) {
        ret = bl_read_flash(READ_MAIN_CMD, BL_ARGUMENT_BASE_OFFSET + VERTIFY_START_OFFSET, vertify1, 2);
        if (ret < 0) {
            DBG_DIRECT("bl_write_flash_vertify: read fail\n");
            continue;
        }

        if (memcmp(vertify, vertify1, 2) == 0) {
            ret = 0;
            break;
        } else {
            ret = -1;
        }
        cnt++;
    }
    SET_WAKEUP_HIGH;
    MDELAY(20);
    return ret;
}

static int bl_write_flash_vertify(void)
{
    unsigned char cnt = 0;
    int ret = 0;
    unsigned char vertify[2] = {0xAA, 0x55};
    unsigned char vertify1[2] = {0};
    SET_WAKEUP_LOW;
    MDELAY(50);
    bl_soft_reset_switch_int_wakemode();
    while (cnt < 3) {
        ret = bl_write_flash(WRITE_MAIN_CMD, BL_ARGUMENT_BASE_OFFSET + VERTIFY_START_OFFSET, vertify, 2);
        if (ret < 0) {
            DBG_DIRECT("bl_write_flash_vertify: write fail\n");
            continue;
        }

        MDELAY(10);

        ret = bl_read_flash(READ_MAIN_CMD, BL_ARGUMENT_BASE_OFFSET + VERTIFY_START_OFFSET, vertify1, 2);
        if (ret < 0) {
            DBG_DIRECT("bl_write_flash_vertify: read fail\n");
            continue;
        }

        if (memcmp(vertify, vertify1, 2) == 0) {
            ret = 0;
            break;
        } else {
            ret = -1;
        }
        cnt++;
    }
    SET_WAKEUP_HIGH;
    MDELAY(20);
    return ret;
}

static int bl_update_flash(unsigned char update_type, unsigned char firmware_file_type, unsigned char *pfwbin, int fwsize, int specificArgAddr)
{
    int retry = 0;
    int ret = 0;
    unsigned short fw_checksum = 0x0;
    unsigned short fw_bin_checksum = 0x0;
    unsigned short lk_checksum = 0x0;
    unsigned short lk_bin_checksum = 0x0;
    retry = 3;
    while (retry--) {
        SET_WAKEUP_LOW;
        MDELAY(50);
        bl_soft_reset_switch_int_wakemode();
        switch (update_type) {
        case ARG_UPDATE:
            ret = bl_download_argument(pfwbin, specificArgAddr, BL_ARGUMENT_FLASH_SIZE);
            break;
        case FW_ARG_UPDATE:
            ret = bl_download_fw(pfwbin, fwsize);
            break;
        }
        if (ret < 0) {
            DBG_DIRECT("btl fw update start bl_download_fw error retry=%d\n", retry);
            continue;
        }

        MDELAY(50);

        SET_WAKEUP_HIGH;

        MDELAY(200);

        switch (update_type) {
        case ARG_UPDATE:
            bl_get_lk_bin_checksum(pfwbin, &lk_bin_checksum, firmware_file_type, fwsize, specificArgAddr);
            ret = bl_get_lk_checksum(&lk_checksum);
            lk_checksum -= 0xff;
            DBG_DIRECT("btl fw update ARG_UPDATE,lk checksum = 0x%x,lk_bin_checksum =0x%x\n", lk_checksum, lk_bin_checksum);
            break;
        case FW_ARG_UPDATE:
            bl_get_fw_bin_checksum(pfwbin, &fw_bin_checksum, firmware_file_type, fwsize, specificArgAddr);
            ret = bl_get_fw_checksum(&fw_checksum);
            fw_checksum -= 0xff;
            DBG_DIRECT("btl fw update end,fw checksum = 0x%x,fw_bin_checksum =0x%x\n", fw_checksum, fw_bin_checksum);
            break;
        }

        if ((ret < 0) || ((update_type == ARG_UPDATE) && (lk_checksum != lk_bin_checksum)) || ((update_type == FW_ARG_UPDATE) && (fw_checksum != fw_bin_checksum))) {
            DBG_DIRECT("btl fw update start bl_download_fw bl_get_fw_checksum error");
            continue;
        }

        if (((lk_checksum == lk_bin_checksum) && (update_type == ARG_UPDATE)) || ((update_type == FW_ARG_UPDATE) && (fw_checksum == fw_bin_checksum))) {
            ret = bl_write_flash_vertify();
            if (ret < 0) {
                continue;
            }
        }
        break;
    }

    if (retry < 0) {
        DBG_DIRECT("btl fw update error\n");
        return -1;
    }

    DBG_DIRECT("btl fw update success___\n");

    return 0;
}

static unsigned char choose_update_type_for_self_ctp(unsigned char isBlank, unsigned char touchSwitch, unsigned char *fw_data, unsigned char fwVer, unsigned char arguVer, unsigned short fwChecksum, unsigned short fwBinChecksum, int specifyArgAddr, unsigned char binFwVer, unsigned char binArgVer)
{
    unsigned char update_type = NONE_UPDATE;
    if (isBlank) {
        update_type = FW_ARG_UPDATE;
        DBG_DIRECT("Update case 0:FW_ARG_UPDATE\n");
    } else {
        if (fwVer < binFwVer) {
            update_type = FW_ARG_UPDATE;
            DBG_DIRECT("Update case 1:FW_ARG_UPDATE\n");
        } else {
            if (touchSwitch) {
                update_type = ARG_UPDATE;
                DBG_DIRECT("Update case 3:ARG_UPDATE\n");
            } else {
                if (fwVer == binFwVer) {
                    if (arguVer < binArgVer) {
                        update_type = ARG_UPDATE;
                        DBG_DIRECT("Update case 2:ARG_UPDATE\n");
                    } else {
                        if (arguVer == binArgVer) {
                            if (fwChecksum != fwBinChecksum) {
                                update_type = FW_ARG_UPDATE;
                                DBG_DIRECT("Update case 3:FW_ARG_UPDATE\n");
                            } else {
                                update_type = NONE_UPDATE;
                                DBG_DIRECT("Update case 4:NONE_UPDATE\n");
                            }
                        } else {
                            update_type = NONE_UPDATE;
                            DBG_DIRECT("Update case 5:NONE_UPDATE\n");
                        }
                    }
                } else {
                    update_type = NONE_UPDATE;
                    DBG_DIRECT("Update case 6:NONE_UPDATE\n");
                }
            }
        }
    }
    return update_type;
}
#if 0
static unsigned char choose_update_type_for_compatible_ctp(unsigned isBlank, unsigned char touchSwitch, unsigned char *fw_data, unsigned char prjID, unsigned short fwChecksum, unsigned fwBinChecksum, int specifyArgAddr)
{
    unsigned char update_type = NONE_UPDATE;
    if (isBlank || touchSwitch) {
        update_type = FW_ARG_UPDATE;
        DBG_DIRECT("Update case 0:FW_ARG_UPDATE\n");
    } else {
        if (prjID < fw_data[specifyArgAddr + PJ_ID_OFFSET]) {
            update_type = ARG_UPDATE;
            DBG_DIRECT("Update case 1:FW_ARG_UPDATE\n");
        } else {
            if (prjID == fw_data[specifyArgAddr + PJ_ID_OFFSET]) {
                if (fwChecksum != fwBinChecksum) {
                    update_type = FW_ARG_UPDATE;
                    DBG_DIRECT("Update case 3:FW_ARG_UPDATE\n");
                } else {
                    update_type = NONE_UPDATE;
                    DBG_DIRECT("Update case 4:NONE_UPDATE\n");
                }
            } else {
                update_type = NONE_UPDATE;
                DBG_DIRECT("Update case 5:NONE_UPDATE\n");
            }
        }
    }
    return update_type;
}
#endif
static int bl_update_fw_for_self_ctp(unsigned char firmware_file_type, unsigned char *fw_data, int fw_size)
{
    unsigned char fwArgPrjID[3];    //firmware version/argument version/project identification
    unsigned char chip_id = 0x00;   //The IC identification
    int ret = 0x00;
    unsigned char isBlank = 0x0;    //Indicate the IC have any firmware
    unsigned short fw_checksum = 0x0;  //The checksum for firmware in IC
    unsigned short fw_bin_checksum = 0x0;  //The checksum for firmware in file
    unsigned char update_type = NONE_UPDATE;
    unsigned int downSize = 0x0;       //The available size of firmware data in file
    unsigned char cobID[6] = {0};           //The identification for COB project
    unsigned char fwCobID[12] = {0};         //The identification saved in touch controller IC
    unsigned int specificArguAddr = BL_ARGUMENT_BASE_OFFSET;   //The specific argument base address in firmware date with cobID
    unsigned char arguCount = 0x0;      //The argument count for COB firmware
    unsigned char checkCobID = 0x0;     //Specify whether the cobID is same or not with firmware data and hardware
    unsigned char IsCobPrj = 0;         //Judge the project type depend firmware file
    unsigned char touchSwitch = 0;      //Judge wether switch touch
    unsigned char specifyCobID[12] = {0x00};//The choose screen sensor id
    unsigned char binFwArgPrjID[3] = {0x00};
    DBG_DIRECT("bl_update_fw_for_self_ctp start\n");

//Check chipID
#ifdef BTL_CHECK_CHIPID
    ret = bl_get_chip_id(&chip_id);

    if (ret < 0 || chip_id != BTL_FLASH_ID) {
        DBG_DIRECT("bl_update_fw_for_self_ctp:chip_id = %d", chip_id);
        return -1;
    }
    DBG_DIRECT("bl_update_fw_for_self_ctp:chip_id = %x", chip_id);
#endif

//Step 1:Obtain project type
    IsCobPrj = bl_is_cob_project(fw_data, fw_size);
    DBG_DIRECT("bl_update_fw_for_self_ctp:IsCobPrj = %x", IsCobPrj);

//Step 2:Obtain IC version number
    MDELAY(5);
    ret = bl_get_fwArgPrj_id(fwArgPrjID);
    if ((ret < 0)
        || ((ret == 0) && (fwArgPrjID[0] == 0))
        || ((ret == 0) && (fwArgPrjID[0] == 0xff))
        || ((ret == 0) && (fwArgPrjID[0] == BL_FWVER_PJ_ID_REG))
        || (bl_read_flash_vertify() < 0)) {
        isBlank = 1;
        DBG_DIRECT("bl_update_fw_for_self_ctp:This is blank IC ret = %x fwArgPrjID[0]=%x\n", ret, fwArgPrjID[0]);
    } else {
        isBlank = 0;
        DBG_DIRECT("bl_update_fw_for_self_ctp:ret=%d fwID=%d argID=%d prjID=%d\n", ret, fwArgPrjID[0], fwArgPrjID[1], fwArgPrjID[2]);
    }
    DBG_DIRECT("bl_update_fw_for_self_ctp:isBlank = %x\n", isBlank);
#if BL_FORCE_UPDATE_FIRMWARE
    isBlank = 1;//Force update firmware
    DBG_DIRECT(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    DBG_DIRECT("          Force update firmware:isBlank = %x", isBlank);
    DBG_DIRECT("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
#endif // BL_Force_UPDATE_FARMWARE
//Step 3:Specify download size
    if (IsCobPrj) {
        downSize = bl_get_cob_project_down_size_arguCnt(fw_data, fw_size, &arguCount);
        DBG_DIRECT("bl_update_fw_for_self_ctp:downSize = %x,arguCount = %x\n", downSize, arguCount);
    } else {
        downSize = fw_size;
        DBG_DIRECT("bl_update_fw_for_self_ctp:downSize = %x\n", downSize);
    }

UPDATE_SECOND_FOR_COB:
//Step 4:Update the fwArgPrjID
    if (!isBlank) {
        bl_get_fwArgPrj_id(fwArgPrjID);
    }

//Step 5:Specify the argument data for cob project
    if (IsCobPrj && !isBlank) {
        MDELAY(50);
        ret = bl_get_cob_id(cobID);
        if (ret < 0) {
            DBG_DIRECT("bl_update_fw_for_self_ctp:bl_get_cob_id error\n");
            ret = -1;
            goto UPDATE_ERROR;
        } else {
            DBG_DIRECT("bl_update_fw_for_self_ctp:cobID = %x %x %x %x %x %x\n", cobID[0], cobID[1], cobID[2], cobID[3], cobID[4], cobID[5]);
        }
        ret = bl_get_specific_argument(&specificArguAddr, cobID, fw_data, fw_size, arguCount);
        if (ret < 0) {
            DBG_DIRECT("Can't found argument for CTP module\n");
            goto UPDATE_ERROR;
        }
        DBG_DIRECT("bl_update_fw_for_self_ctp:specificArguAddr = %x\n", specificArguAddr);
    }

    DBG_DIRECT("fw_data[] = %x  fw_data[] = %x", fw_data[BL_ARGUMENT_BASE_OFFSET + VERTIFY_START_OFFSET], fw_data[BL_ARGUMENT_BASE_OFFSET + VERTIFY_END_OFFSET]);

//Step 6:Specify whether switch the touch
    if (IsCobPrj && !isBlank) {
        bl_get_flash_cob_id(fwCobID);
        DBG_DIRECT("bl_update_fw_for_self_ctp:fwCobID = %x %x %x %x %x %x %x %x %x %x %x %x\n", fwCobID[0], fwCobID[1], fwCobID[2], fwCobID[3], fwCobID[4], fwCobID[5], fwCobID[6], fwCobID[7], fwCobID[8], fwCobID[9], fwCobID[10], fwCobID[11]);

//		memcpy((uint8_t *)specifyCobID, (uint8_t *)(TP_ADDR+2+specificArguAddr + BL_COB_ID_OFFSET), 12);
//		ret = memcmp(specifyCobID, fwCobID, BL_COB_ID_LEN);
        ret = memcmp(fw_data + specificArguAddr + BL_COB_ID_OFFSET, fwCobID, BL_COB_ID_LEN);
        if (ret) {
            touchSwitch = 1;
            DBG_DIRECT("touchSwitch = %d", touchSwitch);
        }
    }

//Step 7:Obtain project infomation
#if 0
    if (!isBlank) {
        ret = bl_check_prj_info(fw_data, IsCobPrj, specificArguAddr);
        if (ret < 0) {
            DBG_DIRECT("bl_update_fw_for_self_ctp:check project info error\n");
            ret = -1;
            goto UPDATE_ERROR;
        }
    }
#endif
//Step 8:Obtain IC firmware checksum when the version number is same between IC and host firmware

//	memcpy((uint8_t *)binFwArgPrjID, (uint8_t *)(TP_ADDR+2+specificArguAddr + BL_FWVER_MAIN_OFFSET), 3);
    DBG_DIRECT("isBlank = %d  ver1 = %d ver2 = %d binVer1 = %d binVer2 = %d specificAddr = %x\n", isBlank, fwArgPrjID[0], fwArgPrjID[1], binFwArgPrjID[0], binFwArgPrjID[1], specificArguAddr);
    //if((!isBlank) && (fwArgPrjID[0]==binFwArgPrjID[0]) && (fwArgPrjID[1]==binFwArgPrjID[1])&&(fwArgPrjID[2]==binFwArgPrjID[2]))
    if ((!isBlank) && (fwArgPrjID[0] == fw_data[specificArguAddr + BL_FWVER_MAIN_OFFSET]) && (fwArgPrjID[1] == fw_data[specificArguAddr + BL_FWVER_ARGU_OFFSET]) && (fwArgPrjID[2] == fw_data[specificArguAddr + BL_PROJECT_ID_OFFSET])) {
        bl_get_fw_bin_checksum(fw_data, &fw_bin_checksum, firmware_file_type, downSize, specificArguAddr);
        ret = bl_get_fw_checksum(&fw_checksum);
        if ((ret < 0) || (fw_checksum != fw_bin_checksum)) {
            DBG_DIRECT("bl_update_fw_for_self_ctp:Read checksum fail fw_checksum = %x\n", fw_checksum);
            fw_checksum = 0x00;
        }
        DBG_DIRECT("bl_update_fw_for_self_ctp:fw_checksum = 0x%x,fw_bin_checksum = 0x%x___\n", fw_checksum, fw_bin_checksum);
    }

//Step 9:Select update fw+arg or only update arg
    update_type = choose_update_type_for_self_ctp(isBlank, touchSwitch, fw_data, fwArgPrjID[0], fwArgPrjID[1], fw_checksum, fw_bin_checksum, specificArguAddr, binFwArgPrjID[0], binFwArgPrjID[1]);

//Step 10:Start Update depend condition
    if (update_type != NONE_UPDATE) {
        ret = bl_update_flash(update_type, firmware_file_type, fw_data, downSize, specificArguAddr);
        if (ret < 0) {
            DBG_DIRECT("bl_update_fw_for_self_ctp:bl_update_flash failed\n");
            goto UPDATE_ERROR;
        }
    }

//Step 11:Execute second update flow when project firmware is cob and last update_type is FW_ARG_UPDATE
    if ((ret == 0) && (IsCobPrj) && (update_type == FW_ARG_UPDATE)) {
        isBlank = 0;
        DBG_DIRECT("bl_update_fw_for_self_ctp:bl_update_flash for COB project need second update with blank IC:isBlank = %d\n", isBlank);
        goto UPDATE_SECOND_FOR_COB;
    }
    DBG_DIRECT("bl_update_fw_for_self_ctp exit\n");

UPDATE_ERROR:
    return ret;
}
#if 0
static int bl_update_fw_for_compatible_ctp(unsigned char firmware_file_type, unsigned char *fw_data, int fw_size)
{
    unsigned char prjID;    //firmware version
    unsigned char chip_id = 0x00;   //The IC identification
    int ret = 0x00;
    unsigned char isBlank = 0x0;    //Indicate the IC have any firmware
    unsigned short fw_checksum = 0x0;  //The checksum for firmware in IC
    unsigned short fw_bin_checksum = 0x0;  //The checksum for firmware in file
    unsigned char update_type = NONE_UPDATE;
    unsigned int downSize = 0x0;       //The available size of firmware data in file
    unsigned char cobID[6] = {0};           //The identification for COB project
    unsigned char fwCobID[12] = {0};         //The identification saved in touch controller IC
    unsigned int specificArguAddr = BL_ARGUMENT_BASE_OFFSET;   //The specific argument base address in firmware date with cobID
    unsigned char arguCount = 0x0;      //The argument count for COB firmware
    unsigned char checkCobID = 0x0;     //Specify whether the cobID is same or not with firmware data and hardware
    unsigned char IsCobPrj = 0;         //Judge the project type depend firmware file
    unsigned char touchSwitch = 0;      //Judge wether switch touch

    DBG_DIRECT("bl_update_fw_for_compatible_ctp start\n");

//Step 1:Obtain project type
    IsCobPrj = bl_is_cob_project(fw_data, fw_size);
    DBG_DIRECT("bl_update_fw_for_compatible_ctp:IsCobPrj = %x", IsCobPrj);

//Step 2:Obtain IC version number
    MDELAY(5);
    ret = bl_get_prj_id(&prjID);
    if ((ret < 0)
        || ((ret == 0) && (prjID == 0))
        || ((ret == 0) && (prjID == 0xff))
        || ((ret == 0) && (prjID == BL_PRJ_ID_REG))) {
        isBlank = 1;
        DBG_DIRECT("bl_update_fw_for_compatible_ctp:This is blank IC ret = %x fwArgPrjID[0]=%x\n", ret);
    } else {
        isBlank = 0;
        DBG_DIRECT("bl_update_fw_for_compatible_ctp:ret=%d fwID=%d argID=%d prjID=%d\n", ret, prjID);
    }

//Step 3:Specify download size
    if (IsCobPrj) {
        downSize = bl_get_cob_project_down_size_arguCnt(fw_data, fw_size, &arguCount);
        DBG_DIRECT("bl_update_fw_for_compatible_ctp:downSize = %x,arguCount = %x\n", downSize, arguCount);
    } else {
        downSize = fw_size;
        DBG_DIRECT("bl_update_fw_for_compatible_ctp:downSize = %x\n", downSize);
    }

UPDATE_SECOND_FOR_COB:
//Step 4:Update the fwArgPrjID
    if (!isBlank) {
        bl_get_prj_id(&prjID);
    }
//Step 5:Specify the argument data for cob project
    if (IsCobPrj && !isBlank) {
        ret = bl_get_cob_id(cobID);
        if (ret < 0) {
            DBG_DIRECT("bl_update_fw_for_compatible_ctp:bl_get_cob_id error\n");
            ret = -1;
            goto UPDATE_ERROR;
        } else {
            DBG_DIRECT("bl_update_fw_for_compatible_ctp:cobID = %x %x %x %x %x %x %x\n", cobID[0], cobID[1], cobID[2], cobID[3], cobID[4], cobID[5], cobID[6]);
        }
        ret = bl_get_specific_argument(&specificArguAddr, cobID, fw_data, fw_size, arguCount);
        if (ret < 0) {
            DBG_DIRECT("Can't found argument for CTP module\n");
            goto UPDATE_ERROR;
        }
        DBG_DIRECT("bl_update_fw_for_compatible_ctp:specificArguAddr = %x\n", specificArguAddr);
    }

//Step5:Specify whether switch the touch
    if (IsCobPrj && !isBlank) {
        bl_get_flash_cob_id(fwCobID);
        ret = memcmp(fw_data + specificArguAddr + BL_COB_ID_OFFSET, fwCobID, BL_COB_ID_LEN);
        if (ret) {
            touchSwitch = 1;
        }
    }

//Step 6:Obtain project infomation
#if 0
    if (!isBlank) {
        ret = bl_check_prj_info(fw_data, IsCobPrj, specificArguAddr);
        if (ret < 0) {
            DBG_DIRECT("bl_update_fw_for_compatible_ctp:check project info error\n");
            ret = -1;
            goto UPDATE_ERROR;
        }
    }
#endif

//Step 7:Obtain IC firmware checksum when the version number is same between IC and host firmware
    if ((!isBlank) && (prjID == fw_data[specificArguAddr + PJ_ID_OFFSET])) {
        bl_get_fw_bin_checksum(fw_data, &fw_bin_checksum, firmware_file_type, downSize, specificArguAddr);
        ret = bl_get_fw_checksum(&fw_checksum);
        if ((ret < 0) || (fw_checksum != fw_bin_checksum)) {
            DBG_DIRECT("bl_update_fw_for_compatible_ctp:Read checksum fail fw_checksum = %x\n", fw_checksum);
            fw_checksum = 0x00;
        }
        DBG_DIRECT("bl_update_fw_for_compatible_ctp:fw_checksum = 0x%x,fw_bin_checksum = 0x%x___\n", fw_checksum, fw_bin_checksum);
    }

//Step 8:Select update fw+arg or only update arg
    update_type = choose_update_type_for_compatible_ctp(isBlank, touchSwitch, fw_data, prjID, fw_checksum, fw_bin_checksum, specificArguAddr);

//Step 9:Start Update depend condition
    if (update_type != NONE_UPDATE) {
        ret = bl_update_flash(update_type, firmware_file_type, fw_data, downSize, specificArguAddr);
        if (ret < 0) {
            DBG_DIRECT("bl_update_fw_for_compatible_ctp:bl_update_flash failed\n");
            goto UPDATE_ERROR;
        }
    }

//Step 10:Execute second update flow when project firmware is cob and last update_type is FW_ARG_UPDATE
    if ((ret == 0) && (IsCobPrj) && (update_type == FW_ARG_UPDATE)) {
        isBlank = 0;
        DBG_DIRECT("bl_update_fw_for_compatible_ctp:bl_update_flash for COB project need second update with blank IC:isBlank = %d\n", isBlank);
        goto UPDATE_SECOND_FOR_COB;
    }
    DBG_DIRECT("bl_update_fw_for_compatible_ctp exit\n");

UPDATE_ERROR:
    return ret;
}
#endif
static int bl_update_fw(unsigned char fileType, unsigned char ctpType, unsigned char *pFwData, unsigned int fwLen)
{
    int ret = 0;
    switch (ctpType) {
    case SELF_CTP:
        ret = bl_update_fw_for_self_ctp(fileType, pFwData, fwLen);
        break;

    case COMPATIBLE_CTP:
        //ret = bl_update_fw_for_compatible_ctp(fileType,pFwData,fwLen);
        break;
    }
    return ret;
}
#ifdef BL_AUTO_UPDATE_FARMWARE
int bl_auto_update_fw(void)
{
    /*int ret = 0;
    unsigned char fwLen[2] = {0};

    memcpy((uint8_t *)fwLen, (uint8_t *)TP_ADDR, 2);
    DBG_DIRECT("bl_auto_update_fw:fwLen[0] = %x,fwLen[1]=%x\n",fwLen[0],fwLen[1]);
    if((fwLen[0] == 0xff) && (fwLen[1]==0xff))
    {
        ret = -1;
    	DBG_DIRECT("bl_auto_update_fw:Please write BIN to flash\n");
    }
    else
    {
        ret = bl_update_fw(HEADER_FILE_UPDATE, SELF_CTP, NULL, (fwLen[0] << 8 | fwLen[1]));
    }

    if(ret < 0)
    {
    	DBG_DIRECT("bl_auto_update_fw: bl_update_fw fail\n");
    }
    else
    {
    	DBG_DIRECT("bl_auto_update_fw: bl_update_fw success\n");
    }
    return ret;*/
    int ret = 0;
    unsigned int fwLen = sizeof(fwbin);

    DBG_DIRECT("bl_auto_update_fw:fwLen = %x\n", fwLen);
    ret = bl_update_fw(HEADER_FILE_UPDATE, SELF_CTP, (unsigned char *)fwbin, fwLen);

    if (ret < 0) {
        DBG_DIRECT("bl_auto_update_fw: bl_update_fw fail\n");
    } else {
        DBG_DIRECT("bl_auto_update_fw: bl_update_fw success\n");
    }

    return ret;
}
#endif

#if 0//def BL_UPDATE_FARMWARE_WITH_BIN
static int bl_GetFirmwareSize(char *firmware_path)
{
    struct file *pfile = NULL;
    struct inode *inode;
    unsigned long magic;
    off_t fsize = 0;

    if (NULL == pfile) {
        pfile = filp_open(firmware_path, O_RDONLY, 0);
    }
    if (IS_ERR(pfile)) {
        pr_err("error occured while opening file %s.\n", firmware_path);
        return -EIO;
    }

    inode = pfile->f_dentry->d_inode;
    magic = inode->i_sb->s_magic;
    fsize = inode->i_size;
    filp_close(pfile, NULL);

    return fsize;
}

static int bl_ReadFirmware(char *firmware_path, unsigned char *firmware_buf)
{
    struct file *pfile = NULL;
    struct inode *inode;
    unsigned long magic;
    off_t fsize;
    loff_t pos;
    mm_segment_t old_fs;

    if (NULL == pfile) {
        pfile = filp_open(firmware_path, O_RDONLY, 0);
    }

    if (IS_ERR(pfile)) {
        DBG_DIRECT("error occured while opening file %s. %d\n", firmware_path, PTR_ERR(pfile));
        return -EIO;
    }

    inode = pfile->f_dentry->d_inode;
    magic = inode->i_sb->s_magic;
    fsize = inode->i_size;

    old_fs = get_fs();
    set_fs(KERNEL_DS);

    pos = 0;
    vfs_read(pfile, firmware_buf, fsize, &pos);
    filp_close(pfile, NULL);
    set_fs(old_fs);

    return 0;
}

int bl_fw_upgrade_with_bin_file(struct ts_info *ts, unsigned char *firmware_path)
{
    unsigned char *pbt_buf = NULL;
    int ret = 0;
    int fwsize = 0;
    int i = 0;

    //Obtain the size of firmware bin
    fwsize = bl_GetFirmwareSize(firmware_path);
    if (fwsize <= 0) {
        DBG_DIRECT("Get firmware size error:%d\n", fwsize);
        return -1;
    }
    DBG_DIRECT("%s:Get firmware size %x\n", __func__, fwsize);

    pbt_buf = kmalloc(fwsize + 1, GFP_KERNEL);
    if (pbt_buf == NULL) {
        DBG_DIRECT("%s ERROR:Get buf failed\n", __func__);
        return -1;
    }

    ret = bl_ReadFirmware(firmware_path, pbt_buf);
    if (ret < 0) {
        DBG_DIRECT("%s ERROR:Read firmware data failed\n", __func__);
        kfree(pbt_buf);
        return -EIO;
    }

    //Call the upgrade procedure
    ret = bl_update_fw(ts, BIN_FILE_UPDATE, pbt_buf, fwsize);

    kfree(pbt_buf);
    return ret;
}
#endif


#endif


