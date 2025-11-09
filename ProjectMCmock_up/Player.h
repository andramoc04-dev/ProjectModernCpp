#pragma once
#include "Card.h"

#include <cstdint>
#include <list>
#include <string>
#include <stack>
class Player
{
public:

	Player();

	Player(const std::string& name);

	Player(
		std::string name,
		std::list<Card> hand,
		std::uint16_t hoursPlayed = 0,
		std::uint16_t wins = 0,
		std::uint16_t gamesPlayed = 0,
		std::uint16_t unusedCards = 0
	);

	void score();
	bool placeCards(Card choosenCard,Card descendingStack1, Card descendingStack2, Card ascendingStack1, Card ascendingStack2);
	void takeCard(std::stack<Card>& Stack);
	void viewProfile() const;

	void setUnusedCards(std::uint16_t numberOfCards);
	std::uint16_t getUnusedCards();

	void setWins(std::uint16_t gamesWon);
	std::uint16_t getWins();
	std::list<Card> getHand();

	std::uint16_t getScore();

	std::string getName();
private:
	std::string m_name;
	std::list<Card> m_hand;
	std::uint16_t m_hoursPlayed;
	std::uint16_t m_wins;
	std::uint16_t m_gamesPlayed;
	std::uint16_t m_unusedCards;
	std::uint16_t m_performanceScore;
};

