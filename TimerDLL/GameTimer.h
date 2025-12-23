#pragma once
#include <chrono>

#ifdef TIMERDLL_EXPORTS
#define TIMER_API __declspec(dllexport)
#else
#define TIMER_API __declspec(dllimport)
#endif

class TIMER_API GameTimer
{
public:
    GameTimer();

    void start(int seconds);
    void stop();
    bool expired() const;
    bool running() const;

private:
    std::chrono::steady_clock::time_point m_start;
    std::chrono::seconds m_duration;
    bool m_running;
};
