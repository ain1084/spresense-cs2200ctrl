#include <stdio.h>
#include "cs2200_register_common.h"
#include "cs2200_register_command.h"
#include "cs2200_register_all.h"

struct ReadAllContext
{
    int fd;
    uint8_t slaveId;
};

static bool readAllEachItem(void* pContext, const struct CS2200_REGISTER_COMMAND* pCommand)
{
    struct ReadAllContext* rc = (struct ReadAllContext*)pContext;
    if (canReadCommand(pCommand))
    {
      printf("%s\n", pCommand->pDescription);
      int r = pCommand->pfnRead(rc->fd, rc->slaveId);
      if (r != OK)
      {
          printf("\tFailed (result = %d).\n", r);
      }
    }
    return true;
}

static int readAll(int fd, uint8_t slaveId)
{
    struct ReadAllContext rc;
    rc.fd = fd;
    rc.slaveId = slaveId;
    enumCommands(1, &rc, readAllEachItem);
    return OK;
}

static bool helpAllEachItem(void* pContext, const struct CS2200_REGISTER_COMMAND* pCommand)
{
    pCommand->pfnHelp();
    return true;
}

static void helpAll(void)
{
    enumCommands(1, NULL, helpAllEachItem);
}

const struct CS2200_REGISTER_COMMAND cs2200_register_all =
{
    "Read all registers",
    "all",
    readAll,
    NULL,
    helpAll,
    { NULL }
};
