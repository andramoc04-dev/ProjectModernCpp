#pragma once
#include<string>

class Chat
{
public:
	void setMessage(std::string m_message);
	void printMessage();
private:
	std::string m_message;
	std::string m_name;
};

