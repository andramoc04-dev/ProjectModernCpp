#pragma once
#include<string>
#include"Player.h"

class Message {
public:
	Message(Player player,std::string message);

	std::string GetMessage();
	void SetMessage(std::string message);

private:
	std::string m_message;
	std::string m_player_name;
};