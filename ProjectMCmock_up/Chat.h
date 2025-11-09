#pragma once
#include<string>

class Chat
{
public:
	Chat(const std::string& name, const std::string& message)
		: m_name(name), m_message(message) {}

	void setMessage(std::string m_message);
	void printMessage(std::string m_name,std::string m_message);
private:
	std::string m_message;
	std::string m_name;
};

