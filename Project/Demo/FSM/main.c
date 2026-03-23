/****************************************************************************************************
 * Defines
 ****************************************************************************************************/

#define MAIN_RUN_SECONDS (30)
#define MAIN_SLEEP_MICROSECONDS (100000)

/****************************************************************************************************
 * Includes
 ****************************************************************************************************/

#include "Eclectic/Miscellaneous/memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "traffic.h"
#include <unistd.h>

/****************************************************************************************************
 * Function Definitions (Public)
 ****************************************************************************************************/

/*** Main Entry ***/
int main(void) {
  /*** Main Entry ***/
  /* Variables */
  time_t startTime;

  /* Set Up */
  startTime = time(NULL);

  /* Initialize */
  memory_init(free, malloc);
  traffic_init();

  /* Run */
  do {
    /* Tick */
    traffic_tick();

    /* Sleep */
    usleep(MAIN_SLEEP_MICROSECONDS);
  } while ((time(NULL) - startTime) < MAIN_RUN_SECONDS);

  /* Deinitialize */
  traffic_deinit();

  /* Exit */
  return 0;
}
