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


#endif //HW5_REGISTER_HPP