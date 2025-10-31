#pragma once
#include<string>

class Chat
{
public:
	std::string setMessage();
	void printMessage();
private:
	std::string m_message;
	std::string m_name;
};

