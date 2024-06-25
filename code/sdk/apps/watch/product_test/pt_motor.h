#ifndef __PRODUCT_TEST_MOTOR_H
#define __PRODUCT_TEST_MOTOR_H

// 振动类型
#define PT_MOTOR_TYPE_LOOP		0 // 循环
#define PT_MOTOR_TYPE_SPEC		1 // 固定

struct pt_motor {
    u8  type;		// 振动类型
    u8  loop_cnt;	// 循环次数
    u16 step; 		// 振动步伐，单位：ms
    u16 level_cur; 	// 振动当前等级
    u16 level_max; 	// 振动最大等级
};

struct pt_motor_result {
    u32 result;	// 错误值
    struct pt_motor motor;
};

int pt_motor_getinfo(struct pt_motor_result *);
int pt_motor_setinfo(struct pt_motor *);

int pt_motor_start(void);
int pt_motor_stop(void);

#endif // #define __PRODUCT_TEST_MOTOR_H

