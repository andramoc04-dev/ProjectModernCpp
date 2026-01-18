#include "Game.h"
#include <ranges>
#include <deque>
#include <random>
#include <vector>



Game::Game(const std::vector<Player>& players, const std::stack<Card>& cardsDeck, const std::shared_ptr<Chat>& chat) :
	m_players(players), m_cardsDeck(cardsDeck), m_chat(std::move(chat)), m_descendingDeck1(DESCENDING_DECK_START), m_descendingDeck2(DESCENDING_DECK_START),
	m_ascendingDeck1(ASCENDING_DECK_START), m_ascendingDeck2(ASCENDING_DECK_START), m_areCardsDealt(false), m_areCardsShuffled(false), m_currentPlayerIndex(0)
{
	m_players.reserve(5);
}

void Game::ShuffleCards() {
	std::srand(std::time(0));
	std::vector<int> sortedCards;
	for (int i = 2; i <= 99; i++)
		sortedCards.push_back(i);

	std::random_device rd;
	std::mt19937 g(rd());

	std::ranges::shuffle(sortedCards, g);

	for (int card : sortedCards) {
		m_cardsDeck.push(Card(card));
	}

	m_areCardsShuffled = true;
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
	m_players.emplace_back(player);
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

Player& Game::GetPlayer(int id) {
	auto it = std::ranges::find_if(m_players, [id](const auto& p) {
		return p.GetId() == id;
		});

	if (it == m_players.end()) {

		throw std::runtime_error("Player with id " + std::to_string(id) + " not found");
	}
	return *it;
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

std::shared_ptr<Chat>& Game::GetChat()
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

Game::GameStatus Game::GetGameStatus() const {
    if (m_gameLost) return GameStatus::Lost;

    if (m_cardsDeck.empty()) {
        
        bool victory = std::ranges::all_of(m_players, [](const auto& p) {
            return p.GetHand().empty();
        });

        if (victory) return GameStatus::Won;
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
		m_turnTimer.start(60);
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

	m_gameLost = true;
}

bool Game::EquipPowerUp(int playerId, int inventoryId, int maxEquipped)
{
	auto& equipped = m_equippedInventoryIdsByPlayer[playerId];
	if (equipped.contains(inventoryId)) {
		return true;
	}

	if (static_cast<int>(equipped.size()) >= maxEquipped) {
		return false;
	}

	equipped.insert(inventoryId);
	return true;
}

bool Game::UnequipPowerUp(int playerId, int inventoryId)
{
	auto it = m_equippedInventoryIdsByPlayer.find(playerId);
	if (it == m_equippedInventoryIdsByPlayer.end()) {
		return false;
	}

	return it->second.erase(inventoryId) > 0;
}

void Game::ResetEquippedPowerUps(int playerId)
{
	m_equippedInventoryIdsByPlayer.erase(playerId);
}

bool Game::IsPowerUpEquipped(int playerId, int inventoryId) const
{
	auto it = m_equippedInventoryIdsByPlayer.find(playerId);
	if (it == m_equippedInventoryIdsByPlayer.end()) {
		return false;
	}

	return it->second.contains(inventoryId);
}

std::vector<int> Game::GetEquippedInventoryIds(int playerId) const
{
	std::vector<int> result;

	auto it = m_equippedInventoryIdsByPlayer.find(playerId);
	if (it == m_equippedInventoryIdsByPlayer.end()) {
		return result;
	}

	result.reserve(it->second.size());
	for (int id : it->second) {
		result.push_back(id);
	}

	return result;
}