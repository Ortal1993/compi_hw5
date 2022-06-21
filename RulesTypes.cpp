#include "RulesTypes.hpp"
#include "Utilities.hpp"
#include "bp.hpp"
#include <string>

RetTypeClass::RetTypeClass(std::string type) : type(type) {}
std::string RetTypeClass::getType() {return type;}

TypeClass::TypeClass(std::string type) : type(type) {}
std::string TypeClass::getType() {return type;}

IDClass::IDClass(std::string id, int quad) : id(id), quad(quad) {}
std::string IDClass::getId() {return id;}
int IDClass::getQuad() {return quad;}

StringClass::StringClass(std::string value) : value(value) {}
std::string StringClass::getValue() {return value;}

NUMClass::NUMClass(std::string value) : value(value) {}
std::string NUMClass::getValue() {return value;}

FormalsClass::FormalsClass(std::vector<std::string> argsType, std::vector<std::string> argsID) : vecArgsType(argsType), vecArgsID(argsID) {}
std::vector<std::string> FormalsClass::getVecArgsType() {return vecArgsType;}
std::vector<std::string> FormalsClass::getVecArgsID() {return vecArgsID;}

FormalsListClass::FormalsListClass(std::vector<std::string> argsType, std::vector<std::string> argsID) : vecArgsType(argsType), vecArgsID(argsID) {}
std::vector<std::string> FormalsListClass::getVecArgsType() {return vecArgsType;}
std::vector<std::string> FormalsListClass::getVecArgsID() {return vecArgsID;}
void FormalsListClass::addNewArg(std::string argType, std::string argID)
{
    vecArgsType.push_back(argType);
    vecArgsID.push_back(argID);
}

FormalDeclClass::FormalDeclClass(std::string type, std::string id) : argType(type), argID(id) {}
std::string FormalDeclClass::getArgType() {return argType;}
std::string FormalDeclClass::getArgID() {return argID;}

ExpClass::ExpClass(OP_TYPE opType, std::string type, std::string value,
                   BaseClass* exp1, BaseClass* exp2, BaseClass* opExp) :
                    type(type), value(value), reg(Register()), opType(opType),
                    truelist(vector<pair<int,BranchLabelIndex>>()),
                    falselist(vector<pair<int,BranchLabelIndex>>()),
{
    std::string code;
    std::string opStr = opExp->getOpStr();///For relop and binary
    switch (opType) {
        case EXP_OP_ID:
            if(!value.empty()){

            }
            break;
        case EXP_OP_CALL:

            break;
        case EXP_OP_NUM:
            break;

        case EXP_OP_NUM_B:
            break;

        case EXP_OP_STRING:
            break;

        case EXP_OP_TRUE:
            break;

        case EXP_OP_FALSE:
            break;

        case EXP_OP_NOT:
            this->truelist = exp1->getFalselist();
            this->falselist = exp1->getTruelist();
            break;
        case EXP_OP_OR:
            codeBuffer.bpatch(exp1->getFalselist(), opExp->getLabel());
            this->truelist = codeBuffer.merge(exp1->getTruelist(), exp2->getTruelist());
            this->falselist = exp2->getFalselist();
            break;
        case EXP_OP_AND:
            codeBuffer.bpatch(exp1->getTruelist(), opExp->getLabel());
            this->truelist = exp2->getTruelist();
            this->falselist = codeBuffer.merge(exp1->getFalselist(), exp2->getFalselist());
            break;
        case EXP_OP_EQUAL:
            if (opStr == "==") {
                ///NEED TO CHECK THE SIZE WE COMPARE! i8, i16 etc.
                code = this->reg.getRegName() + " = icmp eq " + "SIZE WE COMPARE" + exp1->getValue() + ", " + exp2->getValue();
            } else { //"!="
                code = this->reg.getRegName() + " = icmp ne " + "SIZE WE COMPARE" + exp1->getValue() + ", " + exp2->getValue();
            }
            break;
        case EXP_OP_GLT:
            if (opStr == "<") {
                ///NEED TO CHECK THE SIZE WE ADD/SUB! i8, i16 etc.
                ///NEED TO CHECK ABOUT UNSIGNED? udiv, sdiv
                code = this->reg.getRegName() + " = ult " + "SIZE WE COMPARE" + exp1->getValue() + ", " + exp2->getValue();
            } else if (opStr == ">") {
                code = this->reg.getRegName() + " = ugt " + "SIZE WE COMPARE" + exp1->getValue() + ", " + exp2->getValue();
            } else if (opStr == "<=") {
                code = this->reg.getRegName() + " = ule " + "SIZE WE COMPARE" + exp1->getValue() + ", " + exp2->getValue();
            } else { //">="
                code = this->reg.getRegName() + " = uge " + "SIZE WE COMPARE" + exp1->getValue() + ", " + exp2->getValue();
            }
            break;
        case EXP_OP_ADD:
            if (opStr == "+") {
                ///NEED TO CHECK THE SIZE WE ADD/SUB! i8, i16 etc.
                //$t3 = add $t2, $t3
                code = this->reg.getRegName() + " = add " + exp1->getReg() + ", " + exp2->getReg();
            } else { //"-"
                //$t3 = sub $t2, $t3
                code = this->reg.getRegName() + " = sub " + exp1->getReg() + ", " + exp2->getReg();
            }
            break;
        case EXP_OP_MUL:
            if (opStr == "*") {
                ///NEED TO CHECK THE SIZE WE ADD/SUB! i8, i16 etc.
                ///NEED TO CHECK ABOUT UNSIGNED? udiv, sdiv
                //$t3 = add $t2, $t3
                code = this->reg.getRegName() + " = mul " + exp1->getReg() + ", " + exp2->getReg();
            } else { //"/"
                //$t3 = div $t2, $t3
                code = this->reg.getRegName() + " = div " + exp1->getReg() + ", " + exp2->getReg();
            }
            break;

        case EXP_OP_CAST:
            break;
        case EXP_OP_L_EXP_R:
            this->truelist = exp1->getTruelist();
            this->falselist = exp1->getFalselist();
            break;
    }

    codeBuffer.emit(code);
}
std::string ExpClass::getType() {return type;}
std::string ExpClass::getValue() {return value;}
std::string ExpClass::getReg() {return reg.getRegName();}

ExpListClass::ExpListClass(std::vector<std::string> vecArgsType) : vecArgsType(vecArgsType) {}
std::vector<std::string> ExpListClass::getVecArgsType() {return vecArgsType;}
void ExpListClass::addNewArgType(std::string argType) {
    vecArgsType.push_back(argType);
}

StatementClass::StatementClass() : nextlist(vector<pair<int,BranchLabelIndex>>()){}
vector<pair<int,BranchLabelIndex>> StatementClass::getNextlist() {return nextlist;}

CallClass::CallClass(std::string type) : type(type) {}
std::string CallClass::getType() {return type;}
//std::string CallClass::getId() {return id;}

OpClass::OpClass(std::string opStr) : opStr(opStr){}
std::string OpClass::getOpStr() {return opStr;}

M_Class::M_Class() : label(codeBuffer.genLabel()){}
std::string M_Class::getLabel() {return label;}

N_Class::N_Class(){
    int bufferLocation = codeBuffer.emit("goto @");
    pair<int,BranchLabelIndex> item = pair<int,BranchLabelIndex>(bufferLocation, FIRST);
    nextlist = codeBuffer.makelist(item);
}
