/****************************************************************************************************
 * Include
 ****************************************************************************************************/

#include <ctype.h>
#include "Eclectic/CLI/cli.h"
#include "Eclectic/Data/list.h"
#include "Eclectic/Miscellaneous/memory.h"
#include "Eclectic/Miscellaneous/project.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/****************************************************************************************************
 * Variable
 ****************************************************************************************************/

PROJECT_STATIC(const cli_record_t *cli_ActiveDirectoryRecord);
PROJECT_STATIC(cli_record_t cli_changeDirectoryCommandRecord);
PROJECT_STATIC(char cli_inputBuffer[CLI_INPUT_MAXIMUM_LENGTH + 1]);
PROJECT_STATIC(cli_record_t cli_listCommandRecord);
PROJECT_STATIC(cli_printCallback_t cli_printCallback);
PROJECT_STATIC(cli_processInputCallback_t cli_processInputCallback);
PROJECT_STATIC(char cli_receiveBuffer[CLI_INPUT_MAXIMUM_LENGTH + 1]);
PROJECT_STATIC(size_t cli_receiveBufferIndex);
PROJECT_STATIC(cli_record_t cli_rootDirectoryRecord);

/****************************************************************************************************
 * Function Prototype
 ****************************************************************************************************/

static void cli_changeDirectoryCommandHandlerCallback(size_t argc, char *argv[]);
static int cli_compareCallback(const void * const Data1, const void * const Data2);
static void cli_deinitDirectory(cli_record_t * const directoryRecord);
PROJECT_STATIC(bool cli_getCommandAndArgumentList(char * const input, char **command, size_t * const argc, char *argv[]));
static void cli_listCommandHandlerCallback(size_t argc, char *argv[]);
static void cli_listCommandHandlerCallbackHelper(const cli_record_t * const Directory, const size_t IndentSize, const bool Recursive);
static void cli_presetRecord(cli_record_t * const record, const cli_recordType_t RecordType, const char * const Name, const cli_record_t * const ParentDirectory);
PROJECT_STATIC(void cli_printAncestry(void));
PROJECT_STATIC(void cli_printCommandPrompt(void));
PROJECT_STATIC(void cli_processCommand(const char * const Command, const size_t Argc, char *argv[]));
PROJECT_STATIC(bool cli_verifyNameAcceptable(const char * const Name, const cli_record_t * const ParentDirectory));

/****************************************************************************************************
 * Function Definition (Public)
 ****************************************************************************************************/

/*** Add Receive Character ***/
bool cli_addReceiveCharacter(const char Character)
{
    /*** Add Receive Character ***/
    /* Variable */
    bool success;
    
    /* Set Up */
    success = false;
    
    /* Error Check */
    if(cli_processInputCallback != NULL)
    {
        /* Handle */
        if(Character == '\b')
        {
            /* Backspace */
            if(cli_receiveBufferIndex != 0)
                cli_receiveBuffer[--cli_receiveBufferIndex] = '\0';
        }
        else
        {
            /* Other Character */
            cli_receiveBuffer[cli_receiveBufferIndex++] = Character;
        }
        
        /* Process Input */
        if((Character == '\n') || (cli_receiveBufferIndex == CLI_INPUT_MAXIMUM_LENGTH))
        {
            /* Copy And Reset */
            cli_receiveBuffer[CLI_INPUT_MAXIMUM_LENGTH] = '\0'; // For When cli_receiveBufferIndex == CLI_INPUT_MAXIMUM_LENGTH
            (void)memcpy(cli_inputBuffer, cli_receiveBuffer, sizeof(cli_receiveBuffer));
            (void)memset(cli_receiveBuffer, 0, sizeof(cli_receiveBuffer));
            cli_receiveBufferIndex = 0;
            
            /* Process Input */
            cli_processInputCallback();
        }
        
        /* Success */
        success = true;
    }
    
    /* Exit */
    return success;
}

/*** Deinitialize ***/
void cli_deinit(void)
{
    /*** Deinitialize ***/
    cli_ActiveDirectoryRecord = NULL;
    cli_printCallback = NULL;
    cli_processInputCallback = NULL;
    cli_deinitDirectory(&cli_rootDirectoryRecord);
}

