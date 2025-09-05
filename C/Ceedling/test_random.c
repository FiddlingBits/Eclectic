/****************************************************************************************************
 * Include
 ****************************************************************************************************/

#include "random.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "unity.h"

/****************************************************************************************************
 * Variable
 ****************************************************************************************************/

extern random_U32Callback_t random_U32Callback;

/****************************************************************************************************
 * Helper
 ****************************************************************************************************/

uint32_t test_U32Callback(void)
{
    int32_t s32 =  ((rand() & 0xFFFF) << 16) | (rand() & 0xFFFF);
    return s32;
}

/****************************************************************************************************
 * Set Up/Tear Down
 ****************************************************************************************************/

void setUp(void)
{
    srand(time(NULL));
    random_init(test_U32Callback);
}

void tearDown(void)
{
}

/****************************************************************************************************
 * Test (Public)
 ****************************************************************************************************/

/*** Initialize ***/
void test_init_1(void)
{
    /*** Initialize ***/
    /* Initialize */
    random_init(test_U32Callback);

    /* Verify */
    TEST_ASSERT_EQUAL_PTR(test_U32Callback, random_U32Callback);
}

/*** Signed 32-Bit ***/
void test_S32_1(void)
{
    /*** Signed 32-Bit (U32 Callback NULL) ***/
    /* Variable */
    int32_t s32;

    /* Set Up */
    random_U32Callback = NULL;

    /* Signed 32-Bit */
    s32 = random_S32();

    /* Verify */
    TEST_ASSERT_EQUAL_INT32(0, s32);
}

void test_S32_2(void)
{
    /*** Signed 32-Bit ***/
    /* Variable */
    int32_t s32;

    /* Signed 32-Bit */
    for(size_t i = 0; i < 5; i++)
    {
        s32 = random_S32();
        if(s32 != 0) // It Shouldn't Be Zero Every Time
            break;
    }

    /* Verify */
    TEST_ASSERT_TRUE(s32 != 0);
}

/*** Signed 32-Bit Within Range ***/
void test_S32WithinRange_1(void)
{
    /*** Signed 32-Bit Within Range (U32 Callback NULL) ***/
    /* Variable */
    int32_t s32;

    /* Set Up */
    random_U32Callback = NULL;

    /* Signed 32-Bit */
    s32 = random_S32WithinRange(-10, 10);

    /* Verify */
    TEST_ASSERT_EQUAL_INT32(0, s32);
}

void test_S32WithinRange_2(void)
{
    /*** Signed 32-Bit Within Range (Minimum > Maximum) ***/
    /* Variable */
    int32_t s32;

    /* Signed 32-Bit */
    s32 = random_S32WithinRange(10, -10);

    /* Verify */
    TEST_ASSERT_EQUAL_INT32(0, s32);
}

void test_S32WithinRange_3(void)
{
    /*** Signed 32-Bit Within Range (Minimum == Maximum) ***/
    /* Structure */
    typedef struct
    {
        const int32_t range;
        const int32_t expectedOutput;
    } testData_t;

    /* Test Data */
    const testData_t TestData[] =
    {
        {-10, -10},
        {0, 0},
        {10, 10}
    };
    const size_t TestDataCount = sizeof(TestData) / sizeof(TestData[0]);

    /* Variable */
    int32_t s32;

    /* Signed 32-Bit Within Range */
    for(size_t i = 0; i < TestDataCount; i++)
    {
        /* Signed 32-Bit Within Range */
        s32 = random_S32WithinRange(TestData[i].range, TestData[i].range);

        /* Verify */
        TEST_ASSERT_EQUAL_INT32(TestData[i].expectedOutput, s32);
    }
}

void test_S32WithinRange_4(void)
{
    /*** Signed 32-Bit Within Range (Minimum < Maximum) ***/
    /* Structure */
    typedef struct
    {
        const int32_t minimum;
        const int32_t maximum;
    } testData_t;

    /* Test Data */
    const testData_t TestData[] =
    {
        {-11, -1},
        {-5, 5},
        {1, 11}
    };
    const size_t TestDataCount = sizeof(TestData) / sizeof(TestData[0]);

    /* Variable */
    int32_t count[11], s32;

    /* Signed 32-Bit Within Range */
    for(size_t i = 0; i < TestDataCount; i++)
    {
        /* Set Up */
        memset(count, 0, sizeof(count));

        /* Signed 32-Bit Within Range */
        for(size_t j = 0; j < 100; j++)
        {
            /* Signed 32-Bit Within Range */
            s32 = random_S32WithinRange(TestData[i].minimum, TestData[i].maximum);

            /* Verify */
            TEST_ASSERT_TRUE((s32 >= TestData[i].minimum) && (s32 <= TestData[i].maximum));

            /* Count */
            count[s32 - TestData[i].minimum]++;
        }

        /* Verify */
        for(size_t j = 0; j < 11; j++)
            TEST_ASSERT_TRUE(count[j] > 0);
    }
}

