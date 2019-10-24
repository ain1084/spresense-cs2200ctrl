#include <stdio.h>
#include "cs2200.h"
#include "cs2200_register_common.h"
#include "cs2200_register_function_configuration2.h"

static const struct CS2200_REGISTER_FIELD pllClockOutputOnUnlockField = {
    "Enable PLL Clock Output on Unlock (ClkOutUnl)",
    {
        {"clk_out_low", (int)PLL_CLK_OUT_LOW_WHEN_UNLOCKED, "Clock outputs are driven \"low\" when PLL is unlocked"},
        {"clk_out_always", (int)PLL_CLK_OUT_ALWAYS_ENABLED, "Clock outputs are always enabled"},
        {NULL, 0, NULL}
    }
};

static int readFunctionConfiguration2(int fd, uint8_t slaveId)
{
    enum CS2200_PLL_CLK_OUTPUT_ON_UNLOCK pllClockOutputOnUnlock;
    int r = cs2200_read_function_configuration2(fd, slaveId, &pllClockOutputOnUnlock);
    if (r != OK)
    {
        return r;
    }
    printFieldExplanationWithDescription(pllClockOutputOnUnlock, &pllClockOutputOnUnlockField);
    return OK;
}

static int writeFunctionConfiguration2(int fd, uint8_t slaveId, char *args[], int argn, int argc)
{
    enum CS2200_PLL_CLK_OUTPUT_ON_UNLOCK pllClockOutputOnUnlock = PLL_CLK_OUT_LOW_WHEN_UNLOCKED;
    for (; argn < argc; ++argn)
    {
        const char* arg = args[argn];
        const struct CS2200_REGISTER_FIELD_VALUE* pFieldValue = findFieldValueFromArgument(arg, &pllClockOutputOnUnlockField);
        if (pFieldValue != NULL)
        {
            pllClockOutputOnUnlock = (enum CS2200_PLL_CLK_OUTPUT_ON_UNLOCK)pFieldValue->value;
            continue;
        }
        fprintf(stderr, "Unknown argument: %s\n", arg);
    }
    return cs2200_write_function_configuration2(fd, slaveId, pllClockOutputOnUnlock);
}

static void help(void)
{
    showCommonHelp(&cs2200_register_function_configuration2);    
}

const struct CS2200_REGISTER_COMMAND cs2200_register_function_configuration2 = {
    "Function Configuration 2 (Address 17h)",
    "function_configuration2",
    readFunctionConfiguration2,
    writeFunctionConfiguration2,
    help,
    { &pllClockOutputOnUnlockField, NULL }
};
