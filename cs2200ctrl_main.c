#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "i2c.h"
#include "cs2200_register_device_control.h"
#include "cs2200_register_device_configuration1.h"
#include "cs2200_register_device_id.h"
#include "cs2200_register_global_configuration.h"
#include "cs2200_register_ratio.h"
#include "cs2200_register_function_configuration1.h"
#include "cs2200_register_function_configuration2.h"
#include "cs2200_set_frequency.h"

enum Operation
{
  opHelp,
  opRead,
  opWrite,
  opUnknown
};

static int readAll(int fd, uint8_t slaveId);
static void helpAll(void);

static const struct CS2200_REGISTER_COMMAND cs2200_register_all =
{
  "Read all registers",
  "all",
  readAll,
  NULL,
  helpAll,
  { NULL }
};

static const struct CS2200_REGISTER_COMMAND* const commands[] =
{
  &cs2200_register_all,
  &cs2200_register_device_id,
  &cs2200_register_device_control,
  &cs2200_register_device_configuration1,
  &cs2200_register_global_configuration,
  &cs2200_register_ratio,
  &cs2200_register_function_configuration1,
  &cs2200_register_function_configuration2,
  &cs2200_set_frequency,
  NULL
};

static bool canWriteCommand(const struct CS2200_REGISTER_COMMAND* pCommand)
{
  return pCommand->pfnWrite != NULL;
}

static bool canReadCommand(const struct CS2200_REGISTER_COMMAND* pCommand)
{
  return pCommand->pfnRead != NULL;
}

static const char* getReadWriteSymbol(const struct CS2200_REGISTER_COMMAND* pCommand)
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

static int readAll(int fd, uint8_t slaveId)
{
  for (const struct CS2200_REGISTER_COMMAND* const* ppCommand = &commands[1]; *ppCommand != NULL; ++ppCommand)
  {
    const struct CS2200_REGISTER_COMMAND* pCommand = *ppCommand;
    if (canReadCommand(pCommand))
    {
      printf("%s\n", pCommand->pDescription);
      pCommand->pfnRead(fd, slaveId);
    }
  }
  return OK;
}

static void helpAll(void)
{
  for (const struct CS2200_REGISTER_COMMAND* const* ppCommand = &commands[1]; *ppCommand != NULL; ++ppCommand)
  {
    (*ppCommand)->pfnHelp();
  }
}

static void showUsage(void)
{
  fputs("Usage: cs2200ctrl <operation> <register name> <slave id> [args...]\n", stderr);
  fputs("Ex.: cs2200ctrl r device_id 4e\n", stderr);
  fputs("Operations:\n", stderr);
  fputs("\t? : help\n", stderr);
  fputs("\tr : read\n", stderr);
  fputs("\tw : write\n", stderr);
  fputs("Registers:\n", stderr);
  for (const struct CS2200_REGISTER_COMMAND* const* ppCommand = commands; *ppCommand != NULL; ++ppCommand)
  {
    const struct CS2200_REGISTER_COMMAND* pCommand = *ppCommand;    
    fprintf(stderr, "\t%s : %s [%s]\n", pCommand->pName, pCommand->pDescription, getReadWriteSymbol(pCommand));
  }
}

static const struct CS2200_REGISTER_COMMAND* findCommand(const char* str)
{
  for (const struct CS2200_REGISTER_COMMAND* const* ppCommand = commands; *ppCommand != NULL; ++ppCommand)
  {
    const struct CS2200_REGISTER_COMMAND* pCommand = *ppCommand;
    if (strcmp(pCommand->pName, str) == 0)
    {
      return pCommand;
    }
  }
  return NULL;
}

static int parseSlaveId(const char* str)
{
  char *pEnd;
  long r = strtol(str, &pEnd, 16);
  if (*pEnd != '\0')
  {
    return -1;
  }
  if (r < 0 || r > 0x7F)
  {
    return -1;
  }
  uint8_t upperMaskValue = (r & 0b1111000) >> 3;
  if (upperMaskValue == 0b0000 || upperMaskValue == 0b1111)
  {
    return -1;
  }
  return r;
}

static enum Operation parseOperation(const char* str)
{
  if (strcmp("?", str) == 0)
  {
    return opHelp;
  }
  else if (strcmp("r", str) == 0)
  {
    return opRead;
  }
  else if (strcmp("w", str) == 0)
  {
    return opWrite;
  }
  else
  {
    return opUnknown;
  }
}

static int executeCommand(enum Operation operation,
 const struct CS2200_REGISTER_COMMAND* pCommand,
  int fd, uint8_t slaveId, char* argv[], int argn, int argc)
{
  int r;
  printf("%s\n", pCommand->pDescription);
  if (operation == opWrite)
  {
    r = pCommand->pfnWrite(fd, slaveId, argv, argn, argc);
    if (r != OK)
    {
      return r;
    }
  }
  if (canReadCommand(pCommand))
  {
    r = pCommand->pfnRead(fd, slaveId);
  }
  return r;
}

int cs2200ctrl_main(int argc, char *argv[])
{
  if (argc < 3)
  {
    showUsage();
    return -1;
  }

  // parse operation
  enum Operation operation = parseOperation(argv[1]);
  if (operation == opUnknown)
  {
    fprintf(stderr, "Unknown operation: %s", argv[1]);
    return -1;
  }

  // parse register (command)
  const struct CS2200_REGISTER_COMMAND* pCommand = findCommand(argv[2]);
  if (pCommand == NULL)
  {
    fprintf(stderr, "Unknown register: %s\n", argv[2]);
    return -1;
  }

  // check operation supported
  if ((operation == opRead && !canReadCommand(pCommand)) || (operation == opWrite && !canWriteCommand(pCommand)))
  {
    fprintf(stderr, "Oeration is not supported (Register: %s).\n", pCommand->pName);
    return -1;
  }

  // show help
  if (operation == opHelp)
  {
    pCommand->pfnHelp();
    return 0;
  }

  // parse slave id
  if (argc < 4)
  {
    fputs("Slave ID is required.\n", stderr);
    return -1;
  }
  int slaveId = parseSlaveId(argv[3]);
  if (slaveId < 0)
  {
    fprintf(stderr, "Invalid slave ID: %s.\n", argv[3]);
    return -1;
  }

  // open i2c device
  int fd = i2c_open(0);
  if (fd < 0)
  {
    fputs("I2C initialization failed.\n", stderr);
    return -1;
  }

  // execute r/w
  int argn = 4;
  int r = executeCommand(operation, pCommand, fd, (uint8_t)slaveId, argv, argn, argc);
  if (r != OK)
  {
    fprintf(stderr, "Failed (result = %d)\n", r);
  }
  else
  {
    fputs("Succeeded.\n", stderr);
  }
  i2c_close(fd);
  return 0;
}
