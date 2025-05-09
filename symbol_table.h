#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <queue>
#include "enums.h"
#include <fstream>
using namespace std;

struct SingleEntry {
    dataType type;
    void * value;
    bool isConst = false;
    bool isUsed = false; // To check if the variable is used or not
    queue<pair<dataType, string>> *args;
    dataType returnType;
    // No need to put name because it is the key in the map
    
    SingleEntry(dataType type, void * value, bool isConst,  queue<pair<dataType, string>> *args, dataType returnType, bool isUsed) : type(type), value(value), isConst(isConst),  args(args), returnType(returnType), isUsed(isUsed) {}
};

class SymbolTable {
    unordered_map<string, SingleEntry*> table;
    SymbolTable * parent;
public:

   

    SymbolTable(SymbolTable * parent = nullptr) : parent(parent) {}

    SymbolTable * getParent();

    void insert(string name, dataType type, void * value, bool isConst = false, queue<pair<dataType, string>> *args = nullptr, dataType returnType = VOID_TYPE, bool isUsed = false);

    void update_Value(string name, void * value, dataType type);

    void update_Func(string name);

    SingleEntry * get_SingleEntry(string name);

    void checkUnused(); // Check if there are any unused variables

    void printTable(); // Prints the current table ONLY

    void printTableToFile(); // Prints the current table to a file



};
