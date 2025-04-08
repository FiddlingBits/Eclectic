/****************************************************************************************************
 * Include
 ****************************************************************************************************/

#include "Eclectic/crc16.h"
#include "Eclectic/memory.h"
#include "Eclectic/misc.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/****************************************************************************************************
 * Constant
 ****************************************************************************************************/

const crc16_configuration_t crc16_Configuration[] =
{
    {{"ARC", "CRC-16", "CRC-16/LHA", "CRC-IBM"}, 4, 0xBB3D, 0x0000, NULL, "CRC-16/ARC", 0x8005, true, true, 0x0000, 0x0000},
    {{}, 0, 0x4C06, 0xFFFF, NULL, "CRC-16/CDMA2000", 0xC867, false, false, 0x0000, 0x0000},
    {{}, 0, 0xAEE7, 0xFFFF, NULL, "CRC-16/CMS", 0x8005, false, false, 0x0000, 0x0000},
    {{}, 0, 0x9ECF, 0x800D, NULL, "CRC-16/DDS-110", 0x8005, false, false, 0x0000, 0x0000},
    {{"R-CRC-16"}, 1, 0x007E, 0x0000, NULL, "CRC-16/DECT-R", 0x0589, false, false, 0x0589, 0x0001},
    {{"X-CRC-16"}, 1, 0x007F, 0x0000, NULL, "CRC-16/DECT-X", 0x0589, false, false, 0x0000, 0x0000},
    {{}, 0, 0xEA82, 0x0000, NULL, "CRC-16/DNP", 0x3D65, true, true, 0x66C5, 0xFFFF},
    {{}, 0, 0xC2B7, 0x0000, NULL, "CRC-16/EN-13757", 0x3D65, false, false, 0xA366, 0xFFFF},
    {{"CRC-16/DARC", "CRC-16/EPC", "CRC-16/EPC-C1G2", "CRC-16/I-CODE"}, 4, 0xD64E, 0xFFFF, NULL, "CRC-16/GENIBUS", 0x1021, false, false, 0x1D0F, 0xFFFF},
    {{}, 0, 0xCE3C, 0x0000, NULL, "CRC-16/GSM", 0x1021, false, false, 0x1D0F, 0xFFFF},
    {{"CRC-16/AUTOSAR", "CRC-16/CCITT-FALSE"}, 2, 0x29B1, 0xFFFF, NULL, "CRC-16/IBM-3740", 0x1021, false, false, 0x0000, 0x0000},
    {{"CRC-16/ISO-HDLC", "CRC-16/ISO-IEC-14443-3-B", "CRC-16/X-25", "CRC-B", "X-25"}, 5, 0x906E, 0xFFFF, NULL, "CRC-16/IBM-SDLC", 0x1021, true, true, 0xF0B8, 0xFFFF},
    {{"CRC-A"}, 1, 0xBF05, 0xC6C6, NULL, "CRC-16/ISO-IEC-14443-3-A", 0x1021, true, true, 0x0000, 0x0000},
    {{"CRC-16/BLUETOOTH", "CRC-16/CCITT", "CRC-16/CCITT-TRUE", "CRC-16/V-41-LSB", "CRC-CCITT", "KERMIT"}, 6, 0x2189, 0x0000, NULL, "CRC-16/KERMIT", 0x1021, true, true, 0x0000, 0x0000},
    {{}, 0, 0xBDF4, 0x0000, NULL, "CRC-16/LJ1200", 0x6F63, false, false, 0x0000, 0x0000},
    {{}, 0, 0x772B, 0xFFFF, NULL, "CRC-16/M17", 0x5935, false, false, 0x0000, 0x0000},
    {{"CRC-16/MAXIM"}, 1, 0x44C2, 0x0000, NULL, "CRC-16/MAXIM-DOW", 0x8005, true, true, 0xB001, 0xFFFF},
    {{}, 0, 0x6F91, 0xFFFF, NULL, "CRC-16/MCRF4XX", 0x1021, true, true, 0x0000, 0x0000},
    {{"MODBUS"}, 1, 0x4B37, 0xFFFF, NULL, "CRC-16/MODBUS", 0x8005, true, true, 0x0000, 0x0000},
    {{}, 0, 0xA066, 0xFFFF, NULL, "CRC-16/NRSC-5", 0x080B, true, true, 0x0000, 0x0000},
    {{}, 0, 0x5D38, 0x0000, NULL, "CRC-16/OPENSAFETY-A", 0x5935, false, false, 0x0000, 0x0000},
    {{}, 0, 0x20FE, 0x0000, NULL, "CRC-16/OPENSAFETY-B", 0x755B, false, false, 0x0000, 0x0000},
    {{"CRC-16/IEC-61158-2"}, 1, 0xA819, 0xFFFF, NULL, "CRC-16/PROFIBUS", 0x1DCF, false, false, 0xE394, 0xFFFF},
    {{}, 0, 0x63D0, 0xB2AA, NULL, "CRC-16/RIELLO", 0x1021, true, true, 0x0000, 0x0000},
    {{"CRC-16/AUG-CCITT"}, 1, 0xE5CC, 0x1D0F, NULL, "CRC-16/SPI-FUJITSU", 0x1021, false, false, 0x0000, 0x0000},
    {{}, 0, 0xD0DB, 0x0000, NULL, "CRC-16/T10-DIF", 0x8BB7, false, false, 0x0000, 0x0000},
    {{}, 0, 0x0FB3, 0x0000, NULL, "CRC-16/TELEDISK", 0xA097, false, false, 0x0000, 0x0000},
    {{}, 0, 0x26B1, 0x89EC, NULL, "CRC-16/TMS37157", 0x1021, true, true, 0x0000, 0x0000},
    {{"CRC-16/BUYPASS", "CRC-16/VERIFONE"}, 2, 0xFEE8, 0x0000, NULL, "CRC-16/UMTS", 0x8005, false, false, 0x0000, 0x0000},
    {{}, 0, 0xB4C8, 0xFFFF, NULL, "CRC-16/USB", 0x8005, true, true, 0xB001, 0xFFFF},
    {{"CRC-16/ACORN", "CRC-16/LTE", "CRC-16/V-41-MSB", "XMODEM", "ZMODEM"}, 5, 0x31C3, 0x0000, NULL, "CRC-16/XMODEM", 0x1021, false, false, 0x0000, 0x0000}
};
const size_t crc16_ConfigurationCount = sizeof(crc16_Configuration) / sizeof(crc16_Configuration[0]);

