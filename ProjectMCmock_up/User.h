#pragma once
#include "Card.h"
#include <cstdint>
#include <string>

class User
{
public:
	User(); 

	User(const std::string& name);

	User(int id, const std::string& name);

	User(
		std::string name,
		std::uint16_t hoursPlayed = 0,
		std::uint16_t wins = 0,
		std::uint16_t gamesPlayed = 0,
		std::uint16_t unusedCards = 0,
		std::string password = "",
		std::uint32_t id = 0,
		std::uint16_t performanceScore=0
	);
private:
	std::uint32_t m_id;
	std::string m_password;
	std::string m_name;
	std::uint16_t m_hoursPlayed;
	std::uint16_t m_wins;
	std::uint16_t m_gamesPlayed;
	std::uint16_t m_unusedCards;
	std::uint16_t m_performanceScore;
};

