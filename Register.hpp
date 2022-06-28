#ifndef HW5_REGISTER_HPP
#define HW5_REGISTER_HPP

#include <iostream>
#include <string>

class Register {
private:
    static int counter;
    std::string regName;
public:
    Register();
    ~Register() = default;
    void incCounter();
    std::string getRegName();
};

//there will be only 1 of this. every time we create an object of this class and call getName we will receive the correct reg.
//so maybe singleton? or static name enough?
class StackScopeRegister {
private:
    static int counter;
    static std::string regName;
public:
    StackScopeRegister();
    ~StackScopeRegister() = default;
    void incCounter();
    std::string getRegName();
    void setNewRegName(); //every time we allocate a new stack, we receive new address for it, and need to save it in new reg name.
};

class StringRegister {
private:
    static int counter;
    std::string regName;
public:
    StringRegister();
    ~StringRegister() = default;
    void incCounter();
    std::string getRegName();
};




#endif //HW5_REGISTER_HPP
