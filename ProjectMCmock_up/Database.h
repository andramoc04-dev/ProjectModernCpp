#pragma once
#include <string>
#include <vector>

#include <crow.h>
#include <sqlite_orm/sqlite_orm.h>
namespace sql = sqlite_orm;

#include "User.h"
#include "PowerUp.h"
#include "Inventory.h"

namespace http
{

	

	inline auto CreateDataBase(const std::string& filename)
	{
		return sql::make_storage(
			filename,
			sql::make_table(
				"Users",
				sql::make_column("id", &User::SetId, &User::GetId, sql::primary_key().autoincrement()),
				sql::make_column("name", &User::SetUsername, &User::GetUsername),
				sql::make_column("password", &User::SetPassword, &User::GetPassword),
				sql::make_column("score", &User::SetScore, &User::GetScore),
				sql::make_column("wins", &User::SetWins, &User::GetWins),
				sql::make_column("hoursPlayed", &User::SetHoursPlayed, &User::GetHoursPlayed),
				sql::make_column("unusedCards", &User::SetUnusedCards, &User::GetUnusedCards),
				sql::make_column("gamesPlayed", &User::SetGamesPlayed, &User::GetGamesPlayed)

			),


			sql::make_table(
				"PowerUps",
				sql::make_column("id", &PowerUp::SetDatabaseId, &PowerUp::GetDatabaseId, sql::primary_key().autoincrement()),
				sql::make_column("type", &PowerUp::SetTypeString, &PowerUp::GetTypeString),
				sql::make_column("name", &PowerUp::SetName, &PowerUp::GetName),
				sql::make_column("description", &PowerUp::SetDescription, &PowerUp::GetDescription)
			),

			sql::make_table(
				"UserPowerUps",
				sql::make_column("id", &Inventory::SetId, &Inventory::GetId,sql::primary_key().autoincrement()),
				sql::make_column("userId", &Inventory::SetUserId, &Inventory::GetUserId),
				sql::make_column("powerUpId", &Inventory::SetPowerUpId, &Inventory::GetPowerUpId),
				sql::make_column("isEquipped", &Inventory::SetIsEquipped, &Inventory::GetIsEquipped)
			)


		);

	}

	using DataBase = decltype(CreateDataBase(""));

	class UserProfiles
	{
	public:
		bool Initialize();

		std::vector<User> GetUsers();
		
		void AddUserToGame(const std::string& name, const std::string& password = "");
		bool HasUserWithName(const std::string& name);
		void UpdateUser(const User& user);

		void InitializePowerUps();
		std::vector<PowerUp> GetAllPowerUps();
		
		std::optional<PowerUp> GetPowerUpByType(PowerUpType type);
		uint32_t AddPowerUpToDataBase(const PowerUp& powerUp);

		void AddPowerUpToUser(uint32_t userId, uint32_t powerUpId);
		void RemovePowerUpFromUser(uint32_t userId, uint32_t powerUpId);
		std::vector<PowerUp> GetUserInventory(uint32_t userId);
		std::vector<Inventory> GetUserInventoryEntries(uint32_t userId);
		bool UserHasPowerUp(uint32_t userId, uint32_t powerUpId);
		int GetPowerUpCount(uint32_t userId, PowerUpType type);


		void EquipPowerUp(uint32_t userId, uint32_t inventoryId);
		void UnequipPowerUp(uint32_t userId, uint32_t inventoryId);
		std::vector<PowerUp> GetEquippedPowerUps(uint32_t userId);
		bool IsPowerUpEquipped(uint32_t userId, uint32_t powerUpId);


		template<typename T>
		std::optional<T> GetById(int id)
		{
			try {
				return m_db.get<T>(id);
			}
			catch (const std::system_error&) {
				return std::nullopt;
			}
		}


	private:
		const std::string kDbFile{ "players.sqlite" };

	private:
		DataBase m_db = CreateDataBase(kDbFile);
	};

	class AddUserToGameHandler {
	public:
		AddUserToGameHandler(UserProfiles& profiles);

		crow::response operator() (const crow::request& req);

	private:
		UserProfiles& m_db;
	};

}