#ifndef _RULES_TYPES_
#define _RULES_TYPES_

#include <iostream>
#include <vector>
#include <string>
#include "Register.hpp"
#include "bp.hpp"

enum OP_TYPE{
    DEFAULT,
    EXP_OP_ID,
    EXP_OP_CALL,
    EXP_OP_NUM,
    EXP_OP_NUM_B,
    EXP_OP_STRING,
    EXP_OP_TRUE,
    EXP_OP_FALSE,
    EXP_OP_NOT,
    EXP_OP_OR,
    EXP_OP_AND,
    EXP_OP_EQUAL,
    EXP_OP_GLT,
    EXP_OP_ADD,
    EXP_OP_MUL,
    EXP_OP_CAST,
    EXP_OP_L_EXP_R
};

enum STATEMENTS_TYPE {
    STATEMENTS_STATEMENT,
    STATEMENTS_STATEMENTS_STATEMENT
};

enum STATEMENT_TYPE{
    STATEMENT_ID,
    STATEMENT_TYPE_ID_ASS_EXP,
    STATEMENT_AUTO_ID_ASS_EXP,
    STATEMENT_ID_ASS_EXP,
    STATEMENT_CALL,
    STATEMENT_RET,
    STATEMENT_RET_EXP,
    STATEMENT_BREAK,
    STATEMENT_CONTINUE,
    STATEMENT_IF,
    STATEMENT_IF_ELSE,
    STATEMENT_WHILE,
    STATEMENT_L_STATEMENTS_R
};

enum CALL_TYPE {
    CALL_ID,
    CALL_ID_EXPLIST
};

class BaseClass {
public:
    virtual ~BaseClass() = default;
    virtual std::string getType() {std::cout << "error with getType()!" << std::endl;}
    virtual std::string getId() {std::cout << "error with getId()!" << std::endl;}
    virtual std::vector<std::string> getVecArgsType() {std::cout << "error with getVecArgsType()!" << std::endl;}
    virtual std::vector<std::string> getVecArgsID() {std::cout << "error with getVecArgsID()!" << std::endl;}
    virtual std::vector<std::string> getVecArgsValue() {std::cout << "error with getVecArgsValue()!" << std::endl;}
    virtual void addNewArg(std::string argType, std::string argID) {std::cout << "error with addNewArg()!" << std::endl;}
    virtual std::string getArgType() {std::cout << "error with getArgType()!" << std::endl;}
    virtual std::string getArgID() {std::cout << "error with getArgID()!" << std::endl;}
    virtual std::string getValue() {std::cout << "error with getValue()!" << std::endl;}
    virtual int getQuad() {std::cout << "error with getQuad()!" << std::endl;}
    virtual std::string getLabel() {std::cout << "error with getLabel()!" << std::endl;}
    virtual void addNewArgType(std::string argType, std::string argValue) {std::cout << "error with addNewArgType()!" << std::endl;}
    virtual std::string getRegName() {std::cout << "error with getReg()!" << std::endl;}
    virtual std::string getOpStr() {std::cout << "error with getOp()!" << std::endl;}
    virtual OP_TYPE getOpType() {std::cout << "error with ConvertOpToEnum()!" << std::endl;}
    virtual vector<pair<int,BranchLabelIndex>> getTruelist() {std::cout << "error with getTruelist()!" << std::endl;}
    virtual vector<pair<int,BranchLabelIndex>> getFalselist() {std::cout << "error with getFalselist()!" << std::endl;}
    virtual vector<pair<int,BranchLabelIndex>> getNextlist() {std::cout << "error with getNextlist()!" << std::endl;}
    virtual vector<pair<int,BranchLabelIndex>> getBreaklist() {std::cout << "error with getBreaklist()!" << std::endl;}
};

class RetTypeClass : public BaseClass {
private:
    std::string type;
public:
    RetTypeClass(std::string type);
    ~RetTypeClass() = default;
    std::string getType() override;
};

class TypeClass : public BaseClass {
private:
    std::string type;
public:
    TypeClass(std::string type);
    ~TypeClass() = default;
    std::string getType() override;
};

class IDClass : public BaseClass {
private:
    std::string id;
    int quad;
public:
    IDClass(std::string id, int quad);
    ~IDClass() = default;
    std::string getId() override;
    int getQuad() override;
};

class StringClass : public BaseClass {
private:
    StringRegister strReg;
    std::string value;
public:
    StringClass(std::string value);
    ~StringClass() = default;
    std::string getValue() override;
    std::string getRegName() override;
};

class NUMClass : public BaseClass {
private:
    std::string value;
public:
    NUMClass(std::string value);
    ~NUMClass() = default;
    std::string getValue() override;
};

