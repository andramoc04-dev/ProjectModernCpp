module;

#include <crow.h>
#include "Database.h"

export module UserRoutes;

export class UserRoutes {
public:
    static void Register(crow::SimpleApp& app, http::UserProfiles& storage);
};
