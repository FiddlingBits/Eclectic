/****************************************************************************************************
* Include
 ****************************************************************************************************/

#include "Eclectic/misc.h"
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
