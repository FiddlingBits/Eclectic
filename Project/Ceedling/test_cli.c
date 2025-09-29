/****************************************************************************************************
 * Include
 ****************************************************************************************************/

#include "cli.h"
#include "list.h"
#include "mock_memory.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "unity.h"

/****************************************************************************************************
 * Variable
 ****************************************************************************************************/

/*** CLI ***/
extern const cli_record_t *cli_ActiveDirectoryRecord;
extern cli_record_t cli_changeDirectoryCommandRecord;
extern char cli_inputBuffer[CLI_INPUT_MAXIMUM_LENGTH];
extern cli_record_t cli_listCommandRecord;
extern cli_printCallback_t cli_printCallback;
extern cli_processInputCallback_t cli_processInputCallback;
extern char cli_receiveBuffer[CLI_INPUT_MAXIMUM_LENGTH];
extern size_t cli_receiveBufferIndex;
extern cli_record_t cli_rootDirectoryRecord;

/*** Helper ***/
static size_t helper_expectedArgc;
static char *helper_expectedArgv[CLI_INPUT_MAXIMUM_ARGUMENTS];
static char helper_output[CLI_INPUT_MAXIMUM_LENGTH];
static size_t helper_outputIndex;
static bool helper_memoryMallocStubFail;
static bool helper_processInput;

/****************************************************************************************************
 * Function Prototype
 ****************************************************************************************************/

extern bool cli_getCommandAndArgumentList(char * const input, char **command, size_t * const argc, char *argv[]);
extern void cli_printCommandPrompt(void);
extern void cli_printAncestry(void);
extern void cli_processCommand(const char * const Command, const size_t Argc, char *argv[]);
extern bool cli_verifyNameAcceptable(const char * const Name, const cli_record_t * const ParentDirectory);

/****************************************************************************************************
 * Helper
 ****************************************************************************************************/

/*** Command Handler Callback ***/
void helper_commandHandlerCallback(size_t argc, char *argv[])
{
    /*** Command Handler Callback ***/
    /* Variable */
    size_t i;
    
    /* Verify */
    TEST_ASSERT_EQUAL_UINT32(helper_expectedArgc, argc);
    for(i = 0; i < helper_expectedArgc; i++)
        TEST_ASSERT_EQUAL_STRING(helper_expectedArgv[i], argv[i]);
}

/*** Memory Free Stub ***/
static void helper_memoryFreeStub(void **memory, int cmock_num_calls)
{
    /*** Memory Free Stub ***/
    /* Free */
    free(*memory);
    
    /* Clean Up */
    *memory = NULL;
}

/*** Memory Malloc Stub ***/
void *helper_memoryMallocStub(const size_t Size, int cmock_num_calls)
{
    /*** Memory Malloc Stub ***/
    /* Variable */
    void *memory;
    
    /* Malloc */
    if(helper_memoryMallocStubFail)
        memory = NULL;
    else
        memory = malloc(Size);
    
    /* Exit */
    return memory;
}

/*** Print Callback ***/
static void helper_printCallback(const char * const Format, ...)
{    
    /*** Print Callback ***/
    /* Variable */
    va_list arguments;

    /* Decode */
    va_start(arguments, Format);
    helper_outputIndex += (size_t)vsnprintf(&helper_output[helper_outputIndex], sizeof(helper_output) - helper_outputIndex, Format, arguments);
    va_end(arguments);
}

/*** Process Input Callback ***/
static void helper_processInputCallback(void)
{
    /*** Process Callback ***/
    helper_processInput = true;
}

/****************************************************************************************************
 * Set Up/Tear Down
 ****************************************************************************************************/

void setUp(void)
{
    /*** Set Up ***/  
    /* Variable */
    size_t i;
    
    /* Function */
    memory_free_Stub(helper_memoryFreeStub);
    memory_malloc_Stub(helper_memoryMallocStub);
    helper_memoryMallocStubFail = false;
    (void)cli_init(helper_printCallback, helper_processInputCallback);
    
    /* Set Up */
    helper_expectedArgc = 0;
    for(i = 0; i < CLI_INPUT_MAXIMUM_ARGUMENTS; i++)
        helper_expectedArgv[i] = NULL;
    (void)memset(helper_output, 0, sizeof(helper_output)); // Must Come After cli_init, So Print Buffer Doesn't Have Command Prompt In It
    helper_outputIndex = 0;
    helper_processInput = false;
}

void tearDown(void)
{
}

/****************************************************************************************************
 * Test (Public)
 ****************************************************************************************************/

/*** Add Receive Character ***/
void test_addReceiveCharacter_1(void)
{
    /*** Add Receive Character (Not Initialized) ***/
    /* Set Up */
    cli_deinit(); // Deinitialize Because CLI Is Initialized In Set Up
    
    /* Add Receive Character */
    TEST_ASSERT_FALSE(cli_addReceiveCharacter('h'));
}

