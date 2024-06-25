#include "stdio.h"
#include "string.h"
#include "alipay_iot.h"
//#include "yc_debug.h"
#include "kdf.h"
#include "app_config.h"

#if TCFG_PAY_ALIOS_ENABLE && (TCFG_PAY_ALIOS_WAY_SEL==TCFG_PAY_ALIOS_WAY_T_HEAD)
typedef int FILE_FD;                            /*file handle type*/
#define TEST_ERROR -1
#define TEST_OK 0
#define SUID_NAME "suid"

/*test case to verify hal_***_asset api*/

#if 0

static uint8_t test_data[] = {
    0x68, 0x65, 0x6C, 0x6C, 0x6F, 0x2C, 0x74, 0x68,
    0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x63, 0x73,
    0x69, 0x20, 0x74, 0x65, 0x73, 0x74, 0x20, 0x73,
    0x74, 0x72, 0x69, 0x6E, 0x67, 0x0A, 0x20, 0x73
};
static char name1[] = "asset_test_1";
static uint32_t temp;
static uint8_t readbuf[32];

int hal_asset_test_demo(void)
{
    int ret = 0;
    unsigned int fd;
    temp = sizeof(readbuf);

    MyPrintf("\n=====================hal_asset_test_demo start!=====================\n");

    ret = hal_open_asset((void **)(&fd), name1);
    if (ret) {
        MyPrintf("%s is open failed! \n", name1);
        goto ERROR;
    } else {
        MyPrintf("%s is open success! \n", name1);
    }

    ret = hal_write_asset((void *)fd, 0, test_data, sizeof(test_data));
    if (ret) {
        MyPrintf("%s is write failed! \n", name1);
        goto ERROR;
    } else {
        MyPrintf("%s is write success! \n", name1);
    }

    ret = hal_read_asset((void *)fd, 0, readbuf, &temp);
    if (ret) {
        MyPrintf("%s is read failed! \n", name1);
        goto ERROR;
    } else {
        if (memcmp(test_data, readbuf, temp)) {
            MyPrintf("%s is read failed! read data not match write.\n", name1);
            goto ERROR;
        } else {
            MyPrintf("%s is read success! \n", name1);
        }
    }

    ret = hal_close_asset((void *)fd);
    if (ret) {
        MyPrintf("%s is close failed! \n", name1);
        goto ERROR;
    } else {
        MyPrintf("%s is close success! \n", name1);
    }

    ret = hal_remove_asset(name1);
    if (ret) {
        MyPrintf("%s is remove failed! \n", name1);
        goto ERROR;
    } else {
        MyPrintf("%s is remove success! \n", name1);
    }

    MyPrintf("\n hal_asset_test_demo Fuzzing testing pass! \n");
    MyPrintf("=====================hal_asset_test_demo end!=====================\n");
    return HAL_OK;
ERROR:
    MyPrintf("\n hal_asset_test_demo Fuzzing testing not pass! \n");
    MyPrintf("=====================hal_asset_test_demo end!=====================\n");
    return HAL_ERROR;
}

int hal_test_suid_update(void)
{
    int ret = 0;
    unsigned int fd;
    uint8_t data[288] = {0};

    ret = hal_open_asset((void **)(&fd), SUID_NAME);
    if (ret) {
        MyPrintf("%s is open failed! \n", SUID_NAME);
        return -1;
    } else {
        MyPrintf("%s is open success! \n", SUID_NAME);
    }

    ret = hal_write_asset((void *)fd, 0, data, sizeof(data));
    if (ret) {
        MyPrintf("%s is write failed! match as expected! \n", SUID_NAME);
        ret = 0;
    } else {
        MyPrintf("%s is write success! not match as expected!\n", SUID_NAME);
        ret = -2;
    }

    hal_close_asset((void *)fd);

    return ret;
}

