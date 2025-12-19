#include <crow.h>
#include "Database.h"
#include <mutex>
#include <vector>
#include <unordered_map>
#include "Game.h"


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
		std::unordered_map<int, Game> m_activeGames;
		struct LobbyInfo {
			int m_gameId = -1;
			bool m_started = false;
			int m_matchedPlayerId = -1;
		};

		std::mutex m_lobbyMutex;
		std::vector<int> m_waitingPlayers;
		std::unordered_map<int, LobbyInfo> m_playerLobbyMap;
		int m_nextGameId = 1;
	};
}