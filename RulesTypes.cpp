#include "RulesTypes.hpp"
#include "Utilities.hpp"
#include "bp.hpp"
#include <string>
#include <algorithm>

RetTypeClass::RetTypeClass(std::string type) : type(type) {}
std::string RetTypeClass::getType() {return type;}

TypeClass::TypeClass(std::string type) : type(type) {}
std::string TypeClass::getType() {return type;}

IDClass::IDClass(std::string id, int quad) : id(id), quad(quad) {}
std::string IDClass::getId() {return id;}
int IDClass::getQuad() {return quad;}

StringClass::StringClass(std::string value) : strReg(StringRegister()), value(value) {
     this->value = value.substr(1, value.size()-2);
}
std::string StringClass::getValue() {return value;}

std::string StringClass::getRegName() {return strReg.getRegName();}

NUMClass::NUMClass(std::string value) : value(value) {}
std::string NUMClass::getValue() {return value;}

FormalsClass::FormalsClass(std::vector<std::string> argsType, std::vector<std::string> argsID) :
                            vecArgsType(argsType), vecArgsID(argsID) {}
std::vector<std::string> FormalsClass::getVecArgsType() {return vecArgsType;}
std::vector<std::string> FormalsClass::getVecArgsID() {return vecArgsID;}

FormalsListClass::FormalsListClass(std::vector<std::string> argsType, std::vector<std::string> argsID) :
                                    vecArgsType(argsType), vecArgsID(argsID) {}
std::vector<std::string> FormalsListClass::getVecArgsType() {return vecArgsType;}
std::vector<std::string> FormalsListClass::getVecArgsID() {return vecArgsID;}
void FormalsListClass::addNewArg(std::string argType, std::string argID)
{
    vecArgsType.insert(vecArgsType.begin(), argType);
    vecArgsID.insert(vecArgsID.begin(), argID);
}

FormalDeclClass::FormalDeclClass(std::string type, std::string id) : argType(type), argID(id) {}
std::string FormalDeclClass::getArgType() {return argType;}
std::string FormalDeclClass::getArgID() {return argID;}

