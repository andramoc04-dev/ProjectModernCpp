#include "Database.h"

bool http::UserProfiles::Initialize()
{
    m_db.sync_schema();
    auto initPlayersCount = m_db.count<User>();
    if (initPlayersCount == 0)
        PopulateGame();

    auto playersCount = m_db.count<User>();
    return playersCount != 0;
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



void http::UserProfiles::PopulateGame()
{
    std::vector<User> players{
            User{ -1, "Milk"},
            User{ -1, "Egg"},
            User{ -1, "Flour"},
            User{ -1, "Butter"},
            User{ -1, "Salt"},
            User{ -1, "Jam"},
            User{ -1, "Viorica"}
    };
    m_db.insert_range(players.begin(), players.end());
}

http::AddUserToGameHandler::AddUserToGameHandler(UserProfiles& profiles)
    : m_db{ profiles }
{
}

crow::response http::AddUserToGameHandler::operator()(const crow::request& req)
{
  /*  auto bodyArgs = parseUrlArgs(req.body);
    auto end = bodyArgs.end();
    auto nameIter = bodyArgs.find("name");
    auto passwordIter = bodyArgs.find("password");
    if (nameIter != end && passwordIter != end)
    {
        m_db.AddPlayerToGame(nameIter->second, passwordIter->second);
    }
    return crow::response(201);*/


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

    // Adăugăm jucătorul și returnăm 201; dacă AddPlayerToGame ar returna id, îl putem include
    m_db.AddUserToGame(name, password);

    crow::json::wvalue success;
    success["message"] = "Player created";
    auto resp = crow::response(201, success.dump());
    resp.set_header("Content-Type", "application/json");
    return resp;
}
