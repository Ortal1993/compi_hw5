#include "SymbolTable.hpp"


SymbolTable::SymbolTable() : scopes(std::list<TableScope>()) {}

void SymbolTable::pushScope() {
    TableScope newScope;
    scopes.push_back(newScope);
}
void SymbolTable::popScope() {
    scopes.pop_back();
}

TableScope& SymbolTable::getTopScope() {
    return scopes.back();
}

TableEntry* SymbolTable::findEntryInTable(std::string id) {
    for (std::list<TableScope>::iterator it=scopes.begin(); it != scopes.end(); ++it) {
        TableEntry* tableEntry = (*it).findEntryInScope(id);
        //if we found a valid entry in current TableScope
        if (tableEntry != nullptr) {
            return tableEntry;
        }
    }
    //didn't find any entry in any scope in SymbolTable
    return nullptr;
}

TableScope& SymbolTable::getFirstScope(){
	return scopes.front();
}
