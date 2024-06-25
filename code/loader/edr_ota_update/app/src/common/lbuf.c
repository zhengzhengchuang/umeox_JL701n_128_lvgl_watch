/*  ******************************************
 *
 *  |offset| hentry | priv | data|
 *  |              ret     |
 *  | 					   |
 *	|---------align--------|
 *
 *  *****************************************/

#include "lbuf.h"
#include "common.h"

#define LOG_TAG_CONST       LBUF
#define LOG_TAG             "[LBUF]"
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"

#ifdef ALIGN
#undef ALIGN
#endif

#define ALIGN(a, b) \
	({ \
	 	int m = (u32)(a) & ((b)-1); \
		int ret = (u32)(a) + (m?((b)-m):0);	 \
		ret;\
	})

//计算数据包的最小长度和hentry管理结构体放一起后要对齐的偏移,lbuf管理结构体的默认对齐是4,但数据包的对齐可以任意指定所以需要进行对齐偏移计算
#define LBUF_OFFSET(head) \
	({ \
		int m = (sizeof(struct hentry)+head->priv_len) & (head->align-1); \
	 	m?(head->align-m):0;\
	 })


//根据hentry结构体大小得到hentry结构体地址
#define __get_entry(lbuf)   \
	(struct hentry *)((u8 *)lbuf - sizeof(struct hentry))

#if LBUF_DEBUG

#define lbuf_entry_check0(head, entry,rets) \
    do { \
    ASSERT(entry->magic_a == 0x12345678 && entry->magic_b == 0x23456789, \
        "%d,%x,%x,%x\n", __LINE__, entry->magic_a, entry->magic_b,rets); \
    ASSERT(head->magic_a == 0x87654321 && head->magic_b == 0x98765432, \
        "%d,%p,%x,%x,%x,%x\n", __LINE__, head, head->total_size, head->magic_a, head->magic_b,rets); \
    } while(0)

#define lbuf_entry_check(head, entry) \
    do { \
    ASSERT(entry->magic_a == 0x12345678 && entry->magic_b == 0x23456789, \
        "%d,%x,%x\n", __LINE__, entry->magic_a, entry->magic_b); \
    ASSERT(head->magic_a == 0x87654321 && head->magic_b == 0x98765432, \
        "%d,%p,%x,%x,%x\n", __LINE__, head, head->total_size, head->magic_a, head->magic_b); \
    } while(0)

#else

#define lbuf_entry_check0(head, entry,rets) do { } while (0)
#define lbuf_entry_check(head, entry) do { } while (0)

#endif

struct hfree {
    struct list_head entry;
    u32 len;
};

struct hentry {
#if LBUF_DEBUG
    int magic_a;
#endif
    struct list_head entry;
    u16 len;
    ///hentry结构体与lbuf_head结构体的头地址的偏移
    u16 offset;
    ///需要被读的次数
    u8 ref;
    ///需要映射的通道
    u8 channel_map;
#if LBUF_DEBUG
    int magic_b;
#endif
    int rets;
};


struct lbuff_head *lbuf_init(void *buf, u32 len, int align, int priv_head_len)
{
    struct hfree *free;
    //首地址4对齐
    struct lbuff_head *head = (struct lbuff_head *)ALIGN(buf, 4);
    //用户使用的地址按照用户的需求进行对齐
    free = (struct hfree *)ALIGN(head + 1, align);
    free->len = len - ((u8 *)free - (u8 *)buf);

    head->align = align;
    head->priv_len = priv_head_len;
    head->last_addr = 0;
#if LBUF_DEBUG
    head->magic_a = 0x87654321;
    head->magic_b = 0x98765432;
#endif
    head->total_size = len;

    spin_lock_init(&head->lock);
    INIT_LIST_HEAD(&head->head);
    INIT_LIST_HEAD(&head->free);

    list_add_tail(&free->entry, &head->free);

    //log_info("0000000000000000lbuf_init: %p,%x\n", head, (u32)head + len);

    return head;
}

int lbuf_avaliable(struct lbuff_head *head, int size)
{
    struct hfree *p;
    int avaliable = 0;

    spin_lock(&head->lock);

    int offset = LBUF_OFFSET(head);
    size = ALIGN(sizeof(struct hentry) + head->priv_len + offset + size, head->align);

    list_for_each_entry(p, &head->free, entry) {
        avaliable += p->len / size;
    }

    spin_unlock(&head->lock);

    return avaliable;
}

