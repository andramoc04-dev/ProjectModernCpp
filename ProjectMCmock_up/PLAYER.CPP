#include <algorithm>
#include <cmath>
#include "Player.h"

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
	if (GetGamesPlayed() == 0)
		return SetScore(0);

	const double winRate = static_cast<double>(GetWins()) / GetGamesPlayed();
	const double penalty = 0.1 * GetUnusedCards();

	const double rawScore = 1.0 + 4.0 * winRate - penalty;
	const uint16_t finalScore = static_cast<uint16_t>(
		std::round(std::clamp(rawScore, 1.0, 5.0))
		);

	SetScore(finalScore);
}



bool Player::PlaceCard(const Card& choosenCard, Card& descendingDeck1, Card& descendingDeck2, Card& ascendingDeck1, Card& ascendingDeck2, int pileId)
{
	

	if (m_hand.empty()) {
		return false;
	}

	auto foundCard = std::find(m_hand.begin(), m_hand.end(), choosenCard);
	if (foundCard == m_hand.end()) {
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

	Card* targetPile = nullptr;
	Card::OrderDirection dir;

	switch (pileId) {
	case 0: targetPile = &descendingDeck1; dir = Descending; break;
	case 1: targetPile = &descendingDeck2; dir = Descending; break;
	case 2: targetPile = &ascendingDeck1;  dir = Ascending;  break;
	case 3: targetPile = &ascendingDeck2;  dir = Ascending;  break;
	default:
		return false;
	}

	if (canPlace(choosenCard, *targetPile, dir)) {
		targetPile->SetNumber(choosenCard.GetNumber());
		targetPile->SetDirection(dir);

		m_hand.erase(foundCard);

		return true;
	}

	return false;


}


std::unordered_set<Card>& Player::GetHand()
{
	return m_hand;
}

const std::unordered_set<Card>& Player::GetHand() const
{
	return m_hand;
}


void Player::IncrementCardsPlacedThisTurn()
{
	m_numberOfPlacedCards++;
}

int Player::GetCardsPlacedThisTurn() const
{
	return m_numberOfPlacedCards;
}

void Player::ResetCardsPlacedThisTurn()
{
	m_numberOfPlacedCards = 0;
}

Player Player::ConvertUserToPlayer(const User& user)
{
	return Player(user.GetId(), user.GetUsername());
}





