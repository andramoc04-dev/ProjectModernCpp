#include "GameTimer.h"

GameTimer::GameTimer()
    : m_running(false), m_duration(0)
{
}

void GameTimer::start(int seconds)
{
    m_duration = std::chrono::seconds(seconds);
    m_start = std::chrono::steady_clock::now();
    m_running = true;
}

void GameTimer::stop()
{
    m_running = false;
}

bool GameTimer::running() const
{
    return m_running;
}

bool GameTimer::expired() const
{
    if (!m_running)
        return false;

    return (std::chrono::steady_clock::now() - m_start) >= m_duration;
}