ExpClass::ExpClass(OP_TYPE opType, std::string type, std::string value,
                   BaseClass* exp1, BaseClass* exp2, BaseClass* opExp, BaseClass* MExp) :
                    opType(opType), type(type), value(value), reg(Register()),
                    truelist(vector<pair<int,BranchLabelIndex>>()),
                    falselist(vector<pair<int,BranchLabelIndex>>())
{
    std::string code;
    std::string opStr;
    if (opExp != nullptr) {
        opStr = opExp->getOpStr();///For relop and binary
    }
    switch (opType) {
        case EXP_OP_ID:
        {
            //gets the offset of var. will help us ID value is in stack.
            //distinguish between local variables and function arguments case (in call?)
            Register valueReg; //temporary register to save value. if not boolean, will be stored in exp reg.
            int idOffset = getOffsetById(exp1->getId());
            if(idOffset < 0){
                //argument case
                //exp_reg = add %(the offset of argument * -1 (-1)), 0;
                int argRegNum = (idOffset*(-1)) - 1;
                std::string argRegName = "$" + std::to_string(argRegNum);
                code = valueReg.getRegName() + " = add " + getSizeByType(type) + " " + argRegName + ", 0";
                codeBuffer.emit(DOUBLE_TAB + code);
            }
            else {
                //local var case
                //exp_reg = load  idptr;
                std::string idOffsetStr = to_string(idOffset);
                //all vars are saved in the stack in size i32, as said in PDF and recommended in PIAZZA
                Register addrReg; //will hold the pointer of the value in the stack
                //need to create pointer for the stack, and special register to save it (stack location register).
                code = addrReg.getRegName() + " = getelementptr [50 x i32], [50 x i32]* " + stackRegister.getRegName() + ", i32 0, i32 " + idOffsetStr;
                codeBuffer.emit(DOUBLE_TAB + code);
                //now we need to load the value to the register we now work with.
                //there are 2 cases - if the var we work on is not INT, we need to trunc the size first.
                if(type != "INT") {
                    Register tempReg;
                    codeBuffer.emit(DOUBLE_TAB+ tempReg.getRegName() + " = load i32, i32* " + addrReg.getRegName()); //load val from ptr.
                    codeBuffer.emit(DOUBLE_TAB+ valueReg.getRegName() + " = trunc i32 " + tempReg.getRegName() + " to " + getSizeByType(type)); //check if logic OK.
                }
                else { //can't put in exp reg if the value is boolean!!! not allowed! need to create jump
                    codeBuffer.emit(DOUBLE_TAB + valueReg.getRegName() + " = load i32, i32* " + addrReg.getRegName()); //load val from ptr.
                }
            }
            //check if boolean
            if (type == "BOOL") {
                Register tempReg;
                code = tempReg.getRegName() + " = icmp eq i1 " + valueReg.getRegName() + ", 1"; //checking if value is truelist
                codeBuffer.emit(DOUBLE_TAB + code);
                int bufferLocation = codeBuffer.emit(DOUBLE_TAB + std::string("br i1 ") + tempReg.getRegName() + ", label @, label @");
                pair<int,BranchLabelIndex> ifTrue = pair<int,BranchLabelIndex>(bufferLocation, FIRST);
                pair<int,BranchLabelIndex> ifFalse = pair<int,BranchLabelIndex>(bufferLocation, SECOND);
                this->truelist = codeBuffer.makelist(ifTrue);
                this->falselist = codeBuffer.makelist(ifFalse);
            }
            else {
                codeBuffer.emit(DOUBLE_TAB+ reg.getRegName() + " = add " + getSizeByType(type) + " " + valueReg.getRegName() + ", 0");
            }

            break;
        }
        case EXP_OP_CALL:
        {
            if (type == "BOOL") {
                Register tempReg;
                code = tempReg.getRegName() + " = icmp eq i1 " + exp1->getRegName() + ", 1"; //checking if value is truelist
                codeBuffer.emit(DOUBLE_TAB + code);
                int bufferLocation = codeBuffer.emit(DOUBLE_TAB + "br i1 " + tempReg.getRegName() + ", label @, label @");
                pair<int,BranchLabelIndex> ifTrue = pair<int,BranchLabelIndex>(bufferLocation, FIRST);
                pair<int,BranchLabelIndex> ifFalse = pair<int,BranchLabelIndex>(bufferLocation, SECOND);
                this->truelist = codeBuffer.makelist(ifTrue);
                this->falselist = codeBuffer.makelist(ifFalse);
            }
            else {
                //exp_reg = add call_reg, 0;
                code = reg.getRegName() + " = add " + getSizeByType(type) + " " + exp1->getRegName() + ", 0";
                codeBuffer.emit(DOUBLE_TAB + code);
            }
            break;
        }
        case EXP_OP_NUM:
        {
            //it this case, type is always "INT"
            code = reg.getRegName() + " = add " + getSizeByType(type) + " " + value + ", 0";
            codeBuffer.emit(DOUBLE_TAB + code);
            break;
        }
        case EXP_OP_NUM_B:
        {
            //it this case, type is always "BYTE"
            code = reg.getRegName() + " = add " + getSizeByType(type) + " " + value + ", 0";
            codeBuffer.emit(DOUBLE_TAB + code);
            break;
        }
        case EXP_OP_STRING:
        {
            std::string sizeStr =  std::to_string(exp1->getValue().size());
            code = exp1->getRegName() + " = internal constant [" + sizeStr + " x i8] c\"" + exp1->getValue() + "\\00\"" ;
            codeBuffer.emitGlobal(code);
            code = reg.getRegName() + " add i8* " + exp1->getRegName() + ", 0";
            codeBuffer.emit(DOUBLE_TAB + code);
            break;
        }
        case EXP_OP_TRUE:
        {
            int bufferLocation = codeBuffer.emit(DOUBLE_TAB + "br label @");
            pair<int,BranchLabelIndex> item = pair<int,BranchLabelIndex>(bufferLocation, FIRST);
            this->truelist = codeBuffer.makelist(item);
            break;
        }
        case EXP_OP_FALSE:
        {
            int bufferLocation = codeBuffer.emit(DOUBLE_TAB + "br label @");
            pair<int,BranchLabelIndex> item = pair<int,BranchLabelIndex>(bufferLocation, FIRST);
            this->falselist = codeBuffer.makelist(item);
            break;
        }
        case EXP_OP_NOT:
        {
            this->truelist = exp1->getFalselist();
            this->falselist = exp1->getTruelist();
            break;
        }
        case EXP_OP_OR:
        {
            codeBuffer.bpatch(exp1->getFalselist(), MExp->getLabel());
            this->truelist = codeBuffer.merge(exp1->getTruelist(), exp2->getTruelist());
            this->falselist = exp2->getFalselist();
            break;
        }
        case EXP_OP_AND:
        {
            codeBuffer.bpatch(exp1->getTruelist(), MExp->getLabel());
            this->truelist = exp2->getTruelist();
            this->falselist = codeBuffer.merge(exp1->getFalselist(), exp2->getFalselist());
            break;
        }
        case EXP_OP_EQUAL:
        {
            std::string regExp1 = exp1->getRegName();
            std::string regExp2 = exp2->getRegName();
            if(exp1->getType() != exp2->getType()){
                Register tempReg;
                if(exp1->getType() == "BYTE"){
                    code = tempReg.getRegName() + " = zext i8 " + exp1->getRegName() + " to i32";
                    regExp1 = tempReg.getRegName();
                }else{
                    code = tempReg.getRegName() + " = zext i8 " + exp2->getRegName() + " to i32";
                    regExp2 = tempReg.getRegName();
                }
                codeBuffer.emit(DOUBLE_TAB + code);
            }
            if (opStr == "==") {
                code = this->reg.getRegName() + " = icmp eq " + getSizeByType(type) + regExp1 + ", " + regExp2;
            } else { //"!="
                code = this->reg.getRegName() + " = icmp ne " + getSizeByType(type) + regExp1 + ", " + regExp2;
            }
            codeBuffer.emit(DOUBLE_TAB + code);
            int bufferLocation = codeBuffer.emit(DOUBLE_TAB + "br i1 %" + this->reg.getRegName() + ", label @, label @");
            pair<int,BranchLabelIndex> ifTrue = pair<int,BranchLabelIndex>(bufferLocation, FIRST);
            pair<int,BranchLabelIndex> ifFalse = pair<int,BranchLabelIndex>(bufferLocation, SECOND);
            this->truelist = codeBuffer.makelist(ifTrue);
            this->falselist = codeBuffer.makelist(ifFalse);
            break;
        }
        case EXP_OP_GLT:
        {
            std::string regExp1 = exp1->getRegName();
            std::string regExp2 = exp2->getRegName();
            std::string signedChar = "s";
            if(exp1->getType() != exp2->getType()){
                Register tempReg;
                if(exp1->getType() == "BYTE"){
                    code = tempReg.getRegName() + " = zext i8 " + exp1->getRegName() + " to i32";
                    regExp1 = tempReg.getRegName();
                }else{
                    code = tempReg.getRegName() + " = zext i8 " + exp2->getRegName() + " to i32";
                    regExp2 = tempReg.getRegName();
                }
                codeBuffer.emit(DOUBLE_TAB + code);
            }
            else {
                if(exp1->getType() == "BYTE"){
                    signedChar = "u";
                }
            }

            if (opStr == "<") {
                code = this->reg.getRegName() + " = icmp " + signedChar + "lt " + getSizeByType(type) + " " + regExp1 + ", " + regExp2;
            } else if (opStr == ">") {
                code = this->reg.getRegName() + " = icmp " + signedChar + "gt " + getSizeByType(type) + " " + regExp1 + ", " + regExp2;
            } else if (opStr == "<=") {
                code = this->reg.getRegName() + " = icmp " + signedChar + "le " + getSizeByType(type) + " " + regExp1 + ", " + regExp2;
            } else { //">="
                code = this->reg.getRegName() + " = icmp " + signedChar + "ge " + getSizeByType(type) + " " + regExp1 + ", " + regExp2;
            }
            codeBuffer.emit(DOUBLE_TAB + code);
            int bufferLocation = codeBuffer.emit(DOUBLE_TAB + "br i1 %" + this->reg.getRegName() + ", label @, label @");
            pair<int,BranchLabelIndex> ifTrue = pair<int,BranchLabelIndex>(bufferLocation, FIRST);
            pair<int,BranchLabelIndex> ifFalse = pair<int,BranchLabelIndex>(bufferLocation, SECOND);
            this->truelist = codeBuffer.makelist(ifTrue);
            this->falselist = codeBuffer.makelist(ifFalse);
            break;
        }
        case EXP_OP_ADD:
        {
            std::string regExp1 = exp1->getRegName();
            std::string regExp2 = exp2->getRegName();

            if(exp1->getType() != exp2->getType()){
                Register tempReg;
                if(exp1->getType() == "BYTE"){
                    code = tempReg.getRegName() + " = zext i8 " + exp1->getRegName() + " to i32";
                    regExp1 = tempReg.getRegName();
                }else{
                    code = tempReg.getRegName() + " = zext i8 " + exp2->getRegName() + " to i32";
                    regExp2 = tempReg.getRegName();
                }
                codeBuffer.emit(DOUBLE_TAB + code);
            }
            if (opStr == "+") {
                //$t3 = add $t2, $t3
                code = this->reg.getRegName() + " = add " + getSizeByType(type) + " " + regExp1 + ", " + regExp2;
            } else { //"-"
                //$t3 = sub $t2, $t3
                code = this->reg.getRegName() + " = sub " + getSizeByType(type) + " " + regExp1 + ", " + regExp2;
            }
            codeBuffer.emit(DOUBLE_TAB + code);
            this->truelist = codeBuffer.merge(exp1->getTruelist(), exp2->getTruelist());
            this->falselist = codeBuffer.merge(exp1->getFalselist(), exp2->getFalselist());
            break;
        }
        case EXP_OP_MUL:
        {
            std::string regExp1 = exp1->getRegName();
            std::string regExp2 = exp2->getRegName();
            std::string signedChar = "s";
            if(exp1->getType() != exp2->getType()){ //1 int and 1 byte --> type = int
                Register tempReg;
                if(exp1->getType() == "BYTE"){
                    code = tempReg.getRegName() + " = zext i8 " + exp1->getRegName() + " to i32";
                    regExp1 = tempReg.getRegName();
                }
                else {
                    code = tempReg.getRegName() + " = zext i8 " + exp2->getRegName() + " to i32";
                    regExp2 = tempReg.getRegName();
                }
                codeBuffer.emit(DOUBLE_TAB + code);
            }
            else { // 2 byte or 2 int --> type = type
                if(exp1->getType() == "BYTE"){
                    signedChar = "u";
                }
            }
            if (opStr == "*") {
                //$t3 = add $t2, $t3
                code = this->reg.getRegName() + " = mul " + getSizeByType(type) + " " + exp1->getRegName() + ", " + exp2->getRegName();
            }
            else { //division
                //check if dividing by 0:
                Register ifzeroReg;
                codeBuffer.emit(DOUBLE_TAB + ifzeroReg.getRegName() + " = icmp eq " + getSizeByType(type) +  " " + exp2->getRegName() + ", 0");
                int bufferLocation = codeBuffer.emit(DOUBLE_TAB + "br i1 " + ifzeroReg.getRegName() + ", label @, label @");

                std::string ifZeroLabel = codeBuffer.genLabel();
                Register zeroErrAddr;
                codeBuffer.emit(DOUBLE_TAB + zeroErrAddr.getRegName() + " = getelementptr [23 x i8], [23 x i8]* @.errDivByZero, i32 0, i32 0");
                codeBuffer.emit(DOUBLE_TAB + "call void @print(i8* " + zeroErrAddr.getRegName() + ")");
                codeBuffer.emit(DOUBLE_TAB + "call void @exit(i32 1)");
                int finishedDivErr = codeBuffer.emit(DOUBLE_TAB + "br label @");

                std::string isDivLabel = codeBuffer.genLabel();
                code = this->reg.getRegName() + " = " + signedChar + "div "+ getSizeByType(type) + " " + exp1->getRegName() + ", " + exp2->getRegName();

                pair<int,BranchLabelIndex> ifZeroItem =  pair<int,BranchLabelIndex>(bufferLocation, FIRST);
                vector<pair<int,BranchLabelIndex>> zeroItemList = codeBuffer.makelist(ifZeroItem);
                codeBuffer.bpatch(zeroItemList, ifZeroLabel);

                pair<int,BranchLabelIndex> isDivItem =  pair<int,BranchLabelIndex>(bufferLocation, SECOND);
                vector<pair<int,BranchLabelIndex>> divItemList = codeBuffer.makelist(isDivItem);
                codeBuffer.bpatch(divItemList, isDivLabel);

                pair<int,BranchLabelIndex> isFinishDivErr =  pair<int,BranchLabelIndex>(finishedDivErr, FIRST);
                vector<pair<int,BranchLabelIndex>> isFinishDivErrList = codeBuffer.makelist(isFinishDivErr);
                codeBuffer.bpatch(isFinishDivErrList, isDivLabel);
            }
            codeBuffer.emit(DOUBLE_TAB + code);
            this->truelist = codeBuffer.merge(exp1->getTruelist(), exp2->getTruelist());
            this->falselist = codeBuffer.merge(exp1->getFalselist(), exp2->getFalselist());
            break;
        }
        case EXP_OP_CAST:
        {
            if(type == "BYTE" && exp1->getType() == "INT"){
                //t2 = trunc i32 exp1.reg to i8
                code = this->reg.getRegName() + " = trunc i32 " + exp1->getRegName() + " to i8";
            }
            else if(type == "INT" && exp1->getType() == "BYTE"){
                //this.reg = zext i8 exp1.reg to I32
                code = this->reg.getRegName() + " = zext i8 " + exp1->getRegName() + " to i32";
            }else{
                code = this->reg.getRegName() + " = add " + getSizeByType(type) + " " + exp1->getRegName() + ", 0";
            }
            codeBuffer.emit(DOUBLE_TAB + code);
            this->truelist = exp1->getTruelist();
            this->falselist = exp1->getFalselist();
        }
        case EXP_OP_L_EXP_R:
        {
            if (type != "BOOL") {
                code = reg.getRegName() + " = add " + getSizeByType(type) + " " + exp1->getRegName() + ", 0";
            }
            this->truelist = exp1->getTruelist();
            this->falselist = exp1->getFalselist();
            break;
        }

    }
    codeBuffer.emit(DOUBLE_TAB + code);
}
std::string ExpClass::getType() {return type;}
std::string ExpClass::getValue() {return value;}
std::string ExpClass::getRegName() {return reg.getRegName();}
vector<pair<int,BranchLabelIndex>> ExpClass::getTruelist() {return truelist;}
vector<pair<int,BranchLabelIndex>> ExpClass::getFalselist(){ return falselist;}

