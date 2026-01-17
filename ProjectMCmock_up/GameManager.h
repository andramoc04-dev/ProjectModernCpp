#pragma once
#include <unordered_map>
#include <memory>
#include <mutex>
#include "Game.h"

class GameManager {
public:
    GameManager(std::mutex& mutex, std::unordered_map<int, std::unique_ptr<Game>>& games);

    Game* Get(int gameId);
    std::mutex& Mutex();

private:
    std::mutex& m_mutex;
    std::unordered_map<int, std::unique_ptr<Game>>& m_games;
};