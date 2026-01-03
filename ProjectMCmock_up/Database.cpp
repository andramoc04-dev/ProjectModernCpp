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

void http::UserProfiles::AddPowerUpToUser(uint32_t userId, uint32_t powerUpId)
{

    InventoryEntry entry;
    entry.SetUserId(userId);
    entry.SetPowerUpId(powerUpId);
    m_db.insert(entry);

}

std::vector<PowerUp> http::UserProfiles::GetUserInventory(uint32_t userId)
{

    auto entries = m_db.get_all<InventoryEntry>(sql::where(sql::c(&InventoryEntry::GetUserId) == userId));
    std::vector<PowerUp> inventory;
    for (auto& e : entries) 
    {
        auto powerUp = m_db.get<PowerUp>(e.GetPowerUpId());
        inventory.push_back(powerUp);
    }
    return inventory;

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
