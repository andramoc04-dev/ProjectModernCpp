#pragma once
#include <cstdint>
#include<iostream>
#include <functional>
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
	Card(const Card& other);

	std::uint16_t GetNumber() const;
	OrderDirection GetDirection() const;

	void SetNumber(std::uint16_t number) { m_number = number; }

	void SetDirection(OrderDirection direction);

	friend std::istream& operator>>(std::istream& in,Card& c);

	bool operator ==(const Card& other) const;

	Card& operator=(const Card& other) {
		if (this != &other) {
			m_number = other.m_number;
			m_direction = other.m_direction;
		}
		return *this;
	}

	
private:
	std::uint16_t m_number;
	OrderDirection m_direction;
};


    template<>
    struct std::hash<Card>
    {
        std::size_t operator()(Card const& c) const noexcept
        {
            return std::hash<std::uint16_t>()(c.GetNumber());
        }
    };




