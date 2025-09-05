/****************************************************************************************************
 * Include
 ****************************************************************************************************/

#include "list.h"
#include "mock_memory.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "unity.h"

/****************************************************************************************************
 * Helper
 ****************************************************************************************************/

static int test_compareCallback(const void * const Data1, const void * const Data2)
{
    /*** Compare ***/
    return strcmp((const char *)Data1, (const char *)Data2);
}

static void test_verifyListDeinitialized(const list_list_t * const List)
{
    /*** Verify List Deinitialized ***/
    TEST_ASSERT_NULL(List->compareCallback);
    TEST_ASSERT_NULL(List->destroyCallback);
    TEST_ASSERT_NULL(List->head);
    TEST_ASSERT_NULL(List->tail);
    TEST_ASSERT_EQUAL_UINT32(0, List->size);
}

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

/*** Deinitialize ***/
void test_deinit_1(void)
{
    /*** Deinitialize (List NULL) ***/
    list_deinit(NULL);
}

void test_deinit_2(void)
{
    /*** Deinitialize (Size 0) ***/
    /* Variable */
    list_list_t list;

    /* Set Up */
    list_init(&list, NULL, NULL);

    /* Deinitialize */
    list_deinit(&list);

    /* Verify */
    test_verifyListDeinitialized(&list);
}

void test_deinit_3(void)
{
    /*** Deinitialize (Destroy Callback NULL) ***/
    /* Test Data */
    int testData[3] = {-1, 0, 1};
    const size_t TestDataCount = sizeof(testData) / sizeof(testData[0]);

    /* Variable */
    list_list_t list;
    void *memory[TestDataCount];

    /* Set Up */
    list_init(&list, NULL, NULL);
    for(size_t i = 0; i < TestDataCount; i++)
    {
        /* Set Up */
        memory[i] = malloc(LIST_NODE_MEMORY_SIZE);

        /* Mock */
        memory_malloc_ExpectAndReturn(LIST_NODE_MEMORY_SIZE, memory[i]);

        /* Push */
        list_pushTail(&list, &testData[i]);
    }

    /* Mock */
    for(size_t i = 0; i < TestDataCount; i++)
        memory_free_ExpectAnyArgs();

    /* Deinitialize */
    list_deinit(&list);

    /* Verify */
    test_verifyListDeinitialized(&list);

    /* Clean Up */
    for(size_t i = 0; i < TestDataCount; i++)
        free(memory[i]);
}

void test_deinit_4(void)
{
    /*** Deinitialize (Destroy Callback) ***/
    /* Test Data */
    const int TestData[3] = {-1, 0, 1};
    const size_t TestDataCount = sizeof(TestData) / sizeof(TestData[0]);

    /* Variable */
    int *testData[TestDataCount];
    list_list_t list;
    void *memory[TestDataCount];

    /* Set Up */
    list_init(&list, NULL, free);
    for(size_t i = 0; i < TestDataCount; i++)
    {
        /* Set Up */
        testData[i] = malloc(sizeof(TestData[i]));
        *testData[i] = TestData[i];
        memory[i] = malloc(LIST_NODE_MEMORY_SIZE);

        /* Mock */
        memory_malloc_ExpectAndReturn(LIST_NODE_MEMORY_SIZE, memory[i]);

        /* Push */
        list_pushTail(&list, testData[i]);
    }

    /* Mock */
    for(size_t i = 0; i < TestDataCount; i++)
        memory_free_ExpectAnyArgs();

    /* Deinitialize */
    list_deinit(&list);

    /* Verify */
    test_verifyListDeinitialized(&list);

    /* Clean Up */
    for(size_t i = 0; i < TestDataCount; i++)
        free(memory[i]);
}

/*** Find ***/
void test_find_1(void)
{
    /*** Find (List NULL) ***/
    /* Variable */
    const char * const TestData = "Data";

    /* Find */
    list_find(NULL, TestData);
}

void test_find_2(void)
{
    /*** Find (Compare Callback NULL) ***/
    /* Test Data */
    const char * const TestData = "Data";

    /* Variable */
    size_t index;
    list_list_t list;

    /* Set Up */
    list_init(&list, NULL, NULL);

    /* Find */
    index = list_find(&list, TestData);

    /* Verify */
    TEST_ASSERT_EQUAL_UINT32(LIST_FIND_NOT_FOUND_INDEX, index);
}

