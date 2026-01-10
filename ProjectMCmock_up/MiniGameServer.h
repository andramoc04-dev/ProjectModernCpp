#pragma once
#include <cstdint>
#include <string>
#include <vector>
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

        const std::vector<MinigameReward>& GetRewardTiers() const;

    private:
        void InitializeRewardTiers();

        std::vector<MinigameReward> m_rewardTiers;
    };
}