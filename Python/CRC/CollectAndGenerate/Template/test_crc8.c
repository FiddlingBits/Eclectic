/****************************************************************************************************
 * Include
 ****************************************************************************************************/

#include "crc8.h"
#include "misc.h"
#include "mock_memory.h"
#include "random.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "unity.h"

/****************************************************************************************************
 * Constant
 ****************************************************************************************************/

const uint8_t test_CheckData[9] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};

/****************************************************************************************************
 * Helper
 ****************************************************************************************************/

uint32_t test_U32Callback(void)
{
    return ((rand() & 0xFFFF) << 16) | (rand() & 0xFFFF);
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

/*** Calculate ***/
void test_calculate_1(void)
{
    /*** Calculate (Configuration NULL) ***/
    /* Variable */
    uint8_t data[1];

    /* Calculate */
    TEST_ASSERT_EQUAL_HEX8(0x00, crc8_calculate(NULL, data, sizeof(data)));
}

void test_calculate_2(void)
{
    /*** Calculate (Data NULL) ***/
    /* Variable */
    crc8_configuration_t configuration;
    uint8_t data[1];

    /* Calculate */
    TEST_ASSERT_EQUAL_HEX8(0x00, crc8_calculate(&configuration, NULL, sizeof(data)));
}

void test_calculate_3(void)
{
    /*** Calculate (Loop) ***/
    /* Constant */
    const char *CrcName[] = ##### REPLACEMENT 1 #####;
    const size_t CrcNameCount = sizeof(CrcName) / sizeof(CrcName[0]);

    /* Variable */
    crc8_configuration_t configuration;
    uint8_t crc;

    /* Calculate */
    for(size_t i = 0; i < CrcNameCount; i++)
    {
        /* Initialize */
        crc8_init(CrcName[i], &configuration, false); // false (Loop)

        /* Calculate */
        crc = crc8_calculate(&configuration, test_CheckData, sizeof(test_CheckData));

        /* Verify */
        TEST_ASSERT_EQUAL_HEX8(configuration.check, crc);
    }
}

void test_calculate_4(void)
{
    /*** Calculate (Lookup Table) ***/
    /* Constant */
    const char *CrcName[] = ##### REPLACEMENT 1 #####;
    const size_t CrcNameCount = sizeof(CrcName) / sizeof(CrcName[0]);

    /* Variable */
    crc8_configuration_t configuration;
    uint8_t crc;
    void *memory;

    /* Calculate */
    for(size_t i = 0; i < CrcNameCount; i++)
    {
        /* Set Up */
        memory = malloc(CRC8_LOOKUP_TABLE_MEMORY_SIZE);

        /* Mock */
        memory_malloc_ExpectAndReturn(CRC8_LOOKUP_TABLE_MEMORY_SIZE, memory);

        /* Initialize */
        crc8_init(CrcName[i], &configuration, true); // true (Lookup Table)

        /* Calculate */
        crc = crc8_calculate(&configuration, test_CheckData, sizeof(test_CheckData));

        /* Verify */
        TEST_ASSERT_EQUAL_HEX8(configuration.check, crc);

        /* Clean Up */
        free(memory);
    }
}

/*** Calculate And Append ***/
void test_calculateAndAppend_1(void)
{
    /*** Calculate And Append (Configuration NULL) ***/
    /* Variable */
    uint8_t buffer[sizeof(test_CheckData) + sizeof(uint8_t)];

    /* Calculate And Append */
    crc8_calculateAndAppend(NULL, buffer, sizeof(buffer), sizeof(buffer) - sizeof(uint8_t));
}

void test_calculateAndAppend_2(void)
{
    /*** Calculate And Append (Buffer NULL) ***/
    /* Variable */
    uint8_t buffer[sizeof(test_CheckData) + sizeof(uint8_t)];
    crc8_configuration_t configuration;

    /* Calculate And Append */
    crc8_calculateAndAppend(&configuration, NULL, sizeof(buffer), sizeof(buffer) - sizeof(uint8_t));
}

void test_calculateAndAppend_3(void)
{
    /*** Calculate And Append (Buffer Too Small) ***/
    /* Variable */
    uint8_t buffer[sizeof(test_CheckData)];
    crc8_configuration_t configuration;

    /* Calculate And Append */
    crc8_calculateAndAppend(&configuration, buffer, sizeof(buffer), sizeof(buffer));
}

void test_calculateAndAppend_4(void)
{
    /*** Calculate And Append (Loop) ***/
    /* Constant */
    const uint8_t ExpectedBuffer[sizeof(test_CheckData) + sizeof(uint8_t)] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', ##### REPLACEMENT 2 #####};

    /* Variable */
    uint8_t actualBuffer[sizeof(test_CheckData) + sizeof(uint8_t)];
    crc8_configuration_t configuration;

    /* Set Up */
    (void)memcpy(actualBuffer, test_CheckData, sizeof(test_CheckData));

    /* Calculate And Append */
    crc8_init(##### REPLACEMENT 3 #####, &configuration, false); // false (Loop)
    crc8_calculateAndAppend(&configuration, actualBuffer, sizeof(actualBuffer), sizeof(actualBuffer) - sizeof(uint8_t));

    /* Verify */
    TEST_ASSERT_EQUAL_HEX8_ARRAY(ExpectedBuffer, actualBuffer, sizeof(ExpectedBuffer));
}

void test_calculateAndAppend_5(void)
{
    /*** Calculate And Append (Lookup Table) ***/
    /* Constant */
    const uint8_t ExpectedBuffer[sizeof(test_CheckData) + sizeof(uint8_t)] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', ##### REPLACEMENT 2 #####};

    /* Variable */
    uint8_t actualBuffer[sizeof(test_CheckData) + sizeof(uint8_t)];
    crc8_configuration_t configuration;
    void *memory;

    /* Set Up */
    (void)memcpy(actualBuffer, test_CheckData, sizeof(test_CheckData));
    memory = malloc(CRC8_LOOKUP_TABLE_MEMORY_SIZE);

    /* Mock */
    memory_malloc_ExpectAndReturn(CRC8_LOOKUP_TABLE_MEMORY_SIZE, memory);

    /* Calculate And Append */
    crc8_init(##### REPLACEMENT 3 #####, &configuration, true); // true (Lookup Table)
    crc8_calculateAndAppend(&configuration, actualBuffer, sizeof(actualBuffer), sizeof(actualBuffer) - sizeof(uint8_t));

    /* Verify */
    TEST_ASSERT_EQUAL_HEX8_ARRAY(ExpectedBuffer, actualBuffer, sizeof(ExpectedBuffer));

    /* Clean Up */
    free(memory);
}

/*** Calculate Partial ***/
void test_calculatePartial_1(void)
{
    /*** Calculate Partial (Configuration NULL) ***/
    /* Variable */
    uint8_t crc;

    /* Calculate Partial */
    crc = crc8_calculatePartial(NULL, 0x00, 0x00, false);

    /* Verify */
    TEST_ASSERT_EQUAL_HEX8(0x00, crc);
}

void test_calculatePartial_2(void)
{
    /*** Calculate Partial (Loop) ***/
    /* Constant */
    const char *CrcName[] = ##### REPLACEMENT 1 #####;
    const size_t CrcNameCount = sizeof(CrcName) / sizeof(CrcName[0]);

    /* Variable */
    crc8_configuration_t configuration;
    uint8_t crc;

    /* Calculate Partial */
    for(size_t i = 0; i < CrcNameCount; i++)
    {
        /* Set Up */
        crc8_init(CrcName[i], &configuration, false); // false (Loop)

        /* Calculate */
        crc = configuration.initial;
        for(size_t j = 0; j < sizeof(test_CheckData); j++)
            crc = crc8_calculatePartial(&configuration, crc, test_CheckData[j], (j == (sizeof(test_CheckData) - 1)));

        /* Verify */
        TEST_ASSERT_EQUAL_HEX8(configuration.check, crc);
    }
}

void test_calculatePartial_3(void)
{
    /*** Calculate Partial (Lookup Table) ***/
    /* Constant */
    const char *CrcName[] = ##### REPLACEMENT 1 #####;
    const size_t CrcNameCount = sizeof(CrcName) / sizeof(CrcName[0]);

    /* Variable */
    crc8_configuration_t configuration;
    uint8_t crc;
    void *memory;

    /* Calculate Partial */
    for(size_t i = 0; i < CrcNameCount; i++)
    {
        /* Set Up */
        memory = malloc(CRC8_LOOKUP_TABLE_MEMORY_SIZE);

        /* Mock */
        memory_malloc_ExpectAndReturn(CRC8_LOOKUP_TABLE_MEMORY_SIZE, memory);

        /* Initialize (Lookup Table) */
        crc8_init(CrcName[i], &configuration, true); // true (Lookup Table)

        /* Calculate */
        crc = configuration.initial;
        for(size_t j = 0; j < sizeof(test_CheckData); j++)
            crc = crc8_calculatePartial(&configuration, crc, test_CheckData[j], (j == (sizeof(test_CheckData) - 1)));

        /* Verify */
        TEST_ASSERT_EQUAL_HEX8(configuration.check, crc);

        /* Clean Up */
        free(memory);
    }
}

/*** Deinitialize ***/
void test_deinit_1(void)
{
    /*** Deinitialize (Configuration NULL) ***/
    /* Deinitialize */
    crc8_deinit(NULL);
}

void test_deinit_2(void)
{
    /*** Deinitialize (Loop) ***/
    /* Variable */
    crc8_configuration_t configuration;

    /* Mock */
    memory_free_Expect((void **)&configuration.lookupTable);

    /* Set Up */
    crc8_init(##### REPLACEMENT 3 #####, &configuration, false); // false (Loop)

    /* Initialize */
    crc8_deinit(&configuration);

    /* Verify */
    TEST_ASSERT_EQUAL_HEX8(0x00, configuration.check);
    TEST_ASSERT_EQUAL_HEX8(0x00, configuration.initial);
    TEST_ASSERT_NULL(configuration.lookupTable);
    TEST_ASSERT_EQUAL_HEX8(0x00, configuration.polynomial);
    TEST_ASSERT_EQUAL_INT(false, configuration.reflectIn);
    TEST_ASSERT_EQUAL_INT(false, configuration.reflectOut);
    TEST_ASSERT_EQUAL_INT(false, configuration.residue);
    TEST_ASSERT_EQUAL_HEX8(0x00, configuration.xorOut);
}

void test_deinit_3(void)
{
    /*** Deinitialize (Lookup Table) ***/
    /* Variable */
    crc8_configuration_t configuration;
    void *memory;

    /* Set Up */
    memory = malloc(CRC8_LOOKUP_TABLE_MEMORY_SIZE);

    /* Mock */
    memory_malloc_ExpectAndReturn(CRC8_LOOKUP_TABLE_MEMORY_SIZE, memory);
    memory_free_Expect((void **)&configuration.lookupTable);

    /* Set Up */
    crc8_init(##### REPLACEMENT 3 #####, &configuration, true); // true (Lookup Table)

    /* Initialize */
    crc8_deinit(&configuration);

    /* Verify */
    TEST_ASSERT_EQUAL_HEX8(0x00, configuration.check);
    TEST_ASSERT_EQUAL_HEX8(0x00, configuration.initial);
    TEST_ASSERT_NULL(configuration.lookupTable);
    TEST_ASSERT_EQUAL_HEX8(0x00, configuration.polynomial);
    TEST_ASSERT_EQUAL_INT(false, configuration.reflectIn);
    TEST_ASSERT_EQUAL_INT(false, configuration.reflectOut);
    TEST_ASSERT_EQUAL_INT(false, configuration.residue);
    TEST_ASSERT_EQUAL_HEX8(0x00, configuration.xorOut);

    /* Clean Up */
    free(memory);
}

/*** Initialize ***/
void test_init_1(void)
{
    /*** Initialize (Name NULL) ***/
    /* Variable */
    crc8_configuration_t configuration;

    /* Initialize */
    crc8_init(NULL, &configuration, false);
}

void test_init_2(void)
{
    /*** Initialize (Configuration NULL) ***/
    /* Initialize */
    crc8_init(##### REPLACEMENT 3 #####, NULL, false);
}

void test_init_3(void)
{
    /*** Initialize (Name Invalid) ***/
    /* Variable */
    crc8_configuration_t configuration;

    /* Invalid Error */
    crc8_init("CRC-8/INVALID", &configuration, false);
}

void test_init_4(void)
{
    /*** Initialize (Memory Allocation Failure) ***/
    /* Constant */
    const crc8_configuration_t ExpectedConfiguration = ##### REPLACEMENT 4 #####;

    /* Variable */
    crc8_configuration_t actualConfiguration;

    /* Mock */
    memory_malloc_ExpectAndReturn(CRC8_LOOKUP_TABLE_MEMORY_SIZE, NULL);

    /* Initialize */
    crc8_init(##### REPLACEMENT 3 #####, &actualConfiguration, true); // true (Lookup Table)

    /* Verify */
    TEST_ASSERT_EQUAL_UINT(ExpectedConfiguration.aliasCount, actualConfiguration.aliasCount);
    for(size_t j = 0; j < ExpectedConfiguration.aliasCount; j++)
        TEST_ASSERT_EQUAL_STRING(ExpectedConfiguration.alias[j], actualConfiguration.alias[j]);
    TEST_ASSERT_EQUAL_HEX8(ExpectedConfiguration.check, actualConfiguration.check);
    TEST_ASSERT_EQUAL_HEX8(ExpectedConfiguration.initial, actualConfiguration.initial);
    TEST_ASSERT_NULL(actualConfiguration.lookupTable);
    TEST_ASSERT_EQUAL_STRING(ExpectedConfiguration.name, actualConfiguration.name);
    TEST_ASSERT_EQUAL_HEX8(ExpectedConfiguration.polynomial, actualConfiguration.polynomial);
    TEST_ASSERT_EQUAL_INT(ExpectedConfiguration.reflectIn, actualConfiguration.reflectIn);
    TEST_ASSERT_EQUAL_INT(ExpectedConfiguration.reflectOut, actualConfiguration.reflectOut);
    TEST_ASSERT_EQUAL_HEX8(ExpectedConfiguration.residue, actualConfiguration.residue);
    TEST_ASSERT_EQUAL_HEX8(ExpectedConfiguration.xorOut, actualConfiguration.xorOut);
}

void test_init_5(void)
{
    /*** Initialize (Loop) ***/
    /* Structure */
    typedef struct testData_s
    {
        char *name;
        crc8_configuration_t expectedConfiguration;
    } testData_t;

    /* Test Data */
    const testData_t TestData[] =
    {
##### REPLACEMENT 5 #####
    };
    const size_t TestDataCount = sizeof(TestData) / sizeof(TestData[0]);

    /* Variable */
    crc8_configuration_t actualConfiguration;

    /*** Initialize ***/
    for(size_t i = 0; i < TestDataCount; i++)
    {
        /* Initialize */
        crc8_init(TestData[i].name, &actualConfiguration, false); // false (Loop)

        /* Verify */
        TEST_ASSERT_EQUAL_UINT(TestData[i].expectedConfiguration.aliasCount, actualConfiguration.aliasCount);
        for(size_t j = 0; j < TestData[i].expectedConfiguration.aliasCount; j++)
        	TEST_ASSERT_EQUAL_STRING(TestData[i].expectedConfiguration.alias[j], actualConfiguration.alias[j]);
        TEST_ASSERT_EQUAL_HEX8(TestData[i].expectedConfiguration.check, actualConfiguration.check);
        TEST_ASSERT_EQUAL_HEX8(TestData[i].expectedConfiguration.initial, actualConfiguration.initial);
        TEST_ASSERT_NULL(actualConfiguration.lookupTable);
        TEST_ASSERT_EQUAL_STRING(TestData[i].expectedConfiguration.name, actualConfiguration.name);
        TEST_ASSERT_EQUAL_HEX8(TestData[i].expectedConfiguration.polynomial, actualConfiguration.polynomial);
        TEST_ASSERT_EQUAL_INT(TestData[i].expectedConfiguration.reflectIn, actualConfiguration.reflectIn);
        TEST_ASSERT_EQUAL_INT(TestData[i].expectedConfiguration.reflectOut, actualConfiguration.reflectOut);
        TEST_ASSERT_EQUAL_HEX8(TestData[i].expectedConfiguration.residue, actualConfiguration.residue);
        TEST_ASSERT_EQUAL_HEX8(TestData[i].expectedConfiguration.xorOut, actualConfiguration.xorOut);
    }
}

void test_init_6(void)
{
    /*** Initialize (Lookup Table) ***/
    /* Structure */
    typedef struct testData_s
    {
        char *name;
        crc8_configuration_t expectedConfiguration;
    } testData_t;

    /* Test Data */
    const testData_t TestData[] =
    {
##### REPLACEMENT 6 #####
    };
    const size_t TestDataCount = sizeof(TestData) / sizeof(TestData[0]);

    /* Variable */
    crc8_configuration_t actualConfiguration;
    void *memory;

    /*** Initialize ***/
    for(size_t i = 0; i < TestDataCount; i++)
    {
        /* Set Up */
        memory = malloc(CRC8_LOOKUP_TABLE_MEMORY_SIZE);

        /* Mock */
    	memory_malloc_ExpectAndReturn(CRC8_LOOKUP_TABLE_MEMORY_SIZE, memory);

        /* Initialize */
        crc8_init(TestData[i].name, &actualConfiguration, true); // true (Lookup Table)

        /* Verify */
        TEST_ASSERT_EQUAL_UINT(TestData[i].expectedConfiguration.aliasCount, actualConfiguration.aliasCount);
        for(size_t j = 0; j < TestData[i].expectedConfiguration.aliasCount; j++)
        	TEST_ASSERT_EQUAL_STRING(TestData[i].expectedConfiguration.alias[j], actualConfiguration.alias[j]);
        TEST_ASSERT_EQUAL_HEX8(TestData[i].expectedConfiguration.check, actualConfiguration.check);
        TEST_ASSERT_EQUAL_HEX8(TestData[i].expectedConfiguration.initial, actualConfiguration.initial);
        TEST_ASSERT_NOT_NULL(actualConfiguration.lookupTable);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(TestData[i].expectedConfiguration.lookupTable, actualConfiguration.lookupTable, CRC8_LOOKUP_TABLE_COUNT);
        TEST_ASSERT_EQUAL_STRING(TestData[i].expectedConfiguration.name, actualConfiguration.name);
        TEST_ASSERT_EQUAL_HEX8(TestData[i].expectedConfiguration.polynomial, actualConfiguration.polynomial);
        TEST_ASSERT_EQUAL_INT(TestData[i].expectedConfiguration.reflectIn, actualConfiguration.reflectIn);
        TEST_ASSERT_EQUAL_INT(TestData[i].expectedConfiguration.reflectOut, actualConfiguration.reflectOut);
        TEST_ASSERT_EQUAL_HEX8(TestData[i].expectedConfiguration.residue, actualConfiguration.residue);
        TEST_ASSERT_EQUAL_HEX8(TestData[i].expectedConfiguration.xorOut, actualConfiguration.xorOut);

        /* Clean Up */
        free(memory);
    }
}

/*** Verify ***/
void test_verify_1(void)
{
    /*** Verify (Configuration NULL) ***/
    /* Variable */
    uint8_t buffer[128 + sizeof(uint8_t)];

    /* Verify */
    TEST_ASSERT_FALSE(crc8_verify(NULL, buffer, sizeof(buffer)));
}

void test_verify_2(void)
{
    /*** Verify (Buffer NULL) ***/
    /* Variable */
    crc8_configuration_t configuration;
    uint8_t buffer[128 + sizeof(uint8_t)];

    /* Verify */
    TEST_ASSERT_FALSE(crc8_verify(&configuration, NULL, sizeof(buffer)));
}

void test_verify_3(void)
{
    /*** Verify (Loop) ***/
    /* Constant */
    const char *CrcName[] = ##### REPLACEMENT 1 #####;
    const size_t CrcNameCount = sizeof(CrcName) / sizeof(CrcName[0]);

    /* Variable */
    crc8_configuration_t configuration;
    uint8_t buffer[128 + sizeof(uint8_t)];

    /* Calculate */
    for(size_t i = 0; i < CrcNameCount; i++)
    {
        /* Set Up */
        crc8_init(CrcName[i], &configuration, false); // false (Loop)
        random_setBuffer(buffer, sizeof(buffer) - sizeof(uint8_t));
        crc8_calculateAndAppend(&configuration, buffer, sizeof(buffer), sizeof(buffer) - sizeof(uint8_t));

        /* Verify */
        TEST_ASSERT_TRUE(crc8_verify(&configuration, buffer, sizeof(buffer)));
    }
}

void test_verify_4(void)
{
    /*** Verify (Lookup Table) ***/
    /* Constant */
    const char *CrcName[] = ##### REPLACEMENT 1 #####;
    const size_t CrcNameCount = sizeof(CrcName) / sizeof(CrcName[0]);

    /* Variable */
    crc8_configuration_t configuration;
    uint8_t buffer[128 + sizeof(uint8_t)];
    void *memory;

    /* Calculate */
    for(size_t i = 0; i < CrcNameCount; i++)
    {
        /* Set Up */
        memory = malloc(CRC8_LOOKUP_TABLE_MEMORY_SIZE);

        /* Mock */
        memory_malloc_ExpectAndReturn(CRC8_LOOKUP_TABLE_MEMORY_SIZE, memory);

        /* Set Up */
        crc8_init(CrcName[i], &configuration, true); // true (Lookup Table)
        random_setBuffer(buffer, sizeof(buffer) - sizeof(uint8_t));
        crc8_calculateAndAppend(&configuration, buffer, sizeof(buffer), sizeof(buffer) - sizeof(uint8_t));

        /* Verify */
        TEST_ASSERT_TRUE(crc8_verify(&configuration, buffer, sizeof(buffer)));

        /* Clean Up */
        free(memory);
    }
}
