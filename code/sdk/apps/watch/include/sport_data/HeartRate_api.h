#ifndef HEARTRATE_API_H
#define HEARTRATE_API_H

#if 0
#define spare_code __attribute__((section(".heart_sparse_code")))
#define norm_code  __attribute__((section(".heart_code")))
#define spare_const  __attribute__((section(".heart_const")))
#endif

int get_HeartRate_buf(int len, int fs);
void init_HeartRate(void *ptr, int fs, int len);
int get_weardect_buf(int len);
void init_WearDect(void *ptr, int fs, int len);
void wear_detection(void *ptr, int len, int *data, int *out);
void HeartRate_run(void *ptr1, void *ptr2, int len, int *out);
int getCurrentHR(void *ptr);


#endif
