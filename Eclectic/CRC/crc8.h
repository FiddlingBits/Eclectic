/****************************************************************************************************
 * Pragma
 ****************************************************************************************************/

#pragma once

/****************************************************************************************************
 * Define
 ****************************************************************************************************/

#define CRC8_LOOKUP_TABLE_COUNT       (256)
#define CRC8_LOOKUP_TABLE_MEMORY_SIZE (CRC8_LOOKUP_TABLE_COUNT * sizeof(uint8_t))
#define CRC8_MAXIMUM_ALIAS_COUNT      (2)

/****************************************************************************************************
 * Include
 ****************************************************************************************************/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/****************************************************************************************************
 * Type Definition
 ****************************************************************************************************/

/*** Structure ***/
typedef struct crc8_configuration_s
{
    char *Alias[CRC8_MAXIMUM_ALIAS_COUNT];
    size_t aliasCount;
    uint8_t check;
    uint8_t initial;
    uint8_t *lookupTable;
    char *Name;
    uint8_t polynomial;
    bool reflectIn;
    bool reflectOut;
    uint8_t residue;
    uint8_t xorOut;
} crc8_configuration_t;

/****************************************************************************************************
 * Function Prototype
 ****************************************************************************************************/

extern uint8_t crc8_calculate(const crc8_configuration_t * const Configuration, const uint8_t *Data, const size_t DataLength);
extern void crc8_calculateAndAppend(const crc8_configuration_t * const Configuration, uint8_t *const buffer, const size_t BufferLength, const size_t DataLength);
extern uint8_t crc8_calculatePartial(const crc8_configuration_t * const Configuration, uint8_t crc, const uint8_t Data, const bool Last);
extern void crc8_deinit(crc8_configuration_t * const configuration);
extern void crc8_init(const char * const Name, crc8_configuration_t * const configuration, const bool CreateLookupTable);
extern bool crc8_verify(const crc8_configuration_t * const Configuration, const uint8_t *Buffer, const size_t BufferLength);
