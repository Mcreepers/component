#ifndef __COM_TYPES_H__
#define __COM_TYPES_H__

// #include "component.h"

#ifdef COM_TYPES
#ifndef __stdint_h
#ifndef __need_wint_t
/*
 * 'signed' is redundant below, except for 'signed char' and if
 * the typedef is used to declare a bitfield.
 */

/* 7.18.1.1 */

/* exact-width signed integer types */
typedef signed char int8_t;
typedef signed short int int16_t;
typedef signed int int32_t;
typedef signed __INT64 int64_t;

/* exact-width unsigned integer types */
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
typedef unsigned __INT64 uint64_t;

/* 7.18.1.2 */

/* smallest type of at least n bits */
/* minimum-width signed integer types */
typedef signed char int_least8_t;
typedef signed short int int_least16_t;
typedef signed int int_least32_t;
typedef signed __INT64 int_least64_t;

/* minimum-width unsigned integer types */
typedef unsigned char uint_least8_t;
typedef unsigned short int uint_least16_t;
typedef unsigned int uint_least32_t;
typedef unsigned __INT64 uint_least64_t;

/* 7.18.1.3 */

/* fastest minimum-width signed integer types */
typedef signed int int_fast8_t;
typedef signed int int_fast16_t;
typedef signed int int_fast32_t;
typedef signed __INT64 int_fast64_t;

/* fastest minimum-width unsigned integer types */
typedef unsigned int uint_fast8_t;
typedef unsigned int uint_fast16_t;
typedef unsigned int uint_fast32_t;
typedef unsigned __INT64 uint_fast64_t;

/* 7.18.1.4 integer types capable of holding object pointers */
#if __sizeof_ptr == 8
typedef signed __INT64 intptr_t;
typedef unsigned __INT64 uintptr_t;
#else
typedef signed int intptr_t;
typedef unsigned int uintptr_t;
#endif

/* 7.18.1.5 greatest-width integer types */
typedef signed __LONGLONG intmax_t;
typedef unsigned __LONGLONG uintmax_t;

#endif
#endif
#endif

enum COM_StatusTypeDef {
    COM_OK = 0,
    COM_ERROR = 1,
    COM_BUSY = 2,
    COM_TIMEOUT = 3
};  // component status

#endif  // __COM_TYPES_H__
