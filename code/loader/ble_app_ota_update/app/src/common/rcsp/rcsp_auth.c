#include "lib_include.h"
#include "typedef.h"
//#include "string.h"
//#include "bredr/bigint.h"
//#include "mem/malloc.h"
//#include "utils.h"
//////////////////////////////////////function B///////////////////////////////////////////////////

extern void *JL_controller_malloc(int size);
extern void JL_controller_free(void *p);

#if 0
#pragma code_seg(".rcsp_code")

//#include "xm_auth.h"
//#include "string.h"
//#include "XM_mem.h"
//#include "cpu/rand64.h"
//#include "cpu/uart.h"
//#include "bredr/bigint.h"
/* #include "mem/malloc.h"\
 */
//#include "utils.h"
//////////////////////////////////////function B///////////////////////////////////////////////////

extern void *JL_controller_calloc(int count, int size);
extern void get_random_number(u8 *ptr, u8 len);
//extern int get_random_number(u8 *ptr, u8 len);
#define XM_malloc 	JL_controller_malloc
#define XM_free		JL_controller_free
//used in function roundkeygenerate
//Realize Fuction Bi, i=2~17,Bi is a bias vector of Ki
//The Function is

//Bi=B[p][i]=  ((45**(45**(17*p+i+1) mod 257) mod 257)mod 256 )
//                    for i=0~15  p=1~17
//and by p is meant the number of round
//i is defined  as  the number of K

//The output will be convert to a table
//because the reslut is constant
//it's unnecessary to calculate every time


//to simple the function B , a table is made as following

static const u8  B[17][16] = {
    {0x77, 0xf1, 0x56, 0x24, 0x7e, 0x47, 0x1b, 0x86, 0xbd, 0x70, 0x8e, 0x1e, 0x3b, 0x73, 0x16, 0x03},
    {0x64, 0xac, 0x28, 0x5a, 0xc9, 0xb3, 0x37, 0xc5, 0x0a, 0x10, 0xb7, 0xa3, 0xba, 0xb1, 0x97, 0x46},
    {0x3d, 0x05, 0xdc, 0x66, 0x6e, 0xf6, 0x9a, 0xf8, 0x0d, 0x58, 0x95, 0x67, 0xc6, 0xaa, 0xab, 0xec},
    {0xa0, 0x68, 0x9b, 0x96, 0xd4, 0xeb, 0xbf, 0x43, 0x49, 0x36, 0xe9, 0x6a, 0x89, 0xd8, 0xc3, 0x8a},
    {0x94, 0x63, 0x99, 0xbc, 0x7b, 0xbe, 0xc1, 0x22, 0xbb, 0x5c, 0x71, 0xd5, 0x1f, 0x92, 0x57, 0x5d},
    {0x8f, 0x44, 0x41, 0x1d, 0x51, 0xe6, 0x40, 0x17, 0xfb, 0xfd, 0x19, 0x32, 0x34, 0xb8, 0x61, 0x2a},
    {0xca, 0x23, 0x6f, 0xda, 0x39, 0xf7, 0xa2, 0x01, 0x7f, 0xd6, 0x31, 0xe7, 0xde, 0x80, 0x04, 0xdd},
    {0x2c, 0x59, 0x82, 0xaf, 0xa8, 0xe0, 0x0f, 0xcd, 0xa1, 0x12, 0x3e, 0x30, 0xd1, 0x1c, 0xd0, 0x3a},
    {0x33, 0x72, 0x2e, 0x4f, 0x90, 0x02, 0x13, 0x06, 0x75, 0xce, 0x87, 0xc2, 0xef, 0xb2, 0xad, 0x7d},
    {0x38, 0x15, 0xe1, 0x52, 0x9f, 0x7a, 0x6c, 0x2f, 0x27, 0xc4, 0xe2, 0x81, 0xa9, 0xcf, 0x8d, 0xc0},
    {0xd7, 0xdf, 0xff, 0x60, 0x76, 0x14, 0x8c, 0x5e, 0x55, 0x09, 0xe4, 0x08, 0xc7, 0x42, 0x20, 0xfc},
    {0xd2, 0x50, 0x91, 0xd9, 0x4c, 0x62, 0x9e, 0xe8, 0xb9, 0xa6, 0xf9, 0x1a, 0x00, 0x21, 0x0b, 0xfa},
    {0x35, 0x9c, 0x4e, 0x4b, 0x69, 0x48, 0xcb, 0x0e, 0xc8, 0xa4, 0x5b, 0xea, 0x84, 0x07, 0xb4, 0x18},
    {0xf4, 0xae, 0x6b, 0xdb, 0xa7, 0xcc, 0x3f, 0x8b, 0x4a, 0x0c, 0x3c, 0x25, 0xe5, 0x54, 0x4d, 0x45},
    {0x83, 0xed, 0x11, 0xf0, 0xb0, 0x53, 0x93, 0xf2, 0x74, 0x26, 0xb5, 0x9d, 0x6d, 0x7c, 0xf3, 0x2d},
    {0xf1, 0x56, 0x24, 0x7e, 0x47, 0x1b, 0x86, 0xbd, 0x70, 0x8e, 0x1e, 0x3b, 0x73, 0x16, 0x03, 0xb6},
    {0xac, 0x28, 0x5a, 0xc9, 0xb3, 0x37, 0xc5, 0x0a, 0x10, 0xb7, 0xa3, 0xba, 0xb1, 0x97, 0x46, 0x88}
};


