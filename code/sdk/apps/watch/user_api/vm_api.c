#include "vm.h"
#include "asm/crc16.h"
#include "system/syscfg_id.h"
#include "system/os/os_api.h"

#pragma pack(1)
struct mult_vm {
    u16 it_id;
    u16 total_crc;		//所有数据总长度CRC
    u16 len;
    u8	end_flag;//0xff:表示结束标志
    u8	data[0];
};
#pragma pack()

#define VM_API_ONCE_MAX_ITEM		(256L)
///<大数据量VM保存接口, 需要多个VM INDEX, 分次保存, delay是系统延续(每次写后延时)
extern u32 timer_get_ms(void);
void vm_api_write_mult(u16 start_id, u16 end_id, void *buf, u16 len, u32 delay)
{
    struct mult_vm head;
    u8 *tmp = NULL;
    /* u8 tmp[VM_API_ONCE_MAX_ITEM + sizeof(struct mult_vm)]; */
    u16 remain_len = 0;
    u16 i, counter, offset, vm_id;
    u8 *data_buf = (u8 *)buf;
    u16 total_crc = CRC16((const void *)data_buf, len);

    offset = 0;
    vm_id = start_id;
    counter = len / VM_API_ONCE_MAX_ITEM;
    remain_len = len % VM_API_ONCE_MAX_ITEM;

    /* printf("%s, %d, %d-----------countr = %d, len = %d---------------\n", __FUNCTION__, __LINE__, sizeof(struct mult_vm), counter, len); */
    if (counter > (end_id - start_id)) {
        printf("VM data is too long err \n");
        return ;
    }

    tmp = zalloc(VM_API_ONCE_MAX_ITEM + sizeof(struct mult_vm));
    if (tmp == NULL) {
        printf("malloc fail,%s\n", __FUNCTION__);
        return ;
    }

    /* u32 test_tick = 0;; */
    /* test_tick = timer_get_ms(); */
    for (i = 0; i < counter; i++) {
        head.total_crc = total_crc;//CRC16((const void *)(data_buf + offset), VM_API_ONCE_MAX_ITEM);
        head.it_id = vm_id;
        head.len = VM_API_ONCE_MAX_ITEM;
        if (remain_len) {
            head.end_flag = 0;
        } else {
            if (i == (counter - 1)) {
                head.end_flag = 0xff;
            }
        }
        memcpy(tmp, (u8 *)&head, sizeof(struct mult_vm));
        memcpy(tmp + sizeof(struct mult_vm), data_buf + offset, VM_API_ONCE_MAX_ITEM);

        syscfg_write(vm_id, tmp, sizeof(struct mult_vm) + VM_API_ONCE_MAX_ITEM);
        /* printf("-%d-\n", i); */
        os_time_dly(delay);
        offset += VM_API_ONCE_MAX_ITEM;
        vm_id++;
    }
    /* printf("####################################################%d##################################\n", timer_get_ms() - test_tick); */

    if (remain_len) {
        head.total_crc = total_crc;//CRC16((const void *)(data_buf + offset), remain_len);
        head.it_id = vm_id;
        head.len = remain_len;
        head.end_flag = 0xff;
        memcpy(tmp, (u8 *)&head, sizeof(struct mult_vm));
        memcpy(tmp + sizeof(struct mult_vm), data_buf + offset, remain_len);
        syscfg_write(vm_id, tmp, sizeof(struct mult_vm) + remain_len);
    }
    if (tmp) {
        free(tmp);
    }
    printf("music VM write succ\n");
}

///<大数据VM读取接口
int vm_api_read_mult(u16 start_id, u16 end_id, void *buf, u16 len)
{
    u8 *data_buf = (u8 *)buf;
    u16 crc_tmp = 0;
    int ret;
    int err = 0;
    struct mult_vm head;
    u16 offset, vm_id;
    /* u8 tmp[VM_API_ONCE_MAX_ITEM + sizeof(struct mult_vm)] = {0}; */
    u8 *tmp = NULL;
    if (!data_buf) {
        return 0;
    }

    tmp = zalloc(VM_API_ONCE_MAX_ITEM + sizeof(struct mult_vm));
    if (tmp == NULL) {
        printf("malloc fail,%s\n", __FUNCTION__);
        return 0;
    }

    vm_id = start_id;
    offset = 0;
    do {
        ret = syscfg_read(vm_id, tmp, VM_API_ONCE_MAX_ITEM + sizeof(struct mult_vm));
        if (ret) {
            memcpy((u8 *)&head, tmp, sizeof(struct mult_vm));
            if (head.len > VM_API_ONCE_MAX_ITEM) {
                printf("data err 1\n");
                err = -1;
                goto __read_mult_exit;
            }

            /* crc_tmp = CRC16(tmp+sizeof(struct mult_vm), head.len); */
            /* if(crc_tmp != head.total_crc) */
            /* { */
            /* printf("data err 2\n"); */
            /* err = -1; */
            /* goto __read_mult_exit; */
            /* } */

            if ((head.len + offset) > len) {
                printf("data err 3\n");
                err = -1;
                goto __read_mult_exit;
            }

            memcpy(data_buf + offset, tmp + sizeof(struct mult_vm), head.len);
            offset += head.len;
            if (head.end_flag == 0xff) {
                ///数据获取完毕
                break;
            }
        }
        vm_id++;
    } while (vm_id <= end_id);

    crc_tmp = CRC16(data_buf, offset);
    if (head.total_crc == crc_tmp) {
        printf("%s succ, vm read_len = %d\n", __FUNCTION__, offset);
        err = 0;
    } else {
        printf("%s crc fail!!!!\n", __FUNCTION__);
        err = -1;
    }

__read_mult_exit:
    if (tmp) {
        free(tmp);
    }

    if (err) {
        return 0;
    } else {
        return offset;
    }
}
#if 0
#define TST_BUF_SIZE 2048
static u8 tst_buf[TST_BUF_SIZE] = {0};
static u8 tst_counter = 0x55;


void vm_debug_t(void)
{
    u32 i;
    for (i = 0; i < TST_BUF_SIZE; i++) {
        tst_buf[i] = tst_counter;
    }
    int ret = syscfg_write(CFG_USB_BREAKPOINT0, tst_buf, TST_BUF_SIZE);
    printf("wr, vm tst_counter = %d, ret = %d\n", tst_counter, ret);
    if (ret) {
        memset(tst_buf, 0, TST_BUF_SIZE);
        ret = syscfg_read(CFG_USB_BREAKPOINT0, tst_buf, TST_BUF_SIZE);
        if (ret) {
            printf("read vm tst_counter = %d, ret = %d\n", tst_counter, ret);
            put_buf(tst_buf, TST_BUF_SIZE);
        }
    }
    tst_counter++;
}
#endif
