#ifndef _HEART_NET_ORG_DLL_H_  
#define _HEART_NET_ORG_DLL_H_

#include "../inc/goodix_type.h"

extern uint32_t *get_knSmallWeightsArr_addr();
extern uint32_t get_knSmallWeightsArr_size();
extern void get_knSmallWeightsArr_version(char* ver, uint8_t len);

extern uint32_t *get_knWeightsArr_addr();
extern uint32_t get_knWeightsArr_size();
extern void get_knWeightsArr_version(char* ver, uint8_t len);

extern uint32_t *get_knConfNetWeightsArr_addr();
extern uint32_t get_knConfNetWeightsArr_size();
extern void get_knConfNetWeightsArr_version(char* ver, uint8_t len);

extern uint32_t *get_knSceneNetWeightsArr_addr();
extern uint32_t get_knSceneNetWeightsArr_size();
extern void get_knSceneNetWeightsArr_version(char* ver, uint8_t len);

extern uint32_t *get_knSceneSwitchWeightsArr_addr();
extern uint32_t get_knSceneSwitchWeightsArr_size();
extern void get_knSceneSwitchWeightsArr_version(char* ver, uint8_t len);

#if (_WIN32 || _WIN64)
#define VERSION_SIZE_MAX        10
typedef struct
{
	char net_version[VERSION_SIZE_MAX];
	uint32_t para_size;
	uint32_t *weight_arr;

}st_hrnet_params_info;

typedef struct {
	st_hrnet_params_info knWeightsSmall;
	st_hrnet_params_info knWeights;
	st_hrnet_params_info knConfNetWeights;
	st_hrnet_params_info knSceneNetWeights;
	st_hrnet_params_info knSceneSwitchWeights;
}goodix_win_params;

DRVDLL_API void goodix_heart_net_dll_set_params_inner(goodix_win_params * dll_params_in);
void heart_net_dll_param_init();
#endif

#endif
