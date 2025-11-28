#include "GameServer.h"

void http::Routing::Run(PlayerProfiles& storage)
{
	CROW_ROUTE(m_app, "/player/<int>")([&storage](int id) {

		auto player = storage.GetPlayerById(id);

		if (!player.has_value()) {
			return crow::response(404, "Player not found");
		}


		const auto& p = player.value();
		
		crow::json::wvalue json{
				{"id", p.GetId()},  
				{"name", p.GetName()},
				{"score", p.GetScore()},
				{"wins", p.GetWins()},
				{"hoursPlayed", p.GetHoursPlayed()},
				{"unusedCards", p.GetUnusedCards()}
				};
		
		return crow::response{ json.dump() };

		});


	CROW_ROUTE(m_app, "/addplayer").methods(crow::HTTPMethod::POST)
		([&storage](const crow::request& req) {
		auto body = crow::json::load(req.body);
		if (!body) return crow::response(400);

		std::string name = body["name"].s();
		std::string password = body["password"].s();

		storage.AddPlayerToGame(name, password);
		return crow::response(201);
			});

	m_app.port(18080).multithreaded().run();
}

crow::response http::Routing::AddPlayersToGameRute(PlayerProfiles& storage, const crow::request& req, int playerId) const
{
	return crow::response();
}

