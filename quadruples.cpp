#include "quadruples.h"
#include <iostream>
#include <iomanip> // Required for std::setw and std::left
#include <fstream>

using namespace std;

Quadruple::Quadruple(string op, string arg1, string arg2, string result)
{
    this->op = op;
    this->arg1 = arg1;
    this->arg2 = arg2;
    this->result = result;
}

// Removed typeAndValueToString implementation

// Simplified addQuadruple implementation to use string arguments directly
void Quadruples::addQuadruple(string op, string arg1, string arg2, string result)
{
    quadruples.push_back(new Quadruple(op, arg1, arg2, result));
}

void Quadruples::printQuadruples()
{
    printf("%d\n", quadruples.size());

    cout << std::left;
    cout << std::setw(8) << "Index" << "|"
         << std::setw(16) << "Op" << "|"
         << std::setw(16) << "Arg1" << "|"
         << std::setw(16) << "Arg2" << "|"
         << std::setw(16) << "Result" << endl;
    cout << std::string(8, '-') << "+"
         << std::string(16, '-') << "+"
         << std::string(16, '-') << "+"
         << std::string(16, '-') << "+"
         << std::string(16, '-') << endl;

    for (int i = 0; i < quadruples.size(); i++)
    {
        cout << std::setw(8) << i << "|"
             << std::setw(16) << quadruples[i]->op << "|"
             << std::setw(16) << quadruples[i]->arg1 << "|"
             << std::setw(16) << quadruples[i]->arg2 << "|"
             << std::setw(16) << quadruples[i]->result << endl;
        cout << std::string(8, '-') << "+"
             << std::string(16, '-') << "+"
             << std::string(16, '-') << "+"
             << std::string(16, '-') << "+"
             << std::string(16, '-') << endl;
    }
}

void Quadruples::printQuadruplesToFile(const std::string &filename)
{
    std::ofstream outFile(filename);
    if (!outFile.is_open())
    {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    outFile << std::left;
    outFile << std::setw(8) << "Index" << "|"
            << std::setw(16) << "Op" << "|"
            << std::setw(16) << "Arg1" << "|"
            << std::setw(16) << "Arg2" << "|"
            << std::setw(16) << "Result" << std::endl;
    outFile << std::string(8, '-') << "+"
            << std::string(16, '-') << "+"
            << std::string(16, '-') << "+"
            << std::string(16, '-') << "+"
            << std::string(16, '-') << std::endl;

    for (int i = 0; i < quadruples.size(); i++)
    {
        outFile << std::setw(8) << i << "|"
                << std::setw(16) << quadruples[i]->op << "|"
                << std::setw(16) << quadruples[i]->arg1 << "|"
                << std::setw(16) << quadruples[i]->arg2 << "|"
                << std::setw(16) << quadruples[i]->result << std::endl;
        outFile << std::string(8, '-') << "+"
                << std::string(16, '-') << "+"
                << std::string(16, '-') << "+"
                << std::string(16, '-') << "+"
                << std::string(16, '-') << std::endl;
    }

    outFile.close();
}

// Merge the current Quadruples with the Main Quadruples and return the merged Quadruples
Quadruples *Quadruples::merge(Quadruples *mainQuadruples)
{

    mainQuadruples->quadruples.insert(mainQuadruples->quadruples.end(), quadruples.begin(), quadruples.end());
    return mainQuadruples;
}

// reverse the order of the calling quadruples then merge them with the main quadruples
Quadruples *Quadruples::reverseMerge(Quadruples *mainQuadruples)
{

    vector<Quadruple *> reversedQuadruples;
    for (int i = quadruples.size() - 1; i >= 0; i--)
    {
        reversedQuadruples.push_back(quadruples[i]);
    }
    mainQuadruples->quadruples.insert(mainQuadruples->quadruples.end(), reversedQuadruples.begin(), reversedQuadruples.end());
    return mainQuadruples;
}