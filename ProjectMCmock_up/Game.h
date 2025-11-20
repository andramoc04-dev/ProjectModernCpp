#pragma once
#include "Player.h"
#include "Chat.h"

#include <vector>
#include <stack>

class Game
{
public:
	Game(std::vector<Player>& players, std::stack<Card>& cardsDeck, Chat& chat);
	Game() = default;
	void ShuffleCards();
	bool EndGame(int numberOfPlacedCards);
	void RunGame();
	void DisplayGameState(const Card& descending1, const Card& descending2, const Card& ascending1, 
		const Card& ascending2, Player& player);
	void CardsDistribution();
private:
	std::vector<Player> m_players;
	std::stack<Card> m_cardsDeck;
	Chat m_chat;
};

