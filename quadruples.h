#pragma once

#include <vector>
#include <string>
#include "enums.h"

using namespace std;

class Quadruple
{
public:
    string op;
    string arg1;
    string arg2;
    string result;

    Quadruple(string op, string arg1, string arg2, string result);
};

class Quadruples
{
public:
    vector<Quadruple *> quadruples;
    int resultCounter = 0;
    int labelCounter = 0;

    void addQuadruple(string op, string arg1, string arg2, string result = "");
    void printQuadruples();
    Quadruples *merge(Quadruples *mainQuadruples);
    Quadruples *reverseMerge(Quadruples *mainQuadruples);
    void printQuadruplesToFile(const std::string &filename);
};