int hal_test_suid_remove(void)
{
    int ret = 0;
    unsigned int fd;
    uint8_t data[288] = {0};

    ret = hal_remove_asset(SUID_NAME);
    if (ret) {
        MyPrintf("%s remove failed!match as expected!\n", SUID_NAME);

    } else {
        MyPrintf("%s remove success! not match as expected\n", SUID_NAME);
        return -1;
    }

    csi_clear_assets();

    ret = test_get_credential();
    if (ret) {
        MyPrintf("suid remove case get credential failed:%d\r\n", ret);
    } else {
        ret = hal_hardware_compat_check();
        if (ret) {
            MyPrintf("suid update remove hal_hardware_compat_check failed:%d\r\n", ret);
        }
    }

    return ret;
}

int hal_test_rand(void)
{
    int i, ret = HAL_OK;
    uint8_t rng[10];
    MyPrintf("\n=====================hal_rng_get_data start!=====================\n");

    ret = hal_rng_get_data(rng, sizeof(rng));
    if (ret) {
        MyPrintf("Get random 10bytes Fuzzing testing not pass! \n");
        return HAL_ERROR;
    } else {
        /*
        MyPrintf("rand data: ");
        for (i = 0; i < sizeof(rng); i++) {
        	MyPrintf("%d ", rng[i]);
        }*/
        MyPrintf(" Get random 10bytes Fuzzing testing pass! \n");
    }

    ret = hal_rng_get_data(NULL, sizeof(rng));
    if (ret) {
        MyPrintf("Get random with random buffer is NULL Fuzzing testing pass!\n");
    } else {
        MyPrintf("Get random with random buffer is NULL Fuzzing testing not pass!\n");
        return HAL_ERROR;
    }

    MyPrintf("=====================hal_rng_get_data end!=====================\n");
    return HAL_OK;
}

int hal_test_uuid(void)
{
    int ret = HAL_OK;
    uint32_t uuid[4] = {0};
    uint32_t uuid_len;

    MyPrintf("\n=====================hal_get_huk start!=====================\n");

    ret = hal_get_huk((uint8_t *)uuid, &uuid_len);
    if (ret) {
        MyPrintf("Get board uuid Fuzzing testing not pass!\n");
        return HAL_ERROR;
    } else {
        MyPrintf("get uuid: 0x%x 0x%x 0x%x 0x%x; Get board uuid Fuzzing testing pass!\n", uuid[0], uuid[1], uuid[2], uuid[3]);
    }

    ret = hal_get_huk(NULL, &uuid_len);
    if (ret) {
        MyPrintf("Get board uuid with huk buffer is NULL Fuzzing testing pass!\n");
    } else {
        MyPrintf("Get board uuid with huk buffer is NULL Fuzzing testing not pass!\n");
        return HAL_ERROR;
    }

    ret = hal_get_huk((uint8_t *)uuid, NULL);
    if (ret) {
        MyPrintf("Get board uuid with huk len is NULL Fuzzing testing pass!\n");
    } else {
        MyPrintf("Get board uuid with huk len is NULL Fuzzing testing not pass!\n");
        return HAL_ERROR;
    }

    ret = hal_get_huk(NULL, NULL);
    if (ret) {
        MyPrintf("Get board uuid with huk buffer & len is NULL Fuzzing testing pass!\n");
    } else {
        MyPrintf("Get board uuid with huk buffer & len is NULL Fuzzing testing not pass!\n");
        return HAL_ERROR;
    }
    MyPrintf("=====================hal_get_huk end!=====================\n");

    return HAL_OK;
}

