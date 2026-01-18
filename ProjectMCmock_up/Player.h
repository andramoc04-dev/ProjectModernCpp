#pragma once
#include "Card.h"
#include "User.h"
#include <cstdint>
#include <unordered_set>
#include <string>
#include <stack>



class Player : public User
{
public:

	Player();

	Player(int id, const std::string& name);

	Player(const Player& other);

	void Score();
	bool PlaceCard(const Card& choosenCard, 
		Card& descendingDeck1,
		Card& descendingDeck2, 
		Card& ascendingDeck1, 
		Card& ascendingDeck2,
		int pileId);
	
	std::unordered_set<Card>& GetHand();
	const std::unordered_set<Card>& GetHand() const;

	
	void IncrementCardsPlacedThisTurn();
	int GetCardsPlacedThisTurn() const;
	void ResetCardsPlacedThisTurn();
	
	static Player ConvertUserToPlayer(const User& user);

private:
	
	std::unordered_set<Card> m_hand;
	const int NoPlacedCards = 0;
	int m_numberOfPlacedCards = NoPlacedCards;
};

