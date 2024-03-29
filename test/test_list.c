/****************************************************************************************************
 * Define
 ****************************************************************************************************/

#define LIST_TEST_LIST_COUNT (5)

/****************************************************************************************************
 * Include
 ****************************************************************************************************/

#include "list.h"
#include "mock_eclectic.h"
#include <string.h>
#include <time.h>
#include "unity.h"

/****************************************************************************************************
 * Helper
 ****************************************************************************************************/

/*** Compare Callback ***/
static int listTest_compareCallback(const void * const Data1, const void * const Data2)
{    
    /*** Compare Callback ***/
    return *(const int *)Data1 - *(const int *)Data2;
}

void test_compareCallback_success(void)
{
    /*** Test Data ***/
    /* Type Definition */
    typedef struct testData_s
    {
        const int Integer1;
        const int Integer2;
        const int ExpectedResult;
    } testData_t;

    /* Test Data */
    const testData_t TestData[] =
    {
        /* Integer1 Smaller Than Integer2 */
        {-2, -1, -1},
        {-2, 1, -3},
        {1, 2, -1},
        
        /* Integer1 Equal To Integer2 */
        {-2, -2, 0},
        {0, 0, 0},
        {1, 1, 0},
        
        /* Integer1 Larger Than Integer2 */
        {2, 1, 1},
        {1, -2, 3},
        {-1, -2, 1},
    };
    const size_t TestDataCount = sizeof(TestData) / sizeof(TestData[0]);

    /* Variable */
    int actualResult;
    uint8_t i;
    
    /*** Compare Callback ***/
    for(i = 0; i < TestDataCount; i++)
    {
        /* Compare Callback */
        actualResult = listTest_compareCallback(&TestData[i].Integer1, &TestData[i].Integer2);
        
        /* Verify */
        TEST_ASSERT_EQUAL_INT(TestData[i].ExpectedResult, actualResult);
    }
}

/****************************************************************************************************
 * Set Up/Tear Down
 ****************************************************************************************************/

void setUp(void)
{
    /*** Set Up ***/
    srand(time(NULL));
}

void tearDown(void)
{
}

/****************************************************************************************************
 * Test (Public)
 ****************************************************************************************************/

/*** Destroy ***/
void test_destroy_error(void)
{
    /*** Destroy ***/
    /* (NULL Pointer Error */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_ERROR_NULL_POINTER, list_destroy(NULL));
}

void test_destroy_success(void)
{
    /*** Test Data ***/
    /* Test Data */
    const int TestData[] = {24, 63, -59, 64, -85};
    const size_t TestDataCount = sizeof(TestData) / sizeof(TestData[0]);

    /* Variable */
    int *data;
    uint8_t i;
    list_list_t list;
    list_node_t *memory;
    
    /*** Destroy ***/
    /* Initialize */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_init(&list, NULL, free));
    
    /* Malloc */
    memory = malloc(TestDataCount * sizeof(list_node_t));
    
    /* Push Tail */
    for(i = 0; i < TestDataCount; i++)
    {
        /* Malloc */
        data = malloc(sizeof(*data));
        *data = TestData[i];
        
        /* Mock */
        eclectic_malloc_ExpectAndReturn(sizeof(list_node_t), &memory[i]);
        
        /* Push Tail */
        TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_pushTail(&list, data));
        
        /* Size */
        TEST_ASSERT_EQUAL_UINT((i + 1), list_size(&list));
    }
    
    /* Mock */
    for(i = 0; i < TestDataCount; i++)
        eclectic_free_ExpectAnyArgs();
    
    /* Destroy */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_destroy(&list));
    
    /* Free */
    free(memory);
    
    /* Verify (Same State As When init'd) */
    TEST_ASSERT_EQUAL_PTR(NULL, list.compareCallback);
    TEST_ASSERT_EQUAL_PTR(free, list.destroyCallback);
    TEST_ASSERT_NULL(list.head);
    TEST_ASSERT_NULL(list.tail);
    TEST_ASSERT_EQUAL_UINT(0, list.size);
}

/*** Find ***/
void test_find_error(void)
{
    /*** Test Data ***/
    /* Variable */
    void *data;
    list_list_t list;
    
    /*** Find ***/
    /* Initialize */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_init(&list, NULL, NULL));
    
    /* NULL Pointer Error */
    TEST_ASSERT_EQUAL_UINT(LIST_FIND_NOT_FOUND_INDEX, list_find(NULL, data));
    TEST_ASSERT_EQUAL_UINT(LIST_FIND_NOT_FOUND_INDEX, list_find(&list, data)); // list.compareCallback Is NULL
}

