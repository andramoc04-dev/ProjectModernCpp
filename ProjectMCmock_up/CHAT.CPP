#include "Chat.h"
#include<cctype>
#include<iostream>
void Chat::setMessage(std::string message)
{
    bool contains_numbers = false;
    for (char c : message)
    {
        if (std::isdigit(c)) {
            contains_numbers = true;
            break;
        }
    }
    if (contains_numbers)
        m_message = "###";
    else
        m_message = message;
}

void Chat::printMessage(std::string m_name,std::string m_message)
{
    std::cout <<m_name<<": "<< m_message << std::endl;
}


