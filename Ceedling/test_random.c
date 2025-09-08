/****************************************************************************************************
 * Define
 ****************************************************************************************************/

#define TEST_RANDOM_NUM_SAMPLES (10000)
#define TEST_RANDOM_TOLERANCE   (0.05)

/****************************************************************************************************
 * Include
 ****************************************************************************************************/

#include "random.h"
#include <stdlib.h>
#include <string.h>
#include "unity.h"

/****************************************************************************************************
 * Helper
 ****************************************************************************************************/

/*** S32 Buffer Standard Deviation ***/
double helper_S32BufferStandardDeviation(const int32_t * const S32Buffer, const size_t S32BufferSize)
{
    /*** S32 Buffer Standard Deviation ***/
    /* Variable */
    double mean = 0.0;
    double variance;
    
    /* Set Up */
    variance = 0.0;
    
    /* Calculate Mean */
    for(size_t i = 0; i < S32BufferSize; i++)
        mean += S32Buffer[i];
    mean /= (double)S32BufferSize;
    
    /* Calculate Variance */
    for(size_t i = 0; i < S32BufferSize; i++)
        variance += pow((S32Buffer[i] - mean), 2.0);
    variance /= (double)S32BufferSize;
        
    /* Exit */
    return sqrt(variance); // Standard Deviation
}

/*** U8 Buffer Standard Deviation ***/
double helper_U8BufferStandardDeviation(const uint8_t * const U8Buffer, const size_t U8BufferSize)
{
    /*** U8 Buffer Standard Deviation ***/
    /* Variable */
    double mean = 0.0;
    double variance;
    
    /* Set Up */
    variance = 0.0;
    
    /* Calculate Mean */
    for(size_t i = 0; i < U8BufferSize; i++)
        mean += U8Buffer[i];
    mean /= (double)U8BufferSize;
    
    /* Calculate Variance */
    for(size_t i = 0; i < U8BufferSize; i++)
        variance += pow((U8Buffer[i] - mean), 2.0);
    variance /= (double)U8BufferSize;
        
    /* Exit */
    return sqrt(variance); // Standard Deviation
}

