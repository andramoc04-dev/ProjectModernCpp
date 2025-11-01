#include "Game.h"
#include <iostream>
#include <deque>

void Game::shuffleCards()
{

}

bool Game::endGame(int numberOfPlacedCards)
{
	if (numberOfPlacedCards == -1)
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
	int choosenCard, numberOfPlacedCards = -1;
	for (int i = 0; i < m_players.size() && endGame(numberOfPlacedCards) == false; i++)
	{
		std::cout << "Choose at least two cards, or 0 if you're done";
		std::cin >> choosenCard;
		while (choosenCard != 0)
		{
			Card card(choosenCard);
			numberOfPlacedCards += m_players[i].placeCards(card, descendingStack1, descendingStack2, ascendingStack1, ascendingStack2);
			std::cin >> choosenCard;
		}
		if(numberOfPlacedCards>=2)
			for(int j = 0; j < numberOfPlacedCards; j++)
				m_players[i].takeCard(m_cardsStack);
	}
}
