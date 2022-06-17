#include "Register.h"

int Register::counter = 0;

Register::Register(){
    this->regName = "%t" + std::to_string(this->counter);
    incCounter();
}

void Register::incCounter() {
    this->counter++;
}

std::string Register::getRegName() {
    return this->regName;
}

