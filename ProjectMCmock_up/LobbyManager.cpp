module;

#include "Database.h"
#include "User.h"
#include "Player.h"
#include "Game.h"
#include "Chat.h"
#include "Card.h"
#include <memory>
#include <stack>
#include <mutex>
#include <vector>
#include <unordered_map>
#include <crow.h>
#include "../TimerDLL/GameTimer.h"

module LobbyManager;

LobbyManager::LobbyManager(std::mutex& mutex,
    std::unordered_map<int, std::unique_ptr<Game>>& games)
    : m_lobbyMutex(mutex), m_activeGames(games)
{
}

void LobbyManager::Join(http::UserProfiles& storage, int playerId)
{
    std::lock_guard<std::mutex> lk(m_lobbyMutex);

    if (m_playerLobbyMap.find(playerId) != m_playerLobbyMap.end())
        return;

    auto userOpt = storage.GetById<User>(playerId);
    if (!userOpt.has_value())
        return;

    const auto& user = userOpt.value();

    WaitingPlayer wp;
    wp.playerId = playerId;
    wp.score = (int)user.GetScore();
    wp.timer.start(kLobbyTimeoutSeconds);
    wp.timerStarted = true;

    m_waitingPlayers.push_back(std::move(wp));
    m_playerLobbyMap[playerId] = LobbyInfo{};

    while (TryStartMatchLocked(storage)) {
        // keep matching
    }
}

crow::json::wvalue LobbyManager::GetStatus(http::UserProfiles& storage, int playerId)
{
    std::lock_guard<std::mutex> lk(m_lobbyMutex);

    // încearcă să pornească meciuri dacă sunt condiții
    while (TryStartMatchLocked(storage)) {
        // keep matching
    }

    crow::json::wvalue resp;

    // caută jucătorul în lobby
    auto lobbyIt = m_playerLobbyMap.find(playerId);
    if (lobbyIt == m_playerLobbyMap.end()) {
        resp["started"] = false;
        resp["timedOut"] = false;
        resp["message"] = "Not in lobby";
        return resp;
    }

    // caută jucătorul în lista de așteptare (pentru timeout)
    auto wpIt = std::find_if(
        m_waitingPlayers.begin(),
        m_waitingPlayers.end(),
        [playerId](const WaitingPlayer& wp) {
            return wp.playerId == playerId;
        }
    );

    // ===== TIMEOUT =====
    if (wpIt != m_waitingPlayers.end() && wpIt->timer.expired()) {
        // scoate jucătorul din lobby
        m_waitingPlayers.erase(wpIt);
        m_playerLobbyMap.erase(playerId);

        resp["started"] = false;
        resp["timedOut"] = true;
        return resp;
    }

    // ===== NORMAL STATUS =====
    const LobbyInfo& info = lobbyIt->second;
    resp["started"] = info.m_started;
    resp["timedOut"] = false;
    resp["gameId"] = info.m_gameId;
    resp["opponentId"] = info.m_matchedPlayerId;

    return resp;
}


bool LobbyManager::TryStartMatchLocked(http::UserProfiles& storage)
{
    if ((int)m_waitingPlayers.size() < kMinPlayersPerGame)
        return false;

    if ((int)m_waitingPlayers.size() >= kMaxPlayersPerGame) {
        std::vector<int> playerIds;
        playerIds.reserve(kMaxPlayersPerGame);
        for (int i = 0; i < kMaxPlayersPerGame; i++) {
            playerIds.push_back(m_waitingPlayers[i].playerId);
        }

        StartGameForGroupLocked(storage, playerIds);
        m_waitingPlayers.erase(m_waitingPlayers.begin(),
            m_waitingPlayers.begin() + kMaxPlayersPerGame);
        return true;
    }

    int oldestIdx = 0;
    int oldestElapsed = -1;
    for (int i = 0; i < (int)m_waitingPlayers.size(); i++) {
        int e = m_waitingPlayers[i].timer.elapsedSeconds();
        if (e > oldestElapsed) {
            oldestElapsed = e;
            oldestIdx = i;
        }
    }

    const bool oldestExpired =
        m_waitingPlayers[oldestIdx].timerStarted &&
        m_waitingPlayers[oldestIdx].timer.expired();

    if (!oldestExpired)
        return false;

    //  CAZUL 1: NU sunt suficienți jucători → TIMEOUT
    if ((int)m_waitingPlayers.size() < kMinPlayersPerGame) {

        for (const auto& wp : m_waitingPlayers) {
            LobbyInfo info;
            info.m_started = false;
            info.m_timedOut = true;   //  IMPORTANT
            m_playerLobbyMap[wp.playerId] = info;
        }

        m_waitingPlayers.clear();
        return true;
    }

    // CAZUL 2: suficienți jucători → pornești joc
    std::vector<int> playerIds;
    for (const auto& wp : m_waitingPlayers) {
        playerIds.push_back(wp.playerId);
    }

    StartGameForGroupLocked(storage, playerIds);
    m_waitingPlayers.clear();
    return true;

}

void LobbyManager::StartGameForGroupLocked(http::UserProfiles& storage,
    const std::vector<int>& playerIds)
{
    if ((int)playerIds.size() < kMinPlayersPerGame)
        return;

    const int gid = m_nextGameId++;

    std::vector<Player> players;
    players.reserve(playerIds.size());
    for (int pid : playerIds) {
        auto u = storage.GetById<User>(pid).value();
        players.push_back(Player::ConvertUserToPlayer(u));
    }

    auto chatPtr = std::make_shared<Chat>();
    m_activeGames[gid] = std::make_unique<Game>(
        players,
        std::stack<Card>{},
        chatPtr
    );

    for (int pid : playerIds) {
        LobbyInfo info;
        info.m_started = true;
        info.m_gameId = gid;
        info.m_matchedPlayerId = -1;
        for (int other : playerIds) {
            if (other != pid) {
                info.m_matchedPlayerId = other;
                break;
            }
        }
        m_playerLobbyMap[pid] = info;
    }
}