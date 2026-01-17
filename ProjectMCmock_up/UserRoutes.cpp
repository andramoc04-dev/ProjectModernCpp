#include "UserRoutes.h"
#include "Game.h"
#include "Player.h"
#include "User.h"
#include <cmath>

void UserRoutes::Register(crow::SimpleApp& app, http::UserProfiles& storage)
{
    // ===================== GET PLAYER =====================
    CROW_ROUTE(app, "/player/<int>")
        ([&storage](int id) {
        auto player = storage.GetById<User>(id);
        if (!player.has_value())
            return crow::response(404, "Player not found");

        const auto& p = player.value();
        crow::json::wvalue json;
        json["id"] = p.GetId();
        json["name"] = p.GetUsername();
        json["score"] = p.GetScore();
        json["wins"] = p.GetWins();
        json["hoursPlayed"] = p.GetHoursPlayed();
        json["unusedCards"] = p.GetUnusedCards();

        return crow::response{ json.dump() };
            });

    // ===================== ADD PLAYER =====================
    CROW_ROUTE(app, "/addplayer").methods(crow::HTTPMethod::POST)
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

    // ===================== LOGIN =====================
    CROW_ROUTE(app, "/login").methods(crow::HTTPMethod::POST)
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

    // ===================== UPDATE PLAYTIME =====================
    CROW_ROUTE(app, "/player/<int>/updateplaytime").methods(crow::HTTPMethod::POST)
        ([&storage](const crow::request& req, int playerId) {
        auto body = crow::json::load(req.body);
        if (!body) {
            return crow::response(400, R"({"error":"Invalid JSON"})");
        }

        if (!body.has("seconds")) {
            return crow::response(400, R"({"error":"Missing seconds field"})");
        }

        int seconds = (int)body["seconds"].i();

        if (seconds < 0 || seconds > 86400) {
            return crow::response(400, R"({"error":"Invalid seconds value"})");
        }

        auto userOpt = storage.GetById<User>(playerId);
        if (!userOpt.has_value()) {
            return crow::response(404, R"({"error":"Player not found"})");
        }

        User user = userOpt.value();

        double hoursToAdd = seconds / 3600.0;
        uint16_t currentHours = user.GetHoursPlayed();
        uint16_t newHours = static_cast<uint16_t>(currentHours + std::round(hoursToAdd));

        user.SetHoursPlayed(newHours);

        try {
            storage.UpdateUser(user);

            crow::json::wvalue resp;
            resp["success"] = true;
            resp["secondsAdded"] = seconds;
            resp["totalHours"] = newHours;
            resp["message"] = "Play time updated successfully";

            return crow::response(200, resp.dump());
        }
        catch (const std::exception& e) {
            crow::json::wvalue errorResp;
            errorResp["error"] = std::string("Database error: ") + e.what();
            return crow::response(500, errorResp.dump());
        }
            });

    // ===================== RECORD WIN =====================
    CROW_ROUTE(app, "/game/<int>/player/<int>/win").methods(crow::HTTPMethod::POST)
        ([&storage](int gameId, int playerId) {
        auto userOpt = storage.GetById<User>(playerId);
        if (!userOpt.has_value()) {
            return crow::response(404, R"({"error":"Player not found"})");
        }

        User user = userOpt.value();

        user.SetWins(user.GetWins() + 1);
        user.SetGamesPlayed(user.GetGamesPlayed() + 1);

        try {
            storage.UpdateUser(user);

            crow::json::wvalue resp;
            resp["success"] = true;
            resp["wins"] = user.GetWins();
            resp["gamesPlayed"] = user.GetGamesPlayed();
            resp["message"] = "Win recorded successfully";

            return crow::response(200, resp.dump());
        }
        catch (const std::exception& e) {
            crow::json::wvalue errorResp;
            errorResp["error"] = std::string("Database error: ") + e.what();
            return crow::response(500, errorResp.dump());
        }
            });

    // ===================== RECORD LOSS =====================
    CROW_ROUTE(app, "/game/<int>/player/<int>/loss").methods(crow::HTTPMethod::POST)
        ([&storage](const crow::request& req, int gameId, int playerId) {
        auto body = crow::json::load(req.body);
        if (!body) {
            return crow::response(400, R"({"error":"Invalid JSON"})");
        }

        if (!body.has("unusedCards")) {
            return crow::response(400, R"({"error":"Missing unusedCards field"})");
        }

        int unusedCards = (int)body["unusedCards"].i();

        if (unusedCards < 0) {
            return crow::response(400, R"({"error":"Invalid unusedCards value"})");
        }

        auto userOpt = storage.GetById<User>(playerId);
        if (!userOpt.has_value()) {
            return crow::response(404, R"({"error":"Player not found"})");
        }

        User user = userOpt.value();

        user.SetUnusedCards(user.GetUnusedCards() + unusedCards);
        user.SetGamesPlayed(user.GetGamesPlayed() + 1);

        try {
            storage.UpdateUser(user);

            crow::json::wvalue resp;
            resp["success"] = true;
            resp["totalUnusedCards"] = user.GetUnusedCards();
            resp["gamesPlayed"] = user.GetGamesPlayed();
            resp["message"] = "Loss recorded successfully";

            return crow::response(200, resp.dump());
        }
        catch (const std::exception& e) {
            crow::json::wvalue errorResp;
            errorResp["error"] = std::string("Database error: ") + e.what();
            return crow::response(500, errorResp.dump());
        }
            });

    // ===================== RECALCULATE SCORE =====================
    CROW_ROUTE(app, "/player/<int>/recalculate-score").methods(crow::HTTPMethod::POST)
        ([&storage](int playerId) {
        auto userOpt = storage.GetById<User>(playerId);
        if (!userOpt.has_value()) {
            return crow::response(404, R"({"error":"Player not found"})");
        }

        User user = userOpt.value();

        Player tempPlayer = Player::ConvertUserToPlayer(user);
        tempPlayer.SetWins(user.GetWins());
        tempPlayer.SetGamesPlayed(user.GetGamesPlayed());
        tempPlayer.SetUnusedCards(user.GetUnusedCards());

        tempPlayer.Score();

        user.SetScore(tempPlayer.GetScore());

        try {
            storage.UpdateUser(user);

            crow::json::wvalue resp;
            resp["success"] = true;
            resp["newScore"] = user.GetScore();
            resp["wins"] = user.GetWins();
            resp["gamesPlayed"] = user.GetGamesPlayed();
            resp["unusedCards"] = user.GetUnusedCards();
            resp["message"] = "Score recalculated successfully";

            return crow::response(200, resp.dump());
        }
        catch (const std::exception& e) {
            crow::json::wvalue errorResp;
            errorResp["error"] = std::string("Database error: ") + e.what();
            return crow::response(500, errorResp.dump());
        }
            });
}