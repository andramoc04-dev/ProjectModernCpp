#pragma once
#include "Card.h"
#include "User.h"
#include "PowerUp.h"
#include <cstdint>
#include <unordered_set>
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
	bool PlaceCard(const Card& choosenCard, 
		Card& descendingDeck1,
		Card& descendingDeck2, 
		Card& ascendingDeck1, 
		Card& ascendingDeck2,
		int pileId);
	void TakeCard(std::stack<Card>& Deck);
	
	std::unordered_set<Card>& GetHand();
	const std::unordered_set<Card>& GetHand() const;
	bool GetHasPowerUpsReady() const;

	
	void IncrementCardsPlacedThisTurn();
	int GetCardsPlacedThisTurn() const;
	void ResetCardsPlacedThisTurn();
	
	static Player ConvertUserToPlayer(const User& user);

	void AddPowerUp(const PowerUp& powerUp);
	const std::vector<PowerUp>& GetInventory() const;

	void SetHasPowerUpsReady(bool hasPowerUpsReady);
private:
	
	std::unordered_set<Card> m_hand;
	int m_numberOfPlacedCards = 0;
	std::vector<PowerUp> m_inventory;
	bool m_hasPowerUpsReady = false;
};