void test_addReceiveCharacter_2(void)
{
    /*** Add Receive Character ***/
    /* Structure */
    typedef struct
    {
        const char *Input;
        const char *InputBuffer;
        bool processInput;
        const char *ReceiveBuffer;
    } testData_t;
    
    /* Test Data */
    const testData_t TestData[] =
    {
        {"h", "", false, "h"},
        {"help", "", false, "help"},
        {"help\n", "help\n", true, ""},
        {"\b\b\b", "", false, ""},
        {"hee\blp\n", "help\n", true, ""},
        {"hee\bll\bppp\b\b\n", "help\n", true, ""},
        {
            "helphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelp",
            "helphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelphelp",
            true,
            ""
        }
    };
    const size_t TestDataCount = sizeof(TestData) / sizeof(TestData[0]);
    
    /* Variable */
    size_t i, j;
    
    /* Test */
    for(i = 0; i < TestDataCount; i++)
    {       
        /* Set Up */
        (void)memset(&cli_inputBuffer, 0, sizeof(cli_inputBuffer));
        (void)memset(&cli_receiveBuffer, 0, sizeof(cli_receiveBuffer));
        cli_receiveBufferIndex = 0;
        helper_processInput = false;
        
        /* Add Receive Character */
        for(j = 0; j < strlen(TestData[i].Input); j++)
            TEST_ASSERT_TRUE(cli_addReceiveCharacter(TestData[i].Input[j]));
        
        /* Verify */
        TEST_ASSERT_EQUAL_STRING(TestData[i].InputBuffer, cli_inputBuffer);
        TEST_ASSERT_EQUAL_STRING(TestData[i].ReceiveBuffer, cli_receiveBuffer);
        if(TestData[i].processInput)
            TEST_ASSERT_EQUAL_UINT32(0, cli_receiveBufferIndex);
        else
            TEST_ASSERT_EQUAL_UINT32(strlen(cli_receiveBuffer), cli_receiveBufferIndex);
        TEST_ASSERT_EQUAL(TestData[i].processInput, helper_processInput);
    }
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

/*** Deinitialize ***/
void test_deinit_1(void)
{
    /*** Deinitialize ***/
    /* Deinitialize */
    cli_deinit();
    
    /* Verify */
    TEST_ASSERT_NULL(cli_ActiveDirectoryRecord);
    TEST_ASSERT_NULL(cli_printCallback);
    TEST_ASSERT_NULL(cli_processInputCallback);
    TEST_ASSERT_EQUAL_UINT32(0, cli_rootDirectoryRecord.entries.size);
}

/*** Get Argument/Option Pair ***/
void test_getArgumentOptionPair_1(void)
{
    /*** Get Argument/Option Pair (NULL Pointer) ***/
    /* Variable */
    cli_argumentOptionPair_t argumentOptionPair;
    
    /* Get Argument/Option Pair */
    TEST_ASSERT_FALSE(cli_getArgumentOptionPair(NULL, &argumentOptionPair));
    TEST_ASSERT_FALSE(cli_getArgumentOptionPair("-i", NULL));
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

void test_getArgumentOptionPair_2(void)
{
    /*** Get Argument/Option Pair (Invalid Input) ***/
    /* Structure */
    typedef struct
    {
        cli_argumentOptionPair_t argumentOptionPair;
        const char *Input;
    } testData_t;
    
    /* Test Data */
    const testData_t TestData[] =
    {
        {{NULL, NULL}, ""},
        {{NULL, NULL}, "-"},
        {{NULL, NULL}, "--"},
        {{NULL, NULL}, "i"},
        {{NULL, NULL}, "i-"},
        {{NULL, NULL}, "info"}
    };
    const size_t TestDataCount = sizeof(TestData) / sizeof(TestData[0]);
    
    /* Variable */
    cli_argumentOptionPair_t argumentOptionPair;
    size_t i;
    char input[100];
    
    /* Test */
    for(i = 0; i < TestDataCount; i++)
    {        
        /* Set Up */
        (void)strcpy(input, TestData[i].Input);
        
        /* Get Argument/Option Pair */
        TEST_ASSERT_FALSE(cli_getArgumentOptionPair(input, &argumentOptionPair));
    }
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

void test_getArgumentOptionPair_3(void)
{
    /*** Get Argument/Option Pair ***/
    /* Structure */
    typedef struct
    {
        cli_argumentOptionPair_t argumentOptionPair;
        const char *Input;
    } testData_t;
    
    /* Test Data */
    const testData_t TestData[] =
    {
        {{NULL, "i"}, "-i"},
        {{"5", "i"}, "-i5"},
        {{"verbose", "i"}, "-iverbose"},
        {{NULL, "info"}, "--info"},
        {{"5", "info"}, "--info=5"},
        {{"verbose", "info"}, "--info=verbose"}
    };
    const size_t TestDataCount = sizeof(TestData) / sizeof(TestData[0]);
    
    /* Variable */
    cli_argumentOptionPair_t argumentOptionPair;
    size_t i;
    char input[100];
    
    /* Test */
    for(i = 0; i < TestDataCount; i++)
    {        
        /* Set Up */
        (void)strcpy(input, TestData[i].Input);
        
        /* Get Argument/Option Pair */
        TEST_ASSERT_TRUE(cli_getArgumentOptionPair(input, &argumentOptionPair));
        
        /* Verify */
        TEST_ASSERT_EQUAL_STRING(TestData[i].argumentOptionPair.argument, argumentOptionPair.argument);
        TEST_ASSERT_EQUAL_STRING(TestData[i].argumentOptionPair.option, argumentOptionPair.option);
    }
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

/*** Get Signed 32-Bit Integer ***/
void test_getS32_1(void)
{
    /*** Get Signed 32-Bit Integer (NULL Pointer) ***/
    /* Variable */
    int32_t s32;
    
    /* Get Argument/Option Pair */
    TEST_ASSERT_FALSE(cli_getS32(NULL, &s32));
    TEST_ASSERT_FALSE(cli_getS32("123", NULL));
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

void test_getS32_2(void)
{
    /*** Get Signed 32-Bit Integer (Length Error) ***/
    /* Variable */
    int32_t s32;
    
    /* Get Argument/Option Pair */
    TEST_ASSERT_FALSE(cli_getS32("", &s32));
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

void test_getS32_3(void)
{
    /*** Get Signed 32-Bit Integer (Invalid Input) ***/
    /* Variable */
    int32_t s32;
    
    /* Get Argument/Option Pair */
    TEST_ASSERT_FALSE(cli_getS32("123xyz", &s32));
    TEST_ASSERT_FALSE(cli_getS32("xyz", &s32));
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

void test_getS32_4(void)
{
    /*** Get Signed 32-Bit Integer ***/
    /* Structure */
    typedef struct
    {
        const char *Input;
        int32_t s32;
    } testData_t;
    
    /* Test Data */
    const testData_t TestData[] =
    {
        {"-2147483648", INT32_MIN},
        {"-2147483647", INT32_MIN + 1},
        {"-1", -1},
        {"0", 0},
        {"1", 1},
        {"2147483646", INT32_MAX - 1},
        {"2147483647", INT32_MAX}
    };
    const size_t TestDataCount = sizeof(TestData) / sizeof(TestData[0]);
    
    /* Variable */
    size_t i;
    int32_t s32;
    
    /* Test */
    for(i = 0; i < TestDataCount; i++)
    {                
        /* Get Signed 32-Bit Integer */
        TEST_ASSERT_TRUE(cli_getS32(TestData[i].Input, &s32));
        
        /* Verify */
        TEST_ASSERT_EQUAL_INT32(TestData[i].s32, s32);
    }
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

/*** Get Unsigned 32-Bit Integer ***/
void test_getU32_1(void)
{
    /*** Get Unsigned 32-Bit Integer (NULL Pointer) ***/
    /* Variable */
    uint32_t u32;
    
    /* Get Argument/Option Pair */
    TEST_ASSERT_FALSE(cli_getU32(NULL, &u32));
    TEST_ASSERT_FALSE(cli_getU32("123", NULL));
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

void test_getU32_2(void)
{
    /*** Get Unsigned 32-Bit Integer (Length Error) ***/
    /* Variable */
    uint32_t u32;
    
    /* Get Argument/Option Pair */
    TEST_ASSERT_FALSE(cli_getU32("", &u32));
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

void test_getU32_3(void)
{
    /*** Get Unsigned 32-Bit Integer (Invalid Input) ***/
    /* Variable */
    uint32_t u32;
    
    /* Get Argument/Option Pair */
    TEST_ASSERT_FALSE(cli_getU32("123xyz", &u32));
    TEST_ASSERT_FALSE(cli_getU32("xyz", &u32));
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

void test_getU32_4(void)
{
    /*** Get Unsigned 32-Bit Integer ***/
    /* Structure */
    typedef struct
    {
        const char *Input;
        uint32_t u32;
    } testData_t;
    
    /* Test Data */
    const testData_t TestData[] =
    {
        {"0", 0},
        {"1", 1},
        {"4294967294", UINT32_MAX - 1},
        {"4294967295", UINT32_MAX}
    };
    const size_t TestDataCount = sizeof(TestData) / sizeof(TestData[0]);
    
    /* Variable */
    size_t i;
    uint32_t u32;
    
    /* Test */
    for(i = 0; i < TestDataCount; i++)
    {                
        /* Get Unsigned 32-Bit Integer */
        TEST_ASSERT_TRUE(cli_getU32(TestData[i].Input, &u32));
        
        /* Verify */
        TEST_ASSERT_EQUAL_UINT32(TestData[i].u32, u32);
    }
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

/*** Initialize ***/
void test_init_1(void)
{
    /*** Initialize (NULL Pointer) ***/    
    TEST_ASSERT_FALSE(cli_init(helper_printCallback, NULL));
    TEST_ASSERT_FALSE(cli_init(NULL, helper_processInputCallback));
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

void test_init_2(void)
{
    /*** Initialize ***/
    /* Variable */
    char expectedCommandPrompt[100];
    cli_record_t key;
    
    /* Set Up */
    cli_deinit(); // Deinitialize Because CLI Is Initialized In Set Up
    (void)sprintf(expectedCommandPrompt, "[%s/]%s ", CLI_DIRECTORY_ROOT_NAME, CLI_OUTPUT_COMMAND_PROMPT);

    /* Initialize */
    TEST_ASSERT_TRUE(cli_init(helper_printCallback, helper_processInputCallback));
    
    /* Verify */
    TEST_ASSERT_EQUAL_PTR(&cli_rootDirectoryRecord, cli_ActiveDirectoryRecord);
    TEST_ASSERT_EQUAL_PTR(helper_printCallback, cli_printCallback);
    TEST_ASSERT_EQUAL_PTR(helper_processInputCallback, cli_processInputCallback);
    TEST_ASSERT_EQUAL_UINT32(2, cli_rootDirectoryRecord.entries.size); // Built-In Commands
    key.Name = CLI_COMMAND_CHANGE_DIRECTORY_NAME; // Compare Callback Expects Name Within Record
    TEST_ASSERT_TRUE(list_find(&(cli_rootDirectoryRecord.entries), &key) != LIST_FIND_NOT_FOUND_INDEX);
    key.Name = CLI_COMMAND_LIST_NAME;
    TEST_ASSERT_TRUE(list_find(&(cli_rootDirectoryRecord.entries), &key) != LIST_FIND_NOT_FOUND_INDEX);
    TEST_ASSERT_EQUAL_UINT32(0, cli_receiveBufferIndex);
    TEST_ASSERT_EQUAL_STRING(expectedCommandPrompt, helper_output);
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

/*** Process Input ***/
void test_processInput_1(void)
{
    /*** Process Input (Not Initialized) ***/
    /* Set Up */
    cli_printCallback = NULL;

    /* Process Input */
    TEST_ASSERT_FALSE(cli_processInput());
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

void test_processInput_2(void)
{
    /*** Process Input (No Command) ***/
    /* Variable */
    char expectedCommandPrompt[100];
    
    /* Set Up */
    (void)sprintf(expectedCommandPrompt, "[%s/]%s ", CLI_DIRECTORY_ROOT_NAME, CLI_OUTPUT_COMMAND_PROMPT);
    (void)strcpy(cli_inputBuffer, "\n");
    
    /* Process Input */
    TEST_ASSERT_FALSE(cli_processInput());
    
    /* Verify */
    TEST_ASSERT_EQUAL_STRING(expectedCommandPrompt, helper_output);
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

void test_processInput_3(void)
{
    /*** Process Input ***/
    /* Constant */
    const char * const CommandName = "help";
    
    /* Variable */
    cli_record_t commandRecord;
    char expectedCommandPrompt[100];
    
    /* Set Up */
    helper_expectedArgc = 1;
    helper_expectedArgv[0] = "please";
    (void)sprintf(expectedCommandPrompt, "[%s/]%s ", CLI_DIRECTORY_ROOT_NAME, CLI_OUTPUT_COMMAND_PROMPT);
    (void)strcpy(cli_inputBuffer, "help please\n");
    TEST_ASSERT_TRUE(cli_registerCommand(&commandRecord, CommandName, CLI_PARENT_DIRECTORY_ROOT, helper_commandHandlerCallback));
    
    /* Process Input */
    TEST_ASSERT_TRUE(cli_processInput());
    
    /* Verify */
    TEST_ASSERT_EQUAL_STRING(expectedCommandPrompt, helper_output);
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

/*** Register Command ***/
void test_registerCommand_1(void)
{
    /*** Register Command (NULL Pointer) ***/
    /* Variable */
    cli_record_t commandRecord;
    
    /* Register Command */
    TEST_ASSERT_FALSE(cli_registerCommand(NULL, "help", CLI_PARENT_DIRECTORY_ROOT, helper_commandHandlerCallback));
    TEST_ASSERT_FALSE(cli_registerCommand(&commandRecord, NULL, CLI_PARENT_DIRECTORY_ROOT, helper_commandHandlerCallback));
    TEST_ASSERT_FALSE(cli_registerCommand(&commandRecord, "help", CLI_PARENT_DIRECTORY_ROOT, NULL));
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

void test_registerCommand_2(void)
{
    /*** Register Command (Invalid Name) ***/
    /* Variable */
    cli_record_t commandRecord;
    
    /* Register Command */
    TEST_ASSERT_FALSE(cli_registerCommand(&commandRecord, "this and that", CLI_PARENT_DIRECTORY_ROOT, helper_commandHandlerCallback));
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

void test_registerCommand_3(void)
{
    /*** Register Command (Parent Directory Not A Directory) ***/
    /* Variable */
    cli_record_t commandRecord[2];
    
    /* Set Up */
    TEST_ASSERT_TRUE(cli_registerCommand(&commandRecord[0], "this", CLI_PARENT_DIRECTORY_ROOT, helper_commandHandlerCallback));
    
    /* Register Command */
    TEST_ASSERT_FALSE(cli_registerCommand(&commandRecord[1], "that", &commandRecord[0], helper_commandHandlerCallback));
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

void test_registerCommand_4(void)
{
    /*** Register Command (Root Parent Directory) ***/
    /* Constant */
    const char * const CommandName = "help";
    
    /* Variable */
    cli_record_t commandRecord, key;
    
    /* Register Command */
    TEST_ASSERT_TRUE(cli_registerCommand(&commandRecord, CommandName, CLI_PARENT_DIRECTORY_ROOT, helper_commandHandlerCallback));
    
    /* Verify */
    TEST_ASSERT_EQUAL_INT(CLI_RECORD_TYPE_COMMAND, commandRecord.type);
    key.Name = CommandName; // Compare Callback Expects Name Within Record
    TEST_ASSERT_TRUE(list_find(&(cli_rootDirectoryRecord.entries), &key) != LIST_FIND_NOT_FOUND_INDEX);
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

void test_registerCommand_5(void)
{
    /*** Register Command (Custom Parent Directory) ***/
    /* Constant */
    const char * const CommandName = "help";
    const char * const DirectoryName = "folder";
    
    /* Variable */
    cli_record_t commandRecord, directoryRecord, key;
    
    /* Set Up */
    TEST_ASSERT_TRUE(cli_registerDirectory(&directoryRecord, DirectoryName, CLI_PARENT_DIRECTORY_ROOT));
    
    /* Register Command */
    TEST_ASSERT_TRUE(cli_registerCommand(&commandRecord, CommandName, &directoryRecord, helper_commandHandlerCallback));
    
    /* Verify */
    key.Name = DirectoryName; // Compare Callback Expects Name Within Record
    TEST_ASSERT_TRUE(list_find(&(cli_rootDirectoryRecord.entries), &key) != LIST_FIND_NOT_FOUND_INDEX);
    TEST_ASSERT_EQUAL_INT(CLI_RECORD_TYPE_COMMAND, commandRecord.type);
    key.Name = CommandName;
    TEST_ASSERT_TRUE(list_find(&(directoryRecord.entries), &key) != LIST_FIND_NOT_FOUND_INDEX);
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

/*** Register Directory ***/
void test_registerDirectory_1(void)
{
    /*** Register Directory (NULL Pointer) ***/
    /* Variable */
    cli_record_t directoryRecord;
    
    /* Register Command */
    TEST_ASSERT_FALSE(cli_registerDirectory(NULL, "folder", CLI_PARENT_DIRECTORY_ROOT));
    TEST_ASSERT_FALSE(cli_registerDirectory(&directoryRecord, NULL, CLI_PARENT_DIRECTORY_ROOT));
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

void test_registerDirectory_2(void)
{
    /*** Register Directory (Invalid Name) ***/
    /* Variable */
    cli_record_t directoryRecord;
    
    /* Register Directory */
    TEST_ASSERT_FALSE(cli_registerDirectory(&directoryRecord, "this and that", CLI_PARENT_DIRECTORY_ROOT));
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

void test_registerDirectory_3(void)
{
    /*** Register Directory (Parent Directory Not A Directory) ***/
    /* Variable */
    cli_record_t commandRecord, directoryRecord;
    
    /* Set Up */
    TEST_ASSERT_TRUE(cli_registerCommand(&commandRecord, "this", CLI_PARENT_DIRECTORY_ROOT, helper_commandHandlerCallback));
    
    /* Register Directory */
    TEST_ASSERT_FALSE(cli_registerDirectory(&directoryRecord, "that", &commandRecord));
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

void test_registerDirectory_4(void)
{
    /*** Register Directory (Root Parent Directory) ***/
    /* Constant */
    const char * const DirectoryName = "help";
    
    /* Variable */
    cli_record_t directoryRecord, key;
    
    /* Register Directory */
    TEST_ASSERT_TRUE(cli_registerDirectory(&directoryRecord, DirectoryName, CLI_PARENT_DIRECTORY_ROOT));
    
    /* Verify */
    key.Name = DirectoryName; // Compare Callback Expects Name Within Record
    TEST_ASSERT_TRUE(list_find(&(cli_rootDirectoryRecord.entries), &key) != LIST_FIND_NOT_FOUND_INDEX);
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

void test_registerDirectory_5(void)
{
    /*** Register Directory (Other Parent Directory) ***/
    /* Constant */
    const char * const DirectoryName = "help";
    const char * const ParentName = "folder";
    
    /* Variable */
    cli_record_t directoryRecord[2], key;
    
    /* Set Up */
    TEST_ASSERT_TRUE(cli_registerDirectory(&directoryRecord[0], ParentName, CLI_PARENT_DIRECTORY_ROOT));
    
    /* Register Directory */
    TEST_ASSERT_TRUE(cli_registerDirectory(&directoryRecord[1], DirectoryName, &directoryRecord[0]));
    
    /* Verify */
    key.Name = ParentName; // Compare Callback Expects Name Within Record
    TEST_ASSERT_TRUE(list_find(&(cli_rootDirectoryRecord.entries), &key) != LIST_FIND_NOT_FOUND_INDEX);
    key.Name = DirectoryName;
    TEST_ASSERT_TRUE(list_find(&(directoryRecord[0].entries), &key) != LIST_FIND_NOT_FOUND_INDEX);
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

/****************************************************************************************************
 * Test (Private)
 ****************************************************************************************************/

/*** Change Directory Command Handler ***/
void test_changeDirectoryCommandHandlerCallback_1(void)
{
    /*** Change Directory Command Handler (Help, Short Option) ***/
    /* Variable */
    char expectedOutput[CLI_INPUT_MAXIMUM_LENGTH];
    int index;
    
    /* Set Up */
    index = 0;
    index += sprintf(&expectedOutput[index], "usage: %s [DIRECTORY|OPTION], Change Working Directory\n", CLI_COMMAND_CHANGE_DIRECTORY_NAME);
    index += sprintf(&expectedOutput[index], "  -h, --help, Display Help\n");
    index += sprintf(&expectedOutput[index], "[%s/]%s ", CLI_DIRECTORY_ROOT_NAME, CLI_OUTPUT_COMMAND_PROMPT);
    (void)sprintf(cli_inputBuffer, "%s -h\n", CLI_COMMAND_CHANGE_DIRECTORY_NAME);
    
    /* Process Input */
    TEST_ASSERT_TRUE(cli_processInput());
    
    /* Verify */
    TEST_ASSERT_EQUAL_STRING(expectedOutput, helper_output);
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

void test_changeDirectoryCommandHandlerCallback_2(void)
{
    /*** Change Directory Command Handler (Help, Long Option) ***/
    /* Variable */
    char expectedOutput[CLI_INPUT_MAXIMUM_LENGTH];
    int index;
    
    /* Set Up */
    index = 0;
    index += sprintf(&expectedOutput[index], "usage: %s [DIRECTORY|OPTION], Change Working Directory\n", CLI_COMMAND_CHANGE_DIRECTORY_NAME);
    index += sprintf(&expectedOutput[index], "  -h, --help, Display Help\n");
    index += sprintf(&expectedOutput[index], "[%s/]%s ", CLI_DIRECTORY_ROOT_NAME, CLI_OUTPUT_COMMAND_PROMPT);
    (void)sprintf(cli_inputBuffer, "%s --help\n", CLI_COMMAND_CHANGE_DIRECTORY_NAME);
    
    /* Process Input */
    TEST_ASSERT_TRUE(cli_processInput());
    
    /* Verify */
    TEST_ASSERT_EQUAL_STRING(expectedOutput, helper_output);
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

void test_changeDirectoryCommandHandlerCallback_3(void)
{
    /*** Change Directory Command Handler (Help, Invalid Arguments) ***/
    /* Constant */
    const char *Input[] = {"-i", "-hplease", "--help=please", "--helpme", "this that", "--this", "--this --that"};
    const size_t InputSize = sizeof(Input) / sizeof(Input[0]);
    
    /* Variable */
    char expectedOutput[CLI_INPUT_MAXIMUM_LENGTH];
    size_t i;
    int index;
    
    /* Set Up */
    index = 0;
    index += sprintf(&expectedOutput[index], "usage: %s [DIRECTORY|OPTION], Change Working Directory\n", CLI_COMMAND_CHANGE_DIRECTORY_NAME);
    index += sprintf(&expectedOutput[index], "  -h, --help, Display Help\n");
    index += sprintf(&expectedOutput[index], "[%s/]%s ", CLI_DIRECTORY_ROOT_NAME, CLI_OUTPUT_COMMAND_PROMPT);
    
    /* Test */
    for(i = 0; i < InputSize; i++)
    {
        /* Set Up */
        (void)sprintf(cli_inputBuffer, "%s %s\n", CLI_COMMAND_CHANGE_DIRECTORY_NAME, Input[i]);
        (void)memset(helper_output, 0, sizeof(helper_output));
        helper_outputIndex = 0;
        
        /* Process Input */
        TEST_ASSERT_TRUE(cli_processInput());
        
        /* Verify */
        TEST_ASSERT_EQUAL_STRING(expectedOutput, helper_output);
    }
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

void test_changeDirectoryCommandHandlerCallback_4(void)
{
    /*** Change Directory Command Handler (Change Directory, Root) ***/
    /* Constant */
    const char *DirectoryName[CLI_OUTPUT_ANCESTRY_MAXIMUM_DEPTH - 1] = {"one", "two", "three", "four"};
    
    /* Variable */
    cli_record_t directoryRecord[CLI_OUTPUT_ANCESTRY_MAXIMUM_DEPTH - 1];
    char expectedOutput[CLI_INPUT_MAXIMUM_LENGTH];
    size_t i;
    
    /* Set Up */
    cli_ActiveDirectoryRecord = &directoryRecord[CLI_OUTPUT_ANCESTRY_MAXIMUM_DEPTH - 2];
    for(i = 0; i < (CLI_OUTPUT_ANCESTRY_MAXIMUM_DEPTH - 1); i++)
    {
        if(i == 0)
            TEST_ASSERT_TRUE(cli_registerDirectory(&directoryRecord[i], DirectoryName[i], CLI_PARENT_DIRECTORY_ROOT));
        else
            TEST_ASSERT_TRUE(cli_registerDirectory(&directoryRecord[i], DirectoryName[i], &directoryRecord[i - 1]));
    }
    (void)sprintf(expectedOutput, "[%s/]%s ", CLI_DIRECTORY_ROOT_NAME, CLI_OUTPUT_COMMAND_PROMPT);
    (void)sprintf(cli_inputBuffer, "%s\n", CLI_COMMAND_CHANGE_DIRECTORY_NAME);
    
    /* Process Input */
    TEST_ASSERT_TRUE(cli_processInput());
    
    /* Verify */
    TEST_ASSERT_EQUAL_PTR(&cli_rootDirectoryRecord, cli_ActiveDirectoryRecord);
    TEST_ASSERT_EQUAL_STRING(expectedOutput, helper_output);
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

void test_changeDirectoryCommandHandlerCallback_5(void)
{
    /*** Change Directory Command Handler (Change Directory, Parent) ***/
    /* Constant */
    const char *DirectoryName = "one";
    
    /* Variable */
    cli_record_t directoryRecord;
    char expectedOutput[CLI_INPUT_MAXIMUM_LENGTH];
    
    /* Set Up */
    cli_ActiveDirectoryRecord = &directoryRecord;
    TEST_ASSERT_TRUE(cli_registerDirectory(&directoryRecord, DirectoryName, CLI_PARENT_DIRECTORY_ROOT));
    (void)sprintf(expectedOutput, "[%s/]%s ", CLI_DIRECTORY_ROOT_NAME, CLI_OUTPUT_COMMAND_PROMPT);
    (void)sprintf(cli_inputBuffer, "%s ..\n", CLI_COMMAND_CHANGE_DIRECTORY_NAME);
    
    /* Process Input */
    TEST_ASSERT_TRUE(cli_processInput());
    
    /* Verify */
    TEST_ASSERT_EQUAL_PTR(&cli_rootDirectoryRecord, cli_ActiveDirectoryRecord);
    TEST_ASSERT_EQUAL_STRING(expectedOutput, helper_output);
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

void test_changeDirectoryCommandHandlerCallback_6(void)
{
    /*** Change Directory Command Handler (Change Directory, No Parent) ***/    
    /* Variable */
    char expectedOutput[CLI_INPUT_MAXIMUM_LENGTH];
    
    /* Set Up */
    (void)sprintf(expectedOutput, "[%s/]%s ", CLI_DIRECTORY_ROOT_NAME, CLI_OUTPUT_COMMAND_PROMPT);
    (void)sprintf(cli_inputBuffer, "%s ..\n", CLI_COMMAND_CHANGE_DIRECTORY_NAME);
    
    /* Process Input */
    TEST_ASSERT_TRUE(cli_processInput());
    
    /* Verify */
    TEST_ASSERT_EQUAL_PTR(&cli_rootDirectoryRecord, cli_ActiveDirectoryRecord);
    TEST_ASSERT_EQUAL_STRING(expectedOutput, helper_output);
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

void test_changeDirectoryCommandHandlerCallback_7(void)
{
    /*** Change Directory Command Handler (Change Directory, Named) ***/
    /* Constant */
    const char *DirectoryName = "one";
    
    /* Variable */
    cli_record_t directoryRecord;
    char expectedOutput[CLI_INPUT_MAXIMUM_LENGTH];
    
    /* Set Up */
    TEST_ASSERT_TRUE(cli_registerDirectory(&directoryRecord, DirectoryName, CLI_PARENT_DIRECTORY_ROOT));
    (void)sprintf(expectedOutput, "[%s/%s/]%s ", CLI_DIRECTORY_ROOT_NAME, DirectoryName, CLI_OUTPUT_COMMAND_PROMPT);
    (void)sprintf(cli_inputBuffer, "%s %s\n", CLI_COMMAND_CHANGE_DIRECTORY_NAME, DirectoryName);
    
    /* Process Input */
    TEST_ASSERT_TRUE(cli_processInput());
    
    /* Verify */
    TEST_ASSERT_EQUAL_PTR(&directoryRecord, cli_ActiveDirectoryRecord);
    TEST_ASSERT_EQUAL_STRING(expectedOutput, helper_output);
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

void test_changeDirectoryCommandHandlerCallback_8(void)
{
    /*** Change Directory Command Handler (Change Directory, Named, Not Found) ***/
    /* Constant */
    const char *DirectoryName = "bogus";
    
    /* Variable */
    char expectedOutput[CLI_INPUT_MAXIMUM_LENGTH];
    
    /* Set Up */
    (void)sprintf(expectedOutput, "Directory Not Found\n[%s/]%s ", CLI_DIRECTORY_ROOT_NAME, CLI_OUTPUT_COMMAND_PROMPT);
    (void)sprintf(cli_inputBuffer, "%s %s\n", CLI_COMMAND_CHANGE_DIRECTORY_NAME, DirectoryName);
    
    /* Process Input */
    TEST_ASSERT_TRUE(cli_processInput());
    
    /* Verify */
    TEST_ASSERT_EQUAL_PTR(&cli_rootDirectoryRecord, cli_ActiveDirectoryRecord);
    TEST_ASSERT_EQUAL_STRING(expectedOutput, helper_output);
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

void test_changeDirectoryCommandHandlerCallback_9(void)
{
    /*** Change Directory Command Handler (Change Directory, Named, Not A Directory) ***/
    /* Constant */
    const char *CommandName = "help";
    
    /* Variable */
    cli_record_t commandRecord;
    char expectedOutput[CLI_INPUT_MAXIMUM_LENGTH];
    
    /* Set Up */
    TEST_ASSERT_TRUE(cli_registerCommand(&commandRecord, CommandName, CLI_PARENT_DIRECTORY_ROOT, helper_commandHandlerCallback));
    (void)sprintf(expectedOutput, "Not A Directory\n[%s/]%s ", CLI_DIRECTORY_ROOT_NAME, CLI_OUTPUT_COMMAND_PROMPT);
    (void)sprintf(cli_inputBuffer, "%s %s\n", CLI_COMMAND_CHANGE_DIRECTORY_NAME, CommandName);
    
    /* Process Input */
    TEST_ASSERT_TRUE(cli_processInput());
    
    /* Verify */
    TEST_ASSERT_EQUAL_PTR(&cli_rootDirectoryRecord, cli_ActiveDirectoryRecord);
    TEST_ASSERT_EQUAL_STRING(expectedOutput, helper_output);
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

/*** Get Command And Argument List ***/
void test_getCommandAndArgumentList_1(void)
{
    /*** Get Command And Argument List (Zero Length String) ***/
    /* Variable */
    size_t argc;
    char *argv[CLI_INPUT_MAXIMUM_ARGUMENTS], *command, input[CLI_INPUT_MAXIMUM_LENGTH];
    
    /* Set Up */
    (void)strcpy(input, "");
    
    /* Get Command And Argument List */
    TEST_ASSERT_FALSE(cli_getCommandAndArgumentList(input, &command, &argc, argv));
    
    /* Verify */
    TEST_ASSERT_EQUAL_UINT32(0, argc);
    TEST_ASSERT_NULL(command);
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

void test_getCommandAndArgumentList_2(void)
{
    /*** Get Command And Argument List ***/
    /* Structure */
    typedef struct
    {
        const size_t Argc;
        const char *Argv[CLI_INPUT_MAXIMUM_ARGUMENTS];
        const char *Command;
        const char *Input;
    } testData_t;
    
    /* Test Data */
    const testData_t TestData[] =
    {
        {0, {NULL}, "help", "help\n"},
        {1, {"one", NULL}, "help", "help one\n"},
        {9, {"one", "two", "three", "four", "five", "six", "seven", "eight", "nine", NULL}, "help", "help one two three four five six seven eight nine\n"},
        {10, {"one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten"}, "help", "help one two three four five six seven eight nine ten\n"},
        {10, {"one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten"}, "help", "help one two three four five six seven eight nine ten eleven\n"}
    };
    const size_t TestDataCount = sizeof(TestData) / sizeof(TestData[0]);
    
    /* Variable */
    size_t argc, i, j;
    char *argv[CLI_INPUT_MAXIMUM_ARGUMENTS], *command, input[CLI_INPUT_MAXIMUM_LENGTH];
    
    /* Test */
    for(i = 0; i < TestDataCount; i++)
    {
        /* Set Up */
        (void)memset(input, 0, sizeof(input));
        (void)strcpy(input, TestData[i].Input);
        
        /* Get Command And Argument List */
        TEST_ASSERT_TRUE(cli_getCommandAndArgumentList(input, &command, &argc, argv));
        
        /* Verify */
        TEST_ASSERT_EQUAL_UINT32(TestData[i].Argc, argc);
        for(j = 0; j < TestData[i].Argc; j++)
            TEST_ASSERT_EQUAL_STRING(TestData[i].Argv[j], argv[j]);
        TEST_ASSERT_EQUAL_STRING(TestData[i].Command, command);
    }
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

/*** List Command Handler ***/
void test_listCommandHandlerCallback_1(void)
{
    /*** List Command Handler (Help, Short Option) ***/
    /* Variable */
    char expectedOutput[CLI_INPUT_MAXIMUM_LENGTH];
    int index;
    
    /* Set Up */
    index = 0;
    index += sprintf(&expectedOutput[index], "usage: %s [OPTION], List Directory Entries\n", CLI_COMMAND_LIST_NAME);
    index += sprintf(&expectedOutput[index], "  -h, --help, Display Help\n");
    index += sprintf(&expectedOutput[index], "  -r, --recursive, List Subdirectories Recursively\n");
    index += sprintf(&expectedOutput[index], "[%s/]%s ", CLI_DIRECTORY_ROOT_NAME, CLI_OUTPUT_COMMAND_PROMPT);
    (void)sprintf(cli_inputBuffer, "%s --help\n", CLI_COMMAND_LIST_NAME);
    
    /* Process Input */
    TEST_ASSERT_TRUE(cli_processInput());
    
    /* Verify */
    TEST_ASSERT_EQUAL_STRING(expectedOutput, helper_output);
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

void test_listCommandHandlerCallback_2(void)
{
    /*** List Command Handler (Help, Long Option) ***/
    /* Variable */
    char expectedOutput[CLI_INPUT_MAXIMUM_LENGTH];
    int index;
    
    /* Set Up */
    index = 0;
    index += sprintf(&expectedOutput[index], "usage: %s [OPTION], List Directory Entries\n", CLI_COMMAND_LIST_NAME);
    index += sprintf(&expectedOutput[index], "  -h, --help, Display Help\n");
    index += sprintf(&expectedOutput[index], "  -r, --recursive, List Subdirectories Recursively\n");
    index += sprintf(&expectedOutput[index], "[%s/]%s ", CLI_DIRECTORY_ROOT_NAME, CLI_OUTPUT_COMMAND_PROMPT);
    (void)sprintf(cli_inputBuffer, "%s --help\n", CLI_COMMAND_LIST_NAME);
    
    /* Process Input */
    TEST_ASSERT_TRUE(cli_processInput());
    
    /* Verify */
    TEST_ASSERT_EQUAL_STRING(expectedOutput, helper_output);
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

void test_listCommandHandlerCallback_3(void)
{
    /*** List Command Handler (Help, Invalid Arguments) ***/
    /* Constant */
    const char *Input[] = {"-i", "-hplease", "--help=please", "--helpme", "this", "this that", "--this", "--this --that"};
    const size_t InputSize = sizeof(Input) / sizeof(Input[0]);
    
    /* Variable */
    char expectedOutput[CLI_INPUT_MAXIMUM_LENGTH];
    size_t i;
    int index;
    
    /* Set Up */
    index = 0;
    index += sprintf(&expectedOutput[index], "usage: %s [OPTION], List Directory Entries\n", CLI_COMMAND_LIST_NAME);
    index += sprintf(&expectedOutput[index], "  -h, --help, Display Help\n");
    index += sprintf(&expectedOutput[index], "  -r, --recursive, List Subdirectories Recursively\n");
    index += sprintf(&expectedOutput[index], "[%s/]%s ", CLI_DIRECTORY_ROOT_NAME, CLI_OUTPUT_COMMAND_PROMPT);
    (void)sprintf(cli_inputBuffer, "%s --help\n", CLI_COMMAND_LIST_NAME);
    
    /* Test */
    for(i = 0; i < InputSize; i++)
    {
        /* Set Up */
        (void)sprintf(cli_inputBuffer, "%s %s\n", CLI_COMMAND_LIST_NAME, Input[i]);
        (void)memset(helper_output, 0, sizeof(helper_output));
        helper_outputIndex = 0;
        
        /* Process Input */
        TEST_ASSERT_TRUE(cli_processInput());
        
        /* Verify */
        TEST_ASSERT_EQUAL_STRING(expectedOutput, helper_output);
    }
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

void test_listCommandHandlerCallback_4(void)
{
    /*** List Command Handler (Not Recursive) ***/
    /* Constant */
    const char *CommandName[CLI_OUTPUT_ANCESTRY_MAXIMUM_DEPTH] = {"zerozero", "oneone", "twotwo", "threethree", "fourfour"};
    const char *DirectoryName[CLI_OUTPUT_ANCESTRY_MAXIMUM_DEPTH - 1] = {"one", "two", "three", "four"};
    
    /* Variable */
    cli_record_t commandRecord[CLI_OUTPUT_ANCESTRY_MAXIMUM_DEPTH], directoryRecord[CLI_OUTPUT_ANCESTRY_MAXIMUM_DEPTH - 1];
    char expectedOutput[CLI_INPUT_MAXIMUM_LENGTH];
    size_t i;
    int index;
    
    /* Set Up */
    for(i = 0; i < (CLI_OUTPUT_ANCESTRY_MAXIMUM_DEPTH - 1); i++)
    {
        if(i == 0)
        {
            TEST_ASSERT_TRUE(cli_registerCommand(&commandRecord[i], CommandName[i], CLI_PARENT_DIRECTORY_ROOT, helper_commandHandlerCallback));
            TEST_ASSERT_TRUE(cli_registerDirectory(&directoryRecord[i], DirectoryName[i], CLI_PARENT_DIRECTORY_ROOT));
        }
        else
        {
            TEST_ASSERT_TRUE(cli_registerCommand(&commandRecord[i], CommandName[i], &directoryRecord[i - 1], helper_commandHandlerCallback));
            TEST_ASSERT_TRUE(cli_registerDirectory(&directoryRecord[i], DirectoryName[i], &directoryRecord[i - 1]));
        }
    }
    TEST_ASSERT_TRUE(cli_registerCommand(&commandRecord[i], CommandName[i], &directoryRecord[i - 1], helper_commandHandlerCallback));
    index = 0;
    index += sprintf(&expectedOutput[index], "%s\n", CLI_COMMAND_CHANGE_DIRECTORY_NAME);
    index += sprintf(&expectedOutput[index], "%s\n", CLI_COMMAND_LIST_NAME);
    index += sprintf(&expectedOutput[index], "%s/\n", DirectoryName[0]); 
    index += sprintf(&expectedOutput[index], "%s\n", CommandName[0]);
    index += sprintf(&expectedOutput[index], "[%s/]%s ", CLI_DIRECTORY_ROOT_NAME, CLI_OUTPUT_COMMAND_PROMPT);
    (void)sprintf(cli_inputBuffer, "%s\n", CLI_COMMAND_LIST_NAME);
    
    /* Process Input */
    TEST_ASSERT_TRUE(cli_processInput());
    
    /* Verify */
    TEST_ASSERT_EQUAL_STRING(expectedOutput, helper_output);
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

void test_listCommandHandlerCallback_5(void)
{
    /*** List Command Handler (Recursive, Short Option) ***/
    /* Constant */
    const char *CommandName[CLI_OUTPUT_ANCESTRY_MAXIMUM_DEPTH] = {"oneone", "twotwo", "threethree", "fourfour", "fivefive"};
    const char *DirectoryName[CLI_OUTPUT_ANCESTRY_MAXIMUM_DEPTH - 1] = {"one", "two", "three", "four"};
    
    /* Variable */
    cli_record_t commandRecord[CLI_OUTPUT_ANCESTRY_MAXIMUM_DEPTH], directoryRecord[CLI_OUTPUT_ANCESTRY_MAXIMUM_DEPTH - 1];
    char expectedOutput[CLI_INPUT_MAXIMUM_LENGTH];
    size_t i;
    int index;
    
    /* Set Up */
    for(i = 0; i < (CLI_OUTPUT_ANCESTRY_MAXIMUM_DEPTH - 1); i++)
    {
        if(i == 0)
        {
            TEST_ASSERT_TRUE(cli_registerCommand(&commandRecord[i], CommandName[i], CLI_PARENT_DIRECTORY_ROOT, helper_commandHandlerCallback));
            TEST_ASSERT_TRUE(cli_registerDirectory(&directoryRecord[i], DirectoryName[i], CLI_PARENT_DIRECTORY_ROOT));
        }
        else
        {
            TEST_ASSERT_TRUE(cli_registerCommand(&commandRecord[i], CommandName[i], &directoryRecord[i - 1], helper_commandHandlerCallback));
            TEST_ASSERT_TRUE(cli_registerDirectory(&directoryRecord[i], DirectoryName[i], &directoryRecord[i - 1]));
        }
    }
    TEST_ASSERT_TRUE(cli_registerCommand(&commandRecord[i], CommandName[i], &directoryRecord[i - 1], helper_commandHandlerCallback));
    index = 0;
    index += sprintf(&expectedOutput[index], "%s\n", CLI_COMMAND_CHANGE_DIRECTORY_NAME);
    index += sprintf(&expectedOutput[index], "%s\n", CLI_COMMAND_LIST_NAME);
    index += sprintf(&expectedOutput[index], "one/\n");
    index += sprintf(&expectedOutput[index], "  two/\n");
    index += sprintf(&expectedOutput[index], "    three/\n");
    index += sprintf(&expectedOutput[index], "      four/\n");
    index += sprintf(&expectedOutput[index], "        fivefive\n");
    index += sprintf(&expectedOutput[index], "      fourfour\n");
    index += sprintf(&expectedOutput[index], "    threethree\n");
    index += sprintf(&expectedOutput[index], "  twotwo\n");
    index += sprintf(&expectedOutput[index], "oneone\n");
    index += sprintf(&expectedOutput[index], "[%s/]%s ", CLI_DIRECTORY_ROOT_NAME, CLI_OUTPUT_COMMAND_PROMPT);
    (void)sprintf(cli_inputBuffer, "%s -r\n", CLI_COMMAND_LIST_NAME);
    
    /* Process Input */
    TEST_ASSERT_TRUE(cli_processInput());
    
    /* Verify */
    TEST_ASSERT_EQUAL_STRING(expectedOutput, helper_output);
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

void test_listCommandHandlerCallback_6(void)
{
    /*** List Command Handler (Recursive, Long Option) ***/
    /* Constant */
    const char *CommandName[CLI_OUTPUT_ANCESTRY_MAXIMUM_DEPTH] = {"oneone", "twotwo", "threethree", "fourfour", "fivefive"};
    const char *DirectoryName[CLI_OUTPUT_ANCESTRY_MAXIMUM_DEPTH - 1] = {"one", "two", "three", "four"};
    
    /* Variable */
    cli_record_t commandRecord[CLI_OUTPUT_ANCESTRY_MAXIMUM_DEPTH], directoryRecord[CLI_OUTPUT_ANCESTRY_MAXIMUM_DEPTH - 1];
    char expectedOutput[CLI_INPUT_MAXIMUM_LENGTH];
    size_t i;
    int index;
    
    /* Set Up */
    for(i = 0; i < (CLI_OUTPUT_ANCESTRY_MAXIMUM_DEPTH - 1); i++)
    {
        if(i == 0)
        {
            TEST_ASSERT_TRUE(cli_registerCommand(&commandRecord[i], CommandName[i], CLI_PARENT_DIRECTORY_ROOT, helper_commandHandlerCallback));
            TEST_ASSERT_TRUE(cli_registerDirectory(&directoryRecord[i], DirectoryName[i], CLI_PARENT_DIRECTORY_ROOT));
        }
        else
        {
            TEST_ASSERT_TRUE(cli_registerCommand(&commandRecord[i], CommandName[i], &directoryRecord[i - 1], helper_commandHandlerCallback));
            TEST_ASSERT_TRUE(cli_registerDirectory(&directoryRecord[i], DirectoryName[i], &directoryRecord[i - 1]));
        }
    }
    TEST_ASSERT_TRUE(cli_registerCommand(&commandRecord[i], CommandName[i], &directoryRecord[i - 1], helper_commandHandlerCallback));
    index = 0;
    index += sprintf(&expectedOutput[index], "%s\n", CLI_COMMAND_CHANGE_DIRECTORY_NAME);
    index += sprintf(&expectedOutput[index], "%s\n", CLI_COMMAND_LIST_NAME);
    index += sprintf(&expectedOutput[index], "one/\n");
    index += sprintf(&expectedOutput[index], "  two/\n");
    index += sprintf(&expectedOutput[index], "    three/\n");
    index += sprintf(&expectedOutput[index], "      four/\n");
    index += sprintf(&expectedOutput[index], "        fivefive\n");
    index += sprintf(&expectedOutput[index], "      fourfour\n");
    index += sprintf(&expectedOutput[index], "    threethree\n");
    index += sprintf(&expectedOutput[index], "  twotwo\n");
    index += sprintf(&expectedOutput[index], "oneone\n");
    index += sprintf(&expectedOutput[index], "[%s/]%s ", CLI_DIRECTORY_ROOT_NAME, CLI_OUTPUT_COMMAND_PROMPT);
    (void)sprintf(cli_inputBuffer, "%s --recursive\n", CLI_COMMAND_LIST_NAME);
    
    /* Process Input */
    TEST_ASSERT_TRUE(cli_processInput());
    
    /* Verify */
    TEST_ASSERT_EQUAL_STRING(expectedOutput, helper_output);
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

void test_listCommandHandlerCallback_7(void)
{
    /*** List Command Handler (Recursive, Long Option, Malloc Failure) ***/
    /* Constant */
    const char *CommandName[CLI_OUTPUT_ANCESTRY_MAXIMUM_DEPTH] = {"oneone", "twotwo", "threethree", "fourfour", "fivefive"};
    const char *DirectoryName[CLI_OUTPUT_ANCESTRY_MAXIMUM_DEPTH - 1] = {"one", "two", "three", "four"};
    
    /* Variable */
    cli_record_t commandRecord[CLI_OUTPUT_ANCESTRY_MAXIMUM_DEPTH], directoryRecord[CLI_OUTPUT_ANCESTRY_MAXIMUM_DEPTH - 1];
    char expectedOutput[CLI_INPUT_MAXIMUM_LENGTH];
    size_t i;
    int index;
    
    /* Set Up */
    for(i = 0; i < (CLI_OUTPUT_ANCESTRY_MAXIMUM_DEPTH - 1); i++)
    {
        if(i == 0)
        {
            TEST_ASSERT_TRUE(cli_registerCommand(&commandRecord[i], CommandName[i], CLI_PARENT_DIRECTORY_ROOT, helper_commandHandlerCallback));
            TEST_ASSERT_TRUE(cli_registerDirectory(&directoryRecord[i], DirectoryName[i], CLI_PARENT_DIRECTORY_ROOT));
        }
        else
        {
            TEST_ASSERT_TRUE(cli_registerCommand(&commandRecord[i], CommandName[i], &directoryRecord[i - 1], helper_commandHandlerCallback));
            TEST_ASSERT_TRUE(cli_registerDirectory(&directoryRecord[i], DirectoryName[i], &directoryRecord[i - 1]));
        }
    }
    TEST_ASSERT_TRUE(cli_registerCommand(&commandRecord[i], CommandName[i], &directoryRecord[i - 1], helper_commandHandlerCallback));
    index = 0;
    index += sprintf(&expectedOutput[index], "%s\n", CLI_COMMAND_CHANGE_DIRECTORY_NAME);
    index += sprintf(&expectedOutput[index], "%s\n", CLI_COMMAND_LIST_NAME);
    index += sprintf(&expectedOutput[index], "one/\n");
    index += sprintf(&expectedOutput[index], "two/\n");
    index += sprintf(&expectedOutput[index], "three/\n");
    index += sprintf(&expectedOutput[index], "four/\n");
    index += sprintf(&expectedOutput[index], "fivefive\n");
    index += sprintf(&expectedOutput[index], "fourfour\n");
    index += sprintf(&expectedOutput[index], "threethree\n");
    index += sprintf(&expectedOutput[index], "twotwo\n");
    index += sprintf(&expectedOutput[index], "oneone\n");
    index += sprintf(&expectedOutput[index], "[%s/]%s ", CLI_DIRECTORY_ROOT_NAME, CLI_OUTPUT_COMMAND_PROMPT);
    (void)sprintf(cli_inputBuffer, "%s --recursive\n", CLI_COMMAND_LIST_NAME);
    helper_memoryMallocStubFail = true;
    
    /* Process Input */
    TEST_ASSERT_TRUE(cli_processInput());
    
    /* Verify */
    TEST_ASSERT_EQUAL_STRING(expectedOutput, helper_output);
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

/*** Print Command Prompt ***/
void test_printCommandPrompt_1(void)
{
    /*** Print Command Prompt ***/
    /* Constant */
    const char *DirectoryName[CLI_OUTPUT_ANCESTRY_MAXIMUM_DEPTH] = {"one", "two", "three", "four", "five"};
    
    /* Variable */
    cli_record_t directoryRecord[CLI_OUTPUT_ANCESTRY_MAXIMUM_DEPTH];
    char expectedCommandPrompt[100];
    size_t i, j;
    int index;
    
    /* Set Up */
    for(i = 0; i < CLI_OUTPUT_ANCESTRY_MAXIMUM_DEPTH; i++)
    {
        if(i == 0)
            TEST_ASSERT_TRUE(cli_registerDirectory(&directoryRecord[i], DirectoryName[i], CLI_PARENT_DIRECTORY_ROOT));
        else
            TEST_ASSERT_TRUE(cli_registerDirectory(&directoryRecord[i], DirectoryName[i], &directoryRecord[i - 1]));
    }
    
    /* Test */
    for(i = 0; i < (CLI_OUTPUT_ANCESTRY_MAXIMUM_DEPTH + 1); i++)
    {        
        /* Set Up */        
        if(i != 0)
            cli_ActiveDirectoryRecord = &directoryRecord[i - 1];
        if(i == CLI_OUTPUT_ANCESTRY_MAXIMUM_DEPTH)
        {
            /* Beyond Maximum Ancestry Depth (e.g. one/two/three/) */
            index = 0;
            index = sprintf(&expectedCommandPrompt[index], "[");
            for(j = 0; j < i; j++)
                index += sprintf(&expectedCommandPrompt[index], "%s/", DirectoryName[j]);
            index += sprintf(&expectedCommandPrompt[index], "]%s ", CLI_OUTPUT_COMMAND_PROMPT);
        }
        else
        {
            /* Up To Maximum Ancestry Depth (e.g. root/one/two/) */
            index = 0;
            index = sprintf(&expectedCommandPrompt[index], "[%s/", CLI_DIRECTORY_ROOT_NAME);
            for(j = 0; j < i; j++)
                index += sprintf(&expectedCommandPrompt[index], "%s/", DirectoryName[j]);
            index += sprintf(&expectedCommandPrompt[index], "]%s ", CLI_OUTPUT_COMMAND_PROMPT);
        }
        (void)memset(helper_output, 0, sizeof(helper_output));
        helper_outputIndex = 0;
        
        /* Print Command Prompt */
        cli_printCommandPrompt();
        
        /* Verify */
        TEST_ASSERT_EQUAL_STRING(expectedCommandPrompt, helper_output);
    }
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

/*** Print Ancestry ***/
void test_printAncestry_1(void)
{
    /*** Print Ancestry ***/
    /* Constant */
    const char *DirectoryName[CLI_OUTPUT_ANCESTRY_MAXIMUM_DEPTH - 1] = {"one", "two", "three", "four"};
    
    /* Variable */
    cli_record_t directoryRecord[CLI_OUTPUT_ANCESTRY_MAXIMUM_DEPTH - 1];
    char expectedAncestry[100];
    size_t i, j;
    int index;
    
    /* Set Up */
    for(i = 0; i < (CLI_OUTPUT_ANCESTRY_MAXIMUM_DEPTH - 1); i++)
    {
        if(i == 0)
            TEST_ASSERT_TRUE(cli_registerDirectory(&directoryRecord[i], DirectoryName[i], CLI_PARENT_DIRECTORY_ROOT));
        else
            TEST_ASSERT_TRUE(cli_registerDirectory(&directoryRecord[i], DirectoryName[i], &directoryRecord[i - 1]));
    }
    
    /* Test */
    for(i = 0; i < CLI_OUTPUT_ANCESTRY_MAXIMUM_DEPTH; i++)
    {        
        /* Set Up */        
        if(i != 0)
            cli_ActiveDirectoryRecord = &directoryRecord[i - 1];
        index = 0;
        index += sprintf(&expectedAncestry[index], "%s/", CLI_DIRECTORY_ROOT_NAME);
        for(j = 0; j < i; j++)
            index += sprintf(&expectedAncestry[index], "%s/", DirectoryName[j]);
        (void)memset(helper_output, 0, sizeof(helper_output));
        helper_outputIndex = 0;
        
        /* Print Ancestry */
        cli_printAncestry();
        
        /* Verify */
        TEST_ASSERT_EQUAL_STRING(expectedAncestry, helper_output);
    }
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

/*** Process Command ***/
void test_processCommand_1(void)
{
    /*** Process Command (Not Found) ***/
    /* Variable */
    char expectedOutput[100];
    
    /* Set Up */
    (void)sprintf(expectedOutput, "Command Not Found\n");
    
    /* Process Command */
    cli_processCommand("invalid", 0, NULL);
    
    /* Verify */
    TEST_ASSERT_EQUAL_STRING(expectedOutput, helper_output);
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

void test_processCommand_2(void)
{
    /*** Process Command (Not A Command) ***/
    /* Constant */
    const char * const DirectoryName = "folder";
    
    /* Variable */
    cli_record_t directoryRecord;
    char expectedOutput[100];
    
    /* Set Up */
    (void)sprintf(expectedOutput, "Not A Command\n");
    TEST_ASSERT_TRUE(cli_registerDirectory(&directoryRecord, DirectoryName, CLI_PARENT_DIRECTORY_ROOT));
    
    /* Process Command */
    cli_processCommand(DirectoryName, 0, NULL);
    
    /* Verify */
    TEST_ASSERT_EQUAL_STRING(expectedOutput, helper_output);
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

void test_processCommand_3(void)
{
    /*** Process Command (Change Directory Command) ***/
    /* Constant */
    const char * const DirectoryName = "folder";
    
    /* Variable */
    size_t argc;
    char argument[100], *argv[1];
    cli_record_t directoryRecord;
    
    /* Set Up */
    argc = sizeof(argv) / sizeof(argv[0]);
    (void)strcpy(argument, DirectoryName);
    argv[0] = argument;
    TEST_ASSERT_TRUE(cli_registerDirectory(&directoryRecord, DirectoryName, CLI_PARENT_DIRECTORY_ROOT));
    
    /* Process Command */
    cli_processCommand(CLI_COMMAND_CHANGE_DIRECTORY_NAME, argc, argv);
    
    /* Verify */
    TEST_ASSERT_EQUAL_PTR(&directoryRecord, cli_ActiveDirectoryRecord);
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

void test_processCommand_4(void)
{
    /*** Process Command (List) ***/
    /* Variable */
    char expectedOutput[100];
    
    /* Set Up */
    (void)sprintf(expectedOutput, "%s\n%s\n", CLI_COMMAND_CHANGE_DIRECTORY_NAME, CLI_COMMAND_LIST_NAME);
    
    /* Process Command */
    cli_processCommand(CLI_COMMAND_LIST_NAME, 0, NULL);
    
    /* Verify */
    TEST_ASSERT_EQUAL_STRING(expectedOutput, helper_output);
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

void test_processCommand_5(void)
{
    /*** Process Command (Custom Command) ***/
    /* Define */
    #define ARGUMENT_COUNT (3)
    
    /* Constant */
    const char * const Argv[ARGUMENT_COUNT] = {"one", "two", "three"};
    const char * const CommandName = "help";
    
    /* Variable */
    size_t argc;
    char actualArgument[ARGUMENT_COUNT][100], *argv[ARGUMENT_COUNT], expectedArgument[ARGUMENT_COUNT][100];
    cli_record_t commandRecord;
    size_t i;
    
    /* Set Up */
    argc = sizeof(argv) / sizeof(argv[0]);
    helper_expectedArgc = argc;
    for(i = 0; i < argc; i++)
    {
        (void)strcpy(actualArgument[i], Argv[i]);
        (void)strcpy(expectedArgument[i], Argv[i]);
        argv[i] = actualArgument[i];
        helper_expectedArgv[i] = expectedArgument[i];
    }
    TEST_ASSERT_TRUE(cli_registerCommand(&commandRecord, CommandName, CLI_PARENT_DIRECTORY_ROOT, helper_commandHandlerCallback));
    
    /* Process Command */
    cli_processCommand(CommandName, argc, argv);
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}

/*** Verify Name Acceptable ***/
void test_verifyNameAcceptable_1(void)
{
    /*** Verify Name Acceptable ***/
    /* Constant */
    const char * const CommandName = "help";
    const char * const DirectoryName = "folder";
    
    /* Structure */
    typedef struct
    {
        bool acceptable;
        const char * const Name;
    } testData_t;
    
    /* Test Data */
    const testData_t TestData[] =
    {
        {false, "te\tst"},
        {false, "te st"},
        {false, "te.st"},
        {false, CLI_DIRECTORY_ROOT_NAME},
        {false, DirectoryName},
        {false, CommandName},
        {true, "other"}
    };
    const size_t TestDataCount = sizeof(TestData) / sizeof(TestData[0]);
    
    /* Variable */
    cli_record_t commandRecord, directoryRecord;
    size_t i;
    
    /* Set Up */
    TEST_ASSERT_TRUE(cli_registerDirectory(&directoryRecord, DirectoryName, CLI_PARENT_DIRECTORY_ROOT));
    
    /* Register Command */
    TEST_ASSERT_TRUE(cli_registerCommand(&commandRecord, CommandName, &directoryRecord, helper_commandHandlerCallback));
    
    /* Test */
    for(i = 0; i < TestDataCount; i++)
    {
        /* Verify Name Acceptable */
        TEST_ASSERT_EQUAL(TestData[i].acceptable, cli_verifyNameAcceptable(TestData[i].Name, &directoryRecord));
    }
    
    /* Clean Up */
    cli_deinit(); // Called Here Instead Of In tearDown Because Some Tests Register Commands/Directories With Record Variables On Stack And To Be Consistent
}
