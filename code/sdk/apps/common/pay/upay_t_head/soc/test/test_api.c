#include "yunit.h"
//#include "yc_debug.h"
#include "app_config.h"
#if TCFG_PAY_ALIOS_ENABLE && (TCFG_PAY_ALIOS_WAY_SEL==TCFG_PAY_ALIOS_WAY_T_HEAD)
#undef _DEBUG_H_
#define LOG_TAG_CONST       TEST_API          //修改成文件名
#define LOG_TAG             "[TEST_API]"
#include "debug.h"
#define LOG_v(t)  log_tag_const_v_ ## t
#define LOG_i(t)  log_tag_const_i_ ## t
#define LOG_d(t)  log_tag_const_d_ ## t
#define LOG_w(t)  log_tag_const_w_ ## t
#define LOG_e(t)  log_tag_const_e_ ## t
#define LOG_c(t)  log_tag_const_c_ ## t
#define LOG_tag(tag, n) n(tag)
const char LOG_tag(LOG_TAG_CONST, LOG_v) AT(.LOG_TAG_CONST) = 0;
const char LOG_tag(LOG_TAG_CONST, LOG_i) AT(.LOG_TAG_CONST) = 1; //log_info
const char LOG_tag(LOG_TAG_CONST, LOG_d) AT(.LOG_TAG_CONST) = 1; //log_debug
const char LOG_tag(LOG_TAG_CONST, LOG_w) AT(.LOG_TAG_CONST) = 1;
const char LOG_tag(LOG_TAG_CONST, LOG_e) AT(.LOG_TAG_CONST) = 1;
const char LOG_tag(LOG_TAG_CONST, LOG_c) AT(.LOG_TAG_CONST) = 1;
#define MAX_CREDENTIAL_LEN 1500
extern int hal_asset_test_demo_1(void);
extern int hal_asset_test_demo_2(void);
extern int hal_asset_test_demo_3(void);
extern int hal_asset_test_demo_4(void);
extern int hal_asset_pressure_test(void);
extern int hal_random_pressure_test(void);
extern int hal_muc_pressure_test(void);

extern int hal_test_rand(void);
extern int hal_test_uuid(void);
extern int hal_test_sn(void);
extern int hal_test_deviceid(void);
extern int hal_test_timestamp(void);
extern int hal_test_companyname(void);
extern int hal_test_protoctype(void);
extern int hal_test_ble_write(void);

extern int hal_identify_chip_binding_test(void);
extern int kdf_function_pressrue_test(void) ;

extern int hal_mem_test(void);
extern int test_case_sha(void);
extern int test_case_aes(void);

#if 0

static void test_alipay_hal_mem()
{
    log_info("%s\n", __func__);
    int result = hal_mem_test();
    log_info("%s[line:%d]\n", __func__, __LINE__);
    YUNIT_ASSERT_MSG_QA(result == 0, "the test_alipay_hal_mem is %d", result, "ALIPAY_HAL_MME");
    log_info("%s[line:%d]\n", __func__, __LINE__);
}

static void test_alipay_hal_sha()
{
    int result = test_case_sha();
    YUNIT_ASSERT_MSG_QA(result == 0, "the test_alipay_hal_sha is %d", result, "ALIPAY_HAL_SHA");
}

static void test_alipay_hal_aes()
{
    int result = test_case_aes();
    YUNIT_ASSERT_MSG_QA(result == 0, "the test_alipay_hal_aes is %d", result, "ALIPAY_HAL_AES");
}

static void test_alipay_hal_asset()
{


    int result = 0;
    result = hal_asset_test_demo();
    YUNIT_ASSERT_MSG_QA(result == 0, "the test_alipay_hal_asset is %d", result, "ALIPAY_HAL_ASSET");
}

static void test_alipay_hal_suid_update()
{
    int result;
    result = hal_test_suid_update();
    YUNIT_ASSERT_MSG_QA(result == 0, "the test_alipay_hal_suid_update is %d", result, "ALIPAY_HAL_SUID_UPDATE");
}

