#include "Game.h"
#include <iostream>
#include <ranges>
#include <deque>

Game::Game(const std::vector<Player>& players, const std::stack<Card>& cardsDeck, const Chat& chat):
	m_players(players), m_cardsDeck(cardsDeck), m_chat(chat)
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
		m_cardsDeck.push(card);
		sortedCards.erase(sortedCards.begin() + position);
	}
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
	Card descendingDeck1(100), descendingDeck2(100), ascendingDeck1(1), ascendingDeck2(1);
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
			DisplayGameState(descendingDeck1, descendingDeck2, ascendingDeck1, ascendingDeck2, m_players[i]);
			std::cin >> choosenCard;
			while (choosenCard.GetNumber() != 0)
			{
				std::cout << "Choosed card value is " << static_cast<int>( choosenCard.GetNumber() )<< "\n";
				numberOfPlacedCards += m_players[i].PlaceCard(choosenCard, descendingDeck1, descendingDeck2, ascendingDeck1, ascendingDeck2);
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

}

