#include "Game.h"
#include <iostream>
#include <ranges>
#include <deque>

Game::Game(std::vector<Player>& players, std::stack<Card>& cardsStack, Chat& chat):
	m_players(players), m_cardsStack(cardsStack), m_chat(chat)
{
}

void Game::ShuffleCards()
{
	std::srand(std::time(0));
	std::deque<int> sortedCards;
	for (int i = 2; i <= 99; i++)
		sortedCards.push_back(i);
	int position;
	while (!sortedCards.empty())
	{
		position = std::rand() % sortedCards.size();
		Card card(sortedCards[position]);
		m_cardsStack.push(card);
		sortedCards.erase(sortedCards.begin() + position);
	}
}

bool Game::EndGame(int numberOfPlacedCards)
{
	int minCardsRequired = m_cardsStack.empty() ? 1 : 2;
	if ( numberOfPlacedCards < minCardsRequired )
	{
		std::cout << "You lost..." << std::endl;
		for (auto& player : m_players)
		{
			player.SetUnusedCards(player.GetUnusedCards()+1);
		}
		return true;
	}
	if (m_cardsStack.empty())
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
	Card descendingStack1(100), descendingStack2(100), ascendingStack1(1), ascendingStack2(1);
	Card choosenCard; int numberOfPlacedCards=0;
	ShuffleCards();
	bool gameEnded = false;
	std::cout << "Game started.\n";

	CardsDistribution();

	while (!gameEnded)
	{
		for (int i = 0; i < m_players.size(); i++)
		{
			std::cout << "It's " << m_players[i].GetName() << "'s turn.\n";

			numberOfPlacedCards = 0;
			if (m_cardsStack.empty())
				std::cout << "Choose one card to place\n";
			else
			std::cout << "Choose at least two cards\n";
			DisplayGameState(descendingStack1, descendingStack2, ascendingStack1, ascendingStack2, m_players[i]);
			std::cin >> choosenCard;
			while (choosenCard.GetNumber() != 0)
			{
				std::cout << "Choosed card value is " << static_cast<int>( choosenCard.GetNumber() )<< "\n";
				numberOfPlacedCards += m_players[i].PlaceCard(choosenCard, descendingStack1, descendingStack2, ascendingStack1, ascendingStack2);
				std::cin >> choosenCard;
			}
			if (numberOfPlacedCards >= 2 && !m_cardsStack.empty())
				for (int j = 0; j < numberOfPlacedCards; j++)
				{
					if (!m_cardsStack.empty())
						m_players[i].TakeCard(m_cardsStack);
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
	const Card& ascending2, Player& player)
{

	std::cout << "First descending stack: " << static_cast<int>(descending1.GetNumber()) << " ->place lower cards\n";
	std::cout << "Second descending stack: " << static_cast<int>(descending2.GetNumber()) << " ->place lower cards\n";
	std::cout << "First ascending stack: " << static_cast<int>(ascending1.GetNumber()) << " ->place higher cards\n";
	std::cout << "Second ascending stack: " << static_cast<int>(ascending2.GetNumber()) << " ->place higher cards\n";
	std::cout << "Cards remaining in deck: " << m_cardsStack.size() << "\n";
	std::list<Card> hand = player.GetHand();
	for (auto card : hand)
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
		m_players[i % numberOfPlayers].GetHand().push_back(m_cardsStack.top());
		m_cardsStack.pop();
	}

}

