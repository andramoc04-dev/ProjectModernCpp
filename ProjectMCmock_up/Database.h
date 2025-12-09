#pragma once
#include <string>
#include <vector>

#include <crow.h>
#include <sqlite_orm/sqlite_orm.h>
namespace sql = sqlite_orm;

#include "Utils.h"
#include "User.h"
#include "Game.h"

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
				sql::make_column("unusedCards", &User::SetUnusedCards, &User::GetUnusedCards)

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