/*** Unsigned 32-Bit ***/
void test_U32_1(void)
{
    /*** Unsigned 32-Bit (U32 Callback NULL) ***/
    /* Variable */
    uint32_t u32;

    /* Set Up */
    random_U32Callback = NULL;

    /* Unsigned 32-Bit */
    u32 = random_U32();

    /* Verify */
    TEST_ASSERT_EQUAL_INT32(0, u32);
}

void test_U32_2(void)
{
    /*** Unsigned 32-Bit ***/
    /* Variable */
    uint32_t u32;

    /* Unsigned 32-Bit */
    for(size_t i = 0; i < 5; i++)
    {
        u32 = random_U32();
        if(u32 != 0) // It Shouldn't Be Zero Every Time
            break;
    }

    /* Verify */
    TEST_ASSERT_TRUE(u32 != 0);
}

/*** Unsigned 32-Bit Within Range ***/
void test_U32WithinRange_1(void)
{
    /*** Unsigned 32-Bit Within Range (U32 Callback NULL) ***/
    /* Variable */
    uint32_t u32;

    /* Set Up */
    random_U32Callback = NULL;

    /* Unsigned 32-Bit */
    u32 = random_U32WithinRange(0, 11);

    /* Verify */
    TEST_ASSERT_EQUAL_INT32(0, u32);
}

void test_U32WithinRange_2(void)
{
    /*** Unsigned 32-Bit Within Range (Minimum > Maximum) ***/
    /* Variable */
    uint32_t u32;

    /* Unsigned 32-Bit */
    u32 = random_U32WithinRange(11, 0);

    /* Verify */
    TEST_ASSERT_EQUAL_INT32(0, u32);
}

void test_U32WithinRange_3(void)
{
    /*** Unsigned 32-Bit Within Range (Minimum == Maximum) ***/
    /* Structure */
    typedef struct
    {
        const uint32_t range;
        const uint32_t expectedOutput;
    } testData_t;

    /* Test Data */
    const testData_t TestData[] =
    {
        {0, 0},
        {10, 10}
    };
    const size_t TestDataCount = sizeof(TestData) / sizeof(TestData[0]);

    /* Variable */
    uint32_t u32;

    /* Unsigned 32-Bit Within Range */
    for(size_t i = 0; i < TestDataCount; i++)
    {
        /* Unsigned 32-Bit Within Range */
        u32 = random_U32WithinRange(TestData[i].range, TestData[i].range);

        /* Verify */
        TEST_ASSERT_EQUAL_INT32(TestData[i].expectedOutput, u32);
    }
}

void test_U32WithinRange_4(void)
{
    /*** Unsigned 32-Bit Within Range (Minimum < Maximum) ***/
    /* Constant */
    uint32_t Maximum = 11, Minimum = 0;

    /* Variable */
    uint32_t count[11], u32;

    /* Set Up */
    memset(count, 0, sizeof(count));

    /* Unsigned 32-Bit Within Range */
    for(size_t i = 0; i < 100; i++)
    {
        /* Unsigned 32-Bit Within Range */
        u32 = random_U32WithinRange(Minimum, Maximum);

        /* Verify */
        TEST_ASSERT_TRUE((u32 >= Minimum) && (u32 <= Maximum));

        /* Count */
        count[u32 - Minimum]++;
    }

    /* Verify */
    for(size_t j = 0; j < 11; j++)
        TEST_ASSERT_TRUE(count[j] > 0);
}

/*** Set Buffer ***/
void test_setBuffer_1(void)
{
    /*** Set Buffer (U32 Callback NULL) ***/
    /* Variable */
    uint8_t actualBuffer[10], expectedBuffer[10];

    /* Set Up */
    memset(expectedBuffer, 0, sizeof(expectedBuffer));
    random_U32Callback = NULL;

    /* Set Buffer */
    random_setBuffer(actualBuffer, sizeof(actualBuffer));

    /* Verify */
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedBuffer, actualBuffer, sizeof(expectedBuffer));
}

void test_setBuffer_2(void)
{
    /*** Set Buffer (Buffer NULL) ***/
    /* Variable */
    uint8_t buffer[10];

    /* Set Buffer */
    random_setBuffer(NULL, sizeof(buffer));
}

void test_setBuffer_3(void)
{
    /*** Set Buffer (Buffer NULL) ***/
    /* Variable */
    uint8_t buffer[10];

    /* Set Buffer */
    random_setBuffer(buffer, 0);
}

void test_setBuffer_4(void)
{
    /*** Set Buffer ***/
    /* Variable */
    uint8_t buffer[10];
    size_t nonZero;

    /* Set Up */
    nonZero = 0;

    /* Set Buffer */
    random_setBuffer(buffer, sizeof(buffer));

    /* Verify */
    for(size_t i = 0; i < sizeof(buffer); i++)
    {
        if(buffer[i] > 0)
            nonZero++;
    }
    TEST_ASSERT_TRUE(nonZero > 0);
}
