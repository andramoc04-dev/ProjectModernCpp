#include "Inventory.h"


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
