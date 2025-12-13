#include "GameServer.h"
#include <mutex>
#include <vector>
#include <unordered_map>
#include "Game.h"


struct GameInstance {
	std::stack<Card> cardsDeck;
	std::vector<int> playerIds;  
	bool shuffled = false;
};

static std::unordered_map<int, GameInstance> activeGames;

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

				GameInstance& gameInst = activeGames[gid];
				gameInst.playerIds.push_back(a);
				gameInst.playerIds.push_back(b);

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




	
	using namespace std::chrono;

	
	CROW_ROUTE(m_app, "/chat/send").methods(crow::HTTPMethod::POST)
		([&](const crow::request& req) {
		auto body = crow::json::load(req.body);
		if (!body) return crow::response(400, R"({"error":"Invalid JSON"})");

		std::string text = body["text"].s();
		std::string playerName = body["playerName"].s(); // sau află din id: body["playerId"]

		// creează Message (folosesc timestamp acum)
		auto now = system_clock::now();
		Message msg{ text, playerName, now };

		{
			std::lock_guard<std::mutex> lk(m_chatMutex);
			m_chat.AddMessage(msg);
		}

		crow::json::wvalue resp;
		resp["status"] = "ok";
		return crow::response{ resp.dump() };
			});

	


	CROW_ROUTE(m_app, "/chat/messages").methods(crow::HTTPMethod::GET)
		([&](const crow::request&) {

		crow::json::wvalue response;

		{
			std::lock_guard<std::mutex> lk(m_chatMutex);
			const auto& msgs = m_chat.GetMessages();

			int index = 0;
			for (const auto& m : msgs)
			{
				crow::json::wvalue item;

				item["playerName"] = m.GetPlayerName();
				item["text"] = m.GetTextFromPlayer();

				auto ts = std::chrono::duration_cast<std::chrono::milliseconds>(
					m.GetTimeStamp().time_since_epoch()
				).count();

				item["ts"] = ts;

				response[index] = std::move(item); 
				index++;
			}
		}

		return crow::response(response.dump());
			});


	CROW_ROUTE(m_app, "/game/<int>/shuffle").methods(crow::HTTPMethod::POST)
		([&storage](int gameId) {
		std::lock_guard<std::mutex> lk(lobbyMutex);

		auto it = activeGames.find(gameId);
		if (it == activeGames.end()) {
			return crow::response(404, R"({"error":"Game not found"})");
		}

		GameInstance& gameInst = it->second;

		
		if (!gameInst.shuffled) {
			std::srand(std::time(0));
			std::deque<int> sortedCards;
			for (int i = 2; i <= 99; i++)
				sortedCards.push_back(i);

			int position;
			while (!sortedCards.empty()) {
				position = std::rand() % sortedCards.size();
				Card card(sortedCards[position]);
				gameInst.cardsDeck.push(card);
				sortedCards.erase(sortedCards.begin() + position);
			}

			gameInst.shuffled = true;
		}

		crow::json::wvalue resp;
		resp["gameId"] = gameId;
		resp["status"] = "shuffled";
		resp["cardsInDeck"] = gameInst.cardsDeck.size();

		return crow::response{ resp.dump() };
			});



	m_app.port(18080).multithreaded().run();
}

crow::response http::Routing::AddUsersToGameRute(UserProfiles& storage, const crow::request& req, int playerId) const
{
	return crow::response();
}

