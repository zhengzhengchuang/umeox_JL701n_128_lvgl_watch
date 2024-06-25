#ifndef __TIMER_TICK_H__
#define __TIMER_TICK_H__

/*不同功能的循环检测功能统一用LOOP_DETECT_REGISTER注册，可搜索关键字看例子*/
struct loop_detect_handler {
    int time;/*TIMER周期的倍数。timer周期默认一般是2ms */
    int *priv;
    void (*fun)(int *);
};

extern struct loop_detect_handler loop_detect_handler_begin[];
extern struct loop_detect_handler loop_detect_handler_end[];

#define LOOP_DETECT_REGISTER( p, t, fn) \
 	static const struct loop_detect_handler __event_handler_##fn SEC_USED(.loop_detect_region) = { \
		.time = (t), \
        .priv = (p), \
		.fun = (fn), \
	}\


#define list_for_each_loop_detect(h) \
	for (h=loop_detect_handler_begin; h<loop_detect_handler_end; h++)

extern u32  get_cur_sys_tick(void);
extern void timer_tick_init();

#endif
