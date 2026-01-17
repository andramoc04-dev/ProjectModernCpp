#include "GameRoutes.h"
#include "PowerUp.h"
#include "Database.h"
#include <algorithm>
#include "Game.h"

module GameManager;


void GameRoutes::Register(crow::SimpleApp& app, GameManager& gm, http::UserProfiles& storage)
{
    // ===================== CHAT SEND =====================
    CROW_ROUTE(app, "/game/<int>/chat/send").methods(crow::HTTPMethod::POST)
        ([&gm](const crow::request& req, int gameId) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400);

        std::string text = body["text"].s();
        std::string playerName = body["playerName"].s();

        std::lock_guard<std::mutex> lk(gm.Mutex());
        auto* game = gm.Get(gameId);
        if (!game) return crow::response(404);

        Message msg(text, playerName);
        game->GetChat()->AddMessage(msg);

        return crow::response(200);
            });

    // ===================== CHAT MESSAGES =====================
    CROW_ROUTE(app, "/game/<int>/chat/messages").methods(crow::HTTPMethod::GET)
        ([&gm](int gameId) {
        std::lock_guard<std::mutex> lk(gm.Mutex());
        auto* game = gm.Get(gameId);
        if (!game) return crow::response(404);

        crow::json::wvalue response;
        int idx = 0;
        for (const auto& m : game->GetChat()->GetMessages()) {
            response[idx]["playerName"] = m.GetPlayerName();
            response[idx]["text"] = m.GetTextFromPlayer();
            idx++;
        }

        return crow::response(response.dump());
            });

    // ===================== SHUFFLE =====================
    CROW_ROUTE(app, "/game/<int>/shuffle").methods(crow::HTTPMethod::POST)
        ([&gm](int gameId) {
        std::lock_guard<std::mutex> lk(gm.Mutex());

        auto* game = gm.Get(gameId);
        if (!game) {
            return crow::response(404, R"({"error":"Game not found"})");
        }

        if (!game->GetAreCardsShuffled()) {
            game->ShuffleCards();
        }

        crow::json::wvalue resp;
        resp["gameId"] = gameId;
        resp["status"] = "shuffled";
        resp["cardsInDeck"] = game->GetCardsDeck().size();

        return crow::response{ resp.dump() };
            });

    // ===================== DISTRIBUTION =====================
    CROW_ROUTE(app, "/game/<int>/distribution").methods(crow::HTTPMethod::POST)
        ([&gm](int gameId) {
        std::lock_guard<std::mutex> lk(gm.Mutex());

        auto* game = gm.Get(gameId);
        if (!game) {
            return crow::response(404, R"({"error":"Game not found"})");
        }

        if (!game->GetAreCardsShuffled()) {
            return crow::response(400, R"({"error":"Deck not shuffled yet"})");
        }

        if (game->GetAreCardsDealt() == true) {
            return crow::response(400, R"({"error":"Cards already dealt"})");
        }

        game->CardsDistribution();

        crow::json::wvalue resp;
        resp["gameId"] = gameId;
        resp["status"] = "cards dealt";

        return crow::response{ resp.dump() };
            });

    // ===================== PLAYER HAND =====================
    CROW_ROUTE(app, "/game/<int>/player/<int>/hand").methods(crow::HTTPMethod::GET)
        ([&gm](int gameId, int playerId) {
        std::lock_guard<std::mutex> lk(gm.Mutex());

        auto* game = gm.Get(gameId);
        if (!game) {
            return crow::response(404, R"({"error":"Game not found"})");
        }

        crow::json::wvalue resp;
        resp["playerId"] = playerId;
        resp["gameId"] = gameId;

        const auto& hand = game->GetPlayer(playerId).GetHand();

        int index = 0;
        for (const auto& card : hand) {
            crow::json::wvalue cardJson;
            cardJson["value"] = card.GetNumber();
            resp["hand"][index++] = std::move(cardJson);
        }

        return crow::response{ resp.dump() };
            });

    // ===================== PLACE CARD =====================
    CROW_ROUTE(app, "/game/<int>/player/<int>/place").methods(crow::HTTPMethod::POST)
        ([&gm](const crow::request& req, int gameId, int playerId) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, R"({"error":"Invalid JSON"})");
        if (!body.has("cardValue") || !body.has("pileId"))
            return crow::response(400, R"({"error":"Missing cardValue or pileId"})");

        int cardValue = (int)body["cardValue"].i();
        int pileId = (int)body["pileId"].i();

        std::lock_guard<std::mutex> lk(gm.Mutex());
        auto* game = gm.Get(gameId);
        if (!game) {
            return crow::response(404, R"({"error":"Game not found"})");
        }

        game->CheckTurnTimeout();
        if (game->GetGameStatus() == Game::GameStatus::Lost) {
            return crow::response(400, R"({"error":"Time expired. Game over."})");
        }

        try {
            Player& player = game->GetPlayer(playerId);
            Card chosen(cardValue);

            bool placed = player.PlaceCard(
                chosen,
                game->GetDescendingDeck1(),
                game->GetDescendingDeck2(),
                game->GetAscendingDeck1(),
                game->GetAscendingDeck2(),
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
                resp["newPileValue"] = game->GetDescendingDeck1().GetNumber();
                break;
            case 1:
                resp["newPileValue"] = game->GetDescendingDeck2().GetNumber();
                break;
            case 2:
                resp["newPileValue"] = game->GetAscendingDeck1().GetNumber();
                break;
            case 3:
                resp["newPileValue"] = game->GetAscendingDeck2().GetNumber();
                break;
            }

            return crow::response{ resp.dump() };
        }
        catch (const std::exception& e) {
            return crow::response(400, R"({"error":")" + std::string(e.what()) + "\"}");
        }
            });

    // ===================== GAME STATE =====================
    CROW_ROUTE(app, "/game/<int>/state").methods(crow::HTTPMethod::GET)
        ([&gm](int gameId) {
        std::lock_guard<std::mutex> lk(gm.Mutex());
        auto* game = gm.Get(gameId);
        if (!game) {
            return crow::response(404, R"({"error":"Game not found"})");
        }

        game->StartTurnTimerIfNeeded();
        game->CheckTurnTimeout();

        auto status = game->GetGameStatus();

        crow::json::wvalue resp;
        resp["gameId"] = gameId;

        try {
            const Player& currentPlayer = game->GetCurrentPlayer();
            resp["currentPlayerIndex"] = game->GetCurrentPlayerIndex();
            resp["currentPlayerId"] = currentPlayer.GetId();
            resp["currentPlayerName"] = currentPlayer.GetUsername();
        }
        catch (...) {
            resp["currentPlayerIndex"] = 0;
            resp["currentPlayerId"] = -1;
            resp["currentPlayerName"] = "Unknown";
        }

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

        resp["piles"] = std::vector<crow::json::wvalue>{
            game->GetDescendingDeck1().GetNumber(),
            game->GetDescendingDeck2().GetNumber(),
            game->GetAscendingDeck1().GetNumber(),
            game->GetAscendingDeck2().GetNumber()
        };

        std::vector<crow::json::wvalue> playersArray;
        for (const auto& player : game->GetPlayers()) {
            crow::json::wvalue playerJson;
            playerJson["id"] = player.GetId();
            playerJson["username"] = player.GetUsername();
            playerJson["cardsPlacedThisTurn"] = player.GetCardsPlacedThisTurn();

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

    // ===================== DRAW =====================
    CROW_ROUTE(app, "/game/<int>/player/<int>/draw").methods(crow::HTTPMethod::POST)
        ([&gm](int gameId, int playerId) {
        std::lock_guard<std::mutex> lk(gm.Mutex());

        auto* game = gm.Get(gameId);
        if (!game) {
            return crow::response(404, R"({"error":"Game not found"})");
        }

        if (game->GetCardsDeck().empty()) {
            return crow::response(400, R"({"error":"No cards left in deck"})");
        }

        try {
            Player& player = game->GetPlayer(playerId);

            Card drawnCard = game->GetCardsDeck().top();
            game->GetCardsDeck().pop();

            player.GetHand().insert(drawnCard);

            crow::json::wvalue resp;
            resp["success"] = true;
            resp["cardValue"] = drawnCard.GetNumber();
            resp["cardsLeftInDeck"] = game->GetCardsDeck().size();

            return crow::response{ resp.dump() };
        }
        catch (const std::exception& e) {
            return crow::response(400, R"({"error":")" + std::string(e.what()) + "\"}");
        }
            });

    // ===================== END TURN =====================
    CROW_ROUTE(app, "/game/<int>/player/<int>/endturn").methods(crow::HTTPMethod::POST)
        ([&gm](int gameId, int playerId) {
        std::lock_guard<std::mutex> lk(gm.Mutex());

        auto* game = gm.Get(gameId);
        if (!game) {
            return crow::response(404, R"({"error":"Game not found"})");
        }

        try {
            Player& player = game->GetPlayer(playerId);

            int cardsPlaced = player.GetCardsPlacedThisTurn();
            int cardsInDeck = game->GetCardsDeck().size();

            int minCardsRequired = (cardsInDeck > 0) ? 2 : 1;

            if (cardsPlaced < minCardsRequired) {
                crow::json::wvalue errorResp;
                errorResp["success"] = false;
                errorResp["error"] = "You must place at least " + std::to_string(minCardsRequired) +
                    " card(s) before ending your turn";
                errorResp["cardsPlaced"] = cardsPlaced;
                errorResp["minRequired"] = minCardsRequired;
                return crow::response(400, errorResp.dump());
            }

            int cardsToDraw = std::min(cardsPlaced, cardsInDeck);

            std::vector<int> drawnCards;
            for (int i = 0; i < cardsToDraw; i++) {
                if (!game->GetCardsDeck().empty()) {
                    Card drawnCard = game->GetCardsDeck().top();
                    game->GetCardsDeck().pop();
                    player.GetHand().insert(drawnCard);
                    drawnCards.push_back(drawnCard.GetNumber());
                }
            }

            game->StopTurnTimer();
            player.ResetCardsPlacedThisTurn();
            game->AdvanceTurn();

            crow::json::wvalue resp;
            resp["success"] = true;
            resp["cardsPlaced"] = cardsPlaced;
            resp["cardsToDraw"] = cardsToDraw;
            resp["cardsDrawn"] = drawnCards.size();
            resp["cardsLeftInDeck"] = game->GetCardsDeck().size();

            int idx = 0;
            for (int cardVal : drawnCards) {
                resp["drawnCardValues"][idx++] = cardVal;
            }

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
}