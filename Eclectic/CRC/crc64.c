/****************************************************************************************************
 * Include
 ****************************************************************************************************/

#include "Eclectic/CRC/crc64.h"
#include "Eclectic/Miscellaneous/memory.h"
#include "Eclectic/Miscellaneous/misc.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/****************************************************************************************************
 * Constant
 ****************************************************************************************************/

const crc64_configuration_t crc64_Configuration[] =
{
    {{"CRC-64"}, 1, 0x6C40DF5F0B497347, 0x0000000000000000, NULL, "CRC-64/ECMA-182", 0x42F0E1EBA9EA3693, false, false, 0x0000000000000000, 0x0000000000000000},
    {{}, 0, 0xB90956C775A41001, 0xFFFFFFFFFFFFFFFF, NULL, "CRC-64/GO-ISO", 0x000000000000001B, true, true, 0x5300000000000000, 0xFFFFFFFFFFFFFFFF},
    {{}, 0, 0x75D4B74F024ECEEA, 0xFFFFFFFFFFFFFFFF, NULL, "CRC-64/MS", 0x259C84CBA6426349, true, true, 0x0000000000000000, 0x0000000000000000},
    {{}, 0, 0xAE8B14860A799888, 0xFFFFFFFFFFFFFFFF, NULL, "CRC-64/NVME", 0xAD93D23594C93659, true, true, 0xF310303B2B6F6E42, 0xFFFFFFFFFFFFFFFF},
    {{}, 0, 0xE9C6D914C4B8D9CA, 0x0000000000000000, NULL, "CRC-64/REDIS", 0xAD93D23594C935A9, true, true, 0x0000000000000000, 0x0000000000000000},
    {{}, 0, 0x62EC59E3F1A4F00A, 0xFFFFFFFFFFFFFFFF, NULL, "CRC-64/WE", 0x42F0E1EBA9EA3693, false, false, 0xFCACBEBD5931A992, 0xFFFFFFFFFFFFFFFF},
    {{"CRC-64/GO-ECMA"}, 1, 0x995DC9BBDF1939FA, 0xFFFFFFFFFFFFFFFF, NULL, "CRC-64/XZ", 0x42F0E1EBA9EA3693, true, true, 0x49958C9ABD7D353F, 0xFFFFFFFFFFFFFFFF}
};
const size_t crc64_ConfigurationCount = sizeof(crc64_Configuration) / sizeof(crc64_Configuration[0]);

/****************************************************************************************************
 * Function Definition (Public)
 ****************************************************************************************************/

/*** Calculate ***/
uint64_t crc64_calculate(const crc64_configuration_t * const Configuration, const uint8_t *Data, const size_t DataLength)
{
    uint64_t crc = 0x0000000000000000;

    /*** Calculate ***/
    /* Error Check */
    if((Configuration != NULL) && (Data != NULL))
    {
        /* Set Up */
        crc = Configuration->initial;

        /* Calculate */
        for(size_t i = 0; i < DataLength; i++)
            crc = crc64_calculatePartial(Configuration, crc, Data[i], (i == 0), (i == (DataLength - 1)));
    }

    return crc;
}

/*** Calculate And Append ***/
void crc64_calculateAndAppend(const crc64_configuration_t * const Configuration, uint8_t *const buffer, const size_t BufferLength, const size_t DataLength)
{
    uint64_t crc = 0x0000000000000000;

    /*** Calculate And Append ***/
    if((Configuration != NULL) && (buffer != NULL) && (BufferLength >= (DataLength + sizeof(crc))))
    {
        crc = crc64_calculate(Configuration, buffer, DataLength);
        misc_insert64(buffer + DataLength, BufferLength - DataLength, crc, !Configuration->reflectOut);
    }
}

