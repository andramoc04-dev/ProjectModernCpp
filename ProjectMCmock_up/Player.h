#pragma once
#include "Card.h"
#include "User.h"
#include <cstdint>
#include <list>
#include <string>
#include <stack>


class Player : public User
{
public:

	Player();

	Player(const std::string& name);

	Player(int id, const std::string& name);

	/*Player(
		std::string name,
		std::list<Card> hand,
		std::uint16_t hoursPlayed = 0,
		std::uint16_t wins = 0,
		std::uint16_t gamesPlayed = 0,
		std::uint16_t unusedCards = 0,
		std::string password="",
		std::uint16_t performanceScore
	);*/

	void Score();
	bool PlaceCard(Card choosenCard, Card& descendingDeck1, Card& descendingDeck2, Card& ascendingDeck1, Card& ascendingDeck2);
	void TakeCard(std::stack<Card>& Deck);
	
	std::list<Card>& GetHand();



private:
	
	std::list<Card> m_hand;
	
};

