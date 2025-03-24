/****************************************************************************************************
* Pragma
 ****************************************************************************************************/

#pragma once

/****************************************************************************************************
 * Define
 ****************************************************************************************************/

/*** Project Specific ***/
#ifdef CEEDLING
  #define PROJECT_STATIC(DECLARATION) extern DECLARATION; DECLARATION
#else
  #define PROJECT_STATIC(DECLARATION) static DECLARATION
#endif
