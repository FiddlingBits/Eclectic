/****************************************************************************************************
 * Pragma
 ****************************************************************************************************/

#pragma once

/****************************************************************************************************
 * Define
 ****************************************************************************************************/

#define CLI_COMMAND_CHANGE_DIRECTORY_NAME ("cd")
#define CLI_COMMAND_LIST_NAME             ("ls")
#define CLI_DIRECTORY_ROOT_NAME           ("root")
#define CLI_INPUT_MAXIMUM_ARGUMENTS       (10)
#define CLI_INPUT_MAXIMUM_LENGTH          (256)
#define CLI_LIST_INDENT_SIZE              (2)
#define CLI_OUTPUT_ANCESTRY_MAXIMUM_DEPTH (5)
#define CLI_OUTPUT_COMMAND_PROMPT         ("$")
#define CLI_PARENT_DIRECTORY_ROOT         (NULL)

/****************************************************************************************************
 * Include
 ****************************************************************************************************/

#include "Eclectic/Data/list.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/****************************************************************************************************
 * Type Definition
 ****************************************************************************************************/

typedef struct cli_argumentOptionPair_s
{
    char *argument;
    char *option;
} cli_argumentOptionPair_t;

typedef void (*cli_commandHandlerCallback_t)(size_t argc, char *argv[]);
typedef void (*cli_printCallback_t)(const char * const Format, ...);
typedef void (*cli_processInputCallback_t)(void);

typedef enum cli_recordType_e
{
    CLI_RECORD_TYPE_COMMAND,
    CLI_RECORD_TYPE_DIRECTORY
} cli_recordType_t;

typedef struct cli_record_s
{
    union
    {
        list_list_t entries;
        cli_commandHandlerCallback_t handler;
    };
    const char *Name;
    const struct cli_record_s *Parent;
    cli_recordType_t type;
} cli_record_t;

/****************************************************************************************************
 * Function Prototype
 ****************************************************************************************************/

extern bool cli_addReceiveCharacter(const char Character);
extern void cli_deinit(void);
extern bool cli_getArgumentOptionPair(char * const input, cli_argumentOptionPair_t * const argumentOptionPair);
extern bool cli_getS32(const char * const Input, int32_t * const s32);
extern bool cli_getU32(const char * const Input, uint32_t * const u32);
extern bool cli_init(const cli_printCallback_t PrintCallback, const cli_processInputCallback_t ProcessInputCallback);
extern bool cli_processInput(void);
extern bool cli_registerCommand(cli_record_t * const record, const char * const Name, cli_record_t *parentDirectory, const cli_commandHandlerCallback_t CommandHandlerCallback);
extern bool cli_registerDirectory(cli_record_t * const record, const char * const Name, cli_record_t *parentDirectory);
