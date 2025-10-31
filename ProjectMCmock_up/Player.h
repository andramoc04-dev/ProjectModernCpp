#pragma once
#include "Card.h"

#include <cstdint>
#include <list>
#include <string>

class Player
{
public:
	Player(
		std::string name,
		std::list<Card> hand,
		std::uint16_t hoursPlayed = 0,
		std::uint16_t wins = 0,
		std::uint16_t gamesPlayed = 0,
		std::uint16_t unusedCards = 0
	);

	std::uint8_t score() const;
	void placeCard();
	void takeCard();
	void viewProfile() const;

private:
	std::string m_name;
	std::list<Card> m_hand;
	std::uint16_t m_hoursPlayed;
	std::uint16_t m_wins;
	std::uint16_t m_gamesPlayed;
	std::uint16_t m_unusedCards;
	std::uint8_t m_performanceScore;
};

