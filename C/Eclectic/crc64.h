/****************************************************************************************************
 * Pragma
 ****************************************************************************************************/

#pragma once

/****************************************************************************************************
 * Define
 ****************************************************************************************************/

#define CRC64_LOOKUP_TABLE_COUNT       (256)
#define CRC64_LOOKUP_TABLE_MEMORY_SIZE (CRC64_LOOKUP_TABLE_COUNT * sizeof(uint64_t))
#define CRC64_MAXIMUM_ALIAS_COUNT      (1)

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
typedef struct crc64_configuration_s
{
    char *alias[CRC64_MAXIMUM_ALIAS_COUNT];
    size_t aliasCount;
    uint64_t check;
    uint64_t initial;
    uint64_t *lookupTable;
    char *name;
    uint64_t polynomial;
    bool reflectIn;
    bool reflectOut;
    uint64_t residue;
    uint64_t xorOut;
} crc64_configuration_t;

/****************************************************************************************************
 * Function Prototype
 ****************************************************************************************************/

extern uint64_t crc64_calculate(const crc64_configuration_t * const Configuration, const uint8_t *Data, const size_t DataLength);
extern void crc64_calculateAndAppend(const crc64_configuration_t * const Configuration, uint8_t *const buffer, const size_t BufferLength, const size_t DataLength);
extern uint64_t crc64_calculatePartial(const crc64_configuration_t * const Configuration, uint64_t crc, const uint8_t Data, const bool First, const bool Last);
extern void crc64_deinit(crc64_configuration_t * const configuration);
extern void crc64_init(const char * const Name, crc64_configuration_t * const configuration, const bool CreateLookupTable);
extern bool crc64_verify(const crc64_configuration_t * const Configuration, const uint8_t *Buffer, const size_t BufferLength);
