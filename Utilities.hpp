#ifndef _UTILITIES_
#define _UTILITIES_

#include "SymbolTable.hpp"
#include "bp.hpp"
#include "Register.hpp"
#include "RulesTypes.hpp"
#include <iostream>


const std::string DOUBLE_TAB = "\t\t";

extern CodeBuffer& codeBuffer;
extern StackScopeRegister stackRegister;

void checkIfBValid(std::string value);
void checkIfBool(std::string type);
void checkIfNumeric(std::string type);
void checkIfAssignValidByType(std::string dstType, std::string srcType);
void checkIfAutoAssignValid(std::string type);
void checkIfBreakValid();
void checkIfContinueValid();
std::string setRetBinopType(std::string typeLeft, std::string typeRight);
std::string getTypeById(std::string id);
void addVarNewEntry(std::string id, std::string type);
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
void addPrintFunctionsToTable();
void printScope();
std::string getSizeByType(std::string type);
int getOffsetById(std::string id);
std::vector<std::string> getFuncVecTypes(std::string funcId);
void HandleReturn(std::string retType);
void allocateFuncStack();
void declarePrintFunctions();
void addDivisionByZeroError();
void defineFuncDecl(std::string retType, std::string id, std::vector<std::string> vecArgsType);
void printBuffer();
std::string getCurrFuncType();
void setValToExpReg(BaseClass* exp);
#endif //_UTILITIES_