void test_find_3(void)
{
    /*** Find (Empty List) ***/
    /* Test Data */
    const char * const TestData = "Data";

    /* Variable */
    size_t index;
    list_list_t list;

    /* Set Up */
    list_init(&list, test_compareCallback, NULL);

    /* Find */
    index = list_find(&list, TestData);

    /* Verify */
    TEST_ASSERT_EQUAL_UINT32(LIST_FIND_NOT_FOUND_INDEX, index);
}

void test_find_4(void)
{
    /*** Find (Data Not Found) ***/
    /* Test Data */
    char *testData[] = {"One", "Two", "Three"};
    const size_t TestDataCount = sizeof(testData) / sizeof(testData[0]);

    /* Variable */
    size_t index;
    list_list_t list;
    void *memory[TestDataCount];

    /* Set Up */
    list_init(&list, test_compareCallback, NULL);
    for(size_t i = 0; i < TestDataCount; i++)
    {
        /* Set Up */
        memory[i] = malloc(LIST_NODE_MEMORY_SIZE);

        /* Mock */
        memory_malloc_ExpectAndReturn(LIST_NODE_MEMORY_SIZE, memory[i]);

        /* Push */
        list_pushTail(&list, testData[i]);
    }

    /* Find */
    index = list_find(&list, "Bogus");

    /* Verify */
    TEST_ASSERT_EQUAL_UINT32(LIST_FIND_NOT_FOUND_INDEX, index);

    /* Clean Up */
    for(size_t i = 0; i < TestDataCount; i++)
        free(memory[i]);
}

void test_find_5(void)
{
    /*** Find (Data Found) ***/
    /* Test Data */
    char *testData[] = {"One", "Two", "Three"};
    const size_t TestDataCount = sizeof(testData) / sizeof(testData[0]);

    /* Variable */
    size_t index;
    list_list_t list;
    void *memory[TestDataCount];

    /* Set Up */
    list_init(&list, test_compareCallback, NULL);
    for(size_t i = 0; i < TestDataCount; i++)
    {
        /* Set Up */
        memory[i] = malloc(LIST_NODE_MEMORY_SIZE);

        /* Mock */
        memory_malloc_ExpectAndReturn(LIST_NODE_MEMORY_SIZE, memory[i]);

        /* Push */
        list_pushTail(&list, testData[i]);
    }

    /* Find */
    for(size_t i = 0; i < TestDataCount; i++)
    {
        /* Find */
        index = list_find(&list, testData[i]);

        /* Verify */
        TEST_ASSERT_EQUAL_UINT32(i, index);
    }

    /* Clean Up */
    for(size_t i = 0; i < TestDataCount; i++)
        free(memory[i]);
}

/*** Initialize ***/
void test_init_1(void)
{
    /*** Initialize (List NULL) ***/
    list_init(NULL, test_compareCallback, free);
}

void test_init_2(void)
{
    /*** Initialize ***/
    /* Variable */
    list_list_t list;

    /* Initialize */
    list_init(&list, test_compareCallback, free);

    /* Verify */
    TEST_ASSERT_EQUAL_PTR(test_compareCallback, list.compareCallback);
    TEST_ASSERT_EQUAL_PTR(free, list.destroyCallback);
    TEST_ASSERT_NULL(list.head);
    TEST_ASSERT_NULL(list.tail);
    TEST_ASSERT_EQUAL_UINT32(0, list.size);
}

/*** Peek At ***/
void test_peekAt_1(void)
{
    /*** Peek At (List NULL) ***/
    /* Variable */
    void *data;

    /* Peek At */
    data = list_peekAt(NULL, 0);

    /* Verify */
    TEST_ASSERT_NULL(data);
}

void test_peekAt_2(void)
{
    /*** Peek At (Length 0) ***/
    /* Variable */
    void *data;
    list_list_t list;

    /* Set Up */
    list_init(&list, NULL, NULL);

    /* Peek At */
    data = list_peekAt(&list, 0);

    /* Verify */
    TEST_ASSERT_NULL(data);
}

