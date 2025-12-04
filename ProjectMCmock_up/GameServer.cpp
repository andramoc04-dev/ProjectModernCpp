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
		if (!body) {
			return crow::response(400, R"({"error":"Invalid JSON"})");
		}

		std::string name = body["name"].s();
		std::string password = body["password"].s();

		if (name.empty() || password.empty()) {
			return crow::response(400, R"({"error":"Missing name or password"})");
		}

		if (storage.HasPlayerWithName(name)) {
			return crow::response(409, R"({"error":"Player already exists"})");
		}

		storage.AddPlayerToGame(name, password);

		return crow::response(201, R"({"status":"Player created"})");
			});


	CROW_ROUTE(m_app, "/login").methods(crow::HTTPMethod::POST)
		([&storage](const crow::request& req) {
		auto body = crow::json::load(req.body);
		if (!body) {
			return crow::response(400, R"({"error":"Invalid JSON"})");
		}

		std::string name = body["name"].s();
		std::string password = body["password"].s();

		if (name.empty() || password.empty()) {
			return crow::response(400, R"({"error":"Missing name or password"})");
		}

		// verificăm dacă există jucătorul
		auto players = storage.GetPlayers();
		for (const auto& p : players) {
			if (p.GetName() == name && p.GetPassword() == password) {
				crow::json::wvalue success;
				success["id"] = p.GetId();
				success["name"] = p.GetName();
				success["score"] = p.GetScore();
				success["wins"] = p.GetWins();
				success["hoursPlayed"] = p.GetHoursPlayed();
				success["unusedCards"] = p.GetUnusedCards();
				return crow::response(200, success.dump());
			}
		}

		return crow::response(401, R"({"error":"Invalid credentials"})");
			});



	m_app.port(18080).multithreaded().run();
}

crow::response http::Routing::AddPlayersToGameRute(PlayerProfiles& storage, const crow::request& req, int playerId) const
{
	return crow::response();
}

