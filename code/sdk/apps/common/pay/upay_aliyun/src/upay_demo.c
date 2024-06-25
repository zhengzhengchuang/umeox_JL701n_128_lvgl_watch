#include "upay.h"
#include "app_config.h"
#include "upay_vendor.h"
#include "bizbus_common.h"
#include "le_smartbox_adv.h"

#if TCFG_PAY_ALIOS_ENABLE && (TCFG_PAY_ALIOS_WAY_SEL==TCFG_PAY_ALIOS_WAY_ALIYUN)

#if 0
void upay_binding_test();
void upay_main()
{
    printf("%s called\n", __func__);
    //初始化函数
    //bizbus_init((void *)(& 0));
    bizbus_init(NULL);

    upay_env_init();

    upay_binding_test();
}

void upay_binding_test()
{
    int timeout = 0; //100s
    printf("Start binding...\n");
    printf("Before init ,status is %02X\n", upay_get_binding_status());
    upay_env_init();    //环境初始化，此时状态应该为STATUS_START_BINDING
    printf("After init ,status is %02X\n", upay_get_binding_status());
    //upay_ble_service_enable();    //打开Alipay BLE通信    @iot
    uint8_t device_info[57] = {0};
    uint32_t lenth_info = 0;
    upay_get_binding_string(device_info, &lenth_info);    //获取绑定码
    printf("need generage qrcode: device_info[%d]: %s\n", lenth_info, device_info);
    //////////二维码编码和显示////////////////
    //    uint8_t bitdata[QR_MAX_BITDATA];
    //    int side = qr_encode(QR_LEVEL_L, 0, device_info, lenth_info, bitdata);
    //    printf("Qrcode side length:%d\n", side);
    //    alipay_showQR(bitdata, side, 14, 14, 5);
    /////////////绑定状态轮询///////////////
    while (!timeout) {
        int status = upay_get_binding_status();
        // printf("quiry once, status is %02X now\n", status);
        if (UPAY_STATUS_BINDING_OK == status) {
            printf("Bind OK!\n");
            break;
        }
        if (UPAY_STATUS_BINDING_FAIL == status) {
            printf("Bind FAIL!\n");
            break;
        }
        os_time_dly(2);//vTaskDelay(2000);
    }
    upay_env_deinit();    //注销Alipay绑定环境
    printf("after deinit ,status is %02X\n", upay_get_binding_status());
    printf("End...\n ");
}

#endif

extern int le_controller_get_mac(void *addr);

extern void aliyun_bt_recv_register(void *cb_priv, void (*cb)(void *priv, u8 *data, int len));
extern void aliyun_upay_recv(void *priv, u8 *data, int len);

void aliyun_upay_init(void)
{
    extern int aliyun_storage_init(void);
    aliyun_storage_init();

    u8 mac[6];
    le_controller_get_mac(mac);
    y_printf("mac:");
    put_buf(mac, 6);

    // aliyun
    bizbus_init_para_t param = {0};
    strcpy(param.pk_str, 		TCFG_PAY_ALIOS_PK);
    strcpy(param.ps_str, 		TCFG_PAY_ALIOS_PS);
    strcpy(param.version_str, 	TCFG_PAY_ALIOS_VERSION);
    strcpy(param.vendor_str, 	TCFG_PAY_ALIOS_VENDOR);
    sprintf(param.mac_str, "%02X%02X%02X%02X%02X%02X",
            mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
    strcpy(param.id2_pk, 		TCFG_PAY_ALIOS_ID2_PK);

    param.dn_without_prefix = 1;

    printf("pk:%s \n", param.pk_str);
    printf("ps:%s \n", param.ps_str);
    printf("version:%s \n", param.version_str);
    printf("vendor:%s \n", param.vendor_str);
    printf("mac:%s \n", param.mac_str);
    printf("id2_pk:%s \n", param.id2_pk);

    bizbus_init(&param);

    // aliyun
    aliyun_bt_recv_register(NULL, aliyun_upay_recv);

    // upay
    upay_ble_regiest_recv_handle(upay_recv_data_handle);
    // void clear_upay_data(void);
    // clear_upay_data();


}

#endif /* #if TCFG_PAY_ALIOS_ENABLE && (TCFG_PAY_ALIOS_WAY_SEL==TCFG_PAY_ALIOS_WAY_ALIYUN) */

