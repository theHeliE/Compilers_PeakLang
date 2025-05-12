#pragma once

#include <string>
#include "enums.h"

// Define Value struct to be used across the entire project
struct Value
{
    int intVal;
    float floatVal;
    bool boolVal;
    char *stringVal;
    char charVal;
    void *voidVal;
    dataType type; // For compatibility with SymbolTable
};

// Function declarations
std::string getValueName(const Value &v);
dataType getValueType(const Value &v);