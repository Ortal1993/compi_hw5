#ifndef _TABLE_ENTRY_
#define _TABLE_ENTRY_

#include <iostream>
#include <vector>

class TableEntry {
private:
    std::string id;
    int offset;
    std::string type;
    std::string value;
    bool isFunc;
    std::vector<std::string> argsTypes;
public:
    TableEntry(std::string id, int offset, std::string type,
               std::string value = std::string(), bool isFunc = false,
               std::vector<std::string> argsTypes = std::vector<std::string>());
    ~TableEntry() = default;
    std::string& getId();
    int getOffset();
    std::string& getType();
	std::string setType(std::string newType);
    std::string getValue();
    bool getIsFunc();
    std::vector<std::string> getVecArgsTypes();
};

#endif //_TABLE_ENTRY_