void test_find_success(void)
{
    /*** Test Data ***/    
    /* Type Definition */
    typedef struct testData_s
    {
        const int FindInteger;
        const size_t ExpectedResult;
    } testData_t;

    /* Test Data */
    const int PushIntegerList[] = {24, 63, -59, 64, -85};
    const size_t PushIntegerListCount = sizeof(PushIntegerList) / sizeof(PushIntegerList[0]);
    const testData_t TestData[] =
    {
        {24, 0},
        {-85, 4},
        {-24, LIST_FIND_NOT_FOUND_INDEX},
        {58, LIST_FIND_NOT_FOUND_INDEX},
        {80, LIST_FIND_NOT_FOUND_INDEX},
        {64, 3},
        {63, 1},
        {51, LIST_FIND_NOT_FOUND_INDEX},
        {-59, 2},
        {16, LIST_FIND_NOT_FOUND_INDEX}
    };
    const size_t TestDataCount = sizeof(TestData) / sizeof(TestData[0]);

    /* Variable */
    uint8_t i;
    list_list_t list;
    list_node_t *memory;
    int pushIntegerList[PushIntegerListCount];
    
    /* Set Up */
    memcpy(pushIntegerList, PushIntegerList, sizeof(PushIntegerList));
    
    /*** Find ***/
    /* Initialize */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_init(&list, listTest_compareCallback, NULL));
    
    /* Malloc */
    memory = malloc(TestDataCount * sizeof(list_node_t));
    
    /* Push Tail */
    for(i = 0; i < PushIntegerListCount; i++)
    {
        /* Mock */
        eclectic_malloc_ExpectAndReturn(sizeof(list_node_t), &memory[i]);
        
        /* Push Tail */
        TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_pushTail(&list, &pushIntegerList[i]));
        
        /* Size */
        TEST_ASSERT_EQUAL_UINT((i + 1), list_size(&list));
    }
    
    /* Find */
    for(i = 0; i < TestDataCount; i++)
        TEST_ASSERT_EQUAL_UINT(TestData[i].ExpectedResult, list_find(&list, &TestData[i].FindInteger));
    
    /* Mock */
    for(i = 0; i < PushIntegerListCount; i++)
        eclectic_free_ExpectAnyArgs();
    
    /* Destroy */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_destroy(&list));
    
    /* Free */
    free(memory);
}

/*** Initialize ***/
void test_init_error(void)
{    
    /*** Initialize ***/
    /* NULL Pointer Error */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_ERROR_NULL_POINTER, list_init(NULL, listTest_compareCallback, free));
}

void test_init_success(void)
{
    /*** Set Up ***/
    /* Variable */
    list_list_t list;
    
    /*** Initialize ***/
    /* Initialize */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_init(&list, listTest_compareCallback, free));
    
    /* Verify */
    TEST_ASSERT_EQUAL_PTR(listTest_compareCallback, list.compareCallback);
    TEST_ASSERT_EQUAL_PTR(free, list.destroyCallback);
    TEST_ASSERT_NULL(list.head);
    TEST_ASSERT_NULL(list.tail);
    TEST_ASSERT_EQUAL_UINT(0, list.size);
}

/*** Peek At ***/
void test_list_peekAt_error(void)
{
    /*** Test Data ***/
    /* Variable */
    void *data;
    list_list_t list;
    
    /*** Peek At ***/
    /* Initialize */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_init(&list, NULL, NULL));
    
    /* NULL Pointer Error */
    TEST_ASSERT_FALSE(list_peekAt(NULL, 0));
    
    /* Length Error */
    TEST_ASSERT_FALSE(list_peekAt(&list, 0));
}

