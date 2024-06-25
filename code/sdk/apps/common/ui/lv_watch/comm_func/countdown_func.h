#ifndef __COUNTDOWN_FUNC_H__
#define __COUNTDOWN_FUNC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

enum
{
    countdown_state_reset,
    countdown_state_puase,
    countdown_state_start,
};
typedef uint8_t countdown_state_t;

typedef struct
{
    int16_t percent;
    uint32_t cur_cnt;
    uint32_t total_cnt;
}countdown_ctx_t;

typedef struct
{
    bool exist;
    countdown_ctx_t ctx;
    countdown_state_t state;
}CountdownInfoPara_t;
extern CountdownInfoPara_t Countdown_Info;

int16_t GetPercentMax(void);

void UserCountdownStart(void);
void UserCountdownReset(void);
void UserCountdownPause(void);
void UserCountdownResume(void);
void UserCountdownTimeoutHandle(void);
void UserCountdownCreate(uint32_t cnt);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
