#include <string.h>
#include <stdio.h>
#include "cs2200_register_common.h"

static const struct CS2200_REGISTER_FIELD_VALUE* findFieldValue(
  const void* pContext,
  const struct CS2200_REGISTER_FIELD_VALUE fieldValues[],
  bool (*pfnCallback)(const void* pContext, const struct CS2200_REGISTER_FIELD_VALUE* pField))
{
  for (const struct CS2200_REGISTER_FIELD_VALUE* pFieldValue = fieldValues; pFieldValue->pExplanation != NULL; ++pFieldValue)
  {
    if (pfnCallback(pContext, pFieldValue))
    {
      return pFieldValue;
    }
  }
  return NULL;
}

static bool isMatchItemArgument(const void* pContext, const struct CS2200_REGISTER_FIELD_VALUE* pFieldValue)
{
  return strcmp((const char*)pContext, pFieldValue->pArgument) == 0;
}

static bool isMatchItemObject(const void* pContext, const struct CS2200_REGISTER_FIELD_VALUE* pFieldValue)
{
  return (int)pContext == pFieldValue->object;
}

static const char* findExplanationFromObject(int object, const struct CS2200_REGISTER_FIELD_VALUE fieldValues[])
{
    const struct CS2200_REGISTER_FIELD_VALUE* pFieldValue = findFieldValue((const void*)object, fieldValues, isMatchItemObject);
    return (pFieldValue != NULL) ? pFieldValue->pExplanation : "Unknown";
}

const struct CS2200_REGISTER_FIELD_VALUE* findFieldValueObjectFromArgument(const char* pArgument, const struct CS2200_REGISTER_FIELD* pRegisterField)
{
    return findFieldValue(pArgument, pRegisterField->values, isMatchItemArgument);
}

void printFieldValueExplanationWithDescription(int object, const struct CS2200_REGISTER_FIELD* pRegisterField)
{
    printf("\t%s : (%02x) %s.\n", pRegisterField->pDescription, (uint8_t)object, findExplanationFromObject(object, pRegisterField->values));
}

void showCommonHelp(const struct CS2200_REGISTER_COMMAND* pCommand)
{
  fprintf(stderr, "%s arguments:\n", pCommand->pDescription);
  if (pCommand->fields[0] == NULL)
  {
    fprintf(stderr, "\tNone.\n");
    return;
  }

  for (const struct CS2200_REGISTER_FIELD* const* ppField = pCommand->fields; *ppField != NULL; ++ppField)
  {
    const struct CS2200_REGISTER_FIELD* pField = *ppField;
    if (pField->values[0].pArgument == NULL)
    {
      // readonly field
      continue;
    }
    fprintf(stderr, "\t%s:\n", pField->pDescription);
    for (const struct CS2200_REGISTER_FIELD_VALUE* pValue = pField->values; pValue->pExplanation != NULL; ++pValue)
    {
      fprintf(stderr, "\t\t%s : %s\n", pValue->pArgument, pValue->pExplanation);
    }
  }
}
