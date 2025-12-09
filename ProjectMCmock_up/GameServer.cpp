#include "GameServer.h"
#include <mutex>
#include <vector>
#include <unordered_map>


struct LobbyInfo {
	int gameId = -1;
	bool started = false;
	int opponentId = -1;
};

static std::mutex lobbyMutex;
static std::vector<int> waitingPlayers; // coadă de jucători în lobby
static std::unordered_map<int, LobbyInfo> playerLobbyMap;
static int nextGameId = 1;


void http::Routing::Run(UserProfiles& storage)
{
	CROW_ROUTE(m_app, "/player/<int>")([&storage](int id) {

		auto player = storage.GetUserById(id);

		if (!player.has_value()) {
			return crow::response(404, "Player not found");
		}


		const auto& p = player.value();
		
		crow::json::wvalue json{
				{"id", p.GetId()},  
				{"name", p.GetUsername()},
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

		if (storage.HasUserWithName(name)) {
			return crow::response(409, R"({"error":"Player already exists"})");
		}

		storage.AddUserToGame(name, password);

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
		auto players = storage.GetUsers();
		for (const auto& p : players) {
			if (p.GetUsername() == name && p.GetPassword() == password) {
				crow::json::wvalue success;
				success["id"] = p.GetId();
				success["name"] = p.GetUsername();
				success["score"] = p.GetScore();
				success["wins"] = p.GetWins();
				success["hoursPlayed"] = p.GetHoursPlayed();
				success["unusedCards"] = p.GetUnusedCards();
				return crow::response(200, success.dump());
			}
		}

		return crow::response(401, R"({"error":"Invalid credentials"})");
			});




	CROW_ROUTE(m_app, "/join").methods(crow::HTTPMethod::POST)
		([&storage](const crow::request& req) {
		auto body = crow::json::load(req.body);
		if (!body) return crow::response(400, R"({"error":"Invalid JSON"})");
		if (!body.has("id")) return crow::response(400, R"({"error":"Missing id"})");

		int playerId = (int)body["id"].i();

		{
			std::lock_guard<std::mutex> lk(lobbyMutex);

			// dacă nu e deja în map, îl adăugăm în coadă
			if (playerLobbyMap.find(playerId) == playerLobbyMap.end()) {
				waitingPlayers.push_back(playerId);
				playerLobbyMap[playerId] = LobbyInfo{};
			}

			// dacă sunt cel puțin 2, formăm o pereche simplă
			if (waitingPlayers.size() >= 2) {
				int a = waitingPlayers[0];
				int b = waitingPlayers[1];

				LobbyInfo infoA, infoB;
				infoA.started = true;
				infoB.started = true;
				infoA.opponentId = b;
				infoB.opponentId = a;
				int gid = nextGameId++;
				infoA.gameId = gid;
				infoB.gameId = gid;

				playerLobbyMap[a] = infoA;
				playerLobbyMap[b] = infoB;

				// scoatem primii doi din coadă
				waitingPlayers.erase(waitingPlayers.begin());
				waitingPlayers.erase(waitingPlayers.begin());
			}
		}

		crow::json::wvalue resp;
		resp["status"] = "ok";
		return crow::response{ resp.dump() };
			});

	CROW_ROUTE(m_app, "/status/<int>").methods(crow::HTTPMethod::GET)
		([&storage](const crow::request& /*req*/, int playerId) {
		std::lock_guard<std::mutex> lk(lobbyMutex);
		crow::json::wvalue resp;
		auto it = playerLobbyMap.find(playerId);
		if (it == playerLobbyMap.end()) {
			resp["started"] = false;
			resp["message"] = "Not in lobby";
			return crow::response{ resp.dump() };
		}
		const LobbyInfo& info = it->second;
		resp["started"] = info.started;
		resp["gameId"] = info.gameId;
		resp["opponentId"] = info.opponentId;
		return crow::response{ resp.dump() };
			});






	m_app.port(18080).multithreaded().run();
}

crow::response http::Routing::AddUsersToGameRute(UserProfiles& storage, const crow::request& req, int playerId) const
{
	return crow::response();
}

