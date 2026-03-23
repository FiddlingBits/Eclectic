/****************************************************************************************************
 * Includes
 ****************************************************************************************************/

#include "Eclectic/Data/list.h"
#include "Eclectic/FSM/fsm.h"
#include <stddef.h>
#include <string.h>

/****************************************************************************************************
 * Variables
 ****************************************************************************************************/

static fsm_event_t fsm_eventEntry = {FSM_SIGNAL_ENTRY};
static fsm_event_t fsm_eventExecute = {FSM_SIGNAL_EXECUTE};
static fsm_event_t fsm_eventExit = {FSM_SIGNAL_EXIT};

/****************************************************************************************************
 * Function Definitions (Public)
 ****************************************************************************************************/

/*** Deinitialize ***/
void fsm_deinit(fsm_t * const fsm)
{
    /*** Deinitialize ***/
    /* Error Check */
    if(fsm != NULL)
    {
        /* Exit Current State */
        if(fsm->currentState != NULL)
            fsm->currentState(fsm, &fsm_eventExit);

        /* Deinitialize */
        list_deinit(&fsm->eventList);

        /* Reset */
        (void)memset(fsm, 0, sizeof(*fsm));
    }
}

/*** Dispatch ***/
void fsm_dispatch(fsm_t * const fsm, fsm_event_t * const event)
{
    /*** Dispatch ***/
    /* Error Check */
    if((fsm != NULL) && (event != NULL))
    {
        /* Push Event To List */
        list_pushTail(&fsm->eventList, event);
    }
}

/*** Initialize ***/
void fsm_init(fsm_t * const fsm, const fsm_state_t InitialState)
{
    /*** Initialize ***/
    /* Error Check */
    if((fsm != NULL) && (InitialState != NULL))
    {
        /* Set Up */
        fsm->currentState = InitialState;

        /* Initialize */
        list_init(&fsm->eventList, NULL, NULL);

        /* Enter Initial State */
        fsm->currentState(fsm, &fsm_eventEntry);
    }
}

/*** Tick ***/
void fsm_tick(fsm_t * const fsm)
{
    /*** Tick ***/
    /* Variable */
    fsm_event_t *event;

    /* Error Check */
    if((fsm != NULL) && (fsm->currentState != NULL))
    {
        /* Process Event Or Execute Current State */
        if(fsm->eventList.size > 0)
        {
            /* Dequeue */
            event = list_popHead(&fsm->eventList);

            /* Dispatch To Current State */
            fsm->currentState(fsm, event);
        }
        else
        {
            /* Execute Current State */
            fsm->currentState(fsm, &fsm_eventExecute);
        }
    }
}

/*** Transition ***/
void fsm_transition(fsm_t * const fsm, const fsm_state_t NextState)
{
    /*** Transition ***/
    /* Error Check */
    if((fsm != NULL) && (NextState != NULL))
    {
        /* Exit Current State */
        if(fsm->currentState != NULL)
            fsm->currentState(fsm, &fsm_eventExit);

        /* Change State */
        fsm->currentState = NextState;

        /* Enter New State */
        fsm->currentState(fsm, &fsm_eventEntry);
    }
}
