#ifndef _MEM_HEAP_H_
#define _MEM_HEAP_H_

#ifdef __cplusplus
extern "C" {
#endif

extern void *malloc(size_t size);
extern void *zalloc(size_t size);
extern void *calloc(size_t count, size_t size);
extern void *realloc(void *rmem, size_t newsize);
extern void  free(void *mem);


extern void *kmalloc(size_t size, int flags);
extern void *vmalloc(size_t size);
extern void vfree(void *addr);
extern void *kzalloc(unsigned int len, int a);
extern void kfree(void *p);

extern void malloc_stats(void);

extern void malloc_dump();

void memory_init(void);

void mem_stats(void);

extern void *phy_malloc(size_t size);
extern void phy_free(void *pv);
extern void *dma_malloc(size_t size);
extern void dma_free(void *pv);
extern int memory_in_phy(const void *pv);
extern int memory_in_vtl(const void *pv);
extern u8 memory_defrag(void);
extern u32 pmalloc_get_unused_addr();
extern u16 addr_map_2_ram(u32 start);
extern void save_limit_h_ram(void);
extern void recover_limit_h_ram(void);




#ifdef __cplusplus
}
#endif

#endif /* _MEM_HEAP_H_ */
