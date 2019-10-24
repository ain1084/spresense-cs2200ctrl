#include <stdio.h>
#include "cs2200.h"
#include "cs2200_register_common.h"
#include "cs2200_register_command.h"
#include "cs2200_register_device_control.h"

// readonly
static const struct CS2200_REGISTER_FIELD pllUnlockedField =
{
    "Unlock Indicator (Unlock)",
    {
        { NULL, 0, "PLL is Locked"},
        { NULL, 1, "PLL is Unlocked"},
        { NULL, 0, NULL }
    }
};

static const struct CS2200_REGISTER_FIELD auxOutDisabledField =
{
    "Auxiliary Output Disable (AuxOutDis)",
    {
        {"aux_out_enable", 0, "AUX_OUT output driver enabled"},
        {"aux_out_disable", 1, "AUX_OUT output driver disabled"},
        { NULL, 0, NULL }
    }
};

static const struct CS2200_REGISTER_FIELD pllClockOutputDisabledField =
{
    "PLL Clock Output Disable (ClkOutDis)",
    {
        {"clk_out_enable", 0, "CLK_OUT output driver enabled"},
        {"clk_out_disable", 1, "CLK_OUT output driver set to high-impedance"},
        { NULL, 0, NULL }
    }
};

static int readDeviceControl(int fd, uint8_t slaveId)
{
    bool isPllUnlocked;
    bool isAuxOutDisabled;
    bool isPllClockOutputDisabled;
    int r = cs2200_read_device_control(fd, slaveId, &isPllUnlocked, &isAuxOutDisabled, &isPllClockOutputDisabled);
    if (r != OK)
    {
        return r;
    }
    printFieldExplanationWithDescription(isPllUnlocked ? 1 : 0, &pllUnlockedField);
    printFieldExplanationWithDescription(isAuxOutDisabled ? 1 : 0, &auxOutDisabledField);
    printFieldExplanationWithDescription(isPllClockOutputDisabled ? 1 : 0, &pllClockOutputDisabledField);
    return OK;
}

static int writeDeviceControl(int fd, uint8_t slaveId, char *args[], int argn, int argc)
{
    bool isAuxOutDisabled = false;
    bool isPllClockOutputDisabled = false;
    for (; argn < argc; ++argn)
    {
        const char *arg = args[argn];
        const struct CS2200_REGISTER_FIELD_VALUE* pFieldValue = findFieldValueFromArgument(arg, &auxOutDisabledField);
        if (pFieldValue != NULL)
        {
            isAuxOutDisabled = pFieldValue->value != 0;
            continue;
        }
        pFieldValue = findFieldValueFromArgument(arg, &pllClockOutputDisabledField);
        if (pFieldValue != NULL)
        {
            isPllClockOutputDisabled = pFieldValue->value != 0;
            continue;
        }
        fprintf(stderr, "Unknown argument: %s\n", arg);
    }
    return cs2200_write_device_control(fd, slaveId, isAuxOutDisabled, isPllClockOutputDisabled);
}

static void help(void)
{
    showCommonHelp(&cs2200_register_device_control);    
}

const struct CS2200_REGISTER_COMMAND cs2200_register_device_control = {
    "Device Control (Address 02h)",
    "device_control",
    readDeviceControl,
    writeDeviceControl,
    help,
    { &pllUnlockedField, &auxOutDisabledField, &pllClockOutputDisabledField, NULL }
};
