#include "PowerUpRoutes.h"
#include "MiniGameServer.h"
#include "PowerUp.h"
#include "User.h"
#include "Game.h"

void PowerUpRoutes::Register(crow::SimpleApp& app, GameManager& gm, http::UserProfiles& storage)
{
    // ===================== MINIGAME COMPLETE =====================
    CROW_ROUTE(app, "/minigame/complete").methods(crow::HTTPMethod::POST)
        ([&storage](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body) {
            return crow::response(400, R"({"error":"Invalid JSON"})");
        }

        if (!body.has("playerId") || !body.has("score")) {
            return crow::response(400, R"({"error":"Missing playerId or score"})");
        }

        int playerId = (int)body["playerId"].i();
        int score = (int)body["score"].i();

        auto userOpt = storage.GetById<User>(playerId);
        if (!userOpt.has_value()) {
            return crow::response(404, R"({"error":"Player not found"})");
        }

        http::MinigameServer minigame;

        if (!minigame.HasReward(score)) {
            crow::json::wvalue resp;
            resp["success"] = true;
            resp["rewardEarned"] = false;
            resp["message"] = "Score too low for a reward";
            resp["score"] = score;
            return crow::response{ resp.dump() };
        }

        PowerUpType rewardType = minigame.GetRewardForScore(score);
        auto powerUpOpt = storage.GetPowerUpByType(rewardType);
        if (!powerUpOpt) {
            return crow::response(500, R"({"error":"Failed to find power-up in database"})");
        }

        uint32_t powerUpId = powerUpOpt->GetDatabaseId();

        try {
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

    
    // ===================== POWERUPS LIST =====================
    CROW_ROUTE(app, "/game/<int>/player/<int>/powerups").methods(crow::HTTPMethod::GET)
        ([&gm, &storage](int gameId, int playerId) {
        std::lock_guard<std::mutex> lk(gm.Mutex());

        auto* game = gm.Get(gameId);
        if (!game) {
            return crow::response(404, R"({"error":"Game not found"})");
        }

        auto userOpt = storage.GetById<User>(playerId);
        if (!userOpt.has_value()) {
            return crow::response(404, R"({"error":"Player not found"})");
        }

        auto inventoryEntries = storage.GetUserInventoryEntries(playerId);

        crow::json::wvalue resp;
        resp["gameId"] = gameId;
        resp["playerId"] = playerId;
        resp["totalPowerUps"] = (int)inventoryEntries.size();

        int idx = 0;
        for (const auto& entry : inventoryEntries) {
            auto powerUpOpt = storage.GetById<PowerUp>((int)entry.GetPowerUpId());
            if (!powerUpOpt.has_value()) {
                continue;
            }

            const auto& powerUp = powerUpOpt.value();
            resp["powerUps"][idx]["inventoryId"] = entry.GetId();
            resp["powerUps"][idx]["powerUpId"] = powerUp.GetDatabaseId();
            resp["powerUps"][idx]["type"] = powerUp.GetTypeString();
            resp["powerUps"][idx]["name"] = powerUp.GetName();
            resp["powerUps"][idx]["description"] = powerUp.GetDescription();
            resp["powerUps"][idx]["isEquipped"] = game->IsPowerUpEquipped(playerId, (int)entry.GetId());
            idx++;
        }

        return crow::response{ resp.dump() };
            });
    CROW_ROUTE(app, "/game/<int>/player/<int>/powerups/equipped").methods(crow::HTTPMethod::GET)
        ([&gm, &storage](int gameId, int playerId) {
        std::lock_guard<std::mutex> lk(gm.Mutex());

        auto* game = gm.Get(gameId);
        if (!game) {
            return crow::response(404, R"({"error":"Game not found"})");
        }

        auto userOpt = storage.GetById<User>(playerId);
        if (!userOpt.has_value()) {
            return crow::response(404, R"({"error":"Player not found"})");
        }

        crow::json::wvalue resp;
        resp["gameId"] = gameId;
        resp["playerId"] = playerId;

        const auto equippedInventoryIds = game->GetEquippedInventoryIds(playerId);
        resp["totalEquipped"] = (int)equippedInventoryIds.size();

        int idx = 0;
        for (int inventoryId : equippedInventoryIds) {
            auto entryOpt = storage.GetById<Inventory>(inventoryId);
            if (!entryOpt.has_value()) {
                continue;
            }

            const auto& entry = entryOpt.value();
            if (entry.GetUserId() != (uint32_t)playerId) {
                continue;
            }

            auto powerUpOpt = storage.GetById<PowerUp>((int)entry.GetPowerUpId());
            if (!powerUpOpt.has_value()) {
                continue;
            }

            const auto& powerUp = powerUpOpt.value();
            resp["powerUps"][idx]["inventoryId"] = inventoryId;
            resp["powerUps"][idx]["id"] = powerUp.GetDatabaseId();
            resp["powerUps"][idx]["type"] = powerUp.GetTypeString();
            resp["powerUps"][idx]["name"] = powerUp.GetName();
            resp["powerUps"][idx]["description"] = powerUp.GetDescription();
            idx++;
        }

        return crow::response{ resp.dump() };
            });
    CROW_ROUTE(app, "/game/<int>/player/<int>/powerups/<int>/equip").methods(crow::HTTPMethod::POST)
        ([&gm, &storage](int gameId, int playerId, int inventoryId) {
        std::lock_guard<std::mutex> lk(gm.Mutex());

        auto* game = gm.Get(gameId);
        if (!game) {
            return crow::response(404, R"({"error":"Game not found"})");
        }

        auto userOpt = storage.GetById<User>(playerId);
        if (!userOpt.has_value()) {
            return crow::response(404, R"({"error":"Player not found"})");
        }

        auto entryOpt = storage.GetById<Inventory>(inventoryId);
        if (!entryOpt.has_value() || entryOpt->GetUserId() != (uint32_t)playerId) {
            return crow::response(400, R"({"error":"Player does not own this power-up"})");
        }

        const int maxEquipped = 3;
        if (!game->EquipPowerUp(playerId, inventoryId, maxEquipped)) {
            return crow::response(400, R"({"error":"Too many equipped power-ups"})");
        }

        crow::json::wvalue resp;
        resp["success"] = true;
        resp["message"] = "Power-up equipped successfully";
        resp["gameId"] = gameId;
        resp["playerId"] = playerId;
        resp["inventoryId"] = inventoryId;
        return crow::response{ resp.dump() };
            });

    // ===================== UNEQUIP (GAME) =====================
    CROW_ROUTE(app, "/game/<int>/player/<int>/powerups/<int>/unequip").methods(crow::HTTPMethod::POST)
        ([&gm, &storage](int gameId, int playerId, int inventoryId) {
        std::lock_guard<std::mutex> lk(gm.Mutex());

        auto* game = gm.Get(gameId);
        if (!game) {
            return crow::response(404, R"({"error":"Game not found"})");
        }

        auto userOpt = storage.GetById<User>(playerId);
        if (!userOpt.has_value()) {
            return crow::response(404, R"({"error":"Player not found"})");
        }

        auto entryOpt = storage.GetById<Inventory>(inventoryId);
        if (!entryOpt.has_value() || entryOpt->GetUserId() != (uint32_t)playerId) {
            return crow::response(400, R"({"error":"Player does not own this power-up"})");
        }

        game->UnequipPowerUp(playerId, inventoryId);

        crow::json::wvalue resp;
        resp["success"] = true;
        resp["message"] = "Power-up unequipped successfully";
        resp["gameId"] = gameId;
        resp["playerId"] = playerId;
        resp["inventoryId"] = inventoryId;
        return crow::response{ resp.dump() };
            });

    // ===================== USE POWERUP IN GAME =====================
    CROW_ROUTE(app, "/game/<int>/player/<int>/usepower").methods(crow::HTTPMethod::POST)
        ([&gm, &storage](const crow::request& req, int gameId, int playerId) {
        auto body = crow::json::load(req.body);
        if (!body) {
            return crow::response(400, R"({"error":"Invalid JSON"})");
        }

        if (!body.has("powerUpId")) {
            return crow::response(400, R"({"error":"Missing powerUpId"})");
        }

        uint32_t powerUpId = (uint32_t)body["powerUpId"].i();

        std::lock_guard<std::mutex> lk(gm.Mutex());

        auto* game = gm.Get(gameId);
        if (!game) {
            return crow::response(404, R"({"error":"Game not found"})");
        }

        try {
            const Player& currentPlayer = game->GetCurrentPlayer();
            if (currentPlayer.GetId() != playerId) {
                return crow::response(400, R"({"error":"Not your turn"})");
            }
        }
        catch (...) {
            return crow::response(500, R"({"error":"Failed to get current player"})");
        }

        /*if (!storage.IsPowerUpEquipped(playerId, powerUpId)) {
            return crow::response(400, R"({"error":"Power-up not equipped or not owned"})");
        }*/

        auto powerUpOpt = storage.GetById<PowerUp>(powerUpId);
        if (!powerUpOpt) {
            return crow::response(404, R"({"error":"Power-up not found"})");
        }

        try {
            Player& player = game->GetPlayer(playerId);
            PowerUp powerUp = powerUpOpt.value();

            int targetPlayerId = body.has("targetPlayerId") ? (int)body["targetPlayerId"].i() : -1;
            int targetCardValue = body.has("targetCardValue") ? (int)body["targetCardValue"].i() : -1;
            int targetPileId = body.has("targetPileId") ? (int)body["targetPileId"].i() : -1;

            bool success = powerUp.Use(player, *game, targetPlayerId, targetCardValue, targetPileId);

            if (success) {
                storage.RemovePowerUpFromUser(playerId, powerUpId);

                if (powerUp.GetType() == PowerUpType::SKIP_TURN) {
                    player.ResetCardsPlacedThisTurn();
                    game->AdvanceTurn();
                }

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
}