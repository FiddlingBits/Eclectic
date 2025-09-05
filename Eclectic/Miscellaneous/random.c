/****************************************************************************************************
 * Include
 ****************************************************************************************************/

#include "Eclectic/Miscellaneous/project.h"
#include "Eclectic/Miscellaneous/random.h"
#include <stdint.h>
#include <string.h>

/****************************************************************************************************
 * Variable
 ****************************************************************************************************/

PROJECT_STATIC(random_U32Callback_t random_U32Callback);

/****************************************************************************************************
 * Function Definition (Public)
 ****************************************************************************************************/

/*** Initialize ***/
void random_init(const random_U32Callback_t U32Callback)
{
    /*** Initialize ***/
    random_U32Callback = U32Callback;
}

/*** Signed 32-Bit ***/
int32_t random_S32(void)
{
    int32_t s32;

    /*** Signed 32-Bit ***/
    /* Error Check */
    if(random_U32Callback != NULL)
        s32 = (int32_t)random_U32Callback();
    else
        s32 = 0;

    return s32;
}

/*** Signed 32-Bit Within Range ***/
int32_t random_S32WithinRange(const int32_t MinimumS32, const int32_t MaximumS32)
{
    int32_t s32;

    /*** Signed 32-Bit Within Range ***/
    /* Error Check */
    if((random_U32Callback != NULL) && (MinimumS32 <= MaximumS32))
        s32 = ((int32_t)random_U32WithinRange(0, (uint32_t)(MaximumS32 - MinimumS32)) + MinimumS32);
    else
        s32 = 0;

    return s32;
}

/*** Unsigned 32-Bit ***/
uint32_t random_U32(void)
{
    uint32_t u32;

    /*** Unsigned 32-Bit ***/
    /* Error Check */
    if(random_U32Callback != NULL)
        u32 = random_U32Callback();
    else
        u32 = 0;

    return u32;
}

/*** Unsigned 32-Bit Within Range ***/
uint32_t random_U32WithinRange(const uint32_t MinimumU32, const uint32_t MaximumU32)
{
    uint32_t u32;

    /*** Unsigned 32-Bit Within Range ***/
    /* Error Check */
    if((random_U32Callback != NULL) && (MinimumU32 <= MaximumU32))
        u32 = (random_U32Callback() % (((uint64_t)MaximumU32 - MinimumU32 + 1))) + MinimumU32;
    else
        u32 = 0;

    return u32;
}

/*** Set Buffer ***/
void random_setBuffer(uint8_t * const buffer, const size_t BufferLength)
{
    /*** Set Buffer ***/
    /* Error Check */
    if((buffer != NULL) && (BufferLength > 0))
    {
        /* Set Buffer */
        if(random_U32Callback != NULL)
        {
            for(size_t i = 0; i < BufferLength; i++)
                buffer[i] = (uint8_t)random_U32Callback();
        }
        else
        {
            memset(buffer, 0, BufferLength);
        }
    }
}
