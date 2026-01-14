#pragma once
#include<string>
#include<deque>
#include"Message.h"


class Chat
{
public:
	Chat()=default;

	void AddMessage(const Message& message);
	std::deque<Message> GetMessages() const;
	void PrintMessages() const;
private:
	std::deque<Message>m_messages;
};

