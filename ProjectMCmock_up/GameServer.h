#include <crow.h>
#include "Database.h"
#include <mutex>
#include <vector>
#include <unordered_map>
#include "Game.h"
#include "MiniGameServer.h"

#include "../TimerDLL/GameTimer.h"

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

		
		std::unordered_map<int, Game> m_activeGames;

		struct LobbyInfo {
			int m_gameId = -1;
			bool m_started = false;
			int m_matchedPlayerId = -1;
		};

		struct WaitingPlayer {
			int playerId{};
			int score{};
			GameTimer timer;
			bool timerStarted{false};
		};

		std::mutex m_lobbyMutex;
		std::vector<WaitingPlayer> m_waitingPlayers;
		std::unordered_map<int, LobbyInfo> m_playerLobbyMap;
		int m_nextGameId =1;

	private:
		static constexpr int kMaxScoreDiff =1;
		static constexpr int kLobbyTimeoutSeconds =30;
		static constexpr int kMinPlayersPerGame =2;
		static constexpr int kMaxPlayersPerGame =5;

		bool TryStartMatchLocked(UserProfiles& storage);
		void StartGameForGroupLocked(UserProfiles& storage, const std::vector<int>& playerIds);
	};
}