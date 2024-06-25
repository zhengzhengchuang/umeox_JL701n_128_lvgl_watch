//*********************************************************************************//
// Module name : typedef.h                                                         //
// Description : typedef head file                                                 //
// By Designer : zequan_liu                                                        //
// Dat changed :                                                                   //
//*********************************************************************************//

#ifndef __typedef_h__
#define __typedef_h__

//------------------------------------------------------//
// typedef
//------------------------------------------------------//

//typedef bool                _bit, bit;
typedef float               _f32, f32;
typedef double              _d64, d64;

typedef signed char         _s08, s08, _s8, s8;
typedef signed short        _s16, s16;
typedef signed int          _s32, s32;
typedef signed long long    _s64, s64;

typedef unsigned char       _u08, u08, _u8, u8, bool, BOOL;
typedef unsigned short      _u16, u16;
typedef unsigned int        _u32, u32;
typedef unsigned long long  _u64, u64;


#define ARRAY_SIZE(array)  (sizeof(array)/sizeof(array[0]))

//------------------------------------------------------//
// condition
//------------------------------------------------------//

#define SIMEND       asm ("trigger")

#define FALSE        0
#define TRUE         1
#define false        0
#define true         1
#define NULL         0

#define AT(x)        	__attribute__ ((section(#x)))
#define SEC_USED(x)     __attribute__((section(#x),used))
#define __INLINE__     	__attribute__ ((always_inline))
#define __NOINLINE__    __attribute__ ((noinline))
#define __WEAK__	    __attribute__((weak))
#define __NO_INIT__
#define ___interrupt 	__attribute__((interrupt("")))

//------------------------------------------------------//
// common define
//------------------------------------------------------//

#define hi(adr)           ( (adr) >> 16 )
#define lo(adr)           ( (adr) & 0xffff )

#define LDR_W(ptr)       *( (volatile _u32*)(ptr) )
#define LDR_H(ptr)       *( (volatile _u16*)(ptr) )
#define LDR_B(ptr)       *( (volatile _u08*)(ptr) )

#define STR_W(ptr,vld)   *( (volatile _u32*)(ptr) ) = (_u32)(vld);
#define STR_H(ptr,vld)   *( (volatile _u16*)(ptr) ) = (_u16)(vld);
#define STR_B(ptr,vld)   *( (volatile _u08*)(ptr) ) = (_u08)(vld);

#define BIT(n)            (        1<<n  )
#define BITSET(r,n)       ( r |=  (1<<n) )
#define BITCLR(r,n)       ( r &= ~(1<<n) )
#define BITXOR(r,n)       ( r ^=  (1<<n) )
#define BITTST(r,n)       ( r &   (1<<n) )

#define BIT_CHK1(r,n)     ( BITTST(r,n) == (1<<n) )
#define BIT_CHK0(r,n)     ( BITTST(r,n) == (0<<n) )

#define MIN(a,b)          ( (a<b) ? a : b )
#define MAX(a,b)          ( (a>b) ? a : b )

#define SFR(sfr, start, len, dat) (sfr = sfr & ~((~(0xffffffff << len)) << start) | ((dat & (~(0xffffffff << len))) << start))

#define NOP()				asm volatile ("nop")

#endif

//*********************************************************************************//
//                                                                                 //
//                               end of this module                                //
//                                                                                 //
//*********************************************************************************//
