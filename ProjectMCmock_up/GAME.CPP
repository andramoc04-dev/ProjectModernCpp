#include "Game.h"
#include <iostream>
#include <ranges>
#include <deque>

Game::Game(const std::vector<Player>& players, const std::stack<Card>& cardsDeck, const Chat& chat):
	m_players(players), m_cardsDeck(cardsDeck), m_chat(chat), m_descendingDeck1(100), m_descendingDeck2(100),
	m_ascendingDeck1(1), m_ascendingDeck2(1), m_areCardsDealt(false), m_areCardsShuffled(false), m_currentPlayerIndex(0)
{
	m_chatTimer.start(999999); 

	m_descendingDeck1 = Card(100);
	m_descendingDeck2 = Card(100);
	m_ascendingDeck1 = Card(1);
	m_ascendingDeck2 = Card(1);

}

void Game::ShuffleCards()
{
	std::srand(std::time(0));
	std::deque<int> sortedCards;
	for (int i = 2; i <= 17; i++)
		sortedCards.push_back(i);
	int position;
	while (!sortedCards.empty())
	{
		position = std::rand() % sortedCards.size();
		Card card(sortedCards[position]);
		m_cardsDeck.push(card);
		sortedCards.erase(sortedCards.begin() + position);
	}
	m_areCardsShuffled = true;
}

bool Game::EndGame(int numberOfPlacedCards)
{
	int minCardsRequired = m_cardsDeck.empty() ? 1 : 2;
	if ( numberOfPlacedCards < minCardsRequired )
	{
		std::cout << "You lost..." << std::endl;
		for (auto& player : m_players)
		{
			player.SetUnusedCards(player.GetUnusedCards()+player.GetHand().size());
		}
		return true;
	}
	if (m_cardsDeck.empty())
	{
		bool emptyHands = true;
		for (auto& player : m_players)
		{
			if (!player.GetHand().empty())
				emptyHands = false;
		}
		if(emptyHands==true)
		{
			std::cout << "You won!" << std::endl;
			for (auto& player : m_players)
			{
				player.SetWins(player.GetWins()+1);
			}
			return true;
		}
	}
	return false;
}

void Game::RunGame()
{
	
	Card choosenCard; int numberOfPlacedCards=0;
	ShuffleCards();
	bool gameEnded = false;
	std::cout << "Game started.\n";

	CardsDistribution();

	while (!gameEnded)
	{
		for (int i = 0; i < m_players.size(); i++)
		{
			std::cout << "It's " << m_players[i].GetUsername() << "'s turn.\n";

			numberOfPlacedCards = 0;
			if (m_cardsDeck.empty())
				std::cout << "Choose one card to place\n";
			else
			std::cout << "Choose at least two cards\n";
			DisplayGameState(m_descendingDeck1, m_descendingDeck2, m_ascendingDeck1, m_ascendingDeck2, m_players[i]);
			std::cin >> choosenCard;

			int pileId;
			std::cout << "Choose pile (0=desc1, 1=desc2, 2=asc1, 3=asc2): ";
			std::cin >> pileId;

			while (choosenCard.GetNumber() != 0)
			{
				std::cout << "Choosed card value is " << static_cast<int>( choosenCard.GetNumber() )<< "\n";
				numberOfPlacedCards += m_players[i].PlaceCard(choosenCard, m_descendingDeck1, m_descendingDeck2, m_ascendingDeck1, m_ascendingDeck2,pileId);
				std::cout << "Press 0 if you are done\n";
				std::cin >> choosenCard;
			}
			if (numberOfPlacedCards >= 2 && !m_cardsDeck.empty())
				for (int j = 0; j < numberOfPlacedCards; j++)
				{
					if (!m_cardsDeck.empty())
						m_players[i].TakeCard(m_cardsDeck);
					else
						break;

				}
			if (EndGame(numberOfPlacedCards))
			{
				gameEnded = true;
				break;
			}
		}
	}
}

void Game::DisplayGameState(const Card& descending1, const Card& descending2, const Card& ascending1, 
	const Card& ascending2, const Player& player)
{

	std::cout << "First descending deck: " << static_cast<int>(descending1.GetNumber()) << " ->place lower cards\n";
	std::cout << "Second descending deck: " << static_cast<int>(descending2.GetNumber()) << " ->place lower cards\n";
	std::cout << "First ascending deck: " << static_cast<int>(ascending1.GetNumber()) << " ->place higher cards\n";
	std::cout << "Second ascending deck: " << static_cast<int>(ascending2.GetNumber()) << " ->place higher cards\n";
	std::cout << "Cards remaining in deck: " << m_cardsDeck.size() << "\n";
	const auto& hand = player.GetHand();
	for (const auto& card : hand)
	{
		std::cout << static_cast<int>(card.GetNumber()) << " ";
	}
	
}

