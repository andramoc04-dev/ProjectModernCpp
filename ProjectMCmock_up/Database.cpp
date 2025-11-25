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

std::vector<Player> http::PlayerProfiles::GetPlayer()
{
    return m_db.get_all<Player>();
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

