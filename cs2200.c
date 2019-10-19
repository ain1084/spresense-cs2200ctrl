#include <sdk/config.h>
#include <string.h>
#include "i2c.h"
#include "cs2200.h"

//#define TEST_I2C

#define CS2200_REG_INCR 0x80

#define CS2200_REG_DEVICE_ID 0x01
union CS2200_REG_DEVICE_ID_FIELD
{
    uint8_t Value;
    struct
    {
        uint8_t Revision : 3;
        uint8_t DeviceId : 5;
    };
};

#define CS2200_REG_DEVICE_CONTROL 0x02
union CS2200_REG_DEVICE_CONTROL_FIELD
{
    uint8_t Value;
    struct
    {
        uint8_t ClkOutDis : 1;
        uint8_t AuxOutDis : 1;
        uint8_t reserved : 5;
        uint8_t Unlock : 1;
    };
};

#define CS2200_REG_DEVICE_CONFIGURATION1 0x03
union CS2200_REG_DEVICE_CONFIGURATION1_FIELD
{
    uint8_t Value;
    struct
    {
        uint8_t EnDevCfg1 : 1;
        uint8_t AuxOutSrc : 2;
        uint8_t reserved : 2;
        uint8_t RModSel : 3;
    };
};

#define CS2200_REG_GLOBAL_CONFIGURATION 0x05
union CS2200_REG_GLOBAL_CONFIGURATION_FIELD {
    uint8_t Value;
    struct
    {
        uint8_t EnDevCfg2 : 1;
        uint8_t reserved1 : 2;
        uint8_t Freeze : 1;
        uint8_t reserved2 : 4;
    };
};

#define CS2200_REG_RATIO 0x06

#define CS2200_REG_FUNCTION_CONFIGURATION1 0x16
union CS2200_REG_FUNCTION_CONFIGURATION1_FIELD
{
    uint8_t Value;
    struct
    {
        uint8_t reserved1 : 3;
        uint8_t RefClkDiv : 2;
        uint8_t reserved2 : 1;
        uint8_t AuxLockCfg : 1;
        uint8_t reserved3 : 1;
    };
};

#define CS2200_REG_FUNCTION_CONFIGURATION2 0x17
union CS2200_REG_FUNCTION_CONFIGURATION2_FIELD
{
    uint8_t Value;
    struct
    {
        uint8_t reserved1 : 4;
        uint8_t ClkOutUnl : 1;
        uint8_t reserved2 : 3;
    };
};

#if defined(TEST_I2C)

static uint8_t cs2200_register[32];

#endif

static int cs2200_read_register(int fd, uint8_t deviceAddr, uint8_t regAddr, uint8_t* pData, ssize_t length)
{
#if defined(TEST_I2C)
    int index = regAddr & 0x7F;
    if ((regAddr & 0x80) != 0)
    {
        memcpy(pData, &cs2200_register[index], length);
    }
    else
    {
        memset(pData, cs2200_register[index], length);
    }
    return 0;
#else
    int r = i2c_write_data(fd, deviceAddr, &regAddr, sizeof(regAddr));
    if (r != OK)
    {
        return r;
    }
    return i2c_read_data(fd, deviceAddr, pData, length);
#endif
}

static int cs2200_write_register(int fd, uint8_t deviceAddr, uint8_t regAddr, const uint8_t* pData, ssize_t length)
{
#if defined(TEST_I2C)
    int index = regAddr & 0x7F;
    if ((regAddr & 0x80) != 0)
    {
        memcpy(&cs2200_register[index], pData, length);
    }
    else
    {
        memset(&cs2200_register[index], *pData, length);
    }
    return 0;
#else
    uint8_t buffer[5];
    buffer[0] = regAddr;
    memcpy(&buffer[1], pData, length);
    return i2c_write_data(fd, deviceAddr, buffer, length + 1);
#endif
}

int cs2200_read_device_id(int fd, uint8_t deviceAddr, enum CS2200_DEVICE_ID *pId, enum CS2200_REVISION *pRevision)
{
    union CS2200_REG_DEVICE_ID_FIELD field;
    int ret = cs2200_read_register(fd, deviceAddr, CS2200_REG_DEVICE_ID, &field.Value, sizeof(field.Value));
    if (ret != OK)
    {
        return ret;
    }
    if (pId != NULL)
    {
        *pId = (enum CS2200_DEVICE_ID)field.DeviceId;
    }
    if (pRevision != NULL)
    {
        *pRevision = (enum CS2200_REVISION)field.Revision;
    }
    return OK;
}

