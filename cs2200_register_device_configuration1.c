#include <stdio.h>
#include "cs2200.h"
#include "cs2200_register_common.h"
#include "cs2200_register_device_configuration1.h"

static const struct CS2200_REGISTER_FIELD rmodSelectionField = {
    "R-Mod Selection (RModSel[2:0])",
    {
        {"left0", (int)R_MOD_LEFT_SHIFT_BY0, "Left-shift R-value by 0 (x 1)"},
        {"left1", (int)R_MOD_LEFT_SHIFT_BY1, "Left-shift R-value by 1 (x 2)"},
        {"left2", (int)R_MOD_LEFT_SHIFT_BY2, "Left-shift R-value by 2 (x 4)"},
        {"left3", (int)R_MOD_LEFT_SHIFT_BY3, "Left-shift R-value by 3 (x 8)"},
        {"righ1", (int)R_MOD_RIGHT_SHIFT_BY1, "Right-shift R-value by 1 (1/2)"},
        {"righ2", (int)R_MOD_RIGHT_SHIFT_BY2, "Right-shift R-value by 1 (1/4)"},
        {"righ3", (int)R_MOD_RIGHT_SHIFT_BY3, "Right-shift R-value by 1 (1/8)"},
        {"righ4", (int)R_MOD_RIGHT_SHIFT_BY4, "Right-shift R-value by 1 (1/16)"},
        {NULL, 0, NULL}
    }
};

static const struct CS2200_REGISTER_FIELD auxOutSourceField = {
    "Auxiliary Output Source Selection (AuxOutSrc[1:0])",
    {
        {"refclk", (int)AUX_OUT_REFCLK, "RefClk"},
        {"reserved", (int)AUX_OUT_RESERVED, "Reserved"},
        {"clk_out", (int)AUX_OUT_CLK_OUT, "CLK_OUT"},
        {"pll_lock", (int)AUX_OUT_PLL_LOCK_STATUS_INDICATOR, "PLL Lock Status Indicator"},
        {NULL, 0, NULL}
    }
};

static const struct CS2200_REGISTER_FIELD enabledDeviceConfigurationField = {
    "Enable Device Configuration Register 1 (EnDevCfg1)",
    {
        { "enable", 1, "Enabled" },
        { "disable", 0, "Disabled" },
        { NULL, 0, NULL}
    }
};

static int readDeviceConfiguration1(int fd, uint8_t slaveId)
{
    enum CS2200_R_MOD_SELECTION rmodSelection;
    enum CS2200_AUX_OUT_SOURCE auxOutSource;
    bool isEnabledDeviceConfiguration1;
    int r = cs2200_read_device_configuration1(fd, slaveId, &rmodSelection, &auxOutSource, &isEnabledDeviceConfiguration1);
    if (r != OK)
    {
        return r;
    }
    printFieldExplanationWithDescription(rmodSelection, &rmodSelectionField);
    printFieldExplanationWithDescription(auxOutSource, &auxOutSourceField);
    printFieldExplanationWithDescription(isEnabledDeviceConfiguration1 ? 1 : 0, &enabledDeviceConfigurationField);
    return OK;
}

static int writeDeviceConfiguration1(int fd, uint8_t slaveId, char *args[], int argn, int argc)
{
    enum CS2200_R_MOD_SELECTION rmodSelection = R_MOD_LEFT_SHIFT_BY0;
    enum CS2200_AUX_OUT_SOURCE auxOutSource = AUX_OUT_REFCLK;
    bool isEnabledDeviceConfiguration1 = false;
    for (; argn < argc; ++argn)
    {
        const char* arg = args[argn];
        const struct CS2200_REGISTER_FIELD_VALUE* pFieldValue = findFieldValueFromArgument(arg, &rmodSelectionField);
        if (pFieldValue != NULL)
        {
            rmodSelection = (enum CS2200_R_MOD_SELECTION)pFieldValue->value;
            continue;
        }
        pFieldValue = findFieldValueFromArgument(arg, &auxOutSourceField);
        if (pFieldValue != NULL)
        {
            auxOutSource = (enum CS2200_AUX_OUT_SOURCE)pFieldValue->value;
            continue;
        }
        pFieldValue = findFieldValueFromArgument(arg, &enabledDeviceConfigurationField);
        if (pFieldValue != NULL)
        {
            isEnabledDeviceConfiguration1 = pFieldValue->value != 0;
            continue;
        }
        fprintf(stderr, "Unknown argument: %s\n", arg);
    }
    return cs2200_write_device_configuration1(fd, slaveId, rmodSelection, auxOutSource, isEnabledDeviceConfiguration1);
}

static void help(void)
{
    showCommonHelp(&cs2200_register_device_configuration1);    
}

const struct CS2200_REGISTER_COMMAND cs2200_register_device_configuration1 = {
    "Device Configuration 1 (Address 03h)",
    "device_configuration1",
    readDeviceConfiguration1,
    writeDeviceConfiguration1,
    help,
    { &rmodSelectionField, &auxOutSourceField, &enabledDeviceConfigurationField, NULL }
};
