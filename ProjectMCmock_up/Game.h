#pragma once
#include "Player.h"
#include "Chat.h"

#include <vector>
#include <stack>

class Game
{
public:
	Game(const std::vector<Player>& players, const std::stack<Card>& cardsDeck, const Chat& chat);
	Game() = default;
	void ShuffleCards();
	bool EndGame(int numberOfPlacedCards);
	void RunGame();
	void DisplayGameState(const Card& descending1, const Card& descending2, const Card& ascending1, 
		const Card& ascending2, const Player& player);
	void CardsDistribution();
	bool GetAreCardsDealt() const;
	void SetCardsDeck(std::stack<Card> cardsDeck);
private:
	std::vector<Player> m_players;
	std::stack<Card> m_cardsDeck;
	Chat m_chat;
	Card m_descendingDeck1, m_descendingDeck2, m_ascendingDeck1, m_ascendingDeck2;
	bool m_areCardsDealt;
};

