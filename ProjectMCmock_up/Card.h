#pragma once
#include <cstdint>
#include<iostream>
class Card
{
public:
	enum class OrderDirection : std::uint16_t {
		Ascending = 1,
		Descending = 2,
		Unknown = 0
	};

	Card(std::uint16_t number);
	Card();
	
	std::uint16_t GetNumber() const;
	OrderDirection GetDirection() const;

	void SetDirection(OrderDirection direction);

	friend std::istream& operator>>(std::istream& in,Card& c);

	bool operator ==(const Card& other);

	
private:
	std::uint16_t m_number;
	OrderDirection m_direction;
};

