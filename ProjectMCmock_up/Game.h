#pragma once
#include "Player.h"
#include "Chat.h"

#include <vector>
#include <stack>

class Game
{
public:
	void shuffleCards();
	void endGame();
	void runGame();
private:
	std::vector<Player> m_players;
	std::stack<Card> m_cardsStack;
	Chat m_chat;
};