void test_list_peekAt_success(void)
{
    /*** Test Data ***/
    /* Test Data */
    const int TestData[] = {24, 63, -59, 64, -85};
    const size_t TestDataCount = sizeof(TestData) / sizeof(TestData[0]);

    /* Variable */
    int *data;
    uint8_t i;
    list_list_t list;
    list_node_t *memory;
    int pushIntegerList[TestDataCount];
    
    /* Set Up */
    memcpy(pushIntegerList, TestData, sizeof(TestData));
    
    /*** Peek At ***/
    /* Initialize */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_init(&list, NULL, NULL));
    
    /* Malloc */
    memory = malloc(TestDataCount * sizeof(list_node_t));
    
    /* Push Tail */
    for(i = 0; i < TestDataCount; i++)
    {
        /* Mock */
        eclectic_malloc_ExpectAndReturn(sizeof(list_node_t), &memory[i]);
        
        /* Push Tail */
        TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_pushTail(&list, &pushIntegerList[i]));
        
        /* Size */
        TEST_ASSERT_EQUAL_UINT((i + 1), list_size(&list));
    }
    
    /* Peek At */
    for(i = 0; i < TestDataCount; i++)
    {
        /* Peek At */
        data = list_peekAt(&list, i);
        
        /* Verify */
        TEST_ASSERT_EQUAL_INT(pushIntegerList[i], *data);
        TEST_ASSERT_EQUAL_PTR(&pushIntegerList[i], data);
    }
    
    /* Mock */
    for(i = 0; i < TestDataCount; i++)
        eclectic_free_ExpectAnyArgs();
    
    /* Destroy */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_destroy(&list));
    
    /* Free */
    free(memory);
}

/*** Pop At ***/
void test_list_popAt_error(void)
{
    /*** Test Data ***/
    /* Variable */
    void *data;
    list_list_t list;
    
    /*** Pop At ***/
    /* Initialize */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_init(&list, NULL, NULL));
    
    /* NULL Pointer Error */
    TEST_ASSERT_FALSE(list_popAt(NULL, 0));
    
    /* Length Error */
    TEST_ASSERT_FALSE(list_popAt(&list, 0));
}

void test_list_popAt_success(void)
{
    /*** Test Data ***/
    /* Type Definition */
    typedef struct testData_s
    {
        const size_t PopIndex;
        const int ExpectedPopInteger;
        const int ExpectedIntegerList[LIST_TEST_LIST_COUNT - 1];
    } testData_t;

    /* Test Data */
    const int PushIntegerList[LIST_TEST_LIST_COUNT] = {24, 63, -59, 64, -85};
    const testData_t TestData[] =
    {
        {
            0,
            24,
            {63, -59, 64, -85}
        },
        {
            1,
            63,
            {24, -59, 64, -85}
        },
        {
            2,
            -59,
            {24, 63, 64, -85}
        },
        {
            3,
            64,
            {24, 63, -59, -85}
        },
        {
            4,
            -85,
            {24, 63, -59, 64}
        }
    };
    const size_t TestDataCount = sizeof(TestData) / sizeof(TestData[0]);

    /* Variable */
    int *data;
    uint8_t i, j;
    list_list_t list;
    list_node_t *memory;
    int pushIntegerList[LIST_TEST_LIST_COUNT];
    
    /* Set Up */
    memory = malloc(LIST_TEST_LIST_COUNT * sizeof(list_node_t));
    
    /*** Pop At ***/
    for(i = 0; i < TestDataCount; i++)
    {
        /* Set Up */
        memcpy(pushIntegerList, PushIntegerList, sizeof(PushIntegerList));
        
        /* Initialize */
        TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_init(&list, NULL, NULL));
        
        /* Push Tail */
        for(j = 0; j < LIST_TEST_LIST_COUNT; j++)
        {
            /* Mock */
            eclectic_malloc_ExpectAndReturn(sizeof(list_node_t), &memory[j]);
            
            /* Push Tail */
            TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_pushTail(&list, &pushIntegerList[j]));
            
            /* Size */
            TEST_ASSERT_EQUAL_UINT((j + 1), list_size(&list));
        }
        
        /* Mock */
        eclectic_free_ExpectAnyArgs();
        
        /* Pop At */
        data = list_popAt(&list, TestData[i].PopIndex);
        
        /* Verify */
        TEST_ASSERT_NOT_NULL(data);
        TEST_ASSERT_EQUAL_INT(TestData[i].ExpectedPopInteger, *data);
        TEST_ASSERT_EQUAL_UINT((LIST_TEST_LIST_COUNT - 1), list_size(&list));
        
        /* Peek At */
        for(j = 0; j < (LIST_TEST_LIST_COUNT - 1); j++)
        {
            /* Peek At */
            data = list_peekAt(&list, j);
            
            /* Verify */
            TEST_ASSERT_EQUAL_INT(TestData[i].ExpectedIntegerList[j], *data);
        }
        
        /* Pop At Until Empty (In Random Order) */
        for(j = 0; j < (LIST_TEST_LIST_COUNT - 1); j++)
        {
            /* Mock */
            eclectic_free_ExpectAnyArgs();
            
            /* Pop At */
            (void)list_popAt(&list, (rand() % list_size(&list)));
        }
    }
    
    /* Free */
    free(memory);
}

