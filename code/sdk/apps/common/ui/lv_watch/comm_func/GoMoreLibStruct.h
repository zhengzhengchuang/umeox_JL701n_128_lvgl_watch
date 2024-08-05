#ifndef __GoMoreAlgoHalStruct__
#define __GoMoreAlgoHalStruct__

//#include <stdint.h>
#include "../include/ui_menu.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ***********  updateIndex  ************ */
// UpdateFitnessOutput
union UpdateWorkoutOutput {
    uint8_t padding;
//     struct UpdateSwimOutput{
//         int startIndex;
//         int endIndex;
//         uint16_t cntStroke;
//         uint8_t updateType;
//         uint8_t frontcrawlPerc;
//         uint8_t breaststrokePerc;
//         uint8_t backstrokePerc;
//         uint8_t butterflyPerc;
//         uint8_t updateStatus;
//     } swim;
// 	struct UpdateSwimEarOutput {
// 		int startIndex;
// 		int endIndex;
// 		uint16_t cntLap;
// 		float speedPerLap;
// 		uint8_t distance;
// 		// uint8_t updateType;
// 		uint8_t updateStatus;
// 	} swimEar;
//     // Run Hand
//     struct UpdateRunOutput{
//         // run stance, old module output
//         float orgCadence;
//         float orgStance;
//         float orgStaBalance;

//         // run form
//         float cadence;
// 	    int stepCnt;
//         float stance;
//         float flight;
// 	    float staFlightRatio;
//         float staBalance;
//         float verticalOscillation;
//         float runPowerWatt;
//         float stepLen;
//         float strideLen;

//         // run summary
//         int totalStep;
//         float pace;
//         float maxPace;
//         float minPace;
//         float avgPace;
//         float avgSpeed;
//         float avgCadence;
//         float distance;
//     } run;
//     struct UpdateJumpRopeOutput{
//         int totalCount;
//         int tripCount;
//         int sessionCount;
//     } jumpRope;
//     struct UpdateSkiingOutput{
//         float avgSpeed;
//         float maxSpeed;
//         float distance;
//     } skiing;
//     struct UpdateSoccerOutput{
//         float avgSpeed;
//         float maxSpeed;
//         float distance;
//     } soccer;
//     struct UpdateBaseballOutput{
//         int count;
//     } baseball;
//     struct UpdateGolfOutput{
//         int backswing;
//         int downswing;
//         float tempo;
//         int count;
//     } golf;
//     struct UpdateRowingOutput{
//         float cadence;
//         uint32_t rowCnt;
//     } row;
//     struct UpdateTabletennisOutput{
//         uint32_t cntFore;
//         uint32_t cntBack;
//         uint32_t cntTotal;
//     } tabletennis;
//     // Run Foot
//     struct UpdateRunFormFootOutput{
//         int stepCntTotal;
//         float cadence;
//         float stance;
//         float flight;
//         float stanceFlightRatio;
//         float speed;                // kph
//         float distance;
//         float stepLen;
//         float cadencePaceRatio;
//         float pace;                 // unit: minute per km
        
//         float groundImpact;
//         uint8_t footStrikeMajor; // 0/1/2:forefoot/midfoot/heel
//         float eversionDegree;

//         float staBalance;
//         float runPowerWatt;
//     } runFormFoot;
//     struct UpdateCyclingFormOutput{
//         float cadence;
//     } cyclingForm;
//     struct UpdateEllipticalOutput{
//         float cadence;
//         int steps;
//     } elliptical;
//     struct UpdateDumbbellOutput{
//         int dumbbellCount;
//     } dumbbell;
//     struct UpdateBoxingOutput{
//         uint16_t punchCount;
//     } boxing;
//     struct UpdateStaticBalanceOutput{
//         uint8_t level;
//         uint8_t status;
//     } staticBalance;
//     struct UpdateBodyweightTrainingOutput{
//         uint32_t count;
//     } bodyweightTraining;
//     struct UpdateBasketballOutput{
//         int shotCounts;
//         float dribbleTime;
//         uint8_t dribbleSectionStatus;
//     } basketball;
//     struct UpdateFrisbeeOutput{
//         uint32_t count;
//     } frisbee;
//     struct UpdateCricketOutput{
//         int count;
//     } cricket;
//     struct UpdateSquashOutput{
//         int cntFore;
//         int cntBack;
// 		int cntSwing;
//     } squash;
//     struct UpdateTennisOutput{
//         int cntFore;
//         int cntBack;
// 		int cntSwing;
//     } tennis;
//     struct UpdateBadmintonOutput{
//         int cntFore;
//         int cntBack;
// 		int cntSwing;
//     } badminton;
//     // Run Ear
//     struct UpdateRunFormEarOutput{
//         int stepCntTotal;
//         float cadence;
//         float stance;
//         float flight;
//         float distance;
//         float stepLen;
//         // float staBalance;
//         float verticalOscillation;
//         float speed;
//         float pace;
//         float sta_fli_ratio;
//         float cad_pace_ratio;
//         float power;
//     } runFormEar;
};

