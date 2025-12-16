#include "Player.h"
#include <iostream>
#include <cmath>
Player::Player():
	User()
{
}


Player::Player(int id, const std::string& name):
	User(id, name),	
	m_hand()
{

}

Player::Player(const Player& other)
	: User(other), 
	m_hand(other.m_hand)
{
}


void Player::Score()
{
	if (this->GetGamesPlayed() > 0)
	{
		double FloatScore = 5.0 * this->GetWins() / (this->GetGamesPlayed() + 0.5 * this->GetUnusedCards());

		uint16_t score= static_cast<uint16_t>(FloatScore);
		this->SetScore(score);
	}
	else
		this->SetScore(0);
}



bool Player::PlaceCard(const Card& choosenCard, Card& descendingDeck1, Card& descendingDeck2, Card& ascendingDeck1, Card& ascendingDeck2, int pileId)
{
	
	if (m_hand.empty())
	{
		std::cout << "Your hand is empty!!!\n";
		return false;
	}


	auto foundCard = std::find(m_hand.begin(), m_hand.end(), choosenCard);
	if (foundCard == m_hand.end())
	{
		std::cout << "This card isn't in your hand! Choose another one.";
		return false;
	}

	using enum Card::OrderDirection;
	auto canPlace = [](const Card& chosen, const Card& target, Card::OrderDirection dir) -> bool {
		if (dir == Descending) {
			
			return (chosen.GetNumber() - target.GetNumber() == 10) || (chosen.GetNumber() < target.GetNumber());
		}
		else { 
			
			return (target.GetNumber() - chosen.GetNumber() == 10) || (chosen.GetNumber() > target.GetNumber());
		}
		};

	
	struct Target {
		Card* card;
		Card::OrderDirection dir;
	};

	Target targets[] = {
		{ &descendingDeck1, Descending },
		{ &descendingDeck2, Descending },
		{ &ascendingDeck1,  Ascending  },
		{ &ascendingDeck2,  Ascending  }
	};

	for (auto& t : targets) {
		if (canPlace(choosenCard, *t.card, t.dir)) {
			
			*t.card = std::move(*foundCard);
			t.card->SetDirection(t.dir);

			
			m_hand.erase(foundCard);
			return true;
		}
	}

	std::cout << "This card doesn't belong in any of the stacks!";
	return false;


	//if (m_hand.empty()) {
	//	std::cout << "Your hand is empty!\n";
	//	return false;
	//}

	//auto foundCard = std::find(m_hand.begin(), m_hand.end(), choosenCard);
	//if (foundCard == m_hand.end()) {
	//	std::cout << "This card isn't in your hand!\n";
	//	return false;
	//}

	//using enum Card::OrderDirection;

	//auto canPlace = [](const Card& chosen, const Card& target, Card::OrderDirection dir) -> bool {
	//	if (dir == Descending) {
	//		return (chosen.GetNumber() - target.GetNumber() == 10) || (chosen.GetNumber() < target.GetNumber());
	//	}
	//	else {
	//		return (target.GetNumber() - chosen.GetNumber() == 10) || (chosen.GetNumber() > target.GetNumber());
	//	}
	//	};

	//Card* targetPile = nullptr;
	//Card::OrderDirection dir;

	//switch (pileId) {
	//case 0: targetPile = &descendingDeck1; dir = Descending; break;
	//case 1: targetPile = &descendingDeck2; dir = Descending; break;
	//case 2: targetPile = &ascendingDeck1;  dir = Ascending;  break;
	//case 3: targetPile = &ascendingDeck2;  dir = Ascending;  break;
	//default:
	//	std::cout << "Invalid pileId!\n";
	//	return false;
	//}

	//if (canPlace(choosenCard, *targetPile, dir)) {
	//	// actualizează valoarea teancului cu cartea aleasă
	//	targetPile->SetNumber(choosenCard.GetNumber());
	//	targetPile->SetDirection(dir);

	//	// scoate cartea din mână
	//	m_hand.erase(foundCard);

	//	return true;
	//}

	//std::cout << "Invalid move: card cannot be placed on this pile!\n";
	//return false;


}


void Player::TakeCard(std::stack<Card>& Deck)
{
		m_hand.insert(Deck.top());
		Deck.pop();
	
	
}


std::unordered_set<Card>& Player::GetHand()
{
	return m_hand;
}

const std::unordered_set<Card>& Player::GetHand() const
{
	return m_hand;
}

Player Player::ConvertUserToPlayer(const User& user)
{
	return Player(user.GetId(), user.GetUsername());
}





