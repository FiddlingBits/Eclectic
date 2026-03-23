/****************************************************************************************************
 * Defines
 ****************************************************************************************************/

#define TRAFFIC_LIGHT_GREEN_DURATION (5)
#define TRAFFIC_LIGHT_YELLOW_DURATION (2)

/****************************************************************************************************
 * Includes
 ****************************************************************************************************/

#include <curses.h>
#include "Eclectic/FSM/fsm.h"
#include <stddef.h>
#include <string.h>
#include <time.h>
#include "traffic.h"

/****************************************************************************************************
 * Type Definitions
 ****************************************************************************************************/

/*** Enumerations ***/
typedef enum traffic_signal_e
{
    TRAFFIC_SIGNAL_ENTRY = FSM_SIGNAL_ENTRY,
    TRAFFIC_SIGNAL_EXECUTE = FSM_SIGNAL_EXECUTE,
    TRAFFIC_SIGNAL_EXIT = FSM_SIGNAL_EXIT,
    TRAFFIC_SIGNAL_TIMEOUT = FSM_SIGNAL_USER_START,
    TRAFFIC_SIGNAL_TURN_GREEN,
} traffic_signal_t;

/*** Structures ***/
typedef struct traffic_data_s
{
    time_t startTime;
} traffic_data_t;

/****************************************************************************************************
 * Function Prototypes
 ****************************************************************************************************/

static void traffic_drawLight(WINDOW * const window, const char * const Title, const int ColorPair);
static void traffic_eastWestGreenState(fsm_t * const fsm, fsm_event_t * const event);
static void traffic_eastWestRedState(fsm_t * const fsm, fsm_event_t * const event);
static void traffic_eastWestYellowState(fsm_t * const fsm, fsm_event_t * const event);
static void traffic_northSouthGreenState(fsm_t * const fsm, fsm_event_t * const event);
static void traffic_northSouthRedState(fsm_t * const fsm, fsm_event_t * const event);
static void traffic_northSouthYellowState(fsm_t * const fsm, fsm_event_t * const event);

/****************************************************************************************************
 * Variables
 ****************************************************************************************************/

static fsm_t traffic_eastWestFsm;
static WINDOW *traffic_eastWestWindow;
static fsm_t traffic_northSouthFsm;
static WINDOW *traffic_northSouthWindow;

/****************************************************************************************************
 * Function Definitions (Public)
 ****************************************************************************************************/

/*** Deinitialize ***/
void traffic_deinit(void)
{
    /*** Deinitialize ***/
    /* Curses */
    (void)delwin(traffic_eastWestWindow);
    (void)delwin(traffic_northSouthWindow);
    (void)endwin();

    /* FSM */
    fsm_deinit(&traffic_eastWestFsm);
    fsm_deinit(&traffic_northSouthFsm);
}

/*** Initialize ***/
void traffic_init(void)
{
    /*** Initialize ***/
    /* Variables */
    int cols, half, rows;

    /* Curses Basic */
    (void)initscr();
    (void)noecho();
    (void)curs_set(0); // Hide Cursor

    /* Curses Colors */
    (void)start_color();
    (void)use_default_colors();
    (void)init_pair(1, COLOR_BLACK, COLOR_RED);
    (void)init_pair(2, COLOR_BLACK, COLOR_YELLOW);
    (void)init_pair(3, COLOR_BLACK, COLOR_GREEN);

    /* Curses Windows */
    (void)getmaxyx(stdscr, rows, cols);
    half = cols / 2;
    traffic_eastWestWindow = derwin(stdscr, rows, half, 0, 0);
    traffic_northSouthWindow = derwin(stdscr, rows, half, 0, half);

    /* FSM */
    fsm_init(&traffic_eastWestFsm, traffic_eastWestGreenState);
    fsm_init(&traffic_northSouthFsm, traffic_northSouthRedState);
}

/*** Tick ***/
void traffic_tick(void)
{
    /*** Tick ***/
    /* Variable */
    time_t currentTime;
    static fsm_event_t event;
    static time_t lastTickTime = 0;

    /* Set Up */
    currentTime = time(NULL);
    event.signal = TRAFFIC_SIGNAL_TIMEOUT;
    if(lastTickTime == 0)
        lastTickTime = currentTime;

    /* Post Timeout Every Second */
    if((currentTime - lastTickTime) >= 1)
    {
        /* Dispatch Timeout Event */
        fsm_dispatch(&traffic_eastWestFsm, &event);
        fsm_dispatch(&traffic_northSouthFsm, &event);

        /* Record Last Tick Time */
        lastTickTime = currentTime;
    }

    /* Tick */
    fsm_tick(&traffic_eastWestFsm);
    fsm_tick(&traffic_northSouthFsm);

    /* Update Curses */
    (void)doupdate();
}

/****************************************************************************************************
 * Function Definitions (Private)
 ****************************************************************************************************/

/*** Draw Light ***/
static void traffic_drawLight(WINDOW * const window, const char * const Title, const int ColorPair)
{
    /*** Draw Light ***/
    /* Variables */
    int cols, rows, titleLength, x, y;

    /* Set Up */
    (void)getmaxyx(window, rows, cols);
    titleLength = (int)strlen(Title);

    /* Color Background */
    (void)wbkgd(window, COLOR_PAIR(ColorPair));
    (void)werase(window);

    /* Print Title */
    (void)wattron(window, COLOR_PAIR(ColorPair));
    x = (titleLength >= cols) ? 0 : (cols - titleLength) / 2;
    y = rows / 2;
    (void)mvwprintw(window, y, x, "%s", Title);
    (void)wattroff(window, COLOR_PAIR(ColorPair));

    /* Update Window */
    (void)wnoutrefresh(window);
}

