#ifndef __STOPWATCH_FUNC_H__
#define __STOPWATCH_FUNC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

enum
{
    stopwatch_state_reset = 0x00,
    stopwatch_state_pause,
    stopwatch_state_start,
};
typedef uint8_t stopwatch_state_t;

typedef struct
{
    uint8_t minute;
	uint8_t second;
	uint8_t m_second;
}stopwatch_ctx_t;

typedef struct
{
    bool exist;
    stopwatch_ctx_t ctx;
    stopwatch_state_t state;
}StopwatchInfoPara_t;
extern StopwatchInfoPara_t Stopwatch_Info;

void UserStopwatchStart(void);
void UserStopwatchReset(void);
void UserStopwatchPause(void);
void UserStopwatchResume(void);
void UserStopwatchCreate(void);

void UserStopwatchOverHandle(void);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