// typedef struct mFitnessUpdate{
//     float aerobic;
//     float anaerobic;
//     float stamina;
//     float teAerobic;
//     float teAnaerobic;
//     float teStamina;
//     float hrTeA;
//     float hrTeAn;
//     int filterHr;
//     int hrZone;
// 	int16_t fitnessStatus;
// } mFitnessUpdate;

struct UpdateFitnessOutput {
    uint8_t padding;
//     union UpdateWorkoutOutput workout;
//     struct mFitnessUpdate fitness;
};

typedef struct IndexIO {
    // Input
    uint32_t timestamp;
    int16_t timeZoneOffset;
    float* ppg1;
    // float* ppg2;
    // float* ppg3;
    // uint16_t* ppiRef;
    float* accX;
    float* accY;
    float* accZ;
    // float* gyroX;
    // float* gyroY;
    // float* gyroZ;
    // float* ecg;
    // float* gsr;
    uint8_t ppgNumChannels;
    uint8_t ppgLength;
    // uint8_t ppiSize;
    uint8_t accLength;
    // uint8_t gyroLength;
    // uint8_t ecgLength;
    // uint8_t gsrLength;
    // float longitude;
    // float latitude;
    // float altitude;
    // float gpsSpeed;
    // float gpsAccuracy;
    // float incline;
    float hrRef;
    // float speedRef;
    // float cyclingCadence;
    // float cyclingPower;
    // float cyclingResistance;
    // float bodyTemperature;
    // int8_t sleepStage;
    // int8_t sleepStageStatus;
    // float spo2;
    // float* biaI;
    // float* biaQ;
    // int8_t* biaFlag;
    // uint8_t biaLength;

    // Output
    // float hrOut;
    // float paceOut;
    // float speedOut;
    float stepCountOut;
    // float stressOut;
    // float allDayStaminaOut;
    // float vo2maxOut;
    float kcalOut;
    // float METOut;
    // float fatOut;
    // float carbOut;
    // float METsOut[5];
    // float staticKcalOut;
    float activityKcalOut;
    // uint8_t activityScoreOut; // range 0 ~ 100
    // uint8_t activityScoreStatusOut;
    float rrOut;
    float rrConfOut;
    // float hrRestOut;
    // uint8_t hrRestEstStatusOut;
    // float hrvOut;
    // float actCountOut;
    // float speedGpsOut;
    // float baseHrvOut;
    // float baseHrOut;
    // int8_t speedStatusOut;
    // int8_t speedSourceOut;
    // int8_t activeTypeOut;
    uint8_t cadenceOut;
    // uint8_t pqiFreqOut;
    // uint8_t pqiTimeOut;
    // uint8_t pqiStatusOut;
    // uint8_t ppgHrFlagOut;
    
    // uint8_t accStageOut;

    // uint8_t sleepPeakOut[16];
    // uint8_t sleepPeakSizeOut;
    // uint8_t sleepPeakStatusOut;

    int8_t sleepStageOut;
    int8_t sleepStageStatusOut;
    int8_t sleepStagePpgOnOffOut;
    // int8_t sleepMovementLevelOut;       // 0 ~ 3: No / Low / Medium / Intense Movement
    // int8_t sleepMovementStatusOut;      // 0: No Output, 1: Output Level

    uint32_t sleepPeriodStartTSOut;
    uint32_t sleepPeriodEndTSOut;
    uint8_t sleepPeriodStatusOut;
    uint8_t sleepPeriodStateLogOut;
    uint8_t sleepPeriodPeriodLogOut;
    uint8_t sleepPeriodCombineLogOut;

    // uint8_t sleepFeatOut[40];
    // uint8_t sleepFeatSizeOut;

    // uint32_t allDayStepCntOut;
    uint16_t allDayKcalOut;
    // uint16_t allDayActKcalOut;
    // float allDayDistanceOut;

    // Fitness Summary Output
    // uint32_t hrZoneCountOut[5];
    // uint16_t fitnessKcalOut;
    // uint32_t fitnessStepCntOut;
    // float maxSpeedOut;
    // float avgSpeedOut;
    // float maxPaceOut;
    // float avgPaceOut;
    // float maxCadenceOut;
    // float avgCadenceOut;
    // float totalDistanceOut;

    // Fitness Output

	//Fitness machine cali out
	// int16_t calibStatusOut;

    // int avgHrOut;
    // float avgSpdOut;
    // int avgCadenceOut;
    // int totalStepOut;
    // int durationOut;

    // Fall Detect Output
    // uint8_t fallDetectOut;


    // Mixed Exercise Training
    // uint8_t metStatusOut;
    // uint8_t metRepsOut[40];
    // float   metKcalOut;
    // uint8_t metRemainingStepSecsOut;
    // uint8_t metCompletionRateOut;
    // uint8_t metEffortsOut[40];
    // uint16_t metTrainingLoadOut[4]; // cardio, upper, lower, core
    // uint8_t metPerformanceOut[2]; // cardio, muscular
    // uint8_t metAasOut;
    // uint8_t metChecksumOut[23+6];
    
    // Chronotype
    // float chronotypeFeatOut;
    // uint8_t chronotypeFeatStatusOut;
    uint8_t chronotypeOut;
    uint8_t chronotypeConfOut;
    int8_t chronotypeIdealbedOut;
    uint8_t chronotypeStatusOut;
    uint8_t chronotypeNdayOut;

    // Ppi from PPG-HR
    // uint16_t ppiPpgOut[40];
    // uint8_t ppiPpgSizeOut;

    // Ppi Td
    // uint16_t ppiTdOut[120];
    // uint8_t ppiTdBeatNumOut;
    // uint8_t ppiTdStatusOut;

    // Ppg Afib
    // uint8_t ppgAfibClassOut;
    // uint8_t ppgAfibConfOut;
    // uint8_t ppgAfibStatusOut;

    // Ecg Afib
    // uint8_t ecgAfibClassOut;
    // uint8_t ecgAfibConfOut;
    // uint8_t ecgAfibStatusOut;

    // RRI
    // uint16_t rriOut[120];
    // uint8_t rriBeatNumOut;
    // uint8_t rriStatusOut;

    // Ecg Det
    // int8_t ecgDetStatusOut;
    // int16_t ecgDetSigPeakOut[80]; //R peak
    // uint8_t ecgDetSigPeakLenOut;
    // int16_t ecgDetPPeakOut[80]; //P peak
    // uint8_t ecgDetPPeakLenOut;
    // int16_t ecgDetTPeakOut[80]; //T peak
    // uint8_t ecgDetTPeakLenOut;

    // Wrist On Sleep
    // uint8_t wristOnSleepOut;
    // uint8_t wristOnSleepPpgOnOffOut;

    // Fitness
    // struct UpdateFitnessOutput fitnessOut;

    // workout auto detection (15 min+)
    // uint8_t workoutSummaryStatusOut; // 1: output workout summary
    // uint32_t workoutStartTSOut;
    // uint32_t workoutDurationOut; // seconds
    // uint32_t workoutStepOut;
    // uint8_t workoutIntensityOut; // 0: Low, 1: Medium, 2: High
    // uint8_t workoutTypeOut; // 0: undefined, 1: walk, 2: run
    // float workoutKcalOut;

    // OSA
    // uint8_t osaOut; 
    // uint8_t osaSpo2ConfOut; 
    // uint8_t osaStatusOut;

    // GSR
    // uint8_t gsrStressOut;
    // uint8_t gsrEmotionOut;
	// uint8_t gsrConfOut;
    // uint8_t gsrStatusOut;

    // fat percentage
    // float fatPercentageOut;
    // float fatPercentageAgeOut;
    // uint8_t fatPercentageStatusOut;

} IndexIO;

