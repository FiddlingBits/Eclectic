/****************************************************************************************************
 * Pragmas
 ****************************************************************************************************/

#pragma once

/****************************************************************************************************
 * Includes
 ****************************************************************************************************/

#include "Eclectic/Data/list.h"

/****************************************************************************************************
 * Type Definitions
 ****************************************************************************************************/

/*** Enumerations ***/
typedef enum fsm_signal_e
{
    FSM_SIGNAL_ENTRY,
    FSM_SIGNAL_EXECUTE,
    FSM_SIGNAL_EXIT,
    FSM_SIGNAL_USER_START
} fsm_signal_t;

/*** Structures ***/
typedef struct fsm_event_s
{
    int signal;
} fsm_event_t;

/*** Callbacks ***/
/* Forward Declaration */
typedef struct fsm_s fsm_t;

/* Callback */
typedef void (*fsm_state_t)(fsm_t * const fsm, fsm_event_t * const event);

/*** Structures ***/
typedef struct fsm_s
{
    fsm_state_t currentState;
    list_list_t eventList;
} fsm_t;

/****************************************************************************************************
 * Function Prototypes
 ****************************************************************************************************/

extern void fsm_deinit(fsm_t * const fsm);
extern void fsm_dispatch(fsm_t * const fsm, fsm_event_t * const event);
extern void fsm_init(fsm_t * const fsm, const fsm_state_t InitialState);
extern void fsm_tick(fsm_t * const fsm);
extern void fsm_transition(fsm_t * const fsm, const fsm_state_t NextState);
