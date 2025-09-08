/****************************************************************************************************
 * Include
 ****************************************************************************************************/

#include "Eclectic/Miscellaneous/random.h"
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/random.h> 

/****************************************************************************************************
 * Function Definition (Public)
 ****************************************************************************************************/

/*** Buffer ***/
void random_buffer(uint8_t * const buffer, const size_t BufferLength)
{
    /*** Buffer ***/
    /* Error Check */
    if((buffer != NULL) && (BufferLength > 0))
        (void)getrandom(buffer, BufferLength, 0); // Buffer
}

/*** Signed 32-Bit ***/
int32_t random_S32(void)
{
    /*** Signed 32-Bit ***/    
    /* Exit */
    return (int32_t)random_U32();
}

/*** Signed 32-Bit Within Inclusive Range ***/
int32_t random_S32WithinRange(const int32_t MinimumS32, const int32_t MaximumS32)
{
    /*** Signed 32-Bit Within Inclusive Range ***/
    /* Variable */
    uint32_t u32;
    uint64_t range, max;
    
    /* Set Up */
    u32 = 0;
    
    /* Error Check */
    if(MinimumS32 <= MaximumS32)
    {
        /* Signed 32-Bit Within Inclusive Range */
        if(MinimumS32 == MaximumS32)
        {
            /* Zero Range */
            u32 = MinimumS32;
        }
        else if((MinimumS32 == INT32_MIN) && (MaximumS32 == INT32_MAX))
        {
            /* Full Range */
            u32 = random_S32();
        }
        else
        {
            /* Convert Signed Range To Unsigned */
            range = (uint64_t)((int64_t)MaximumS32 - (int64_t)MinimumS32 + 1);
            max = UINT32_MAX - (UINT32_MAX % (uint32_t)range);
            
            /* Generate Without Bias */
            do
            {
                /* Unsigned 32-Bit */
                u32 = random_U32();
            } while(u32 >= (uint32_t)max); // Don't Allow Bias
            
            /* Convert Back To Signed */
            u32 = MinimumS32 + (int32_t)(u32 % (uint32_t)range);
        }
    }
    
    /* Exit */
    return u32;
}

/*** Unsigned 32-Bit ***/
uint32_t random_U32(void)
{
    /*** Unsigned 32-Bit ***/
    /* Variable */
    uint32_t u32;
    
    /* Unsigned 32-Bit */
    (void)getrandom(&u32, sizeof(u32), 0);
    
    /* Exit */
    return u32;
}

/*** Unsigned 32-Bit Within Inclusive Range ***/
uint32_t random_U32WithinRange(const uint32_t MinimumU32, const uint32_t MaximumU32)
{
    /*** Unsigned 32-Bit Within Inclusive Range ***/
    /* Variable */
    uint32_t max, range, u32;
    
    /* Set Up */
    u32 = 0;
    
    /* Error Check */
    if(MinimumU32 <= MaximumU32)
    {
        /* Unsigned 32-Bit Within Inclusive Range */
        if(MinimumU32 == MaximumU32)
        {
            /* Zero Range */
            u32 = MinimumU32;
        }
        else if((MinimumU32 == 0) && (MaximumU32 == UINT32_MAX))
        {
            /* Full Range */
            u32 = random_U32();
        }
        else
        {
            /* Unsigned 32-Bit Within Inclusive Range */
            range = MaximumU32 - MinimumU32 + 1;
            max = UINT32_MAX - (UINT32_MAX % range);
            do
            {
                /* Unsigned 32-Bit */
                u32 = random_U32();
            } while(u32 >= max); // Don't Allow Bias
            u32 = MinimumU32 + (u32 % range);
        }
    }

    /* Exit */
    return u32;
}