ExpListClass::ExpListClass(std::vector<std::string> vecArgsType, std::vector<std::string> vecArgsValue) :
                            vecArgsType(vecArgsType), vecArgsValue(vecArgsValue) {}
std::vector<std::string> ExpListClass::getVecArgsType() {return vecArgsType;}
std::vector<std::string> ExpListClass::getVecArgsValue() {return vecArgsValue;}
void ExpListClass::addNewArgType(std::string argType, std::string argValue) {
    vecArgsType.push_back(argType);
    vecArgsValue.push_back(argValue);
}

StatementsClass::StatementsClass(STATEMENTS_TYPE stsType,
                                 BaseClass* exp1, BaseClass* exp2) :
                                 stsType(stsType),
                                 nextlist(vector<pair<int,BranchLabelIndex>>()),
                                 breaklist(vector<pair<int,BranchLabelIndex>>()) {
    switch (stsType) {
        case STATEMENTS_STATEMENT:
        {
            this->nextlist = exp1->getNextlist();
            this->breaklist = exp1->getBreaklist();
            break;
        }
        case STATEMENTS_STATEMENTS_STATEMENT:
        {
            this->nextlist = codeBuffer.merge(exp1->getNextlist(), exp2->getNextlist());
            this->breaklist = codeBuffer.merge(exp1->getBreaklist(), exp2->getBreaklist());
            break;
        }
    }
}
vector<pair<int,BranchLabelIndex>> StatementsClass::getNextlist() {return nextlist;}
vector<pair<int,BranchLabelIndex>> StatementsClass::getBreaklist(){ return breaklist;}

