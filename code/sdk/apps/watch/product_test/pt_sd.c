#include "product_test.h"
#include "pt_sd.h"

#if PT_SD_ENABLE && (TCFG_SD0_ENABLE || TCFG_SD1_ENABLE)

static u8 pt_sd_busy = 0; // 忙碌标记
static u8 pt_sd_res = 0;      		// 测试结果
static char dev_name[6] = {0};
extern struct device *force_open_sd(char *sdx);

int pt_sd_init(void)
{
    if (force_open_sd("sd0")) {
        memcpy(dev_name, "sd0", strlen("sd0"));
    } else if (force_open_sd("sd1")) {
        memcpy(dev_name, "sd1", strlen("sd1"));
    } else {
        return PT_E_NO_DEV;
    }

    return 0;
}

int pt_sd_test_start(void)
{
    FILE *fp = NULL;
    int err = 0;
    char mount_path[32] = "storage/";
    char file_name[64] = {0};
    if (!strlen(dev_name)) {
        err = PT_E_MOD_UNREADY;
        goto _end;
    }
    strcat(mount_path, dev_name);
    memcpy(file_name, mount_path, strlen(mount_path));
    strcat(file_name, "/C"); //分区信息C\D\E\F
    strcat(file_name, "/abc.txt");//需要操作的文件
    printf("pt_sd_test >>>> mount_path: %s, test_file = %s", mount_path, file_name);
    void *mnt = mount(dev_name, mount_path, "fat", 3, NULL);
    if (mnt) {
        printf("sd mount fat succ");
    } else {
        printf("sd mount fat failed");
        err = PT_E_MOD_TEST_ERROR;
        goto _end;
    }

    u8 str[] = "This is a test string.";
    u8 buf[100];
    u8 len;

    fp = fopen(file_name, "w+");
    if (!fp) {
        printf("open file ERR!");
        err = PT_E_MOD_TEST_ERROR;
        goto _end;
    }

    len = fwrite(fp, str, sizeof(str));

    if (len != sizeof(str)) {
        printf("write file ERR!");
        err = PT_E_MOD_TEST_ERROR;
        goto _end;
    }
#if 1
    fseek(fp, 0, SEEK_SET);

    len = fread(fp, buf, sizeof(str));

    if (len != sizeof(str)) {
        printf("read file ERR!");
        err = PT_E_MOD_TEST_ERROR;
        goto _end;
    }

    put_buf(buf, sizeof(str));
#endif
    printf("SD ok!");

_end:
    if (fp) {
        fdelete(fp);
        fp = NULL;
    }
    pt_sd_res = err;
    return err;
}

int pt_sd_start(void)
{
    if (pt_sd_busy) {
        return PT_E_MOD_RUN;
    }
    pt_sd_res = PT_E_MOD_RUN;
    int msg[3] = {0};
    msg[0] = (int)pt_sd_test_start;
    msg[1] = 1;
    msg[2] = (int)0;
    do {
        int os_err = os_taskq_post_type("app_core", Q_CALLBACK, 3, msg);
        if (os_err == OS_ERR_NONE) {
            break;
        }
        if (os_err != OS_Q_FULL) {
            pt_sd_res = PT_E_SYS_ERROR;
            return PT_E_SYS_ERROR;
        }
        os_time_dly(1);
    } while (1);
    pt_sd_busy = 1;

    return 0;
}

int pt_sd_stop(void)
{
    if (pt_sd_res == PT_E_MOD_RUN) {
        pt_sd_res = PT_E_MOD_STOP_NO_END;
    }
    return 0;
}

int pt_sd_ioctrl(u32 order, int len, void *param)
{
    u32 result = 0;
    switch (PT_ORDER_C_GET(order)) {
    case PT_N_C_START:
        result = pt_sd_start();
        break;
    case PT_N_C_STOP:
        result = pt_sd_stop();
        break;
    case PT_N_C_GET_RESULT:
        result = pt_sd_res;
        break;
    default:
        result = PT_E_PARAM;
        break;
    }
    product_test_push_data(order, 4, &result);
    return result;
}


REGISTER_PT_MODULE(sd) = {
    .module = PT_M_SD,
    .attr	= PT_ATTR_SELF,
    .init	= pt_sd_init,
    .ioctrl	= pt_sd_ioctrl,
};

void pt_sd_test(void)
{
    pt_sd_init();
    pt_sd_start();
}

#endif /* #if PT_SD_ENABLE */

