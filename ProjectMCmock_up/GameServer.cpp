#include "GameServer.h"
#include "GameRoutes.h"
#include "PowerUpRoutes.h"

import UserRoutes;
import GameManager;
import LobbyManager;

namespace http
{
    Routing::Routing()
        : m_gameManager(nullptr),
        m_lobbyManager(nullptr),
        m_gameRoutes(nullptr),
        m_powerUpRoutes(nullptr),
        m_userRoutes(nullptr)
    {
    }

    Routing::~Routing() = default;

    void Routing::Run(UserProfiles& storage)
    {
        // Inițializează managerii cu referințe la structurile partajate
        m_gameManager = std::make_unique<GameManager>(m_lobbyMutex, m_activeGames);
        m_lobbyManager = std::make_unique<LobbyManager>(m_lobbyMutex, m_activeGames);
        m_gameRoutes = std::make_unique<GameRoutes>();
        m_powerUpRoutes = std::make_unique<PowerUpRoutes>();
        m_userRoutes = std::make_unique<UserRoutes>();

        // ===================== LOBBY ROUTES =====================
        CROW_ROUTE(m_app, "/join").methods(crow::HTTPMethod::POST)
            ([this, &storage](const crow::request& req) {
            auto body = crow::json::load(req.body);
            if (!body) return crow::response(400, R"({"error":"Invalid JSON"})");
            if (!body.has("id")) return crow::response(400, R"({"error":"Missing id"})");

            int playerId = (int)body["id"].i();
            auto userOpt = storage.GetById<User>(playerId);
            if (!userOpt.has_value()) {
                return crow::response(404, R"({"error":"Player not found"})");
            }

            m_lobbyManager->Join(storage, playerId);

            crow::json::wvalue resp;
            resp["status"] = "ok";
            return crow::response{ resp.dump() };
                });

        CROW_ROUTE(m_app, "/status/<int>").methods(crow::HTTPMethod::GET)
            ([this, &storage](const crow::request& /*req*/, int playerId) {
            crow::json::wvalue resp = m_lobbyManager->GetStatus(storage, playerId);
            return crow::response{ resp.dump() };
                });

        // ===================== REGISTER COMPONENT ROUTES =====================
        GameRoutes::Register(m_app, *m_gameManager, storage);
        PowerUpRoutes::Register(m_app, *m_gameManager, storage);
        UserRoutes::Register(m_app, storage);

        // Pornește serverul
        m_app.port(18080).multithreaded().run();
    }
}