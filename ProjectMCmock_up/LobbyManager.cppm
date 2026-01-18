module;

#include <mutex>
#include <vector>
#include <unordered_map>
#include <memory>
#include <crow.h>
#include "Game.h"
#include "../TimerDLL/GameTimer.h"

export module LobbyManager;

namespace http { export class UserProfiles; }

export class LobbyManager {
public:
    LobbyManager(std::mutex& mutex,
        std::unordered_map<int, std::unique_ptr<Game>>& games);

    void Join(http::UserProfiles& storage, int playerId);
    crow::json::wvalue GetStatus(http::UserProfiles& storage, int playerId);

private:
    struct LobbyInfo {
        int m_gameId = -1;
        bool m_started = false;
        bool m_timedOut = false;
        int m_matchedPlayerId = -1;
    };

    struct WaitingPlayer {
        int playerId{};
        int score{};
        GameTimer timer;
        bool timerStarted{ false };
    };

    std::mutex& m_lobbyMutex;
    std::vector<WaitingPlayer> m_waitingPlayers;
    std::unordered_map<int, LobbyInfo> m_playerLobbyMap;
    std::unordered_map<int, std::unique_ptr<Game>>& m_activeGames;
    int m_nextGameId = 1;

    static constexpr int kLobbyTimeoutSeconds = 30;
    static constexpr int kMinPlayersPerGame = 2;
    static constexpr int kMaxPlayersPerGame = 5;

    bool TryStartMatchLocked(http::UserProfiles& storage);
    void StartGameForGroupLocked(http::UserProfiles& storage, const std::vector<int>& playerIds);
};
