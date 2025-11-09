#include <iostream>

#include "Game.h";
#include "Chat.h"

int main()
{
	Player player1("Viorica");
	Player player2("Marcel");
	std::vector<Player> players{ player1, player2 };
	std::stack<Card> cards;
	Chat chat=Chat();
	Game game(players, cards, chat);
	game.runGame();
	return 0;
}