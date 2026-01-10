#pragma once
#include <cstdint>
class Inventory
{
	
public:

	void SetId(uint32_t id);
	uint32_t GetId() const;

	void SetUserId(uint32_t id);
	uint32_t GetUserId() const;

	void SetPowerUpId(uint32_t id);
	uint32_t GetPowerUpId() const;

private:
	uint32_t m_id;
	uint32_t userId;
	uint32_t powerUpId;

	
};

