
#include "data_export.h"
#include "system/includes.h"

#define DATA_EXPORT_FILE_NUM		3

typedef struct {
    int hdl[DATA_EXPORT_FILE_NUM];
    u8  ch_num;
    u8  ch_name[DATA_EXPORT_FILE_NUM];
    FILE *file;
    de_ch_ops ops;
} de_to_file ;

static int de_file_output(void *priv, void *buf, int len)
{
    de_to_file *de_file = priv;
    int wlen = fwrite(de_file->file, buf, len);
    /* printf("fw:0x%x, %d,%d \n", (int)de_file->file, wlen, len); */
    return wlen;
}

void data_export_file_close(void *hdl)
{
    de_to_file *de_file = hdl;
    if (!de_file) {
        return;
    }
    /* printf("data_export_file_close:0x%x, 0x%x, 0x%x \n", de_file, (int)de_file->hdl, (int)de_file->file); */
    for (int i = 0; i < DATA_EXPORT_FILE_NUM; i++) {
        if (de_file->hdl[i]) {
            data_export_ch_release((void *)de_file->hdl[i]);
            de_file->hdl[i] = 0;
        }
    }
    fclose(de_file->file);
    free(de_file);
}

void *data_export_file_open(void *path, int ch_num, int buf_len)
{
    de_to_file *de_file = zalloc(sizeof(de_to_file));
    if (!de_file) {
        return NULL;
    }
    de_file->ops.priv = de_file;
    de_file->ops.write = de_file_output;
    de_file->file = fopen(path, "w+");
    if (!de_file->file) {
        free(de_file);
        printf("create file error:%s \n", path);
        return NULL;
    }
    de_file->ch_num = ch_num;
    if (de_file->ch_num > DATA_EXPORT_FILE_NUM) {
        de_file->ch_num = DATA_EXPORT_FILE_NUM;
    }
    for (int i = 0; i < DATA_EXPORT_FILE_NUM; i++) {
        de_file->hdl[i] = data_export_ch_create(buf_len, &de_file->ops);
        if (!de_file->hdl[i]) {
            printf("data_export_ch_create \n");
            goto __error;
        }
    }
    /* printf("data_export_file_open:0x%x, 0x%x, 0x%x \n", de_file, (int)de_file->hdl, (int)de_file->file); */

    return de_file;

__error:
    for (int i = 0; i < DATA_EXPORT_FILE_NUM; i++) {
        if (de_file->hdl[i]) {
            data_export_ch_release((void *)de_file->hdl[i]);
            de_file->hdl[i] = 0;
        }
    }
    fclose(de_file->file);
    free(de_file);
    return NULL;
}

int data_export_file_set_ch_name(void *hdl, u8 ch, u8 ch_name)
{
    de_to_file *de_file = hdl;
    if (!de_file) {
        return false;
    }
    if (ch >= de_file->ch_num) {
        return false;
    }
    de_file->ch_name[ch] = ch_name;
    return true;
}

int data_export_file_write(void *hdl, u8 ch, void *buf, int len)
{
    de_to_file *de_file = hdl;
    if (!de_file) {
        return false;
    }
    if (ch >= de_file->ch_num) {
        return false;
    }
    return data_export_ch_write(de_file->hdl[ch], de_file->ch_name[ch], buf, len);
}


u8 *data_export_file_write_alloc(void *hdl, u8 ch, int len)
{
    de_to_file *de_file = hdl;
    if (!de_file) {
        return NULL;
    }
    return data_export_ch_write_alloc(de_file->hdl[ch], de_file->ch_name[ch], len);
}



int data_export_file_write_alloc_push(void *hdl, u8 ch, void *buf)
{
    de_to_file *de_file = hdl;
    if (!de_file) {
        return -1;
    }
    return data_export_ch_write_alloc_push(de_file->hdl[ch], buf);
}



// test
#if 0

#include "app_config.h"

static de_to_file *de_file_test = NULL;
static u16 de_file_tmr = 0;
static u32 de_file_test_cnt = 0;
static u8 de_file_test_buf[512];

static void de_file_test_w(void *priv)
{
    int len = 256 + rand32() % 256;
    u8 *buf = de_file_test_buf;
    for (int i = 0; i < len; i++) {
        buf[i] = len + i;
    }
    /* printf("%s,%d \n", __func__, __LINE__); */
    data_export_file_write(de_file_test, rand32() % 2, buf, len);
    de_file_test_cnt ++;
    if (de_file_test_cnt > 200) {
        printf("%s,%d \n", __func__, __LINE__);
        sys_hi_timer_del(de_file_tmr);
        de_file_tmr = 0;
    }
}

void data_export_file_test(void)
{
    printf("%s,%d \n", __func__, __LINE__);
    data_export_init();

    printf("%s,%d \n", __func__, __LINE__);
#if TCFG_SD0_ENABLE
    de_file_test = data_export_file_open("storage/sd0/C/defile.dat", 2, 5 * 1024);
#elif TCFG_SD1_ENABLE
    de_file_test = data_export_file_open("storage/sd1/C/defile.dat", 2, 5 * 1024);
#elif TCFG_UDISK_ENABLE
    de_file_test = data_export_file_open("storage/udisk0/C/defile.dat", 2, 5 * 1024);
#else
#error "no storage dev"
#endif
    printf("%s,%d \n", __func__, __LINE__);
    if (de_file_test) {
        data_export_file_set_ch_name(de_file_test, 0, 3);
        data_export_file_set_ch_name(de_file_test, 1, 5);
        de_file_tmr = sys_hi_timer_add(NULL, de_file_test_w, 50);
        os_time_dly(800);
        printf("%s,%d \n", __func__, __LINE__);
        if (de_file_tmr) {
            sys_hi_timer_del(de_file_tmr);
            de_file_tmr = 0;
        }
        data_export_file_close(de_file_test);
        de_file_test = NULL;
        printf("%s,%d \n", __func__, __LINE__);
    }
}



static u8 de_to_file_idle(void)
{
    return !de_file_test;
}

REGISTER_LP_TARGET(de_to_file_lp_target) = {
    .name = "de_to_file",
    .is_idle = de_to_file_idle,
};

#endif // test

