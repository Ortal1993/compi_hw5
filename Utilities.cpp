#include "Utilities.hpp"
#include "hw3_output.hpp"
#include "OffsetTable.hpp"
#include <algorithm>

SymbolTable symbolTable;
OffsetTable offset;
int WhileCounter = 0;
bool isReturn = false;
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
//the vecArgsType and vecArgsId are already in the right order
void addFuncNewEntry(std::string id, std::string retType, std::vector<std::string> vecArgsType) {
    TableEntry* entryOfId = symbolTable.findEntryInTable(id);
    if (entryOfId != nullptr) {
        output::errorDef(yylineno, id);
        exit(0);
    }
    //need to change the directions of the args in the vector.
    //std::reverse(vecArgsType.begin(), vecArgsType.end());
    //push final result to table
    TableScope& topScope = symbolTable.getTopScope();
    topScope.pushEntry(id, 0, retType, true, vecArgsType);
}

//adding functions arguments to the new function scope
//the vecArgsType and vecArgsId are already in the right order
void addFuncArgsToTable(std::vector<std::string> vecArgsType, std::vector<std::string> vecArgsId) {
    //should be the same size
    checkValidArgs(vecArgsId); //see description above the function implementation

    if (!vecArgsType.empty()){ //case there is at least 1 argument
       TableScope& topScope = symbolTable.getTopScope();
       int argOffset = -1; //ensures that their indexes are negative order.
       for(size_t i = 0; i < vecArgsType.size(); i++){
           topScope.pushEntry(vecArgsId[i], argOffset, vecArgsType[i]);
           argOffset--;
       }
       /*for (size_t i = (vecArgsType.size() - 1); i >= 0; i--) { //need to switch the order.
          topScope.pushEntry(vecArgsId[i], argOffset, vecArgsType[i]);
          argOffset--;
          if (i == 0){
              break;
          }
       }*/
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
    //std::reverse(vecGivenArgsTypes.begin(), vecGivenArgsTypes.end());

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

void addPrintFunctionsToTable() {
    std::vector<std::string> printArgs;
    printArgs.push_back("STRING");
    addFuncNewEntry("print", "VOID", printArgs);

    std::vector<std::string> printiArgs;
    printiArgs.push_back("INT");
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

std::vector<std::string> getFuncVecTypes(std::string funcId) {
    TableEntry* funcEntry = symbolTable.getFirstScope().findEntryInScope(funcId); //checking specifically in the "func scope"
    if (funcEntry == nullptr || funcEntry->getIsFunc() == false) {
        output::errorUndefFunc(yylineno, funcId);
        exit(0);
    }
    std::vector<std::string> vecFuncTypes = funcEntry->getVecArgsTypes(); //already reversed!!!
    return vecFuncTypes;
}

void setIsReturn(bool status) {
    isReturn = status;
}

void HandleReturn(std::string retType) {
    if (isReturn == true) {
        return;
    }
    else {
        if (retType == "VOID") {
            codeBuffer.emit("ret void");
        }
        else {
            codeBuffer.emit("ret " + getSizeByType(retType) + " 0");
        }
    }
    codeBuffer.emit("}");
    codeBuffer.emit(""); //'New line'
}

void allocateFuncStack() {
    std::string code;
    code = stackRegister.getRegName() + "= alloca [50 x i32]";
    codeBuffer.emit(code);
    stackRegister.setNewRegName();
}

void declarePrintFunctions() {
    std::string code;
    codeBuffer.emitGlobal("declare i32 @printf(i8*, ...)");
    codeBuffer.emitGlobal("declare void @exit(i32)");
    codeBuffer.emitGlobal("@.int_specifier = constant [4 x i8] c\"%d\0A\00\"");
    codeBuffer.emitGlobal("@.str_specifier = constant [4 x i8] c\"%s\0A\00\"");
    codeBuffer.emit("\n");

    codeBuffer.emit("define void @printi(i32) {");
    codeBuffer.emit("%spec_ptr = getelementptr [4 x i8], [4 x i8]* @.int_specifier, i32 0, i32 0");
    codeBuffer.emit("call i32 (i8*, ...) @printf(i8* %spec_ptr, i32 %0)");
    codeBuffer.emit("ret void");
    codeBuffer.emit("}");
    codeBuffer.emit("\n");

    codeBuffer.emit("define void @print(i8*) {");
    codeBuffer.emit("%spec_ptr = getelementptr [4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0");
    codeBuffer.emit("call i32 (i8*, ...) @printf(i8* %spec_ptr, i8* %0)");
    codeBuffer.emit("ret void");
    codeBuffer.emit("}");
    codeBuffer.emit("\n");
}

void addDivisionByZeroError() {
    codeBuffer.emitGlobal("@.errDivByZero = internal constant [22 x i8] c\"Error division by zero\"");
}

void defineFuncDecl(std::string retType, std::string id, std::vector<std::string> vecArgsType){
        std::string code;
        code = "define " + getSizeByType(retType) + " @" + id + "(";
        for(int i = 0; i < vecArgsType.size(); i++){
            code += getSizeByType(vecArgsType[i]) + ", ";
        }
        if(vecArgsType.size() != 0) {
            code = code.substr(0,code.size() - 2); //remove the last ", " from last iteration
        }
        code = + ") {";
        codeBuffer.emit(code);
};

void printBuffer(){
    codeBuffer.printGlobalBuffer();
    codeBuffer.printCodeBuffer();
}
