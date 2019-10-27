#include <stdio.h>
#include <string.h>
#include "cs2200_register_all.h"
#include "cs2200_register_device_control.h"
#include "cs2200_register_device_configuration1.h"
#include "cs2200_register_device_id.h"
#include "cs2200_register_global_configuration.h"
#include "cs2200_register_ratio.h"
#include "cs2200_register_function_configuration1.h"
#include "cs2200_register_function_configuration2.h"
#include "cs2200_set_frequency.h"
#include "cs2200_register_common.h"
#include "cs2200_register_command.h"

static const struct CS2200_REGISTER_COMMAND *const commands[] =
{
    &cs2200_register_all,
    &cs2200_register_device_id,
    &cs2200_register_device_control,
    &cs2200_register_device_configuration1,
    &cs2200_register_global_configuration,
    &cs2200_register_ratio,
    &cs2200_set_frequency,
    &cs2200_register_function_configuration1,
    &cs2200_register_function_configuration2,
};

const struct CS2200_REGISTER_COMMAND* enumCommands(int startIndex, void* pContext, bool (*pfnCallback)(void* pContext, const struct CS2200_REGISTER_COMMAND* pCommand))
{
    for (int i = startIndex; i < sizeof(commands) / sizeof(commands[0]); ++i)
    {
        const struct CS2200_REGISTER_COMMAND* pCommand = commands[i];
        if (!pfnCallback(pContext, pCommand))
        {
            return pCommand;
        }
    }
    return NULL;
}

bool canWriteCommand(const struct CS2200_REGISTER_COMMAND *pCommand)
{
    return pCommand->pfnWrite != NULL;
}

bool canReadCommand(const struct CS2200_REGISTER_COMMAND *pCommand)
{
    return pCommand->pfnRead != NULL;
}

const char* getReadWriteSymbol(const struct CS2200_REGISTER_COMMAND* pCommand)
{
    if (canWriteCommand(pCommand) && canReadCommand(pCommand))
    {
        return "R/W";
    }
    else if (!canReadCommand(pCommand))
    {
        return "W only";
    }
    else
    {
        return "R only";
    }
}

static bool findCommandEachItem(void* pContext, const struct CS2200_REGISTER_COMMAND* pCommand)
{
    return strcmp(pCommand->pName, (const char*)pContext) != 0;
}

const struct CS2200_REGISTER_COMMAND* findCommand(const char* str)
{
    return enumCommands(0, (void*)str, findCommandEachItem);
}

int executeCommand(enum Operation operation, const struct CS2200_REGISTER_COMMAND* pCommand, int fd, uint8_t slaveId, char* argv[], int argn, int argc)
{
    int r;
    printf("%s\n", pCommand->pDescription);
    if (operation == opWrite)
    {
        r = pCommand->pfnWrite(fd, slaveId, argv, argn, argc);
    }
    else
    {
        r = pCommand->pfnRead(fd, slaveId);
    }
    return r;
}

static bool showUsageEachItem(void* pContext, const struct CS2200_REGISTER_COMMAND* pCommand)
{
    fprintf(stderr, "\t%s : %s [%s]\n", pCommand->pName, pCommand->pDescription, getReadWriteSymbol(pCommand));
    return true;
}

void showUsage(void)
{
    fputs("Usage: cs2200ctrl <operation> <register name> <slave id> [args...]\n", stderr);
    fputs("Ex.: cs2200ctrl r device_id 4e\n", stderr);
    fputs("Operations:\n", stderr);
    fputs("\t? : help\n", stderr);
    fputs("\tr : read\n", stderr);
    fputs("\tw : write\n", stderr);
    fputs("Registers:\n", stderr);
    enumCommands(0, NULL, showUsageEachItem);
}