/*** Calculate Partial ***/
uint64_t crc64_calculatePartial(const crc64_configuration_t * const Configuration, uint64_t crc, const uint8_t Data, const bool First, const bool Last)
{
    /*** Calculate Partial ***/
    /* Error Check */
    if(Configuration != NULL)
    {
        /* Calculate Partial */
        if(Configuration->lookupTable == NULL)
        {
            /* Reflect */
            if(Configuration->reflectIn)
                crc ^= ((uint64_t)misc_reflect8(Data) << 56);
            else
                crc ^= ((uint64_t)Data << 56);

            /* Calculate */
            for(size_t bit = 0; bit < 8; bit++)
            {
                if((crc & 0x8000000000000000) == 0x8000000000000000)
                    crc = (crc << 1) ^ Configuration->polynomial;
                else
                    crc <<= 1;
            }

            /* Last */
            if(Last)
            {
                crc ^= Configuration->xorOut;
                if(Configuration->reflectOut)
                    crc = misc_reflect64(crc);
            }
        }
        else
        {
            /* Reflect */
            if(Configuration->reflectIn)
            {
                /* Reflect */
                if(First)
                    crc = misc_reflect64(crc);

                /* Lookup Table */
                crc = (crc >> 8) ^ Configuration->lookupTable[(crc ^ Data) % 256];
            }
            else
            {
                /* Lookup Table */
                crc = (crc << 8) ^ Configuration->lookupTable[(crc >> 56) ^ Data];
            }

            /* Last */
            if(Last)
                crc ^= Configuration->xorOut;
        }
    }

    return crc;
}

/*** Deinitialize ***/
void crc64_deinit(crc64_configuration_t * const configuration)
{
    /*** Deinitialize ***/
    /* Error Check */
    if(configuration != NULL)
    {
        /* Deinitialize */
        memory_free((void**)&(configuration->lookupTable));
        memset(configuration, 0, sizeof(*configuration));
    }
}

/*** Initialize ***/
void crc64_init(const char * const Name, crc64_configuration_t * const configuration, const bool CreateLookupTable)
{
    /*** Initialize ***/
    /* Error Check */
    if((Name != NULL) && (configuration != NULL))
    {
        bool found = false;
        size_t i;

        /* Algorithm */
        for(i = 0; i < crc64_ConfigurationCount; i++)
        {
            /* Name */
            if(strcmp(Name, crc64_Configuration[i].name) == 0)
            {
                /* Found */
                found = true;
                break;
            }
            else
            {
                /* Alias */
                for(size_t j = 0; j < crc64_Configuration[i].aliasCount; j++)
                {
                    if(strcmp(Name, crc64_Configuration[i].alias[j]) == 0)
                    {
                        /* Found */
                        found = true;
                        break;
                    }
                }

                /* Found */
                if(found)
                    break;
            }
        }

        /* Found */
        if(found)
        {
            /* Copy */
            (void)memcpy(configuration, &crc64_Configuration[i], sizeof(crc64_Configuration[i]));

            /* Generate Lookup Table */
            if(CreateLookupTable)
            {
                uint64_t *lookupTable;

                if((lookupTable = memory_malloc(CRC64_LOOKUP_TABLE_MEMORY_SIZE)) != NULL)
                {
                    for(i = 0; i < 256; i++)
                        lookupTable[i] = crc64_calculatePartial(configuration, 0x0000000000000000, (uint8_t)i, true, true) ^ configuration->xorOut;
                    configuration->lookupTable = lookupTable;
                }
            }
        }
        else
        {
            memset(configuration, 0, sizeof(*configuration));
        }
    }
}

/*** Verify ***/
bool crc64_verify(const crc64_configuration_t * const Configuration, const uint8_t *Buffer, const size_t BufferLength)
{
    bool verified = false;

    /*** Verify ***/
    /* Error Check */
    if((Configuration != NULL) && (Buffer != NULL))
    {
        const uint64_t Crc = crc64_calculate(Configuration, Buffer, BufferLength);
        verified = ((Crc ^ Configuration->xorOut) == Configuration->residue);
    }

    return verified;
}