////////////////////////////////////Funciton e/////////////////////////////////

//This function can realize function e

//function e : ( 45**i (mod 257) ) (mod 256)

//function l is the reverse function of function e

//in the main function it will be made of two table as the etable the and ltable

//this funciton will be used in function roundkeygenerate


static const u8 etable[256] = {
    1, 45, 226, 147, 190, 69, 21, 174, 120, 3, 135, 164, 184, 56, 207, 63,
    8, 103, 9, 148, 235, 38, 168, 107, 189, 24, 52, 27, 187, 191, 114, 247,
    64, 53, 72, 156, 81, 47, 59, 85, 227, 192, 159, 216, 211, 243, 141, 177,
    255, 167, 62, 220, 134, 119, 215, 166, 17, 251, 244, 186, 146, 145, 100, 131,
    241, 51, 239, 218, 44, 181, 178, 43, 136, 209, 153, 203, 140, 132, 29, 20,
    129, 151, 113, 202, 95, 163, 139, 87, 60, 130, 196, 82, 92, 28, 232, 160,
    4, 180, 133, 74, 246, 19, 84, 182, 223, 12, 26, 142, 222, 224, 57, 252,
    32, 155, 36, 78, 169, 152, 158, 171, 242, 96, 208, 108, 234, 250, 199, 217,
    0, 212, 31, 110, 67, 188, 236, 83, 137, 254, 122, 93, 73, 201, 50, 194,
    249, 154, 248, 109, 22, 219, 89, 150, 68, 233, 205, 230, 70, 66, 143, 10,
    193, 204, 185, 101, 176, 210, 198, 172, 30, 65, 98, 41, 46, 14, 116, 80,
    2, 90, 195, 37, 123, 138, 42, 91, 240, 6, 13, 71, 111, 112, 157, 126,
    16, 206, 18, 39, 213, 76, 79, 214, 121, 48, 104, 54, 117, 125, 228, 237,
    128, 106, 144, 55, 162, 94, 118, 170, 197, 127, 61, 175, 165, 229, 25, 97,
    253, 77, 124, 183, 11, 238, 173, 75, 34, 245, 231, 115, 35, 33, 200, 5,
    225, 102, 221, 179, 88, 105, 99, 86, 15, 161, 49, 149, 23, 7, 58, 40,
};
static const u8 ltable[256] = {
    128, 0, 176, 9, 96, 239, 185, 253, 16, 18, 159, 228, 105, 186, 173, 248,
    192, 56, 194, 101, 79, 6, 148, 252, 25, 222, 106, 27, 93, 78, 168, 130,
    112, 237, 232, 236, 114, 179, 21, 195, 255, 171, 182, 71, 68, 1, 172, 37,
    201, 250, 142, 65, 26, 33, 203, 211, 13, 110, 254, 38, 88, 218, 50, 15,
    32, 169, 157, 132, 152, 5, 156, 187, 34, 140, 99, 231, 197, 225, 115, 198,
    175, 36, 91, 135, 102, 39, 247, 87, 244, 150, 177, 183, 92, 139, 213, 84,
    121, 223, 170, 246, 62, 163, 241, 17, 202, 245, 209, 23, 123, 147, 131, 188,
    189, 82, 30, 235, 174, 204, 214, 53, 8, 200, 138, 180, 226, 205, 191, 217,
    208, 80, 89, 63, 77, 98, 52, 10, 72, 136, 181, 86, 76, 46, 107, 158,
    210, 61, 60, 3, 19, 251, 151, 81, 117, 74, 145, 113, 35, 190, 118, 42,
    95, 249, 212, 85, 11, 220, 55, 49, 22, 116, 215, 119, 167, 230, 7, 219,
    164, 47, 70, 243, 97, 69, 103, 227, 12, 162, 59, 28, 133, 24, 4, 29,
    41, 160, 143, 178, 90, 216, 166, 126, 238, 141, 83, 75, 161, 154, 193, 14,
    122, 73, 165, 44, 129, 196, 199, 54, 43, 127, 67, 149, 51, 242, 108, 104,
    109, 240, 2, 40, 206, 221, 155, 234, 94, 153, 124, 20, 134, 207, 229, 66,
    184, 64, 120, 45, 58, 233, 100, 31, 146, 144, 125, 57, 111, 224, 137, 48,
};

