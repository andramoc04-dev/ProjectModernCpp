#pragma once
#include <cstdint>

class Card
{
public:
	Card(std::uint8_t number);

	std::uint8_t getNumber() const;
private:
	std::uint8_t m_number;
};

