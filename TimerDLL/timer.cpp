#include "TimerDLL.h"

GameTimer::GameTimer()
    : m_duration(0), m_running(false)
{
}

void GameTimer::start(int seconds)
{
    m_duration = seconds;
    m_start = std::chrono::steady_clock::now();
    m_running = true;
}

bool GameTimer::expired() const
{
    if (!m_running) return false;
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - m_start).count();
    return elapsed >= m_duration;
}

int GameTimer::elapsedSeconds() const
{
    if (!m_running) return 0;
    auto now = std::chrono::steady_clock::now();
    return (int)std::chrono::duration_cast<std::chrono::seconds>(now - m_start).count();
}


