#include <stdio.h>
#include <stdlib.h>
#include "cs2200.h"
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

static const struct CS2200_REGISTER_FIELD ratioField = {
    "Frequency",
    {
        {"<32bit value>", 0, "Input Frequency (0 - 4294967295)Hz"},
        {"<32bit value>", 0, "Output Frequency (1 - 4294967295)Hz"},
        {NULL, 0, NULL}
    }
};

static void help(void)
{
    showCommonHelp(&cs2200_set_frequency);    
}

const struct CS2200_REGISTER_COMMAND cs2200_set_frequency = {
    "Frequency (Address 06h - 09h)",
    "frequency",
    NULL,
    writeFrequency,
    help,
    { &ratioField, NULL }
};

