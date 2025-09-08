/****************************************************************************************************
 * Pragma
 ****************************************************************************************************/

#pragma once

/****************************************************************************************************
 * Include
 ****************************************************************************************************/

#include <stddef.h>
#include <stdint.h>

/****************************************************************************************************
 * Function Prototype
 ****************************************************************************************************/

extern void random_buffer(uint8_t * const buffer, const size_t BufferLength);
extern int32_t random_S32(void);
extern int32_t random_S32WithinRange(const int32_t MinimumS32, const int32_t MaximumS32);
extern uint32_t random_U32(void);
extern uint32_t random_U32WithinRange(const uint32_t MinimumU32, const uint32_t MaximumU32);
