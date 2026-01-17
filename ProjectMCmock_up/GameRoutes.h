#pragma once
#include <crow.h>
#include "GameManager.h"

namespace http { class UserProfiles; }

class GameRoutes {
public:
    static void Register(crow::SimpleApp& app, GameManager& gm, http::UserProfiles& storage);
};