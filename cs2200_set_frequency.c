#include <stdio.h>
#include "cs2200.h"
#include "cs2200_register_common.h"
#include "cs2200_register_command.h"
#include "cs2200_set_frequency.h"

static int writeFrequency(int fd, uint8_t slaveId, char* args[], int argn, int argc)
{
    unsigned long input;
    unsigned long output;

    if (argn >= argc || sscanf(args[argn++], "%lu", &input) != 1)
    {
        fprintf(stderr, "invalid input frequency.\n");
        return ERROR;
    }
    if (argn >= argc || sscanf(args[argn], "%lu", &output) != 1 || output == 0)
    {
        fprintf(stderr, "invalid output frequency.\n");
        return ERROR;
    }
    unsigned long ratio = (unsigned long)(((uint64_t)output << 20) / input);
    printf("Ratio value = %lu (0x%08lx)\n", ratio, ratio);
    return cs2200_write_ratio(fd, slaveId, ratio);
}

static void help(void)
{
    fprintf(stderr, "%s\n", cs2200_set_frequency.pDescription);
    fputs("\t<Input frequency Hz> <Output frequency Hz>\n", stderr);
}

const struct CS2200_REGISTER_COMMAND cs2200_set_frequency = {
    "Frequency (Address 06h - 09h)",
    "frequency",
    NULL,
    writeFrequency,
    help,
    { NULL }
};

