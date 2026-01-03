#pragma once
#include <string>
#include <vector>

#include <crow.h>
#include <sqlite_orm/sqlite_orm.h>
namespace sql = sqlite_orm;

#include "Utils.h"
#include "User.h"
#include "Game.h"
#include "PowerUp.h"

namespace http
{

	struct InventoryEntry {
		uint32_t m_id;
		uint32_t userId;
		uint32_t powerUpId;


		void SetId(uint32_t id) { m_id = id; }
		uint32_t GetId() const { return m_id; }

		void SetUserId(uint32_t id) { userId = id; }
		uint32_t GetUserId() const { return userId; }

		void SetPowerUpId(uint32_t id) { powerUpId = id; }
		uint32_t GetPowerUpId() const { return powerUpId; }
	};

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
				sql::make_column("unusedCards", &User::SetUnusedCards, &User::GetUnusedCards)

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
				sql::make_column("id", &InventoryEntry::SetId, &InventoryEntry::GetId,sql::primary_key().autoincrement()),
				sql::make_column("userId", &InventoryEntry::SetUserId, &InventoryEntry::GetUserId),
				sql::make_column("powerUpId", &InventoryEntry::SetPowerUpId, &InventoryEntry::GetPowerUpId)
			)


		);

	}

	using DataBase = decltype(CreateDataBase(""));

	class UserProfiles
	{
	public:
		bool Initialize();

		std::vector<User> GetUsers();
		std::optional<User> GetUserById(int id);
		void AddUserToGame(const std::string& name, const std::string& password = "");
		bool HasUserWithName(const std::string& name);

		void AddPowerUpToUser(uint32_t userId, uint32_t powerUpId);
		std::vector<PowerUp> GetUserInventory(uint32_t userId);



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