/*** Pop Head ***/
void test_list_popHead_error(void)
{
    /*** Test Data ***/
    /* Variable */
    void *data;
    list_list_t list;
    
    /*** Pop Head ***/
    /* Initialize */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_init(&list, NULL, NULL));
    
    /* NULL Pointer Error */
    data = list_popHead(NULL);
    TEST_ASSERT_NULL(data);
    
    /* Length Error */
    data = list_popHead(&list);
    TEST_ASSERT_NULL(data);
}

void test_list_popHead_success(void)
{
    /*** Test Data ***/
    /* Test Data */
    const int TestData[] = {24, 63, -59, 64, -85};
    const size_t TestDataCount = sizeof(TestData) / sizeof(TestData[0]);

    /* Variable */
    int *data;
    uint8_t i;
    list_list_t list;
    list_node_t *memory;
    int pushIntegerList[TestDataCount];
    
    /* Set Up */
    memcpy(pushIntegerList, TestData, sizeof(TestData));
    
    /*** Pop Head ***/
    /* Initialize */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_init(&list, NULL, NULL));
    
    /* Malloc */
    memory = malloc(TestDataCount * sizeof(list_node_t));
    
    /* Push Tail */
    for(i = 0; i < TestDataCount; i++)
    {
        /* Mock */
        eclectic_malloc_ExpectAndReturn(sizeof(list_node_t), &memory[i]);
        
        /* Push Tail */
        TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_pushTail(&list, &pushIntegerList[i]));
        
        /* Size */
        TEST_ASSERT_EQUAL_UINT((i + 1), list_size(&list));
    }
    
    /* Pop Head */
    for(i = 0; i < TestDataCount; i++)
    {
        /* Mock */
        eclectic_free_ExpectAnyArgs();
        
        /* Pop Head */
        data = list_popHead(&list);
        
        /* Verify */
        TEST_ASSERT_EQUAL_INT(pushIntegerList[i], *data);
        TEST_ASSERT_EQUAL_PTR(&pushIntegerList[i], data);
        TEST_ASSERT_EQUAL_UINT((TestDataCount - (i + 1)), list_size(&list));
    }
    
    /* Destroy */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_destroy(&list));
    
    /* Free */
    free(memory);
}

/*** Pop Tail ***/
void test_list_popTail_error(void)
{
    /*** Test Data ***/
    /* Variable */
    void *data;
    list_list_t list;
    
    /*** Pop Tail ***/
    /* Initialize */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_init(&list, NULL, NULL));
    
    /* NULL Pointer Error */
    data = list_popTail(NULL);
    TEST_ASSERT_NULL(data);
    
    /* Length Error */
    data = list_popTail(&list);
    TEST_ASSERT_NULL(data);
}

void test_list_popTail_success(void)
{
    /*** Test Data ***/
    /* Test Data */
    const int TestData[] = {24, 63, -59, 64, -85};
    const size_t TestDataCount = sizeof(TestData) / sizeof(TestData[0]);

    /* Variable */
    int *data;
    uint8_t i;
    list_list_t list;
    list_node_t *memory;
    int pushIntegerList[TestDataCount];
    
    /* Set Up */
    memcpy(pushIntegerList, TestData, sizeof(TestData));
    
    /*** Pop Tail ***/
    /* Malloc */ 
    memory = malloc(TestDataCount * sizeof(list_node_t));
    
    /* Initialize */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_init(&list, NULL, free));
    
    /* Push Head */
    for(i = 0; i < TestDataCount; i++)
    {
        /* Mock */
        eclectic_malloc_ExpectAndReturn(sizeof(list_node_t), &memory[i]);
        
        /* Push Head */
        TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_pushHead(&list, &pushIntegerList[i]));
        
        /* Size */
        TEST_ASSERT_EQUAL_UINT((i + 1), list_size(&list));
    }
    
    /* Pop Tail */
    for(i = 0; i < TestDataCount; i++)
    {
        /* Mock */
        eclectic_free_ExpectAnyArgs();
        
        /* Pop Tail */
        data = list_popTail(&list);
        
        /* Verify */
        TEST_ASSERT_EQUAL_INT(pushIntegerList[i], *data);
        TEST_ASSERT_EQUAL_PTR(&pushIntegerList[i], data);
        TEST_ASSERT_EQUAL_UINT((TestDataCount - (i + 1)), list_size(&list));
    }
    
    /* Destroy */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_destroy(&list));
    
    /* Free */
    free(memory);
}

