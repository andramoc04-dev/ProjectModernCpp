#include "Card.h"

Card::Card(std::uint16_t number)
    : m_number(number),
    m_direction(OrderDirection::Unknown)
{
   
}

Card::Card()
    : m_number(0),
    m_direction(OrderDirection::Unknown)
{
  
}

Card::Card(const Card& other)
    : m_number(other.m_number),
    m_direction(other.m_direction)
{
}

std::uint16_t Card::GetNumber() const
{
    return m_number;
}

Card::OrderDirection Card::GetDirection() const
{
    return m_direction;
}

void Card::SetNumber(std::uint16_t number)
{
    m_number = number;
}

void Card::SetDirection(OrderDirection direction)
{
    m_direction = direction;
}

bool Card::operator==(const Card& other) const
{
    return m_number == other.m_number;
}

Card& Card::operator=(const Card& other)
{
    if (this != &other) {
        m_number = other.m_number;
        m_direction = other.m_direction;
    }
    return *this;
}

std::istream& operator>>(std::istream& in, Card& c)
{
    std::uint16_t number;
    in >>number;
    c = Card(number);
    return in;
}