/*** U32 Buffer Standard Deviation ***/
double helper_U32BufferStandardDeviation(const uint32_t * const U32Buffer, const size_t U32BufferSize)
{
    /*** U32 Buffer Standard Deviation ***/
    /* Variable */
    double mean = 0.0;
    double variance;
    
    /* Set Up */
    variance = 0.0;
    
    /* Calculate Mean */
    for(size_t i = 0; i < U32BufferSize; i++)
        mean += U32Buffer[i];
    mean /= (double)U32BufferSize;
    
    /* Calculate Variance */
    for(size_t i = 0; i < U32BufferSize; i++)
        variance += pow((U32Buffer[i] - mean), 2.0);
    variance /= (double)U32BufferSize;
        
    /* Exit */
    return sqrt(variance); // Standard Deviation
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

/*** Buffer ***/
void test_buffer_1(void)
{
    /*** Buffer (NULL And Zero Length) ***/
    /* Variable */
    uint8_t buffer[1];
    
    /* Buffer */
    random_buffer(NULL, sizeof(buffer));
    random_buffer(buffer, 0);
    random_buffer(NULL, 0);
}

void test_buffer_2(void)
{
    /*** Buffer ***/
    /* Constant */
    const double ExpectedStandardDeviation = sqrt((pow(UINT8_MAX - 0, 2.0) - 1.0) / 12.0);
    
    /* Variable */
    double actualStandardDeviation;
    uint8_t buffer[TEST_RANDOM_NUM_SAMPLES];

    /* Unsigned 8-Bit */
    random_buffer(buffer, sizeof(buffer));
    
    /* Standard Deviation */
    actualStandardDeviation = helper_U8BufferStandardDeviation(buffer, TEST_RANDOM_NUM_SAMPLES);

    /* Verify */
    TEST_ASSERT_DOUBLE_WITHIN((ExpectedStandardDeviation * TEST_RANDOM_TOLERANCE), ExpectedStandardDeviation, actualStandardDeviation);
}

/*** Signed 32-Bit ***/
void test_S32_1(void)
{
    /*** Signed 32-Bit ***/    
    /* Constant */
    const double ExpectedStandardDeviation = sqrt((pow((double)INT32_MAX - INT32_MIN, 2.0) - 1.0) / 12.0);
    
    /* Variable */
    double actualStandardDeviation;
    int32_t s32[TEST_RANDOM_NUM_SAMPLES];

    /* Signed 32-Bit */
    for(size_t i = 0; i < TEST_RANDOM_NUM_SAMPLES; i++)
        s32[i] = random_S32();
    
    /* Standard Deviation */
    actualStandardDeviation = helper_S32BufferStandardDeviation(s32, TEST_RANDOM_NUM_SAMPLES);

    /* Verify */
    TEST_ASSERT_DOUBLE_WITHIN((ExpectedStandardDeviation * TEST_RANDOM_TOLERANCE), ExpectedStandardDeviation, actualStandardDeviation);
}

/*** Signed 32-Bit Within Range ***/
void test_S32WithinRange_1(void)
{
    /*** Signed 32-Bit Within Range: Minimum > Maximum ***/
    TEST_ASSERT_EQUAL_INT32(0, random_S32WithinRange(INT32_MAX, INT32_MAX - 1));
}

void test_S32WithinRange_2(void)
{
    /*** Signed 32-Bit Within Range ***/    
    /* Structure */
    typedef struct
    {
        int32_t min, max;
    } testData_t;
    
    /* Test Data */
    const testData_t TestData[] =
    {
        {INT32_MIN, INT32_MIN},         // Zero Range At Minimum
        {0, 0},                         // Zero Range In Middle
        {INT32_MAX, INT32_MAX},         // Zero Range At Maximum
        {INT32_MIN, INT32_MIN + 1},     // Small Range At Near Minimum
        {INT32_MAX - 1, INT32_MAX},     // Small Range At Near Maximum
        {-123, 456},                    // Small Arbitrary Range
        {-5000, 500000},                // Medium Arbitrary Range
        {-100000, 2000000},             // Larger Arbitrary Range
        {INT32_MIN + 1, INT32_MAX - 1}, // Almost Full Range
        {INT32_MIN, INT32_MAX}          // Full Range
    };
    const size_t TestDataCount = sizeof(TestData) / sizeof(TestData[0]);
    
    /* Variable */
    double actualStandardDeviation, expectedStandardDeviation;
    int64_t range;
    int32_t s32[TEST_RANDOM_NUM_SAMPLES];
    
    /* Test */
    for(size_t i = 0; i < TestDataCount; i++)
    {
        /* Set Up */
        range = (int64_t)TestData[i].max - TestData[i].min + 1;
        expectedStandardDeviation = sqrt((pow((double)range, 2.0) - 1.0) / 12.0);
        
        /* Signed 32-Bit Within Range */
        for(size_t j = 0; j < TEST_RANDOM_NUM_SAMPLES; j++)
        {
            s32[j] = random_S32WithinRange(TestData[i].min, TestData[i].max);
            TEST_ASSERT((s32[j] >= TestData[i].min) && (s32[j] <= TestData[i].max));
        }
        
        /* Standard Deviation */
        actualStandardDeviation = helper_S32BufferStandardDeviation(s32, TEST_RANDOM_NUM_SAMPLES);

        /* Verify */
        TEST_ASSERT_DOUBLE_WITHIN((expectedStandardDeviation * TEST_RANDOM_TOLERANCE), expectedStandardDeviation, actualStandardDeviation);
    }
}

/*** Unsigned 32-Bit ***/
void test_U32_1(void)
{
    /*** Unsigned 32-Bit ***/    
    /* Constant */
    const double ExpectedStandardDeviation = sqrt((pow(UINT32_MAX - 0, 2.0) - 1.0) / 12.0);
    
    /* Variable */
    double actualStandardDeviation;
    uint32_t u32[TEST_RANDOM_NUM_SAMPLES];

    /* Unsigned 32-Bit */
    for(size_t i = 0; i < TEST_RANDOM_NUM_SAMPLES; i++)
        u32[i] = random_U32();
    
    /* Standard Deviation */
    actualStandardDeviation = helper_U32BufferStandardDeviation(u32, TEST_RANDOM_NUM_SAMPLES);

    /* Verify */
    TEST_ASSERT_DOUBLE_WITHIN((ExpectedStandardDeviation * TEST_RANDOM_TOLERANCE), ExpectedStandardDeviation, actualStandardDeviation);
}

/*** Unsigned 32-Bit Within Range ***/
void test_U32WithinRange_1(void)
{
    /*** Unsigned 32-Bit Within Range: Minimum > Maximum ***/
    TEST_ASSERT_EQUAL_UINT32(0, random_U32WithinRange(UINT32_MAX, UINT32_MAX - 1));
}

void test_U32WithinRange_2(void)
{
    /*** Unsigned 32-Bit Within Range ***/    
    /* Structure */
    typedef struct
    {
        uint32_t min, max;
    } testData_t;
    
    /* Test Data */
    const testData_t TestData[] =
    {
        {0, 0},                           // Zero Range At Minimum
        {UINT32_MAX / 2, UINT32_MAX / 2}, // Zero Range In Middle
        {UINT32_MAX, UINT32_MAX},         // Zero Range At Maximum
        {0, 1},                           // Small Range At Near Minimum
        {UINT32_MAX - 1, UINT32_MAX},     // Small Range At Near Maximum
        {123, 456},                       // Small Arbitrary Range
        {5000, 500000},                   // Medium Arbitrary Range
        {100000, 2000000},                // Larger Arbitrary Range
        {1, UINT32_MAX - 1},              // Almost Full Range
        {0, UINT32_MAX}                   // Full Range
    };
    const size_t TestDataCount = sizeof(TestData) / sizeof(TestData[0]);
    
    /* Variable */
    double actualStandardDeviation, expectedStandardDeviation;
    uint32_t u32[TEST_RANDOM_NUM_SAMPLES];
    uint64_t range;
    
    /* Test */
    for(size_t i = 0; i < TestDataCount; i++)
    {
        /* Set Up */
        range = (uint64_t)TestData[i].max - TestData[i].min + 1;
        expectedStandardDeviation = sqrt((pow((double)range, 2.0) - 1.0) / 12.0);
        
        /* Unsigned 32-Bit Within Range */
        for(size_t j = 0; j < TEST_RANDOM_NUM_SAMPLES; j++)
        {
            u32[j] = random_U32WithinRange(TestData[i].min, TestData[i].max);
            TEST_ASSERT((u32[j] >= TestData[i].min) && (u32[j] <= TestData[i].max));
        }
        
        /* Standard Deviation */
        actualStandardDeviation = helper_U32BufferStandardDeviation(u32, TEST_RANDOM_NUM_SAMPLES);

        /* Verify */
        TEST_ASSERT_DOUBLE_WITHIN((expectedStandardDeviation * TEST_RANDOM_TOLERANCE), expectedStandardDeviation, actualStandardDeviation);
    }
}
