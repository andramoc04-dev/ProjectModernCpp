#include <crow.h>
#include "Database.h"

namespace http
{
	class Routing
	{
	public:
		void Run(UserProfiles& storage);

	private:
		crow::response AddUsersToGameRute(UserProfiles& storage, const crow::request& req, int playerId)const;

	private:
		crow::SimpleApp m_app;
	};
}