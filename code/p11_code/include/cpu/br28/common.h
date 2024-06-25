#ifndef _COMMON_H_
#define _COMMON_H_

void delay(u32 cnt);
void udelay(u32 us);
void mdelay(u32 ms);
void debug_io_1(void);
void debug_io_0(void);
void debug_io(void);


#define ASSERT(a,...)   \
    do { \
        if(!(a)){ \
            printf("file:%s, line:%d", __FILE__, __LINE__); \
            printf("ASSERT-FAILD: "#a" "__VA_ARGS__); \
            while(1); \
        } \
    }while(0);




#endif /* #ifndef _COMMON_H_ */