void lbuf_state(struct lbuff_head *head, struct lbuff_state *state)
{
    struct hfree *p, *max = NULL;

    state->num = 0;
    state->fragment = 0;
    state->avaliable = 0 ;

    spin_lock(&head->lock);

    list_for_each_entry(p, &head->free, entry) {
        state->fragment ++;
        state->avaliable += p->len;
        state->num++;
//		log_info("lbuf_state: %x, %x\n", p, p->len);
        if (!max || max->len < p->len) {
            max = p;
        }
    }

    if (!max) {
        state->max_continue_len = 0;
    } else {
        int offset = LBUF_OFFSET(head);
        state->max_continue_len = max->len -
                                  sizeof(struct hentry) - head->priv_len - offset;
    }
    spin_unlock(&head->lock);
}


void lbuf_dump(struct lbuff_head *head)
{
    int total_size = 0;
    struct hfree *p, *n;
    struct hentry *h;

    spin_lock(&head->lock);

    list_for_each_entry_safe(p, n, &head->free, entry) {
        /* log_info("fragment: %x, %x\n", (u32)p, p->len); */
        total_size += p->len;
    }

    list_for_each_entry(h, &head->head, entry) {
        /* log_info("alloc: %x, %x, call_from: %x\n", h, h->len, h->rets); */
    }

    log_info("lbuf_state:%x,%x\n", head->total_size, total_size);

    spin_unlock(&head->lock);
}

u32 lbuf_free_space(struct lbuff_head *head)
{
    int max_len = 0;
    struct hfree *p;

    spin_lock(&head->lock);

    ///找到最大的内存块
    list_for_each_entry(p, &head->free, entry) {
        if (max_len < p->len) {
            max_len = p->len;
        }
    }

    spin_unlock(&head->lock);

    int len =  sizeof(struct hentry) + LBUF_OFFSET(head) + head->priv_len;
    if (max_len >= len) {
        max_len -= len;
        max_len &= ~(head->align - 1);
        return max_len;
    }

    return 0;
}

void *lbuf_alloc(struct lbuff_head *head, u32 len)
{
    int offset;
    int max_len = 0;
    void *ret = NULL;
    struct hfree *p;
    struct hfree *new;
    struct hfree *free = NULL, *free_0 = NULL;
    struct hentry *entry;
    u32 rets_addr;
    __asm__ volatile("%0 = rets ;" : "=r"(rets_addr));
    /* printf("lbuf_alloc:0x%x %d\n", rets_addr, len); */

    //计算数据包的最小长度和hentry管理结构体放一起后要对齐的偏移
    offset = LBUF_OFFSET(head);
    //申请的内存需要对齐的长度
    len = ALIGN(sizeof(*entry) + head->priv_len + offset + len, head->align);

    spin_lock(&head->lock);

    list_for_each_entry(p, &head->free, entry) {
        //长度越界检查
        if ((u8 *)p <= (u8 *)head || (u8 *)p > (u8 *)head + head->total_size) {
            asm("trigger");
            log_info("alloc-er1: %x, %x, %x\n", (u32)head->free.next, (u32)p, p->len);
        }
        //在hfree管理块中寻找一块足够长度的内存块
        if (p->len < len) {
            continue;
        }
        //找到了
        if (!free || free->len > p->len) {
            free = p;
        }
        //尽量找一块是在上一次内存分割后面的内存块,避免内存碎片
        if ((u32)p > head->last_addr && !free_0) {
            free_0 = p;
            break;
        }
    }

    p = free_0 ? free_0 : free;

    if (p) {
        head->last_addr = (u32)p;

        //剩余的长度可以进行hfree结构体管理的内存分配
        if (p->len > len + sizeof(struct hfree)) {
            new = (struct hfree *)((u8 *)p + len);
            new->len = p->len - len;
            //hfree管理的内存进行分割
            __list_add(&new->entry, p->entry.prev, p->entry.next);
        } else {
            len = p->len;
            __list_del_entry(&p->entry);
        }

        //进行hentry结构体管理的内存分配
        entry = (struct hentry *)((u8 *)p + offset);
        entry->len = len;
        entry->offset = (u8 *)entry - (u8 *)head;
        entry->channel_map = 0;
        entry->ref = 1;
#if LBUF_DEBUG
        entry->magic_a = 0x12345678;
        entry->magic_b = 0x23456789;
#endif
        INIT_LIST_HEAD(&entry->entry);

        //返回hentry结构体后面的地址进行存储数据包
        ret = entry + 1;
    }

    spin_unlock(&head->lock);

    if (ret == NULL) {
        /*log_info("alloc-err: %x\n", len);*/
        /*putchar('#');*/
    }

    return ret;
}

