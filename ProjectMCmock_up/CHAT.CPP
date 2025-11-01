#include "Chat.h"
#include<cctype>
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
