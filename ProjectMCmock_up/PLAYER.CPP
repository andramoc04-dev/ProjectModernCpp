#include "Player.h"
#include <iostream>
#include <cmath>
Player::Player():
	m_name("Unknown"),
	m_hand(),
	m_hoursPlayed(0),
	m_wins(0),
	m_gamesPlayed(0),
	m_unusedCards(0)
{
}
Player::Player(const std::string& name):
	m_name(name),
	m_hand(),
	m_hoursPlayed(0),
	m_wins(0),
	m_gamesPlayed(0),
	m_unusedCards(0)
{
}
Player::Player(std::string name, std::list<Card> hand, std::uint16_t hoursPlayed, std::uint16_t wins, std::uint16_t gamesPlayed, std::uint16_t unusedCards)
	:m_name(name),
	m_hand(hand),
	m_hoursPlayed(hoursPlayed),
	m_wins(wins),
	m_gamesPlayed(gamesPlayed),
	m_unusedCards(unusedCards)
{
}
void Player::Score()
{
	if (m_gamesPlayed > 0)
	{
		double FloatScore = 5.0 * m_wins / (m_gamesPlayed + 0.5 * m_unusedCards);
		m_performanceScore = static_cast<uint16_t>(FloatScore);
	}
	else
		m_performanceScore = 0;
}

bool Player::PlaceCard(Card choosenCard, Card descendingStack1, Card descendingStack2, Card ascendingStack1, Card ascendingStack2)
{
	if (!m_hand.empty())

	{
		if (choosenCard.GetNumber() - descendingStack1.GetNumber() == 10 or choosenCard.GetNumber() < descendingStack1.GetNumber())
			descendingStack1 = choosenCard;
		else
			if (choosenCard.GetNumber() - descendingStack2.GetNumber() == 10 or choosenCard.GetNumber() - descendingStack2.GetNumber())
				descendingStack2 = choosenCard;
			else
				if (ascendingStack1.GetNumber() - choosenCard.GetNumber() == 10 or choosenCard.GetNumber() - ascendingStack1.GetNumber())
					ascendingStack1 = choosenCard;
				else
					if (ascendingStack2.GetNumber() - choosenCard.GetNumber() == 10 or choosenCard.GetNumber() - ascendingStack2.GetNumber())
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

void Player::TakeCard(std::stack<Card>& Stack)
{
		m_hand.push_back(Stack.top());
		Stack.pop();
	
	
}

void Player::ViewProfile() const
{
	std::cout << "Username: " << m_name << "\n";
	std::cout << "Total hours played: " << m_hoursPlayed << "\n";
	std::cout << "Total number of games played: " << m_gamesPlayed << "\n";
	std::cout << "Score: " << m_performanceScore << "\n";
}

void Player::SetUnusedCards(std::uint16_t numberOfCards)
{
	m_unusedCards = numberOfCards;
}

std::uint16_t Player::GetUnusedCards()
{
	return m_unusedCards;
}

void Player::SetWins(std::uint16_t gamesWon)
{
	m_wins = gamesWon;
}

std::uint16_t Player::GetWins()
{
	return m_wins;
}

std::list<Card> Player::GetHand()
{
	return m_hand;
}

std::uint16_t Player::GetScore()
{
	return m_performanceScore;
}

std::string Player::GetName()
{
	return m_name;
}
