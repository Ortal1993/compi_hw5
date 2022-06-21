#ifndef _SYMBOL_TABLE_
#define _SYMBOL_TABLE_

#include <iostream>
#include <list>
#include "TableScope.hpp"

class SymbolTable {
private:
    std::list<TableScope> scopes;
public:
    SymbolTable();
    ~SymbolTable() = default;
    void pushScope();
    void popScope();
    TableScope& getTopScope();

    //TODO: ADD A FIND FUNCTION!!! --> return pointer to the right entry if var exist. if not --> returns null.
    TableEntry* findEntryInTable(std::string id);
	TableScope& getFirstScope();
};

#endif //_SYMBOL_TABLE_
