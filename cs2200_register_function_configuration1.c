#include <stdio.h>
#include "cs2200.h"
#include "cs2200_register_common.h"
#include "cs2200_register_function_configuration1.h"

static const struct CS2200_REGISTER_FIELD auxLockCfgField = {
    "AUX PLL Lock Output Configuration (AuxLockCfg)",
    {
        {"push_pull", (int)AUX_LOCK_OUT_PUSH_PULL, "Push-Pull. Active High"},
        {"open_drain", (int)AUX_LOCK_OUT_OPEN_DRAIN, "Open Dragin. Active Low"},
        {NULL, 0, NULL}
    }
};

static const struct CS2200_REGISTER_FIELD refClkDivField = {
    "Reference Clock Input Divider (RefClkDiv[1:0])",
    {
        {"divide4", (int)REF_CLK_DIV_BY4, "1/4 32 MHz to 56 MHz (50 MHz with XTI)"},
        {"divide2", (int)REF_CLK_DIV_BY2, "1/2 16 MHz to 28 MHz"},
        {"divide1", (int)REF_CLK_DIV_BY1, "1/1 8 MHz to 14 MHz"},
        {NULL, 0, NULL}
    }
};

static int readFunctionConfiguration1(int fd, uint8_t slaveId)
{
    enum CS2200_AUX_LOCK_CFG auxLockCfg;
    enum CS2200_REF_CLK_DIV refClkDiv;
    int r = cs2200_read_function_configuration1(fd, slaveId, &auxLockCfg, &refClkDiv);
    if (r != OK)
    {
        return r;
    }
    printFieldExplanationWithDescription(auxLockCfg, &auxLockCfgField);
    printFieldExplanationWithDescription(refClkDiv, &refClkDivField);
    return OK;
}

static int writeFunctionConfiguration1(int fd, uint8_t slaveId, char *args[], int argn, int argc)
{
    enum CS2200_AUX_LOCK_CFG auxLockCfg = AUX_LOCK_OUT_PUSH_PULL;
    enum CS2200_REF_CLK_DIV refClkDiv = REF_CLK_DIV_BY4;
    for (; argn < argc; ++argn)
    {
        const char* arg = args[argn];
        const struct CS2200_REGISTER_FIELD_VALUE* pFieldValue = findFieldValueFromArgument(arg, &auxLockCfgField);
        if (pFieldValue != NULL)
        {
            auxLockCfg = (enum CS2200_AUX_LOCK_CFG)pFieldValue->value;
            continue;
        }
        pFieldValue = findFieldValueFromArgument(arg, &refClkDivField);
        if (pFieldValue != NULL)
        {
            refClkDiv = (enum CS2200_REF_CLK_DIV)pFieldValue->value;
            continue;
        }
        fprintf(stderr, "Unknown argument: %s\n", arg);
    }
    return cs2200_write_function_configuration1(fd, slaveId, auxLockCfg, refClkDiv);
}

static void help(void)
{
    showCommonHelp(&cs2200_register_function_configuration1);    
}

const struct CS2200_REGISTER_COMMAND cs2200_register_function_configuration1 = {
    "Function Configuration 1 (Address 16h)",
    "function_configuration1",
    readFunctionConfiguration1,
    writeFunctionConfiguration1,
    help,
    { &auxLockCfgField, &refClkDivField, NULL }
};