/*** Push At ***/
void test_list_pushAt_error(void)
{
    /*** Test Data ***/    
    /* Variable */
    int data[3];
    list_list_t list;
    list_node_t *memory;
    
    /*** Push At ***/
    /* Malloc */ 
    memory = malloc(2 * sizeof(list_node_t));
    
    /* Initialize */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_init(&list, NULL, NULL));
    
    /* Mock */
    eclectic_malloc_ExpectAndReturn(sizeof(list_node_t), &memory[0]);
    
     /* Push Head */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_pushHead(&list, &data[0]));
    
     /* Mock */
    eclectic_malloc_ExpectAndReturn(sizeof(list_node_t), &memory[1]);
    
     /* Push Head */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_pushHead(&list, &data[1]));
    
    /* Size */
    TEST_ASSERT_EQUAL_UINT(2, list_size(&list));
    
    /* NULL Pointer Error */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_ERROR_NULL_POINTER, list_pushAt(NULL, &data[2], 0));
    
    /* Invalid Error */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_ERROR_INVALID, list_pushAt(&list, &data[2], 2));
    
    /* Mock */
    eclectic_malloc_ExpectAndReturn(sizeof(list_node_t), NULL);
    
    /* Memory Allocation Error */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_ERROR_MEMORY_ALLOCATION, list_pushAt(&list, &data[2], 1));
    TEST_ASSERT_EQUAL_UINT(2, list_size(&list));
    
    /* Free */
    free(memory);
}

void test_list_pushAt_success(void)
{
    /*** Test Data ***/
    /* Type Definition */
    typedef struct testData_s
    {
        const size_t PushIndex;
        const int PushInteger;
        const int ExpectedIntegerList[LIST_TEST_LIST_COUNT];
    } testData_t;

    /* Test Data */
    const int PushIntegerList[LIST_TEST_LIST_COUNT - 1] = {24, 63, -59, 64};
    const testData_t TestData[] =
    {
        {
            0,
            -85,
            {-85, 24, 63, -59, 64}
        },
        {
            1,
            -85,
            {24, -85, 63, -59, 64}
        },
        {
            2,
            -85,
            {24, 63, -85, -59, 64}
        },
        {
            3,
            -85,
            {24, 63, -59, -85, 64}
        }
    };
    const size_t TestDataCount = sizeof(TestData) / sizeof(TestData[0]);

    /* Variable */
    int *peekedData, pushedData;
    uint8_t i, j;
    list_list_t list;
    list_node_t *memory;
    int pushIntegerList[LIST_TEST_LIST_COUNT - 1];
    
    /* Malloc */ 
    memory = malloc(LIST_TEST_LIST_COUNT * sizeof(list_node_t));
    
    /*** Push At ***/
    for(i = 0; i < TestDataCount; i++)
    {
        /* Set Up */
        memcpy(pushIntegerList, PushIntegerList, sizeof(PushIntegerList));
        
        /* Initialize */
        TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_init(&list, NULL, NULL));
        
        /* Push Tail */
        for(j = 0; j < (LIST_TEST_LIST_COUNT - 1); j++)
        {
            /* Mock */
            eclectic_malloc_ExpectAndReturn(sizeof(list_node_t), &memory[j]);
            
            /* Push Tail */
            TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_pushTail(&list, &pushIntegerList[j]));
            
            /* Size */
            TEST_ASSERT_EQUAL_UINT((j + 1), list_size(&list));
        }
        
        /* Mock */
        eclectic_malloc_ExpectAndReturn(sizeof(list_node_t), &memory[LIST_TEST_LIST_COUNT - 1]);
        
        /* Push At */
        pushedData = TestData[i].PushInteger;
        TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_pushAt(&list, &pushedData, TestData[i].PushIndex));
        
        /* Size */
        TEST_ASSERT_EQUAL_UINT(LIST_TEST_LIST_COUNT, list_size(&list));
        
        /* Peek At */
        for(j = 0; j < LIST_TEST_LIST_COUNT; j++)
        {
            /* Peek At */
            peekedData = list_peekAt(&list, j);
            
            /* Verify */
            TEST_ASSERT_EQUAL_INT(TestData[i].ExpectedIntegerList[j], *peekedData);
        }
        
        /* Mock */
        for(j = 0; j < LIST_TEST_LIST_COUNT; j++)
            eclectic_free_ExpectAnyArgs();
        
        /* Destroy */
        TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_destroy(&list));
    }
    
    /* Free */
    free(memory);
}

