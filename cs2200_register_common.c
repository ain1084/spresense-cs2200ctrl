#include <stdio.h>
#include <string.h>
#include "cs2200_register_command.h"
#include "cs2200_register_common.h"

static const struct CS2200_REGISTER_FIELD_VALUE* findFieldValue(const void* pContext,
    const struct CS2200_REGISTER_FIELD_VALUE fieldValues[], bool (*pfnCallback)(const void* pContext, const struct CS2200_REGISTER_FIELD_VALUE* pField))
{
    for (const struct CS2200_REGISTER_FIELD_VALUE *pFieldValue = fieldValues; pFieldValue->pExplanation != NULL; ++pFieldValue)
    {
        if (pfnCallback(pContext, pFieldValue))
        {
            return pFieldValue;
        }
    }
    return NULL;
}

static bool isMatchItemArgument(const void *pContext, const struct CS2200_REGISTER_FIELD_VALUE *pFieldValue)
{
    return strcmp((const char *)pContext, pFieldValue->pArgument) == 0;
}

static bool isMatchItemValue(const void *pContext, const struct CS2200_REGISTER_FIELD_VALUE *pFieldValue)
{
    return (int)pContext == pFieldValue->value;
}

static const char *findExplanationFromValue(int object, const struct CS2200_REGISTER_FIELD_VALUE fieldValues[])
{
    const struct CS2200_REGISTER_FIELD_VALUE *pFieldValue = findFieldValue((const void *)object, fieldValues, isMatchItemValue);
    return (pFieldValue != NULL) ? pFieldValue->pExplanation : "Unknown";
}

const struct CS2200_REGISTER_FIELD_VALUE *findFieldValueFromArgument(const char *pArgument, const struct CS2200_REGISTER_FIELD *pRegisterField)
{
    return findFieldValue(pArgument, pRegisterField->values, isMatchItemArgument);
}

void printFieldExplanationWithDescription(int object, const struct CS2200_REGISTER_FIELD *pRegisterField)
{
    printf("\t%s : (%02x) %s.\n", pRegisterField->pDescription, (uint8_t)object, findExplanationFromValue(object, pRegisterField->values));
}

void showCommonHelp(const struct CS2200_REGISTER_COMMAND *pCommand)
{
    fprintf(stderr, "%s arguments:\n", pCommand->pDescription);
    if (pCommand->fields[0] == NULL)
    {
        fprintf(stderr, "\tNone.\n");
        return;
    }

    for (const struct CS2200_REGISTER_FIELD *const *ppField = pCommand->fields; *ppField != NULL; ++ppField)
    {
        const struct CS2200_REGISTER_FIELD *pField = *ppField;
        if (pField->values[0].pArgument == NULL)
        {
            // readonly field
            continue;
        }
        fprintf(stderr, "\t%s:\n", pField->pDescription);
        for (const struct CS2200_REGISTER_FIELD_VALUE *pValue = pField->values; pValue->pExplanation != NULL; ++pValue)
        {
            fprintf(stderr, "\t\t%s : %s\n", pValue->pArgument, pValue->pExplanation);
        }
    }
}