StatementClass::StatementClass(STATEMENT_TYPE stType,
                               BaseClass* exp1, BaseClass* exp2,
                               BaseClass* exp3, BaseClass* exp4) :
                                stType(stType),
                                nextlist(vector<pair<int,BranchLabelIndex>>()),
                                breaklist(vector<pair<int,BranchLabelIndex>>())
{
    std::string code;
    switch (stType) {
        case STATEMENT_ID:
        {
            int idOffset = getOffsetById(exp1->getId());
            std::string idOffsetStr = to_string(idOffset);
            Register addrReg;
            code = addrReg.getRegName() + " = getelementptr [50 x i32], [50 x i32]* " + stackRegister.getRegName() + ", i32 0, i32 " + idOffsetStr;
            codeBuffer.emit(DOUBLE_TAB + code);
            code = "store i32 0, i32* " + addrReg.getRegName();
            codeBuffer.emit(DOUBLE_TAB + code);
            break;
        }
        case STATEMENT_TYPE_ID_ASS_EXP:
        {
            std::string storeReg;
            Register tempReg;
            std::string type = exp2->getType();
            if(type == "BOOL") {
                //create true label
                std::string ifTrueLabel = codeBuffer.genLabel();
                codeBuffer.bpatch(exp2->getTruelist(), ifTrueLabel);
                int bufferLocationTrue = codeBuffer.emit(DOUBLE_TAB + "br label @");
                pair<int,BranchLabelIndex> isTrue = pair<int,BranchLabelIndex>(bufferLocationTrue, FIRST);
                vector<pair<int,BranchLabelIndex>> patchTrue = codeBuffer.makelist(isTrue);

                //create false label
                std::string ifFalseLabel = codeBuffer.genLabel();
                codeBuffer.bpatch(exp2->getFalselist(), ifFalseLabel);
                int bufferLocationFalse = codeBuffer.emit(DOUBLE_TAB + "br label @");
                pair<int,BranchLabelIndex> isFalse = pair<int,BranchLabelIndex>(bufferLocationFalse, FIRST);
                vector<pair<int,BranchLabelIndex>> patchFalse = codeBuffer.makelist(isFalse);

                //create phi calculation
                std::string phiLabel = codeBuffer.genLabel();
                codeBuffer.bpatch(patchTrue, phiLabel);
                codeBuffer.bpatch(patchFalse, phiLabel);
                codeBuffer.emit(DOUBLE_TAB + tempReg.getRegName() + " = phi i32 [1, " + ifTrueLabel + "], [0, " + ifFalseLabel + "]");
                storeReg = tempReg.getRegName();

            }
            else if (type == "BYTE") {
                code = tempReg.getRegName() + " = zext " + getSizeByType(type) + " " + exp2->getRegName() + " to i32";
                codeBuffer.emit(DOUBLE_TAB + code);
                storeReg = tempReg.getRegName();
            }
            else {
                storeReg = exp2->getRegName();
            }

            int idOffset = getOffsetById(exp1->getId());
            std::string idOffsetStr = to_string(idOffset);
            Register addrToStoreReg;
            code = addrToStoreReg.getRegName() + " = getelementptr [50 x i32], [50 x i32]* " + stackRegister.getRegName() + ", i32 0, i32 " + idOffsetStr;
            codeBuffer.emit(DOUBLE_TAB + code);
            code = "store i32 " + storeReg + ", i32* " + addrToStoreReg.getRegName();
            codeBuffer.emit(DOUBLE_TAB + code);

            break;
        }
        case STATEMENT_AUTO_ID_ASS_EXP:
        {
            //practically the same as STATEMENT_TYPE_ID_ASS_EXP
            break;
        }
        case STATEMENT_ID_ASS_EXP:
        {
            //practically the same as STATEMENT_TYPE_ID_ASS_EXP
            break;

        }
        case STATEMENT_CALL:
        {
            //nothing to be done... all done on STATEMENT->CALL->in callClass we add code to LLVM.
            break;
        }
        case STATEMENT_RET:
        {
            setIsReturn(true);
            code = "ret void";
            codeBuffer.emit(DOUBLE_TAB + code);
            codeBuffer.genLabel();
            break;
        }
        case STATEMENT_RET_EXP:
        {
            //handle boolean return
            std::string retReg;
            Register tempReg;
            std::string funcRetType = getCurrFuncType();
            std::string type = exp1->getType();
            if(type == "BOOL") {
                //create true label
                std::string ifTrueLabel = codeBuffer.genLabel();
                codeBuffer.bpatch(exp1->getTruelist(), ifTrueLabel);
                int bufferLocationTrue = codeBuffer.emit(DOUBLE_TAB + "br label @");
                pair<int, BranchLabelIndex> isTrue = pair<int, BranchLabelIndex>(bufferLocationTrue, FIRST);
                vector<pair<int, BranchLabelIndex>> patchTrue = codeBuffer.makelist(isTrue);

                //create false label
                std::string ifFalseLabel = codeBuffer.genLabel();
                codeBuffer.bpatch(exp1->getFalselist(), ifFalseLabel);
                int bufferLocationFalse = codeBuffer.emit(DOUBLE_TAB + "br label @");
                pair<int, BranchLabelIndex> isFalse = pair<int, BranchLabelIndex>(bufferLocationFalse, FIRST);
                vector<pair<int, BranchLabelIndex>> patchFalse = codeBuffer.makelist(isFalse);

                //create phi calculation
                std::string phiLabel = codeBuffer.genLabel();
                codeBuffer.bpatch(patchTrue, phiLabel);
                codeBuffer.bpatch(patchFalse, phiLabel);
                codeBuffer.emit(DOUBLE_TAB + tempReg.getRegName() + " = phi i32 [1, " + ifTrueLabel + "], [0, " + ifFalseLabel + "]");
                retReg = tempReg.getRegName();
            }
            else if (type != funcRetType) {
                code = tempReg.getRegName() + " = zext " + getSizeByType(type) + " " + exp1->getRegName() + " to i32";
                codeBuffer.emit(DOUBLE_TAB + code);
                retReg = tempReg.getRegName();
            }
            else {
                retReg = exp1->getRegName();
            }
            setIsReturn(true);
            codeBuffer.emit(DOUBLE_TAB + "ret " + getSizeByType(type) + " " + retReg);
            codeBuffer.genLabel();
            break;
        }
        case STATEMENT_BREAK:
        {
            code = "br label @";
            int bufferLocation = codeBuffer.emit(DOUBLE_TAB + code);
            pair<int,BranchLabelIndex> ifBreak = pair<int,BranchLabelIndex>(bufferLocation, FIRST);
            this->breaklist = codeBuffer.makelist(ifBreak);
            break;
        }
        case STATEMENT_CONTINUE:
        {
            code = "br label @";
            int bufferLocation = codeBuffer.emit(DOUBLE_TAB + code);
            pair<int,BranchLabelIndex> ifContinue = pair<int,BranchLabelIndex>(bufferLocation, FIRST);
            this->nextlist = codeBuffer.makelist(ifContinue);
            break;
        }
        case STATEMENT_IF:
        {
            //exp1 -> (if) exp, exp2 -> (if) M, exp3 ->  S1, exp4 -> ifElse
            if(exp4->getElseType() == ELSE_UNUSED){
                codeBuffer.bpatch(exp1->getTruelist(), exp2->getLabel());
                nextlist = codeBuffer.merge(exp1->getFalselist(), exp3->getNextlist());

                int bufferLocation = codeBuffer.emit(DOUBLE_TAB + "br label @"); //generating the final jump to the label of the next block
                pair<int,BranchLabelIndex> endBlockPair = pair<int,BranchLabelIndex>(bufferLocation, FIRST);
                nextlist = codeBuffer.merge(this->nextlist, codeBuffer.makelist(endBlockPair));
                std::string endOfBlock = codeBuffer.genLabel();
                codeBuffer.bpatch(this->nextlist, endOfBlock);
                breaklist = exp3->getBreaklist(); //in case of: while < if < while

            } else {
                codeBuffer.bpatch(exp1->getTruelist(), exp2->getLabel());
                codeBuffer.bpatch(exp1->getFalselist(), exp4->getLabel());
                nextlist = codeBuffer.merge(exp3->getNextlist(), exp4->getNextlist());

                int bufferLocation = codeBuffer.emit(DOUBLE_TAB + "br label @");
                pair<int,BranchLabelIndex> endBlockPair = pair<int,BranchLabelIndex>(bufferLocation, FIRST);
                nextlist = codeBuffer.merge(nextlist, codeBuffer.makelist(endBlockPair));
                std::string endOfBlock = codeBuffer.genLabel();
                codeBuffer.bpatch(this->nextlist, endOfBlock);
                breaklist = codeBuffer.merge(exp3->getBreaklist(), exp4->getBreaklist());
            }
            break;
        }
        case STATEMENT_IF_ELSE:
        {
            //nothing to do
            break;
        }
        case STATEMENT_WHILE:
        {
            codeBuffer.bpatch(exp4->getNextlist(),exp1->getLabel());
            codeBuffer.bpatch(exp2->getTruelist(), exp3->getLabel());
            nextlist = exp2->getFalselist();
            code = "br label %" + exp1->getLabel();
            std::string finalLabel = codeBuffer.genLabel();
            codeBuffer.bpatch(nextlist, finalLabel);
            codeBuffer.bpatch(exp4->getBreaklist(), finalLabel);
            break;
        }
        case STATEMENT_L_STATEMENTS_R:
        {
            this->nextlist = exp1->getNextlist();
            this->breaklist = exp1->getBreaklist();
            break;
        }
        default:
        {
            std::cerr << "STATEMENT_TYPE ERROR!" << std::endl;
        }
    }

}
vector<pair<int,BranchLabelIndex>> StatementClass::getNextlist() {return nextlist;}
vector<pair<int,BranchLabelIndex>> StatementClass::getBreaklist() {return breaklist;}


