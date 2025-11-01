#include "Card.h"

Card::Card(std::uint8_t number)
{
    m_number = number;
}

std::uint8_t Card::getNumber() const
{
    return m_number;
}
