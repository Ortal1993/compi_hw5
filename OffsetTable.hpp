#ifndef _OFFSET_TABLE_
#define _OFFSET_TABLE_

#include <iostream>
#include <list>

//offset management
class OffsetTable {
private:
    std::list<int> offset;
public:
    OffsetTable();
    ~OffsetTable() = default;
    void pushOffsetScope();
    void popOffsetScope();
    void incOffset();
    int getOffset();
};

#endif //_OFFSET_TABLE_
