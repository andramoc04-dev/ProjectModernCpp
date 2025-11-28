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
    for (const auto& player : GetPlayers()){   // players_ este vectorul tău de Player
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

    m_db.insert(newPlayer);

}



void http::PlayerProfiles::PopulateGame()
{
}

http::AddPlayerToGameHandler::AddPlayerToGameHandler(PlayerProfiles& profiles)
    : m_db{ profiles }
{
}

crow::response http::AddPlayerToGameHandler::operator()(const crow::request& req) const
{
    auto bodyArgs = parseUrlArgs(req.body);
    auto end = bodyArgs.end();
    auto nameIter = bodyArgs.find("name");
    auto passwordIter = bodyArgs.find("password");
    if (nameIter != end && passwordIter != end)
    {
        m_db.AddPlayerToGame(nameIter->second, passwordIter->second);
    }
    return crow::response(201);
}
