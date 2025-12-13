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

User::User(std::uint32_t id, const std::string& username, const std::string& password, 
	std::uint16_t score, std::uint16_t wins, std::uint16_t hoursPlayed, std::uint16_t unusedCards):
		m_id(id),
	m_username(username),
	m_password(password),
	m_performanceScore(score),
	m_wins(wins),
	m_hoursPlayed(hoursPlayed),
	m_unusedCards(unusedCards)
{
}

User::User(const User& other)
	: m_id(other.m_id),
	m_username(other.m_username),
	m_password(other.m_password),
	m_hoursPlayed(other.m_hoursPlayed),
	m_wins(other.m_wins),
	m_gamesPlayed(other.m_gamesPlayed),
	m_unusedCards(other.m_unusedCards),
	m_performanceScore(other.m_performanceScore)
{
}



void User::SetUsername(const std::string& username)
{
	m_username = username;
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

void User::ViewProfile() const
{
	std::cout << "Username: " << m_username << "\n";
	std::cout << "Total hours played: " << m_hoursPlayed << "\n";
	std::cout << "Total number of games played: " << m_gamesPlayed << "\n";
	std::cout << "Score: " << m_performanceScore << "\n";
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

void User::SetPassword(const std::string& password)
{
	m_password = password;
}