static void test_alipay_hal_suid_remove()
{
    int result;
    result = hal_test_suid_remove();
    YUNIT_ASSERT_MSG_QA(result == 0, "the test_alipay_hal_suid_remove is %d", result, "ALIPAY_HAL_SUID_REMOVE");
}

static void test_alipay_hal_random_pressure()
{
    int result;
    result = hal_random_pressure_test();
    YUNIT_ASSERT_MSG_QA(result == 0, "the test_alipay_hal_random_pressure is %d", result, "ALIPAY_HAL_RANDOM_PRESSURE");
}

static void test_alipay_hal_mcu_pressure()
{
    int result;
    result = hal_muc_pressure_test();
    YUNIT_ASSERT_MSG_QA(result == 0, "the test_alipay_hal_mcu_pressure is %d", result, "ALIPAY_HAL_MCU_PRESSURE");
}

static void test_alipay_hal_random()
{
    int result;
    result = hal_test_rand();
    YUNIT_ASSERT_MSG_QA(result == 0, "the test_alipay_hal_random is %d", result, "ALIPAY_HAL_RANDOM");
}

static void test_alipay_hal_uuid()
{
    int result;
    result = hal_test_uuid();
    YUNIT_ASSERT_MSG_QA(result == 0, "the test_alipay_hal_uuid is %d", result, "ALIPAY_HAL_UUID");
}

static void test_alipay_hal_sn()
{
    int result;
    result = hal_test_sn();
    YUNIT_ASSERT_MSG_QA(result == 0, "the test_alipay_hal_sn is %d", result, "ALIPAY_HAL_SN");
}

static void test_alipay_hal_deviceid()
{
    int result;
    result = hal_test_deviceid();
    YUNIT_ASSERT_MSG_QA(result == 0, "the test_alipay_hal_deviceid is %d", result, "ALIPAY_HAL_DEVICEDID");
}

static void test_alipay_get_fw_version()
{
    int result;
    uint8_t version[8] = {0};
    result = csi_get_fw_version(version);

    YUNIT_ASSERT_MSG_QA(result == 0 && version[0] != 0, "the test_alipay_get_fw_version is %d", result, "GET_FW_VERSION");
}


static void test_alipay_hal_timestamp()
{
    int result;
    result = hal_test_timestamp();
    YUNIT_ASSERT_MSG_QA(result == 0, "the test_alipay_hal_timestamp is %d", result, "ALIPAY_HAL_TIMESTAMP");
}

static void test_alipay_hal_companyname()
{
    int result;
    result = hal_test_companyname();
    YUNIT_ASSERT_MSG_QA(result == 0, "the test_alipay_hal_companyname is %d", result, "ALIPAY_HAL_COMPANYNAME");
}

static void test_alipay_hal_protoctype()
{
    int result;
    result = hal_test_protoctype();
    YUNIT_ASSERT_MSG_QA(result == 0, "the test_alipay_hal_protoctype is %d", result, "ALIPAY_HAL_PROTOCTYPE");
}

static void test_alipay_hal_blewrite()
{
    int result;
    result = hal_test_ble_write();
    YUNIT_ASSERT_MSG_QA(result == 0, "the test_alipay_hal_blewrite is %d", result, "ALIPAY_HAL_BLEWRITE");
}

static void case_test_hal_mem(void)
{


    test_alipay_hal_mem();
}

static void case_test_hal_sha(void)
{
    test_alipay_hal_sha();
}

static void case_test_hal_aes(void)
{
    test_alipay_hal_aes();
}

static void case_test_hal_asset(void)
{
    test_alipay_hal_asset();
}

static void case_test_hal_suid_update(void)
{
    test_alipay_hal_suid_update();
}

static void case_test_hal_suid_remove(void)
{
    test_alipay_hal_suid_remove();
}

static void case_test_hal_random_pressure(void)
{
    test_alipay_hal_random_pressure();
}


static void case_test_hal_mcu_pressure(void)
{
    test_alipay_hal_mcu_pressure();
}

