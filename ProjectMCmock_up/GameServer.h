#include <crow.h>
#include "Database.h"
#include <mutex>
#include <vector>
#include "Chat.h"
#include "Message.h"

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

		Chat m_chat; 
		std::mutex m_chatMutex;
	};
}