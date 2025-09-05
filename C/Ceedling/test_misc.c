/****************************************************************************************************
 * Include
 ****************************************************************************************************/

#include "misc.h"
#include "unity.h"

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

/*** Insert 16 ***/
void test_insert16_1(void)
{
    /*** Insert 16 (Buffer NULL) ***/
    /* Variable */
    uint8_t buffer[sizeof(uint16_t)];

    /* Insert */
    misc_insert16(NULL, sizeof(buffer), 0xD00D, true); // true (Big Endian)
}

void test_insert16_2(void)
{
    /*** Insert 16 (Buffer Length Too Small) ***/
    /* Variable */
    uint8_t buffer[sizeof(uint8_t)];

    /* Insert */
    misc_insert16(buffer, sizeof(buffer), 0xD00D, true); // true (Big Endian)
}

void test_insert16_3(void)
{
    /*** Insert 16 (Big Endian) ***/
    /* Structure */
    typedef struct testData_s
    {
        uint16_t input;
        uint8_t expectedOutput[sizeof(uint16_t)];
    } testData_t;

    /* Test Data */
    const testData_t TestData[] =
    {
        {0x0000, {0x00, 0x00}},
        {0x0001, {0x00, 0x01}},
        {0x00FF, {0x00, 0xFF}},
        {0x1234, {0x12, 0x34}},
        {0xBEEF, {0xBE, 0xEF}},
        {0xFF00, {0xFF, 0x00}},
        {0xFFFF, {0xFF, 0xFF}}
    };
    const size_t TestDataCount = sizeof(TestData) / sizeof(TestData[0]);

    /* Variable */
    uint8_t actualOutput[sizeof(uint16_t)];

    /* Insert */
    for(size_t i = 0; i < TestDataCount; i++)
    {
        /* Extract */
        misc_insert16(actualOutput, sizeof(actualOutput), TestData[i].input, true); // true (Big Endian)

        /* Verify */
        TEST_ASSERT_EQUAL_HEX8_ARRAY(TestData[i].expectedOutput, actualOutput, sizeof(TestData[i].expectedOutput));
    }
}

void test_insert16_4(void)
{
    /*** Insert 16 (Little Endian) ***/
    /* Structure */
    typedef struct testData_s
    {
        uint16_t input;
        uint8_t expectedOutput[sizeof(uint16_t)];
    } testData_t;

    /* Test Data */
    const testData_t TestData[] =
    {
		{0x0000, {0x00, 0x00}},
        {0x0001, {0x01, 0x00}},
        {0x00FF, {0xFF, 0x00}},
        {0x1234, {0x34, 0x12}},
        {0xBEEF, {0xEF, 0xBE}},
        {0xFF00, {0x00, 0xFF}},
        {0xFFFF, {0xFF, 0xFF}}
    };
    const size_t TestDataCount = sizeof(TestData) / sizeof(TestData[0]);

    /* Variable */
    uint8_t actualOutput[sizeof(uint16_t)];

    /* Insert */
    for(size_t i = 0; i < TestDataCount; i++)
    {
        /* Extract */
        misc_insert16(actualOutput, sizeof(actualOutput), TestData[i].input, false); // true (Little Endian)

        /* Verify */
        TEST_ASSERT_EQUAL_HEX8_ARRAY(TestData[i].expectedOutput, actualOutput, sizeof(TestData[i].expectedOutput));
    }
}

/*** Insert 32 ***/
void test_insert32_1(void)
{
    /*** Insert 32 (Buffer NULL) ***/
    /* Variable */
    uint8_t buffer[sizeof(uint32_t)];

    /* Insert */
    misc_insert32(NULL, sizeof(buffer), 0xCAFED00D, true); // true (Big Endian)
}

void test_insert32_2(void)
{
    /*** Insert 32 (Buffer Length Too Small) ***/
    /* Variable */
    uint8_t buffer[sizeof(uint16_t)];

    /* Insert */
    misc_insert32(buffer, sizeof(buffer), 0xCAFED00D, true); // true (Big Endian)
}

void test_insert32_3(void)
{
    /*** Insert 32 (Big Endian) ***/
    /* Structure */
    typedef struct testData_s
    {
        uint32_t input;
        uint8_t expectedOutput[sizeof(uint32_t)];
    } testData_t;

    /* Test Data */
    const testData_t TestData[] =
    {
        {0x00000000, {0x00, 0x00, 0x00, 0x00}},
        {0x00000001, {0x00, 0x00, 0x00, 0x01}},
        {0x000000FF, {0x00, 0x00, 0x00, 0xFF}},
        {0x12345678, {0x12, 0x34, 0x56, 0x78}},
        {0xDEADBEEF, {0xDE, 0xAD, 0xBE, 0xEF}},
        {0xFF000000, {0xFF, 0x00, 0x00, 0x00}},
        {0xFFFFFFFF, {0xFF, 0xFF, 0xFF, 0xFF}}
    };
    const size_t TestDataCount = sizeof(TestData) / sizeof(TestData[0]);

    /* Variable */
    uint8_t actualOutput[sizeof(uint32_t)];

    /* Insert */
    for(size_t i = 0; i < TestDataCount; i++)
    {
        /* Extract */
        misc_insert32(actualOutput, sizeof(actualOutput), TestData[i].input, true); // true (Big Endian)

        /* Verify */
        TEST_ASSERT_EQUAL_HEX8_ARRAY(TestData[i].expectedOutput, actualOutput, sizeof(TestData[i].expectedOutput));
    }
}