void test_peekAt_3(void)
{
    /*** Peek At ***/
    /* Test Data */
    char *testData[] = {"One", "Two", "Three"};
    const size_t TestDataCount = sizeof(testData) / sizeof(testData[0]);

    /* Variable */
    char *data;
    list_list_t list;
    void *memory[TestDataCount];

    /* Set Up */
    list_init(&list, NULL, NULL);
    for(size_t i = 0; i < TestDataCount; i++)
    {
        /* Set Up */
        memory[i] = malloc(LIST_NODE_MEMORY_SIZE);

        /* Mock */
        memory_malloc_ExpectAndReturn(LIST_NODE_MEMORY_SIZE, memory[i]);

        /* Push */
        list_pushTail(&list, testData[i]);
    }

    /* Peek At */
    for(size_t i = 0; i < TestDataCount; i++)
    {
        /* Peek At */
        data = list_peekAt(&list, i);

        /* Verify */
        TEST_ASSERT_EQUAL_STRING(data, testData[i]);
        TEST_ASSERT_EQUAL_UINT32(TestDataCount, list.size);
    }

    /* Clean Up */
    for(size_t i = 0; i < TestDataCount; i++)
        free(memory[i]);
}

/*** Pop At ***/
void test_popAt_1(void)
{
    /*** Pop At (List NULL) ***/
    /* Variable */
    void *data;

    /* Pop At */
    data = list_popAt(NULL, 0);

    /* Verify */
    TEST_ASSERT_NULL(data);
}

void test_popAt_2(void)
{
    /*** Pop At (Length 0) ***/
    /* Variable */
    void *data;
    list_list_t list;

    /* Set Up */
    list_init(&list, NULL, NULL);

    /* Pop At */
    data = list_popAt(&list, 0);

    /* Verify */
    TEST_ASSERT_NULL(data);
}

void test_popAt_3(void)
{
    /*** Pop At (Head) ***/
    /* Test Data */
    char *testData[] = {"One", "Two", "Three"};
    const size_t TestDataCount = sizeof(testData) / sizeof(testData[0]);

    /* Variable */
    char *data;
    list_list_t list;
    void *memory[TestDataCount];

    /* Set Up */
    list_init(&list, NULL, NULL);
    for(size_t i = 0; i < TestDataCount; i++)
    {
        /* Set Up */
        memory[i] = malloc(LIST_NODE_MEMORY_SIZE);

        /* Mock */
        memory_malloc_ExpectAndReturn(LIST_NODE_MEMORY_SIZE, memory[i]);

        /* Push */
        list_pushTail(&list, testData[i]);
    }

    /* Mock */
    memory_free_ExpectAnyArgs();

    /* Pop At (Head) */
    data = list_popAt(&list, 0);

    /* Verify */
    TEST_ASSERT_EQUAL_STRING(data, testData[0]);
    TEST_ASSERT_EQUAL_UINT32(TestDataCount - 1, list.size);
    TEST_ASSERT_EQUAL_STRING(testData[1], list_peekAt(&list, 0));
    TEST_ASSERT_EQUAL_STRING(testData[2], list_peekAt(&list, 1));

    /* Clean Up */
    for(size_t i = 0; i < TestDataCount; i++)
        free(memory[i]);
}

void test_popAt_4(void)
{
    /*** Pop At (Middle) ***/
    /* Test Data */
    char *testData[] = {"One", "Two", "Three"};
    const size_t TestDataCount = sizeof(testData) / sizeof(testData[0]);

    /* Variable */
    char *data;
    list_list_t list;
    void *memory[TestDataCount];

    /* Set Up */
    list_init(&list, NULL, NULL);
    for(size_t i = 0; i < TestDataCount; i++)
    {
        /* Set Up */
        memory[i] = malloc(LIST_NODE_MEMORY_SIZE);

        /* Mock */
        memory_malloc_ExpectAndReturn(LIST_NODE_MEMORY_SIZE, memory[i]);

        /* Push */
        list_pushTail(&list, testData[i]);
    }

    /* Mock */
    memory_free_ExpectAnyArgs();

    /* Pop At (Middle) */
    data = list_popAt(&list, 1);

    /* Verify */
    TEST_ASSERT_EQUAL_STRING(data, testData[1]);
    TEST_ASSERT_EQUAL_UINT32(TestDataCount - 1, list.size);
    TEST_ASSERT_EQUAL_STRING(testData[0], list_peekAt(&list, 0));
    TEST_ASSERT_EQUAL_STRING(testData[2], list_peekAt(&list, 1));

    /* Clean Up */
    for(size_t i = 0; i < TestDataCount; i++)
        free(memory[i]);
}

