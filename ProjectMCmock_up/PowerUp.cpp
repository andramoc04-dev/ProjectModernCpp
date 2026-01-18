#include "Powerup.h"
#include "Game.h"
#include <algorithm>
#include <random>


PowerUp::PowerUp()
    : m_databaseId(0)
    , m_type(PowerUpType::SKIP_TURN)
    , m_name("")
    , m_description("")

{
}

PowerUp::PowerUp(uint32_t dbId, PowerUpType type)
    : m_databaseId(dbId)
    , m_type(type)
{
 
    switch (type) {
    case PowerUpType::SKIP_TURN:
        m_name = "Skip Turn";
        m_description = "Skip your turn without placing cards";
        break;
    case PowerUpType::PLACE_ANY_CARD:
        m_name = "Wild Card";
        m_description = "Place any card on any pile";
        break;
    case PowerUpType::DISCARD_CARD:
        m_name = "Discard";
        m_description = "Remove a card from your hand";
        break;
    case PowerUpType::NO_SCORE_LOSS:
        m_name = "Safety Net";
        m_description = "Don't lose points if you lose";
        break;
    case PowerUpType::STEAL_CARD:
        m_name = "Pickpocket";
        m_description = "Steal a random card from another player";
        break;
    }
}

PowerUp::PowerUp(uint32_t dbId, PowerUpType type, const std::string& name, const std::string& description)
    : m_databaseId(dbId)
    , m_type(type)
    , m_name(name)
    , m_description(description)
{
}

PowerUpType PowerUp::GetType() const
{
    return m_type;
}

std::string PowerUp::GetName() const
{
    return m_name;
}



std::string PowerUp::GetDescription() const
{
    return m_description;
}

uint32_t PowerUp::GetDatabaseId() const
{
    return m_databaseId;
}





void PowerUp::SetName(const std::string& name)
{
    m_name = name;
}

void PowerUp::SetDescription(const std::string& desc)
{
    m_description = desc;
}


void PowerUp::SetDatabaseId(uint32_t id)
{
    m_databaseId = id;
}


bool PowerUp::Use(Player& user, Game& game, int targetPlayerId, int targetCardValue, int targetPileId)
{
    if (!CanBeUsed(user, game)) {
        return false;
    }

    bool success = false;

    switch (m_type) {
    case PowerUpType::SKIP_TURN:
        success = ExecuteSkipTurn(user, game);
        break;

    case PowerUpType::PLACE_ANY_CARD:
        if (targetCardValue == -1 || targetPileId == -1) {
            return false;
        }
        success = ExecutePlaceAnyCard(user, game, targetCardValue, targetPileId);
        break;

    case PowerUpType::DISCARD_CARD:
        if (targetCardValue == -1) {
            return false;
        }
        success = ExecuteDiscardCard(user, targetCardValue);
        break;

    case PowerUpType::NO_SCORE_LOSS:
        success = ExecuteNoScoreLoss(user, game);
        break;

    case PowerUpType::STEAL_CARD:
        if (targetPlayerId == -1) {
            return false;
        }      
        success = ExecuteStealCard(user, game, targetPlayerId).has_value();
        break;
    }

    return success;
}

bool PowerUp::CanBeUsed(const Player& user, const Game& game) const
{
    switch (m_type) {
    case PowerUpType::DISCARD_CARD:
    case PowerUpType::PLACE_ANY_CARD:
        return !user.GetHand().empty();

    case PowerUpType::STEAL_CARD:
        return game.GetPlayers().size() > 1;

    default:
        return true;
    }
}

// Factory methods
PowerUp PowerUp::CreateSkipTurn(uint32_t dbId)
{
    return PowerUp(dbId, PowerUpType::SKIP_TURN);
}

PowerUp PowerUp::CreatePlaceAnyCard(uint32_t dbId)
{
    return PowerUp(dbId, PowerUpType::PLACE_ANY_CARD);
}

PowerUp PowerUp::CreateDiscardCard(uint32_t dbId)
{
    return PowerUp(dbId, PowerUpType::DISCARD_CARD);
}

PowerUp PowerUp::CreateNoScoreLoss(uint32_t dbId)
{
    return PowerUp(dbId, PowerUpType::NO_SCORE_LOSS);
}

