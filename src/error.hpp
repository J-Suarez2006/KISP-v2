#ifndef ERROR_HPP
#define ERROR_HPP

#include <iostream>
#include <string>

static void syntaxError(int line, std::string message)
{
    std::cerr << "Error at line " << line << ": " << message << std::endl;
}

#endif