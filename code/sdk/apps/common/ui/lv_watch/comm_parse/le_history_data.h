#ifndef __LE_HISTORY_DATA_H__
#define __LE_HISTORY_DATA_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../include/ui_menu.h"

void HistoryHrDataSend(int cmd, int year, int month, int day);
void HistoryBoDataSend(int cmd, int year, int month, int day);
void HistoryPedoDataSend(int cmd, int year, int month, int day);
void HistorySleepDataSend(int cmd, int year, int month, int day);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif 
