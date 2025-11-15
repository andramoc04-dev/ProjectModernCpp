#include "Card.h"

Card::Card(std::uint8_t number)
{
    m_number = number;
}

std::uint8_t Card::GetNumber() const
{
    return m_number;
}

Card::OrderDirection Card::GetDirection() const
{
    return m_direction;
}

void Card::SetDirection(OrderDirection direction)
{
    m_direction = direction;
}

std::istream& operator>>(std::istream& in, Card& c)
{
    std::uint8_t number;
    in >>number;
    c = Card(number);
    return in;
}