PowerUp PowerUp::CreateStealCard(uint32_t dbId)
{
    return PowerUp(dbId, PowerUpType::STEAL_CARD);
}

std::string PowerUp::TypeToString(PowerUpType type)
{
    switch (type) {
    case PowerUpType::SKIP_TURN: return "SKIP_TURN";
    case PowerUpType::PLACE_ANY_CARD: return "PLACE_ANY_CARD";
    case PowerUpType::DISCARD_CARD: return "DISCARD_CARD";
    case PowerUpType::NO_SCORE_LOSS: return "NO_SCORE_LOSS";
    case PowerUpType::STEAL_CARD: return "STEAL_CARD";
    default: return "UNKNOWN";
    }
}

PowerUpType PowerUp::StringToType(const std::string& typeStr)
{
    if (typeStr == "SKIP_TURN") return PowerUpType::SKIP_TURN;
    if (typeStr == "PLACE_ANY_CARD") return PowerUpType::PLACE_ANY_CARD;
    if (typeStr == "DISCARD_CARD") return PowerUpType::DISCARD_CARD;
    if (typeStr == "NO_SCORE_LOSS") return PowerUpType::NO_SCORE_LOSS;
    if (typeStr == "STEAL_CARD") return PowerUpType::STEAL_CARD;
    return PowerUpType::SKIP_TURN;
}

std::string PowerUp::GetTypeString() const
{
   return TypeToString(m_type);
}

void PowerUp::SetTypeString(const std::string& typeStr)
{
    m_type = StringToType(typeStr);
}

bool PowerUp::ExecuteSkipTurn(Player& user, Game& game)
{
    user.ResetCardsPlacedThisTurn();
    return true;
}

bool PowerUp::ExecutePlaceAnyCard(Player& user, Game& game, int cardValue, int pileId)
{
    auto& hand = user.GetHand();
    auto it = std::find_if(hand.begin(), hand.end(),
        [cardValue](const Card& c) { return c.GetNumber() == cardValue; });

    if (it == hand.end()) {
        return false;
    }

    Card cardToPlace = *it;
    hand.erase(it);

    switch (pileId) {
    case 0:
        game.GetDescendingDeck1() = cardToPlace;
        break;
    case 1:
        game.GetDescendingDeck2() = cardToPlace;
        break;
    case 2:
        game.GetAscendingDeck1() = cardToPlace;
        break;
    case 3:
        game.GetAscendingDeck2() = cardToPlace;
        break;
    default:
        hand.insert(cardToPlace);
        return false;
    }

    user.IncrementCardsPlacedThisTurn();
    return true;
}

bool PowerUp::ExecuteDiscardCard(Player& user, int cardValue)
{
    auto& hand = user.GetHand();
    auto it = std::find_if(hand.begin(), hand.end(),
        [cardValue](const Card& c) { return c.GetNumber() == cardValue; });

    if (it == hand.end()) {
        return false;
    }

    hand.erase(it);
    return true;
}

bool PowerUp::ExecuteNoScoreLoss(Player& user, Game& game)
{
    user.SetHasScoreLossProtection(true);
    return true;
}




[[nodiscard]]
std::expected<void, Error> PowerUp::ExecuteStealCard(Player& user, Game& game, int targetPlayerId) {
    if (user.GetId() == targetPlayerId) {
        return std::unexpected(Error::InvalidTarget);
    }

    Player* target;
    try {
        target = &game.GetPlayer(targetPlayerId);
    }
    catch (...) {
        return std::unexpected(Error::InvalidTarget);
    }

    auto& targetHand = target->GetHand();
    if (targetHand.empty()) {
        return std::unexpected(Error::EmptyHand);
    }

    static thread_local std::mt19937 gen{ std::random_device{}() };

 
    std::uniform_int_distribution<size_t> dist(0, targetHand.size() - 1);
    size_t randomIndex = dist(gen);

    auto it = std::ranges::begin(targetHand);
    std::ranges::advance(it, randomIndex);

   
    user.GetHand().insert(std::move(*it));
    targetHand.erase(it);
    return {};
}