/*** East-West Green State ***/
static void traffic_eastWestGreenState(fsm_t * const fsm, fsm_event_t * const event)
{
    /*** East-West Green State ***/
    /* Variables */
    static time_t startTime;

    /* Handle Signal */
    switch(event->signal)
    {
        case TRAFFIC_SIGNAL_ENTRY:
            /* Entry */
            startTime = time(NULL);
            traffic_drawLight(traffic_eastWestWindow, "East-West", 3);
            break;
        case TRAFFIC_SIGNAL_TIMEOUT:
            /* Timeout */
            if((time(NULL) - startTime) >= TRAFFIC_LIGHT_GREEN_DURATION)
                fsm_transition(fsm, traffic_eastWestYellowState);
            break;
        case TRAFFIC_SIGNAL_EXECUTE:
        case TRAFFIC_SIGNAL_EXIT:
        default:
            /* Do Nothing */
            break;
    }
}

/*** East-West Red State ***/
static void traffic_eastWestRedState(fsm_t * const fsm, fsm_event_t * const event)
{
    /*** East-West Red State ***/
    /* Handle Signal */
    switch(event->signal)
    {
        case TRAFFIC_SIGNAL_ENTRY:
            /* Entry */
            traffic_drawLight(traffic_eastWestWindow, "East-West", 1);
            break;
        case TRAFFIC_SIGNAL_TURN_GREEN:
            /* Turn Green */
            fsm_transition(fsm, traffic_eastWestGreenState);
            break;
        case TRAFFIC_SIGNAL_EXECUTE:
        case TRAFFIC_SIGNAL_EXIT:
        default:
            /* Do Nothing */
            break;
    }
}

/*** East-West Yellow State ***/
static void traffic_eastWestYellowState(fsm_t * const fsm, fsm_event_t * const event)
{
    /*** East-West Yellow State ***/
    /* Variables */
    static fsm_event_t turnGreenEvent;
    static time_t startTime;

    /* Set Up */
    turnGreenEvent.signal = TRAFFIC_SIGNAL_TURN_GREEN;

    /* Handle Signal */
    switch(event->signal)
    {
        case TRAFFIC_SIGNAL_ENTRY:
            /* Entry */
            startTime = time(NULL);
            traffic_drawLight(traffic_eastWestWindow, "East-West", 2);
            break;
        case TRAFFIC_SIGNAL_TIMEOUT:
            /* Timeout */
            if((time(NULL) - startTime) >= TRAFFIC_LIGHT_YELLOW_DURATION)
            {
                /* Transition To Red */
                fsm_transition(fsm, traffic_eastWestRedState);

                /* Notify North-South To Turn Green */
                fsm_dispatch(&traffic_northSouthFsm, &turnGreenEvent);
            }
            break;
        case TRAFFIC_SIGNAL_EXECUTE:
        case TRAFFIC_SIGNAL_EXIT:
        default:
            /* Do Nothing */
            break;
    }
}

/*** North-South Green State ***/
static void traffic_northSouthGreenState(fsm_t * const fsm, fsm_event_t * const event)
{
    /*** North-South Green State ***/
    /* Variables */
    static time_t startTime;

    /* Handle Signal */
    switch(event->signal)
    {
        case TRAFFIC_SIGNAL_ENTRY:
            /* Entry */
            startTime = time(NULL);
            traffic_drawLight(traffic_northSouthWindow, "North-South", 3);
            break;
        case TRAFFIC_SIGNAL_TIMEOUT:
            /* Timeout */
            if((time(NULL) - startTime) >= TRAFFIC_LIGHT_GREEN_DURATION)
                fsm_transition(fsm, traffic_northSouthYellowState);
            break;
        case TRAFFIC_SIGNAL_EXECUTE:
        case TRAFFIC_SIGNAL_EXIT:
        default:
            /* Do Nothing */
            break;
    }
}

/*** North-South Red State ***/
static void traffic_northSouthRedState(fsm_t * const fsm, fsm_event_t * const event)
{
    /*** North-South Red State ***/
    /* Handle Signal */
    switch(event->signal)
    {
        case TRAFFIC_SIGNAL_ENTRY:
            /* Entry */
            traffic_drawLight(traffic_northSouthWindow, "North-South", 1);
            break;
        case TRAFFIC_SIGNAL_TURN_GREEN:
            /* Turn Green */
            fsm_transition(fsm, traffic_northSouthGreenState);
            break;
        case TRAFFIC_SIGNAL_EXECUTE:
        case TRAFFIC_SIGNAL_EXIT:
        default:
            /* Do Nothing */
            break;
    }
}

/*** North-South Yellow State ***/
static void traffic_northSouthYellowState(fsm_t * const fsm, fsm_event_t * const event)
{
    /*** North-South Yellow State ***/
    /* Variables */
    static time_t startTime;
    static fsm_event_t turnGreenEvent;

    /* Set Up */
    turnGreenEvent.signal = TRAFFIC_SIGNAL_TURN_GREEN;

    /* Handle Signal */
    switch(event->signal)
    {
        case TRAFFIC_SIGNAL_ENTRY:
            /* Entry */
            startTime = time(NULL);
            traffic_drawLight(traffic_northSouthWindow, "North-South", 2);
            break;
        case TRAFFIC_SIGNAL_TIMEOUT:
            /* Timeout */
            if((time(NULL) - startTime) >= TRAFFIC_LIGHT_YELLOW_DURATION)
            {
                /* Transition To Red */
                fsm_transition(fsm, traffic_northSouthRedState);

                /* Notify East-West */
                fsm_dispatch(&traffic_eastWestFsm, &turnGreenEvent);
            }
            break;
        case TRAFFIC_SIGNAL_EXECUTE:
        case TRAFFIC_SIGNAL_EXIT:
        default:
            /* Do Nothing */
            break;
    }
}
