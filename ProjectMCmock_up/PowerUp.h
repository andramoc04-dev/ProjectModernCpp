#pragma once
#include <string>
#include <cstdint>
#include <ranges>
#include <expected>

class Player;
class Game;

enum class PowerUpType : uint8_t
{
    SKIP_TURN = 1,
    PLACE_ANY_CARD = 2,
    DISCARD_CARD = 3,
    NO_SCORE_LOSS = 4,
    STEAL_CARD = 5
};

enum class Error
{
    InvalidTarget,
    EmptyHand,
    CardNotFound,
    InvalidPile,
    CannotUse,
    MissingParameters,
    Unexpected
};

class PowerUp
{
public:
    PowerUp();
    PowerUp(uint32_t dbId, PowerUpType type);
    PowerUp(uint32_t dbId, PowerUpType type, const std::string& name, const std::string& description);

    uint32_t GetDatabaseId() const;
    PowerUpType GetType() const;
    std::string GetName() const;
    std::string GetDescription() const;

    void SetName(const std::string& name);
    void SetDescription(const std::string& desc);
    void SetDatabaseId(uint32_t id);

    [[nodiscard]]
    std::expected<void, Error> Use(Player& user, Game& game, int targetPlayerId = -1, int targetCardValue = -1, int targetPileId = -1);

    bool CanBeUsed(const Player& user, const Game& game) const;

    static PowerUp CreateSkipTurn(uint32_t dbId = 0);
    static PowerUp CreatePlaceAnyCard(uint32_t dbId = 0);
    static PowerUp CreateDiscardCard(uint32_t dbId = 0);
    static PowerUp CreateNoScoreLoss(uint32_t dbId = 0);
    static PowerUp CreateStealCard(uint32_t dbId = 0);

    static std::string TypeToString(PowerUpType type);
    static PowerUpType StringToType(const std::string& typeStr);
    static std::string ErrorToString(Error error);

    std::string GetTypeString() const;
    void SetTypeString(const std::string& typeStr);

private:
    uint32_t m_databaseId;
    PowerUpType m_type;
    std::string m_name;
    std::string m_description;

    [[nodiscard]]
    std::expected<void, Error> ExecuteSkipTurn(Player& user, Game& game);

    [[nodiscard]]
    std::expected<void, Error> ExecutePlaceAnyCard(Player& user, Game& game, int cardValue, int pileId);

    [[nodiscard]]
    std::expected<void, Error> ExecuteDiscardCard(Player& user, int cardValue);

    [[nodiscard]]
    std::expected<void, Error> ExecuteNoScoreLoss(Player& user, Game& game);

    [[nodiscard]]
    std::expected<void, Error> ExecuteStealCard(Player& user, Game& game, int targetPlayerId);
};