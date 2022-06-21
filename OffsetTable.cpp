#include "OffsetTable.hpp"

OffsetTable::OffsetTable() : offset(std::list<int>()) {
    offset.push_back(0);
}

void OffsetTable::pushOffsetScope() {
    int lastOffset = offset.back();
    offset.push_back(lastOffset);
}

void OffsetTable::popOffsetScope() {
    offset.pop_back();
}

void OffsetTable::incOffset() {
    offset.back()++;
}

int OffsetTable::getOffset() {
    return offset.back();
}
