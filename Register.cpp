#include "Register.hpp"

int Register::counter = 0;

Register::Register(){
    this->regName = "%t_" + std::to_string(this->counter);
    incCounter();
}

void Register::incCounter() {
    this->counter++;
}

std::string Register::getRegName() {
    return this->regName;
}

int StackScopeRegister::counter = 0;
std::string StackScopeRegister::regName = std::string();

StackScopeRegister::StackScopeRegister() {}

std::string StackScopeRegister::getRegName() {
    return this->regName;
}

void StackScopeRegister::setNewRegName() {
    this->regName = "%stackPtr_" + std::to_string(this->counter);
    incCounter();
}

void StackScopeRegister::incCounter() {
    this->counter++;
}


int StringRegister::counter = 0;

StringRegister::StringRegister(){
    this->regName = "@str_" + std::to_string(this->counter);
    incCounter();
}

void StringRegister::incCounter() {
    this->counter++;
}

std::string StringRegister::getRegName() {
    return this->regName;
}
