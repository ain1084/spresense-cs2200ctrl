#include <stdio.h>
#include "cs2200.h"
#include "cs2200_register_common.h"
#include "cs2200_register_global_configuration.h"

static const struct CS2200_REGISTER_FIELD freezeField = {
    "Device Configuration Freeze (Freeze)",
    {
        {"unfreeze", (int)0, "Register changes take effect immediately"},
        {"freeze", (int)1, "Freeze"},
        {NULL, 0, NULL}
    }
};

static const struct CS2200_REGISTER_FIELD enabledDeviceConfiguration2Field = {
    "Enable Device Configuration Register 2 (EnDevCfg2)",
    {
        {"disable", (int)0, "Disabled"},
        {"enable", (int)1, "Enabled"},
        {NULL, 0, NULL}
    }
};

static int readGlobalConfiguration(int fd, uint8_t slaveId)
{
    int r;
    bool isFreeze;
    bool isEnabledDeviceConfiguration2;

    r = cs2200_read_global_configuration(fd, slaveId, &isFreeze, &isEnabledDeviceConfiguration2);
    if (r != OK)
    {
        return r;
    }
    printFieldExplanationWithDescription(isFreeze ? 1 : 0, &freezeField);
    printFieldExplanationWithDescription(isEnabledDeviceConfiguration2 ? 1 : 0, &enabledDeviceConfiguration2Field);
    return OK;
}

static int writeGlobalConfiguration(int fd, uint8_t slaveId, char *args[], int argn, int argc)
{
    bool isFreeze = false;
    bool isEnabledDeviceConfiguration2 = false;
    for (; argn < argc; ++argn)
    {
        const char* arg = args[argn];
        const struct CS2200_REGISTER_FIELD_VALUE* pFieldValue = findFieldValueFromArgument(arg, &freezeField);
        if (pFieldValue != NULL)
        {
            isFreeze = pFieldValue->value != 0;
            continue;
        }
        pFieldValue = findFieldValueFromArgument(arg, &enabledDeviceConfiguration2Field);
        if (pFieldValue != NULL)
        {
            isEnabledDeviceConfiguration2 = pFieldValue->value != 0;
            continue;
        }
        fprintf(stderr, "Unknown argument: %s\n", arg);
    }
    return cs2200_write_global_configuration(fd, slaveId, isFreeze, isEnabledDeviceConfiguration2);
}

static void help(void)
{
    showCommonHelp(&cs2200_register_global_configuration);    
}

const struct CS2200_REGISTER_COMMAND cs2200_register_global_configuration = {
    "Global Configuration (Address 05h)",
    "global_configuration",
    readGlobalConfiguration,
    writeGlobalConfiguration,
    help,
    { &freezeField, &enabledDeviceConfiguration2Field, NULL }
};
