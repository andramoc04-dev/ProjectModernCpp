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

	Player(const Player& other);

	void Score();
	bool PlaceCard(Card choosenCard, Card& descendingDeck1, Card& descendingDeck2, Card& ascendingDeck1, Card& ascendingDeck2);
	void TakeCard(std::stack<Card>& Deck);
	
	std::list<Card>& GetHand();



private:
	
	std::list<Card> m_hand;
	
};