int cs2200_write_device_control(int fd, uint8_t deviceAddr, bool isAuxOutDisabled, bool isPllClockOutputDisabled)
{
    union CS2200_REG_DEVICE_CONTROL_FIELD field;
    field.reserved = 0;
    field.Unlock = 0;
    field.AuxOutDis = isAuxOutDisabled ? 1 : 0;
    field.ClkOutDis = isPllClockOutputDisabled ? 1 : 0;
    return cs2200_write_register(fd, deviceAddr, CS2200_REG_DEVICE_CONTROL, &field.Value, sizeof(field.Value));
}

int cs2200_read_device_control(int fd, uint8_t deviceAddr, bool* pIsPllUnlocked, bool* pIsAuxOutDisabled, bool* pIsPllClockOutputDisabled)
{
    union CS2200_REG_DEVICE_CONTROL_FIELD field;
    int ret = cs2200_read_register(fd, deviceAddr, CS2200_REG_DEVICE_CONTROL, &field.Value, sizeof(field.Value));
    if (ret != OK)
    {
        return ret;
    }
    if (pIsPllUnlocked != NULL)
    {
        *pIsPllUnlocked = (field.Unlock == 1);
    }
    if (pIsAuxOutDisabled != NULL)
    {
        *pIsAuxOutDisabled = (field.AuxOutDis == 1);
    }
    if (pIsPllClockOutputDisabled != NULL)
    {
        *pIsPllClockOutputDisabled = (field.ClkOutDis == 1);
    }
    return OK;
}

int cs2200_write_device_configuration1(int fd, uint8_t deviceAddr, enum CS2200_R_MOD_SELECTION rmodSel, enum CS2200_AUX_OUT_SOURCE auxOutSource, bool isEnableDeviceConfiguration1)
{
    union CS2200_REG_DEVICE_CONFIGURATION1_FIELD field;
    field.reserved = 0;
    field.RModSel = (uint8_t)rmodSel;
    field.AuxOutSrc = (uint8_t)auxOutSource;
    field.EnDevCfg1 = (uint8_t)(isEnableDeviceConfiguration1 ? 1 : 0);
    return cs2200_write_register(fd, deviceAddr, CS2200_REG_DEVICE_CONFIGURATION1, &field.Value, sizeof(field.Value));
}

int cs2200_read_device_configuration1(int fd, uint8_t deviceAddr, enum CS2200_R_MOD_SELECTION *pRmodSel, enum CS2200_AUX_OUT_SOURCE *pAuxOutSource, bool *pIsEnabledDeviceConfiguration1)
{
    union CS2200_REG_DEVICE_CONFIGURATION1_FIELD field;
    int r = cs2200_read_register(fd, deviceAddr, CS2200_REG_DEVICE_CONFIGURATION1, &field.Value, sizeof(field.Value));
    if (r != OK)
    {
        return r;
    }
    if (pRmodSel != NULL)
    {
        *pRmodSel = (enum CS2200_R_MOD_SELECTION)field.RModSel;
    }
    if (pAuxOutSource != NULL)
    {
        *pAuxOutSource = (enum CS2200_AUX_OUT_SOURCE)field.AuxOutSrc;
    }
    if (pIsEnabledDeviceConfiguration1 != NULL)
    {
        *pIsEnabledDeviceConfiguration1 = (field.EnDevCfg1 != 0);
    }
    return OK;
}

int cs2200_write_ratio(int fd, uint8_t deviceAddr, unsigned long ratio)
{
    uint8_t buffer[4];
    buffer[0] = (uint8_t)(ratio >> 24);
    buffer[1] = (uint8_t)(ratio >> 16);
    buffer[2] = (uint8_t)(ratio >> 8);
    buffer[3] = (uint8_t)(ratio);
    return cs2200_write_register(fd, deviceAddr, CS2200_REG_RATIO | CS2200_REG_INCR, buffer, sizeof(buffer));
}

int cs2200_read_ratio(int fd, uint8_t deviceAddr, unsigned long *pRatio)
{
    uint8_t buffer[4];
    int r = cs2200_read_register(fd, deviceAddr, CS2200_REG_RATIO | CS2200_REG_INCR, buffer, sizeof(buffer));
    if (r != OK)
    {
        return r;
    }
    *pRatio = ((unsigned long)buffer[0] << 24) | ((unsigned long)buffer[1] << 16) | ((unsigned long)buffer[2] << 8) | buffer[3];
    return OK;
}

