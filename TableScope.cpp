#include "TableScope.hpp"

TableScope::TableScope() : entries(std::vector<TableEntry>()){}

void TableScope::pushEntry(std::string id, int offset, std::string type,
                           std::string value, bool isFunc,
                           std::vector<std::string> argsTypes) {
    TableEntry newEntry(id, offset, type, value, isFunc, argsTypes);
    entries.push_back(newEntry);
}

void TableScope::popEntry() {
    entries.pop_back();
}

TableEntry* TableScope::findEntryInScope(std::string id) {
    for (std::vector<TableEntry>::iterator it=entries.begin(); it != entries.end(); ++it) {
        //if we found a valid entry in current TableScope
        if (id == (*it).getId()) {
            return &(*it);
        }
    }
    //didn't find any entry in this scope
    return nullptr;
}

TableEntry& TableScope::topEntry(){
	return entries.back();
}

int TableScope::getScopeSize() {
    return entries.size();
}

TableEntry* TableScope::getEntryByPos(int i) {
    return &(entries[i]);
}
