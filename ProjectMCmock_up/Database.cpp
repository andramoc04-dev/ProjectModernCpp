#include "Database.h"

bool http::UserProfiles::Initialize()
{
    m_db.sync_schema(); 
    return true;
}

std::vector<User> http::UserProfiles::GetUsers()
{
    return m_db.get_all<User>();
}

std::optional<User> http::UserProfiles::GetUserById(int id)
{
    for (const auto& player : GetUsers()){   
        if (player.GetId() == id) {
            return player;
        }
    }

    return std::nullopt;
}

void http::UserProfiles::AddUserToGame(const std::string& name, const std::string& password)
{
    User newPlayer(name);
    newPlayer.SetPassword(password);
    newPlayer.SetScore(0);
    newPlayer.SetWins(0);
    newPlayer.SetHoursPlayed(0);
    newPlayer.SetUnusedCards(0);
    newPlayer.SetGamesPlayed(0);
    newPlayer.SetId(0);

    auto insertedId = m_db.insert(newPlayer);

   
    newPlayer.SetId(static_cast<int>(insertedId));

}

bool http::UserProfiles::HasUserWithName(const std::string& name)
{
    auto players = m_db.get_all<User>();
    for (const auto& p : players)
    {
        if (p.GetUsername() == name)
            return true;
    }
    return false;
}

void http::UserProfiles::UpdateUser(const User& user)
{
	m_db.update(user);
}

void http::UserProfiles::InitializePowerUps()
{
    auto existingPowerUps = m_db.get_all<PowerUp>();
    if (!existingPowerUps.empty()) {
        std::cout << "Power-ups already initialized. Count: " << existingPowerUps.size() << std::endl;
        return;
    }

    std::cout << "Initializing default power-ups..." << std::endl;

    
    std::vector<PowerUp> defaultPowerUps = {
        PowerUp::CreateSkipTurn(),
        PowerUp::CreatePlaceAnyCard(),
        PowerUp::CreateDiscardCard(),
        PowerUp::CreateNoScoreLoss(),
        PowerUp::CreateStealCard()
    };

    for (auto& powerUp : defaultPowerUps) {
        m_db.insert(powerUp);
    }
    std::cout << "Power-ups initialization complete. Total: " << defaultPowerUps.size() << std::endl;

}

std::vector<PowerUp> http::UserProfiles::GetAllPowerUps()
{
    return m_db.get_all<PowerUp>();
}

std::optional<PowerUp> http::UserProfiles::GetPowerUpById(uint32_t id)
{
    try {
        auto powerUp = m_db.get<PowerUp>(id);
        return powerUp;
    }
    catch (const std::system_error&) {
        return std::nullopt;
    }
}

std::optional<PowerUp> http::UserProfiles::GetPowerUpByType(PowerUpType type)
{
    auto allPowerUps = m_db.get_all<PowerUp>();
    for (const auto& powerUp : allPowerUps) {
        if (powerUp.GetType() == type) {
            return powerUp;
        }
    }
    return std::nullopt;
}

uint32_t http::UserProfiles::AddPowerUpToDataBase(const PowerUp& powerUp)
{
    return static_cast<uint32_t>(m_db.insert(powerUp));
}

void http::UserProfiles::AddPowerUpToUser(uint32_t userId, uint32_t powerUpId)
{
    Inventory entry;
    entry.SetUserId(userId);
    entry.SetPowerUpId(powerUpId);
    entry.SetIsEquipped(false);
    m_db.insert(entry);
}

void http::UserProfiles::RemovePowerUpFromUser(uint32_t userId, uint32_t powerUpId)
{
    using namespace sql;

    auto entries = m_db.get_all<Inventory>(
        where(c(&Inventory::GetUserId) == userId &&
            c(&Inventory::GetPowerUpId) == powerUpId),
        limit(1)
    );

    if (!entries.empty()) {
        m_db.remove<Inventory>(entries[0].GetId());
    }
}

std::vector<PowerUp> http::UserProfiles::GetUserInventory(uint32_t userId)
{
    using namespace sql;

    auto entries = m_db.get_all<Inventory>(
        where(c(&Inventory::GetUserId) == userId)
    );

    std::vector<PowerUp> inventory;
    for (const auto& entry : entries) {
        try {
            auto powerUp = m_db.get<PowerUp>(entry.GetPowerUpId());
            inventory.push_back(powerUp);
        }
        catch (const std::system_error&) {
            continue;
        }
    }

    return inventory;
}

