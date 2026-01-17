#pragma once
#include <crow.h>
#include "Database.h"
#include <mutex>
#include <vector>
#include <unordered_map>
#include <memory>
#include "Game.h"
#include "../TimerDLL/GameTimer.h"

import GameManager;

// Forward declarations
class GameManager;
class LobbyManager;
class GameRoutes;
class PowerUpRoutes;
class UserRoutes;

namespace http
{
    class Routing
    {
    public:
        Routing();
        ~Routing();

        void Run(UserProfiles& storage);

    private:
        crow::SimpleApp m_app;

        // Shared data structures
        std::mutex m_lobbyMutex;
        std::unordered_map<int, std::unique_ptr<Game>> m_activeGames;

        // Component managers
        std::unique_ptr<GameManager> m_gameManager;
        std::unique_ptr<LobbyManager> m_lobbyManager;
        std::unique_ptr<GameRoutes> m_gameRoutes;
        std::unique_ptr<PowerUpRoutes> m_powerUpRoutes;
        std::unique_ptr<UserRoutes> m_userRoutes;
    };
}