IfElseClass::IfElseClass(ELSE_TYPE elseType,
               BaseClass* exp1, BaseClass* exp2, BaseClass* exp3) :
               elseType(elseType),
               nextlist(vector<pair<int,BranchLabelIndex>>()),
               breaklist(vector<pair<int,BranchLabelIndex>>()) {
    if(exp1 != nullptr && exp2 != nullptr && exp3 != nullptr){
        label = exp2->getLabel();
        nextlist = codeBuffer.merge(exp1->getNextlist(), exp3->getNextlist());
        breaklist = exp3->getBreaklist();
    }
}
ELSE_TYPE IfElseClass::getElseType() {return elseType;}
vector<pair<int,BranchLabelIndex>> IfElseClass::getNextlist() {return nextlist;}
vector<pair<int,BranchLabelIndex>> IfElseClass::getBreaklist() {return breaklist;}
std::string IfElseClass::getLabel() {return label;}

CallClass::CallClass( CALL_TYPE callType, std::string type, BaseClass* exp1, BaseClass* exp2) : callType(callType), type(type), reg(Register())
{
    std::string code;
    switch (callType) {
        case CALL_ID: //the ID here is the func name
        {
            //in this case, we simply call the func and we don't wait for any ret value
            if (type == "VOID") {
                code = "call " + getSizeByType(type) + " @" + exp1->getId() + "()";
                codeBuffer.emit(DOUBLE_TAB + code);
            }
            //in this case, we do wait for a ret value. Call will save it for EXP later
            else {
                code = reg.getRegName() + " = call " + getSizeByType(type) + " @" + exp1->getId() + "()";
                codeBuffer.emit(DOUBLE_TAB + code);
            }
            break;
        }
        case CALL_ID_EXPLIST:
        {
            std::vector<std::string> typesGiven = exp2->getVecArgsType();
            std::vector<std::string> valuesGiven = exp2->getVecArgsValue();
            std::reverse(typesGiven.begin(), typesGiven.end());
            std::reverse(valuesGiven.begin(), valuesGiven.end());
            std::vector<std::string> vecFuncTypes = getFuncVecTypes(exp1->getId()); //already reversed!!!

            if (type != "VOID") {
                code = reg.getRegName() + " = call " + getSizeByType(type) + " @" + exp1->getId() + "(";
            }
            else {
                code = "call " + getSizeByType(type) + " @" + exp1->getId() + "(";;
            }
            for (int i=0; i < typesGiven.size() ; i++) {
                std::string argType = getSizeByType(typesGiven[i]);
                if (argType == "BOOL") {
                    Register boolValReg;
                    std::string ifTrueLabel = codeBuffer.genLabel();
                    codeBuffer.bpatch(exp1->getTruelist(), ifTrueLabel);
                    int bufferLocationTrue = codeBuffer.emit(DOUBLE_TAB + "br label @");
                    pair<int, BranchLabelIndex> isTrue = pair<int, BranchLabelIndex>(bufferLocationTrue, FIRST);
                    vector<pair<int, BranchLabelIndex>> patchTrue = codeBuffer.makelist(isTrue);

                    //create false label
                    std::string ifFalseLabel = codeBuffer.genLabel();
                    codeBuffer.bpatch(exp1->getFalselist(), ifFalseLabel);
                    int bufferLocationFalse = codeBuffer.emit(DOUBLE_TAB + "br label @");
                    pair<int, BranchLabelIndex> isFalse = pair<int, BranchLabelIndex>(bufferLocationFalse, FIRST);
                    vector<pair<int, BranchLabelIndex>> patchFalse = codeBuffer.makelist(isFalse);

                    //create phi calculation
                    std::string phiLabel = codeBuffer.genLabel();
                    codeBuffer.bpatch(patchTrue, phiLabel);
                    codeBuffer.bpatch(patchFalse, phiLabel);
                    codeBuffer.emit(DOUBLE_TAB + boolValReg.getRegName() + " = phi i32 [1, " + ifTrueLabel + "], [0, " + ifFalseLabel + "]");
                    code += argType + " " + boolValReg.getRegName() + ", " ;
                }
                else if (argType != vecFuncTypes[i]) {
                    Register tempReg;
                    codeBuffer.emit(DOUBLE_TAB + tempReg.getRegName() + " = zext i8 " + valuesGiven[i] + " to i32"); //cast from byte to int
                    code += argType + " " + tempReg.getRegName() + ", " ;
                }
                else {
                    code += argType + " " + valuesGiven[i] + ", " ;
                }
            }
            if(typesGiven.size() != 0){
                code = code.substr(0,code.size() - 2); //remove the last ", " from last iteration
            }
            code += ")";
            codeBuffer.emit(DOUBLE_TAB + code);
            break;
        }
        default:
            std::cerr << "CALL_TYPE ERROR!" << std::endl;
        }
}
std::string CallClass::getType() {return type;}
std::string CallClass::getRegName() {return reg.getRegName();}

OpClass::OpClass(std::string opStr) : opStr(opStr){}
std::string OpClass::getOpStr() {return opStr;}

M_Class::M_Class() : label(codeBuffer.genLabel()){}
std::string M_Class::getLabel() {return label;}

N_Class::N_Class(){
    int bufferLocation = codeBuffer.emit(DOUBLE_TAB + "br label @");
    pair<int,BranchLabelIndex> item = pair<int,BranchLabelIndex>(bufferLocation, FIRST);
    nextlist = codeBuffer.makelist(item);
}

vector<pair<int,BranchLabelIndex>> N_Class::getNextlist() {
    return this->nextlist;
}
