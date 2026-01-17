#pragma once
#include <crow.h>
import GameManager;

namespace http { class UserProfiles; }

class GameRoutes {
public:
    static void Register(crow::SimpleApp& app, GameManager& gm, http::UserProfiles& storage);
};