int lbuf_remain_space(struct lbuff_head *head)
{
    int max_len = 0;
    struct hfree *p;

    spin_lock(&head->lock);

    list_for_each_entry(p, &head->free, entry) {
        max_len += p->len;
    }

    spin_unlock(&head->lock);

    return max_len;
}

void *lbuf_realloc(void *lbuf, int size)
{
    int len;
    int offset;
    int head_len;
    struct hentry *new;
    struct hentry *entry = __get_entry(lbuf);
    struct lbuff_head *head = (struct lbuff_head *)((u8 *)entry - entry->offset);

    lbuf_entry_check(head, entry);
//	log_info("realloc: %x, %x\n", entry->len, size);

    ///把size变成align的倍数
    size = ALIGN(size, head->align);

    ASSERT(size <= entry->len);
    ///不能大于原来的len
    if (size >= entry->len) {
        return NULL;
    }

    offset = LBUF_OFFSET(head);


    ///计算一个数据包的头结构的偏移长度
    head_len = offset + sizeof(*entry) + head->priv_len;

    ///是否能放下两个最小的数据头
    if (entry->len - size < 2 * head_len) {
        return lbuf;
    }
    ///是否能够放下hfree结构体
    if (entry->len - size < sizeof(struct hfree)) {
        return lbuf;
    }

    len = entry->len;
    ///实际的长度为数据头长度+分配的长度
    entry->len = head_len + size;

    new = (struct hentry *)((u8 *)lbuf + head->priv_len + size + offset);
    new->channel_map 	= 0;
    new->ref 	= 1;
    new->len 	= len - entry->len;
    new->offset = (u8 *)new - (u8 *)head;
#if LBUF_DEBUG
    new->magic_a = 0x12345678;
    new->magic_b = 0x23456789;
#endif
    //初始化hentry的内核链表结构体
    INIT_LIST_HEAD(&new->entry);

    ///释放new指向的内存空间,即释放重新分配size大小空间后的剩余的空间
    lbuf_free(new + 1);

    return lbuf;
}


int lbuf_empty(struct lbuff_head *head)
{
    if (list_empty(&head->head)) {
        return 1;
    }
    return 0;
}


void lbuf_clear(struct lbuff_head *head)
{
    struct hentry *p, *n;

    spin_lock(&head->lock);

    list_for_each_entry_safe(p, n, &head->head, entry) {
        lbuf_free(p + 1);
    }

    spin_unlock(&head->lock);
}

int lbuf_real_size(void *lbuf)
{
    struct hentry *entry = __get_entry(lbuf);

    return  entry->len;
}

void lbuf_inc_ref(void *lbuf)
{
    struct hentry *entry = __get_entry(lbuf);

    entry->ref++;
}

void lbuf_push(void *lbuf, u8 channel_map)
{
    int i;
    //根据hentry结构体大小得到hentry结构体地址
    struct hentry *p = __get_entry(lbuf);
    //根据offset得到head结构体地址
    struct lbuff_head *head = (struct lbuff_head *)((u8 *)p - p->offset);

    lbuf_entry_check(head, p);
    ASSERT(channel_map != 0);

    spin_lock(&head->lock);

    ///检测需要被读的次数
    p->ref = 0;
    for (i = 0; i < 8; i++) {
        if (channel_map & BIT(i)) {
            p->ref++;
        }
    }
    p->channel_map = channel_map;
    ///检测hentry管理结构体的内存是否被破坏
    if (list_empty(&p->entry)) {
        lbuf_entry_check(head, p);

        ASSERT(((u32)p->entry.next & 0x03) == 0, "%p,%p", head, p->entry.next);
        ASSERT(((u32)p->entry.prev & 0x03) == 0, "%p,%p", head, p->entry.prev);
        ASSERT(((u32)head->head.prev & 0x03) == 0, "%p,%x,%p", head, head->total_size, head->head.prev);
        ASSERT(((u32)head->head.next & 0x03) == 0, "%p,%x,%p", head, head->total_size, head->head.next);
        ///把hentry链表添加到lbuf_head结构体
        list_add_tail(&p->entry, &head->head);
    }
    lbuf_entry_check(head, p);

    spin_unlock(&head->lock);
}