/*** Get Argument/Option Pair ***/
bool cli_getArgumentOptionPair(char * const input, cli_argumentOptionPair_t * const argumentOptionPair)
{
    /*** Get Argument/Option Pair ***/
    /* Variable */
    size_t i;
    bool success;
    
    /* Set Up */
    success = false;
    
    /* Error Check */
    if((input != NULL) && (strlen(input) >= 2) && (argumentOptionPair != NULL))
    {
        /* Set Up */
        argumentOptionPair->argument = NULL; // May Not Be Present
        
        /* Parse (Structured Like The Following To Pass GCOV) */
        if(input[0] == '-')
        {
            /* Parse */
            if(input[1] == '-')
            {
                /* Error Check */      
                if(input[2] != '\0')
                {
                    /* Set Up */
                    i = 2;
                    
                    /* Long Option */
                    argumentOptionPair->option = &input[2];
                    while(input[i] != '\0')
                    {
                        /* Replace '=' With '\0' */
                        if(input[i] == '=')
                        {
                            input[i] = '\0';
                            argumentOptionPair->argument = &input[i + 1];
                            break;
                        }
                        i++;
                    }
                    
                    /* Success */
                    success = true;
                }
            }
            else
            {
                /* Replace '-' With Option And '\0' Terminate */
                input[0] = input[1];
                input[1] = '\0';
                
                /* Short Option */
                argumentOptionPair->option = &input[0];
                if(input[2] != '\0')
                    argumentOptionPair->argument = &input[2];
                
                /* Success */
                success = true;
            }
        }
    }
    
    /* Exit */
    return success;
}

/*** Get Signed 32-Bit Integer ***/
bool cli_getS32(const char * const Input, int32_t * const s32)
{
    /*** Get Signed 32-Bit Integer ***/
    /* Variable */
    char *end;
    bool success;
    
    /* Set Up */
    end = NULL;
    success = false;
    
    /* Error Check */
    if((Input != NULL) && (strlen(Input) > 0) && (s32 != NULL))
    {
        /* Get Signed 32-Bit Integer */
        *s32 = (int32_t)strtol(Input, &end, 0);
        if(*end == '\0')
            success = true;
    }
    
    /* Exit */
    return success;
}

/*** Get Unsigned 32-Bit Integer ***/
bool cli_getU32(const char * const Input, uint32_t * const u32)
{
    /*** Get Unsigned 32-Bit Integer ***/
    /* Variable */
    char *end;
    bool success;
    
    /* Set Up */
    end = NULL;
    success = false;
    
    /* Error Check */
    if((Input != NULL) && (strlen(Input) > 0) && (u32 != NULL))
    {
        /* Get Unsigned 32-Bit Integer */
        *u32 = (uint32_t)strtoul(Input, &end, 0);
        if(*end == '\0')
            success = true;
    }
    
    /* Exit */
    return success;
}

/*** Initialize ***/
bool cli_init(const cli_printCallback_t PrintCallback, const cli_processInputCallback_t ProcessInputCallback)
{
    /*** Initialize ***/
    /* Variable */
    bool success;
    
    /* Set Up */
    success = false;
    
    /* Error Check */
    if((PrintCallback != NULL) && (ProcessInputCallback != NULL))
    {
        /* Set Up */
        cli_ActiveDirectoryRecord = &cli_rootDirectoryRecord;
        cli_printCallback = PrintCallback;
        cli_processInputCallback = ProcessInputCallback;
        
        /* Root Directory */
        cli_presetRecord(&cli_rootDirectoryRecord, CLI_RECORD_TYPE_DIRECTORY, CLI_DIRECTORY_ROOT_NAME, NULL);
        
        /* Built-In Commands */
        (void)cli_registerCommand(&cli_changeDirectoryCommandRecord, CLI_COMMAND_CHANGE_DIRECTORY_NAME, &cli_rootDirectoryRecord, cli_changeDirectoryCommandHandlerCallback);
        (void)cli_registerCommand(&cli_listCommandRecord, CLI_COMMAND_LIST_NAME, &cli_rootDirectoryRecord, cli_listCommandHandlerCallback);
        
        /* Receive Buffer */
        (void)memset(cli_receiveBuffer, 0, sizeof(cli_receiveBuffer));
        cli_receiveBufferIndex = 0;
        
        /* Print Command Prompt */
        cli_printCommandPrompt();
        
        /* Success */
        success = true;
    }
    
    /* Exit */
    return success;
}

