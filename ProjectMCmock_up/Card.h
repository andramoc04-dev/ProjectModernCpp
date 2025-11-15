#pragma once
#include <cstdint>
#include<iostream>
class Card
{
public:
	enum class OrderDirection : std::uint8_t {
		Ascending = 1,
		Descending = 2,
		Unknown = 0
	};

	Card(std::uint8_t number);
	Card()=default;
	
	std::uint8_t GetNumber() const;
	OrderDirection GetDirection() const;

	void SetDirection(OrderDirection direction);

	friend std::istream& operator>>(std::istream& in,Card& c);

	
private:
	std::uint8_t m_number=0;
	OrderDirection m_direction=OrderDirection::Unknown;
};

