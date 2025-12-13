#pragma once
#include<string>
#include<chrono>
#include"Player.h"

class Message {
public:
	Message();
	Message(const std::string& text, const std::string& playerName, std::chrono::system_clock::time_point m_time);

	std::string GetPlayerName()const;
	std::string GetTextFromPlayer()const;
	std::chrono::system_clock::time_point GetTimeStamp() const;

	void SetPlayerName(const Player& player);
	void SetTextFromPlayer(const std::string& text);
	void SetTimeStamp(std::chrono::system_clock::time_point time);
	

private:
	std::string m_text;
	std::string m_playerName;
	std::chrono::system_clock::time_point m_time;
};