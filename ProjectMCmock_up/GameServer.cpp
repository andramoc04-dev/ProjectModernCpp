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

}

crow::response http::Routing::AddPlayersToGame(PlayerProfiles& profiles, const crow::request& req, int playerId) const
{
	return crow::response();
}
