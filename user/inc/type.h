#ifndef _TYPE_H_
#define _TYPE_H_
#include <LIMITS.h>
#include "stc8.h"

typedef signed char        int8_t;
typedef int                int16_t;
typedef long			   int32_t;
typedef unsigned char      uint8_t;
typedef unsigned int       uint16_t;
typedef unsigned long	   uint32_t;


typedef signed char        s8;
typedef int                s16;
typedef long			   s32;
typedef unsigned char      u8;
typedef unsigned int       u16;
typedef unsigned long	   u32;

typedef int8_t		bool;

#define INT8_MIN          (-127-1)
#define INT16_MIN         (-32767-1)
#define INT32_MIN         (-2147483647-1)
#define INT8_MAX          127
#define INT16_MAX         32767
#define INT32_MAX         2147483647
#define UINT8_MAX         0xffu
#define UINT16_MAX        0xffffu
#define UINT32_MAX        0xffffffffu


#define INT8_C(x)    (x)
#define INT16_C(x)   (x)
#define INT32_C(x)   (x ## L)

#define UINT8_C(x)   (x)
#define UINT16_C(x)  (x)
#define UINT32_C(x)  (x ## UL)

#define INTMAX_C(x)  INT64_C(x)
#define UINTMAX_C(x) UINT64_C(x)


#ifndef FALSE
#define FALSE		0
#endif 

#ifndef TRUE
#define TRUE		!FALSE
#endif 



#endif 
