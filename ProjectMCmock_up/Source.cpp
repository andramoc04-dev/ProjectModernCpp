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
		std::cout << "Faild to initialize the database!";
		return -1;
	}
	else std::cout << "Database initialized";
	storage.InitializePowerUps();
	auto allPowerUps = storage.GetAllPowerUps();
	std::cout << "\n=== Power-ups Verification ===" << std::endl;
	std::cout << "Total power-ups in database: " << allPowerUps.size() << std::endl;

	if (allPowerUps.empty()) {
		std::cout << "WARNING: No power-ups found in database!" << std::endl;
	}
	else {
		std::cout << "Power-ups list:" << std::endl;
		for (const auto& powerUp : allPowerUps) {
			std::cout << "  - ID: " << powerUp.GetDatabaseId()
				<< " | Type: " << powerUp.GetTypeString()
				<< " | Name: " << powerUp.GetName()
				<< " | Description: " << powerUp.GetDescription() << std::endl;
		}
	}
	std::cout << "============================\n" << std::endl;

	Routing r;
	r.Run(storage);
	
	return 0;
}