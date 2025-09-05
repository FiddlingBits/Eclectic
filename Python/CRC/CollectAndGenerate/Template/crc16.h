/****************************************************************************************************
 * Pragma
 ****************************************************************************************************/

#pragma once

/****************************************************************************************************
 * Define
 ****************************************************************************************************/

#define CRC16_LOOKUP_TABLE_COUNT       (256)
#define CRC16_LOOKUP_TABLE_MEMORY_SIZE (CRC16_LOOKUP_TABLE_COUNT * sizeof(uint16_t))
#define CRC16_MAXIMUM_ALIAS_COUNT      (##### REPLACEMENT #####)

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
typedef struct crc16_configuration_s
{
    char *alias[CRC16_MAXIMUM_ALIAS_COUNT];
    size_t aliasCount;
    uint16_t check;
    uint16_t initial;
    uint16_t *lookupTable;
    char *name;
    uint16_t polynomial;
    bool reflectIn;
    bool reflectOut;
    uint16_t residue;
    uint16_t xorOut;
} crc16_configuration_t;

/****************************************************************************************************
 * Function Prototype
 ****************************************************************************************************/

extern uint16_t crc16_calculate(const crc16_configuration_t * const Configuration, const uint8_t *Data, const size_t DataLength);
extern void crc16_calculateAndAppend(const crc16_configuration_t * const Configuration, uint8_t *const buffer, const size_t BufferLength, const size_t DataLength);
extern uint16_t crc16_calculatePartial(const crc16_configuration_t * const Configuration, uint16_t crc, const uint8_t Data, const bool First, const bool Last);
extern void crc16_deinit(crc16_configuration_t * const configuration);
extern void crc16_init(const char * const Name, crc16_configuration_t * const configuration, const bool CreateLookupTable);
extern bool crc16_verify(const crc16_configuration_t * const Configuration, const uint8_t *Buffer, const size_t BufferLength);
