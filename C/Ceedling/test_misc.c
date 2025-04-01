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
        {0x80, 0x01},
        {0xAA, 0x55},
        {0xE3, 0xC7},
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
