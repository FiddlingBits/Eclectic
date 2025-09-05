/****************************************************************************************************
 * Pragma
 ****************************************************************************************************/

#pragma once

/****************************************************************************************************
 * Define
 ****************************************************************************************************/

/*** Signed ***/
#define RANDOM_S8()                       ((int8_t)random_S32())
#define RANDOM_S8_WITHIN_RANGE(MIN, MAX)  ((int8_t)random_S32WithinRange((MIN), (MAX)))
#define RANDOM_S16()                      ((int16_t)random_S32())
#define RANDOM_S16_WITHIN_RANGE(MIN, MAX) ((int16_t)random_S32WithinRange((MIN), (MAX)))
#define RANDOM_S32()                      (random_S32())
#define RANDOM_S32_WITHIN_RANGE(MIN, MAX) (random_S32WithinRange((MIN), (MAX)))

/*** Unsigned ***/
#define RANDOM_GET_U8()                       ((uint8_t)random_U32())
#define RANDOM_GET_U8_WITHIN_RANGE(MIN, MAX)  ((uint8_t)random_U32WithinRange((MIN), (MAX)))
#define RANDOM_GET_U16()                      ((uint16_t)random_U32())
#define RANDOM_GET_U16_WITHIN_RANGE(MIN, MAX) ((uint16_t)random_U32WithinRange((MIN), (MAX)))
#define RANDOM_GET_U32()                      (random_U32())
#define RANDOM_GET_U32_WITHIN_RANGE(MIN, MAX) (random_U32WithinRange((MIN), (MAX)))

/****************************************************************************************************
 * Include
 ****************************************************************************************************/

#include <stddef.h>
#include <stdint.h>

/****************************************************************************************************
 * Type Definition
 ****************************************************************************************************/

/*** Callback ***/
typedef uint32_t (*random_U32Callback_t)(void);

/****************************************************************************************************
 * Function Prototype
 ****************************************************************************************************/

extern void random_init(const random_U32Callback_t U32Callback);
extern int32_t random_S32(void);
extern int32_t random_S32WithinRange(const int32_t MinimumS32, const int32_t MaximumS32);
extern uint32_t random_U32(void);
extern uint32_t random_U32WithinRange(const uint32_t MinimumU32, const uint32_t MaximumU32);
extern void random_setBuffer(uint8_t * const buffer, const size_t BufferLength);