void test_popAt_5(void)
{
    /*** Pop At (Tail) ***/
    /* Test Data */
    char *testData[] = {"One", "Two", "Three"};
    const size_t TestDataCount = sizeof(testData) / sizeof(testData[0]);

    /* Variable */
    char *data;
    list_list_t list;
    void *memory[TestDataCount];

    /* Set Up */
    list_init(&list, NULL, NULL);
    for(size_t i = 0; i < TestDataCount; i++)
    {
        /* Set Up */
        memory[i] = malloc(LIST_NODE_MEMORY_SIZE);

        /* Mock */
        memory_malloc_ExpectAndReturn(LIST_NODE_MEMORY_SIZE, memory[i]);

        /* Push */
        list_pushTail(&list, testData[i]);
    }

    /* Mock */
    memory_free_ExpectAnyArgs();

    /* Pop At (Tail) */
    data = list_popAt(&list, 2);

    /* Verify */
    TEST_ASSERT_EQUAL_STRING(data, testData[2]);
    TEST_ASSERT_EQUAL_UINT32(TestDataCount - 1, list.size);
    TEST_ASSERT_EQUAL_STRING(testData[0], list_peekAt(&list, 0));
    TEST_ASSERT_EQUAL_STRING(testData[1], list_peekAt(&list, 1));

    /* Clean Up */
    for(size_t i = 0; i < TestDataCount; i++)
        free(memory[i]);
}

/*** Pop Head ***/
void test_popHead_1(void)
{
    /*** Pop Head (List NULL) ***/
    /* Variable */
    void *data;

    /* Pop Head */
    data = list_popHead(NULL);

    /* Verify */
    TEST_ASSERT_NULL(data);
}

void test_popHead_2(void)
{
    /*** Pop Head (Length 0) ***/
    /* Variable */
    void *data;
    list_list_t list;

    /* Set Up */
    list_init(&list, NULL, NULL);

    /* Pop Head */
    data = list_popHead(&list);

    /* Verify */
    TEST_ASSERT_NULL(data);
}

void test_popHead_3(void)
{
    /*** Pop Head ***/
    /* Test Data */
    char *testData[] = {"One", "Two", "Three"};
    const size_t TestDataCount = sizeof(testData) / sizeof(testData[0]);

    /* Variable */
    char *data;
    list_list_t list;
    void *memory[TestDataCount];

    /* Set Up */
    list_init(&list, NULL, NULL);
    for(size_t i = 0; i < TestDataCount; i++)
    {
        /* Set Up */
        memory[i] = malloc(LIST_NODE_MEMORY_SIZE);

        /* Mock */
        memory_malloc_ExpectAndReturn(LIST_NODE_MEMORY_SIZE, memory[i]);

        /* Push */
        list_pushTail(&list, testData[i]);
    }

    /* Pop Head */
    for(size_t i = 0; i < TestDataCount; i++)
    {
        /* Mock */
    	memory_free_ExpectAnyArgs();

    	/* Pop Head */
        data = list_popHead(&list);

        /* Verify */
        TEST_ASSERT_EQUAL_STRING(data, testData[i]);
        TEST_ASSERT_EQUAL_UINT32((TestDataCount - 1) - i, list.size);
    }

    /* Clean Up */
    for(size_t i = 0; i < TestDataCount; i++)
        free(memory[i]);
}

/*** Pop Tail ***/
void test_popTail_1(void)
{
    /*** Pop Tail (List NULL) ***/
    /* Variable */
    void *data;

    /* Pop Tail */
    data = list_popTail(NULL);

    /* Verify */
    TEST_ASSERT_NULL(data);
}

