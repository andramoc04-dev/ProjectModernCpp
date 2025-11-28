#pragma once
#include <string>
#include <vector>

#include <crow.h>
#include <sqlite_orm/sqlite_orm.h>
namespace sql = sqlite_orm;

#include "Utils.h"
#include "Player.h"
#include "Game.h"

namespace http
{
	inline auto CreateDataBase(const std::string& filename)
	{
		return sql::make_storage(
			filename,
			sql::make_table(
				"Players",
				sql::make_column("id", &Player::SetId, &Player::GetId, sql::primary_key().autoincrement()),
				sql::make_column("name", &Player::SetName, &Player::GetName),
				sql::make_column("password", &Player::SetPassword, &Player::GetPassword),
				sql::make_column("score", &Player::SetScore, &Player::GetScore),
				sql::make_column("wins", &Player::SetWins, &Player::GetWins),
				sql::make_column("hoursPlayed", &Player::SetHoursPlayed, &Player::GetHoursPlayed),
				sql::make_column("unusedCards", &Player::SetUnusedCards, &Player::GetUnusedCards)

			)
		);
	}

	using DataBase = decltype(CreateDataBase(""));

	class PlayerProfiles
	{
	public:
		bool Initialize();

		std::vector<Player> GetPlayers();
		std::optional<Player> GetPlayerById(int id);
		void AddPlayerToGame(const std::string& name, const std::string& password = "");
		bool HasPlayerWithName(const std::string& name);

	private:
		void PopulateGame();

	private:
		const std::string kDbFile{ "players.sqlite" };

	private:
		DataBase m_db = CreateDataBase(kDbFile);
	};

	class AddPlayerToGameHandler {
	public:
		AddPlayerToGameHandler(PlayerProfiles& profiles);

		crow::response operator() (const crow::request& req);

	private:
		PlayerProfiles& m_db;
	};

}