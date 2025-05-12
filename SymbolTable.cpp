#include "SymbolTable.h"
#include <iomanip>

// extern void ErrorToFile(string msg);
extern string enumToString(dataType type);
extern string getValueName(const Value &v);
extern dataType getValueType(const Value &v);
ofstream SymbolTab("SymbolTable.txt");

void SymbolTable::insert(string name, dataType type, void *value, bool isConst, queue<pair<dataType, string>> *args, dataType returnType, bool isUsed)
{

    cout << "Inserting " << name << endl;

    // Check if the variable is already declared

    if (table.find(name) != table.end() && table[name]->type != FUNC_TYPE)
    {
        // ErrorToFile("Variable " + name + " already declared");
        // throw runtime_error("Variable " + name + " already declared");
        cout << "ERROR: Variable " << name << " already declared" << endl;
    }

    ////////////Functions Checks////////////////////

    if (type == FUNC_TYPE)
    {
        // Check if the function is already declared
        if (table.find(name) != table.end())
        {
            // ErrorToFile("Function " + name + " already declared");
            // throw runtime_error("Function " + name + " already declared");
            cout << "ERROR: Function " << name << " already declared" << endl;
        }
    }

    this->table[name] = new SingleEntry(type, value, isConst, args, returnType, isUsed);

    table[name]->args = new queue<pair<dataType, string>>;
    if (args != nullptr)
    {
        *table[name]->args = *args;
    }

    printf("Inserted %s\n", name.c_str());
}

// Modified method to support Value struct in parser.y
void SymbolTable::insert(const Value &name, const Value &type, void *value, bool isConst, queue<pair<dataType, string>> *args, dataType returnType, bool isUsed)
{
    // Extract the name string and type from the Value structs
    string nameStr = getValueName(name);
    dataType typeVal = getValueType(type);

    // Call the original insert method with extracted values
    insert(nameStr, typeVal, value, isConst, args, returnType, isUsed);
}

void SymbolTable::update_Func(string name) // Update the function to be used
{
    SingleEntry *entry = get_SingleEntry(name);

    entry->isUsed = true;
}

void SymbolTable::checkUnused()
{
    // Check if there are any unused variables
    for (auto const &entry : table)
    {
        if (!entry.second->isUsed)
        {
            // ErrorToFile("Warning, "+ entry.first + " is declared but not used");
            cout << "WARNING: " << entry.first << " is declared but not used" << endl;
        }
    }
}

void SymbolTable::update_Value(string name, void *value, dataType type)
{
    // Check if the variable is in the symbol table

    SingleEntry *entry = get_SingleEntry(name);

    if (entry == nullptr)
    {
        // ErrorToFile("Variable " + name + " not declared");
        // throw runtime_error("Variable " + name + " not declared");
        cout << "ERROR: Variable " << name << " not declared" << endl;
    }
    else
    {
        if (entry->type != type)
        {
            // ErrorToFile("Type mismatch for variable " + name);
            // throw runtime_error("Type mismatch for variable " + name);
            cout << "ERROR: Type mismatch for variable " << name << endl;
        }

        if (entry->isConst)
        {
            // ErrorToFile("Variable " + name + " is constant");
            // throw runtime_error("Variable " + name + " is constant");
            cout << "ERROR: Variable " << name << " is constant" << endl;
        }

        entry->isUsed = true;
        entry->value = value;
    }

    // Use the get_SingleEntry function to find the entry and update the value of the entry
}

// Get Parent

SymbolTable *SymbolTable::getParent()
{
    return parent;
}

SingleEntry *SymbolTable::get_SingleEntry(string name)
{
    // Using recursion to find an entry in the symbol table using its name going back to the parent table if needed\
    // If the entry is not found, the entry will be nullptr
    SingleEntry *entry = nullptr;

    if (table.find(name) != table.end())
    {
        entry = table[name];
    }
    else if (parent != nullptr)
    {
        entry = parent->get_SingleEntry(name);
    }

    return entry;
}

// New quadruple methods
void SymbolTable::addQuadruple(string op, string arg1, string arg2, string result)
{
    quadruples.push_back(Quadruple(op, arg1, arg2, result));
}

string SymbolTable::genTempVar()
{
    return "t" + to_string(tempVarCounter++);
}

vector<Quadruple> &SymbolTable::getQuadruples()
{
    return quadruples;
}

