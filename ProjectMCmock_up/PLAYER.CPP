#include "Player.h"
#include <iostream>
#include <cmath>
void Player::score()
{
	if (m_gamesPlayed > 0)
	{
		double FloatScore = 5.0 * m_wins / (m_gamesPlayed + 0.5 * m_unusedCards);
		m_performanceScore = static_cast<uint16_t>(FloatScore);
	}
	else
		m_performanceScore = 0;
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
		std::cout << "You need to empty your hand before taking another card!";
	
}

void Player::viewProfile() const
{
	std::cout << "Username: " << m_name << "\n";
	std::cout << "Total hours played: " << m_hoursPlayed << "\n";
	std::cout << "Total number of games played: " << m_gamesPlayed << "\n";
	std::cout << "Score: " << m_performanceScore << "\n";
}

void Player::setUnusedCards(std::uint16_t numberOfCards)
{
	m_unusedCards = numberOfCards;
}

std::uint16_t Player::getUnusedCards()
{
	return m_unusedCards;
}

void Player::setWins(std::uint16_t gamesWon)
{
	m_wins = gamesWon;
}

std::uint16_t Player::getWins()
{
	return m_wins;
}

std::list<Card> Player::getHand()
{
	return m_hand;
}

std::uint16_t Player::getScore()
{
	return m_performanceScore;
}

std::string Player::getName()
{
	return m_name;
}