int cs2200_write_global_configuration(int fd, uint8_t deviceAddr, bool isFreeze, bool isEnabledDeviceConfiguration2)
{
    union CS2200_REG_GLOBAL_CONFIGURATION_FIELD field;
    field.reserved1 = 0;
    field.reserved2 = 0;
    field.Freeze = (uint8_t)(isFreeze ? 1 : 0);
    field.EnDevCfg2 = (uint8_t)(isEnabledDeviceConfiguration2 ? 1 : 0);
    return cs2200_write_register(fd, deviceAddr, CS2200_REG_GLOBAL_CONFIGURATION, &field.Value, sizeof(field.Value));
}

int cs2200_read_global_configuration(int fd, uint8_t deviceAddr, bool* pIsFreeze, bool *pIsEnabledDeviceConfiguration2)
{
    union CS2200_REG_GLOBAL_CONFIGURATION_FIELD field;
    int r = cs2200_read_register(fd, deviceAddr, CS2200_REG_GLOBAL_CONFIGURATION, &field.Value, sizeof(field.Value));
    if (r != OK)
    {
        return r;
    }
    if (pIsFreeze != NULL)
    {
        *pIsFreeze = (field.Freeze != 0);
    }
    if (pIsEnabledDeviceConfiguration2 != NULL)
    {
        *pIsEnabledDeviceConfiguration2 = (field.EnDevCfg2 != 0);
    }
    return OK;
}

int cs2200_write_function_configuration1(int fd, uint8_t deviceAddr, enum CS2200_AUX_LOCK_CFG auxLockCfg, enum CS2200_REF_CLK_DIV refClockDiv)
{
    union CS2200_REG_FUNCTION_CONFIGURATION1_FIELD field;
    field.reserved1 = 0;
    field.reserved2 = 0;
    field.reserved3 = 0;
    field.AuxLockCfg = (uint8_t)auxLockCfg;
    field.RefClkDiv = (uint8_t)refClockDiv;
    return cs2200_write_register(fd, deviceAddr, CS2200_REG_FUNCTION_CONFIGURATION1, &field.Value, sizeof(field.Value));
}

int cs2200_read_function_configuration1(int fd, uint8_t deviceAddr, enum CS2200_AUX_LOCK_CFG *pAuxLockCfg, enum CS2200_REF_CLK_DIV *pRefClockDiv)
{
    union CS2200_REG_FUNCTION_CONFIGURATION1_FIELD field;
    int r = cs2200_read_register(fd, deviceAddr, CS2200_REG_FUNCTION_CONFIGURATION1, &field.Value, sizeof(field.Value));
    if (r != OK)
    {
        return r;
    }
    if (pAuxLockCfg != NULL)
    {
        *pAuxLockCfg = (enum CS2200_AUX_LOCK_CFG)field.AuxLockCfg;
    }
    if (pRefClockDiv != NULL)
    {
        *pRefClockDiv = (enum CS2200_REF_CLK_DIV)field.RefClkDiv;
    }
    return OK;
}

int cs2200_write_function_configuration2(int fd, uint8_t deviceAddr, enum CS2200_PLL_CLOCK_OUTPUT_ON_UNLOCK pllClockOutputOnUnlock)
{
    union CS2200_REG_FUNCTION_CONFIGURATION2_FIELD field;
    field.reserved1 = 0;
    field.reserved2 = 0;
    field.ClkOutUnl = (uint8_t)pllClockOutputOnUnlock;
    return cs2200_write_register(fd, deviceAddr, CS2200_REG_FUNCTION_CONFIGURATION2, &field.Value, sizeof(field.Value));
}

int cs2200_read_function_configuration2(int fd, uint8_t deviceAddr, enum CS2200_PLL_CLOCK_OUTPUT_ON_UNLOCK *pPllClockOutputOnUnlock)
{
    union CS2200_REG_FUNCTION_CONFIGURATION2_FIELD field;
    int r = cs2200_read_register(fd, deviceAddr, CS2200_REG_FUNCTION_CONFIGURATION2, &field.Value, sizeof(field.Value));
    if (r != OK)
    {
        return r;
    }
    *pPllClockOutputOnUnlock = (enum CS2200_PLL_CLOCK_OUTPUT_ON_UNLOCK)field.ClkOutUnl;
    return OK;
}