/* ***********  startFitness  ************ */
// StartFitnessInput
union StartWorkoutInput{
    uint8_t padding;
//     struct StartSwimInput {
//         uint8_t handFlag;
//         uint8_t poolSize;                   // unit: meter
//     } swim;
// 	struct StartSwimEarInput {
// 		uint8_t poolSize;                   // unit: meter
// 	} swimEar;
//     // Run Hand
//     struct StartRunInput {
//         uint8_t handFlag;
//         uint8_t gender;
//         float weight;
//         float height;
//     } run;
//     struct StartSoccerInput {
//         uint8_t signalSource;               // 0: Gps Only, 1: Gps + Acc
//     } soccer;
//     struct StartRunFormFootInput {
//     // Run Foot
//         uint8_t footRLFlag;
//         float weight;
//         int stanceFlag;
//     } runFormFoot;
//     struct StartBodyweightTrainingInput {
//         uint8_t movement;
//     } bodyweightTraining;
//     struct StartSquashInput {
//         uint8_t handFlag;
//     } squash;
//     struct StartTennisInput {
//         uint8_t handFlag;
//     } tennis;
//     struct StartBadmintonInput {
//         uint8_t handFlag;
//     } badminton;
//     struct StartTabletennisInput {
//         uint8_t racketType;
//     } tabletennis;
//     struct StartDumbbellInput {
//         uint8_t workoutType; //1:Biceps curl, 2:Shoulder press or bench press
//     } dumbbell;
// 	// Run Ear
//     struct StartRunFormEarInput {
//         uint8_t earRLFlag;
//         uint8_t spInputFlag;
//         uint8_t height_cm;
// 	    uint8_t weight_kg;
//     } runFormEar;
};

