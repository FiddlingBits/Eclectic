/****************************************************************************************************
 * Include
 ****************************************************************************************************/

#include "fsm.h"
#include "mock_list.h"
#include "unity.h"
#include <limits.h>

/****************************************************************************************************
 * Helper
 ****************************************************************************************************/

/*** State 1 ***/
/* Variables */
static int helper_stateSignal_1;

/* State */
static void helper_state_1(fsm_t * const fsm, fsm_event_t * const event)
{
    /*** State ***/
    helper_stateSignal_1 = event->signal;
}

/*** State 2 ***/
/* Variables */
static int helper_stateSignal_2;

/* State */
static void helper_state_2(fsm_t * const fsm, fsm_event_t * const event)
{
    /*** State ***/
    helper_stateSignal_2 = event->signal;
}

/****************************************************************************************************
 * Set Up/Tear Down
 ****************************************************************************************************/

void setUp(void)
{
    helper_stateSignal_1 = INT_MIN;
    helper_stateSignal_2 = INT_MIN;
}

void tearDown(void)
{
}

/****************************************************************************************************
 * Test (Public)
 ****************************************************************************************************/

/*** Deinitialize ***/
void test_deinit_1(void)
{
    /*** Deinitialize (FSM NULL) ***/
    fsm_deinit(NULL);
}

void test_deinit_2(void)
{
    /*** Deinitialize (Current State NULL) ***/
    /* Variables */
    fsm_t fsm;

    /* Set Up */
    fsm.currentState = NULL;

    /* Mock */
    list_deinit_Expect(&fsm.eventList);

    /* Deinitialize */
    fsm_deinit(&fsm);
}

void test_deinit_3(void)
{
    /*** Deinitialize ***/
    /* Variables */
    fsm_t fsm;

    /* Set Up */
    fsm.currentState = helper_state_1;

    /* Mock */
    list_deinit_Expect(&fsm.eventList);

    /* Deinitialize */
    fsm_deinit(&fsm);

    /* Verify */
    TEST_ASSERT_EQUAL_INT(FSM_SIGNAL_EXIT, helper_stateSignal_1);
}

/*** Dispatch ***/
void test_dispatch_1(void)
{
    /*** Dispatch (FSM NULL) ***/
    /* Variables */
    fsm_event_t event;

    /* Dispatch */
    fsm_dispatch(NULL, &event);
}

void test_dispatch_2(void)
{
    /*** Dispatch (Event NULL) ***/
    /* Variables */
    fsm_t fsm;

    /* Dispatch */
    fsm_dispatch(&fsm, NULL);
}

void test_dispatch_3(void)
{
    /*** Dispatch ***/
    /* Variables */
    fsm_event_t event;
    fsm_t fsm;

    /* Mock */
    list_pushTail_Expect(&fsm.eventList, &event);

    /* Dispatch */
    fsm_dispatch(&fsm, &event);
}

/*** Initialize ***/
void test_initialize_1(void)
{
    /*** Initialize (FSM NULL) ***/
    fsm_init(NULL, helper_state_1);
}

void test_initialize_2(void)
{
    /*** Initialize (Initial State NULL) ***/
    /* Variables */
    fsm_t fsm;

    /* Initialize */
    fsm_init(&fsm, NULL);
}

void test_initialize_3(void)
{
    /*** Initialize ***/
    /* Variables */
    fsm_t fsm;

    /* Mock */
    list_init_Expect(&fsm.eventList, NULL, NULL);

    /* Initialize */
    fsm_init(&fsm, helper_state_1);

    /* Verify */
    TEST_ASSERT_EQUAL_PTR(helper_state_1, fsm.currentState);
    TEST_ASSERT_EQUAL_INT(FSM_SIGNAL_ENTRY, helper_stateSignal_1);
}

/*** Tick ***/
void test_tick_1(void)
{
    /*** Tick (FSM NULL) ***/
    fsm_tick(NULL);
}

void test_tick_2(void)
{
    /*** Tick (Current State NULL) ***/
    /* Variables */
    fsm_t fsm;

    /* Set Up */
    fsm.currentState = NULL;

    /* Tick */
    fsm_tick(&fsm);
}

void test_tick_3(void)
{
    /*** Tick (Event List Empty) ***/
    /* Variables */
    fsm_t fsm;

    /* Set Up */
    fsm.currentState = helper_state_1;
    fsm.eventList.size = 0;

    /* Tick */
    fsm_tick(&fsm);

    /* Verify */
    TEST_ASSERT_EQUAL_INT(FSM_SIGNAL_EXECUTE, helper_stateSignal_1);
}

void test_tick_4(void)
{
    /*** Tick (Event List Not Empty) ***/
    /* Variables */
    fsm_event_t event;
    fsm_t fsm;

    /* Set Up */
    event.signal = FSM_SIGNAL_USER_START;
    fsm.currentState = helper_state_1;
    fsm.eventList.size = 1;

    /* Mock */
    list_popHead_ExpectAndReturn(&fsm.eventList, &event);

    /* Tick */
    fsm_tick(&fsm);

    /* Verify */
    TEST_ASSERT_EQUAL_INT(FSM_SIGNAL_USER_START, helper_stateSignal_1);
}

/*** Transition ***/
void test_transition_1(void)
{
    /*** Transition (FSM NULL) ***/
    fsm_transition(NULL, helper_state_1);
}

void test_transition_2(void)
{
    /*** Transition (Next State NULL) ***/
    /* Variables */
    fsm_t fsm;

    /* Transition */
    fsm_transition(&fsm, NULL);
}

void test_transition_3(void)
{
    /*** Transition (Current State NULL) ***/
    /* Variables */
    fsm_t fsm;

    /* Set Up */
    fsm.currentState = NULL;

    /* Transition */
    fsm_transition(&fsm, helper_state_1);

    /* Verify */
    TEST_ASSERT_EQUAL_PTR(helper_state_1, fsm.currentState);
    TEST_ASSERT_EQUAL_INT(FSM_SIGNAL_ENTRY, helper_stateSignal_1);
}

void test_transition_4(void)
{
    /*** Transition (Current State Not NULL) ***/
    /* Variables */
    fsm_t fsm;

    /* Set Up */
    fsm.currentState = helper_state_1;
    fsm.eventList.size = 1;

    /* Transition */
    fsm_transition(&fsm, helper_state_2);

    /* Verify */
    TEST_ASSERT_EQUAL_PTR(helper_state_2, fsm.currentState);
    TEST_ASSERT_EQUAL_INT(FSM_SIGNAL_EXIT, helper_stateSignal_1);
    TEST_ASSERT_EQUAL_INT(FSM_SIGNAL_ENTRY, helper_stateSignal_2);
}