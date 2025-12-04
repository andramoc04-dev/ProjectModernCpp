#include "Card.h"

Card::Card(std::uint16_t number)
{
    m_number = number;
}

Card::Card()
{
    m_number = 0;
    m_direction = OrderDirection::Unknown;
}

std::uint16_t Card::GetNumber() const
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

bool Card::operator==(const Card& other)
{
    return m_number == other.m_number;
}

std::istream& operator>>(std::istream& in, Card& c)
{
    std::uint16_t number;
    in >>number;
    c = Card(number);
    return in;
}
