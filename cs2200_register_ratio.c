#include <stdio.h>
#include "cs2200.h"
#include "cs2200_register_common.h"
#include "cs2200_register_command.h"
#include "cs2200_register_ratio.h"

static int readRatio(int fd, uint8_t slaveId)
{
    unsigned long ratio;
    int r = cs2200_read_ratio(fd, slaveId, &ratio);
    if (r != OK)
    {
        return r;
    }
    printf("\tRatio : %lu (0x%08lx)\n", ratio, ratio);
    return OK;
}

static int writeRatio(int fd, uint8_t slaveId, char *args[], int argn, int argc)
{
    unsigned long ratio;
    if (argn >= argc || sscanf(args[argn], "%lu", &ratio) != 1)
    {
        fprintf(stderr, "need ratio value\n");
        return ERROR;
    }
    return cs2200_write_ratio(fd, slaveId, ratio);
}

static void help(void)
{
    fprintf(stderr, "%s\n", cs2200_register_ratio.pDescription);
    fputs("\t<32bit ratio value>\n", stderr);
}

const struct CS2200_REGISTER_COMMAND cs2200_register_ratio = {
    "Ratio (Address 06h - 09h)",
    "ratio",
    readRatio,
    writeRatio,
    help,
    {NULL}};
