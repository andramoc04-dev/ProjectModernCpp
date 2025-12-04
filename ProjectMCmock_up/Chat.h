#pragma once
#include<string>
#include<vector>
#include"Message.h"

class Chat
{
public:
	Chat()=default;

	void AddMessage(User player, std::string text);
	void PrintMessages();
private:
	std::vector<Message>m_messages;
};

