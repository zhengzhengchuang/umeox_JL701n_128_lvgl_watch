#ifndef _MEM_HEAP_H_
#define _MEM_HEAP_H_

#ifdef __cplusplus
extern "C" {
#endif

extern void *malloc(unsigned long size);
extern void *zalloc(unsigned long size);
extern void *calloc(unsigned long count, unsigned long size);
extern void *realloc(void *rmem, unsigned long newsize);
extern void  free(void *mem);


extern void *kmalloc(unsigned long size, int flags);
extern void *vmalloc(unsigned long size);
extern void vfree(void *addr);
extern void *kzalloc(unsigned int len, int a);
extern void kfree(void *p);

extern void malloc_stats(void);

extern void malloc_dump();


#ifdef __cplusplus
}
#endif

#endif /* _MEM_HEAP_H_ */
