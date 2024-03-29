#pragma once

struct CS2200_REGISTER_FIELD_VALUE
{
    const char *pArgument;
    const int value;
    const char *pExplanation;
};

struct CS2200_REGISTER_FIELD
{
    const char *pDescription;
    const struct CS2200_REGISTER_FIELD_VALUE values[];
};

struct CS2200_REGISTER_COMMAND
{
    const char *pDescription;
    const char *pName;
    const int (*pfnRead)(int fd, uint8_t slaveId);
    const int (*pfnWrite)(int fd, uint8_t slaveId, char *args[], int argn, int argc);
    const void (*pfnHelp)(void);
    const struct CS2200_REGISTER_FIELD *const fields[];
};

const struct CS2200_REGISTER_FIELD_VALUE *findFieldValueFromArgument(const char *pArgument, const struct CS2200_REGISTER_FIELD *pRegisterField);
void printFieldExplanationWithDescription(int object, const struct CS2200_REGISTER_FIELD *pRegisterField);
void showCommonHelp(const struct CS2200_REGISTER_COMMAND *pCommand);