/****************************************************************************************************
 * Function Definition (Public)
 ****************************************************************************************************/

/*** Calculate ***/
uint16_t crc16_calculate(const crc16_configuration_t * const Configuration, const uint8_t *Data, const size_t DataLength)
{
    uint16_t crc = 0x0000;

    /*** Calculate ***/
    /* Error Check */
    if((Configuration != NULL) && (Data != NULL))
    {
        /* Set Up */
        crc = Configuration->initial;

        /* Calculate */
        for(size_t i = 0; i < DataLength; i++)
            crc = crc16_calculatePartial(Configuration, crc, Data[i], (i == 0), (i == (DataLength - 1)));
    }

    return crc;
}

/*** Calculate And Append ***/
void crc16_calculateAndAppend(const crc16_configuration_t * const Configuration, uint8_t *const buffer, const size_t BufferLength, const size_t DataLength)
{
    uint16_t crc = 0x0000;

    /*** Calculate And Append ***/
    if((Configuration != NULL) && (buffer != NULL) && (BufferLength >= (DataLength + sizeof(crc))))
    {
        crc = crc16_calculate(Configuration, buffer, DataLength);
        misc_insert16(buffer + DataLength, BufferLength - DataLength, crc, !Configuration->reflectOut);
    }
}

/*** Calculate Partial ***/
uint16_t crc16_calculatePartial(const crc16_configuration_t * const Configuration, uint16_t crc, const uint8_t Data, const bool First, const bool Last)
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
                crc ^= (misc_reflect8(Data) << 8);
            else
                crc ^= (Data << 8);

            /* Calculate */
            for(size_t bit = 0; bit < 8; bit++)
            {
                if((crc & 0x8000) == 0x8000)
                    crc = (crc << 1) ^ Configuration->polynomial;
                else
                    crc <<= 1;
            }

            /* Last */
            if(Last)
            {
                crc ^= Configuration->xorOut;
                if(Configuration->reflectOut)
                    crc = misc_reflect16(crc);
            }
        }
        else
        {
            /* Reflect */
            if(Configuration->reflectIn)
            {
                /* Reflect */
                if(First)
                    crc = misc_reflect16(crc);

                /* Lookup Table */
                crc = (crc >> 8) ^ Configuration->lookupTable[(crc ^ Data) % 256];
            }
            else
            {
                /* Lookup Table */
                crc = (crc << 8) ^ Configuration->lookupTable[(crc >> 8) ^ Data];
            }

            /* Last */
            if(Last)
                crc ^= Configuration->xorOut;
        }
    }

    return crc;
}

/*** Deinitialize ***/
void crc16_deinit(crc16_configuration_t * const configuration)
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
void crc16_init(const char * const Name, crc16_configuration_t * const configuration, const bool CreateLookupTable)
{
    /*** Initialize ***/
    /* Error Check */
    if((Name != NULL) && (configuration != NULL))
    {
        bool found = false;
        size_t i;

        /* Algorithm */
        for(i = 0; i < crc16_ConfigurationCount; i++)
        {
            /* Name */
            if(strcmp(Name, crc16_Configuration[i].name) == 0)
            {
                /* Found */
                found = true;
                break;
            }
            else
            {
                /* Alias */
                for(size_t j = 0; j < crc16_Configuration[i].aliasCount; j++)
                {
                    if(strcmp(Name, crc16_Configuration[i].alias[j]) == 0)
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
            (void)memcpy(configuration, &crc16_Configuration[i], sizeof(crc16_Configuration[i]));

            /* Generate Lookup Table */
            if(CreateLookupTable)
            {
                uint16_t *lookupTable;

                if((lookupTable = memory_malloc(CRC16_LOOKUP_TABLE_MEMORY_SIZE)) != NULL)
                {
                    for(i = 0; i < 256; i++)
                        lookupTable[i] = crc16_calculatePartial(configuration, 0x0000, (uint8_t)i, true, true) ^ configuration->xorOut;
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
bool crc16_verify(const crc16_configuration_t * const Configuration, const uint8_t *Buffer, const size_t BufferLength)
{
    bool verified = false;

    /*** Verify ***/
    /* Error Check */
    if((Configuration != NULL) && (Buffer != NULL))
    {
        const uint16_t Crc = crc16_calculate(Configuration, Buffer, BufferLength);
        verified = ((Crc ^ Configuration->xorOut) == Configuration->residue);
    }

    return verified;
}