typedef struct mFitnessStart {
    uint32_t rtc;
	int16_t timeZoneOffset;
    char *trainingInfo;
} mFitnessStart;

struct StartFitnessInput {
    int32_t workoutType;
    union StartWorkoutInput workout;
    // struct mFitnessStart fitness;
};

// /* ***********  endFitness  ************ */
// // EndFitnessInput
union EndWorkoutInput {
    uint8_t tmp;
};

// typedef struct mFitnessInfo {
//     uint32_t workoutDate[1];
// 	int8_t hisOP;
// } mFitnessInfo;

// struct EndFitnessInput {
//     union EndWorkoutInput workout;
//     struct mFitnessInfo fitness;
// };

// // EndFitnessOutput
union EndWorkoutOutput{
    uint8_t padding;
//     struct EndSwimOutput{
//         int startIndex;
//         int endIndex;
//         uint16_t cntStroke;
//         uint8_t type;
//         uint8_t frontcrawlPerc;
//         uint8_t breaststrokePerc;
//         uint8_t backstrokePerc;
//         uint8_t butterflyPerc;
//         uint8_t status;

//         // Summary
//         uint16_t strokeCnt;
//         float strokePerMin;
//         float strokePerLap;
//         float swolf;
//         uint8_t cntLap;
//         uint32_t swimDur;
//     } swim;
// 	struct EndSwimEarOutput {
// 		int startIndex;
// 		int endIndex;
// 		// uint8_t swim_type;
// 		uint8_t status;

// 		// Summary
// 		float speedPerLap;
// 		uint8_t distance;
// 		uint8_t cntLap;
// 	} swimEar;
//     struct EndCmjPredictOutput{
//         float slj;
//         float sprint50;
//         uint8_t status;
//     } cmjPredict;
//     // Run Hand
//     struct EndRunFormHandOutput{
//         float meanCadence;
//         float meanStrideLen;
//         float meanStance;
//         float meanFlight;
//         float meanPower;
//         int totalStepCount;
//     } runFormHand;
//     // Run Foot
//     struct EndRunFormFootOutput{
//         float meanCadence;
//         float maxCadence;
//         float meanStepLen;
//         float meanGroundImpact;
//         float maxGroundImpact;
//         float meanStance;
//         float minStance;
//         float meanFlight;
//         float maxFlight;
//         float meanStanceFlightRatio;
//         float minStanceFlightRatio;
//         float meanPace;              // unit: minute per km
//         float maxPace;               // unit: minute per km
//         uint8_t landingFormMajor;  // 1/2/3:toes/sole/heel
//     } runFormFoot;
//     struct EndCyclingFormOutput {
//         float meanCadence;
//         float maxCadence;
//     } cyclingForm;
// 	// Run Ear
//     struct EndRunFormEarOutput{
//         float meanCadence;
//         float meanStance;
//         float meanVerticalOscillation;
//         float meanStepLen;
//         float meanBalance;
//         float meanFlight;
//     } runFormEar;
};
// typedef struct mFitnessSummary {
//     float runLevelOut[9];
//     float vValueOut[2];
//     float pace[2];
//     float cyclingLevelOut[9];
//     float cValueOut[2];
//     float cyclingIndex[2];  /* [0]:ftp [1]:powerPeak */
//     float efficiency[2];
//     int recoveryTime[1];
// 	float personalZone[7];
//     int bestRunTime[11];
//     uint16_t trainingLoad[1];
//     float tlTrend[1];
//     int8_t trainingStatus[1];
// 	float  trainingLoadBound[2];
// 	int8_t trainingLoadWeekTrend[1];
// 	float  weekLoad[1];
//     int8_t fitnessAge[2];
//     float hrEstorOut[1];
//     char trainChecksum[128];
//     uint16_t trainChecksumLen[1];
// 	float pMAP[8];
// 	int   dMAP[8];
// } mFitnessSummary;

