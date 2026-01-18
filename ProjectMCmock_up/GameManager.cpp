module; 
#include <unordered_map>
#include <memory>
#include <mutex>
#include "Game.h"

module GameManager; 

GameManager::GameManager(std::mutex& mutex, std::unordered_map<int, std::unique_ptr<Game>>& games)
    : m_mutex(mutex), m_games(games)
{
}

Game* GameManager::Get(int gameId)
{
    auto it = m_games.find(gameId);
    return it == m_games.end() ? nullptr : it->second.get();
}

std::mutex& GameManager::Mutex()
{
    return m_mutex;
}