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

	
	
	CROW_ROUTE(m_app, "/game/<int>/chat/send").methods(crow::HTTPMethod::POST)
		([this](const crow::request& req, int gameId) {

		auto body = crow::json::load(req.body);
		if (!body) return crow::response(400);

		std::string text = body["text"].s();
		std::string playerName = body["playerName"].s();

		std::lock_guard<std::mutex> lk(m_lobbyMutex);

		auto it = m_activeGames.find(gameId);
		if (it == m_activeGames.end())
			return crow::response(404);

		Message msg(text, playerName);
		it->second.GetChat().AddMessage(msg);

		return crow::response(200);
			});


	

	CROW_ROUTE(m_app, "/game/<int>/chat/messages").methods(crow::HTTPMethod::GET)
		([this](int gameId) {

		std::lock_guard<std::mutex> lk(m_lobbyMutex);

		auto it = m_activeGames.find(gameId);
		if (it == m_activeGames.end())
			return crow::response(404);

		crow::json::wvalue response;
		int idx = 0;

		for (const auto& m : it->second.GetChat().GetMessages()) {
			response[idx]["playerName"] = m.GetPlayerName();
			response[idx]["text"] = m.GetTextFromPlayer();
			idx++;
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
			try {
				const Player& currentPlayer = gameInst.GetCurrentPlayer();
				resp["currentPlayerIndex"] = gameInst.GetCurrentPlayerIndex();
				resp["currentPlayerId"] = currentPlayer.GetId();
				resp["currentPlayerName"] = currentPlayer.GetUsername();
			}
			catch (...) {
				resp["currentPlayerIndex"] = 0;
				resp["currentPlayerId"] = -1;
				resp["currentPlayerName"] = "Unknown";
			}
			auto status = gameInst.GetGameStatus();
			switch (status) {
			case Game::GameStatus::Won:
				resp["gameStatus"] = "won";
				resp["gameMessage"] = "You won!";
				break;
			case Game::GameStatus::Lost:
				resp["gameStatus"] = "lost";
				resp["gameMessage"] = "Game over - you lost";
				break;
			default:
				resp["gameStatus"] = "playing";
				resp["gameMessage"] = "Game in progress";
			}

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
				gameInst.AdvanceTurn();

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


		/////////Minigame routes//////////

		CROW_ROUTE(m_app, "/minigame/rewards").methods(crow::HTTPMethod::GET)
			([this]() {
			MinigameServer minigame;
			const auto& tiers = minigame.GetRewardTiers();

			crow::json::wvalue resp;
			int idx = 0;
			for (const auto& tier : tiers) {
				resp["rewards"][idx]["minScore"] = tier.minScore;
				resp["rewards"][idx]["powerUpType"] = PowerUp::TypeToString(tier.powerUpType);
				resp["rewards"][idx]["description"] = tier.rewardName;
				idx++;
			}

			return crow::response{ resp.dump() };
				});

		// Finalizează minigame-ul și atribuie recompense
		CROW_ROUTE(m_app, "/minigame/complete").methods(crow::HTTPMethod::POST)
			([this, &storage](const crow::request& req) {
			auto body = crow::json::load(req.body);
			if (!body) {
				return crow::response(400, R"({"error":"Invalid JSON"})");
			}

			if (!body.has("playerId") || !body.has("score")) {
				return crow::response(400, R"({"error":"Missing playerId or score"})");
			}

			int playerId = (int)body["playerId"].i();
			int score = (int)body["score"].i();

			// Verifică dacă jucătorul există
			auto userOpt = storage.GetUserById(playerId);
			if (!userOpt.has_value()) {
				return crow::response(404, R"({"error":"Player not found"})");
			}

			MinigameServer minigame;

			// Verifică dacă jucătorul merită o recompensă
			if (!minigame.HasReward(score)) {
				crow::json::wvalue resp;
				resp["success"] = true;
				resp["rewardEarned"] = false;
				resp["message"] = "Score too low for a reward";
				resp["score"] = score;
				return crow::response{ resp.dump() };
			}

			// Determină ce power-up primește
			PowerUpType rewardType = minigame.GetRewardForScore(score);

			auto powerUpOpt = storage.GetPowerUpByType(rewardType);
			if (!powerUpOpt) {
				return crow::response(500, R"({"error":"Failed to find power-up in database"})");
			}

			uint32_t powerUpId = powerUpOpt->GetDatabaseId();

			try {
				// Adaugă power-up-ul în inventarul jucătorului
				storage.AddPowerUpToUser(playerId, powerUpId);

				crow::json::wvalue resp;
				resp["success"] = true;
				resp["rewardEarned"] = true;
				resp["score"] = score;
				resp["powerUpType"] = PowerUp::TypeToString(rewardType);
				resp["powerUpId"] = powerUpId;
				resp["description"] = minigame.GetRewardDescription(score);
				resp["message"] = "Congratulations! You earned a power-up!";

				return crow::response{ resp.dump() };
			}
			catch (const std::exception& e) {
				crow::json::wvalue errorResp;
				errorResp["error"] = std::string("Database error: ") + e.what();
				return crow::response(500, errorResp.dump());
			}
				});

		// Obține inventarul de power-up-uri al unui jucător
		CROW_ROUTE(m_app, "/player/<int>/powerups").methods(crow::HTTPMethod::GET)
			([&storage](int playerId) {
			auto userOpt = storage.GetUserById(playerId);
			if (!userOpt.has_value()) {
				return crow::response(404, R"({"error":"Player not found"})");
			}

			try {
				auto inventory = storage.GetUserInventory(playerId);

				crow::json::wvalue resp;
				resp["playerId"] = playerId;
				resp["totalPowerUps"] = inventory.size();

				int idx = 0;
				for (const auto& powerUp : inventory) {
					resp["powerUps"][idx]["id"] = powerUp.GetDatabaseId();
					resp["powerUps"][idx]["type"] = powerUp.GetTypeString();
					resp["powerUps"][idx]["name"] = powerUp.GetName();
					resp["powerUps"][idx]["description"] = powerUp.GetDescription();
					idx++;
				}

				return crow::response{ resp.dump() };
			}
			catch (const std::exception& e) {
				crow::json::wvalue errorResp;
				errorResp["error"] = std::string("Database error: ") + e.what();
				return crow::response(500, errorResp.dump());
			}
				});

		// Obține power-up-urile echipate ale unui jucător
		CROW_ROUTE(m_app, "/player/<int>/powerups/equipped").methods(crow::HTTPMethod::GET)
			([&storage](int playerId) {
			auto userOpt = storage.GetUserById(playerId);
			if (!userOpt.has_value()) {
				return crow::response(404, R"({"error":"Player not found"})");
			}

			try {
				auto equippedPowerUps = storage.GetEquippedPowerUps(playerId);

				crow::json::wvalue resp;
				resp["playerId"] = playerId;
				resp["totalEquipped"] = equippedPowerUps.size();

				int idx = 0;
				for (const auto& powerUp : equippedPowerUps) {
					resp["powerUps"][idx]["id"] = powerUp.GetDatabaseId();
					resp["powerUps"][idx]["type"] = powerUp.GetTypeString();
					resp["powerUps"][idx]["name"] = powerUp.GetName();
					resp["powerUps"][idx]["description"] = powerUp.GetDescription();
					idx++;
				}

				return crow::response{ resp.dump() };
			}
			catch (const std::exception& e) {
				crow::json::wvalue errorResp;
				errorResp["error"] = std::string("Database error: ") + e.what();
				return crow::response(500, errorResp.dump());
			}
				});

		// Echipează un power-up
		CROW_ROUTE(m_app, "/player/<int>/powerups/<int>/equip").methods(crow::HTTPMethod::POST)
			([&storage](int playerId, int powerUpId) {
			auto userOpt = storage.GetUserById(playerId);
			if (!userOpt.has_value()) {
				return crow::response(404, R"({"error":"Player not found"})");
			}

			// Verifică dacă jucătorul are acest power-up
			if (!storage.UserHasPowerUp(playerId, powerUpId)) {
				return crow::response(400, R"({"error":"Player does not own this power-up"})");
			}

			try {
				storage.EquipPowerUp(playerId, powerUpId);

				crow::json::wvalue resp;
				resp["success"] = true;
				resp["message"] = "Power-up equipped successfully";
				resp["playerId"] = playerId;
				resp["powerUpId"] = powerUpId;

				return crow::response{ resp.dump() };
			}
			catch (const std::exception& e) {
				crow::json::wvalue errorResp;
				errorResp["error"] = std::string("Database error: ") + e.what();
				return crow::response(500, errorResp.dump());
			}
				});

		// Dezechipează un power-up
		CROW_ROUTE(m_app, "/player/<int>/powerups/<int>/unequip").methods(crow::HTTPMethod::POST)
			([&storage](int playerId, int powerUpId) {
			auto userOpt = storage.GetUserById(playerId);
			if (!userOpt.has_value()) {
				return crow::response(404, R"({"error":"Player not found"})");
			}

			try {
				storage.UnequipPowerUp(playerId, powerUpId);

				crow::json::wvalue resp;
				resp["success"] = true;
				resp["message"] = "Power-up unequipped successfully";
				resp["playerId"] = playerId;
				resp["powerUpId"] = powerUpId;

				return crow::response{ resp.dump() };
			}
			catch (const std::exception& e) {
				crow::json::wvalue errorResp;
				errorResp["error"] = std::string("Database error: ") + e.what();
				return crow::response(500, errorResp.dump());
			}
				});

		// Folosește un power-up în timpul jocului
		CROW_ROUTE(m_app, "/game/<int>/player/<int>/usepower").methods(crow::HTTPMethod::POST)
			([this, &storage](const crow::request& req, int gameId, int playerId) {
			auto body = crow::json::load(req.body);
			if (!body) {
				return crow::response(400, R"({"error":"Invalid JSON"})");
			}

			if (!body.has("powerUpId")) {
				return crow::response(400, R"({"error":"Missing powerUpId"})");
			}

			uint32_t powerUpId = (uint32_t)body["powerUpId"].i();

			std::lock_guard<std::mutex> lk(m_lobbyMutex);

			auto it = m_activeGames.find(gameId);
			if (it == m_activeGames.end()) {
				return crow::response(404, R"({"error":"Game not found"})");
			}

			// Verifică dacă jucătorul are power-up-ul și este echipat
			if (!storage.IsPowerUpEquipped(playerId, powerUpId)) {
				return crow::response(400, R"({"error":"Power-up not equipped or not owned"})");
			}

			// Obține power-up-ul din baza de date
			auto powerUpOpt = storage.GetPowerUpById(powerUpId);
			if (!powerUpOpt) {
				return crow::response(404, R"({"error":"Power-up not found"})");
			}

			try {
				Game& gameInst = it->second;
				Player& player = gameInst.GetPlayer(playerId);
				PowerUp powerUp = powerUpOpt.value();

				// Extrage parametrii opționali
				int targetPlayerId = body.has("targetPlayerId") ? (int)body["targetPlayerId"].i() : -1;
				int targetCardValue = body.has("targetCardValue") ? (int)body["targetCardValue"].i() : -1;
				int targetPileId = body.has("targetPileId") ? (int)body["targetPileId"].i() : -1;

				// Folosește power-up-ul
				bool success = powerUp.Use(player, gameInst, targetPlayerId, targetCardValue, targetPileId);

				if (success) {
					// Șterge power-up-ul din inventar după utilizare
					storage.RemovePowerUpFromUser(playerId, powerUpId);

					crow::json::wvalue resp;
					resp["success"] = true;
					resp["message"] = "Power-up used successfully";
					resp["powerUpType"] = powerUp.GetTypeString();
					resp["powerUpName"] = powerUp.GetName();

					return crow::response{ resp.dump() };
				}
				else {
					crow::json::wvalue resp;
					resp["success"] = false;
					resp["error"] = "Failed to use power-up - invalid conditions or parameters";
					return crow::response(400, resp.dump());
				}
			}
			catch (const std::exception& e) {
				crow::json::wvalue errorResp;
				errorResp["error"] = std::string("Error: ") + e.what();
				return crow::response(500, errorResp.dump());
			}
				});



	m_app.port(18080).multithreaded().run();
}

crow::response http::Routing::AddUsersToGameRute(UserProfiles& storage, const crow::request& req, int playerId) const
{
	return crow::response();
}