// struct EndFitnessOutput {
//     union EndWorkoutOutput workout;
//     struct mFitnessSummary fitness;
// };

// /* ***********  startHealth  ************ */
typedef union StartActionInput {
    uint8_t padding; // No functional purpose, it is only included to prevent empty structures or unions.
    // struct StartBodyCompositionInput {
    //     float weight;
    //     float height;
    // } bodyComposition;
} StartInput;

typedef struct StartHealthInput {
    int32_t actionType;
    union StartActionInput action;
} StartHealthInput;

// /* ***********  endHealth  ************ */
typedef union EndActionInput {
    uint8_t padding; // No functional purpose, it is only included to prevent empty structures or unions.
    // struct EndBodyCompositionInput {
    //     int success;
    // } bodyComposition;
} EndInput;

typedef struct EndHealthInput {
    uint8_t padding; // No functional purpose, it is only included to prevent empty structures or unions.
    // union EndActionInput action;
} EndHealthInput;

typedef union EndActionOutput {
    uint8_t padding; // No functional purpose, it is only included to prevent empty structures or unions.
    // struct EndBodyCompositionOutput{
    //     float bmr;          // unit: kcal
    //     float water;        // unit: l
    //     float protein;      // unit: kg
    //     float minerals;     // unit: kg
    //     float fat;          // unit: kg
    //     float smm;          // unit: kg
    //     float ffm;          // unit: kg
    //     uint8_t status;
    // } bodyComposition;
} EndOutput;

typedef struct EndHealthOutput {
    uint8_t padding; // No functional purpose, it is only included to prevent empty structures or unions.
    // union EndActionOutput action;
} EndHealthOutput;

/* ***********  setAuthParameters  ************ */
typedef struct workoutAuthParameters { 
    const char *pKey; 
    /* Base64 decoded pkey. This key gets from 
       GoMore system by calling API or provided by 
       GoMore administrator. */ 
    const char *deviceId; 
    /* Device ID registered in GoMore system. */ 
    uint8_t devIdLen; 
    /* Length of device ID. */ 
    uint32_t rtcCurrentTime;
    /* RTC time of the device. (Unix timestamp) */
} workoutAuthParameters;

/* ***********  chronotype  ************ */
typedef struct ChronotypeOutputManualEndSleep {
    uint8_t manualEndSleepChronotypeOut;
    uint8_t manualEndSleepChronotypeConfOut;
    int8_t manualEndSleepChronotypeIdealbedOut;
    uint8_t manualEndSleepChronotypeStatusOut;
    uint8_t manualEndSleepChronotypeNdayOut;
} ChronotypeOutputManualEndSleep;

