#include <sdk/config.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <nuttx/i2c/i2c_master.h>
#include <arch/board/common/cxd56_i2cdev.h>
#include "i2c.h"

static int i2c_transfer(int fd, struct i2c_msg_s *msgv, int msgc)
{
    struct i2c_transfer_s xfer;
    xfer.msgv = msgv;
    xfer.msgc = msgc;
    return ioctl(fd, I2CIOC_TRANSFER, (unsigned long)((uintptr_t)&xfer));
}

int i2c_open(int deviceNumber)
{
    board_i2cdev_initialize(0);

    char devPath[32];
    sprintf(devPath, "/dev/i2c%d", deviceNumber);
    return open(devPath, O_RDONLY);
}

int i2c_write_data(int fd, uint8_t deviceAddr, uint8_t* pData, ssize_t length)
{
    struct i2c_msg_s msg;
    msg.frequency = I2C_SPEED_STANDARD;
    msg.addr = deviceAddr;
    msg.flags = 0;
    msg.buffer = pData;
    msg.length = length;
    return i2c_transfer(fd, &msg, 1);
}

int i2c_read_data(int fd, uint8_t deviceAddr, uint8_t* pData, ssize_t length)
{
    struct i2c_msg_s msg;
    msg.frequency = I2C_SPEED_STANDARD;
    msg.addr = deviceAddr;
    msg.flags = I2C_M_READ;
    msg.buffer = pData;
    msg.length = length;
    return i2c_transfer(fd, &msg, 1);
}

int i2c_close(int fd)
{
    return close(fd);
}
