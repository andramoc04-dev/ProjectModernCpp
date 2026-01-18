#pragma once
#include<deque>
#include"Message.h"


class Chat
{
public:
	Chat()=default;

	void AddMessage(const Message& message);
	const std::deque<Message>& GetMessages() const;
private:
	std::deque<Message>m_messages;
	static const std::uint8_t MAX_CHAT_MESSAGES = 8;
};

