#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <queue>
#include "enums.h"
#include <fstream>
#include "value_helpers.h"

using namespace std;

struct SingleEntry
{
    dataType type;
    void *value;
    bool isConst = false;
    bool isUsed = false; // To check if the variable is used or not
    bool isError = false;
    queue<pair<dataType, string>> *args;
    dataType returnType;

    SingleEntry(dataType type, void *value, bool isConst, queue<pair<dataType, string>> *args, dataType returnType, bool isUsed) : type(type), value(value), isConst(isConst), args(args), returnType(returnType), isUsed(isUsed) {}
};

// Add Quadruple structure
struct Quadruple
{
    string op;
    string arg1;
    string arg2;
    string result;

    Quadruple(string op, string arg1, string arg2, string result)
        : op(op), arg1(arg1), arg2(arg2), result(result) {}
};

class SymbolTable
{
    unordered_map<string, SingleEntry *> table;
    SymbolTable *parent;
    vector<Quadruple> quadruples; // Store quadruples
    void * value;
    int tempVarCounter = 0;       // Counter for generating temporary variable names
public:
    SymbolTable(SymbolTable *parent = nullptr) : parent(parent) {}

    SymbolTable *getParent();

    void insert(string name, dataType type, void *value, bool isConst = false, queue<pair<dataType, string>> *args = nullptr, dataType returnType = VOID_TYPE, bool isUsed = false);

    // Add support for Value struct
    void insert(const Value &name, const Value &type, void *value = nullptr, bool isConst = false, queue<pair<dataType, string>> *args = nullptr, dataType returnType = VOID_TYPE, bool isUsed = false);

    void update_Value(string name, void *value, dataType type);

    void update_Func(string name);

    SingleEntry *get_SingleEntry(string name);

    // Quadruple methods
    void addQuadruple(string op, string arg1, string arg2, string result);
    string genTempVar();                                // Generate temporary variable names
    vector<Quadruple> &getQuadruples();                 // Get all quadruples
    void printQuadruples();                             // Print all quadruples
    void printQuadruplesToFile(const string &filename); // Print quadruples to file

    void checkUnused(); // Check if there are any unused variables

    void printTable(); // Prints the current table ONLY

    void printTableToFile(); // Prints the current table to a file
};
