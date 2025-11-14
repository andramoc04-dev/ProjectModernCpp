#pragma once
#include<string>
#include<chrono>
#include"Player.h"

class Message {
public:
	Message();

	std::string GetPlayerName()const;
	std::string GetTextFromPlayer()const;
	std::chrono::system_clock::time_point GetTimeStamp() const;

	void SetPlayerName(Player player);
	void SetTextFromPlayer(std::string text);
	void SetTimeStamp(std::chrono::system_clock::time_point time);
	

private:
	std::string m_text;
	std::string m_player_name;
	std::chrono::system_clock::time_point m_time;
};