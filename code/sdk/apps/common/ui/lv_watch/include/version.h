#ifndef __VERSION_H__
#define __VERSION_H__

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************************
                                    版本号(Vx.x.x)                             
*********************************************************************************/
#define Version_Internal    1 //内部版本
#define Version_Release_0   0 //释放版本
#define Version_Release_1   0 //释放版本

#define Num_3_Str(a,b,c) \
    "V" #a "." #b "." #c
#define _Version_Release_Str(a,b,c) \
    Num_3_Str(a,b,c)
#define Version_Release_Str \
    _Version_Release_Str(Version_Release_1, \
        Version_Release_0, Version_Internal)

#ifdef __cplusplus
}
#endif

#endif
