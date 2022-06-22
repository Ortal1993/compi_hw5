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
    this->counter++;
}