/*** Process Input ***/
bool cli_processInput(void)
{
    /*** Process Input ***/
    /* Variable */
    char *argv[CLI_INPUT_MAXIMUM_ARGUMENTS], *command;
    size_t argc = 0;
    bool success;
    
    /* Set Up */
    success = false;
    
    /* Error Check */
    if(cli_printCallback != NULL)
    {        
        /* Parse */
        if(cli_getCommandAndArgumentList(cli_inputBuffer, &command, &argc, argv))
        {
            /* Process */
            cli_processCommand(command, argc, argv);
            
            /* Success */
            success = true;
        }
        
        /* Print Command Prompt */
        cli_printCommandPrompt();
    }
    
    /* Reset */
    (void)memset(cli_inputBuffer, 0, sizeof(cli_inputBuffer));
    
    /* Exit */
    return success;
}

/*** Register Command ***/
bool cli_registerCommand(cli_record_t * const record, const char * const Name, cli_record_t *parentDirectory, const cli_commandHandlerCallback_t CommandHandlerCallback)
{
    /*** Register Command ***/
    /* Variable */
    bool success;
    
    /* Set Up */
    success = false;
    
    /* Error Check */
    if((record != NULL) && (Name != NULL) && (CommandHandlerCallback != NULL))
    {
        /* Set Up */
        if(parentDirectory == CLI_PARENT_DIRECTORY_ROOT)
            parentDirectory = &cli_rootDirectoryRecord;
        
        /* Name */
        if((parentDirectory->type == CLI_RECORD_TYPE_DIRECTORY) && (success = cli_verifyNameAcceptable(Name, parentDirectory)))
        {
            /* Preset Record */
            cli_presetRecord(record, CLI_RECORD_TYPE_COMMAND, Name, parentDirectory);
            record->handler = CommandHandlerCallback;

            /* Add Child Record To Parent Directory */
            list_pushSorted(&(parentDirectory->entries), record, true); // Ascending
        }
    }
    
    /* Exit */
    return success;
}

/*** Register Directory ***/
bool cli_registerDirectory(cli_record_t * const record, const char * const Name, cli_record_t *parentDirectory)
{
    /*** Register Directory ***/
    /* Variable */
    bool success;
    
    /* Set Up */
    success = false;
    
    /* Error Check */
    if((record != NULL) && (Name != NULL))
    {
        /* Set Up */
        if(parentDirectory == CLI_PARENT_DIRECTORY_ROOT)
            parentDirectory = &cli_rootDirectoryRecord;
        
        /* Name */
        if((parentDirectory->type == CLI_RECORD_TYPE_DIRECTORY) && (success = cli_verifyNameAcceptable(Name, parentDirectory)))
        {
            /* Preset Record */
            cli_presetRecord(record, CLI_RECORD_TYPE_DIRECTORY, Name, parentDirectory);

            /* Add Child Record To Parent Directory */
            list_pushSorted(&(parentDirectory->entries), record, true); // Ascending
        }
    }
    
    /* Exit */
    return success;
}

/****************************************************************************************************
 * Function Definition (Private)
 ****************************************************************************************************/