/*** Push Head ***/
void test_list_pushHead_error(void)
{
    /*** Test Data ***/
    /* Variable */
    void *data;
    list_list_t list;
    
    /*** Push Head ***/
    /* Initialize */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_init(&list, NULL, NULL));
    
    /* NULL Pointer Error */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_ERROR_NULL_POINTER, list_pushHead(NULL, data));
    
    /* Mock */
    eclectic_malloc_ExpectAndReturn(sizeof(list_node_t), NULL);
    
    /* Memory Allocation Error */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_ERROR_MEMORY_ALLOCATION, list_pushHead(&list, data));
    TEST_ASSERT_EQUAL_UINT(0, list_size(&list));
}

void test_list_pushHead_success(void)
{
    /*** Test Data ***/
    /* Test Data */
    const int TestInteger = 24;

    /* Variable */
    int *data;
    list_list_t list;
    list_node_t *memory;
    int pushInteger;
    
    /* Set Up */
    pushInteger = TestInteger;
    memory = malloc(sizeof(list_node_t));
    
    /*** Push Head ***/
    /* Initialize */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_init(&list, NULL, NULL));
    
    /* Mock */
    eclectic_malloc_ExpectAndReturn(sizeof(list_node_t), memory);
    
    /* Push Head */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_pushHead(&list, &pushInteger));
    
    /* Size */
    TEST_ASSERT_EQUAL_UINT(1, list_size(&list));
    
    /* Peek At */
    data = list_peekAt(&list, 0);
    
    /* Verify */
    TEST_ASSERT_EQUAL_INT(pushInteger, *data);
    TEST_ASSERT_EQUAL_PTR(&pushInteger, data);
    
    /* Mock */
    eclectic_free_ExpectAnyArgs();
    
    /* Destroy */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_destroy(&list));
    
    /* Free */
    free(memory);
}

/*** Push Sorted ***/
void test_list_pushSorted_error(void)
{
    /*** Test Data ***/
    /* Test Data */
    const int TestData[3] = {-1, 1, 0};
    
    /* Variable */
    size_t i;
    list_list_t list;
    list_node_t *memory;
    int pushIntegerList[3];
    
    /* Set Up */
    memcpy(pushIntegerList, TestData, sizeof(TestData));
    memory = malloc(2 * sizeof(list_node_t));
    
    /*** Push Sorted ***/
    /* NULL Pointer Error */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_ERROR_NULL_POINTER, list_pushSorted(NULL, &pushIntegerList[0], false));
    
    /* Initialize */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_init(&list, NULL, NULL));
    
    /* Not Initialized Error */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_init(&list, NULL, NULL));
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_ERROR_NOT_INITIALIZED, list_pushSorted(&list, &pushIntegerList[0], false)); // list.compareCallback Is NULL
    
    /* Destroy */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_destroy(&list));
    
    /* Initialize */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_init(&list, listTest_compareCallback, NULL));
    
    /* Mock */
    eclectic_malloc_ExpectAndReturn(sizeof(list_node_t), &memory[0]);
    
    /* Push Sorted */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_pushSorted(&list, &pushIntegerList[0], false));
    
    /* Mock */
    eclectic_malloc_ExpectAndReturn(sizeof(list_node_t), &memory[1]);
    
    /* Push Sorted */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_pushSorted(&list, &pushIntegerList[1], false));
    
    /* Size */
    TEST_ASSERT_EQUAL_UINT(2, list_size(&list));
    
    /* Mock */
    eclectic_malloc_ExpectAndReturn(sizeof(list_node_t), NULL);
    
    /* Memory Allocation Error */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_ERROR_MEMORY_ALLOCATION, list_pushSorted(&list, &pushIntegerList[2], false));
    TEST_ASSERT_EQUAL_UINT(2, list_size(&list));
    
    /* Mock */
    for(i = 0; i < 2; i++)
        eclectic_free_ExpectAnyArgs();
    
    /* Destroy */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_destroy(&list));
    
    /* Free */
    free(memory);
}

