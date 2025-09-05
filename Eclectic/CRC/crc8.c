/****************************************************************************************************
 * Include
 ****************************************************************************************************/

#include "Eclectic/CRC/crc8.h"
#include "Eclectic/Miscellaneous/memory.h"
#include "Eclectic/Miscellaneous/misc.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/****************************************************************************************************
 * Constant
 ****************************************************************************************************/

const crc8_configuration_t crc8_Configuration[] =
{
    {{}, 0, 0xDF, 0xFF, NULL, "CRC-8/AUTOSAR", 0x2F, false, false, 0x42, 0xFF},
    {{}, 0, 0x26, 0x00, NULL, "CRC-8/BLUETOOTH", 0xA7, true, true, 0x00, 0x00},
    {{}, 0, 0xDA, 0xFF, NULL, "CRC-8/CDMA2000", 0x9B, false, false, 0x00, 0x00},
    {{}, 0, 0x15, 0x00, NULL, "CRC-8/DARC", 0x39, true, true, 0x00, 0x00},
    {{}, 0, 0xBC, 0x00, NULL, "CRC-8/DVB-S2", 0xD5, false, false, 0x00, 0x00},
    {{}, 0, 0x37, 0x00, NULL, "CRC-8/GSM-A", 0x1D, false, false, 0x00, 0x00},
    {{}, 0, 0x94, 0x00, NULL, "CRC-8/GSM-B", 0x49, false, false, 0x53, 0xFF},
    {{}, 0, 0xB4, 0xFF, NULL, "CRC-8/HITAG", 0x1D, false, false, 0x00, 0x00},
    {{"CRC-8/ITU"}, 1, 0xA1, 0x00, NULL, "CRC-8/I-432-1", 0x07, false, false, 0xAC, 0x55},
    {{}, 0, 0x7E, 0xFD, NULL, "CRC-8/I-CODE", 0x1D, false, false, 0x00, 0x00},
    {{}, 0, 0xEA, 0x00, NULL, "CRC-8/LTE", 0x9B, false, false, 0x00, 0x00},
    {{"CRC-8/MAXIM", "DOW-CRC"}, 2, 0xA1, 0x00, NULL, "CRC-8/MAXIM-DOW", 0x31, true, true, 0x00, 0x00},
    {{}, 0, 0x99, 0xC7, NULL, "CRC-8/MIFARE-MAD", 0x1D, false, false, 0x00, 0x00},
    {{}, 0, 0xF7, 0xFF, NULL, "CRC-8/NRSC-5", 0x31, false, false, 0x00, 0x00},
    {{}, 0, 0x3E, 0x00, NULL, "CRC-8/OPENSAFETY", 0x2F, false, false, 0x00, 0x00},
    {{}, 0, 0xD0, 0xFF, NULL, "CRC-8/ROHC", 0x07, true, true, 0x00, 0x00},
    {{}, 0, 0x4B, 0xFF, NULL, "CRC-8/SAE-J1850", 0x1D, false, false, 0xC4, 0xFF},
    {{"CRC-8"}, 1, 0xF4, 0x00, NULL, "CRC-8/SMBUS", 0x07, false, false, 0x00, 0x00},
    {{"CRC-8/AES", "CRC-8/EBU"}, 2, 0x97, 0xFF, NULL, "CRC-8/TECH-3250", 0x1D, true, true, 0x00, 0x00},
    {{}, 0, 0x25, 0x00, NULL, "CRC-8/WCDMA", 0x9B, true, true, 0x00, 0x00}
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
            for(size_t bit = 0; bit < 8; bit++)
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
