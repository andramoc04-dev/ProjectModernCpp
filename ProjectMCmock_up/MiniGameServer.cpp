#include "MinigameServer.h"
#include <algorithm>

http::MinigameServer::MinigameServer()
{
    InitializeRewardTiers();
}

void http::MinigameServer::InitializeRewardTiers()
{
    m_rewardTiers = { {
        {15, PowerUpType::PLACE_ANY_CARD, "Wild Card - Place any card anywhere"},
        {12, PowerUpType::DISCARD_CARD, "Discard - Remove an unwanted card"},
        {9, PowerUpType::STEAL_CARD, "Pickpocket - Steal a card from opponent"},
        {6, PowerUpType::NO_SCORE_LOSS,"Safety Net - Protection from score loss"},
        {3, PowerUpType::SKIP_TURN, "Skip Turn - Skip without penalty"}
    } };
}

PowerUpType http::MinigameServer::GetRewardForScore(int score) const
{
    
    for (const auto& tier : m_rewardTiers) {
        if (score >= tier.minScore) {
            return tier.powerUpType;
        }
    }

   
    return PowerUpType::SKIP_TURN;
}

bool http::MinigameServer::HasReward(int score) const
{
    if (m_rewardTiers.empty()) {
        return false;
    }

  
    int minScoreRequired = m_rewardTiers.back().minScore;
    return score >= minScoreRequired;
}

std::string http::MinigameServer::GetRewardDescription(int score) const
{
    for (const auto& tier : m_rewardTiers) {
        if (score >= tier.minScore) {
            return tier.rewardName;
        }
    }

    return "No reward - Score too low";
}

const std::array<http::MinigameReward, 5>& http::MinigameServer::GetRewardTiers() const
{
    return m_rewardTiers;
}