void test_popTail_2(void)
{
    /*** Pop Tail (Length 0) ***/
    /* Variable */
    void *data;
    list_list_t list;

    /* Set Up */
    list_init(&list, NULL, NULL);

    /* Pop Tail */
    data = list_popTail(&list);

    /* Verify */
    TEST_ASSERT_NULL(data);
}

void test_popTail_3(void)
{
    /*** Pop Tail ***/
    /* Test Data */
    char *testData[] = {"One", "Two", "Three"};
    const size_t TestDataCount = sizeof(testData) / sizeof(testData[0]);

    /* Variable */
    char *data;
    list_list_t list;
    void *memory[TestDataCount];

    /* Set Up */
    list_init(&list, NULL, NULL);
    for(size_t i = 0; i < TestDataCount; i++)
    {
        /* Set Up */
        memory[i] = malloc(LIST_NODE_MEMORY_SIZE);

        /* Mock */
        memory_malloc_ExpectAndReturn(LIST_NODE_MEMORY_SIZE, memory[i]);

        /* Push Tail */
        list_pushTail(&list, testData[i]);
    }

    /* Pop Tail */
    for(size_t i = 0; i < TestDataCount; i++)
    {
       	/* Mock */
    	memory_free_ExpectAnyArgs();

    	/* Pop Tail */
        data = list_popTail(&list);

        /* Verify */
        TEST_ASSERT_EQUAL_STRING(data, testData[(TestDataCount - 1) - i]);
        TEST_ASSERT_EQUAL_UINT32((TestDataCount - 1) - i, list.size);
    }

    /* Clean Up */
    for(size_t i = 0; i < TestDataCount; i++)
        free(memory[i]);
}

/*** Push At ***/
void test_pushAt_1(void)
{
    /*** Push At (List NULL) ***/
    /* Test Data */
    char * const testData = "Data";

    /* Push At */
    list_pushAt(NULL, testData, 0);
}

void test_pushAt_2(void)
{
    /*** Push At (Invalid Index) ***/
    /* Test Data */
    char * const testData = "Data";

    /* Variable */
    list_list_t list;

    /* Set Up */
    list_init(&list, NULL, NULL);

    /* Push At */
    list_pushAt(&list, testData, list.size + 1);
}

void test_pushAt_3(void)
{
    /*** Push At (Memory Allocation Failure) ***/
    /* Test Data */
    char *testData[] = {"One", "Two", "Three"};
    const size_t TestDataCount = sizeof(testData) / sizeof(testData[0]);

    /* Variable */
    char *data;
    list_list_t list;
    void *memory[TestDataCount];

    /* Set Up */
    list_init(&list, NULL, NULL);
    for(size_t i = 0; i < TestDataCount; i++)
    {
        /* Set Up */
        memory[i] = malloc(LIST_NODE_MEMORY_SIZE);

        /* Mock */
        memory_malloc_ExpectAndReturn(LIST_NODE_MEMORY_SIZE, memory[i]);

        /* Push Tail */
        list_pushTail(&list, testData[i]);
    }

    /* Mock */
    memory_malloc_ExpectAndReturn(LIST_NODE_MEMORY_SIZE, NULL);

    /* Push At (Head) */
    list_pushAt(&list, "Extra", 1); // Middle

    /* Clean Up */
    for(size_t i = 0; i < TestDataCount; i++)
        free(memory[i]);
}

