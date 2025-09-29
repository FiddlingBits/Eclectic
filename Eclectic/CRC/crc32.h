/****************************************************************************************************
 * Pragma
 ****************************************************************************************************/

#pragma once

/****************************************************************************************************
 * Define
 ****************************************************************************************************/

#define CRC32_LOOKUP_TABLE_COUNT       (256)
#define CRC32_LOOKUP_TABLE_MEMORY_SIZE (CRC32_LOOKUP_TABLE_COUNT * sizeof(uint32_t))
#define CRC32_MAXIMUM_ALIAS_COUNT      (5)

/****************************************************************************************************
 * Include
 ****************************************************************************************************/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/****************************************************************************************************
 * Type Definition
 ****************************************************************************************************/

typedef struct crc32_configuration_s
{
    char *Alias[CRC32_MAXIMUM_ALIAS_COUNT];
    size_t aliasCount;
    uint32_t check;
    uint32_t initial;
    uint32_t *lookupTable;
    char *Name;
    uint32_t polynomial;
    bool reflectIn;
    bool reflectOut;
    uint32_t residue;
    uint32_t xorOut;
} crc32_configuration_t;

/****************************************************************************************************
 * Function Prototype
 ****************************************************************************************************/

extern uint32_t crc32_calculate(const crc32_configuration_t * const Configuration, const uint8_t *Data, const size_t DataLength);
extern void crc32_calculateAndAppend(const crc32_configuration_t * const Configuration, uint8_t *const buffer, const size_t BufferLength, const size_t DataLength);
extern uint32_t crc32_calculatePartial(const crc32_configuration_t * const Configuration, uint32_t crc, const uint8_t Data, const bool First, const bool Last);
extern void crc32_deinit(crc32_configuration_t * const configuration);
extern void crc32_init(const char * const Name, crc32_configuration_t * const configuration, const bool CreateLookupTable);
extern bool crc32_verify(const crc32_configuration_t * const Configuration, const uint8_t *Buffer, const size_t BufferLength);