void test_list_pushSorted_success_ascending(void)
{
    /*** Test Data ***/
    /* Type Definition */
    typedef struct testData_s
    {
        const int PushIntegerList[LIST_TEST_LIST_COUNT];
        const int ExpectedIntegerList[LIST_TEST_LIST_COUNT];
    } testData_t;

    /* Test Data */
    const testData_t TestData[] =
    {
        /* Same Value */
        {
            {-85, -85, -85, -85, -85},
            {-85, -85, -85, -85, -85}
        },
        {
            {0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0}
        },
        {
            {64, 64, 64, 64, 64},
            {64, 64, 64, 64, 64}
        },
        
        /* Different Values */
        {
            {-24, -63, -59, -64, -85},
            {-85, -64, -63, -59, -24}
        },
        {
            {24, 63, -59, 64, -85},
            {-85, -59, 24, 63, 64}
        },
        {
            {24, 63, 59, 64, 85},
            {24, 59, 63, 64, 85}
        }
    };
    const size_t TestDataCount = sizeof(TestData) / sizeof(TestData[0]);

    /* Variable */
    int *data;
    uint8_t i, j;
    list_list_t list;
    list_node_t *memory;
    int pushIntegerList[LIST_TEST_LIST_COUNT];
    
    /* Set Up */
    memory = malloc(LIST_TEST_LIST_COUNT * sizeof(list_node_t));
    
    /*** Push Sorted ***/
    for(i = 0; i < TestDataCount; i++)
    {        
        /* Set Up */
        memcpy(pushIntegerList, TestData[i].PushIntegerList, sizeof(TestData[i].PushIntegerList));
        TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_init(&list, listTest_compareCallback, NULL));
        for(j = 0; j < LIST_TEST_LIST_COUNT; j++)
        {
            /* Mock */
            eclectic_malloc_ExpectAndReturn(sizeof(list_node_t), &memory[j]);
            
            /* Push Sorted */
            TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_pushSorted(&list, &pushIntegerList[j], false));
            
            /* Size */
            TEST_ASSERT_EQUAL_UINT((j + 1), list_size(&list));
        }
        
        /* Peek At */
        for(j = 0; j < LIST_TEST_LIST_COUNT; j++)
        {
            /* Peek At */
            data = list_peekAt(&list, j);
            
            /* Verify */
            TEST_ASSERT_EQUAL_INT(TestData[i].ExpectedIntegerList[j], *data);
        }
        
        /* Mock */
        for(j = 0; j < LIST_TEST_LIST_COUNT; j++)
            eclectic_free_ExpectAnyArgs();
        
        /* Destroy */
        TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_destroy(&list));
    }
    
    /* Free */
    free(memory);
}

void test_list_pushSorted_success_descending(void)
{
    /*** Test Data ***/
    /* Type Definition */
    typedef struct testData_s
    {
        const int PushIntegerList[LIST_TEST_LIST_COUNT];
        const int ExpectedIntegerList[LIST_TEST_LIST_COUNT];
    } testData_t;

    /* Test Data */
    const testData_t TestData[] =
    {
        /* Same Value */
        {
            {-85, -85, -85, -85, -85},
            {-85, -85, -85, -85, -85}
        },
        {
            {0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0}
        },
        {
            {64, 64, 64, 64, 64},
            {64, 64, 64, 64, 64}
        },
        
        /* Different Values */
        {
            {-24, -63, -59, -64, -85},
            {-24, -59, -63, -64, -85}
        },
        {
            {24, 63, -59, 64, -85},
            {64, 63, 24, -59, -85}
        },
        {
            {24, 63, 59, 64, 85},
            {85, 64, 63, 59, 24}
        }
    };
    const size_t TestDataCount = sizeof(TestData) / sizeof(TestData[0]);

    /* Variable */
    int *data;
    uint8_t i, j;
    list_list_t list;
    list_node_t *memory;
    int pushIntegerList[LIST_TEST_LIST_COUNT];
    
    /* Set Up */
    memory = malloc(LIST_TEST_LIST_COUNT * sizeof(list_node_t));
    
    /*** Push Sorted ***/
    for(i = 0; i < TestDataCount; i++)
    {
        /* Set Up */
        memcpy(pushIntegerList, TestData[i].PushIntegerList, sizeof(TestData[i].PushIntegerList));
        TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_init(&list, listTest_compareCallback, NULL));
        for(j = 0; j < LIST_TEST_LIST_COUNT; j++)
        {
            /* Mock */
            eclectic_malloc_ExpectAndReturn(sizeof(list_node_t), &memory[j]);
            
            /* Push Sorted */
            TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_pushSorted(&list, &pushIntegerList[j], true));
            
            /* Size */
            TEST_ASSERT_EQUAL_UINT((j + 1), list_size(&list));
        }
        
        /* Peek At */
        for(j = 0; j < LIST_TEST_LIST_COUNT; j++)
        {
            /* Peek At */
            data = list_peekAt(&list, j);
            
            /* Verify */
            TEST_ASSERT_EQUAL_INT(TestData[i].ExpectedIntegerList[j], *data);
        }
        
        /* Mock */
        for(j = 0; j < LIST_TEST_LIST_COUNT; j++)
            eclectic_free_ExpectAnyArgs();
        
        /* Destroy */
        TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_destroy(&list));
    }
    
    /* Free */
    free(memory);
}

