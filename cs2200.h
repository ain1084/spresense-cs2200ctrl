#pragma once
#include <stdbool.h>
#include <stdint.h>

enum CS2200_DEVICE_ID
{
    DEVICE_ID_CS2200 = 0
};
enum CS2200_REVISION
{
    CS2200_REVISION_B2_B3 = 4,
    CS2200_REVISION_C1 = 6
};
enum CS2200_R_MOD_SELECTION
{
    LEFT_SHIFT_BY0 = 0,
    LEFT_SHIFT_BY1 = 1,
    LEFT_SHIFT_BY2 = 2,
    LEFT_SHIFT_BY3 = 3,
    RIGHT_SHIFT_BY1 = 4,
    RIGHT_SHIFT_BY2 = 5,
    RIGHT_SHIFT_BY3 = 6,
    RIGHT_SHIFT_BY4 = 7
};
enum CS2200_AUX_OUT_SOURCE
{
    AUX_OUT_REFCLK = 0,
    AUX_OUT_RESERVED = 1,
    AUX_OUT_CLK_OUT = 2,
    AUX_OUT_PLL_LOCK_STATUS_INDICATOR = 3
};
enum CS2200_REF_CLK_DIV
{
    REFERENCE_CLOCK_DIVIDE_BY4 = 0, // 32MHz to 56MHz
    REFERENCE_CLOCK_DIVIDE_BY2 = 1, // 16MHz to 28MHz
    REFERENCE_CLOCK_DIVIDE_BY1 = 2  // 8MHz to 14MHz
};
enum CS2200_AUX_LOCK_CFG
{
    AUX_OUT_PUSH_PULL = 0,
    AUX_OUT_OPEN_DRAIN = 1
};
enum CS2200_PLL_CLOCK_OUTPUT_ON_UNLOCK
{
    CLK_OUT_LOW = 0,
    CLK_OUT_ALWAYS = 1
};

int cs2200_read_device_id(int fd, uint8_t deviceAddr, enum CS2200_DEVICE_ID* pId, enum CS2200_REVISION* pRevision);
int cs2200_write_device_control(int fd, uint8_t deviceAddr, bool isAuxOutDisabled, bool isPllClockOutputDisabled);
int cs2200_read_device_control(int fd, uint8_t deviceAddr, bool* pIsPllUnlocked, bool* pIsAuxOutDisabled, bool* pIsPllClockOutputDisabled);
int cs2200_write_device_configuration1(int fd, uint8_t deviceAddr, enum CS2200_R_MOD_SELECTION rmodSel, enum CS2200_AUX_OUT_SOURCE auxOutSource, bool isEnableDeviceConfiguration1);
int cs2200_read_device_configuration1(int fd, uint8_t deviceAddr, enum CS2200_R_MOD_SELECTION *pRmodSel, enum CS2200_AUX_OUT_SOURCE* pAuxOutSource, bool* pIsEnabledDeviceConfiguration1);
int cs2200_write_ratio(int fd, uint8_t deviceAddr, unsigned long ratio);
int cs2200_read_ratio(int fd, uint8_t deviceAddr, unsigned long* pRatio);
int cs2200_write_global_configuration(int fd, uint8_t deviceAddr, bool isFreeze, bool isEnabledDeviceConfiguration2);
int cs2200_read_global_configuration(int fd, uint8_t deviceAddr, bool* pIsFreeze, bool* pIsEnabledDeviceConfiguration2);
int cs2200_write_function_configuration1(int fd, uint8_t deviceAddr, enum CS2200_AUX_LOCK_CFG auxLockCfg, enum CS2200_REF_CLK_DIV refClockDiv);
int cs2200_read_function_configuration1(int fd, uint8_t deviceAddr, enum CS2200_AUX_LOCK_CFG* pAuxLockCfg, enum CS2200_REF_CLK_DIV* pRefClockDiv);
int cs2200_write_function_configuration2(int fd, uint8_t deviceAddr, enum CS2200_PLL_CLOCK_OUTPUT_ON_UNLOCK pllClockOutputOnUnlock);
int cs2200_read_function_configuration2(int fd, uint8_t deviceAddr, enum CS2200_PLL_CLOCK_OUTPUT_ON_UNLOCK* pPllClockOutputOnUnlock);
