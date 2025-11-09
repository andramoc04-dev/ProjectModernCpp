#pragma once
#include <cstdint>
#include<iostream>
class Card
{
public:
	Card(std::uint8_t number);
	Card() {};
	std::uint8_t getNumber() const;

	friend std::istream& operator>>(std::istream& in,Card& c);
private:
	std::uint8_t m_number;
};

