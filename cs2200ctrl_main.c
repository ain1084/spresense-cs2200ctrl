#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "i2c.h"
#include "cs2200_register_common.h"
#include "cs2200_register_command.h"

static int parseSlaveId(const char *str)
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

static enum Operation parseOperation(const char *str)
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
    const struct CS2200_REGISTER_COMMAND *pCommand = findCommand(argv[2]);
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
