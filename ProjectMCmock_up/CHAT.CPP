#include "Chat.h"
#include<cctype>
#include<iostream>
#include <chrono>
#include <format>

using namespace std::chrono;

void Chat::AddMessage(Message message)
{
    m_messages.push_back(message);
}

void Chat::PrintMessages()
{
    
    for (const auto& message : m_messages)
    {
      std::cout << std::format("[{:%d.%m.%Y %H:%M:%S}] {}: {}", 
                                message.GetTimeStamp(), 
                                message.GetPlayerName(), 
                                message.GetTextFromPlayer()) << std::endl;
    }
    
}