void test_pushAt_4(void)
{
    /*** Push At (Head) ***/
    /* Constant */
    const size_t HeadIndex = 0;

    /* Test Data */
    char *testData[] = {"One", "Two", "Three"};
    const size_t TestDataCount = sizeof(testData) / sizeof(testData[0]);

    /* Variable */
    list_list_t list;
    void *memory[TestDataCount];

    /* Set Up */
    list_init(&list, NULL, NULL);
    for(size_t i = 0; i < TestDataCount; i++)
    {
        if(i != HeadIndex)
        {
            /* Set Up */
            memory[i] = malloc(LIST_NODE_MEMORY_SIZE);

            /* Mock */
            memory_malloc_ExpectAndReturn(LIST_NODE_MEMORY_SIZE, memory[i]);

            /* Push Tail */
            list_pushTail(&list, testData[i]);
        }
    }

    /* Set Up */
    memory[HeadIndex] = malloc(LIST_NODE_MEMORY_SIZE);

    /* Mock */
    memory_malloc_ExpectAndReturn(LIST_NODE_MEMORY_SIZE, memory[HeadIndex]);

    /* Push At (Head) */
    list_pushAt(&list, testData[HeadIndex], HeadIndex);

    /* Verify */
    TEST_ASSERT_EQUAL_UINT32(TestDataCount, list.size);
    for(size_t i = 0; i < TestDataCount; i++)
		TEST_ASSERT_EQUAL_STRING(testData[i], list_peekAt(&list, i));

    /* Clean Up */
    for(size_t i = 0; i < TestDataCount; i++)
        free(memory[i]);
}

void test_pushAt_5(void)
{
    /*** Push At (Middle) ***/
    /* Constant */
    const size_t MiddleIndex = 1;

    /* Test Data */
    char *testData[] = {"One", "Two", "Three"};
    const size_t TestDataCount = sizeof(testData) / sizeof(testData[0]);

    /* Variable */
    list_list_t list;
    void *memory[TestDataCount];

    /* Set Up */
    list_init(&list, NULL, NULL);
    for(size_t i = 0; i < TestDataCount; i++)
    {
        if(i != MiddleIndex)
        {
            /* Set Up */
            memory[i] = malloc(LIST_NODE_MEMORY_SIZE);

            /* Mock */
            memory_malloc_ExpectAndReturn(LIST_NODE_MEMORY_SIZE, memory[i]);

            /* Push Tail */
            list_pushTail(&list, testData[i]);
        }
    }

    /* Set Up */
    memory[MiddleIndex] = malloc(LIST_NODE_MEMORY_SIZE);

    /* Mock */
    memory_malloc_ExpectAndReturn(LIST_NODE_MEMORY_SIZE, memory[MiddleIndex]);

    /* Push At (Middle) */
    list_pushAt(&list, testData[MiddleIndex], MiddleIndex);

    /* Verify */
    TEST_ASSERT_EQUAL_UINT32(TestDataCount, list.size);
    for(size_t i = 0; i < TestDataCount; i++)
		TEST_ASSERT_EQUAL_STRING(testData[i], list_peekAt(&list, i));

    /* Clean Up */
    for(size_t i = 0; i < TestDataCount; i++)
        free(memory[i]);
}

void test_pushAt_6(void)
{
    /*** Push At (Tail) ***/
    /* Constant */
    const size_t TailIndex = 2;

    /* Test Data */
    char *testData[] = {"One", "Two", "Three"};
    const size_t TestDataCount = sizeof(testData) / sizeof(testData[0]);

    /* Variable */
    list_list_t list;
    void *memory[TestDataCount];

    /* Set Up */
    list_init(&list, NULL, NULL);
    for(size_t i = 0; i < TestDataCount; i++)
    {
        if(i != TailIndex)
        {
            /* Set Up */
            memory[i] = malloc(LIST_NODE_MEMORY_SIZE);

            /* Mock */
            memory_malloc_ExpectAndReturn(LIST_NODE_MEMORY_SIZE, memory[i]);

            /* Push Tail */
            list_pushTail(&list, testData[i]);
        }
    }

    /* Set Up */
    memory[TailIndex] = malloc(LIST_NODE_MEMORY_SIZE);

    /* Mock */
    memory_malloc_ExpectAndReturn(LIST_NODE_MEMORY_SIZE, memory[TailIndex]);

    /* Push At (Tail) */
    list_pushAt(&list, testData[TailIndex], TailIndex);

    /* Verify */
    TEST_ASSERT_EQUAL_UINT32(TestDataCount, list.size);
    for(size_t i = 0; i < TestDataCount; i++)
		TEST_ASSERT_EQUAL_STRING(testData[i], list_peekAt(&list, i));

    /* Clean Up */
    for(size_t i = 0; i < TestDataCount; i++)
        free(memory[i]);
}

