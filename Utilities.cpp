#include "Utilities.hpp"
#include "hw3_output.hpp"
#include "OffsetTable.hpp"
#include <algorithm>

SymbolTable symbolTable;
OffsetTable offset;
int WhileCounter = 0;
static TableEntry& getCurrFunc();


void checkIfBValid(std::string value) {
    try {
        int byteValue = std::stoi(value);
        if (byteValue > 255) {
            output::errorByteTooLarge(yylineno, value);
            exit(0);
        }
    }
    catch (...) {
        std::cout << "recieved exception in checkIfBValid!" << std::endl;
        exit(1);
    }
    return;
}

void checkIfBool(std::string type) {
    if (type != "BOOL") {
        output::errorMismatch(yylineno);
        exit(0);
    }
}

void checkIfNumeric(std::string type) {
    if ((type != "INT") && (type != "BYTE")) {
        output::errorMismatch(yylineno);
        exit(0);
    }
}

void checkIfAssignValidByType(std::string dstType, std::string srcType){
	if(!checkTypeMatch(dstType,srcType)) {
		output::errorMismatch(yylineno);
		exit(0);
	}
}

void checkIfAutoAssignValid(std::string type){
	if ((type != "BOOL") && (type != "INT") && (type != "BYTE")) {
        output::errorMismatch(yylineno);
        exit(0);
    }
}

bool checkTypeMatch(std::string dstType, std::string srcType)
{
    if(dstType == srcType) {
        return true;
    }
    else if ((dstType == "INT") && (srcType == "BYTE")) {
        return true;
    }
    else {
        return false;
    }
}

void checkIfBreakValid(){
	if(WhileCounter == 0){
		output::errorUnexpectedBreak(yylineno);
		exit(0);
	}

}
void checkIfContinueValid(){
	if(WhileCounter == 0){
		output::errorUnexpectedContinue(yylineno);
		exit(0);
	}
}

std::string setRetBinopType(std::string typeLeft, std::string typeRight) {
    checkIfNumeric(typeLeft);
    checkIfNumeric(typeRight);
    if ((typeLeft == "INT") || (typeRight == "INT")) {
        return "INT";
    }
    return "BYTE";
}

std::string getTypeById(std::string id) {
    TableEntry* entryOfId = symbolTable.findEntryInTable(id);
    if (entryOfId == nullptr || entryOfId->getIsFunc() == true) { //the id is of function
        output::errorUndef(yylineno, id);
        exit(0);
    }
    return entryOfId->getType();
}

//adding variable to symbolTable
void addVarNewEntry(std::string id, std::string type) {
    TableEntry* entryOfId = symbolTable.findEntryInTable(id); //check it is not already exist
    if (entryOfId != nullptr) {
        output::errorDef(yylineno,id);
        exit(0);
    }
    TableScope& topScope = symbolTable.getTopScope();
    topScope.pushEntry(id, offset.getOffset(), type);
    offset.incOffset();

}

//adding function to symbolTable
void addFuncNewEntry(std::string id, std::string retType, std::vector<std::string> vecArgsType) {
    TableEntry* entryOfId = symbolTable.findEntryInTable(id);
    if (entryOfId != nullptr) {
        output::errorDef(yylineno, id);
        exit(0);
    }
    //need to change the directions of the args in the vector.
    std::reverse(vecArgsType.begin(), vecArgsType.end()); //now vecArgsTypes is in the regular order
    //push final result to table
    TableScope& topScope = symbolTable.getTopScope();
    topScope.pushEntry(id, 0, retType, true, vecArgsType);
}

//adding functions arguments to the new function scope
void addFuncArgsToTable(std::vector<std::string> vecArgsType, std::vector<std::string> vecArgsId) {
    //should be the same size
    checkValidArgs(vecArgsId); //see description above the function implementation

    if (!vecArgsType.empty()){ //case there is at least 1 argument
       TableScope& topScope = symbolTable.getTopScope();
       int argOffset = -1; //ensures that their indexes are negative order.
       for (size_t i = (vecArgsType.size() - 1); i >= 0; i--) { //need to switch the order.
          topScope.pushEntry(vecArgsId[i], argOffset, vecArgsType[i]);
          argOffset--;
          if (i == 0){
              break;
          }
       }
    }
}

