#pragma once
#include <cstdint>
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

	std::uint16_t GetNumber() const;
	OrderDirection GetDirection() const;

	void SetNumber(std::uint16_t number);

	void SetDirection(OrderDirection direction);

	bool operator ==(const Card& other) const;

	

	
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




