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

void http::PlayerProfiles::PopulateGame()
{
}

