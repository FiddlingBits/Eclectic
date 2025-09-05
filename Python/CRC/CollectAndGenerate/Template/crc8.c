/****************************************************************************************************
 * Include
 ****************************************************************************************************/

#include "Eclectic/crc8.h"
#include "Eclectic/memory.h"
#include "Eclectic/misc.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/****************************************************************************************************
 * Constant
 ****************************************************************************************************/

const crc8_configuration_t crc8_Configuration[] =
{
##### REPLACEMENT #####
};
const size_t crc8_ConfigurationCount = sizeof(crc8_Configuration) / sizeof(crc8_Configuration[0]);

/****************************************************************************************************
 * Function Definition (Public)
 ****************************************************************************************************/

/*** Calculate ***/
uint8_t crc8_calculate(const crc8_configuration_t * const Configuration, const uint8_t *Data, const size_t DataLength)
{
    uint8_t crc = 0x00;

    /*** Calculate ***/
    /* Error Check */
    if((Configuration != NULL) && (Data != NULL))
    {
        /* Set Up */
        crc = Configuration->initial;

        /* Calculate */
        for(size_t i = 0; i < DataLength; i++)
            crc = crc8_calculatePartial(Configuration, crc, Data[i], (i == (DataLength - 1)));
    }

    return crc;
}

/*** Calculate And Append ***/
void crc8_calculateAndAppend(const crc8_configuration_t * const Configuration, uint8_t *const buffer, const size_t BufferLength, const size_t DataLength)
{
    uint8_t crc = 0x00;

    /*** Calculate And Append ***/
    if((Configuration != NULL) && (buffer != NULL) && (BufferLength >= (DataLength + sizeof(crc))))
    {
        crc = crc8_calculate(Configuration, buffer, DataLength);
        buffer[DataLength] = crc;
    }
}

/*** Calculate Partial ***/
uint8_t crc8_calculatePartial(const crc8_configuration_t * const Configuration, uint8_t crc, const uint8_t Data, const bool Last)
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
                crc ^= misc_reflect8(Data);
            else
                crc ^= Data;

            /* Calculate */
            for(uint8_t bit = 0; bit < 8; bit++)
            {
                if((crc & 0x80) == 0x80)
                    crc = (crc << 1) ^ Configuration->polynomial;
                else
                    crc <<= 1;
            }

            /* Last */
            if(Last)
            {
                crc ^= Configuration->xorOut;
                if(Configuration->reflectOut)
                    crc = misc_reflect8(crc);
            }
        }
        else
        {
            /* Lookup Table */
            crc = Configuration->lookupTable[crc ^ Data];
            if(Last)
                crc ^= Configuration->xorOut;
        }
    }

    return crc;
}

/*** Deinitialize ***/
void crc8_deinit(crc8_configuration_t * const configuration)
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
void crc8_init(const char * const Name, crc8_configuration_t * const configuration, const bool CreateLookupTable)
{
    /*** Initialize ***/
    /* Error Check */
    if((Name != NULL) && (configuration != NULL))
    {
        bool found = false;
        size_t i;

        /* Algorithm */
        for(i = 0; i < crc8_ConfigurationCount; i++)
        {
            /* Name */
            if(strcmp(Name, crc8_Configuration[i].name) == 0)
            {
                /* Found */
                found = true;
                break;
            }
            else
            {
                /* Alias */
                for(size_t j = 0; j < crc8_Configuration[i].aliasCount; j++)
                {
                    if(strcmp(Name, crc8_Configuration[i].alias[j]) == 0)
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
            (void)memcpy(configuration, &crc8_Configuration[i], sizeof(crc8_Configuration[i]));

            /* Generate Lookup Table */
            if(CreateLookupTable)
            {
                uint8_t *lookupTable;

                if((lookupTable = memory_malloc(CRC8_LOOKUP_TABLE_MEMORY_SIZE)) != NULL)
                {
                    for(i = 0; i < 256; i++)
                        lookupTable[i] = crc8_calculatePartial(configuration, 0x00, (uint8_t)i, true) ^ configuration->xorOut;
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
bool crc8_verify(const crc8_configuration_t * const Configuration, const uint8_t *Buffer, const size_t BufferLength)
{
    bool verified = false;

    /*** Verify ***/
    /* Error Check */
    if((Configuration != NULL) && (Buffer != NULL))
    {
        const uint8_t Crc = crc8_calculate(Configuration, Buffer, BufferLength);
        verified = ((Crc ^ Configuration->xorOut) == Configuration->residue);
    }

    return verified;
}
