#include "User.h"

User::User():
	m_name("Unknown"),
	m_hoursPlayed (0),
	m_wins(0),
	m_gamesPlayed(0),
	m_unusedCards (0),
	m_password ("12345678"),
	m_id (0),
	m_performanceScore (0)
{

}

User::User(const std::string& name):
	m_name(name),
	m_hoursPlayed(0),
	m_wins(0),
	m_gamesPlayed(0),
	m_unusedCards(0),
	m_password(""),
	m_performanceScore(0),
	m_id(0)
{
}

User::User(int id, const std::string& name):
m_id(id),
m_name(name),
m_hoursPlayed(0),
m_wins(0),
m_gamesPlayed(0),
m_unusedCards(0),
m_password(""),
m_performanceScore(0)
{
}

User::User(std::string name, std::uint16_t hoursPlayed, std::uint16_t wins, std::uint16_t gamesPlayed, std::uint16_t unusedCards, std::string password, std::uint32_t id, std::uint16_t performanceScore):
	m_name(name),
	m_hoursPlayed(hoursPlayed),
	m_wins(wins),
	m_gamesPlayed(gamesPlayed),
	m_unusedCards(unusedCards),
	m_password(password),
	m_id(id),
	m_performanceScore(0)
{
}
