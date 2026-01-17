#pragma once
#include <crow.h>
#include "Database.h"

class UserRoutes {
public:
    static void Register(crow::SimpleApp& app, http::UserProfiles& storage);
};
