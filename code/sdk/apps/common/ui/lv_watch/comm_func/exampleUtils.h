#include "GoMoreLib.h"
#include "GoMoreLibStruct.h"
#include "workout.h"
#include <string.h>
#ifndef EXAMPLE_UTILS_H
#define EXAMPLE_UTILS_H

/* example device id */
static char* deviceIdExample = "F08968476615";
// the first one checksum buffer for verify client_id/device_id/expriation_time
static char *pKeyExample = (char*)"BR4lmTwnTku6kf8e8TNDqk356KZAtS+Qv2Log0k5bGpydVc1dQ868i2bRWHqvWBV";

static void setWorkoutAuthParametersExample(int rtc_current_time, workoutAuthParameters *auth) {
	memset(auth, 0, sizeof(workoutAuthParameters));
	auth->deviceId = deviceIdExample;
	auth->devIdLen = strlen(auth->deviceId);
    auth->pKey = pKeyExample;
	auth->rtcCurrentTime = rtc_current_time;
}

static void setIndexIODefaultValue(IndexIO* mInput) {
    memset(mInput, 0, sizeof(IndexIO));
    // mInput->speedRef = -1.f;// @updateIndexIO
    // mInput->latitude = -999.f;// @updateIndexIO
    // mInput->longitude = -999.f;// @updateIndexIO
    // mInput->altitude = -999.f;// @updateIndexIO
    // mInput->gpsSpeed = -1.f;// @updateIndexIO
    // mInput->gpsAccuracy = -1.f;// @updateIndexIO
    // mInput->cyclingPower = -1.f;// @updateIndexIO
    // mInput->cyclingCadence = -1.f;// @updateIndexIO
}
#endif
