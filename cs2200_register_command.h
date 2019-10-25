#pragma once

#include <stdbool.h>

enum Operation
{
  opHelp,
  opRead,
  opWrite,
  opUnknown
};

struct CS2200_REGISTER_COMMAND;

const struct CS2200_REGISTER_COMMAND* enumCommands(int startIndex, void* pContext, bool (*pfnCallback)(void* pContext, const struct CS2200_REGISTER_COMMAND* pCommand));
bool canWriteCommand(const struct CS2200_REGISTER_COMMAND* pCommand);
bool canReadCommand(const struct CS2200_REGISTER_COMMAND* pCommand);
const char* getReadWriteSymbol(const struct CS2200_REGISTER_COMMAND* pCommand);
int executeCommand(enum Operation operation, const struct CS2200_REGISTER_COMMAND* pCommand, int fd, uint8_t slaveId, char* argv[], int argn, int argc);
const struct CS2200_REGISTER_COMMAND* findCommand(const char* str);
void showUsage(void);
