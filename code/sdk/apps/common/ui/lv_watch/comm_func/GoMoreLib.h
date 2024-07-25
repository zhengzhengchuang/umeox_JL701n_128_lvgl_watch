#ifndef __GoMoreAlgoHal__
#define __GoMoreAlgoHal__

//#include <stdint.h>

#include "../include/ui_menu.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ***********  fitness api  ************ */
// int16_t getSecondsRecovery(int mElapsedSecond, int *output);
// int16_t getPercentageAfterRecovery(int mElapsedSecond, float *output);
// int16_t getPersonalZone(float *output);

// int16_t startFitness(void* StartFitnessInput);
// int16_t endFitness(void* EndFitnessInput, void* EndFitnessOutput);

// int16_t startFitnessMachineCali(uint8_t type);
// int16_t cancelFitnessMachineCali();

/* ***********  main api  ************ */
int16_t setAuthParameters(void* workoutAuthParameters);
void getWellnessVersion(int* version);
void getReleaseVersion(int* relVer);
uint32_t getPreviousDataSize(void);
uint32_t getMemSizeHealthFrame(void);
int16_t healthIndexInitUser(void* sdkMem, uint32_t rtc, const float *userInfo, char* previousData);
int16_t updateIndex(void *IndexIO);
void stopHealthSession(void* sdkMem);

int16_t switchMode(int mode);

/* ***********  sleep api  ************ */
int16_t setSleepConfig(void* SleepConfig);
int16_t getSleepConfig(int mode, void* SleepConfig);
int16_t startSleepPeriod(int freezeMinutes);
int16_t endSleepPeriod(int mode, void* SleepOutput);
int16_t getEmbeddedSleepSummary(void* SleepSummaryInput, void* SleepSummaryOutput);

int8_t gomore_pkey_get(char *pKey, uint8_t Size);
int8_t gomore_device_id_get(char *Device_ID, uint8_t Size);

/* ***********  health api  ************ */
// int16_t startHealth(void* StartHealthInput);
// int16_t endHealth(void* EndHealthInput, void* EndHealthOutput);

/* ***********  fall detect api  ************ */
// int16_t setFallDetectionMode(uint8_t in);

/* ***********  rest hr api  ************ */
// int16_t setRestHrMode(int mode);
// int16_t setRestHrStartOneTimeMeasurement();

/* ***********  acc-spd cali api  ************ */
// int16_t startAccSpdCali(uint8_t mode);
// int16_t getAccSpdDist(float* out_dist);
// int16_t endAccSpdCali(float in_dist);

/* ***********  all day stamina api  ************ */
// int16_t setAllDayStamina(float ads);

/* ***********  mixed exercise api  ************ */
// int8_t startMixedExerSession(void* SessionInfo);
// int8_t stopMixedExerSession();

/* ***********  women health api  ************ */
// int16_t enableWomenHealth(uint32_t currentTs, uint32_t lastMensPeriodTs);
// int16_t setWomenHealthMensRecord(uint32_t *mensRecords, uint8_t mensRecordsSize);
// int16_t analyzeWomenHealthCycle(void* ModuleWomenHealthInput, void* ModuleWomenHealthOutput);

/* ***********  gesture api  ************ */
// int16_t updateGesture(void* IndexIOGesture);
// int16_t setGestureEnableConfig(uint16_t gestureId, int enableConfig);
// uint8_t setGestureHandTypeConfig(uint8_t handType);

/* ***********  debug api  ************ */
// int16_t setFitnessChecksum(char* checksum, uint16_t checksumLen);
// int16_t getFitnessChecksum(char* checksum, uint16_t* checksumLen);
// int16_t setFitnessHrRest(float value);
// int16_t enableModules(int idxs[], int length);
// int16_t getCleanPPGAndPPIPos(float* ppgBuf, float* cleanPpgBuf, int* ppiBuf);
// int16_t setCompetitorMode(int module, int mode);
// int16_t get_pdc(float out[][2]);

enum LOG_LEVEL {
	LOG_ERR	= 0,
	LOG_INF	= 1,
	LOG_DBG	= 2
};
typedef int ( *printf_func ) ( const char * , ...);
int printf_register(printf_func printFunc);
int enable_Log(int en);
int config_Log(int level);

#ifdef __cplusplus
        }
#endif
#endif /* __GoMoreAlgoHal__ */