void SymbolTable::printQuadruples()
{
    cout << "======= QUADRUPLES =======" << endl;
    for (size_t i = 0; i < quadruples.size(); i++)
    {
        cout << i << ": " << quadruples[i].op << " " << quadruples[i].arg1 << " "
             << quadruples[i].arg2 << " " << quadruples[i].result << endl;
    }
    cout << "=========================" << endl;
}

void SymbolTable::printQuadruplesToFile(const string &filename)
{
    ofstream quadFile(filename);
    quadFile << "======= QUADRUPLES =======" << endl;
    for (size_t i = 0; i < quadruples.size(); i++)
    {
        quadFile << i << ": " << quadruples[i].op << " " << quadruples[i].arg1 << " "
                 << quadruples[i].arg2 << " " << quadruples[i].result << endl;
    }
    quadFile << "=========================" << endl;
    quadFile.close();
}

void SymbolTable::printTable()
{
    cout << "Printing Table" << endl;
    // Print the symbol table
    for (auto const &entry : table)
    {
        cout << entry.first << " : " << entry.second->type << " : " << entry.second->isConst << " : ";
        if (entry.second->value == nullptr && entry.second->type != 5)
        {
            cout << "NULL";
        }
        else
        {
            switch (entry.second->type)
            {
            case 0: // int
                cout << *static_cast<int *>(entry.second->value);
                break;
            case 1: // float
                cout << *static_cast<float *>(entry.second->value);
                break;
            case 2:                                               // string
                cout << static_cast<char *>(entry.second->value); // Correctly cast to char*
                break;
            case 3: // bool
                cout << (*static_cast<bool *>(entry.second->value) ? "true" : "false");
                break;
            case 4: // char
                cout << *static_cast<char *>(entry.second->value);
                break;
            case 5: // function
                cout << "NULL: " << endl;
                cout << "return type: " << entry.second->returnType << " and arguments: ";
                if (entry.second->args != nullptr)
                {
                    queue<pair<dataType, string>> argsCopy = *entry.second->args;
                    while (!argsCopy.empty())
                    {
                        auto arg = argsCopy.front();
                        argsCopy.pop();
                        cout << arg.second << ":" << arg.first << " ";
                    }
                }
                break;
            default:
                cout << "Unknown type";
            }
        }
        cout << endl;
    }
    cout << endl
         << "Finishing Table" << endl;
}

void SymbolTable::printTableToFile()
{
    SymbolTab << "////////////////////////////////////////////////////////////////////////////////////////" << endl;
    SymbolTab << std::left << std::setw(20) << "Name" << std::setw(15) << "Type" << std::setw(10) << "isConst" << std::setw(20) << "isUsed" << std::setw(10) << "Value" << endl;
    SymbolTab << "----------------------------------------------------------------------------------------" << endl;
    // Print the symbol table
    for (auto const &entry : table)
    {
        SymbolTab << std::left << std::setw(20) << entry.first << std::setw(15) << enumToString(entry.second->type) << std::setw(10) << (entry.second->isConst ? "true" : "false") << std::setw(20) << (entry.second->isUsed ? "true" : "false");
        if (entry.second->value == nullptr && entry.second->type != 5)
        {
            SymbolTab << "NULL";
        }
        else
        {
            switch (entry.second->type)
            {
            case 0: // int
                SymbolTab << *static_cast<int *>(entry.second->value);
                break;
            case 1: // float
                SymbolTab << *static_cast<float *>(entry.second->value);
                break;
            case 2:                                                    // string
                SymbolTab << static_cast<char *>(entry.second->value); // Correctly cast to char*
                break;
            case 3: // bool
                SymbolTab << (*static_cast<bool *>(entry.second->value) ? "true" : "false");
                break;
            case 4: // char
                SymbolTab << *static_cast<char *>(entry.second->value);
                break;
            case 5: // function
                SymbolTab << "NULL " << endl
                          << "return type: " << enumToString(entry.second->returnType) << " and arguments: ";
                if (entry.second->args != nullptr)
                {
                    queue<pair<dataType, string>> argsCopy = *entry.second->args;
                    while (!argsCopy.empty())
                    {
                        auto arg = argsCopy.front();
                        argsCopy.pop();
                        SymbolTab << arg.second << ":" << enumToString(arg.first) << " ";
                    }
                }
                break;
            default:
                SymbolTab << "Unknown type";
            }
        }
        SymbolTab << endl;
    }
    SymbolTab << endl
              << "////////////////////////////////////////////////////////////////////////////////////////" << endl;
}