/*** Push Head ***/
void test_pushHead_1(void)
{
    /*** Push Head (List NULL) ***/
    /* Test Data */
    char * const testData = "Data";

    /* Push Head */
    list_pushHead(NULL, testData);
}

void test_pushHead_2(void)
{
    /*** Push Head (Memory Allocation Failure) ***/
    /* Test Data */
    char * const testData = "Data";

    /* Variable */
    list_list_t list;

    /* Set Up */
    list_init(&list, NULL, NULL);

    /* Mock */
    memory_malloc_ExpectAndReturn(LIST_NODE_MEMORY_SIZE, NULL);

    /* Push Head */
    list_pushHead(&list, testData);
}

void test_pushHead_3(void)
{
    /*** Push Head ***/
    /* Test Data */
    char *testData[] = {"One", "Two", "Three"};
    const size_t TestDataCount = sizeof(testData) / sizeof(testData[0]);

    /* Variable */
    list_list_t list;
    void *memory[TestDataCount];

    /* Set Up */
    list_init(&list, NULL, NULL);

    /* Push Head */
    for(size_t i = 0; i < TestDataCount; i++)
    {
        /* Set Up */
        memory[i] = malloc(LIST_NODE_MEMORY_SIZE);

        /* Mock */
        memory_malloc_ExpectAndReturn(LIST_NODE_MEMORY_SIZE, memory[i]);

        /* Push Head */
        list_pushHead(&list, testData[i]);
    }

    /* Verify */
    TEST_ASSERT_EQUAL_UINT32(TestDataCount, list.size);
    for(size_t i = 0; i < TestDataCount; i++)
		TEST_ASSERT_EQUAL_STRING(testData[(TestDataCount - 1) - i], list_peekAt(&list, i));

    /* Clean Up */
    for(size_t i = 0; i < TestDataCount; i++)
        free(memory[i]);

}

/*** Push Sorted ***/
void test_pushSorted_1(void)
{
    /*** Push Sorted (List NULL) ***/
    /* Variable */
    char * const testData = "Data";

    /* Push Sorted */
    list_pushSorted(NULL, testData, true);
}

void test_pushSorted_2(void)
{
    /*** Push Sorted (Compare Callback NULL) ***/
    /* Test Data */
    char * const testData = "Data";

    /* Variable */
    list_list_t list;

    /* Set Up */
    list_init(&list, NULL, NULL);

    /* Push Sorted */
    list_pushSorted(&list, testData, true);
}

void test_pushSorted_3(void)
{
    /*** Push Sorted (Memory Allocation Failure) ***/
    /* Test Data */
    char *testData[] = {"One", "Two", "Three"};
    const size_t TestDataCount = sizeof(testData) / sizeof(testData[0]);

    /* Variable */
    list_list_t list;
    void *memory[TestDataCount];

    /* Set Up */
    list_init(&list, test_compareCallback, NULL);

    /* Push Sorted */
    for(size_t i = 0; i < TestDataCount; i++)
    {
        /* Set Up */
        memory[i] = malloc(LIST_NODE_MEMORY_SIZE);

        /* Mock */
        memory_malloc_ExpectAndReturn(LIST_NODE_MEMORY_SIZE, memory[i]);

        /* Push Sorted */
        list_pushSorted(&list, testData[i], true); // Ascending
    }

    /* Mock */
    memory_malloc_ExpectAndReturn(LIST_NODE_MEMORY_SIZE, NULL);

    /* Push Sorted (Middle) */
    list_pushSorted(&list, "Six", true); // Ascending

    /* Clean Up */
    for(size_t i = 0; i < TestDataCount; i++)
        free(memory[i]);
}

