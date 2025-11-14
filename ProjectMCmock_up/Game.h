#pragma once
#include "Player.h"
#include "Chat.h"

#include <vector>
#include <stack>

class Game
{
public:
	Game(std::vector<Player> players, std::stack<Card> cardsStack, Chat chat);
	Game() = default;
	void shuffleCards();
	bool endGame(int numberOfPlacedCards);
	void runGame();
	void displayGameState(const Card& descending1, const Card& descending2, const Card& ascending1, const Card& ascending2);
private:
	std::vector<Player> m_players;
	std::stack<Card> m_cardsStack;
	Chat m_chat;
};

