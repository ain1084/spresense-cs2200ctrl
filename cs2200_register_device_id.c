#include <stdio.h>
#include "cs2200.h"
#include "cs2200_register_common.h"
#include "cs2200_register_command.h"
#include "cs2200_register_device_id.h"

// readonly
static const struct CS2200_REGISTER_FIELD deviceIdField = {
    "Device Identification (Device[4:0])",
    {
        {NULL, (int)DEVICE_ID_CS2200, "CS2200"},
        {NULL, 0, NULL}
    }
};

// readonly
static const struct CS2200_REGISTER_FIELD revisionField = {
    "Device Revision (Revision[2:0])",
    {
        {NULL, (int)REVISION_B2_B3, "B2 and B3"},
        {NULL, (int)REVISION_C1, "C1"},
        {NULL, 0, NULL}
    }
};

static int readDeviceId(int fd, uint8_t slaveId)
{
    enum CS2200_DEVICE_ID deviceId;
    enum CS2200_REVISION revision;
    int r = cs2200_read_device_id(fd, slaveId, &deviceId, &revision);
    if (r != OK)
    {
        return r;
    }
    printFieldExplanationWithDescription(deviceId, &deviceIdField);
    printFieldExplanationWithDescription(revision, &revisionField);
    return OK;
}

static void help(void)
{
    showCommonHelp(&cs2200_register_device_id);    
}

const struct CS2200_REGISTER_COMMAND cs2200_register_device_id = {
    "Device I.D. and Revision (Address 01h)",
    "device_id",
    readDeviceId,
    NULL,
    help,
    { NULL }
};
