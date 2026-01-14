#pragma once
#include <cstdint>
#include <string>
#include <array>
#include "PowerUp.h"

namespace http
{
    struct MinigameReward
    {
        int minScore;
        PowerUpType powerUpType;
        std::string rewardName;
    };

    class MinigameServer
    {
    public:
        MinigameServer();

        PowerUpType GetRewardForScore(int score) const;

        bool HasReward(int score) const;

        std::string GetRewardDescription(int score) const;

        const std::array<http::MinigameReward, 5>& GetRewardTiers() const;

    private:
        void InitializeRewardTiers();

        std::array<http::MinigameReward, 5> m_rewardTiers;
    };
}