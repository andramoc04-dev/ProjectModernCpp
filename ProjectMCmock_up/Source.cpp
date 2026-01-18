#include <iostream>
#include "Game.h"
#include "Chat.h"
#include "Database.h"
#include "GameServer.h"

using namespace http;

int main()
{
	
	UserProfiles storage;
	if (!storage.Initialize())
	{
		return -1;
	}

	storage.InitializePowerUps();

	Routing r;
	r.Run(storage);
	
	return 0;
}