// typedef struct OsaOutputManualEndSleep {
//     uint8_t manualEndSleepOsaOut;
//     uint8_t manualEndSleepOsaSpo2ConfOut;
//     uint8_t manualEndSleepOsaStatusOut;
// } OsaOutputManualEndSleep;

/* ***********  setSleepConfig  ************ */
typedef struct SleepConfig {
    int16_t minNapLength;               // unit: minute, range: [20, 60], default: 20
    int16_t maxGapLength;               // unit: minute, range: [0, 240], default: 120
    uint8_t disablePeriodFinetuning;    // default: 0 (disable period finetuning or not)
    uint8_t disableAutoDetection;       // default: 0 (disable auto detection or not)
    uint8_t longSleepPeriod[2];         // unit: hour, range: [0, 23], default: {20, 8}
} SleepConfig;

/* ***********  endSleepPeriod  ************ */
typedef struct SleepOutput {
    uint32_t sleepPeriodStartTSOut;
    uint32_t sleepPeriodEndTSOut;
    uint8_t sleepPeriodStatusOut;
    uint8_t sleepPeriodStateLogOut;
    uint8_t sleepPeriodPeriodLogOut;
    uint8_t sleepPeriodCombineLogOut;
    // uint8_t sleepFeatOut[40];
    // uint8_t sleepFeatSizeOut;
    
    //manual endSleepPeriod(MES)
    struct ChronotypeOutputManualEndSleep MESchronoOut;
    // struct OsaOutputManualEndSleep MESosaOut;
} SleepOutput;


/* ***********  getSleepSummary ************ */
typedef struct SleepSummaryInput {
    int8_t padding; // No Function
} SleepSummaryInput;

typedef struct SleepSummaryOutput {
    uint32_t startTS;
    uint32_t endTS;
    int8_t *stages;
    // int8_t *movements;
    int32_t numEpochs;      // Number of Stages
    // int32_t numMovements;   // Number of Movements == floor(Number of Stages / 2)
    float totalSleepTime;
    float waso;
    float sleepPeriod;
    float efficiency;
    float wakeRatio;
    float remRatio;
    float lightRatio;
    float deepRatio;
    float wakeNumMinutes;
    float remNumMinutes;
    float lightNumMinutes;
    float deepNumMinutes;
    float score;
    uint8_t type;
} SleepSummaryOutput;


/* ***********  setMixedExerSession  ************ */
// typedef struct SessionInfo {
//     uint8_t numSteps;
//     uint8_t numSets;
//     uint8_t* steps;
//     uint8_t* duration;
//     uint8_t* effort;
//     uint8_t* circuitStartEnd;
//     uint8_t* checksum;
// } SessionInfo;

// /* ***********  updateGesture ************ */
// typedef struct IndexIOGesture {
//     uint64_t timestamp;     // ms
//     float* accX;
//     float* accY;
//     float* accZ;
//     float* gyroX;
//     float* gyroY;
//     float* gyroZ;
//     uint8_t accLength;
//     uint8_t gyroLength;
//     uint16_t gestureTypeOut;
//     uint8_t gestureStatusOut;   // 0: No Event, 1: Gesture Detected.
    
// } IndexIOGesture;

/* ***********  analyzeWomenHealthCycle ************ */
#define WOMEN_HEALTH_POTENTIAL_CYCLE 100
#define WOMEN_HEALTH_CYCLES_TO_PREDICT 5
typedef struct ModuleWomenHealthInput{
    int8_t cycleDuration;
}ModuleWomenHealthInput;

typedef struct ModuleWomenHealthOutput{
    int ovuDistance;
    int mensDistance;
    uint32_t nextOvu[WOMEN_HEALTH_CYCLES_TO_PREDICT];
    uint32_t nextMens[WOMEN_HEALTH_CYCLES_TO_PREDICT];
}ModuleWomenHealthOutput;

#ifdef __cplusplus
        }
#endif
#endif /* __GoMoreAlgoHalStruct__ */
