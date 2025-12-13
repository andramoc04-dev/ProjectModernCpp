#pragma once
#include<string>
#include<vector>
#include"Message.h"


class Chat
{
public:
	Chat()=default;

	void AddMessage(const Message& message);
	std::vector<Message> GetMessages() const;
	void PrintMessages() const;
private:
	std::vector<Message>m_messages;
};

