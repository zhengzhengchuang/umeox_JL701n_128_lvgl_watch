
#include "data_export.h"
#include "system/includes.h"


#define DATA_EXPORT_MAGIC		0X5A // 标识符

// data_export 协议头
typedef struct {
    u8 magic;		//标识符DATA_EXPORT_MAGIC
    u8 ch;			//通道号：0 1 2 N
    u16 seqn;		//序列号：0 1 2 3 4 5 ...
    u16 crc;		//校验码：仅检验 raw data
    u16 len;		//数据长度：raw data length
    u32 timestamp;	//时间戳
    u32 total_len;	//数据累加总长：total_len += len
    u8  data[0];	// raw data
} de_header;

// 每一包的数据
typedef struct {
    struct list_head data_entry;
    int data_len;
    de_header data;
} de_ch_data ;

// 通道
typedef struct {
    struct list_head entry;
    struct list_head data_head;
    volatile u8 start;
    u16 seqn;
    u32 total_len;
    de_ch_ops *ops;
    void *buf;
} data_export_ch ;

//
typedef struct {
    struct list_head head;
    OS_SEM sem;
    OS_MUTEX mutex;
} data_export ;
static data_export de = {0};
#define __this		(&de)

static void DataExport_Task(void *p)
{
    u8 pend;
    data_export_ch *ch;
    while (1) {
        pend = 1;
        os_mutex_pend(&__this->mutex, 0);
        /* printf("%s,%d \n", __func__, __LINE__); */
        list_for_each_entry(ch, &__this->head, entry) {
            /* printf("%s,%d \n", __func__, __LINE__); */
            de_ch_data *ch_dat;
            de_ch_data *n;
            list_for_each_entry_safe(ch_dat, n, &ch->data_head, data_entry) {
                /* printf("%s,%d \n", __func__, __LINE__); */
                /* printf("ch:%d, seqn:%d, total:%d \n", ch_dat->data.ch, ch_dat->data.seqn, ch_dat->data.total_len); */
                pend = 0;
                int wlen = ch->ops->write(ch->ops->priv, &ch_dat->data, ch_dat->data_len);
                if (wlen != ch_dat->data_len) {
                    y_printf("data export write err:%d, %d \n", wlen, ch_dat->data_len);
                }
                local_irq_disable();
                list_del(&ch_dat->data_entry);
                local_irq_enable();
                lbuf_free(ch_dat);
            }
        }
        /* printf("%s,%d \n", __func__, __LINE__); */
        os_mutex_post(&__this->mutex);
        if (pend) {
            /* printf("%s,%d \n", __func__, __LINE__); */
            os_sem_pend(&__this->sem, 0);
        }
    }
}

int data_export_init(void)
{
    INIT_LIST_HEAD(&__this->head);
    os_sem_create(&__this->sem, 0);
    os_mutex_create(&__this->mutex);
    task_create(DataExport_Task, NULL, "data_export");
    return 0;
}

void *data_export_ch_create(int buf_len, de_ch_ops *ops)
{
    data_export_ch *hdl = zalloc(sizeof(data_export_ch) + buf_len);
    if (hdl) {
        hdl->ops = ops;
        hdl->buf = (void *)((int)hdl + sizeof(data_export_ch));
        lbuf_init(hdl->buf, buf_len, 4, 0);
        INIT_LIST_HEAD(&hdl->data_head);
        os_mutex_pend(&__this->mutex, 0);
        list_add_tail(&hdl->entry, &__this->head);
        os_mutex_post(&__this->mutex);
        hdl->start = 1;
    }
    return hdl;
}

void data_export_ch_release(void *hdl)
{
    if (!hdl) {
        return ;
    }
    data_export_ch *ch_hdl = hdl;
    ch_hdl->start = 0;
    os_mutex_pend(&__this->mutex, 0);
    list_del(&ch_hdl->entry);
    os_mutex_post(&__this->mutex);

    free(ch_hdl);
}


u8 *data_export_ch_write_alloc(void *hdl, u8 ch, int len)
{
    if (!hdl) {
        return NULL;
    }
    data_export_ch *ch_hdl = hdl;
    if (!ch_hdl->start) {
        printf("not start \n");
        return NULL;
    }
    de_ch_data *ch_dat = lbuf_alloc((struct lbuff_head *)ch_hdl->buf, sizeof(de_ch_data) + len);
    if (!ch_dat) {
        printf("lbuf_alloc err \n");
        return NULL;
    }

    //printf("%s %d %x\n",__func__,__LINE__,ch_dat);

    ch_dat->data_len = sizeof(de_header) + len;
    ch_dat->data.magic = DATA_EXPORT_MAGIC;
    ch_dat->data.ch = ch;
    ch_dat->data.seqn = ch_hdl->seqn++;
    ch_dat->data.len = len;
    ch_dat->data.timestamp = timer_get_ms();
    ch_hdl->total_len += ch_dat->data.len;
    ch_dat->data.total_len = ch_hdl->total_len;
    return ch_dat->data.data ;
}

int data_export_ch_write_alloc_push(void *hdl, u8 *data)
{
    if (!hdl || !data) {
        return false;
    }
    data_export_ch *ch_hdl = hdl;
    if (!ch_hdl->start) {
        printf("not start \n");
        return false;
    }

    de_header *de_data = container_of(data, de_header, data);
    de_ch_data *ch_dat = container_of(de_data, de_ch_data, data);
    //if (ch_dat) {
    //    printf("%s %d %x \n",__func__,__LINE__,ch_dat);
    //}

    ch_dat->data.crc = CRC16(data, ch_dat->data.len);
    local_irq_disable();
    list_add_tail(&ch_dat->data_entry, &ch_hdl->data_head);
    local_irq_enable();

    os_sem_set(&__this->sem, 0);
    os_sem_post(&__this->sem);

    return true;
}






int data_export_ch_write(void *hdl, u8 ch, void *buf, int len)
{
    if (!hdl || !buf) {
        return false;
    }
    data_export_ch *ch_hdl = hdl;
    if (!ch_hdl->start) {
        printf("not start \n");
        return false;
    }
    de_ch_data *ch_dat = lbuf_alloc((struct lbuff_head *)ch_hdl->buf, sizeof(de_ch_data) + len);
    if (!ch_dat) {
        printf("lbuf_alloc err \n");
        return false;
    }

    ch_dat->data_len = sizeof(de_header) + len;
    ch_dat->data.magic = DATA_EXPORT_MAGIC;
    ch_dat->data.ch = ch;
    ch_dat->data.seqn = ch_hdl->seqn++;
    ch_dat->data.crc = CRC16(buf, len);
    ch_dat->data.len = len;
    ch_dat->data.timestamp = timer_get_ms();
    ch_hdl->total_len += ch_dat->data.len;
    ch_dat->data.total_len = ch_hdl->total_len;
    memcpy(ch_dat->data.data, buf, len);

    /* printf("%s,%d \n", __func__, __LINE__); */
    /* printf("ch:%d, seqn:%d, total:%d \n", ch_dat->data.ch, ch_dat->data.seqn, ch_dat->data.total_len); */
    local_irq_disable();
    list_add_tail(&ch_dat->data_entry, &ch_hdl->data_head);
    local_irq_enable();

    os_sem_set(&__this->sem, 0);
    os_sem_post(&__this->sem);

    return true;
}