class FormalsClass : public BaseClass {
private:
    std::vector<std::string> vecArgsType;
    std::vector<std::string> vecArgsID;
public:
    FormalsClass(std::vector<std::string> argsType = std::vector<std::string>(),
                 std::vector<std::string> argsID = std::vector<std::string>());
    ~FormalsClass() = default;
    std::vector<std::string> getVecArgsType() override;
    std::vector<std::string> getVecArgsID() override;
};

class FormalsListClass : public BaseClass {
private:
    std::vector<std::string> vecArgsType;
    std::vector<std::string> vecArgsID;
public:
    FormalsListClass(std::vector<std::string> argsType = std::vector<std::string>(),
                     std::vector<std::string> argsID = std::vector<std::string>());
    ~FormalsListClass() = default;
    std::vector<std::string> getVecArgsType() override;
    std::vector<std::string> getVecArgsID() override;
    void addNewArg(std::string argType, std::string argID) override;
};

class FormalDeclClass : public BaseClass {
private:
    std::string argType;
    std::string argID;
public:
    FormalDeclClass(std::string type, std::string id);
    ~FormalDeclClass() = default;
    std::string getArgType() override;
    std::string getArgID() override;
};

class ExpClass : public BaseClass {
private:
    std::string type;
    std::string value;
    Register reg;
    OP_TYPE opType;
    vector<pair<int,BranchLabelIndex>> truelist;
    vector<pair<int,BranchLabelIndex>> falselist;
public:
    ExpClass(OP_TYPE opType, std::string type, std::string value = std::string(),
             BaseClass* exp1 = nullptr, BaseClass* exp2 = nullptr, BaseClass* opExp = nullptr);
    ~ExpClass() = default;
    std::string getType() override;
    std::string getValue() override;
    std::string getRegName() override;
    vector<pair<int,BranchLabelIndex>> getTruelist() override;
    vector<pair<int,BranchLabelIndex>> getFalselist() override;

};

class ExpListClass : public BaseClass {
private:
    std::vector<std::string> vecArgsType;
    std::vector<std::string> vecArgsValue; //contains the reg names where the values are held.
public:
    ExpListClass(std::vector<std::string> vecArgsType = std::vector<std::string>(),
                std::vector<std::string> vecArgValues = std::vector<std::string>());
    ~ExpListClass() = default;
    std::vector<std::string> getVecArgsType() override;
    std::vector<std::string> getVecArgsValue() override;
    void addNewArgType(std::string argType, std::string argValue) override;
};

class StatementsClass : public BaseClass {
private:
    STATEMENTS_TYPE stType;
    vector<pair<int,BranchLabelIndex>> nextlist; //also continue cases
    vector<pair<int,BranchLabelIndex>> breaklist;
public:
    StatementsClass(STATEMENT_TYPE stsType, BaseClass* exp1 = nullptr, BaseClass* exp2 = nullptr, BaseClass* M1 = nullptr, BaseClass* M2 = nullptr);
    ~StatementsClass() = default;
    vector<pair<int,BranchLabelIndex>> getNextlist() override;
    vector<pair<int,BranchLabelIndex>> getBreaklist() override;
};

class StatementClass : public BaseClass {
private:
    STATEMENT_TYPE stType;
    vector<pair<int,BranchLabelIndex>> nextlist; //also continue cases
    vector<pair<int,BranchLabelIndex>> breaklist;
public:
    StatementClass(STATEMENT_TYPE stsType, BaseClass* exp1 = nullptr, BaseClass* exp2 = nullptr, BaseClass* M1 = nullptr, BaseClass* M2 = nullptr);
    ~StatementClass() = default;
    vector<pair<int,BranchLabelIndex>> getNextlist() override;
    vector<pair<int,BranchLabelIndex>> getBreaklist() override;
};

class CallClass : public BaseClass {
private:
    CALL_TYPE callType;
    std::string type;
    Register reg;
public:
    CallClass(CALL_TYPE callType,std::string type, BaseClass* exp1 = nullptr, BaseClass* exp2 = nullptr);
    ~CallClass() = default;
    std::string getType() override;
    std::string getRegName() override;
};

class OpClass : public BaseClass {
private:
    std::string opStr;
public:
    OpClass(std::string opStr);
    ~OpClass() = default;
    std::string getOpStr() override;
};

class M_Class : public BaseClass{
private:
    std::string label;
public:
    M_Class();
    ~M_Class() = default;
    std::string getLabel() override;
};

class N_Class : public BaseClass{
private:
    vector<pair<int,BranchLabelIndex>> nextlist;
public:
    N_Class();
    ~N_Class() = default;
    vector<pair<int,BranchLabelIndex>> getNextlist() override;
};

#endif //_RULES_TYPES_