void Game::CardsDistribution()
{
	size_t numberOfPlayers = m_players.size();
	size_t cardsPerPlayer = 0;
	switch (numberOfPlayers)
	{
	case 2:
		cardsPerPlayer = 8;
		break;
	case 3:
		cardsPerPlayer = 7;
		break;
	case 4:
	case 5:
		cardsPerPlayer = 6;
		break;
	default:
		throw std::invalid_argument("Invalid number of players.");

	}
	
	size_t totalCards = cardsPerPlayer * numberOfPlayers;
	for (auto i : std::views::iota(0u, totalCards)) 
	{
		m_players[i % numberOfPlayers].GetHand().insert(m_cardsDeck.top());
		m_cardsDeck.pop();
	}
	m_areCardsDealt = true;
}

void Game::SetCardsDeck(std::stack<Card> cardsDeck)
{
	m_cardsDeck = cardsDeck;
}

void Game::AddPlayer(const Player& player)
{
	m_players.push_back(player);
}

const Player& Game::GetPlayer(int id) const
{
	auto it = std::find_if(m_players.begin(), m_players.end(),
		[id](const Player& p) {
			return p.GetId() == id;
		});

	if (it != m_players.end()) {
		return *it;
	}
	throw std::runtime_error("Player with id " + std::to_string(id) + " not found in game");
}

Player& Game::GetPlayer(int id)
{
	auto it = std::find_if(m_players.begin(), m_players.end(),
		[id](const Player& p) { return p.GetId() == id; });

	if (it != m_players.end()) {
		return *it;
	}
	throw std::runtime_error("Player with id " + std::to_string(id) + " not found in game");

}

bool Game::GetAreCardsDealt() const
{
	return m_areCardsDealt;
}

bool Game::GetAreCardsShuffled() const
{
	return m_areCardsShuffled;
}

std::stack<Card>& Game::GetCardsDeck()
{
	return m_cardsDeck;
}

Card& Game::GetDescendingDeck1()
{
	return m_descendingDeck1;
}

Card& Game::GetDescendingDeck2()
{
	return m_descendingDeck2;
}

Card& Game::GetAscendingDeck1()
{
	return m_ascendingDeck1;
}

Card& Game::GetAscendingDeck2()
{
	return m_ascendingDeck2;
}

std::vector<Player>& Game::GetPlayers()
{
	return m_players;
}

Chat& Game::GetChat()
{
	return m_chat;
}

const std::vector<Player>& Game::GetPlayers() const
{
	return m_players;

}

size_t Game::GetCurrentPlayerIndex() const
{
	return m_currentPlayerIndex;
}

Player& Game::GetCurrentPlayer()
{
	if (m_players.empty())
		throw std::runtime_error("No players in game.");
	return m_players[m_currentPlayerIndex % m_players.size()];
}

const Player& Game::GetCurrentPlayer() const
{
	if (m_players.empty())
		throw std::runtime_error("No players in game.");
	return m_players[m_currentPlayerIndex % m_players.size()];
}

Game::GameStatus Game::GetGameStatus() const
{
	if (m_gameLost)
		return GameStatus::Lost;

	if (m_cardsDeck.empty()) {
		bool allHandsEmpty = true;
		for (const auto& player : m_players) {
			if (!player.GetHand().empty()) {
				allHandsEmpty = false;
				break;
			}
		}
		if (allHandsEmpty) {
			return GameStatus::Won;
		}
	}
	return GameStatus::Playing;
}

void Game::SetCurrentPlayerIndex(size_t index)
{
	if (m_players.empty())
		throw std::runtime_error("No players in game.");
	m_currentPlayerIndex = index % m_players.size();
}

void Game::AdvanceTurn(int steps)
{
	if (m_players.empty())
		return;
	const int n = static_cast<int>(m_players.size());
	int idx = static_cast<int>(m_currentPlayerIndex);
	idx = (idx + (steps % n) + n) % n;
	m_currentPlayerIndex = static_cast<size_t>(idx);
}

bool Game::IsGameOver() const
{
	return GetGameStatus() != GameStatus::Playing;
}


void Game::StartTurnTimerIfNeeded()
{
	if (m_lastPlayerWithTimer != m_currentPlayerIndex)
	{
		m_turnTimer.start(60); // 60 secunde
		m_lastPlayerWithTimer = m_currentPlayerIndex;
	}
}

void Game::StopTurnTimer()
{
	m_turnTimer.stop();
}

void Game::CheckTurnTimeout()
{
	if (!m_turnTimer.running() || !m_turnTimer.expired())
		return;

	Player& player = GetCurrentPlayer();

	int placed = player.GetCardsPlacedThisTurn();
	int minRequired = m_cardsDeck.empty() ? 1 : 2;

	if (placed < minRequired)
	{
		m_gameLost = true;
	}
}

