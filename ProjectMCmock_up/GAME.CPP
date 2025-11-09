#include "Game.h"
#include <iostream>
#include <deque>

Game::Game(std::vector<Player> players, std::stack<Card> cardsStack, Chat chat):
	m_players(players), m_cardsStack(cardsStack), m_chat(chat)
{
}

void Game::shuffleCards()
{
	std::deque<int> sortedCards;
	for (int i = 1; i <= 99; i++)
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

bool Game::endGame(int numberOfPlacedCards)
{
	if (numberOfPlacedCards == 0)
		return false;
	if (numberOfPlacedCards < 2)
	{
		std::cout << "You lost..." << std::endl;
		return true;
	}
	if (m_cardsStack.empty())
	{
		std::cout << "You won!" << std::endl;
		return true;
	}
	return false;
}

void Game::runGame()
{
	Card descendingStack1(100), descendingStack2(100), ascendingStack1(1), ascendingStack2(1);
	Card choosenCard; int numberOfPlacedCards=0;
	shuffleCards();
	while (!endGame(numberOfPlacedCards))
	{
		
		for (int i = 0; i < m_players.size(); i++)
		{
			std::cout << "It's " << m_players[i].getName() << "'s turn.\n";

			numberOfPlacedCards = 0;
			std::cout << "Choose at least two cards, or 0 if you're done";
			std::cin >> choosenCard;
			while (choosenCard.getNumber() != 0)
			{
				numberOfPlacedCards += m_players[i].placeCards(choosenCard, descendingStack1, descendingStack2, ascendingStack1, ascendingStack2);
				std::cin >> choosenCard;
			}
			if (numberOfPlacedCards >= 2 && !m_cardsStack.empty())
				for (int j = 0; j < numberOfPlacedCards; j++)
				{
					if (!m_cardsStack.empty())
						m_players[i].takeCard(m_cardsStack);
					else
						std::cout << "The main stack of cards is empty! Please take cards from another player's hand!";

				}
		}
	}
}
