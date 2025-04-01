/****************************************************************************************************
 * Include
 ****************************************************************************************************/

#include "memory.h"
#include "project.h"
#include <stdlib.h>
#include "unity.h"

/****************************************************************************************************
 * Variable
 ****************************************************************************************************/

extern memory_freeCallback_t memory_freeCallback;
extern memory_mallocCallback_t memory_mallocCallback;

/****************************************************************************************************
 * Set Up/Tear Down
 ****************************************************************************************************/

void setUp(void)
{
}

void tearDown(void)
{
}

/****************************************************************************************************
 * Test (Public)
 ****************************************************************************************************/

/*** Free ***/
void test_free_1(void)
{
    /*** Free (Free Callback NULL) ***/
    /* Variable */
    void *memory;

    /* Set Up */
    memory = malloc(1);
    memory_freeCallback = NULL;

    /* Free */
    memory_free(&memory);

    /* Clean Up */
    free(memory);
}

void test_free_2(void)
{
    /*** Free (Memory NULL) ***/
    /* Set Up */
    memory_freeCallback = free;

    /* Free */
    memory_free(NULL);
}

void test_free_3(void)
{
    /*** Free ***/
    /* Variable */
    void *memory;

    /* Set Up */
    memory = malloc(1);
    memory_freeCallback = free;

    /* Free */
    memory_free(&memory);

    /* Verify */
    TEST_ASSERT_NULL(memory);
}

/*** Initialize ***/
void test_init_1(void)
{
    /*** Initialize ***/
    /* Initialize */
    memory_init(free, malloc);

    /* Verify */
    TEST_ASSERT_EQUAL_PTR(free, memory_freeCallback);
    TEST_ASSERT_EQUAL_PTR(malloc, memory_mallocCallback);
}

/*** Malloc ***/
void test_malloc_1(void)
{
    /*** Malloc (Malloc Callback NULL) ***/
    /* Variable */
    void *memory;

    /* Set Up */
    memory_mallocCallback = NULL;

    /* Free */
    memory = memory_malloc(1);

    /* Verify */
    TEST_ASSERT_NULL(memory);
}

void test_malloc_2(void)
{
    /*** Malloc ***/
    /* Variable */
    void *memory;

    /* Set Up */
    memory_mallocCallback = malloc;

    /* Free */
    memory = memory_malloc(1);

    /* Verify */
    TEST_ASSERT_NOT_NULL(memory);

    /* Clean Up */
    free(memory);
}
