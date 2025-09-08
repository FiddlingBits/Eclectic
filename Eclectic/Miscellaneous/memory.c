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
    /* Error Check */
    if((memory_freeCallback != NULL) && (memory != NULL))
    {
        /* Free */
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
    /* Error Check */
    if(memory_mallocCallback != NULL)
        return memory_mallocCallback(Size); // Malloc
    else
        return NULL; // Error
}
