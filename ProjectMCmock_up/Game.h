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
	std::stack<Card>& GetCardsDeck() { return m_cardsDeck; }

	Card& GetDescendingDeck1() { return m_descendingDeck1; }
	Card& GetDescendingDeck2() { return m_descendingDeck2; }
	Card& GetAscendingDeck1() { return m_ascendingDeck1; }
	Card& GetAscendingDeck2() { return m_ascendingDeck2; }

	const std::vector<Player>& GetPlayers() const { return m_players; }
	std::vector<Player>& GetPlayers() { return m_players; }
	Chat& GetChat() { return m_chat; }
private:
	std::vector<Player> m_players;
	std::stack<Card> m_cardsDeck;
	Chat m_chat;
	Card m_descendingDeck1, m_descendingDeck2, m_ascendingDeck1, m_ascendingDeck2;
	bool m_areCardsDealt;
	bool m_areCardsShuffled;
	GameTimer m_chatTimer;
};

