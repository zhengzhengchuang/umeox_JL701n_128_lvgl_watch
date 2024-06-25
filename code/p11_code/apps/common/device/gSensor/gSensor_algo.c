#include "app_config.h"
#include "includes.h"
#include "msg.h"
#include "p11_algo_api.h"


#define LOG(fmt,...)  //printf("[dev_acc] %s " fmt "\n",__func__, ##__VA_ARGS__)

void algo_event_response(u8 event)
{
    static u8 p11_algo_event;

    p11_algo_event = event;
    u8 low_power_mode  = MSYS_STATE_GET() != MSYS_STATE_NORMAL;

    if (p11_algo_event == ALGO_WRIST_DOWN && low_power_mode) {
        return;
    }
    // algo_log("P11 algo event %d", event);
    user_p11_post_to_main_system(P11_SYS_ALGO_EVENT, 1, &p11_algo_event, 0);  //通知算法事件

}

void gsensor_algo_run(void *accel, short point)
{
    // gpio_set_direction(IO_PORTB_09, 0);
    // gpio_set_output_value(IO_PORTB_09, 1);
    u8 algo_event = algo_gsensor_run((accel_data *)accel, point);
    // gpio_set_output_value(IO_PORTB_09, 0);

    if (algo_event != ALGO_NOTHING) {
        algo_event_response(algo_event);
    }
}


void gsensor_algo_init(void)
{
    algo_type      open_algo;
    accel_config   accel;

    open_algo.all = 0;
    open_algo.wrist_up   = 1;
    open_algo.wrist_down = 1;

    accel.sps   = 25;
    accel.lsb_g = 1024;
    algo_gsensor_init(open_algo, accel);

    LOG("sucess!");
}


void gsensor_algo_uninit(void)
{
    LOG("sucess!");
}