int hal_test_sn(void)
{
    int ret = 0;
    uint32_t sn[16] = {0};
    uint32_t sn_len = sizeof(sn);
    MyPrintf("\n=====================hal_get_sn start!=====================\n");
    //ret = hal_get_sn((uint8_t *)sn, &sn_len);
    if (ret) {
        MyPrintf("Get sn Fuzzing testing not pass!\n");
        return HAL_ERROR;
    } else {
        MyPrintf("Get sn Fuzzing testing pass!\n");
    }

    ret = hal_get_sn(NULL, &sn_len);
    if (ret) {
        MyPrintf("Get sn with buf_sn is NULL Fuzzing testing pass!\n");
    } else {
        MyPrintf("Get sn with buf_sn is NULL Fuzzing testing not pass!\n");
        return HAL_ERROR;
    }

    ret = hal_get_sn((uint8_t *)sn, NULL);
    if (ret) {
        MyPrintf("Get sn with len_sn is NULL Fuzzing testing pass!\n");
    } else {
        MyPrintf("Get sn with len_sn is NULL Fuzzing testing not pass!\n");
        return HAL_ERROR;
    }

    ret = hal_get_sn(NULL, NULL);
    if (ret) {
        MyPrintf("Get sn with buf_sn & len_sn is NULL Fuzzing testing pass!\n");
    } else {
        MyPrintf("Get sn with buf_sn & len_sn is NULL Fuzzing testing not pass!\n");
        return HAL_ERROR;
    }

    MyPrintf("=====================hal_get_sn end!=====================\n");

    return HAL_OK;
}

int hal_test_deviceid(void)
{
    int ret = 0;
    uint32_t deviceid[16] = {0};
    uint32_t deviceid_len = sizeof(deviceid);

    MyPrintf("\n=====================hal_get_deviceid start!=====================\n");
    ret = hal_get_deviceid((uint8_t *)deviceid, &deviceid_len);
    if (ret) {
        MyPrintf("Get devicedid Fuzzing testing not pass!\n");
        return HAL_ERROR;
    } else {
        MyPrintf("Deviceid : %s, deviceid_len:%d; Get devicedid Fuzzing testing pass!\n", (char *)deviceid, deviceid_len);
    }

    ret = hal_get_deviceid(NULL, &deviceid_len);
    if (ret) {
        MyPrintf("Get devicedid with buf_did is NULL Fuzzing testing pass!\n");
    } else {
        MyPrintf("Get devicedid with buf_did is NULL Fuzzing testing not pass!\n");
        return HAL_ERROR;
    }

    ret = hal_get_deviceid((uint8_t *)deviceid, NULL);
    if (ret) {
        MyPrintf("Get devicedid with len_did is NULL Fuzzing testing pass! \n");
    } else {
        MyPrintf("Get devicedid with len_did is NULL Fuzzing testing not pass! \n");
        return HAL_ERROR;
    }

    ret = hal_get_deviceid(NULL, NULL);
    if (ret) {
        MyPrintf("Get devicedid with buf_did & len_did is NULL Fuzzing testing pass! \n");
    } else {
        MyPrintf("Get devicedid with buf_did & len_did is NULL Fuzzing testing not pass! \n");
        return HAL_ERROR;
    }
    MyPrintf("=====================hal_get_deviceid end!=====================\n");
    return HAL_OK;
}

int hal_test_timestamp(void)
{
    int ret = 0;
    uint32_t time = 0;

    MyPrintf("\n=====================hal_get_timestamp start!=====================\n");
    ret = hal_get_timestamp(&time);
    if (ret) {
        MyPrintf("Get timestamp with Fuzzing testing not pass!\n");
        return HAL_ERROR;
    } else {
        MyPrintf("timestamp:%d; Get timestamp with tm is NULL Fuzzing testing pass!\n", time);
    }

    ret = hal_get_timestamp(NULL);
    if (ret) {
        MyPrintf("Get timestamp with tm is NULL Fuzzing testing pass!\n");
    } else {
        MyPrintf("Get timestamp with tm is NULL Fuzzing testing not pass!\n");
        return HAL_ERROR;
    }
    MyPrintf("=====================hal_get_timestamp end!=====================\n");
    return HAL_OK;
}