std::vector<Inventory> http::UserProfiles::GetUserInventoryEntries(uint32_t userId)
{
    using namespace sql;
    return m_db.get_all<Inventory>(
        where(c(&Inventory::GetUserId) == userId)
    );
}

bool http::UserProfiles::UserHasPowerUp(uint32_t userId, uint32_t powerUpId)
{
    using namespace sql;

    auto entries = m_db.get_all<Inventory>(
        where(c(&Inventory::GetUserId) == userId &&
            c(&Inventory::GetPowerUpId) == powerUpId),
        limit(1)
    );

    return !entries.empty();
}

int http::UserProfiles::GetPowerUpCount(uint32_t userId, PowerUpType type)
{
    auto powerUpOpt = GetPowerUpByType(type);
    if (!powerUpOpt) {
        return 0;
    }

    uint32_t powerUpId = powerUpOpt->GetDatabaseId();

    using namespace sql;
    auto entries = m_db.get_all<Inventory>(
        where(c(&Inventory::GetUserId) == userId &&
            c(&Inventory::GetPowerUpId) == powerUpId)
    );

    return static_cast<int>(entries.size());
}


void http::UserProfiles::EquipPowerUp(uint32_t userId, uint32_t powerUpId)
{
    using namespace sql;

    auto entries = m_db.get_all<Inventory>(
        where(c(&Inventory::GetUserId) == userId &&
            c(&Inventory::GetPowerUpId) == powerUpId),
        limit(1)
    );

    if (!entries.empty()) {
        auto entry = entries[0];
        entry.SetIsEquipped(true);
        m_db.update(entry);
    }
}

void http::UserProfiles::UnequipPowerUp(uint32_t userId, uint32_t powerUpId)
{
    using namespace sql;

    auto entries = m_db.get_all<Inventory>(
        where(c(&Inventory::GetUserId) == userId &&
            c(&Inventory::GetPowerUpId) == powerUpId),
        limit(1)
    );

    if (!entries.empty()) {
        auto entry = entries[0];
        entry.SetIsEquipped(false);
        m_db.update(entry);
    }
}

std::vector<PowerUp> http::UserProfiles::GetEquippedPowerUps(uint32_t userId)
{
    using namespace sql;

    auto entries = m_db.get_all<Inventory>(
        where(c(&Inventory::GetUserId) == userId &&
            c(&Inventory::GetIsEquipped) == true)
    );

    std::vector<PowerUp> equippedPowerUps;
    for (const auto& entry : entries) {
        try {
            auto powerUp = m_db.get<PowerUp>(entry.GetPowerUpId());
            equippedPowerUps.push_back(powerUp);
        }
        catch (const std::system_error&) {
            continue;
        }
    }

    return equippedPowerUps;
}

bool http::UserProfiles::IsPowerUpEquipped(uint32_t userId, uint32_t powerUpId)
{
    using namespace sql;

    auto entries = m_db.get_all<Inventory>(
        where(c(&Inventory::GetUserId) == userId &&
            c(&Inventory::GetPowerUpId) == powerUpId &&
            c(&Inventory::GetIsEquipped) == true),
        limit(1)
    );

    return !entries.empty();
}


http::AddUserToGameHandler::AddUserToGameHandler(UserProfiles& profiles)
    : m_db{ profiles }
{
}

crow::response http::AddUserToGameHandler::operator()(const crow::request& req)
{

    auto body = crow::json::load(req.body);
    if (!body) {
        crow::json::wvalue e;
        e["error"] = "Invalid JSON";
        auto resp = crow::response(400, e.dump());
        resp.set_header("Content-Type", "application/json");
        return resp;
    }

    std::string name = body["name"].s();
    std::string password = body["password"].s();

    if (name.empty() || password.empty()) {
        crow::json::wvalue e;
        e["error"] = "Missing name or password";
        auto resp = crow::response(400, e.dump());
        resp.set_header("Content-Type", "application/json");
        return resp;
    }

    if (m_db.HasUserWithName(name)) {
        crow::json::wvalue e;
        e["error"] = "Player already exists";
        auto resp = crow::response(409, e.dump());
        resp.set_header("Content-Type", "application/json");
        return resp;
    }

    try {
        m_db.AddUserToGame(name, password);
    }
    catch (const std::exception& ex) {
        crow::json::wvalue e;
        e["error"] = std::string("Database error: ") + ex.what();
        return crow::response(500, e.dump());
    }

    crow::json::wvalue success;
    success["message"] = "Player created";
    auto resp = crow::response(201, success.dump());
    resp.set_header("Content-Type", "application/json");
    return resp;
    
}
