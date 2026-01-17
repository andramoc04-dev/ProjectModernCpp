#pragma once
#include "Player.h"
#include "Chat.h"
#include "../TimerDLL/GameTimer.h"
#include <vector>
#include <stack>

class Game
{
public:
	
	enum class GameStatus {
		Playing,
		Won,
		Lost
	};
	Game(const std::vector<Player>& players, const std::stack<Card>& cardsDeck, const std::shared_ptr<Chat>& chat);
	Game() = default;
	void ShuffleCards();
	void CardsDistribution();
	void SetCardsDeck(std::stack<Card> cardsDeck);
	void AddPlayer(const Player& player);

	const Player& GetPlayer(int id)const;
	Player& GetPlayer(int id);
	bool GetAreCardsDealt() const;
	bool GetAreCardsShuffled() const;
	std::stack<Card>& GetCardsDeck();

	Card& GetDescendingDeck1();
	Card& GetDescendingDeck2();
	Card& GetAscendingDeck1();
	Card& GetAscendingDeck2();

	const std::vector<Player>& GetPlayers() const;
	std::vector<Player>& GetPlayers();
	std::shared_ptr<Chat>& GetChat();
	size_t GetCurrentPlayerIndex() const;
	Player& GetCurrentPlayer();
	const Player& GetCurrentPlayer() const;
	GameStatus GetGameStatus() const;

	void SetCurrentPlayerIndex(size_t index);
	void AdvanceTurn(int steps = 1);
	bool IsGameOver() const;

	void StartTurnTimerIfNeeded();
	void StopTurnTimer();
	void CheckTurnTimeout();
private:
	std::vector<Player> m_players;
	std::stack<Card> m_cardsDeck;
	std::shared_ptr<Chat> m_chat;
	Card m_descendingDeck1, m_descendingDeck2, m_ascendingDeck1, m_ascendingDeck2;
	bool m_areCardsDealt;
	bool m_areCardsShuffled;
	GameTimer m_chatTimer;
	size_t m_currentPlayerIndex;

	GameTimer m_turnTimer;
	bool m_gameLost = false;
	size_t m_lastPlayerWithTimer = SIZE_MAX;

	static const int DESCENDING_DECK_START = 100;
	static const int ASCENDING_DECK_START = 1;
	
};

