#ifndef _TABLE_SCOPE_
#define _TABLE_SCOPE_

#include <iostream>
#include <vector>
#include "TableEntry.hpp"


class TableScope {
private:
    std::vector<TableEntry> entries;
public:
    TableScope();
    ~TableScope() = default;
    void pushEntry(std::string id, int offset, std::string type,
                   std::string value = std::string(), bool isFunc = false,
                   std::vector<std::string> argsTypes = std::vector<std::string>());
    void popEntry();
	TableEntry& topEntry();
    TableEntry* findEntryInScope(std::string id);
    int getScopeSize();
    TableEntry* getEntryByPos(int i);
};

#endif //_TABLE_SCOPE_
