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

bool Player::placeCards(Card choosenCard, Card descendingStack1, Card descendingStack2, Card ascendingStack1, Card ascendingStack2)
{
	if (!m_hand.empty())

	{
		if (choosenCard.getNumber() - descendingStack1.getNumber() == 10 or choosenCard.getNumber() < descendingStack1.getNumber())
			descendingStack1 = choosenCard;
		else
			if (choosenCard.getNumber() - descendingStack2.getNumber() == 10 or choosenCard.getNumber() - descendingStack2.getNumber())
				descendingStack2 = choosenCard;
			else
				if (ascendingStack1.getNumber() - choosenCard.getNumber() == 10 or choosenCard.getNumber() - ascendingStack1.getNumber())
					ascendingStack1 = choosenCard;
				else
					if (ascendingStack2.getNumber() - choosenCard.getNumber() == 10 or choosenCard.getNumber() - ascendingStack2.getNumber())
						ascendingStack2 = choosenCard;
					else
					{
						std::cout << "This card doesn't belong in any of the stacks!";
						return false;
					}
		return true;
	}
	else
	{
		std::cout << "Your hand is empty!!!\n";
		return false;
	}
}

void Player::takeCard(std::stack<Card>& Stack)
{
	if (m_hand.empty())
		m_hand.push_back(Stack.top());
	else
		std::cout << "You need to empty your hande before taking another card!";
	
}

void Player::viewProfile() const
{
	std::cout << "Username: " << m_name << "\n";
	std::cout << "Total hours played: " << m_hoursPlayed << "\n";
	std::cout << "Total number of games played: " << m_gamesPlayed << "\n";
	std::cout << "Score: " << m_performanceScore << "\n";
}

void Player::setUnusedCards(std::uint16_t numberOfcards)
{
	m_unusedCards = numberOfcards;
}

std::uint16_t Player::getUnusedCards()
{
	return m_unusedCards;
}
