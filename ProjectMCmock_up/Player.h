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
		std::uint16_t unusedCards = 0,
		std::string password=""
	);

	void Score();
	bool PlaceCard(Card choosenCard, Card& descendingStack1, Card& descendingStack2, Card& ascendingStack1, Card& ascendingStack2);
	void TakeCard(std::stack<Card>& Stack);
	void ViewProfile() const;

	void SetUnusedCards(std::uint16_t numberOfCards);
	std::uint16_t GetUnusedCards() const;

	void SetWins(std::uint16_t gamesWon);
	std::uint16_t GetWins() const;
	std::list<Card>& GetHand();

	std::uint16_t GetScore()const;
	void SetName(std::string name);
	std::string GetName()const;
	std::string GetPassword() const;
	void SetPassword(std::string password);

private:
	std::string m_password;
	std::string m_name;
	std::list<Card> m_hand;
	std::uint16_t m_hoursPlayed;
	std::uint16_t m_wins;
	std::uint16_t m_gamesPlayed;
	std::uint16_t m_unusedCards;
	std::uint16_t m_performanceScore;
};