/*
static void Data_inv(u8 *data,u8 len)
{
	u8 i;
	for(i=0;i<(len>>1);i++)
	{
		data[i] ^= data[len-i-1];
		data[len-i-1]^= data[i];
		data[i] ^= data[len-i-1];
	}
}
*/
///////////////////////////////////fuction exp45////////////////////////////////
//this funciton realize the etable and ltable
//used in function roundkeygenerate
static void exp45(u8 *data)
{
    data[0] = etable[data[0]];
    data[3] = etable[data[3]];
    data[4] = etable[data[4]];
    data[7] = etable[data[7]];
    data[8] = etable[data[8]];
    data[11] = etable[data[11]];
    data[12] = etable[data[12]];
    data[15] = etable[data[15]];

    data[1] = ltable[data[1]];
    data[2] = ltable[data[2]];
    data[5] = ltable[data[5]];
    data[6] = ltable[data[6]];
    data[9] = ltable[data[9]];
    data[10] = ltable[data[10]];
    data[13] = ltable[data[13]];
    data[14] = ltable[data[14]];

}

//////////////////////////////////Funciton pht//////////////////////////////////
//realize the PHT convertion
//this function will be used in function roundkeygenerate
static void pht(u8 *data)
{
    u8 i;
    u8 *p = data;
    int temp;

    for (i = 0; i < 8; i++) {
        temp = (*p << 1) + *(p + 1);
        *(p + 1) = *p + *(p + 1);
        *p = temp;
        p += 2;
    }
}

//////////////////////////////////Funciton pht//////////////////////////////////


//////////////////////////////////Funciton premute//////////////////////////////////
//this function will be used in function roundkeygenerate
static void premute(u8 *data)
{
    int i;
    u8 temp[16];
    for (i = 0; i < 16; i++) {
        temp[i] = data[i];
    }
    data[0] = temp[8];
    data[1] = temp[11];
    data[2] = temp[12];
    data[3] = temp[15];
    data[4] = temp[2];
    data[5] = temp[1];
    data[6] = temp[6];
    data[7] = temp[5];
    data[8] = temp[10];
    data[9] = temp[9];
    data[10] = temp[14];
    data[11] = temp[13];
    data[12] = temp[0];
    data[13] = temp[7];
    data[14] = temp[4];
    data[15] = temp[3];
}

//////////////////////////////////Funciton premute//////////////////////////////////


////////////////////////////////////roundkeygenerate/////////////////////////////////////////////////

// used in function roundencrypt

//u8 roundkey[17][16];

//define the shift(or shit!) function which can rotate each octet of the (128bit+8bit) key left by 3 bit positions
static void shift(u8  tmp_key[17])
{
    u8 tmp1;
    u8 m = 0;
    u8 *p = tmp_key;


    for (m = 0; m < 17; m++) {
        tmp1 = *p >> 5;
        *p <<= 3;
        *p += tmp1;
        p++;
    }
    return;
}