int lbuf_traversal(struct lbuff_head *head)
{
    struct hentry *p;
    int num = 0;

    spin_lock(&head->lock);
    list_for_each_entry(p, &head->head, entry) {
        num++;
    }
    spin_unlock(&head->lock);
    return num;
}

void *lbuf_pop(struct lbuff_head *head, u8 channel)
{
    struct hentry *p;

    spin_lock(&head->lock);

    //从头进行查找符合通道映射值的hentry内存块
    list_for_each_entry(p, &head->head, entry) {
        if (p->channel_map & channel) {
            //对应的通道映射值置0
            p->channel_map &= ~channel;
            spin_unlock(&head->lock);
            lbuf_entry_check(head, p);
            return p + 1;
        }
    }

    spin_unlock(&head->lock);

    return NULL;
}

void lbuf_free_check(void *lbuf, u32 rets)
{
    struct hentry *entry;
    struct lbuff_head *head;

    if (lbuf == NULL) {
        return;
    }

    entry = __get_entry(lbuf);
    head = (struct lbuff_head *)((u8 *)entry - entry->offset);

    lbuf_entry_check0(head, entry, rets);
}


int lbuf_free(void *lbuf)
{
    int ret = 0;
    int offset;
    struct hfree *p;
    struct hfree *new;
    struct hfree *prev = NULL;
    struct hfree *next;
    struct hentry *entry;
    struct lbuff_head *head;
    u32 rets_addr;
    __asm__ volatile("%0 = rets ;" : "=r"(rets_addr));

    if (lbuf == NULL) {
        return 0;
    }

    ///得到hentry
    entry = __get_entry(lbuf);
    ///得到lbuf_head
    head = (struct lbuff_head *)((u8 *)entry - entry->offset);

    lbuf_entry_check0(head, entry, rets_addr);

    offset = LBUF_OFFSET(head);
    ///得到hfree入口地址并初始化
    new = (struct hfree *)((u8 *)entry - offset);
    int len = entry->len;

    spin_lock(&head->lock);

    ///异常!该数据包的通道映射还没有被读完,ref--后返回
    if (--entry->ref > 0) {
        goto _exit;
    }

#if LBUF_DEBUG
    entry->magic_a = 0x01234567;
#endif

    ///删除lbuf_head的hentry
    __list_del_entry(&entry->entry);
    new->len = len;

    ///hfree轮询
    list_for_each_entry(p, &head->free, entry) {
        ///地址越界检查
        if ((u8 *)p <= (u8 *)head || (u8 *)p > (u8 *)head + head->total_size) {
            asm("trigger");
            log_info("free-err1: %x, %x, %x, %x, %x, %x %x\n", rets_addr, (u32)lbuf, new->len, head, head->free.next, (u32)p, p->len, rets_addr);
            goto _exit;
        }
        if ((p <= new) && ((u8 *)p + p->len > (u8 *)new)) {
            asm("trigger");
            log_info("free-err: %x, %x, %x, %x %x\n", (u32)lbuf, new->len, (u32)p, p->len, rets_addr);
            goto _exit;
        }
        //按地址高低进行排序,把释放的内存块放入hfree链表
        if (p > new) {
            __list_add(&new->entry, p->entry.prev, &p->entry);
            goto __free;
        }
    }
    ///加入hfree链表尾部
    list_add_tail(&new->entry, &head->free);

__free:
    ///得到hfree结构体
    prev = list_entry(new->entry.prev, struct hfree, entry);
    next = list_entry(new->entry.next, struct hfree, entry);
    ///两块紧挨的内存块进行合并
    if ((u32)prev + prev->len == (u32)new) {
        prev->len += new->len;
        __list_del_entry(&new->entry);
        new = prev;
    }
    if ((u32)new + new->len == (u32)next) {
        new->len += next->len;
        __list_del_entry(&next->entry);
    }
    ret = 1;

_exit:
    spin_unlock(&head->lock);
    return ret;
}






