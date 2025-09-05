/****************************************************************************************************
* Pragma
 ****************************************************************************************************/

#pragma once

/****************************************************************************************************
 * Include
 ****************************************************************************************************/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/****************************************************************************************************
 * Function Prototype
 ****************************************************************************************************/

extern void misc_insert16(uint8_t * const Buffer, const size_t BufferLength, const uint16_t Data, const bool BigEndian);
extern void misc_insert32(uint8_t * const Buffer, const size_t BufferLength, const uint32_t Data, const bool BigEndian);
extern uint8_t misc_reflect8(const uint8_t Data);
extern uint16_t misc_reflect16(const uint16_t Data);
extern uint32_t misc_reflect32(const uint32_t Data);
