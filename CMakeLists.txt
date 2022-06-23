cmake_minimum_required(VERSION 3.22)
project(hw5)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(hw5
        bp.cpp
        bp.hpp
        hw3_output.cpp
        hw3_output.hpp
        OffsetTable.cpp
        OffsetTable.hpp
        Register.cpp
        Register.hpp
        RulesTypes.cpp
        RulesTypes.hpp
        SymbolTable.cpp
        SymbolTable.hpp
        TableEntry.cpp
        TableEntry.hpp
        TableScope.cpp
        TableScope.hpp
        Utilities.cpp
        Utilities.hpp)
