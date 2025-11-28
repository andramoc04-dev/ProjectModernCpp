#include "Database.h"

bool http::PlayerProfiles::Initialize()
{
    m_db.sync_schema();
    auto initPlayersCount = m_db.count<Player>();
    if (initPlayersCount == 0)
        PopulateGame();

    auto playersCount = m_db.count<Player>();
    return playersCount != 0;
}

std::vector<Player> http::PlayerProfiles::GetPlayers()
{
    return m_db.get_all<Player>();
}

std::optional<Player> http::PlayerProfiles::GetPlayerById(int id)
{
    for (const auto& player : GetPlayers()){   
        if (player.GetId() == id) {
            return player;
        }
    }

    return std::nullopt;
}

void http::PlayerProfiles::AddPlayerToGame(const std::string& name, const std::string& password)
{
    Player newPlayer(name);
    newPlayer.SetPassword(password);

    auto insertedId = m_db.insert(newPlayer);

    // setează id-ul în obiect (opțional, util pentru logging)
    newPlayer.SetId(static_cast<int>(insertedId));

}

bool http::PlayerProfiles::HasPlayerWithName(const std::string& name)
{
    auto players = m_db.get_all<Player>();
    for (const auto& p : players)
    {
        if (p.GetName() == name)
            return true;
    }
    return false;
}



void http::PlayerProfiles::PopulateGame()
{
    std::vector<Player> players{
            Player{ -1, "Milk"},
            Player{ -1, "Egg"},
            Player{ -1, "Flour"},
            Player{ -1, "Butter"},
            Player{ -1, "Salt"},
            Player{ -1, "Jam"},
            Player{ -1, "Viorica"}
    };
    m_db.insert_range(players.begin(), players.end());
}

http::AddPlayerToGameHandler::AddPlayerToGameHandler(PlayerProfiles& profiles)
    : m_db{ profiles }
{
}

crow::response http::AddPlayerToGameHandler::operator()(const crow::request& req)
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

    if (m_db.HasPlayerWithName(name)) {
        crow::json::wvalue e;
        e["error"] = "Player already exists";
        auto resp = crow::response(409, e.dump());
        resp.set_header("Content-Type", "application/json");
        return resp;
    }

    // Adăugăm jucătorul și returnăm 201; dacă AddPlayerToGame ar returna id, îl putem include
    m_db.AddPlayerToGame(name, password);

    crow::json::wvalue success;
    success["message"] = "Player created";
    auto resp = crow::response(201, success.dump());
    resp.set_header("Content-Type", "application/json");
    return resp;
}