/*** Push Tail ***/
void test_list_pushTail_error(void)
{
    /*** Test Data ***/
    /* Variable */
    void *data;
    list_list_t list;
    
    /* Set Up */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_init(&list, NULL, NULL));
    
    /*** Push Tail ***/
    /* NULL Pointer Error */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_ERROR_NULL_POINTER, list_pushTail(NULL, data));
    
    /* Mock */
    eclectic_malloc_ExpectAndReturn(sizeof(list_node_t), NULL);
    
    /* Memory Allocation Error */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_ERROR_MEMORY_ALLOCATION, list_pushTail(&list, data));
    TEST_ASSERT_EQUAL_UINT(0, list_size(&list));
}

void test_list_pushTail_success(void)
{
    /*** Test Data ***/
    /* Test Data */
    const int TestInteger = 24;

    /* Variable */
    int *data;
    list_list_t list;
    list_node_t *memory;
    int pushInteger;
    
    /* Set Up */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_init(&list, NULL, NULL));
    memory = malloc(sizeof(list_node_t));
    pushInteger = TestInteger;
    
    /* Mock */
    eclectic_malloc_ExpectAndReturn(sizeof(list_node_t), memory);
    
    /*** Push Tail ***/
    /* Push Tail */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_pushTail(&list, &pushInteger));
    
    /* Size */
    TEST_ASSERT_EQUAL_UINT(1, list_size(&list));
    
    /* Peek At */
    data = list_peekAt(&list, 0);
    
    /* Verify */
    TEST_ASSERT_EQUAL_INT(pushInteger, *data);
    TEST_ASSERT_EQUAL_PTR(&pushInteger, data);
    
    /* Mock */
    eclectic_free_ExpectAnyArgs();
    
    /* Destroy */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_destroy(&list));
    
    /* Free */
    free(memory);
}

/*** Size ***/
void test_list_size_error(void)
{
    /*** Size ***/
    /* NULL Pointer Error */
    TEST_ASSERT_EQUAL_UINT(0, list_size(NULL));
}

void test_list_size_success(void)
{
    /*** Test Data ***/
    /* Test Data */
    const int TestData[] = {24, 63, 64, -69, -85};
    const size_t TestDataCount = sizeof(TestData) / sizeof(TestData[0]);

    /* Variable */
    int *data;
    uint8_t i;
    list_list_t list;
    list_node_t *memory;
    int pushIntegerList[TestDataCount];
    
    /* Set Up */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_init(&list, NULL, NULL));
    memory = malloc(sizeof(list_node_t));
    memcpy(pushIntegerList, TestData, sizeof(TestData));
    
    /*** Size ***/
    for(i = 0; i < TestDataCount; i++)
    {
        /* Mock */
        eclectic_malloc_ExpectAndReturn(sizeof(list_node_t), memory);
        
        /* Push Tail */
        TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_pushTail(&list, &pushIntegerList[i]));
        
        /* Size */
        TEST_ASSERT_EQUAL_UINT((i + 1), list_size(&list));
    }
    
    /* Mock */
    for(i = 0; i < TestDataCount; i++)
        eclectic_free_ExpectAnyArgs();
    
    /* Destroy */
    TEST_ASSERT_EQUAL_INT(ECLECTIC_STATUS_SUCCESS, list_destroy(&list));
    
    /* Free */
    free(memory);
}
