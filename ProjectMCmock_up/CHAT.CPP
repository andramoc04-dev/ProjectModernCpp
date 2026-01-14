#include "Chat.h"
#include<cctype>
#include<iostream>
#include <chrono>
#include <format>

using namespace std::chrono;

void Chat::AddMessage(const Message& message)
{
    m_messages.push_back(message);
}

std::deque<Message> Chat::GetMessages() const
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


