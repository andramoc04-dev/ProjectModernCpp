#pragma once
#include <crow.h>
#include "Database.h"

import GameManager;

class PowerUpRoutes {
public:
    static void Register(crow::SimpleApp& app, GameManager& gm, http::UserProfiles& storage);
};
