
#include "GameFiles/Items/Gear.h"

Gear::Gear(const unsigned int id, const unsigned int weight, const std::string& name, 
		   const unsigned int itemType, const std::string& description) 
		   : Item(id, weight, name, itemType, description)
{

}

Gear::~Gear()
{

}

void Gear::Use()
{

}