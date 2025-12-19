#include "GameServer.h"
#include <mutex>
#include <vector>

struct LobbyInfo {
	int gameId = -1;
	bool started = false;
	int matchedPlayerId = -1;
};

static std::mutex lobbyMutex;
static std::vector<int> waitingPlayers;
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
		([this, &storage](const crow::request& req) {
		auto body = crow::json::load(req.body);
		if (!body) return crow::response(400, R"({"error":"Invalid JSON"})");
		if (!body.has("id")) return crow::response(400, R"({"error":"Missing id"})");

		int playerId = (int)body["id"].i();
		auto userOpt = storage.GetUserById(playerId);
		if (!userOpt.has_value()) {
			return crow::response(404, R"({"error":"Player not found"})");
		}

		const auto& user = userOpt.value();
		std::string playerName = user.GetUsername();
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
				auto userB = storage.GetUserById(b).value();

				LobbyInfo infoA, infoB;
				infoA.started = true;
				infoB.started = true;
				infoA.matchedPlayerId = b;
				infoB.matchedPlayerId = a;
				int gid = nextGameId++;
				infoA.gameId = gid;
				infoB.gameId = gid;


				Game& gameInst = m_activeGames[gid];
				gameInst.AddPlayer(Player(a, user.GetUsername()));
				gameInst.AddPlayer(Player(b, userB.GetUsername()));

				playerLobbyMap[a] = infoA;
				playerLobbyMap[b] = infoB;

				Player player1 = Player().ConvertUserToPlayer(storage.GetUserById(a).value());
				Player player2 = Player().ConvertUserToPlayer(storage.GetUserById(b).value());

				gameInst = Game({ player1, player2 }, std::stack<Card>{}, Chat());


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
		resp["opponentId"] = info.matchedPlayerId;
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
		([this, &storage](int gameId) {
		std::lock_guard<std::mutex> lk(lobbyMutex);

		auto it = m_activeGames.find(gameId);
		if (it == m_activeGames.end()) {
			return crow::response(404, R"({"error":"Game not found"})");
		}

		Game& gameInst = it->second;


		if (!gameInst.GetAreCardsShuffled()) {
			gameInst.ShuffleCards();
		}
		crow::json::wvalue resp;
		resp["gameId"] = gameId;
		resp["status"] = "shuffled";
		resp["cardsInDeck"] = gameInst.GetCardsDeck().size();

		return crow::response{ resp.dump() };
			});


	    CROW_ROUTE(m_app, "/game/<int>/distribution").methods(crow::HTTPMethod::POST)
			([this, &storage](int gameId) {
			std::lock_guard<std::mutex> lk(lobbyMutex);

			auto it = m_activeGames.find(gameId);
			if (it == m_activeGames.end()) {
				return crow::response(404, R"({"error":"Game not found"})");
			}

			Game& gameInst = it->second;

			if (!gameInst.GetAreCardsShuffled()) {
				return crow::response(400, R"({"error":"Deck not shuffled yet"})");
			}

			if (gameInst.GetAreCardsDealt()==true) {
				return crow::response(400, R"({"error":"Cards already dealt"})");
			}

			gameInst.CardsDistribution(); 

			crow::json::wvalue resp;
			resp["gameId"] = gameId;
			resp["status"] = "cards dealt";

			return crow::response{ resp.dump() };
			});
			
		CROW_ROUTE(m_app, "/game/<int>/player/<int>/hand").methods(crow::HTTPMethod::GET)
			([this, &storage](int gameId, int playerId) {
			std::lock_guard<std::mutex> lk(lobbyMutex);
    
			auto it = m_activeGames.find(gameId);
			if (it == m_activeGames.end()) {
				return crow::response(404, R"({"error":"Game not found"})");
			}
    
			Game& gameInst = it->second;
			
			crow::json::wvalue resp;
			resp["playerId"] = playerId;
			resp["gameId"] = gameId;
			
			const auto& hand = gameInst.GetPlayer(playerId).GetHand(); 
			
			int index = 0;
			for (const auto& card : hand)
			{
				crow::json::wvalue cardJson;
				cardJson["value"] = card.GetNumber();
				resp["hand"][index++] = std::move(cardJson);
			}
    
			return crow::response{ resp.dump() };
		});

		CROW_ROUTE(m_app, "/game/<int>/player/<int>/place").methods(crow::HTTPMethod::POST)
			([this, &storage](const crow::request& req, int gameId, int playerId) {
			auto body = crow::json::load(req.body);
			if (!body) return crow::response(400, R"({"error":"Invalid JSON"})");
			if (!body.has("cardValue") || !body.has("pileId"))
				return crow::response(400, R"({"error":"Missing cardValue or pileId"})");

			int cardValue = (int)body["cardValue"].i();
			int pileId = (int)body["pileId"].i();

			std::lock_guard<std::mutex> lk(lobbyMutex);
			auto it = m_activeGames.find(gameId);
			if (it == m_activeGames.end()) {
				return crow::response(404, R"({"error":"Game not found"})");
			}

			Game& gameInst = it->second;

			try {
				Player& player = gameInst.GetPlayer(playerId);
				Card chosen(cardValue);

				bool placed = player.PlaceCard(
					chosen,
					gameInst.GetDescendingDeck1(),
					gameInst.GetDescendingDeck2(),
					gameInst.GetAscendingDeck1(),
					gameInst.GetAscendingDeck2(),
					pileId
				);

				crow::json::wvalue resp;
				resp["success"] = placed;
				resp["cardValue"] = cardValue;
				resp["pileId"] = pileId;


				switch (pileId) {
				case 0:
					resp["newPileValue"] = gameInst.GetDescendingDeck1().GetNumber();
					std::cout << "Pile 0 updated to " << gameInst.GetDescendingDeck1().GetNumber() << std::endl;
					break;
				case 1:
					resp["newPileValue"] = gameInst.GetDescendingDeck2().GetNumber();
					std::cout << "Pile 1 updated to " << gameInst.GetDescendingDeck2().GetNumber() << std::endl;
					break;
				case 2:
					resp["newPileValue"] = gameInst.GetAscendingDeck1().GetNumber();
					std::cout << "Pile 2 updated to " << gameInst.GetAscendingDeck1().GetNumber() << std::endl;
					break;
				case 3:
					resp["newPileValue"] = gameInst.GetAscendingDeck2().GetNumber();
					std::cout << "Pile 3 updated to " << gameInst.GetAscendingDeck2().GetNumber() << std::endl;
					break;
				}


				return crow::response{ resp.dump() };
			}
			catch (const std::exception& e) {
				return crow::response(400, R"({"error":")" + std::string(e.what()) + "\"}");
			}
				});


		CROW_ROUTE(m_app, "/game/<int>/state").methods(crow::HTTPMethod::GET)
			([this, &storage](int gameId) {
			std::lock_guard<std::mutex> lk(lobbyMutex);
			auto it = m_activeGames.find(gameId);
			if (it == m_activeGames.end()) {
				return crow::response(404, R"({"error":"Game not found"})");
			}

			Game& gameInst = it->second;

			crow::json::wvalue resp;
			resp["gameId"] = gameId;

			// piles
			resp["piles"] = std::vector<crow::json::wvalue>{
				gameInst.GetDescendingDeck1().GetNumber(),
				gameInst.GetDescendingDeck2().GetNumber(),
				gameInst.GetAscendingDeck1().GetNumber(),
				gameInst.GetAscendingDeck2().GetNumber()
			};

			// players
			std::vector<crow::json::wvalue> playersArray;
			for (const auto& player : gameInst.GetPlayers()) {
				crow::json::wvalue playerJson;
				playerJson["id"] = player.GetId();                // moștenit din User
				playerJson["username"] = player.GetUsername();    // moștenit din User

				// hand
				std::vector<crow::json::wvalue> handArray;
				for (const auto& card : player.GetHand()) {
					handArray.push_back(card.GetNumber());
				}
				playerJson["hand"] = std::move(handArray);

				playersArray.push_back(std::move(playerJson));
			}
			resp["players"] = std::move(playersArray);

			return crow::response{ resp.dump() };
				});

	m_app.port(18080).multithreaded().run();
}

crow::response http::Routing::AddUsersToGameRute(UserProfiles& storage, const crow::request& req, int playerId) const
{
	return crow::response();
}