static void roundkeygenerate(u8 inputkey[16], u8 roundkey[17][16])
{
    //Realize Fuction Ki, i=1~17
    //Ki is a round key used in Function Ar
    // How to use the function? You can see it in BlueTooth V1.1 core specification Page 174

    //the input parameter is a 128 bit key grouped in 16 octets
    //so in the main function the only need is to define 128 bit key and function B
    //then the function Ki is called

    //this function need the Function B as an input parameter

    //to convenience calculate, the 128 bit key is define as a dimension contained 17 elements
    //and the last element(17) can be find out througth the other 16 elements

    u8 key[17], tmp_key[17];
    u8 i = 0;
    u8 *p;
    u8 *p_end = tmp_key + 16;
    int j = 0;

    //roundkey represents output Ki,i=1~17
    //
    memset(roundkey, 0, 17 * 16);

    //initialize ki,
    key[16] = 0;
    for (i = 0; i < 16; i++) {
        key[i] = inputkey[i];
        key[16] = key[16] ^ key[i];
    }
    //key[16]=0;

    //this step can get roundkey k1
    //and the sametime get tmp_key, tmp_key will be ratate for every time
    for (i = 0; i < 16; i++) {
        roundkey[0][i] = key[i];
        tmp_key[i] = key[i];
    }
    tmp_key[16] = key[16];

    //this recycle can get roundkey k2~k17

    for (i = 0; i < 16; i++) {
        shift(tmp_key);
        //tmp_key[16]=tmp_key[0]^tmp_key[1]^tmp_key[14]^tmp_key[15];
        p = tmp_key + i + 1;
        for (j = 0; j < 16; j++) {
            roundkey[i + 1][j] = *(p++) + B[i + 1][15 - j];
            if (p > p_end) {
                p = tmp_key;
            }
        }
    }
}


//////////////////////////////////////roundencrypt////////////////////////////////////////////////

//used in function Ar or function Ar'

static void roundencrypt(u8  k, u8 *rand, u8 roundkey[17][16])
{
    int m;


    for (m = 0; m < 16; m++) {

        switch (m) {
        case 0:
        case 3:
        case 4:
        case 7:
        case 8:
        case 11:
        case 12:
        case 15:
            *(rand + m) ^= roundkey[2 * k][m];
            break;
        default:
            *(rand + m) = (*(rand + m) + roundkey[2 * k][m]) ;
            break;
        }
    }

    exp45(rand);

    for (m = 0; m < 16; m++) {

        switch (m) {
        case 0:
        case 3:
        case 4:
        case 7:
        case 8:
        case 11:
        case 12:
        case 15:
            *(rand + m) = (*(rand + m) + roundkey[2 * k + 1][m]) ;
            break;
        default:
            *(rand + m) ^= roundkey[2 * k + 1][m];
            break;
        }
    }

    pht(rand);

    premute(rand);

    pht(rand);

    premute(rand);

    pht(rand);

    premute(rand);

    pht(rand);

}

static void function_Ar01(u8 *rand,  u8 roundkey[17][16], u8 mode)
{
//	u8 q=0;
    u8 m = 0;
    u8 i = 0;
    u8 k = 0;
    u8 RAND[16];
    u8(*p)[16] = roundkey;
    for (i = 0; i < 16; i++) {
        RAND[i] = rand[i];
    }
    for (k = 0; k < 8; k++) {
        if (k == 2 && mode)

            for (m = 0; m < 16; m++) {
                switch (m) {
                case 0:
                case 3:
                case 4:
                case 7:
                case 8:
                case 11:
                case 12:
                case 15:
                    *(rand + m) = *(RAND + m) ^ (*(rand + m));
                    break;
                default:
                    *(rand + m) = (*(RAND + m) + * (rand + m));
                    break;
                }
            }
        roundencrypt(k, rand, roundkey);
        p += 2;
    }

    for (m = 0; m < 16; m++) {
        switch (m) {
        case 0:
        case 3:
        case 4:
        case 7:
        case 8:
        case 11:
        case 12:
        case 15:
            *(rand + m) = *(rand + m)^roundkey[16][m];
            break;
        default:
            *(rand + m) = (*(rand + m) + roundkey[16][m]);
            break;
        }
    }

}