int hal_test_companyname(void)
{
    int ret = 0;
    uint8_t companyname[16] = {0};
    uint32_t companyname_len = sizeof(companyname);

    MyPrintf("\n=====================hal_get_companyname start!=====================\n");

    ret = hal_get_companyname(companyname, &companyname_len);
    if (ret) {
        MyPrintf("Get companyname with Fuzzing testing not pass!\n");
        return HAL_ERROR;
    } else {
        MyPrintf("companyname: %s; Get companyname with Fuzzing testing pass!\n", (char *)companyname);
    }

    ret = hal_get_companyname(NULL, &companyname_len);
    if (ret) {
        MyPrintf("Get companyname with buffer is NULL Fuzzing testing pass!\n");
    } else {
        MyPrintf("Get companyname with buffer is NULL Fuzzing testing not pass!\n");
        return HAL_ERROR;
    }

    ret = hal_get_companyname(companyname, NULL);
    if (ret) {
        MyPrintf("Get companyname with len is NULL Fuzzing testing pass!\n");
    } else {
        MyPrintf("Get companyname with len is NULL Fuzzing testing not pass!\n");
        return HAL_ERROR;
    }

    ret = hal_get_companyname(NULL, NULL);
    if (ret) {
        MyPrintf("Get companyname with buffer & len is NULL Fuzzing testing pass!\n");
    } else {
        MyPrintf("Get companyname with buffer & len is NULL Fuzzing testing not pass!\n");
        return HAL_ERROR;
    }

    MyPrintf("=====================hal_get_companyname end!=====================\n");
    return HAL_OK;
}

int hal_test_protoctype(void)
{
    int ret = 0;
    uint32_t protoctype = 0;

    MyPrintf("\n=====================hal_get_protoctype start!=====================\n");
    ret = hal_get_protoctype(&protoctype);
    if (ret) {
        MyPrintf("Get protoctype Fuzzing testing not pass! \n");
        return HAL_ERROR;
    } else {
        MyPrintf("Protoctype:%d; Get protoctype Fuzzing testing pass\n", protoctype);
    }

    ret = hal_get_protoctype(NULL);
    if (ret) {
        MyPrintf("Get protoctype with ptype is NULL Fuzzing testing pass!\n");
    } else {
        MyPrintf("Get protoctype with ptype is NULL Fuzzing testing not pass!\n");
        return HAL_ERROR;
    }

    MyPrintf("=====================hal_get_protoctype end!=====================\n");
    return HAL_OK;
}

int hal_test_ble_write(void)
{
    int ret = 0;
    uint32_t data[16] = {0};
    uint32_t data_len = sizeof(data);

    MyPrintf("\n=====================hal_ble_write start!=====================\n");
    memset(data, 0x66, data_len);
    ret = hal_ble_write((uint8_t *)data, data_len);
    if (ret) {
        MyPrintf("Ble write data with data_len is 16bytes Fuzzing testing not pass!\n");
        return HAL_ERROR;
    } else {
        MyPrintf("Ble write data with data_len is 16bytes Fuzzing testing pass!\n");
    }

    ret = hal_ble_write(NULL, data_len);
    if (ret) {
        MyPrintf("Ble write with data is NULL Fuzzing testing pass!\n");
    } else {
        MyPrintf("Ble write with data is NULL Fuzzing testing not pass!\n");
        return HAL_ERROR;
    }

    MyPrintf("=====================hal_ble_write end!=====================\n");
    return HAL_OK;
}



int hal_random_pressure_test_demo(void)
{
    int ret = 0;
    uint32_t uuid_len = 0;
    static uint8_t data[16] = {0};

    ret = hal_rng_get_data(data, sizeof(data));
    if (ret != 0) {
        MyPrintf("hal_rng_get_data failed, ret:%d!\n\n", ret);
        return ret;
    }

    ret = hal_get_huk(data, &uuid_len);
    if (ret != 0) {
        MyPrintf("hal_get_huk failed, ret:%d!\n\n", ret);
        return ret;
    }
    return ret;
}

