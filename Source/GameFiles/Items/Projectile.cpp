#include "Projectile.h"

Projectile::Projectile(const long id, const unsigned int itemType, float velocity, float damage) 
	: Item(id, itemType)
{
	this->zStacking = true;
	this->zDamage = damage;
	this->zVelocity = velocity;
}

Projectile::Projectile(const Projectile& other)
{
	this->zID = other.zID;
	this->zStacks = other.zStacks;
	this->zWeight = other.zWeight;
	this->zDamage = other.zDamage;
	this->zVelocity = other.zVelocity;
	this->zItemName = other.zItemName;
	this->zIconPath = other.zIconPath;
	this->zItemType = other.zItemType;
	this->zItemDescription = other.zItemDescription;
}

Projectile::Projectile(const Projectile* other)
{
	this->zID = other->zID;
	this->zStacks = other->zStacks;
	this->zWeight = other->zWeight;
	this->zDamage = other->zDamage;
	this->zVelocity = other->zVelocity;
	this->zItemName = other->zItemName;
	this->zIconPath = other->zIconPath;
	this->zItemType = other->zItemType;
	this->zItemDescription = other->zItemDescription;
}

Projectile::~Projectile()
{

}

bool Projectile::Use()
{
	if (this->zStacks > 0)
	{
		this->zStacks -= 1;
		return true;
	}
	return false;
}