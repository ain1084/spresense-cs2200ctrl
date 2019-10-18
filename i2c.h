#pragma once
#include <sys/types.h>

int i2c_open(int deviceNumber);
int i2c_write_data(int fd, uint8_t deivceAddr, uint8_t* pData, ssize_t length);
int i2c_read_data(int fd, uint8_t deviceAddr, uint8_t* pData, ssize_t length);
int i2c_close(int fd);