//helper function - checks that there are not the same ID in func declaration / in table already
void checkValidArgs(std::vector<std::string>& vecArgsId) {
    //check there is not the same id in signature
	for (size_t i = 0; i < vecArgsId.size(); i++) {
		for (size_t j = i + 1; j < vecArgsId.size(); j++) {
			if (vecArgsId[i] == vecArgsId[j]) {
				output::errorDef(yylineno, vecArgsId[i]);
				exit(0);
			}
		}
	}
    //check there is not the same id in symbolTable
	for (std::vector<std::string>::iterator it = vecArgsId.begin(); it != vecArgsId.end(); ++it) {
		if (symbolTable.findEntryInTable(*it) != nullptr) {
			output::errorDef(yylineno, *it);
			exit(0);
		}
	}
}

static TableEntry& getCurrFunc(){
	TableEntry& currFuncEntry = symbolTable.getFirstScope().topEntry();
}

void checkReturnType(std::string type){
	checkIfAssignValidByType(getCurrFunc().getType(), type);
}

void openScope() {
  symbolTable.pushScope();
  offset.pushOffsetScope();
}

void closeScope() {
  output::endScope();
  printScope();
  symbolTable.popScope();
  offset.popOffsetScope();
}

void incWhileCounter() {
    WhileCounter++;
}
void decWhileCounter() {
    WhileCounter--;
}

std::string checkFuncCall(std::string funcId, std::vector<std::string> vecGivenArgsTypes) {
    TableEntry* funcEntry = symbolTable.getFirstScope().findEntryInScope(funcId); //checking specifically in the "func scope"
    if (funcEntry == nullptr || funcEntry->getIsFunc() == false) {
        output::errorUndefFunc(yylineno, funcId);
        exit(0);
    }
    std::vector<std::string> vecFuncTypes = funcEntry->getVecArgsTypes(); //already reversed!!!
    std::reverse(vecGivenArgsTypes.begin(), vecGivenArgsTypes.end());

    if (vecFuncTypes.size() != vecGivenArgsTypes.size()) {
        output::errorPrototypeMismatch(yylineno, funcId, vecFuncTypes);
		exit(0);
    }

    for (int i = 0; i < vecFuncTypes.size(); i++) {
        if(!checkTypeMatch(vecFuncTypes[i],vecGivenArgsTypes[i])) {
            output::errorPrototypeMismatch(yylineno, funcId, vecFuncTypes);
			exit(0);
        }
    }
    return funcEntry->getType();
}

void checkMainExist() {
    TableEntry* funcEntry = symbolTable.getFirstScope().findEntryInScope("main");
    if(funcEntry == nullptr || funcEntry->getType() != "VOID" || !(funcEntry->getVecArgsTypes().empty())) {
        output::errorMainMissing();
		exit(0);
    }
}

void addPrintFunctions() {
    std::vector<std::string> printArgs;
    std::vector<std::string> printiArgs;
    printArgs.push_back("STRING");
    printiArgs.push_back("INT");
    addFuncNewEntry("print", "VOID", printArgs);
    addFuncNewEntry("printi", "VOID", printiArgs);
}

void printScope() {
    TableScope& topScope = symbolTable.getTopScope();
    int scopeSize = topScope.getScopeSize();
    for (int i = 0; i < scopeSize; i++) {
        TableEntry* currEntry = topScope.getEntryByPos(i);
        if(currEntry->getIsFunc() == false) {
            output::printID(currEntry->getId(), currEntry->getOffset(), currEntry->getType());
        }
        else {
            std::vector<std::string> typesToPrint = currEntry->getVecArgsTypes();
            output::printID(currEntry->getId(), currEntry->getOffset(), output::makeFunctionType(currEntry->getType(), typesToPrint));
        }
    }
}

std::string getSizeByType(std::string type){
    if(type == "INT"){
        return "i32";
    }else if(type == "BYTE"){
        return "i8";
    }else if(type == "BOOL"){
        return "i1";
    }
    ///void???
}

int getOffsetById(std::string id){
    TableEntry* entry = symbolTable.findEntryInTable(id);
    if (entry == nullptr) {
        output::errorUndef(yylineno, id);
        exit(0);
    }//do we need to check if not null?
    return entry->getOffset();
}