/*** Change Directory Command Handler **/
static void cli_changeDirectoryCommandHandlerCallback(size_t argc, char *argv[])
{
    /*** Change Directory Command Handler **/
    /* Variable */
    bool help;
    size_t index;
    cli_argumentOptionPair_t argumentOptionPair;
    cli_record_t key, *record;
    
    /* Set Up */
    help = false;
    record = NULL;

    /* Handle */
    switch(argc)
    {
        case 0:
            /* Set Active Directory To Root Directory */
            cli_ActiveDirectoryRecord = &cli_rootDirectoryRecord;
            break;
        case 1:
            /* Handle */
            if(cli_getArgumentOptionPair(argv[0], &argumentOptionPair))
            {
                /* Handle Option/Argument Pair */
                help = true; // Nothing To Handle
            }
            else if(strcmp(argv[0], "..") == 0)
            {
                /* Move Up One Directory, Unless At Root Directory Already */
                if(cli_ActiveDirectoryRecord->Parent != NULL)
                    cli_ActiveDirectoryRecord = cli_ActiveDirectoryRecord->Parent;
            }
            else
            {
                /* Find Directory */
                key.Name = argv[0]; // Compare Callback Expects Name Within Record
                if((index = list_find(&(cli_ActiveDirectoryRecord->entries), &key)) != LIST_FIND_NOT_FOUND_INDEX)
                    record = list_peekAt(&(cli_ActiveDirectoryRecord->entries), index);
                
                /* Switch To Directory */
                if(record == NULL)
                    cli_printCallback("Directory Not Found\n");
                else if(record->type != CLI_RECORD_TYPE_DIRECTORY)
                    cli_printCallback("Not A Directory\n");
                else
                    cli_ActiveDirectoryRecord = record;
            }
            break;
        default:
            /* Not Handled */
            help = true;
            break;
    }
    
    /* Help */
    if(help)
    {
        /* Usage */
        cli_printCallback("usage: %s [DIRECTORY|OPTION], Change Working Directory\n", CLI_COMMAND_CHANGE_DIRECTORY_NAME);
        cli_printCallback("  -h, --help, Display Help\n");
    }
}

/*** Compare Callback ***/
static int cli_compareCallback(const void * const Data1, const void * const Data2)
{
    /*** Compare Callback ***/
    /* Exit */
    return strcmp(((const cli_record_t *)Data1)->Name, ((const cli_record_t *)Data2)->Name);
}

/*** Deinitialize Directory ***/
static void cli_deinitDirectory(cli_record_t * const directoryRecord)
{
    /*** Deinitialize Directory ***/
    /* Variable */
    size_t i;
    cli_record_t *record;

    /* Deinitialize Children Directories */
    for(i = 0; i < directoryRecord->entries.size; i++)
    {
        /* Peek */
        record = list_peekAt(&(directoryRecord->entries), i);
        if(record->type == CLI_RECORD_TYPE_DIRECTORY)
            cli_deinitDirectory(record);
    }

    /* Deinitialize Current Directory */
    list_deinit(&directoryRecord->entries);
}

/*** Get Command And Argument List ***/
PROJECT_STATIC(bool cli_getCommandAndArgumentList(char * const input, char **command, size_t * const argc, char *argv[]))
{
    /*** Get Command And Argument List ***/
    /* Variable */
    bool argumentFound;
    size_t i;
    bool success;
    
    /* Set Up */
    *argc = 0;
    argumentFound = false;
    *command = NULL;
    success = false;
    
    /* Get Command */
    for(i = 0; i < strlen(input); i++)
    {
        /* Parse */
        if(isprint(input[i]) && (input[i] != ' ') && (*command == NULL))
            *command = &input[i]; // Start
        else if((!isprint(input[i]) || (input[i] == ' ')) && (*command != NULL))
            break; // End
    }

    /* Error Check */
    if(*command != NULL)
    {
        /* Find Arguments */
        while((input[i] != '\0') && (*argc < CLI_INPUT_MAXIMUM_ARGUMENTS))
        {
            /* Parse */
            if(!isspace(input[i]))
            {
                /* Start Or Middle Of Argument */
                if(!argumentFound)
                {
                    argumentFound = true;
                    argv[(*argc)++] = &input[i];
                }
            }
            else
            {
                /* Not An Argument */
                argumentFound = false;
                input[i] = '\0'; // Set to NUL so strlen Works on argv Arguments
            }

            /* Increment */
            i++;
        }
        
        /* Termination */
        while(input[i] != '\0')
        {
            /* Parse */
            if(!isprint(input[i]) || (input[i] == ' '))
            {
                input[i] = '\0';
                break;
            }
            
            /* Increment */
            i++;
        }
        
        /* Success */
        success = true;
    }
    
    /* Exit */
    return success;
}

