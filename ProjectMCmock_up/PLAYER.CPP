#include "Player.h"
#include <iostream>
#include <cmath>
std::uint8_t Player::score() const
{
	if (m_wins > 0)
		return 5 * (abs(m_wins / m_gamesPlayed - m_gamesPlayed / m_unusedCards));
	else
		return 0;
}

bool Player::placeCard()
{


	return false;
}

void Player::viewProfile() const
{
	std::cout << "Username: " << m_name << "\n";
	std::cout << "Total hours played: " << m_hoursPlayed << "\n";
	std::cout << "Total number of games played: " << m_gamesPlayed << "\n";
	std::cout << "Score: " << m_performanceScore << "\n";
}
