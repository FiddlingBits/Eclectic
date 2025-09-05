/****************************************************************************************************
* Include
 ****************************************************************************************************/

#include "Eclectic/Miscellaneous/project.h"
#include "Eclectic/Miscellaneous/memory.h"

/****************************************************************************************************
 * Variable
 ****************************************************************************************************/

PROJECT_STATIC(memory_freeCallback_t memory_freeCallback);
PROJECT_STATIC(memory_mallocCallback_t memory_mallocCallback);

/****************************************************************************************************
 * Function Definition (Public)
 ****************************************************************************************************/

/*** Free ***/
void memory_free(void **memory)
{
    /*** Free ***/
    if((memory_freeCallback != NULL) && (memory != NULL))
    {
        memory_freeCallback(*memory);
        *memory = NULL;
    }
}

/*** Initialize ***/
void memory_init(const memory_freeCallback_t FreeCallback, const memory_mallocCallback_t MallocCallback)
{
    /*** Initialize ***/
    memory_freeCallback = FreeCallback;
    memory_mallocCallback = MallocCallback;
}

/*** Malloc ***/
void *memory_malloc(const size_t Size)
{
    /*** Malloc ***/
    if(memory_mallocCallback != NULL)
        return memory_mallocCallback(Size);
    else
        return NULL;
}