int hal_muc_pressure_test_demo(void)
{
    int ret = 0;
    static uint32_t data[16] = {0};
    uint32_t sn_len = sizeof(data);
    uint32_t deviceid_len = sizeof(data);
    uint32_t companyname_len = sizeof(data);
    uint32_t time = 0;
    uint32_t protoctype = 0;

    /*
    ret = hal_get_sn((uint8_t *)data,&sn_len);
    if (ret != 0) {
    	MyPrintf("hal_get_sn failed,ret:%d!\n\n", ret);
    	return ret;
    }*/

    ret = hal_get_deviceid((uint8_t *)data, &deviceid_len);
    if (ret != 0) {
        MyPrintf("hal_get_deviceid failed, ret:%d!\n\n", ret);
        return ret;
    }
    /*
    ret = hal_get_timestamp(&time);
    if (ret != 0) {
    	MyPrintf("hal_get_timestamp failed, ret:%d!\n\n", ret);
    	return ret;
    }*/

    ret = hal_get_companyname((uint8_t *)data, &companyname_len);
    if (ret != 0) {
        MyPrintf("hal_get_companyname failed, ret:%d!\n\n", ret);
        return ret;
    }

    ret = hal_get_protoctype(&protoctype);
    if (ret != 0) {
        MyPrintf("hal_get_protoctype failed, ret:%d!\n\n", ret);
        return ret;
    }

    return ret;
}



int hal_random_pressure_test(void)
{
    int i = 0;
    int ret = 0;
    MyPrintf("hal_random_pressure_test start!\n\n");
    for (i = 1; i <= 1000; i++) {
        ret = hal_random_pressure_test_demo();
        if (ret != 0) {
            MyPrintf("hal_random_pressure_test failed,cycle:%d!\n\n", i);
            return HAL_ERROR;
        } else {
            MyPrintf(". ");
            if (i % 100 == 0) {
                MyPrintf("\n");
            }
        }
    }
    MyPrintf("\n");
    MyPrintf("hal_random_pressure_test end!\n\n");
    return HAL_OK;
}


int hal_muc_pressure_test(void)
{
    int i = 0;
    int ret = 0;
    MyPrintf("hal_muc_pressure_test start!\n\n");
    for (i = 1; i <= 1000; i++) {
        ret = hal_muc_pressure_test_demo();
        if (ret != 0) {
            MyPrintf("hal_muc_pressure_test failed,cycle:%d!\n\n", i);
            return HAL_ERROR;
        } else {
            MyPrintf(". ");
            if (i % 100 == 0) {
                MyPrintf("\n");
            }
        }
    }
    MyPrintf("\n");
    MyPrintf("hal_muc_pressure_test end!\n\n");
    return HAL_OK;
}


#include "iotsec.h"
int hal_mem_test(void)
{
    void *p1 = NULL;
    void *p2 = NULL;
    int ret = TEST_OK;
    p1 = hal_malloc(200);
    if (p1 == NULL) {
        MyPrintf("hal_malloc failed!\n");
        goto ERROR;
    }

    p1 = hal_realloc(p1, 100);
    if (p1 == NULL) {
        MyPrintf("hal_realloc failed!\n");
        goto ERROR;
    }

    p1 = hal_realloc(p1, 255);
    if (p1 == NULL) {
        MyPrintf("hal_realloc failed!\n");
        goto ERROR;
    }

    p2 = hal_calloc(2, 128);
    if (p1 == NULL) {
        MyPrintf("hal_calloc_1 failed!\n");
        goto ERROR;
    }

    hal_free(p1);
    hal_free(p2);

    hal_log_ext("p2_addr:0x%x \n", p2);
    hal_log_ext("hal_mem_test end\n");

    csi_log_ext("vvvvp2_addr:0x%x \n", p2);

    return HAL_OK;

ERROR:
    return HAL_ERROR;
}
#endif

#endif
