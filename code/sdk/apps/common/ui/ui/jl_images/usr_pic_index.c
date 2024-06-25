#include "ui/ui_pic_index.h"
#include "usr_pic_index.h"

    // const struct fid my_fid[] = {
    //     {
    //         .name = 0,
    //         .id = 0,
    //     },  
    
    //     {
    //         .name = "1692777947410.png",
    //         .id = 0x000001,
    //     },
    
    //     {
    //         .name = "arrow.png",
    //         .id = 0x000002,
    //     },
    
    //     {
    //         .name = "bg1.png",
    //         .id = 0x000003,
    //     },
    
    //     {
    //         .name = "bg2.png",
    //         .id = 0x000004,
    //     },
    
    //     {
    //         .name = "bg3.png",
    //         .id = 0x000005,
    //     },
    
    //     {
    //         .name = "bgjpg2.jpg",
    //         .id = 0x000006,
    //     },
    
    //     {
    //         .name = "bird_1.png",
    //         .id = 0x000007,
    //     },
    
    //     {
    //         .name = "bird_2.bmp",
    //         .id = 0x000008,
    //     },
    
    //     {
    //         .name = "bird_2.png",
    //         .id = 0x000009,
    //     },
    
    //     {
    //         .name = "bird_3.bmp",
    //         .id = 0x00000a,
    //     },
    
    //     {
    //         .name = "bird_3.png",
    //         .id = 0x00000b,
    //     },
    
    //     {
    //         .name = "bird_4.png",
    //         .id = 0x00000c,
    //     },
    
    //     {
    //         .name = "bird_5.bmp",
    //         .id = 0x00000d,
    //     },
    
    //     {
    //         .name = "bird_5.png",
    //         .id = 0x00000e,
    //     },
    
    //     {
    //         .name = "bird_6.bmp",
    //         .id = 0x00000f,
    //     },
    
    //     {
    //         .name = "bird_6.png",
    //         .id = 0x000010,
    //     },
    
    //     {
    //         .name = "btn_bg_1.png",
    //         .id = 0x000011,
    //     },
    
    //     {
    //         .name = "btn_bg_2.png",
    //         .id = 0x000012,
    //     },
    
    //     {
    //         .name = "btn_bg_3.png",
    //         .id = 0x000013,
    //     },
    
    //     {
    //         .name = "call1.png",
    //         .id = 0x000014,
    //     },
    
    //     {
    //         .name = "call2.png",
    //         .id = 0x000015,
    //     },
    
    //     {
    //         .name = "clockwise_hour.png",
    //         .id = 0x000016,
    //     },
    
    //     {
    //         .name = "clockwise_min.png",
    //         .id = 0x000017,
    //     },
    
    //     {
    //         .name = "clockwise_sec.png",
    //         .id = 0x000018,
    //     },
    
    //     {
    //         .name = "clouds.png",
    //         .id = 0x000019,
    //     },
    
    //     {
    //         .name = "daily_mission.png",
    //         .id = 0x00001a,
    //     },
    
    //     {
    //         .name = "ecg.png",
    //         .id = 0x00001b,
    //     },
    
    //     {
    //         .name = "flash.png",
    //         .id = 0x00001c,
    //     },
    
    //     {
    //         .name = "grid.png",
    //         .id = 0x00001d,
    //     },
    
    //     {
    //         .name = "heart.png",
    //         .id = 0x00001e,
    //     },
    
    //     {
    //         .name = "heart2.png",
    //         .id = 0x00001f,
    //     },
    
    //     {
    //         .name = "hour.png",
    //         .id = 0x000020,
    //     },
    
    //     {
    //         .name = "house.png",
    //         .id = 0x000021,
    //     },
    
    //     {
    //         .name = "mask.png",
    //         .id = 0x000022,
    //     },
    
    //     {
    //         .name = "measure.png",
    //         .id = 0x000023,
    //     },
    
    //     {
    //         .name = "mute.png",
    //         .id = 0x000024,
    //     },
    
    //     {
    //         .name = "preview.png",
    //         .id = 0x000025,
    //     },
    
    //     {
    //         .name = "rain.png",
    //         .id = 0x000026,
    //     },
    
    //     {
    //         .name = "s1.png",
    //         .id = 0x000027,
    //     },
    
    //     {
    //         .name = "s2.png",
    //         .id = 0x000028,
    //     },
    
    //     {
    //         .name = "s3.png",
    //         .id = 0x000029,
    //     },
    
    //     {
    //         .name = "s4.png",
    //         .id = 0x00002a,
    //     },
    
    //     {
    //         .name = "s5.png",
    //         .id = 0x00002b,
    //     },
    
    //     {
    //         .name = "s6.png",
    //         .id = 0x00002c,
    //     },
    
    //     {
    //         .name = "s7.png",
    //         .id = 0x00002d,
    //     },
    
    //     {
    //         .name = "s8.png",
    //         .id = 0x00002e,
    //     },
    
    //     {
    //         .name = "s9.png",
    //         .id = 0x00002f,
    //     },
    
    //     {
    //         .name = "samatha.png",
    //         .id = 0x000030,
    //     },
    
    //     {
    //         .name = "sec_dot.png",
    //         .id = 0x000031,
    //     },
    
    //     {
    //         .name = "step.png",
    //         .id = 0x000032,
    //     },
    
    //     {
    //         .name = "sun.png",
    //         .id = 0x000033,
    //     },
    
    //     {
    //         .name = "unmute.png",
    //         .id = 0x000034,
    //     },
    
    //     {
    //         .name = "volume.png",
    //         .id = 0x000035,
    //     },
    
    //     {
    //         .name = "wave1.png",
    //         .id = 0x000036,
    //     },
    
    //     {
    //         .name = "wave2.png",
    //         .id = 0x000037,
    //     },
    
    //     {
    //         .name = "weather.png",
    //         .id = 0x000038,
    //     },
    
    //     {
    //         .name = "weather_cloud.png",
    //         .id = 0x000039,
    //     },
    
    //     {
    //         .name = "weather_cloud_fog.png",
    //         .id = 0x00003a,
    //     },
    
    //     {
    //         .name = "weather_sun.png",
    //         .id = 0x00003b,
    //     },
    
    //     {
    //         .name = "weather_sun_cloud.png",
    //         .id = 0x00003c,
    //     },
    
    //     {
    //         .name = "wind.png",
    //         .id = 0x00003d,
    //     },
    
    //     {
    //         .name = "x.png",
    //         .id = 0x00003e,
    //     },
    // };
    //     int get_img_num(){
    //         return sizeof(my_fid)/sizeof(my_fid[0])-1;
    //     }
    //     char *SRCBYID(int id){
    //         if(id<=get_img_num()){
    //             return my_fid[id].name;
    //         }
    //         return 0;
    //     }
        