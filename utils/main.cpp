#include "Utils.hpp"
#include <iostream>

int main()
{
    std::vector<std::string> vec;

    vec = irc::split("arg1,arg2,arg3 k pass passs ", " ");
    for (std::vector<std::string>::const_iterator it = vec.begin(); it != vec.end(); it++)
        std::cout << *it << std::endl;
    vec = irc::split("arg1,arg2,arg3 k pass passs ", " p");
    for (std::vector<std::string>::const_iterator it = vec.begin(); it != vec.end(); it++)
        std::cout << *it << std::endl;
    vec = irc::split("arg1,arg2,arg3 k pass passs ", ",");
    for (std::vector<std::string>::const_iterator it = vec.begin(); it != vec.end(); it++)
        std::cout << *it << std::endl;
    vec = irc::split("", " ");
    for (std::vector<std::string>::const_iterator it = vec.begin(); it != vec.end(); it++)
        std::cout << *it << std::endl;
    vec = irc::split("arg1,arg2,arg3 k pass passs", "");
    for (std::vector<std::string>::const_iterator it = vec.begin(); it != vec.end(); it++)
        std::cout << *it << std::endl;
    vec = irc::split("", "");
    for (std::vector<std::string>::const_iterator it = vec.begin(); it != vec.end(); it++)
        std::cout << *it << std::endl;
    return 1;
}