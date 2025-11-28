#include <iostream>
#include "Game.h"
#include "Chat.h"
#include "Database.h"
#include "GameServer.h"

using namespace http;

int main()
{
	/*Player player1("Viorica");
	Player player2("Marcel");
	std::vector<Player> players{ player1, player2 };
	std::stack<Card> cards;
	Chat chat = Chat();
	Game game(players, cards, chat);
	game.RunGame();*/


	PlayerProfiles storage;
	if (!storage.Initialize())
	{
		std::cout << "Faild to initialize the database!";
		return -1;
	}
	else std::cout << "Database initialized";
	Routing r;
	r.Run(storage);
	
	return 0;
}