void test_insert32_4(void)
{
    /*** Insert 32 (Little Endian) ***/
    /* Structure */
    typedef struct testData_s
    {
        uint32_t input;
        uint8_t expectedOutput[sizeof(uint32_t)];
    } testData_t;

    /* Test Data */
    const testData_t TestData[] =
    {
		{0x00000000, {0x00, 0x00, 0x00, 0x00}},
        {0x00000001, {0x01, 0x00, 0x00, 0x00}},
        {0x000000FF, {0xFF, 0x00, 0x00, 0x00}},
        {0x12345678, {0x78, 0x56, 0x34, 0x12}},
        {0xDEADBEEF, {0xEF, 0xBE, 0xAD, 0xDE}},
        {0xFF000000, {0x00, 0x00, 0x00, 0xFF}},
        {0xFFFFFFFF, {0xFF, 0xFF, 0xFF, 0xFF}}
    };
    const size_t TestDataCount = sizeof(TestData) / sizeof(TestData[0]);

    /* Variable */
    uint8_t actualOutput[sizeof(uint32_t)];

    /* Insert */
    for(size_t i = 0; i < TestDataCount; i++)
    {
        /* Extract */
        misc_insert32(actualOutput, sizeof(actualOutput), TestData[i].input, false); // true (Little Endian)

        /* Verify */
        TEST_ASSERT_EQUAL_HEX8_ARRAY(TestData[i].expectedOutput, actualOutput, sizeof(TestData[i].expectedOutput));
    }
}

/*** Reflect 8 ***/
void test_reflect8(void)
{
    /*** Reflect 8 ***/
    /* Structure */
    typedef struct testData_s
    {
        uint8_t input;
        uint8_t expectedOutput;
    } testData_t;

    /* Test Data */
    const testData_t TestData[] =
    {
        {0x00, 0x00},
        {0x01, 0x80},
        {0xA5, 0xA5},
        {0xAA, 0x55},
        {0xFE, 0x7F},
        {0xFF, 0xFF}
    };
    const size_t TestDataCount = sizeof(TestData) / sizeof(TestData[0]);

    /* Variable */
    uint8_t actualOutput;

    /* Reflect */
    for(size_t i = 0; i < TestDataCount; i++)
    {
        /* Reflect */
        actualOutput = misc_reflect8(TestData[i].input);

        /* Verify */
        TEST_ASSERT_EQUAL_HEX8(TestData[i].expectedOutput, actualOutput);
    }
}

/*** Reflect 16 ***/
void test_reflect16(void)
{
    /*** Reflect 16 ***/
    /* Structure */
    typedef struct testData_s
    {
        uint16_t input;
        uint16_t expectedOutput;
    } testData_t;

    /* Test Data */
    const testData_t TestData[] =
    {
        {0x0000, 0x0000},
        {0x0001, 0x8000},
        {0xAA55, 0xAA55},
        {0xAAAA, 0x5555},
        {0xFFFE, 0x7FFF},
        {0xFFFF, 0xFFFF}
    };
    const size_t TestDataCount = sizeof(TestData) / sizeof(TestData[0]);

    /* Variable */
    uint16_t actualOutput;

    /* Reflect */
    for(size_t i = 0; i < TestDataCount; i++)
    {
        /* Reflect */
        actualOutput = misc_reflect16(TestData[i].input);

        /* Verify */
        TEST_ASSERT_EQUAL_HEX16(TestData[i].expectedOutput, actualOutput);
    }
}

/*** Reflect 32 ***/
void test_reflect32(void)
{
    /*** Reflect 32 ***/
    /* Structure */
    typedef struct testData_s
    {
        uint32_t input;
        uint32_t expectedOutput;
    } testData_t;

    /* Test Data */
    const testData_t TestData[] =
    {
        {0x00000000, 0x00000000},
        {0x00000001, 0x80000000},
        {0xAAAA5555, 0xAAAA5555},
        {0xAAAAAAAA, 0x55555555},
        {0xFFFFFFFE, 0x7FFFFFFF},
        {0xFFFFFFFF, 0xFFFFFFFF}
    };
    const size_t TestDataCount = sizeof(TestData) / sizeof(TestData[0]);

    /* Variable */
    uint32_t actualOutput;

    /* Reflect */
    for(size_t i = 0; i < TestDataCount; i++)
    {
        /* Reflect */
        actualOutput = misc_reflect32(TestData[i].input);

        /* Verify */
        TEST_ASSERT_EQUAL_HEX32(TestData[i].expectedOutput, actualOutput);
    }
}
