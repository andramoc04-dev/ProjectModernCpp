#pragma once

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

private:
    long long m_startTicks;
    int m_duration;
    bool m_running;
};