/*** List Command Handler ***/
static void cli_listCommandHandlerCallback(size_t argc, char *argv[])
{
    /*** List Command Handler ***/
    /* Variable */
    bool help;
    cli_argumentOptionPair_t argumentOptionPair;
    
    /* Set Up */
    help = false;
    
    /*** Handle Arguments ***/
    switch(argc)
    {
        case 0:
            /* List Directory Contents (Not Recursive) */
            cli_listCommandHandlerCallbackHelper(cli_ActiveDirectoryRecord, 0, false);
            break;
        case 1:
            /* Handle */
            if(cli_getArgumentOptionPair(argv[0], &argumentOptionPair))
            {
                /* Handle Argument/Option Pair */
                if((strcmp(argumentOptionPair.option, "h") == 0) || (strcmp(argumentOptionPair.option, "help") == 0))
                {
                    /* Help (Argument Ignored If Any) */
                    help = true;
                }
                else if((strcmp(argumentOptionPair.option, "r") == 0) || (strcmp(argumentOptionPair.option, "recursive") == 0))
                {
                    /* Recursive (Argument Ignored If Any) */
                    cli_listCommandHandlerCallbackHelper(cli_ActiveDirectoryRecord, 0, true);
                }
                else
                {
                    /* Error (Unhandled Argument/Option Pair) */
                    help = true;
                }
            }
            else
            {
                /* Error (Unhandled Argument/Option Pair) */
                help = true;
            }
            break;
        default:
            /* Not Handled */
            help = true;
            break;
    }
    
    /* Help */
    if(help)
    {
        /* Usage */
        cli_printCallback("usage: %s [OPTION], List Directory Entries\n", CLI_COMMAND_LIST_NAME);
        cli_printCallback("  -h, --help, Display Help\n");
        cli_printCallback("  -r, --recursive, List Subdirectories Recursively\n");
    }
}

/*** List Command Handler Callback ***/
static void cli_listCommandHandlerCallbackHelper(const cli_record_t * const Directory, const size_t IndentSize, const bool Recursive)
{
    /*** List Command Handler Callback ***/
    /* Variable */
    size_t i;
    char *indent = NULL;
    cli_record_t *record;
    
    /* Set Up */
    if((indent = memory_malloc(IndentSize + 1)) != NULL)
    {
        for(i = 0; i < IndentSize; i++)
            indent[i] = ' ';
        indent[i] = '\0';
    }

    /* List */
    for(i = 0; i < Directory->entries.size; i++)
    {
        /* Print Record */
        record = list_peekAt(&(Directory->entries), i);
        if(indent != NULL)
            cli_printCallback("%s%s%s\n", indent, record->Name, (record->type == CLI_RECORD_TYPE_DIRECTORY) ? "/" : "");
        else
            cli_printCallback("%s%s\n", record->Name, (record->type == CLI_RECORD_TYPE_DIRECTORY) ? "/" : "");
        
        /* Recursive */
        if(Recursive && (record->type == CLI_RECORD_TYPE_DIRECTORY))
            cli_listCommandHandlerCallbackHelper(record, IndentSize + CLI_LIST_INDENT_SIZE, true);
    }
    
    /* Clean Up */
    if(indent != NULL)
        memory_free((void **)&indent);
}

/*** Preset Record ***/
static void cli_presetRecord(cli_record_t * const record, const cli_recordType_t RecordType, const char * const Name, const cli_record_t * const ParentDirectory)
{
    /*** Preset Record ***/
    record->Name = Name;
    record->Parent = ParentDirectory;
    record->type = RecordType;
    if(record->type == CLI_RECORD_TYPE_DIRECTORY)
        list_init(&(record->entries), cli_compareCallback, NULL);
}

