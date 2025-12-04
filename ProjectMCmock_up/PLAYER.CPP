#include "Player.h"
#include <iostream>
#include <cmath>
Player::Player():
	m_name("Unknown"),
	m_hand(),
	m_hoursPlayed(0),
	m_wins(0),
	m_gamesPlayed(0),
	m_unusedCards(0),
	m_performanceScore(0)
{
}
Player::Player(const std::string& name) :
	m_name(name),
	m_hand(),
	m_hoursPlayed(0),
	m_wins(0),
	m_gamesPlayed(0),
	m_unusedCards(0),
	m_password(""),
	m_performanceScore(0)
{
}

Player::Player(int id, const std::string& name):
	m_id(id),
	m_name(name),
	m_hand(),
	m_hoursPlayed(0),
	m_wins(0),
	m_gamesPlayed(0),
	m_unusedCards(0),
	m_password(""),
	m_performanceScore(0)
{

}

Player::Player(std::string name, std::list<Card> hand, std::uint16_t hoursPlayed, std::uint16_t wins, std::uint16_t gamesPlayed, std::uint16_t unusedCards,std::string password)
	:m_name(name),
	m_hand(hand),
	m_hoursPlayed(hoursPlayed),
	m_wins(wins),
	m_gamesPlayed(gamesPlayed),
	m_unusedCards(unusedCards),
	m_password(password),
	m_performanceScore(0)
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



bool Player::PlaceCard(Card choosenCard, Card& descendingDeck1, Card& descendingDeck2, Card& ascendingDeck1, Card& ascendingDeck2)
{
	
	if (m_hand.empty())
	{
		std::cout << "Your hand is empty!!!\n";
		return false;
	}


	auto foundCard = std::find(m_hand.begin(), m_hand.end(), choosenCard);
	if (foundCard == m_hand.end())
	{
		std::cout << "This card isn't in your hand! Choose another one.";
		return false;
	}

	using enum Card::OrderDirection;
	auto canPlace = [](const Card& chosen, const Card& target, Card::OrderDirection dir) -> bool {
		if (dir == Descending) {
			
			return (chosen.GetNumber() - target.GetNumber() == 10) || (chosen.GetNumber() < target.GetNumber());
		}
		else { 
			
			return (target.GetNumber() - chosen.GetNumber() == 10) || (chosen.GetNumber() > target.GetNumber());
		}
		};

	
	struct Target {
		Card* card;
		Card::OrderDirection dir;
	};

	Target targets[] = {
		{ &descendingDeck1, Descending },
		{ &descendingDeck2, Descending },
		{ &ascendingDeck1,  Ascending  },
		{ &ascendingDeck2,  Ascending  }
	};

	for (auto& t : targets) {
		if (canPlace(choosenCard, *t.card, t.dir)) {
			
			*t.card = std::move(*foundCard);
			t.card->SetDirection(t.dir);

			
			m_hand.erase(foundCard);
			return true;
		}
	}

	std::cout << "This card doesn't belong in any of the stacks!";
	return false;
}


void Player::TakeCard(std::stack<Card>& Deck)
{
		m_hand.push_back(Deck.top());
		Deck.pop();
	
	
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

std::uint16_t Player::GetUnusedCards() const
{
	return m_unusedCards;
}

void Player::SetWins(std::uint16_t gamesWon)
{
	m_wins = gamesWon;
}

std::uint16_t Player::GetWins() const
{
	return m_wins;
}

void Player::SetId(std::uint32_t id)
{
	m_id = id;
}

std::uint32_t Player::GetId() const
{
	return m_id;
}

std::list<Card>& Player::GetHand() 
{
	return m_hand;
}

void Player::SetScore(std::uint16_t score)
{
	m_performanceScore = score;
}

std::uint16_t Player::GetScore() const
{
	return m_performanceScore;
}

void Player::SetName(std::string name)
{
	m_name = name;
}

std::string Player::GetName() const
{
	return m_name;
}

void Player::SetHoursPlayed(std::uint16_t hoursPlayed)
{
	m_hoursPlayed = hoursPlayed;
}

std::uint16_t Player::GetHoursPlayed() const
{
	return m_hoursPlayed;
}

std::string Player::GetPassword() const
{
	return m_password;
}

void Player::SetPassword(std::string password)
{
	m_password = password;
}
