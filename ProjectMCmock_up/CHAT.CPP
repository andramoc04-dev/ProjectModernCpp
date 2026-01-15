#include "Chat.h"
#include <iostream>
#include <format>


void Chat::AddMessage(const Message& message)
{
    m_messages.push_back(message);
}

const std::deque<Message>& Chat::GetMessages() const
{
    return m_messages;
}

void Chat::PrintMessages() const
{
    
    for (const auto& message : m_messages)
    {
      std::cout << std::format(" {}: {}",  
                                message.GetPlayerName(), 
                                message.GetTextFromPlayer()) << std::endl;
    }
    
}


