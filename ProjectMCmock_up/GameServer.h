#include <crow.h>
#include "Database.h"

namespace http
{
	class Routing
	{
	public:
		void Run(PlayerProfiles& storage);

	private:
		crow::response AddPlayersToGameRute(PlayerProfiles& storage, const crow::request& req, int playerId)const;

	private:
		crow::SimpleApp m_app;
	};
}