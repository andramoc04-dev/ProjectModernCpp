export module timer;

import <chrono>;

export class GameTimer {
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
