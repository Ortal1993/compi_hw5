#include "TableEntry.hpp"

TableEntry::TableEntry(std::string id, int offset, std::string type, bool isFunc, std::vector<std::string> argsTypes) :
                       id(id), offset(offset), type(type), value(value),
                       isFunc(isFunc), argsTypes(argsTypes) {}
std::string& TableEntry::getId() {
    return id;
}
int TableEntry::getOffset() {
    return offset;
}
std::string& TableEntry::getType() {
    return type;
}
bool TableEntry::getIsFunc() {
    return isFunc;
}
std::vector<std::string> TableEntry::getVecArgsTypes() {
    return argsTypes;
}
