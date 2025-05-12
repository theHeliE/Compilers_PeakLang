#include "value_helpers.h"

// Functions to extract data from Value structs
std::string getValueName(const Value &v)
{
    if (v.stringVal != nullptr)
    {
        return std::string(v.stringVal);
    }
    return "";
}

dataType getValueType(const Value &v)
{
    return v.type;
}