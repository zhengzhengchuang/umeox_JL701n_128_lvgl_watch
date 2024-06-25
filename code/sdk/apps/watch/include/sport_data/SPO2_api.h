#ifndef SPO2_API_H
#define SPO2_API_H

#if 0
#define norm_code  __attribute__((section(".SPO2__norm_code")))
#define spare_const  __attribute__((section(".SPO2_const")))
#endif

int get_SPO2_buf(int len, int fs);
void init_SPO2(void *ptr, int fs, int len);
void SPO2_run(void *ptr, int len, int *data, int *out);
int getCurrentSPO2(void *ptr);




#endif
