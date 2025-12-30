#include "GameServer.h"
#include <mutex>
#include <vector>
#include <algorithm>


void http::Routing::StartGameForGroupLocked(UserProfiles& storage, const std::vector<int>& playerIds)
{
	if ((int)playerIds.size() < kMinPlayersPerGame)
		return;

	const int gid = m_nextGameId++;

	std::vector<Player> players;
	players.reserve(playerIds.size());
	for (int pid : playerIds) {
		auto u = storage.GetUserById(pid).value();
		players.push_back(Player::ConvertUserToPlayer(u));
	}

	m_activeGames[gid] = Game(players, std::stack<Card>{}, Chat());

	for (int pid : playerIds) {
		LobbyInfo info;
		info.m_started = true;
		info.m_gameId = gid;
		info.m_matchedPlayerId = -1;
		for (int other : playerIds) {
			if (other != pid) { info.m_matchedPlayerId = other; break; }
		}
		m_playerLobbyMap[pid] = info;
	}
}

bool http::Routing::TryStartMatchLocked(UserProfiles& storage)
{
	if ((int)m_waitingPlayers.size() < kMinPlayersPerGame)
		return false;

	if ((int)m_waitingPlayers.size() >= kMaxPlayersPerGame) {
		std::vector<int> playerIds;
		playerIds.reserve(kMaxPlayersPerGame);
		for (int i =0; i < kMaxPlayersPerGame; i++) {
			playerIds.push_back(m_waitingPlayers[i].playerId);
		}

		StartGameForGroupLocked(storage, playerIds);
		m_waitingPlayers.erase(m_waitingPlayers.begin(), m_waitingPlayers.begin() + kMaxPlayersPerGame);
		return true;
	}

	int oldestIdx =0;
	int oldestElapsed = -1;
	for (int i =0; i < (int)m_waitingPlayers.size(); i++) {
		int e = m_waitingPlayers[i].timer.elapsedSeconds();
		if (e > oldestElapsed) {
			oldestElapsed = e;
			oldestIdx = i;
		}
	}

	const bool oldestExpired = m_waitingPlayers[oldestIdx].timerStarted && m_waitingPlayers[oldestIdx].timer.expired();
	if (!oldestExpired)
		return false;

	std::vector<int> playerIds;
	playerIds.reserve(m_waitingPlayers.size());
	for (const auto& wp : m_waitingPlayers) {
		playerIds.push_back(wp.playerId);
	}

	StartGameForGroupLocked(storage, playerIds);
	m_waitingPlayers.clear();
	return true;
}

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
		{
			std::lock_guard<std::mutex> lk(m_lobbyMutex);
			if (m_playerLobbyMap.find(playerId) == m_playerLobbyMap.end()) {
				WaitingPlayer wp;
				wp.playerId = playerId;
				wp.score = (int)user.GetScore();
				wp.timer.start(kLobbyTimeoutSeconds);
				wp.timerStarted = true;

				m_waitingPlayers.push_back(std::move(wp));
				m_playerLobbyMap[playerId] = LobbyInfo{};
			}

			while (TryStartMatchLocked(storage)) {
				// keep matching
			}
		}

		crow::json::wvalue resp;
		resp["status"] = "ok";
		return crow::response{ resp.dump() };
			});

	CROW_ROUTE(m_app, "/status/<int>").methods(crow::HTTPMethod::GET)
		([this, &storage](const crow::request& /*req*/, int playerId) {
		std::lock_guard<std::mutex> lk(m_lobbyMutex);
		while (TryStartMatchLocked(storage)) {
		}

		crow::json::wvalue resp;
		auto it = m_playerLobbyMap.find(playerId);
		if (it == m_playerLobbyMap.end()) {
			resp["started"] = false;
			resp["message"] = "Not in lobby";
			return crow::response{ resp.dump() };
		}
		const LobbyInfo& info = it->second;
		resp["started"] = info.m_started;
		resp["gameId"] = info.m_gameId;
		resp["opponentId"] = info.m_matchedPlayerId;
		return crow::response{ resp.dump() };
			});
	


	using namespace std::chrono;

	
	CROW_ROUTE(m_app, "/chat/send").methods(crow::HTTPMethod::POST)
		([&](const crow::request& req) {
		auto body = crow::json::load(req.body);
		if (!body) return crow::response(400, R"({"error":"Invalid JSON"})");

		std::string text = body["text"].s();
		std::string playerName = body["playerName"].s(); 
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
		std::lock_guard<std::mutex> lk(m_lobbyMutex);

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
		std::lock_guard<std::mutex> lk(m_lobbyMutex);

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
			std::lock_guard<std::mutex> lk(m_lobbyMutex);
    
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

			std::lock_guard<std::mutex> lk(m_lobbyMutex);
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

				if (placed) {
					player.IncrementCardsPlacedThisTurn();
				}

				crow::json::wvalue resp;
				resp["success"] = placed;
				resp["cardValue"] = cardValue;
				resp["pileId"] = pileId;
				resp["cardsPlacedThisTurn"] = player.GetCardsPlacedThisTurn();


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
			std::lock_guard<std::mutex> lk(m_lobbyMutex);
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
				playerJson["id"] = player.GetId();
				playerJson["username"] = player.GetUsername();
				playerJson["cardsPlacedThisTurn"] = player.GetCardsPlacedThisTurn(); // ADAUGĂ

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

		CROW_ROUTE(m_app, "/game/<int>/player/<int>/draw").methods(crow::HTTPMethod::POST)
			([this, &storage](int gameId, int playerId) {
			std::lock_guard<std::mutex> lk(m_lobbyMutex);

			auto it = m_activeGames.find(gameId);
			if (it == m_activeGames.end()) {
				return crow::response(404, R"({"error":"Game not found"})");
			}

			Game& gameInst = it->second;

			// Verifică dacă mai sunt cărți în teanc
			if (gameInst.GetCardsDeck().empty()) {
				return crow::response(400, R"({"error":"No cards left in deck"})");
			}

			try {
				Player& player = gameInst.GetPlayer(playerId);

				// Extrage cartea din vârful stivei
				Card drawnCard = gameInst.GetCardsDeck().top();
				gameInst.GetCardsDeck().pop();

				// Adaugă cartea în mâna jucătorului
				player.GetHand().insert(drawnCard);

				crow::json::wvalue resp;
				resp["success"] = true;
				resp["cardValue"] = drawnCard.GetNumber();
				resp["cardsLeftInDeck"] = gameInst.GetCardsDeck().size();

				return crow::response{ resp.dump() };
			}
			catch (const std::exception& e) {
				return crow::response(400, R"({"error":")" + std::string(e.what()) + "\"}");
			}
				});

		CROW_ROUTE(m_app, "/game/<int>/player/<int>/endturn").methods(crow::HTTPMethod::POST)
			([this, &storage](int gameId, int playerId) {
			std::lock_guard<std::mutex> lk(m_lobbyMutex);

			auto it = m_activeGames.find(gameId);
			if (it == m_activeGames.end()) {
				return crow::response(404, R"({"error":"Game not found"})");
			}

			Game& gameInst = it->second;

			try {
				Player& player = gameInst.GetPlayer(playerId);

				int cardsPlaced = player.GetCardsPlacedThisTurn();
				int cardsInDeck = gameInst.GetCardsDeck().size();

				// Determină numărul minim de cărți necesare
				int minCardsRequired = (cardsInDeck > 0) ? 2 : 1;

				// Verifică dacă jucătorul a plasat numărul minim de cărți
				if (cardsPlaced < minCardsRequired) {
					crow::json::wvalue errorResp;
					errorResp["success"] = false;
					errorResp["error"] = "You must place at least " + std::to_string(minCardsRequired) +
						" card(s) before ending your turn";
					errorResp["cardsPlaced"] = cardsPlaced;
					errorResp["minRequired"] = minCardsRequired;
					return crow::response(400, errorResp.dump());
				}

				// Trage cărțile din teanc (număr egal cu cărțile plasate, dar max cât există)
				int cardsToDraw = std::min(cardsPlaced, cardsInDeck);

				std::vector<int> drawnCards;
				for (int i = 0; i < cardsToDraw; i++) {
					if (!gameInst.GetCardsDeck().empty()) {
						Card drawnCard = gameInst.GetCardsDeck().top();
						gameInst.GetCardsDeck().pop();
						player.GetHand().insert(drawnCard);
						drawnCards.push_back(drawnCard.GetNumber());
					}
				}

				// Reset contorul pentru următoarea tură
				player.ResetCardsPlacedThisTurn();

				crow::json::wvalue resp;
				resp["success"] = true;
				resp["cardsPlaced"] = cardsPlaced;
				resp["cardsToDraw"] = cardsToDraw;
				resp["cardsDrawn"] = drawnCards.size();
				resp["cardsLeftInDeck"] = gameInst.GetCardsDeck().size();

				// Adaugă lista de cărți trase
				int idx = 0;
				for (int cardVal : drawnCards) {
					resp["drawnCardValues"][idx++] = cardVal;
				}

				// Mesaj pentru utilizator
				if (cardsPlaced > 0 && cardsToDraw < cardsPlaced) {
					resp["message"] = "You placed " + std::to_string(cardsPlaced) +
						" cards and received " + std::to_string(cardsToDraw) +
						" cards because that's all that remained in the deck";
				}
				else if (cardsToDraw > 0) {
					resp["message"] = "You placed " + std::to_string(cardsPlaced) +
						" cards and drew " + std::to_string(cardsToDraw) + " new cards";
				}

				return crow::response{ resp.dump() };
			}
			catch (const std::exception& e) {
				return crow::response(400, R"({"error":")" + std::string(e.what()) + "\"}");
			}
				});

	m_app.port(18080).multithreaded().run();
}

crow::response http::Routing::AddUsersToGameRute(UserProfiles& storage, const crow::request& req, int playerId) const
{
	return crow::response();
}

