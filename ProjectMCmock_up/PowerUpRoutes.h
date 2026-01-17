#pragma once
#include <crow.h>
#include "Database.h"
#include "GameManager.h"

class PowerUpRoutes {
public:
    static void Register(crow::SimpleApp& app, GameManager& gm, http::UserProfiles& storage);
};
