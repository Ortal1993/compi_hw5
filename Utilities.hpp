#ifndef _UTILITIES_
#define _UTILITIES_

#include "SymbolTable.hpp"
#include "bp.hpp"
#include <iostream>

CodeBuffer& codeBuffer = CodeBuffer::instance();

void checkIfBValid(std::string value);
void checkIfBool(std::string type);
void checkIfNumeric(std::string type);
void checkIfAssignValidByType(std::string dstType, std::string srcType);
void checkIfAutoAssignValid(std::string type);
void checkIfBreakValid();
void checkIfContinueValid();
std::string setRetBinopType(std::string typeLeft, std::string typeRight);
std::string getTypeById(std::string id);
void addVarNewEntry(std::string id, std::string type, std::string value = std::string());
void addFuncNewEntry(std::string id, std::string type, std::vector<std::string> vecArgsType = std::vector<std::string>());
void addFuncArgsToTable(std::vector<std::string> vecArgsType, std::vector<std::string> vecArgsId);
void checkValidArgs(std::vector<std::string>& vecArgsId);
bool checkTypeMatch(std::string dstType, std::string srcType);
void checkReturnType(std::string type);
void openScope();
void closeScope();
void incWhileCounter();
void decWhileCounter();
std::string checkFuncCall(std::string funcId, std::vector<std::string> vecGivenArgsTypes = std::vector<std::string>());
void checkMainExist();
void addPrintFunctions();
void printScope();

#endif //_UTILITIES_
