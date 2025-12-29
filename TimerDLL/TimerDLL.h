#pragma once
#include <chrono>

#ifdef TIMERDLL_EXPORTS
#define TIMER_API __declspec(dllexport)
#else
#define TIMER_API __declspec(dllimport)
#endif

class TIMER_API GameTimer {
public:
    GameTimer();

    void start(int seconds);
    bool expired() const;
    int elapsedSeconds() const;

private:
    std::chrono::steady_clock::time_point m_start;
    int m_duration;
    bool m_running;
};
