/****************************************************************************************************
 * Include
 ****************************************************************************************************/

#include "Eclectic/crc32.h"
#include "Eclectic/memory.h"
#include "Eclectic/misc.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/****************************************************************************************************
 * Constant
 ****************************************************************************************************/

const crc32_configuration_t crc32_Configuration[] =
{
##### REPLACEMENT #####
};
const size_t crc32_ConfigurationCount = sizeof(crc32_Configuration) / sizeof(crc32_Configuration[0]);

/****************************************************************************************************
 * Function Definition (Public)
 ****************************************************************************************************/

/*** Calculate ***/
uint32_t crc32_calculate(const crc32_configuration_t * const Configuration, const uint8_t *Data, const size_t DataLength)
{
    uint32_t crc = 0x00000000;

    /*** Calculate ***/
    /* Error Check */
    if((Configuration != NULL) && (Data != NULL))
    {
        /* Set Up */
        crc = Configuration->initial;

        /* Calculate */
        for(size_t i = 0; i < DataLength; i++)
            crc = crc32_calculatePartial(Configuration, crc, Data[i], (i == 0), (i == (DataLength - 1)));
    }

    return crc;
}

/*** Calculate And Append ***/
void crc32_calculateAndAppend(const crc32_configuration_t * const Configuration, uint8_t *const buffer, const size_t BufferLength, const size_t DataLength)
{
    uint32_t crc = 0x00000000;

    /*** Calculate And Append ***/
    if((Configuration != NULL) && (buffer != NULL) && (BufferLength >= (DataLength + sizeof(crc))))
    {
        crc = crc32_calculate(Configuration, buffer, DataLength);
        misc_insert32(buffer + DataLength, BufferLength - DataLength, crc, !Configuration->reflectOut);
    }
}

/*** Calculate Partial ***/
uint32_t crc32_calculatePartial(const crc32_configuration_t * const Configuration, uint32_t crc, const uint8_t Data, const bool First, const bool Last)
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
                crc ^= (misc_reflect8(Data) << 24);
            else
                crc ^= (Data << 24);

            /* Calculate */
            for(size_t bit = 0; bit < 8; bit++)
            {
                if((crc & 0x80000000) == 0x80000000)
                    crc = (crc << 1) ^ Configuration->polynomial;
                else
                    crc <<= 1;
            }

            /* Last */
            if(Last)
            {
                crc ^= Configuration->xorOut;
                if(Configuration->reflectOut)
                    crc = misc_reflect32(crc);
            }
        }
        else
        {
            /* Reflect */
            if(Configuration->reflectIn)
            {
                /* Reflect */
                if(First)
                    crc = misc_reflect32(crc);

                /* Lookup Table */
                crc = (crc >> 8) ^ Configuration->lookupTable[(crc ^ Data) % 256];
            }
            else
            {
                /* Lookup Table */
                crc = (crc << 8) ^ Configuration->lookupTable[(crc >> 24) ^ Data];
            }

            /* Last */
            if(Last)
                crc ^= Configuration->xorOut;
        }
    }

    return crc;
}

/*** Deinitialize ***/
void crc32_deinit(crc32_configuration_t * const configuration)
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
void crc32_init(const char * const Name, crc32_configuration_t * const configuration, const bool CreateLookupTable)
{
    /*** Initialize ***/
    /* Error Check */
    if((Name != NULL) && (configuration != NULL))
    {
        bool found = false;
        size_t i;

        /* Algorithm */
        for(i = 0; i < crc32_ConfigurationCount; i++)
        {
            /* Name */
            if(strcmp(Name, crc32_Configuration[i].name) == 0)
            {
                /* Found */
                found = true;
                break;
            }
            else
            {
                /* Alias */
                for(size_t j = 0; j < crc32_Configuration[i].aliasCount; j++)
                {
                    if(strcmp(Name, crc32_Configuration[i].alias[j]) == 0)
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
            (void)memcpy(configuration, &crc32_Configuration[i], sizeof(crc32_Configuration[i]));

            /* Generate Lookup Table */
            if(CreateLookupTable)
            {
                uint32_t *lookupTable;

                if((lookupTable = memory_malloc(CRC32_LOOKUP_TABLE_MEMORY_SIZE)) != NULL)
                {
                    for(i = 0; i < 256; i++)
                        lookupTable[i] = crc32_calculatePartial(configuration, 0x0000, (uint8_t)i, true, true) ^ configuration->xorOut;
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
bool crc32_verify(const crc32_configuration_t * const Configuration, const uint8_t *Buffer, const size_t BufferLength)
{
    bool verified = false;

    /*** Verify ***/
    /* Error Check */
    if((Configuration != NULL) && (Buffer != NULL))
    {
        const uint32_t Crc = crc32_calculate(Configuration, Buffer, BufferLength);
        verified = ((Crc ^ Configuration->xorOut) == Configuration->residue);
    }

    return verified;
}