_WEAK_
static void function_E13(u8 *address_cof, u8 *RAND, u8 *K, u8 L, u8 *result) //1 E1 3 E3
{
    u8 address128[16];
    u8 m = 0;
//	u8 p=0;
//	u8 q=0;

    u8 K1[16];
    u8 *ptr = address_cof;
    u8 *ptr_end = address_cof + L - 1;

    u8 *roundkey;

    roundkey = XM_malloc(17 * 16);

    /////////////////////////////address expand////////////////
    //expand the address with 48bits to one with 128bits
    for (m = 0; m < 16; m++) {
        address128[m] = *(ptr++);
        if (ptr > ptr_end) {
            ptr = address_cof;
        }
    }

    /////////////////////////////address expand////////////////

    for (m = 0; m < 16; m++) {
        result[m] = RAND[m];
    }

    roundkeygenerate(K, (u8(*)[16])roundkey);


    //function_Ar(rand, roundkey);
    function_Ar01(result, (u8(*)[16])roundkey, 0);
    for (m = 0; m < 16; m++) {
        result[m] ^= RAND[m];
        result[m] += address128[m];
    }
    /*u8 temp_k[16];
    for(m=0;m<16;m++) temp_k[m]=K1[15-m];*/
    ////////////////////////////function K'/////////////////////////
    K1[0] = (K[0] + 233);
    K1[1] = K[1] ^ 229;
    K1[2] = (K[2] + 223);
    K1[3] = K[3] ^ 193;
    K1[4] = (K[4] + 179);
    K1[5] = K[5] ^ 167;
    K1[6] = (K[6] + 149);
    K1[7] = K[7] ^ 131;

    K1[ 8] = K[ 8] ^ 233;
    K1[ 9] = (K[ 9] + 229);
    K1[10] = K[10] ^ 223;
    K1[11] = (K[11] + 193);
    K1[12] = K[12] ^ 179;
    K1[13] = (K[13] + 167);
    K1[14] = K[14] ^ 149;
    K1[15] = (K[15] + 131);
    ////////////////////////////function K'/////////////////////////
    roundkeygenerate(K1, (u8(*)[16])roundkey);
    //function_Ar1(rand, roundkey);
    function_Ar01(result, (u8(*)[16])roundkey, 1);

    XM_free(roundkey);
}

//////////////////////////////////////function_E1///////////////////////////////////////////////////

static void function_E1test(u8 *address, u8 *RAND, u8 *K, u8 *result)
{
//	u8 p;
    function_E13(address, RAND, K, 6, result);
}

//////////////////////////////////////function_E21///////////////////////////////////////////////////

static void function_E21(u8 *address, u8 *RAND, u8 *reslut)
{
    u8 m = 0;
//	u8 q;
    u8 *p = address;
    u8 *p_end = address + 5;
    u8 rand[16];
    //u8 rand[16];
    //for(u8 m=0;m<16;m++) rand[m]=RAND[m];;

    u8 *roundkey;

    roundkey = XM_malloc(17 * 16);


    /////////////////////////////address expand////////////////
    //expand the address with 48bits to one with 128bits

    for (m = 0; m < 16; m++) {
        reslut[m] = *(p++);
        rand[m] = RAND[m];
        if (p > p_end) {
            p = address;
        }
    }
    rand[15] = rand[15] ^ 6;
    /////////////////////////////address expand////////////////

    roundkeygenerate(rand, (u8(*)[16])roundkey);


    function_Ar01(reslut, (u8(*)[16])roundkey, 1);
    XM_free(roundkey);
}

static  u8 g_addr[6] = {0x11, 0x22, 0x33, 0x33, 0x22, 0x11};
static  u8 g_link_key[16] = {0x06, 0x77, 0x5f, 0x87, 0x91, 0x8d, 0xd4, 0x23, 0x00, 0x5d, 0xf1, 0xd8, 0xcf, 0x0c, 0x14, 0x2b};

void XM_auth_link_key_reg(const u8 link_key[16])
{
    memcpy(g_link_key, link_key, 16);
}

void XM_auth_link_addr_reg(const u8 addr[6])
{
    memcpy(g_addr, addr, 6);
}

/* void make_rand_num(u8 *buf)
{
    get_random_number(buf, 16);
}
 */

void function_E1(u8 *address, u8 *RAND, u8 *K, u8 *result)
{
    function_E13(address, RAND, K, 6, result);
    //function_E21(address, RAND, result);
}

static void function_xiaomi(u8 *rand, u8 *result)
{
    function_E1(g_addr, rand, g_link_key, result);
}
#else

void *ex_malloc(int size)
{
    return JL_controller_malloc(size);
}

void ex_free(void *p)
{
    JL_controller_free(p);
}
#endif


