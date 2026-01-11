#pragma once
#include "Player.h"
#include "Chat.h"
#include "../TimerDLL/GameTimer.h"
#include <vector>
#include <stack>

class Game
{
public:
	Game(const std::vector<Player>& players, const std::stack<Card>& cardsDeck, const Chat& chat);
	Game() = default;
	void ShuffleCards();
	bool EndGame(int numberOfPlacedCards);
	void RunGame();
	void DisplayGameState(const Card& descending1, const Card& descending2, const Card& ascending1, 
		const Card& ascending2, const Player& player);
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
	Chat& GetChat();
	size_t GetCurrentPlayerIndex() const;
	Player& GetCurrentPlayer();
	const Player& GetCurrentPlayer() const;

	void SetCurrentPlayerIndex(size_t index);
	void AdvanceTurn(int steps = 1);
private:
	std::vector<Player> m_players;
	std::stack<Card> m_cardsDeck;
	Chat m_chat;
	Card m_descendingDeck1, m_descendingDeck2, m_ascendingDeck1, m_ascendingDeck2;
	bool m_areCardsDealt;
	bool m_areCardsShuffled;
	GameTimer m_chatTimer;
	size_t m_currentPlayerIndex;
};

