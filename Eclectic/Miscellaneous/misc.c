/****************************************************************************************************
* Include
 ****************************************************************************************************/

#include "Eclectic/Miscellaneous/misc.h"
#include <stddef.h>
#include <stdint.h>

/****************************************************************************************************
 * Function Definition (Public)
 ****************************************************************************************************/

/*** Insert 16 ***/
void misc_insert16(uint8_t * const Buffer, const size_t BufferLength, const uint16_t Data, const bool BigEndian)
{
    /*** Insert 16 ***/
    /* Error Check */
    if((Buffer != NULL) && (BufferLength >= sizeof(uint16_t)))
    {
        /* Insert 16 */
        for(size_t i = 0; i < sizeof(uint16_t); i++)
        {
            if(BigEndian)
                Buffer[i] = (uint8_t)(Data >> (8 - (i * 8)));
            else
                Buffer[i] = (uint8_t)(Data >> (i * 8));
        }
    }
}

/*** Insert 32 ***/
void misc_insert32(uint8_t * const Buffer, const size_t BufferLength, const uint32_t Data, const bool BigEndian)
{
    /*** Insert 32 ***/
    /* Error Check */
    if((Buffer != NULL) && (BufferLength >= sizeof(uint32_t)))
    {
        /* Insert 32 */
        for(size_t i = 0; i < sizeof(uint32_t); i++)
        {
            if(BigEndian)
                Buffer[i] = (uint8_t)(Data >> (24 - (i * 8)));
            else
                Buffer[i] = (uint8_t)(Data >> (i * 8));
        }
    }
}

/*** Insert 64 ***/
void misc_insert64(uint8_t * const Buffer, const size_t BufferLength, const uint64_t Data, const bool BigEndian)
{
    /*** Insert 64 ***/
    /* Error Check */
    if((Buffer != NULL) && (BufferLength >= sizeof(uint64_t)))
    {
        /* Insert 64 */
        for(size_t i = 0; i < sizeof(uint64_t); i++)
        {
            if(BigEndian)
                Buffer[i] = (uint8_t)(Data >> (56 - (i * 8)));
            else
                Buffer[i] = (uint8_t)(Data >> (i * 8));
        }
    }
}

/*** Reflect 8 ***/
uint8_t misc_reflect8(const uint8_t Data)
{
    uint8_t reflectedData = 0x00;

    /*** Reflect 8 ***/
    for(size_t bit = 0; bit < 8; bit++)
        reflectedData |= ((Data & (1 << bit)) >> bit) << (7 - bit);

    return reflectedData;
}

/*** Reflect 16 ***/
uint16_t misc_reflect16(const uint16_t Data)
{
    uint16_t reflectedData = 0x0000;

    /*** Reflect 16 ***/
    for(size_t bit = 0; bit < 16; bit++)
        reflectedData |= ((Data & (1 << bit)) >> bit) << (15 - bit);

    return reflectedData;
}

/*** Reflect 32 ***/
uint32_t misc_reflect32(const uint32_t Data)
{
    uint32_t reflectedData = 0x00000000;

    /*** Reflect 32 ***/
    for(size_t bit = 0; bit < 32; bit++)
        reflectedData |= ((Data & (1 << bit)) >> bit) << (31 - bit);

    return reflectedData;
}

/*** Reflect 64 ***/
uint64_t misc_reflect64(const uint64_t Data)
{
    uint64_t reflectedData = 0x0000000000000000;

    /*** Reflect 64 ***/
    for(size_t bit = 0; bit < 64; bit++)
        reflectedData |= ((Data & ((uint64_t)1 << bit)) >> bit) << (63 - bit);

    return reflectedData;
}
