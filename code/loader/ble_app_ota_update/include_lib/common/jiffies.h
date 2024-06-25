#ifndef JIFFIES_H
#define JIFFIES_H

#define HZ				100L
#define MSEC_PER_SEC	1000L
#define USEC_PER_MSEC	1000L
#define NSEC_PER_USEC	1000L
#define NSEC_PER_MSEC	1000000L
#define USEC_PER_SEC	1000000L
#define NSEC_PER_SEC	1000000000L
#define FSEC_PER_SEC	1000000000000000LL


#ifndef __ASSEMBLY__
extern volatile unsigned long jiffies;
extern unsigned long jiffies_msec();
extern unsigned long jiffies_half_msec();
#endif

#define JIFFIES_CIRCLE                  0x7FFFFFF


#define time_after_bt(a,b)						 ((long)(b) - (long)(a) < 0)
#define time_before_bt(a,b)			     time_after_bt(b, a)


#define msecs_to_jiffies_2(msec)       ((msec)/2)

#define msecs_to_jiffies_10(msec)       ((msec)/10)
//#define msecs_to_jiffies_100(msec)       ((msec)/100)
#define jiffies_to_msecs(j)       ((j)*10)

#define jiffies_to_msecs_10(j)     (((j)*625UL)/10000)

#define msecs_to_jiffies(msec)       	((msec)/10)
#define time_after(a,b)					((long)(b) - (long)(a) < 0)
#define time_before(a,b)			     time_after(b, a)

#endif


