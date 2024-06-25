#include "lib_include.h"
#include "circular_buf.h"
#if 1
#define  CPU_SR_ALLOC()

//#pragma code_seg(".cbuf_code")

u32 cbuf_read(cbuffer_t *cbuffer, void *buf, u32 len)
{
    CPU_SR_ALLOC();
    u32 r_len = len;
    u32 copy_len;

    if (!cbuffer) {
        return 0;
    }

    if ((u32)cbuffer->read_ptr >= (u32)cbuffer->end) {
        cbuffer->read_ptr = (u8 *)cbuffer->begin;
    }

    if (cbuffer->data_len < len) {
        /* memset(buf, 0, len); */
        return 0;
    }


    copy_len = (u32)cbuffer->end - (u32)cbuffer->read_ptr;
    if (copy_len > len) {
        copy_len = len;
    }
    len -= copy_len;

    memcpy(buf, cbuffer->read_ptr, copy_len);
    //printf_data(cbuffer->read_ptr,copy_len) ;

    if (len == 0) {
        cbuffer->read_ptr += copy_len;
    } else {
        memcpy((u8 *)buf + copy_len, cbuffer->begin, len);
        //printf_data(cbuffer->begin,len);
        cbuffer->read_ptr = cbuffer->begin + len;
    }

    local_irq_disable();
    cbuffer->tmp_len = cbuffer->data_len -= r_len;

    cbuffer->tmp_len = cbuffer->data_len;

    local_irq_enable();

    return r_len;
}

u32 cbuf_is_write_able(cbuffer_t *cbuffer, u32 len)
{
    u32 w_len;

    if (!cbuffer) {
        return 0;
    }
    w_len = cbuffer->total_len - cbuffer->data_len;
    if (w_len < len) {
        return 0;
    }

    return w_len;
}


u32 cbuf_write(cbuffer_t *cbuffer, void *buf, u32 len)
{
    CPU_SR_ALLOC();
    u32 length;
    u32 remain_len;

    if (!cbuffer) {
        return 0;
    }

    if ((cbuffer->total_len - cbuffer->data_len) < len) {
        len = cbuffer->total_len - cbuffer->data_len ;
        if (len == 0) {
            return 0;
        }
    }


    length = (u32)cbuffer->end - (u32)cbuffer->write_ptr;
    if (length >= len) {
        memcpy(cbuffer->write_ptr, buf, len);
        cbuffer->write_ptr += len;
    } else {
        remain_len = len - length;
        memcpy(cbuffer->write_ptr, buf, length);
        memcpy(cbuffer->begin, ((u8 *)buf) + length, remain_len);
        cbuffer->write_ptr = (u8 *)cbuffer->begin + remain_len;
    }

    local_irq_disable();
    cbuffer->data_len += len;
    cbuffer->tmp_len = cbuffer->data_len ;
    cbuffer->tmp_ptr = cbuffer->write_ptr ;
    local_irq_enable();

    return len;
}

void cbuf_init(cbuffer_t *cbuffer, void *buf, u32 size)
{
    cbuffer->data_len = 0;
    cbuffer->tmp_len = 0 ;
    cbuffer->begin = buf;
    cbuffer->read_ptr = buf;
    cbuffer->write_ptr = buf;
    cbuffer->tmp_ptr = buf;
    cbuffer->end = (u8 *)buf + size;
    cbuffer->total_len = size;
}

void cbuf_clear(cbuffer_t *cbuffer)
{
    CPU_SR_ALLOC();
    local_irq_disable();
    cbuffer->read_ptr = cbuffer->begin;
    cbuffer->tmp_ptr = cbuffer->write_ptr = cbuffer->begin;
    cbuffer->data_len = 0;
    cbuffer->tmp_len = 0 ;
    local_irq_enable();
}

u32 cbuf_get_data_size(cbuffer_t *cbuffer)
{
    //printf(">>cbuf_dat_len:%x\n",cbuffer->data_len);
    return cbuffer->data_len;
}

#define CBUF_ENTER_CRITICAL local_irq_disable
#define CBUF_EXIT_CRITICAL	local_irq_enable

void cbuf_read_alloc_len_updata(cbuffer_t *cbuffer, u32 len)
{
    CBUF_ENTER_CRITICAL();

    cbuffer->read_ptr += len;
    if ((u32)cbuffer->read_ptr >= (u32)cbuffer->end) {
        cbuffer->read_ptr = (u8 *)cbuffer->begin + ((u32)cbuffer->read_ptr - (u32)cbuffer->end);
    }

    cbuffer->tmp_len =  cbuffer->data_len -= len;

    CBUF_EXIT_CRITICAL();
}
u32 cbuf_read_alloc_len(cbuffer_t *cbuffer, void *buf, u32 len)
{
    u32 r_len = len;
    u32 copy_len;

    if (!cbuffer) {
        return 0;
    }

    if ((u32)cbuffer->read_ptr >= (u32)cbuffer->end) {
        cbuffer->read_ptr = (u8 *)cbuffer->begin;
    }

    if (cbuffer->data_len < len) {
        /* memset(buf, 0, len); */
        return 0;
    }


    copy_len = (u32)cbuffer->end - (u32)cbuffer->read_ptr;
    if (copy_len > len) {
        copy_len = len;
    }
    len -= copy_len;

    memcpy(buf, cbuffer->read_ptr, copy_len);
    //printf_data(cbuffer->read_ptr,copy_len) ;

    if (len == 0) {
        /* cbuffer->read_ptr += copy_len; */
    } else {
        memcpy((u8 *)buf + copy_len, cbuffer->begin, len);
        //printf_data(cbuffer->begin,len);
        /* cbuffer->read_ptr = cbuffer->begin + len; */
    }

    return r_len;
}

#endif
