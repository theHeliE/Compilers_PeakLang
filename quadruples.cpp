#include "quadruples_struct.hpp"
#include <iostream>
#include <iomanip> // Required for std::setw and std::left
#include <fstream>

using namespace std;

Quadruple::Quadruple(string op, string arg1, string arg2, string result) {
    this->op = op;
    this->arg1 = arg1;
    this->arg2 = arg2;
    this->result = result;
}

// Function to convert TypeAndValue to string
char* Quadruples::typeAndValueToString(TypeAndValue* tv) {
    char* str = (char*)malloc(50 * sizeof(char)); // Allocate enough memory for the string representation

    switch (tv->type) {
        case INT_TYPE_ENUM:
            snprintf(str, 50, "%d", *(int*)(tv->value));
            break;
        case FLOAT_TYPE_ENUM:
            snprintf(str, 50, "%f", *(float*)(tv->value));
            break;
        case STRING_TYPE_ENUM:
            snprintf(str, 50, "%s", (char*)(tv->value));
            break;
        case BOOL_TYPE_ENUM:
            snprintf(str, 50, "%s", (*(bool*)(tv->value)) ? "True" : "False");
            break;
        case CHAR_TYPE_ENUM:
            snprintf(str, 50, "%c", *(char*)(tv->value));
            break;
        default:
            snprintf(str, 50, "unknown");
            break;
    }
    return str;
}

void Quadruples::addQuadruple(string op, TypeAndValue* arg1, TypeAndValue* arg2, string result) {		

    // Convert arg1 and arg2 to strings
    char* arg1Str = nullptr;
    char* arg2Str = nullptr;

    if (arg1->type != UNKNOWN_TYPE_ENUM) {
        arg1Str = typeAndValueToString(arg1);
    }

    if (arg2->type != UNKNOWN_TYPE_ENUM) {
        arg2Str = typeAndValueToString(arg2);
    }
	
    quadruples.push_back(new Quadruple(op, arg1Str ? arg1Str : "", arg2Str ? arg2Str : "", result));

    // Free allocated memory
    if (arg1Str) free(arg1Str);
    if (arg2Str) free(arg2Str);
}

void Quadruples::printQuadruples() {
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

    for (int i = 0; i < quadruples.size(); i++) {
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

void Quadruples::printQuadruplesToFile(const std::string& filename) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
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

    for (int i = 0; i < quadruples.size(); i++) {
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
Quadruples* Quadruples::merge(Quadruples* mainQuadruples) {
	
	mainQuadruples->quadruples.insert(mainQuadruples->quadruples.end(), quadruples.begin(), quadruples.end());
	return mainQuadruples;
}

// reverse the order of the calling quadruples then merge them with the main quadruples
Quadruples* Quadruples::reverseMerge(Quadruples* mainQuadruples) {
    
    vector<Quadruple*> reversedQuadruples;
    for (int i = quadruples.size() - 1; i >= 0; i--) {
        reversedQuadruples.push_back(quadruples[i]);
    }
    mainQuadruples->quadruples.insert(mainQuadruples->quadruples.end(), reversedQuadruples.begin(), reversedQuadruples.end());
    return mainQuadruples;
}