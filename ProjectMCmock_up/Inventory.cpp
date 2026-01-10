#include "Inventory.h"


Inventory::Inventory()
	: m_id(0)
	, userId(0)
	, powerUpId(0)
	, m_isEquipped(false)
{
}

void Inventory::SetId(uint32_t id)
{
	m_id = id;
}
uint32_t Inventory::GetId() const
{
	return m_id;
}
void Inventory::SetUserId(uint32_t id)
{
	userId = id;
}
	uint32_t Inventory::GetUserId() const
{
	return userId;
}

	void Inventory::SetPowerUpId(uint32_t id)
{
	powerUpId = id;
}

uint32_t Inventory::GetPowerUpId() const
{
	return powerUpId;
}

void Inventory::SetIsEquipped(bool equipped)
{
	m_isEquipped = equipped;
}

bool Inventory::GetIsEquipped() const
{
	return m_isEquipped;
}	