#include "Player.h"
#include <iostream>
std::uint8_t Player::score() const
{
    return std::uint8_t();
}

void Player::viewProfile() const
{
	std::cout << "Username: " << m_name << "\n";
	std::cout << "Total hours played: " << m_hoursPlayed << "\n";
	std::cout << "Total number of games played: " << m_gamesPlayed << "\n";
	std::cout << "Score: " << m_performanceScore << "\n";
}
