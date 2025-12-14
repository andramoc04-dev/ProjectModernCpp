#pragma once
#include "Card.h"
#include <cstdint>
#include <string>

class User
{
public:
	User(); 

	User(const std::string& username);

	User(int id, const std::string& username);

	User(
		std::uint32_t id,
		const std::string& username,
		const std::string& password,
		std::uint16_t score,
		std::uint16_t wins,
		std::uint16_t hoursPlayed,
		std::uint16_t unusedCards
	);

	User(const User& other);

	void SetUsername(const std::string& username);
	std::string GetUsername() const;

	void SetHoursPlayed(std::uint16_t hoursPlayed);
	std::uint16_t GetHoursPlayed() const;

	void SetPassword(const std::string& password);
	std::string GetPassword() const;
	
	void SetId(std::uint32_t id);
	std::uint32_t GetId() const;

	void SetScore(std::uint16_t score);
	std::uint16_t GetScore() const;

	void SetWins(std::uint16_t gamesWon);
	std::uint16_t GetWins() const;

	void SetUnusedCards(std::uint16_t numberOfCards);
	std::uint16_t GetUnusedCards() const;

	void SetGamesPlayed(std::uint16_t gamesPlayed);
	std::uint16_t GetGamesPlayed() const;

	void ViewProfile() const;


protected:
	std::uint32_t m_id;
	std::string m_password;
	std::string m_username;
	std::uint16_t m_hoursPlayed;
	std::uint16_t m_wins;
	std::uint16_t m_gamesPlayed;
	std::uint16_t m_unusedCards;
	std::uint16_t m_performanceScore;
};