static void case_test_hal_random(void)
{
    test_alipay_hal_random();
}

static void case_test_hal_uuid(void)
{
    test_alipay_hal_uuid();
}

static void case_test_hal_sn(void)
{
    test_alipay_hal_sn();
}

static void case_test_hal_deviceid(void)
{
    test_alipay_hal_deviceid();
}

static void case_test_get_fw_version(void)
{
    test_alipay_get_fw_version();
}

static void case_test_hal_companyname(void)
{
    test_alipay_hal_companyname();
}

static void case_test_hal_timestamp(void)
{
    test_alipay_hal_timestamp();
}

static void case_test_hal_protoctype(void)
{
    test_alipay_hal_protoctype();
}

static void case_test_hal_blewrite(void)
{
    test_alipay_hal_blewrite();
}

int test_get_credential()
{
    uint8_t *data = NULL;
    uint32_t len = 0;
    int ret;

    data = hal_malloc(1500);
    if (data == NULL) {
        ret = -1;
    } else {
        ret = hal_get_credential_data(data, &len);
        if (ret != 0) {
            ret = -2;
        } else if (len == 0) {
            ret = -3;
        }
    }

    if (data != NULL) {
        hal_free(data);
        data = NULL;
    }

    return ret;
}

static void case_test_get_credential(void)
{
    int ret;

    ret = test_get_credential();
    YUNIT_ASSERT_MSG_QA(ret == 0, "the test_alipay_hal_credential is %d", ret, "ALIPAY_HAL_GET_CREDENTIAL");
}

static void case_test_compat_check(void)
{
    uint32_t len = 0;
    int ret;

    ret = hal_hardware_compat_check();
    YUNIT_ASSERT_MSG_QA(ret == 0, "the test_alipay_hal_compat_check is %d", ret, "ALIPAY_HAL_COMPAT_CHECK");
}

void api_record_test_entry(yunit_test_suite_t *suite)
{
    yunit_add_test_case(suite, "hal_get_credential", case_test_get_credential);
    yunit_add_test_case(suite, "hal_compat_check", case_test_compat_check);
    yunit_add_test_case(suite, "hal_suid_update", case_test_hal_suid_update);
    yunit_add_test_case(suite, "hal_suid_remove", case_test_hal_suid_remove);
    yunit_add_test_case(suite, "hal_mem", case_test_hal_mem);


    yunit_add_test_case(suite, "hal_sha", case_test_hal_sha);


    yunit_add_test_case(suite, "hal_aes", case_test_hal_aes);


    yunit_add_test_case(suite, "hal_asset", case_test_hal_asset);

    yunit_add_test_case(suite, "hal_random_pressure", case_test_hal_random_pressure);
    yunit_add_test_case(suite, "hal_mcu_pressure", case_test_hal_mcu_pressure);
    yunit_add_test_case(suite, "hal_random", case_test_hal_random);
    yunit_add_test_case(suite, "hal_uuid", case_test_hal_uuid);
    yunit_add_test_case(suite, "hal_sn", case_test_hal_sn);
    yunit_add_test_case(suite, "hal_deviceid", case_test_hal_deviceid);
    yunit_add_test_case(suite, "get_fw_version", case_test_get_fw_version);
    yunit_add_test_case(suite, "hal_companyname", case_test_hal_companyname);
    yunit_add_test_case(suite, "hal_timestamp", case_test_hal_timestamp);
    yunit_add_test_case(suite, "hal_prototype", case_test_hal_protoctype);


}


static int init(void)
{
    return 0;
}
static int cleanup(void)
{
    return 0;
}
static void setup(void)
{
    return;
}
static void teardown(void)
{
    return;
}


void test_record_suite_register(void)
{
    log_info("%s\n", __func__);
    yunit_test_suite_t *suite;
    suite = yunit_add_test_suite("ALIPAY_HAL", init, cleanup, setup, teardown);


    api_record_test_entry(suite);
}

void yunit_test_api(void)
{
    log_info("%s\n", __func__);
    test_record_suite_register();
}

#endif

#endif
