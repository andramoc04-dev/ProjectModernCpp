#include "Chat.h"
#include <iostream>
#include <format>


void Chat::AddMessage(const Message& message)
{
    m_messages.push_back(message);
	while (m_messages.size() > MAX_CHAT_MESSAGES)
	{
		m_messages.pop_front();
	}
}

const std::deque<Message>& Chat::GetMessages() const
{
    return m_messages;
}




