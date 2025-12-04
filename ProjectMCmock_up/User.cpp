#include "User.h"

User::User():
	m_username("Unknown"),
	m_hoursPlayed (0),
	m_wins(0),
	m_gamesPlayed(0),
	m_unusedCards (0),
	m_password ("12345678"),
	m_id (0),
	m_performanceScore (0)
{

}

User::User(const std::string& username):
	m_username(username),
	m_hoursPlayed(0),
	m_wins(0),
	m_gamesPlayed(0),
	m_unusedCards(0),
	m_password(""),
	m_performanceScore(0),
	m_id(0)
{
}

User::User(int id, const std::string& username):
m_id(id),
m_username(username),
m_hoursPlayed(0),
m_wins(0),
m_gamesPlayed(0),
m_unusedCards(0),
m_password(""),
m_performanceScore(0)
{
}

User::User(std::string username, std::uint16_t hoursPlayed, std::uint16_t wins, std::uint16_t gamesPlayed, std::uint16_t unusedCards, std::string password, std::uint32_t id, std::uint16_t performanceScore):
	m_username(username),
	m_hoursPlayed(hoursPlayed),
	m_wins(wins),
	m_gamesPlayed(gamesPlayed),
	m_unusedCards(unusedCards),
	m_password(password),
	m_id(id),
	m_performanceScore(0)
{
}

void User::SetUsername(std::string name)
{
	m_username = name;
}

std::string User::GetUsername() const
{
	return m_username;
}

void User::SetUnusedCards(std::uint16_t numberOfCards)
{
	m_unusedCards = numberOfCards;
}

std::uint16_t User::GetUnusedCards() const
{
	return m_unusedCards;
}

void User::SetGamesPlayed(std::uint16_t gamesPlayed)
{
	m_gamesPlayed = gamesPlayed;
}

std::uint16_t User::GetGamesPlayed() const
{
	return m_gamesPlayed;
}

void User::SetWins(std::uint16_t gamesWon)
{
	m_wins = gamesWon;
}

std::uint16_t User::GetWins() const
{
	return m_wins;
}

void User::SetId(std::uint32_t id)
{
	m_id = id;
}

std::uint32_t User::GetId() const
{
	return m_id;
}

void User::SetScore(std::uint16_t score)
{
	m_performanceScore = score;
}

std::uint16_t User::GetScore() const
{
	return m_performanceScore;
}

void User::SetHoursPlayed(std::uint16_t hoursPlayed)
{
	m_hoursPlayed = hoursPlayed;
}

std::uint16_t User::GetHoursPlayed() const
{
	return m_hoursPlayed;
}

std::string User::GetPassword() const
{
	return m_password;
}

void User::SetPassword(std::string password)
{
	m_password = password;
}