void test_pushSorted_4(void)
{
    /*** Push Sorted (Ascending) ***/
    /* Test Data */
    char *testData[] = {"One", "Two", "Three"};
    const size_t TestDataCount = sizeof(testData) / sizeof(testData[0]);

    /* Variable */
    list_list_t list;
    void *memory[TestDataCount];

    /* Set Up */
    list_init(&list, test_compareCallback, NULL);

    /* Push Sorted */
    for(size_t i = 0; i < TestDataCount; i++)
    {
        /* Set Up */
        memory[i] = malloc(LIST_NODE_MEMORY_SIZE);

        /* Mock */
        memory_malloc_ExpectAndReturn(LIST_NODE_MEMORY_SIZE, memory[i]);

        /* Push */
        list_pushSorted(&list, testData[i], true); // Ascending
    }

    /* Verify */
    TEST_ASSERT_EQUAL_UINT32(TestDataCount, list.size);
	TEST_ASSERT_EQUAL_STRING(testData[0], list_peekAt(&list, 0)); // "One"
    TEST_ASSERT_EQUAL_STRING(testData[2], list_peekAt(&list, 1)); // "Three"
    TEST_ASSERT_EQUAL_STRING(testData[1], list_peekAt(&list, 2)); // "Two"

    /* Clean Up */
    for(size_t i = 0; i < TestDataCount; i++)
        free(memory[i]);
}

void test_pushSorted_5(void)
{
    /*** Push Sorted (Descending) ***/
    /* Test Data */
    char *testData[] = {"One", "Two", "Three"};
    const size_t TestDataCount = sizeof(testData) / sizeof(testData[0]);

    /* Variable */
    list_list_t list;
    void *memory[TestDataCount];

    /* Set Up */
    list_init(&list, test_compareCallback, NULL);

    /* Push Sorted */
    for(size_t i = 0; i < TestDataCount; i++)
    {
        /* Set Up */
        memory[i] = malloc(LIST_NODE_MEMORY_SIZE);

        /* Mock */
        memory_malloc_ExpectAndReturn(LIST_NODE_MEMORY_SIZE, memory[i]);

        /* Push */
        list_pushSorted(&list, testData[i], false); // Descending
    }

    /* Verify */
    TEST_ASSERT_EQUAL_UINT32(TestDataCount, list.size);
	TEST_ASSERT_EQUAL_STRING(testData[1], list_peekAt(&list, 0)); // "Two"
    TEST_ASSERT_EQUAL_STRING(testData[2], list_peekAt(&list, 1)); // "Three"
    TEST_ASSERT_EQUAL_STRING(testData[0], list_peekAt(&list, 2)); // "One"

    /* Clean Up */
    for(size_t i = 0; i < TestDataCount; i++)
        free(memory[i]);
}

/*** Push Tail ***/
void test_pushTail_1(void)
{
    /*** Push Tail (List NULL) ***/
    /* Test Data */
    char * const testData = "Data";

    /* Push At */
    list_pushTail(NULL, testData);
}

void test_pushTail_2(void)
{
    /*** Push Tail (Memory Allocation Failure) ***/
    /* Test Data */
    char * const testData = "Data";

    /* Variable */
    list_list_t list;

    /* Set Up */
    list_init(&list, NULL, NULL);

    /* Mock */
    memory_malloc_ExpectAndReturn(LIST_NODE_MEMORY_SIZE, NULL);

    /* Push At */
    list_pushTail(&list, testData);
}

void test_pushTail_3(void)
{
    /*** Push Tail ***/
    /* Test Data */
    char *testData[] = {"One", "Two", "Three"};
    const size_t TestDataCount = sizeof(testData) / sizeof(testData[0]);

    /* Variable */
    list_list_t list;
    void *memory[TestDataCount];

    /* Set Up */
    list_init(&list, NULL, NULL);

    /* Push Tail */
    for(size_t i = 0; i < TestDataCount; i++)
    {
        /* Set Up */
        memory[i] = malloc(LIST_NODE_MEMORY_SIZE);

        /* Mock */
        memory_malloc_ExpectAndReturn(LIST_NODE_MEMORY_SIZE, memory[i]);

        /* Push */
        list_pushTail(&list, testData[i]);
    }

    /* Verify */
    TEST_ASSERT_EQUAL_UINT32(TestDataCount, list.size);
    for(size_t i = 0; i < TestDataCount; i++)
		TEST_ASSERT_EQUAL_STRING(testData[i], list_peekAt(&list, i));

    /* Clean Up */
    for(size_t i = 0; i < TestDataCount; i++)
        free(memory[i]);

}