/*** Print Ancestry ***/
PROJECT_STATIC(void cli_printAncestry(void))
{
    /*** Print Ancestry ***/
    /* Variable */
    const cli_record_t *Ancestry[CLI_OUTPUT_ANCESTRY_MAXIMUM_DEPTH], *Record;
    size_t ancestryCount = 0, i;

    /* Set Ancestry Table */
    Record = cli_ActiveDirectoryRecord;
    while((Record != NULL) && (ancestryCount < CLI_OUTPUT_ANCESTRY_MAXIMUM_DEPTH))
    {
        Ancestry[(CLI_OUTPUT_ANCESTRY_MAXIMUM_DEPTH - 1) - ancestryCount++] = Record;
        Record = Record->Parent;
    }

    /* Print Ancestry Table */
    for(i = (CLI_OUTPUT_ANCESTRY_MAXIMUM_DEPTH - ancestryCount); i < CLI_OUTPUT_ANCESTRY_MAXIMUM_DEPTH; i++)
        cli_printCallback("%s/", Ancestry[i]->Name);
}

/*** Print Command Prompt ***/
PROJECT_STATIC(void cli_printCommandPrompt(void))
{
    /*** Print Command Prompt ***/
    cli_printCallback("[");
    cli_printAncestry();
    cli_printCallback("]%s ", CLI_OUTPUT_COMMAND_PROMPT);
}

/*** Process Command ***/
PROJECT_STATIC(void cli_processCommand(const char * const Command, const size_t Argc, char *argv[]))
{
    /*** Process Command ***/
    /* Variable */
    size_t index;
    cli_record_t key, *record;
    
    /* Set Up */
    record = NULL;
    
    /*** Process Command ***/
    /* Find */
    if((strcmp(Command, CLI_COMMAND_CHANGE_DIRECTORY_NAME) == 0) || (strcmp(Command, CLI_COMMAND_LIST_NAME) == 0))
    {
        /* Built-In Command */
        key.Name = Command; // Compare Callback Expects Name Within Record
        index = list_find(&(cli_rootDirectoryRecord.entries), &key);
        record = list_peekAt(&(cli_rootDirectoryRecord.entries), index);
    }
    else
    {
        /* Active Directory */
        key.Name = Command; // Compare Callback Expects Name Within Record
        if((index = list_find(&(cli_ActiveDirectoryRecord->entries), &key)) != LIST_FIND_NOT_FOUND_INDEX)
            record = list_peekAt(&(cli_ActiveDirectoryRecord->entries), index);
    }
    
    /* Process */
    if(record != NULL)
    {    
        if(record->type == CLI_RECORD_TYPE_COMMAND)
            record->handler(Argc, argv);
        else
            cli_printCallback("Not A Command\n");
    }
    else
    {
        cli_printCallback("Command Not Found\n");
    }
}

/*** Verify Name Acceptable ***/
PROJECT_STATIC(bool cli_verifyNameAcceptable(const char * const Name, const cli_record_t * const ParentDirectory))
{
    /*** Verify Name Acceptable ***/
    /* Variable */
    size_t i;
    cli_record_t key;
    bool success;
    
    /* Set Up */
    success = true;
    
    /* Illegal Characters In Name */
    for(i = 0; i < strlen(Name); i++)
    {
        if(!isprint(Name[i]) || (Name[i] == ' ') || (Name[i] == '.'))
        {
            success = false;
            break;
        }
    }
    
    /* Not Unique Name (Root Directory Name) */
    if(success && (strcmp(cli_rootDirectoryRecord.Name, Name) == 0))
        success = false;
    
    /* Not Unique Name (Parent Directory Name) */
    if(success && (strcmp(ParentDirectory->Name, Name) == 0))
        success = false;
    
    /* Not Unique Name (Peer Names) */
    key.Name = Name; // Compare Callback Expects Name Within Record
    if(success && (list_find(&(ParentDirectory->entries), &key) != LIST_FIND_NOT_FOUND_INDEX))
        success = false;
    
    /* Exit */
    return success;
}
